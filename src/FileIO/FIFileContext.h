#ifndef FIFILECONTEXT_H
#define FIFILECONTEXT_H

#include <Foundation/Object.h>
#include <string>
#ifdef _WIN32
#include <WinBasis/WinBasis.h>
#else 
#include <sys/stat.h>
#endif


namespace Spr{;
class FITypeDesc;

/**	�t�@�C�����[�h���Ɏg�p����R���e�L�X�g
	�t�@�C�������[�h����ۂ́C�f�[�^���m�[�h���ƂɃ��[�h���āC
	�I�u�W�F�N�g����邽�߂̃f�B�X�N���v�^�\����(PHSolidDesc�Ȃ�)��
	�܂����[�h����D
	���̂��ƁC�I�u�W�F�N�g�𐶐�����D	*/
class FIFileContext{
public:
	
	//--------------------------------------------------------------------------
	//	�N���X��`
	struct FileInfo{
		~FileInfo();
		std::string name;	///<	�t�@�C����
		int line;			///<	�s�ԍ�
		const char* start;	///<	�������}�b�v���ꂽ�t�@�C���̐擪
		const char* end;	///<	�������}�b�v���ꂽ�t�@�C���̏I�[
#ifdef _WIN32
		HANDLE hFile, hFileMap;		///<	�t�@�C���n���h���A�t�@�C���}�b�s���O�I�u�W�F�N�g
#else 
		//FILE *hFile;
		//char *buffer;
		int fd;					///<	�t�@�C���f�B�X�N���v�^
		struct stat filestat;	///<	�t�@�C���T�C�Y�𓾂�̂Ɏg��
		void *sourceptr;
#endif
		/// �R���X�g���N�^
		FileInfo():line(0),start(NULL), end(NULL){}
		/// �t�@�C�� �}�b�s���O		
		bool Map(std::string fn);
		/// �t�@�C�� �A���}�b�s���O
		void Unmap();
		///	���[�h�ł����ԂȂ�true
		bool IsGood();
	};
	///	���[�h�����f�X�N���v�^�DCreate�����ꍇ�͏�����Ƃ���Delete����D
	struct Primitive: UTRefCount{
		FITypeDesc* desc;
		void* obj;
		Primitive(FITypeDesc* d=NULL, void* o=NULL);
		~Primitive();
	};

	//--------------------------------------------------------------------------
	//	�ϐ�
	///	���[�h���̃t�@�C���̖��O�ƒ��g�Dinclude�����ꍇ�ɔ�����stack�ɂȂ��Ă���D
	UTStack<FileInfo> fileInfo;	
	///	
	typedef UTStack<ObjectIf*> IfStack;
	///	���݃��[�h���̃I�u�W�F�N�g
	IfStack objects;
	///	���[�h�����f�B�X�N���v�^�̃X�^�b�N�D�u���b�N�̃��[�h�̂��߂ɃX�^�b�N�ɂȂ��Ă���D
	UTStack< UTRef<Primitive> > primitives;
	///	�G���[���b�Z�[�W�o�͗p�̃X�g���[�� cout �Ƃ� DSTR ���w�肷��D
	std::ostream* errorStream;

	//---------------------------------------------------------------------------
	//	�֐�
	///
	FIFileContext():errorStream(NULL){
		errorStream=&DSTR;
	}
	///	�I�u�W�F�N�g�̐���
	ObjectIf* Create(const IfInfo* ifInfo, const void* desc);
	///	�G���[���b�Z�[�W�̏o�́Dpos���t�@�C�����ƍs�ԍ��ɕϊ�����D
	void ErrorMessage(const char* pos, const char* msg);
	///	���b�Z�[�W�̍쐬�Dpos���t�@�C�����ƍs�ԍ��ɕϊ�����D
	void Message(const char* pos, const char* msg);
	///	���[�h�ł����ԂȂ�true
	bool IsGood();
};


}

#endif	// FIFILECONTEXT_H
