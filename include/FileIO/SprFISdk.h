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

struct FIFileIf;
struct FIFileXIf;
struct FIFileSprIf;
struct FIFileVRMLIf;
struct FIFileCOLLADAIf;
struct FIFileBinaryIf;
struct ImportIf;

///	�t�@�C�����o��SDK
struct FISdkIf : public SdkIf{
	SPR_IFDEF(FISdk);

	/// X�t�@�C���I�u�W�F�N�g�̍쐬
	FIFileXIf*			CreateFileX();
	/// Spr�t�@�C���I�u�W�F�N�g�̍쐬
	FIFileSprIf*		CreateFileSpr();
	/// VRML�t�@�C���I�u�W�F�N�g�̍쐬
	FIFileVRMLIf*		CreateFileVRML();
	/// COLLADA�t�@�C���I�u�W�F�N�g�̍쐬
	FIFileCOLLADAIf*	CreateFileCOLLADA();
	/// �o�C�i���t�@�C���I�u�W�F�N�g�̍쐬
	FIFileBinaryIf*		CreateFileBinary();

	/** @brief IfInfo�ɑΉ�����t�@�C���I�u�W�F�N�g���쐬
		@param ii IfInfo
		���m��IfInfo�̏ꍇ��NULL��Ԃ��D
	 */
	FIFileIf*			CreateFile(const IfInfo* ii);

	/** @brief �g���q����t�@�C���I�u�W�F�N�g���쐬
		@param ext	�g���q���܂ރt�@�C���p�X
		�g���q�ƃt�@�C���^�C�v�̑Ή��͈ȉ��̒ʂ�:
		.x		<=> Direct X
		.wrl	<=> VRML
		.dae	<=> Collada
		.dat	<=> Binary
		
		���m�̊g���q�̏ꍇ��NULL��Ԃ��D
	 */
	FIFileIf*		CreateFileFromExt(UTString filename);

	/// ���[�g�C���|�[�g�̍쐬
	ImportIf*		CreateImport();
	/// �C���|�[�g�̍쐬
	ImportIf*		CreateImport(ImportIf* parent, UTString path, ObjectIf* owner, const ObjectIfs& children);

	/// �t�@�C���I�u�W�F�N�g�̍폜
	bool DelChildObject(ObjectIf* o);
	/// �S�t�@�C���I�u�W�F�N�g�̍폜
	void Clear();

	static  FISdkIf* SPR_CDECL CreateSdk();
	static void SPR_CDECL RegisterSdk();
};

//@}

}	//	namespace Spr
#endif
