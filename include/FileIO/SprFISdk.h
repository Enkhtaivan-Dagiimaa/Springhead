/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_FISDKIF_H
#define SPR_FISDKIF_H
#include <Foundation/SprScene.h>

namespace Spr{;


/** \addtogroup gpFileIO	*/
//@{

struct FIFileXIf;
struct FIFileVRMLIf;
struct FIFileCOLLADAIf;
struct FIFileBinaryIf;

///	�t�@�C�����o��SDK
struct FISdkIf : public SdkIf{
	SPR_IFDEF(FISdk);

	/// X�t�@�C���I�u�W�F�N�g�̍쐬
	FIFileXIf*			CreateFileX();
	/// VRML�t�@�C���I�u�W�F�N�g�̍쐬
	FIFileVRMLIf*		CreateFileVRML();
	/// COLLADA�t�@�C���I�u�W�F�N�g�̍쐬
	FIFileCOLLADAIf*	CreateFileCOLLADA();
	/// �o�C�i���t�@�C���I�u�W�F�N�g�̍쐬
	FIFileBinaryIf*		CreateFileBinary();

	/// �t�@�C���I�u�W�F�N�g�̍폜
	bool DelChildObject(ObjectIf* o);
	/// �S�t�@�C���I�u�W�F�N�g�̍폜
	void Clear();

	static  FISdkIf* SPR_CDECL CreateSdk();
};

//@}

}	//	namespace Spr
#endif
