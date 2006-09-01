/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef FIFILE_H
#define FIFILE_H

#include <SprFileIO.h>
#include <Foundation/UTTypeDesc.h>
#include <Foundation/UTLoadContext.h>
#include <Foundation/UTLoadHandler.h>

namespace Spr{;
///	FILoadContext
class FILoadContext: public UTLoadContext{
};

class FISaveContext;
class FISdk;

/**	�t�@�C�����[�_�[�E�Z�[�o�[
*/
class FIFile:public InheritObject<FIFileIf, Object>{
public:
	///	�t�@�C���ɃZ�[�u�E�t�@�C�����烍�[�h����^�̏��(UTTypeDesc)
	UTTypeDescDb typeDb;
	///	���[�h���Ɏ������[�h�Ƃ͕ʂ̏������K�v�ȃm�[�h�̃n���h��
	UTLoadHandlers handlers;
public:
	FISdk* sdk;
	OBJECT_DEF_ABST(FIFile);
	virtual bool Load(ObjectIfs& objs, const char* fn);
	virtual void Load(FILoadContext* fc);
	virtual bool Save(const ObjectIfs& objs, const char* fn);
	virtual void Save(const ObjectIfs& objs, FISaveContext* sc);
	FISdk* GetSdk(){ return sdk; }
	/**	�m�[�h�̃��[�h�D
		���[�h����Desc����I�u�W�F�N�g���쐬����D
		�I�u�W�F�N�g�̍쐬�́C�e�I�u�W�F�N�g��CreateObject()�C�e�̐e��CreateObject()��
		��c��CreateObject()�����ɌĂ�ŁC�쐬�ł����Ƃ���Ŏ~�߂�D
		�ǂ̃I�u�W�F�N�g���쐬�ł��Ȃ��ꍇ�́CSdkIf::CreateSdk()���Ăяo���D	*/
	void LoadNode(FILoadContext* fc);
	///	�m�[�h�̃��[�h�̏I��
	void LoadEndNode(FILoadContext* fc);
	///	�u���b�N(�g�ݗ��Č^)�ɓ���
	void LoadEnterBlock(FILoadContext* fc);
	///	�u���b�N(�g�ݗ��Č^)����o��
	void LoadLeaveBlock(FILoadContext* fc);
	///	���[�h�̎���
	virtual void LoadImp(FILoadContext* fc)=0;
	///	LoadContext�̐ݒ�
	virtual void SetLoaderContext(FILoadContext* fc){}
	/**	���[�h����m�[�h�̃O���[�v��o�^�B�O���[�v�����X�y�[�X�ŋ�؂��Ďw��B
		��FResisterGroup("Foundation Physics Graphics Framework OldSpringhead");
	*/
	void RegisterGroup(const char* gp);

protected:
	
	///	�m�[�h�̃Z�[�u
	void SaveNode(FISaveContext* sc, ObjectIf* obj);
	///	�u���b�N�̃Z�[�u
	void SaveBlock(FISaveContext* sc);

	///	�ۑ������̃n���h���[
	//@{
	///	�t�@�C���J�n���̏���
	virtual void OnSaveFileStart(FISaveContext* sc){}
	///	�t�@�C���I�����̏���
	virtual void OnSaveFileEnd(FISaveContext* sc){}
	///	�m�[�h�J�n���̏���
	virtual void OnSaveNodeStart(FISaveContext* sc){}
	///	�m�[�h�I�����̏���
	virtual void OnSaveNodeEnd(FISaveContext* sc){}
	///	�f�[�^�J�n���̏���
	virtual void OnSaveDataStart(FISaveContext* sc){}
	///	�f�[�^�I�����̏���
	virtual void OnSaveDataEnd(FISaveContext* sc){}
	///	�q�I�u�W�F�N�g�̕ۑ��J�n���̏���
	virtual void OnSaveChildStart(FISaveContext* sc){}
	///	�q�I�u�W�F�N�g�̕ۑ��I�����̏���
	virtual void OnSaveChildEnd(FISaveContext* sc){}

	///	�u���b�N�J�n���̏���
	virtual void OnSaveBlockStart(FISaveContext* sc){}
	///	�u���b�N�I�����̏���
	virtual void OnSaveBlockEnd(FISaveContext* sc){}
	///	�t�B�[���h�J�n���̏���
	virtual void OnSaveFieldStart(FISaveContext* sc, int nElements){}
	///	�t�B�[���h�I�����̏���
	virtual void OnSaveFieldEnd(FISaveContext* sc, int nElements){}
	///	�v�f�J�n���̏���
	virtual void OnSaveElementStart(FISaveContext* sc, int pos, bool last){}
	///	�v�f�I�����̏���
	virtual void OnSaveElementEnd(FISaveContext* sc, int pos, bool last){}
	///	bool�l�̕ۑ�
	virtual void OnSaveBool(FISaveContext* sc, bool val){}
	///	int�l�̕ۑ�
	virtual void OnSaveInt(FISaveContext* sc, int val){}
	///	real�l�̕ۑ�
	virtual void OnSaveReal(FISaveContext* sc, double val){}
	///	string�l�̕ۑ�
	virtual void OnSaveString(FISaveContext* sc, UTString val){}
	virtual void OnSaveRef(FISaveContext* sc){}
	//@}

};

template <class intf, class base>
struct InheritFIFile:public InheritObject<intf, base>{
	virtual bool Load(ObjectIfs& objs, const char* fn){ return base::Load(objs, fn); }
	virtual bool Save(const ObjectIfs& objs, const char* fn){ return base::Save(objs, fn); }
	virtual void RegisterGroup(const char* gp){ return base::RegisterGroup(gp); }
};


}
#endif
