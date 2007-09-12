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
#include "AccessDescDumpCreature.h"

#include "CRController.h"

//@{
namespace Spr{;
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief ���B�^���R���g���[��
*/
class CRReachingController : public CRController, public CRReachingControllerIfInit, public CRReachingControllerDesc {
private:
	/// �֐ߌŒ胂�[�h
	enum CRRFixMode {
		CRR_FIXED = 0, CRR_NORMAL, CRR_UNFIXED,
	} fixmode;

	/// ����p�̃o�l�_���p
	PHSpringIf* spring;

	/// ����p�̉��ڕW����
	PHSolidIf*  soTarget;

	/// �ڕW����
	float time, period;

	/// �B����̑ҋ@����
	float offset;

	/// �ŏI�ڕW�ʒu�E���x�E�p���E�p���x
	Vec3f firstPos, finalPos, finalVel, finalAngV;
	Quaterniond finalQuat;

	/// �ڕW���̂̃��[�J�����W
	Vec3f localPos;

	/// �L�����ǂ���
	bool bActive, bOri;

	/// ����Ώۂ̃{�f�B
	CRHingeHumanBodyIf* body;

public:
	OBJECTDEF(CRReachingController, CRController);
	ACCESS_DESC_CRReachingController(CRController);

	CRReachingController(){}
	CRReachingController(const CRReachingControllerDesc& desc, CRCreatureIf* c=NULL) 
		: CRReachingControllerDesc(desc) 
		, CRController((const CRControllerDesc&)desc, c)
	{
	}

	/** @ brief �����������s����
	*/
	virtual void Init();

	/** @ brief ����̃X�e�b�v�����s����
	*/
	virtual void Step();

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

	/** @brief ���쒆���ǂ����iTargetPoint���ړ����Ă��邩�ǂ����j��Ԃ�
	*/
	virtual bool IsMoving();

	/** @brief ���B�������ǂ�����Ԃ�
	*/
	virtual bool IsReached();

	/** @brief ���쒆���ǂ����i���䂪�L�����ǂ����j��Ԃ�
	*/
	virtual bool IsActive();

	/** @brief ���B�^������߁A������Ԃɖ߂�
	*/
	virtual void Reset();
};
}
//@}

#endif//CRREACHINGCONTROLLER_H
