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
	PHSolidIf* solid; ///< ����Ώۂ̃\���b�h
	CRHingeHumanIf* human; ///< ����Ώۂ̐l�^
	float limitForce; ///< �͂̍ő�l
	float springPos, damperPos; ///< �ʒu����̃o�l�_���p�W��
	float springOri, damperOri; ///< �p������̃o�l�_���p�W��
	float softenRate; ///< �֐߂��_�炩����Ԃɂ��邽�߂̃o�l�_���p�{��
	float hardenRate; ///< �֐߂��ł���Ԃɂ��邽�߂̃o�l�_���p�{��

	CRReachingMovementDesc(){
		solid = NULL;
		human = NULL;
		limitForce = 1000.0f;
		springPos = 500.0f;  damperPos =  20.0f;
		springOri =  10.0f;  damperOri =   0.5f;
		softenRate = 0.2f;
		hardenRate = 5.0f;
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
