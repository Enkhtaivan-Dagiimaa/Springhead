/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_FIFile_H
#define SPR_FIFile_H
#include <Foundation/SprObject.h>

/**	\addtogroup gpFileIO	*/
//@{
namespace Spr{;

///	�t�@�C�����[�_�E�Z�[�o
struct FIFileIf: public ObjectIf{
	IF_DEF(FIFile);
	///	���[�h
	virtual bool Load(ObjectIfs& objs, const char* fn)=0;
	///	�Z�[�u
	virtual bool Save(const ObjectIfs& objs, const char* fn)=0;
};

///	DirectX�t�@�C���̃t�@�C�����[�_�ƃZ�[�o
struct FIFileXIf: public FIFileIf{
	IF_DEF(FIFileX);
	///	���[�h
	virtual bool Load(ObjectIfs& objs, const char* fn)=0;
	///	�Z�[�u
	virtual bool Save(const ObjectIfs& objs, const char* fn)=0;
};


}

#endif
