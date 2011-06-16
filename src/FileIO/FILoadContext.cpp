/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

#include <FileIO/FILoadContext.h>

#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

using namespace std;

namespace Spr{;

//---------------------------------------------------------------------------
//	FILoadFileMap
//	�t�@�C�� �}�b�s���O
//  �����̃t�@�C���̃A�N�Z�X���x������s�����߂ɁA���ۂ̃t�@�C������������Ƀ}�b�s���O����

class FILoadFileMap : public UTFileMap{
	FILoadFileMap::~FILoadFileMap(){
		if (start) Unmap();
	}
#ifdef _WIN32
		HANDLE hFile, hFileMap;		///<	�t�@�C���n���h���A�t�@�C���}�b�s���O�I�u�W�F�N�g
#else 
		//FILE *hFile;
		//char *buffer;
		int fd;					///<	�t�@�C���f�B�X�N���v�^
		struct stat filestat;	///<	�t�@�C���T�C�Y�𓾂�̂Ɏg��
		void *sourceptr;
#endif
public:
	///	�t�@�C���̃}�b�v
	virtual bool Map(const UTString fn, bool binary){
		name = fn;
	#ifdef _WIN32
		// �t�@�C���I�[�v��
		hFile = CreateFile(fn.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, 0);	
		if (hFile == INVALID_HANDLE_VALUE){
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
	/// �t�@�C�� �A���}�b�s���O
	virtual void Unmap(){
		if(!IsGood())
			return;
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
	virtual bool IsGood(){
		return start && end && (end != (char*)-1);
	}
};

//---------------------------------------------------------------------------
//	FILoadContext

void FILoadContext::PushFileMap(const UTString fn, bool binary){
	fileMaps.Push(DBG_NEW FILoadFileMap);
	fileMaps.Top()->Map(fn, binary);
}
void FILoadContext::PopFileMap(){
	fileMaps.Top()->Unmap();
	fileMaps.Pop();
}

}	//	namespace Spr
