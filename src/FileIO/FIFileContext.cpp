#include "FIFileContext.h"
#include <fstream>
#include <sstream>
#ifdef _WIN32
#include <WinBasis/WinBasis.h>
#else
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

namespace Spr{;
//---------------------------------------------------------------------------
//	FIFileContext::FileInfo
// �t�@�C�� �}�b�s���O
//   �����̃t�@�C���̃A�N�Z�X���x������s�����߂ɁA���ۂ̃t�@�C������������Ƀ}�b�s���O����
bool FIFileContext::FileInfo::Map(std::string fn){
#ifdef _WIN32
	// �t�@�C���I�[�v��
	hFile = CreateFile(fn.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, 0);	
	if (!hFile){
		DSTR << "Cannot open input file: " << fn.c_str() << std::endl;
		return false;	
	}		
	// �t�@�C���T�C�Y�̎擾
	DWORD len = GetFileSize(hFile,NULL);	
	// �t�@�C���}�b�s���O�I�u�W�F�N�g�쐬
	hFileMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	// �t�@�C��fn��ǂݑ����Ń}�b�v���A���̐擪�A�h���X���擾
	start = (const char*)MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 0);
	end = start + len;
	return true;	
#else	
	/*
	hFile = fopen(fn.c_str(), "rb");
	if (!hFile) {
		DSTR << "Cannot open input file: " << fn.c_str() << std::endl;
		return false;	
	}		
	fseek(hFile, 0, SEEK_END);
	int const len = ftell(hFile);
	fseek(hFile, 0, SEEK_SET);
	buffer = DBG_NEW char[len];
	fread(buffer, 1, len, hFile);
	start = buffer;
	end = start + len;*/
	fd = open(fn.c_str(), O_RDONLY); 
    if( fd < 0 ) {
		DSTR << "Cannot open input file: " << fn.c_str() << std::endl;
		return false;	
	}		
	if( fstat( fd, &filestat ) == 0 ) {
		// �ǂݍ��ݐ�p�Ńt�@�C���}�b�s���O
        sourceptr = mmap( NULL, filestat.st_size, PROT_READ, MAP_SHARED, fd, 0 );
		if( sourceptr != MAP_FAILED ) {
			start = (char*)sourceptr;
			end = start + filestat.st_size;
			return true;
		} 
	}
	return false;
#endif

}
// �t�@�C�� �A���}�b�s���O
void FIFileContext::FileInfo::Unmap(){
#ifdef _WIN32
	UnmapViewOfFile(start);		// �}�b�v�����t�@�C�����A���}�b�v
	CloseHandle(hFileMap);		// �t�@�C���}�b�s���O�I�u�W�F�N�g���N���[�Y
	CloseHandle(hFile);			// �t�@�C���̃n���h�����N���[�Y
#else
	//fclose(hFile);
	//delete[] buffer;
	munmap(sourceptr, filestat.st_size);
#endif
	start = end = NULL;
}
FIFileContext::FileInfo::~FileInfo(){
	if (start) Unmap();
}

//---------------------------------------------------------------------------
//	FIFileContext
ObjectIf* FIFileContext::Create(const IfInfo* ifInfo, const void* desc){
	for(IfStack::reverse_iterator it = objects.rbegin(); it != objects.rend(); ++it){
		ObjectIf* obj = (*it)->CreateObject(ifInfo, desc);
		if (obj) return obj;
	}
	return NULL;
}

};
