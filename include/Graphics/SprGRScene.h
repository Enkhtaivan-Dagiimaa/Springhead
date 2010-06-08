/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 *	@file SprGRScene.h
 *	@brief �V�[���O���t
*/

/**	\addtogroup	gpGraphics	*/
//@{

#ifndef SPR_GRScene_H
#define SPR_GRScene_H

#include <Graphics/SprGRFrame.h>
#include <Graphics/SprGRRender.h>

namespace Spr{;

///	@brief GRScene ��Desc�D���g�����D
struct GRSceneDesc{
};

struct GRSdkIf;
/**	@brief	�O���t�B�b�N�X�V�[���O���t */
struct GRSceneIf: public SceneIf{
	SPR_IFDEF(GRScene);

	///	�V�[���̃����_�����O
	void Render(GRRenderIf* r);

	///	���[���h�t���[���̎擾
	GRFrameIf* GetWorld();

	///	�J�����̎擾
	GRCameraIf* GetCamera();

	///	�A�j���[�V�����R���g���[���̎擾
	GRAnimationControllerIf* GetAnimationController();

	/// �J�����̍쐬�E�ݒ�
	void SetCamera(const GRCameraDesc& desc);

	/** @brief �r�W���A���̍쐬
		@param desc �r�W���A���̃f�X�N���v�^
		@param parent �e�t���[��
		�t���[���A���b�V���A�}�e���A���A���邢�̓��C�g���쐬���Đe�t���[��parent�̉��ɒǉ�����B
		parent��NULL�̏ꍇ�̓��[���h�t���[�����e�ƂȂ�B
	 */
	GRVisualIf*  CreateVisual(const IfInfo* info, const GRVisualDesc& desc, GRFrameIf* parent = NULL);
	template <class T> GRVisualIf* CreateVisual(const T& desc, GRFrameIf* parent = NULL){
		return CreateVisual(T::GetIfInfo(), desc, parent);
	}
	

	/** @brief ����Scene������SDK��Ԃ�
		@return SDK�̃C���^�t�F�[�X
	*/	
	GRSdkIf* GetSdk();
};


//@}
}
#endif
