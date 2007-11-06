/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRREACHINGCONTROLLER_H
#define CRREACHINGCONTROLLER_H

#include <Springhead.h>

#include <Foundation/Object.h>
#include "IfStubCreature.h"

#include "CRController.h"

//@{
namespace Spr{;
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief ���B�^���R���g���[��
*/
class CRReachingController : public CRController, public CRReachingControllerIfInit , public CRReachingControllerDesc {
private:
	/// ����p�̃o�l�_���p
	PHSpringIf *springDirect, *springHinged;

	/// ����p�̉��ڕW����
	PHSolidIf  *soTargetDirect, *soTargetHinged;

	/// �o�ߎ���
	float time;

	/// ���B�ڕW����
	float period;

	/// �B����̑ҋ@����
	float offset;

	/// �ŏI�ڕW�ʒu�E���x�E�p���E�p���x
	Vec3f finalPos, finalVel, finalAngV;
	Quaterniond finalQuat;

	/// �ڕW���̂̃��[�J�����W
	Vec3f localPos;

	/// ���䃂�[�h
	CRReachingControllerIf::ConstraintMode constraintMode;

	/// �L�����ǂ���
	bool bActive;

	/// ����Ώۂ̃{�f�B
	CRHingeHumanBodyIf* body;

public:
	OBJECTDEF(CRReachingController, CRController);
	ACCESS_DESC(CRReachingController);

	CRReachingController(){}
	CRReachingController(const CRReachingControllerDesc& desc, CRCreatureIf* c=NULL) 
		: CRController((const CRControllerDesc&)desc, c)
		, CRReachingControllerDesc(desc)
	{
	}

	/** @ brief �����������s����
	*/
	virtual void Init();

	/** @ brief ����̃X�e�b�v�����s����
	*/
	virtual void Step();

	/** @brief ����Ώۂ̍��̂�Ԃ�
	*/
	virtual PHSolidIf* GetSolid();

	/** @brief �ڕW�ʒu��ݒ肷��
		@param p �ڕW�ʒu
		@param v �ڕW�̑��x
	*/
	virtual void SetTargetPos(Vec3f p, Vec3f v);

	/** @brief �ڕW�p����ݒ肷��
		@param q �ڕW�p��
		@param av �ڕW�p���x
	*/
	virtual void SetTargetOri(Quaterniond q, Vec3f av);

	/** @brief ���B�ڕW���Ԃ�ݒ肷��
		@param t �ڕW���B����
	*/
	virtual void SetTargetTime(float t);

	/** @brief ���B�^�����J�n����
		@param mode ���B�̍S�����[�h
		@param keeptime ���B�^���I����ɕێ��𑱂��鎞�ԁi���Ȃ�ێ��𑱂���j
	*/
	virtual void Start(CRReachingControllerIf::ConstraintMode mode, float keeptime);

	/** @brief ����̎c�莞�Ԃ�Ԃ�
	*/
	virtual float GetRemainingTime();

	/** @brief ���B�󋵂�Ԃ�
	*/
	virtual CRReachingControllerIf::ReachState GetReachState();

	/** @brief ���B��Ԃ���߂�
	*/
	virtual void Reset();
};
}
//@}

#endif//CRREACHINGCONTROLLER_H
