/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_FIFile_H
#define SPR_FIFile_H
#include <Foundation/SprObject.h>

/**	\defgroup gpFileIO	�t�@�C�����o��SDK	*/
//@{
namespace Spr{;

class UTTypeDescDb;
///	�t�@�C�����[�_�E�Z�[�o
struct FIFileIf: public ObjectIf{
	SPR_IFDEF(FIFile);
	///	���[�h
	bool Load(ObjectIfs& objs, const char* fn);
	///	�Z�[�u
	bool Save(const ObjectIfs& objs, const char* fn);
};

///	DirectX�t�@�C���̃t�@�C�����[�_�ƃZ�[�o
struct FIFileXIf: public FIFileIf{
	SPR_IFDEF(FIFileX);
	///	���[�h
	bool Load(ObjectIfs& objs, const char* fn);
	///	�Z�[�u
	bool Save(const ObjectIfs& objs, const char* fn);
};

///	COLLADA�t�@�C���̃t�@�C�����[�_�ƃZ�[�o
struct FIFileCOLLADAIf: public FIFileIf{
	SPR_IFDEF(FIFileCOLLADA);
	///	���[�h
	bool Load(ObjectIfs& objs, const char* fn);
	///	�Z�[�u
	bool Save(const ObjectIfs& objs, const char* fn);
};

}

#endif
