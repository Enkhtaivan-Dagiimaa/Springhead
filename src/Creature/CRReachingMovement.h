/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRREACHINGMOVEMENT_H
#define CRREACHINGMOVEMENT_H

#include <Springhead.h>

#include <Foundation/Object.h>
#include "IfStubCreature.h"

//@{
namespace Spr{;
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief ���B�^���R���g���[��
*/
class CRReachingMovement : public SceneObject, public CRReachingMovementIfInit, public CRReachingMovementDesc {
private:
	/// ����p�̃o�l�_���p
	PHSpringIf* spring;

	/// ����p�̉��ڕW����
	PHSolidIf*  soTarget;

	/// �ڕW����
	float time, period;

	/// �B����̑ҋ@����
	float offset;

	/// �p������̃o�l�_���p�W��
	float springOri, damperOri;

	/// �ŏI�ڕW�ʒu�E���x�E�p���E�p���x
	Vec3f finalPos, finalVel, finalAngV;
	Quaterniond finalQuat;

	/// �ڕW���̂̃��[�J�����W
	Vec3f localPos;

	/// �L�����ǂ���
	bool bActive, bOri;

	/// ���������s��
	void Init();

	/// ���Z�b�g����
	void Reset();

public:
	OBJECTDEF(CRReachingMovement, SceneObject);
	ACCESS_DESC(CRReachingMovement);

	CRReachingMovement(){}
	CRReachingMovement(const CRReachingMovementDesc& desc, SceneIf* s=NULL) : CRReachingMovementDesc(desc) {
		if(s){SetScene(s);}
		Init();
		Reset();
	}

	/** @brief �ڕW�ʒu��ݒ肷��
		@param p �ڕW�ʒu
		@param v �ڕW�̑��x
		@param t �ڕW���B����
		@param o ���B��̑ҋ@����
	*/
	virtual void SetTarget(Vec3f p, Vec3f v, float t, float o);

	/** @brief �ڕW�ʒu�E�p����ݒ肷��
		@param p �ڕW�ʒu
		@param v �ڕW�̑��x
		@param q �ڕW�p��
		@param av �ڕW�p���x
		@param t �ڕW���B����
		@param o ���B��̑ҋ@����
	*/
	virtual void SetTarget(Vec3f p, Vec3f v, Quaterniond q, Vec3f av, float t, float o);

	/** @ brief ����̃X�e�b�v�����s����
	*/
	virtual void Step();
};
}
//@}

#endif//CRREACHINGMOVEMENT_H
