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
	IF_DEF(PHSdk);
	//	API
	///	Scene�̍쐬
	virtual PHSceneIf* CreateScene()=0;
	///	Scene�̍쐬
	virtual PHSceneIf* CreateScene(const PHSceneDesc& desc)=0;
	///	Scene�̐����擾
	virtual int NScene()=0;
	///	Scene�̎擾
	virtual PHSceneIf* GetScene(size_t i)=0;
	/// Scene�̃}�[�W
	virtual void MergeScene(PHSceneIf* scene0, PHSceneIf* scene1)=0;
	/** @brief Shape���쐬����
		@param desc �쐬����Shape�̃f�B�X�N���v�^
		@return Shape�̃C���^�t�F�[�X
	 */
	virtual CDShapeIf* CreateShape(const IfInfo* ii, const CDShapeDesc& desc)=0;
	template <class T> CDShapeIf* CreateShape(const T& desc){
		return CreateShape(T::GetIfInfo(), desc);
	}
	
	
	///	Shape�̐�
	virtual int NShape()=0;

	///	Shape�̎擾
	virtual CDShapeIf* GetShape(int i)=0;

	//	SDK�����A�o�^�֐�
	///	PHSdk�̃C���X�^���X���쐬
	static PHSdkIf* SPR_CDECL CreateSdk();
	///	PHSdk���t�@�C�����[�_�[�Ȃǂɓo�^
	static void SPR_CDECL RegisterSdk();
};

//@}

}	//	namespace Spr
#endif
