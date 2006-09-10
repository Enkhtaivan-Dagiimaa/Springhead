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
	///	�t���[���̍쐬
	virtual GRFrameIf* CreateFrame(const GRFrameDesc& desc)=0;
	///	�V�[���̃����_�����O
	virtual void Render(GRRenderIf* r)=0;
	///	World
	virtual GRFrameIf* GetWorld()=0;
	///	Camera
	virtual GRCameraIf* GetCamera()=0;
	/** @brief ����Scene������SDK��Ԃ�
		@return SDK�̃C���^�t�F�[�X
	*/	
	virtual GRSdkIf* GetSdk()=0;
};


//@}
}
#endif
