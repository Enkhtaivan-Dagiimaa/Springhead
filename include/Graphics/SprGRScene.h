/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
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
	IF_DEF(GRScene);

	///	�V�[���̃����_�����O
	virtual void Render(GRRenderIf* r)=0;

	///	���[���h�t���[���̎擾
	virtual GRFrameIf* GetWorld()=0;

	///	�J�����̎擾
	virtual GRCameraIf* GetCamera()=0;

	/// �J�����̍쐬�E�ݒ�
	virtual void SetCamera(const GRCameraDesc& desc)=0;

	/** @brief �r�W���A���̍쐬
		@param desc �r�W���A���̃f�X�N���v�^
		@param parent �e�t���[��
		�t���[���A���b�V���A�}�e���A���A���邢�̓��C�g���쐬���Đe�t���[��parent�̉��ɒǉ�����B
		parent��NULL�̏ꍇ�̓��[���h�t���[�����e�ƂȂ�B
	 */
	virtual GRVisualIf*  CreateVisual(const GRVisualDesc& desc, GRFrameIf* parent = NULL)=0;

	/** @brief ����Scene������SDK��Ԃ�
		@return SDK�̃C���^�t�F�[�X
	*/	
	virtual GRSdkIf* GetSdk()=0;
};


//@}
}
#endif
