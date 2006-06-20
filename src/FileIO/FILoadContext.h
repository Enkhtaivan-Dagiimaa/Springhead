/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef FILoadContext_H
#define FILoadContext_H

#include <Foundation/Object.h>
#include <Foundation/UTTypeDesc.h>
#include <string>
#ifdef _WIN32
#include <WinBasis/WinBasis.h>
#else 
#include <sys/stat.h>
#endif


namespace Spr{;

/**	�t�@�C������ObjectDesc��ǂݏo������C�t�@�C���ɏ������񂾂肷�邽�߂̃f�[�^�D
	ObjectDesc �ւ̃|�C���^(data) �� �^��� (type) �����D
	�������̊Ǘ����s���D	*/
class FINodeData: public UTRefCount{
public:
	UTTypeDesc* type;	///<	�f�[�^�̌^ 
	UTString name;		///<	���O
	void* data;			///<	���[�h�����f�[�^
	bool haveData;		///<	data��delete���ׂ����ǂ����D
	FINodeData(UTTypeDesc* t=NULL, void* d=NULL);
	~FINodeData();
};

class FINodeHandlers;
/**	�t�@�C�����[�h���Ɏg�p����R���e�L�X�g
	�t�@�C�������[�h����ۂ́C�f�[�^���m�[�h���ƂɃ��[�h���āC
	�I�u�W�F�N�g����邽�߂̃f�B�X�N���v�^�\����(PHSolidDesc�Ȃ�)��
	�܂����[�h����D
	���̂��ƁC�I�u�W�F�N�g�𐶐�����D	*/
class FILoadContext{
public:
	//--------------------------------------------------------------------------
	//	�N���X��`
	///
	struct FileInfo{
		~FileInfo();
		std::string name;	///<		�t�@�C����
		const char* start;	///<		�������}�b�v���ꂽ�t�@�C���̐擪
		const char* end;	///<		�������}�b�v���ꂽ�t�@�C���̏I�[
		const char* parsingPos;	///<	���݂̃p�[�X�ʒu
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
		FileInfo():start(NULL), end(NULL){}
		/// �t�@�C�� �}�b�s���O		
		bool Map(std::string fn);
		/// �t�@�C�� �A���}�b�s���O
		void Unmap();
		///	���[�h�ł����ԂȂ�true
		bool IsGood();
	};
	///	�^�X�N�N���X�D���[�h��ɂ܂Ƃ߂Ďd���������邽�߂̂��́D
	class Task:public UTRefCount{
	public:
		virtual ~Task(){}
		virtual void Execute(FILoadContext* ctx)=0;	
	};
	///	�^�X�N���X�g
	class Tasks:public std::vector< UTRef<Task> >{
	public:
		void Execute(FILoadContext* ctx);
	};
	///	�m�[�h�ւ̎Q�Ƃ��L�^���Ă����N���X�D�S�����[�h�ł��Ă��烊���N����D
	class LinkTask: public Task{
	public:
		std::vector<NameManagerIf*> nameManagers;
		std::string ref;
		ObjectIf* object;
		const char* pos;
		LinkTask(const ObjectIfs& objs, const char* p, ObjectIf* o, std::string r);
		void Execute(FILoadContext* ctx);
	};
	
	//--------------------------------------------------------------------------
	//	�ϐ�
	///	���[�h���̃t�@�C���̖��O�ƒ��g�D�t�@�C��include�ɔ�����stack�ɂȂ��Ă���D
	UTStack<FileInfo> fileInfo;	
	///	���݃��[�h���̃I�u�W�F�N�g�D�l�X�g�����I�u�W�F�N�g�ɔ����ăX�^�b�N�ɂȂ��Ă���D
	ObjectIfs objects;
	///	�X�^�b�N�ɍŏ��ɋl�܂ꂽ�I�u�W�F�N�g���t�@�C���̈�ԊO�������[�g�̃I�u�W�F�N�g�̋L�^�D
	ObjectIfs rootObjects;
	///	���[�h�����f�B�X�N���v�^�̃X�^�b�N�D�l�X�g�����g�ݗ��Č^�ɔ����ăX�^�b�N�ɂȂ��Ă���D
	UTStack< UTRef<FINodeData> > datas;
	///	���[�h����FITypedesc�̃t�B�[���h�̈ʒu�D�g�ݗ��Č^�̃t�B�[���h�ɔ����ăX�^�b�N�ɂȂ��Ă���D
	UTTypeDescFieldIts fieldIts;
	///	�G���[���b�Z�[�W�o�͗p�̃X�g���[�� cout �Ƃ� DSTR ���w�肷��D
	std::ostream* errorStream;
	///	���t�@�����X����Ń����N���邽�߂̋L�^�D
	Tasks links;
	///	���[�h�ƃ����N���I����Ă��珈������^�X�N
	Tasks postTasks;
	///	�^DB
	UTTypeDescDb* typeDb;

	//---------------------------------------------------------------------------
	//	�֐�
	///
	FILoadContext():errorStream(NULL){
		errorStream=&DSTR;
	}
	///	�G���[���b�Z�[�W�̏o�́Dpos���t�@�C�����ƍs�ԍ��ɕϊ�����D
	void ErrorMessage(const char* pos, const char* msg);
	///	���b�Z�[�W�̍쐬�Dpos���t�@�C�����ƍs�ԍ��ɕϊ�����D
	void Message(const char* pos, const char* msg);
	///	���[�h�ł����ԂȂ�true
	bool IsGood();
	///	type���������鏀��������(type���Z�b�g���CXXDesc��p�ӂ���)
	void PushType(UTTypeDesc* type);
	////
	void PopType();
	///
	void WriteString(std::string v);
	///
	void WriteNumber(double d);
	///
	void WriteBool(bool b);
	///
	void AddLink(std::string ref, const char* pos);
	///
	void Link();
	///
	void PostTask();
	///	�m�[�h�̍쐬
	void PushCreateNode(const IfInfo* info, const void* data);
};


}

#endif	// FILoadContext_H
