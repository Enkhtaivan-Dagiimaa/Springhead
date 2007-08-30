/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_CRAttentionControllerIf_H
#define SPR_CRAttentionControllerIf_H

#include <SprFoundation.h>
#include <SprPhysics.h>

namespace Spr{;

//@{

struct CRAttentionListIf;
struct CRGazeControllerIf;

// ------------------------------------------------------------------------------
/// ���ӃR���g���[���̃f�X�N���v�^
struct CRAttentionControllerDesc{
	/// ���ӑΏۂ̃��X�g
	CRAttentionListIf*  attentionList;

	/// �����i���o�I���Ӂj�R���g���[��
	CRGazeControllerIf* gazeController;

	CRAttentionControllerDesc(){
		attentionList  = NULL;
		gazeController = NULL;
	}
};

/// ���ӃR���g���[���̃C���^�t�F�[�X
struct CRAttentionControllerIf : SceneObjectIf{
	IF_DEF(CRAttentionController);

	/** @brief ���ӂ̐�������s����
	*/
	virtual void Step()= 0;
};

//@}

}

#endif//SPR_CRAttentionControllerIf_H
