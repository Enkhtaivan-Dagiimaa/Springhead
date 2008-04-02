/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
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
public:
	void PushFileMap(const UTString fn);
	virtual ~FILoadContext(){}
};

class FISaveContext;
class FISdk;

/**	�t�@�C�����[�_�[�E�Z�[�o�[
*/
class FIFile:public Object{
public:
	FISdk* sdk;
	SPR_OBJECTDEF_ABST(FIFile);
	FISdk* GetSdk(){ return sdk; }
	virtual bool Load(ObjectIfs& objs, const char* fn);
	virtual void Load(FILoadContext* fc);
	virtual bool Save(const ObjectIfs& objs, const char* fn);
	virtual void Save(const ObjectIfs& objs, FISaveContext* sc);
	/**	�m�[�h�̃��[�h�D
		TODO: �V�d�l�ɂ��킹��

		���[�h����Desc����I�u�W�F�N�g���쐬����D
		�I�u�W�F�N�g�̍쐬�́C�e�I�u�W�F�N�g��CreateObject()�C�e�̐e��CreateObject()��
		��c��CreateObject()�����ɌĂ�ŁC�쐬�ł����Ƃ���Ŏ~�߂�D
		�ǂ̃I�u�W�F�N�g���쐬�ł��Ȃ��ꍇ�́CSdkIf::CreateSdk()���Ăяo���D
	void LoadNode();

	///	�m�[�h�̃��[�h�̏I��
	void LoadEndNode(FILoadContext* fc);
	*/
	///	���[�h�̎���
	virtual void LoadImp(FILoadContext* fc)=0;
	
	//@name ���[�_�[���������߂̃��[�e�B���e�B�֐�
	//@{	
	///	�u���b�N(�g�ݗ��Č^)�ɓ���
	virtual void LBlockStart(FILoadContext* fc);
	///	�u���b�N(�g�ݗ��Č^)����o��
	virtual void LBlockEnd(FILoadContext* fc);
	//@}

protected:	
	///	�m�[�h�̃Z�[�u
	void SaveNode(FISaveContext* sc, ObjectIf* obj);
	///	�u���b�N�̃Z�[�u
	void SaveBlock(FISaveContext* sc);

	//@name �ۑ������̃n���h���[
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


}
#endif
