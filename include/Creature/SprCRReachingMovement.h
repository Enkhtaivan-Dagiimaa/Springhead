/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_CRReachingMovementIf_H
#define SPR_CRReachingMovementIf_H

#include <SprFoundation.h>
#include <SprPhysics.h>

namespace Spr{;

//@{

// ------------------------------------------------------------------------------
/// ���B�^���R���g���[���̃f�X�N���v�^
struct CRReachingMovementDesc{
	float limitForce; ///< �͂̍ő�l
	PHSolidIf* solid; ///< ����Ώۂ̃\���b�h

	CRReachingMovementDesc(){
		limitForce = 1000.0f;
		solid = NULL;
	}
};

/// ���B�^���R���g���[���̃C���^�[�t�F�[�X
struct CRReachingMovementIf : SceneObjectIf{
	IF_DEF(CRReachingMovement);

	/** @brief �ڕW�ʒu��ݒ肷��
		@param p �ڕW�ʒu
		@param v �ڕW�̑��x
		@param t �ڕW���B����
		@param o ���B��̑ҋ@����
	*/
	virtual void SetTarget(Vec3f p, Vec3f v, float t, float o)= 0;

	/** @brief �ڕW�ʒu�E�p����ݒ肷��
		@param p �ڕW�ʒu
		@param v �ڕW�̑��x
		@param q �ڕW�p��
		@param av �ڕW�p���x
		@param t �ڕW���B����
		@param o ���B��̑ҋ@����
	*/
	virtual void SetTarget(Vec3f p, Vec3f v, Quaterniond q, Vec3f av, float t, float o)= 0;

	/** @ brief ����̃X�e�b�v�����s����
	*/
	virtual void Step()= 0;
};

//@}

}

#endif//SPR_CRReachingMovementIf_H
