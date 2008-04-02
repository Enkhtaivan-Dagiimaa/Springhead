/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 *	@file SprPHSdk.h
 *	@brief �����V�~�����[�V����SDK
*/
#ifndef SPR_PHSDKIF_H
#define SPR_PHSDKIF_H
#include <Collision/SprCDShape.h>

namespace Spr{;

/** \addtogroup gpPhysics */
//@{

struct PHSceneIf;
struct PHSceneDesc;

//	�������[�h�̂��߂̒��g�Ȃ��̃f�X�N���v�^
struct PHSdkDesc{
};

///	�����V�~�����[�V����SDK
struct PHSdkIf : public SdkIf{
	SPR_IFDEF(PHSdk);
	//	API
	///	Scene�̍쐬
	PHSceneIf* CreateScene();
	///	Scene�̍쐬
	PHSceneIf* CreateScene(const PHSceneDesc& desc);
	///	Scene�̐����擾
	int NScene();
	///	Scene�̎擾
	PHSceneIf* GetScene(size_t i);
	/// Scene�̃}�[�W
	void MergeScene(PHSceneIf* scene0, PHSceneIf* scene1);
	/** @brief Shape���쐬����
		@param desc �쐬����Shape�̃f�B�X�N���v�^
		@return Shape�̃C���^�t�F�[�X
	 */
	CDShapeIf* CreateShape(const IfInfo* ii, const CDShapeDesc& desc);
	template <class T> CDShapeIf* CreateShape(const T& desc){
		return CreateShape(T::GetIfInfo(), desc);
	}
	
	
	///	Shape�̐�
	int NShape();

	///	Shape�̎擾
	CDShapeIf* GetShape(int i);

	//	SDK�����A�o�^�֐�
	///	PHSdk�̃C���X�^���X���쐬
	static PHSdkIf* SPR_CDECL CreateSdk();
	///	PHSdk���t�@�C�����[�_�[�Ȃǂɓo�^
	static void SPR_CDECL RegisterSdk();
};

//@}

}	//	namespace Spr
#endif
