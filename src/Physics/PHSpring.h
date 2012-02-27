/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PHSPRING_H
#define PHSPRING_H

#include <Physics/SprPHJoint.h>
#include <Physics/PHConstraint.h>
#include <Physics/PHTreeNode.h>
#include <Physics/PHJointMotor.h>

namespace Spr{;

class PHSpring : public PHJoint{
	friend class PHSpringMotor;

	/// �֐߃R���g���[���DPHSpring��6���R�x�֐߁i���S�����Ȃ��j�ł���C�����I�ȃR���g���[����Motor���S���D
	PHSpringMotor  motor;

public:
	SPR_OBJECTDEF(PHSpring);
	SPR_DECLMEMBEROF_PHSpringDesc;

	/// �R���X�g���N�^
	PHSpring(const PHSpringDesc& desc = PHSpringDesc()) {
		motor.joint = this;

		SetDesc(&desc);

		nMovableAxes = 6;
		for (int i=0; i<6; ++i) { movableAxes[i] = i; }
		InitTargetAxes();
	}

	// ----- PHConstraint�̔h���N���X�Ŏ�������@�\

	/// �ǂ̎��R�x�𑬓x�S�����邩��ݒ�
	virtual void SetupAxisIndex() {
		PHJoint::SetupAxisIndex();
		motor.SetupAxisIndex();
	}

	/// LCP�̕␳�l�̌v�Z�D�덷�C���p
	virtual void CompBias() {
		PHJoint::CompBias();
		motor.CompBias();
	}

	// ----- �C���^�t�F�[�X�̎���

	virtual void SetSpring(const Vec3d& spring) { this->spring = spring; }
	virtual Vec3d GetSpring() { return spring; }
	virtual void SetDamper(const Vec3d& damper) { this->damper = damper; }
	virtual Vec3d GetDamper() { return damper; }
	virtual void SetSecondDamper(const Vec3d& secondDamper) { this->secondDamper = secondDamper; }
	virtual Vec3d GetSecondDamper() { return secondDamper; }
	virtual void SetSpringOri(const double& springOri) { this->springOri = springOri; }
	virtual double GetSpringOri() { return springOri; }
	virtual void SetDamperOri(const double& damperOri) { this->damperOri = damperOri; }
	virtual double GetDamperOri() { return damperOri; }
	virtual void SetSecondDamperOri(const double& secondDamperOri) { this->secondDamperOri = secondDamperOri; }
	virtual double GetSecondDamperOri() { return secondDamperOri; }
	virtual void SetYieldStress(const double& yieldStress) { this->yieldStress = yieldStress; }
	virtual double GetYieldStress() { return yieldStress; }
	virtual void SetHardnessRate(const double& hardnessRate) { this->hardnessRate = hardnessRate; }
	virtual double GetHardnessRate() { return hardnessRate; }
	virtual void SetSecondMoment(Vec3d sM) { secondMoment = sM; }
	virtual Vec3d GetSecondMoment() { return secondMoment; }
};

}

#endif
