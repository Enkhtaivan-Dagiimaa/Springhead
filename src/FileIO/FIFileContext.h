#ifndef FIFILECONTEXT_H
#define FIFILECONTEXT_H

#include <Foundation/Object.h>
#include <string>
#ifdef _WIN32
#include <WinBasis/WinBasis.h>
#endif
namespace Spr{;

class FIFileContext{
public:
	struct FileInfo{
		~FileInfo();
		std::string name;	///<	�t�@�C����
		int line;			///<	�s�ԍ�
		const char* start;	///<	�������}�b�v���ꂽ�t�@�C���̐擪
		const char* end;	///<	�������}�b�v���ꂽ�t�@�C���̏I�[
#ifdef _WIN32
		HANDLE hFile, hFileMap;		///<	�t�@�C���n���h���A�t�@�C���}�b�s���O�I�u�W�F�N�g
#else 
		FILE *hFile;
		char *buffer;
#endif
		// �R���X�g���N�^
		FileInfo():line(0),start(NULL), end(NULL){}
		// �t�@�C�� �}�b�s���O		
		bool Map(std::string fn);
		// �t�@�C�� �A���}�b�s���O
		void Unmap();
	};
	UTStack<FileInfo> fileInfo;
	
	typedef UTStack<ObjectIf*> IfStack;
	///	���݃��[�h���̃I�u�W�F�N�g
	IfStack objects;
	///	�R���e�i���K�v�Ȃ΂���
	IfStack container;
	///	
	UTStack<void*> primitives;

	FIFileContext(){
	}
	ObjectIf* Create(const IfInfo* ifInfo, const void* desc);
};


}

#endif	// FIFILECONTEXT_H
