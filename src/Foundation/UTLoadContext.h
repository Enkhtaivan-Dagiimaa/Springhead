/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef UTLoadContext_H
#define UTLoadContext_H

#include <Foundation/Object.h>
#include <Foundation/UTTypeDesc.h>


namespace Spr{;

///	�t�@�C���}�b�v(���̂Ƃ���t�@�C���̃��[�h��p)
class UTFileMap: public UTRefCount{
public:
	std::string name;	///<		�t�@�C����
	const char* start;	///<		�������}�b�v���ꂽ�t�@�C���̐擪
	const char* end;	///<		�������}�b�v���ꂽ�t�@�C���̏I�[
	const char* curr;	///<		���݂̈ʒu
	/// �R���X�g���N�^
	UTFileMap():start(NULL), end(NULL), curr(NULL){}
	///
	/// �t�@�C�� �}�b�s���O		
	virtual bool Map(const UTString fn)=0;
	/// �t�@�C�� �A���}�b�s���O
	virtual void Unmap()=0;
	///	���[�h�ł����ԂȂ�true
	bool IsGood();
};

/**	�t�@�C������ObjectDesc��ǂݏo������C�t�@�C���ɏ������񂾂肷�邽�߂̃f�[�^�D
	ObjectDesc �ւ̃|�C���^(data) �� �^��� (type) �����D
	�������̊Ǘ����s���D	*/
class UTLoadData: public UTRefCount{
public:
	UTTypeDesc* type;	///<	�f�[�^�̌^ 
	UTString name;		///<	���O
	void* data;			///<	���[�h�����f�[�^
	bool haveData;		///<	data��delete���ׂ����ǂ����D
	UTLoadData(UTTypeDesc* t=NULL, void* d=NULL);
	~UTLoadData();
};

class UTLoadContext;
///	���[�h��ɏ����������邽�߂ɁA���[�h���ɐ�������A���[�h��Ɏ��s�����D
class UTLoadTask:public NamedObject{
public:
	OBJECT_DEF_NOIF(UTLoadTask);
	virtual ~UTLoadTask(){}
	virtual void Execute(UTLoadContext* ctx){};
};

///	���[�h��̏������s�����߂̃^�X�N���X�g
class UTLoadTasks:public std::vector< UTRef<UTLoadTask> >{
public:
	void Execute(UTLoadContext* ctx);
};

typedef std::pair<UTRef<ObjectIf>, UTRef<ObjectIf> > UTPairObject;
typedef std::map<UTRef<ObjectIf>, UTRef<ObjectIf> >  UTMapObject;

/**	�t�@�C�����[�h���Ɏg�p����R���e�L�X�g
	�t�@�C�������[�h����ۂ́C�f�[�^���m�[�h���ƂɃ��[�h���āC
	�I�u�W�F�N�g����邽�߂̃f�B�X�N���v�^�\����(PHSolidDesc�Ȃ�)��
	�܂����[�h����D
	���̂��ƁC�I�u�W�F�N�g�𐶐�����D	*/
class UTLoadContext{
public:	
	//--------------------------------------------------------------------------
	//	�ϐ�
	///	���[�h���̃t�@�C���̖��O�ƒ��g�D�t�@�C��include�ɔ�����stack�ɂȂ��Ă���D
	UTStack< UTRef<UTFileMap> > fileMaps;
	///	���݃��[�h���̃I�u�W�F�N�g�D�l�X�g�����I�u�W�F�N�g�ɔ����ăX�^�b�N�ɂȂ��Ă���D
	ObjectIfs objects;
	///	�X�^�b�N�ɍŏ��ɋl�܂ꂽ�I�u�W�F�N�g���t�@�C���̈�ԊO�������[�g�̃I�u�W�F�N�g�̋L�^�D
	ObjectIfs rootObjects;
	///	���[�h�����f�B�X�N���v�^�̃X�^�b�N�D�l�X�g�����g�ݗ��Č^�ɔ����ăX�^�b�N�ɂȂ��Ă���D
	UTStack< UTRef<UTLoadData> > datas;
	///	���[�h����FITypedesc�̃t�B�[���h�̈ʒu�D�g�ݗ��Č^�̃t�B�[���h�ɔ����ăX�^�b�N�ɂȂ��Ă���D
	UTTypeDescFieldIts fieldIts;
	///	�G���[���b�Z�[�W�o�͗p�̃X�g���[�� cout �Ƃ� DSTR ���w�肷��D
	std::ostream* errorStream;
	///	���t�@�����X����Ń����N���邽�߂̋L�^�D
	UTLoadTasks links;
	///	���[�h�ƃ����N���I����Ă��珈������^�X�N
	UTLoadTasks postTasks;
	///	�^DB
	UTTypeDescDb* typeDb;
	// GRMesh�̃}�b�v
	UTMapObject meshMap;	

	//---------------------------------------------------------------------------
	//	�֐�
	///
	UTLoadContext():errorStream(NULL){
		errorStream=&DSTR;
	}
	///	�G���[���b�Z�[�W�̏o�́Dpos���t�@�C�����ƍs�ԍ��ɕϊ�����D
	void ErrorMessage(UTFileMap* info, const char* pos, const char* msg);
	///	���b�Z�[�W�̍쐬�Dpos���t�@�C�����ƍs�ԍ��ɕϊ�����D
	void Message(UTFileMap* info, const char* pos, const char* msg);
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
	///	�t�@�C���}�b�v���쐬���ăX�^�b�N(fileMaps)�ɐς�
	virtual void PushFileMap(const UTString fn)=0;
};

}

#endif	// UTLoadContext_H
