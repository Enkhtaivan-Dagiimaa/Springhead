/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef FISAVECONTEXT_H
#define FISAVECONTEXT_H

#include <Foundation/UTLoadContext.h>
#include <fstream>

namespace Spr{;

class Import;

class FISaveContext: public UTFileContext{
public:
	///	�Z�[�u����t�@�C���̖��O
	UTString fileName;
	///	UTTypeDesc�̃t�B�[���h�ւ̃C�^���[�^�̃X�^�b�N
	UTTypeDescFieldIts fieldIts;
	///	�Z�[�u����f�B�X�N���v�^�̃X�^�b�N�D�l�X�g�����g�ݗ��Č^�ɔ����ăX�^�b�N�ɂȂ��Ă���D
	UTStack< UTRef<UTLoadedData> > datas;
	///
	std::set<ObjectIf*> savedObjects;
	/// �C���|�[�g�X�^�b�N
	UTStack<Import*>	importStack;
	/// SDK�ւ̎Q��
	FISdk*				sdk;
	
	FISaveContext();
	///	�Z�[�u�p�Ƀt�@�C�����J��
	//bool Open(const char* fn, bool binary);
	/// �t�@�C�������
	//void Close();

	virtual void PushFileMap(const UTString fn, bool binary);
	virtual void PopFileMap();
	std::ostream&	Stream();

	///	���b�Z�[�W�̏o��
	void Message(const char* msg);
	///	�G���[���b�Z�[�W�̏o��
	void ErrorMessage(const char* msg);

	/**	�Z�[�u����m�[�h�̃O���[�v��o�^�B�O���[�v�����X�y�[�X�ŋ�؂��Ďw��B
		��FResisterGroupToDb("Foundation Physics Graphics Framework OldSpringhead");
	*/
	void RegisterGroupToDb(const char* gp);

	UTString GetNodeTypeName();
	UTString GetNodeName();
};

}

#endif
