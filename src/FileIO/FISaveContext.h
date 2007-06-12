/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef FISAVECONTEXT_H
#define FISAVECONTEXT_H

#include <Foundation/Object.h>
#include <Foundation/UTTypeDesc.h>
#include <Foundation/UTLoadContext.h>
#include <fstream>

namespace Spr{;

class FINodeHandlers;
class FISaveContext: public UTFileContext{
public:
	///	�Z�[�u����t�@�C���̖��O
	UTString fileName;
	///	�Z�[�u�t�@�C���̃t�@�C���X�g���[��
	std::ofstream file;
	///	UTTypeDesc�̃t�B�[���h�ւ̃C�^���[�^�̃X�^�b�N
	UTTypeDescFieldIts fieldIts;
	///	�Z�[�u����f�B�X�N���v�^�̃X�^�b�N�D�l�X�g�����g�ݗ��Č^�ɔ����ăX�^�b�N�ɂȂ��Ă���D
	UTStack< UTRef<UTLoadedData> > datas;
	///
	std::set<ObjectIf*> savedObjects;
	
	FISaveContext();
	///	�Z�[�u�p�Ƀt�@�C�����J��
	void Open(const char* fn);
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
