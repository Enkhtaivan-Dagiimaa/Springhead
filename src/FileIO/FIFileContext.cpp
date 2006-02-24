#include "FIFileContext.h"
#include <fstream>
#include <sstream>
#ifdef _WIN32
#include <WinBasis/WinBasis.h>
#endif

namespace Spr{;
//---------------------------------------------------------------------------
//	FIFileContext::FileInfo
// �t�@�C�� �}�b�s���O
// �t�@�C��fn��ǂݑ����Ń}�b�v���A���̐擪�A�h���X���|�C���^start�Ɏ擾
bool FIFileContext::FileInfo::Map(std::string fn){
#ifdef _WIN32
	//  �t�@�C�����������Ƀ}�b�s���O���邱�Ƃō������ȒP�Ƀt�@�C����������
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
	// �t�@�C�����}�b�v
	start = (const char*)MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 0);
	end = start + len;
#else	
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
	end = start + len;
#endif
	return true;
}
// �t�@�C�� �A���}�b�s���O
void FIFileContext::FileInfo::Unmap(){
#ifdef _WIN32
	UnmapViewOfFile(start);		// �}�b�v�����t�@�C�����A���}�b�v
	CloseHandle(hFileMap);		// �t�@�C���}�b�s���O�I�u�W�F�N�g���N���[�Y
	CloseHandle(hFile);			// �t�@�C���̃n���h�����N���[�Y
#else
	fclose(hFile);
	delete[] buffer;
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
