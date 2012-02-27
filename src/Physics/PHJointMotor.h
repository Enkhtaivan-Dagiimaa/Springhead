/*
 *  Copyright (c) 2003-2012, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PH_JOINT_MOTOR_H
#define PH_JOINT_MOTOR_H

#include <Physics/SprPHJoint.h>
#include <Physics/PhysicsDecl.hpp>
#include <Physics/PHSpatial.h>

namespace Spr{;

template<int NDOF> class PHNDJoint;

class PHJoint;
class PH1DJoint;
class PHBallJoint;
class PHSpring;

///	�p�����[�^�ꗗ�i1D/3D/6D�����ʂň�����悤�ɂ��邽�߂̃f�X�N���v�^���ǂ��j
template<int NDOF>
struct PHNDJointMotorParam {
	// State
	PTM::TVector<NDOF,double> fAvg;
	SpatialVector             xs;
	bool                      bYielded;
	// Desc
	PTM::TVector<NDOF,double> spring;
	PTM::TVector<NDOF,double> damper;
	PTM::TVector<NDOF,double> secondDamper;
	PTM::TVector<NDOF,double> targetVelocity;
	PTM::TVector<NDOF,double> offsetForce;
	double yieldStress;
	double hardnessRate;
};

///	N���R�x�֐߂̊֐߃R���g���[��
template<int NDOF>
class PHNDJointMotor {
public:
	typedef PTM::TVector<NDOF,double> VecNd;

	/// �R���g���[���̑ΏۂƂȂ�֐�
	PHJoint* joint;

	///< ���݂̂΂˕��̋����i�O�v�f���f���p�j
	SpatialVector newXs;

	/// �R���X�g���N�^
	PHNDJointMotor() {}

	// ----- Motor�̋@�\

	/// �S���������肷��
	virtual void SetupAxisIndex();

	/// dA, db���v�Z����
	virtual void CompBias();

	// ----- ���̃N���X�̋@�\

	/// �e���ό`�p��CompBias
	void CompBiasElastic();

	/// �Y���ό`�p��CompBias
	void CompBiasPlastic();

	/// �~���������ǂ���
	void CheckYielded();

	// ----- �h���N���X�Ŏ�������@�\
	/// propV���v�Z����
	virtual PTM::TVector<NDOF,double> GetPropV() { return PTM::TVector<NDOF,double>(); }

	/// �p�����[�^���擾����
	virtual void GetParams(PHNDJointMotorParam<NDOF>& p) {}

	/// �p�����[�^�𔽉f����
	virtual void SetParams(PHNDJointMotorParam<NDOF>& p) {}
};

///	1���R�x�֐߂̊֐߃R���g���[��
class PH1DJointMotor : public PHNDJointMotor<1> {
public:
	/// �R���X�g���N�^
	PH1DJointMotor() {}

	// ----- PHNDJointMotor�̔h���N���X�Ŏ�������@�\
	/// propV���v�Z����
	virtual PTM::TVector<1,double> GetPropV();

	/// �p�����[�^���擾����
	virtual void GetParams(PHNDJointMotorParam<1>& p);

	/// �p�����[�^�𔽉f����
	virtual void SetParams(PHNDJointMotorParam<1>& p);
};

///	���֐߂̊֐߃R���g���[��
class PHBallJointMotor : public PHNDJointMotor<3> {
public:
	/// �R���X�g���N�^
	PHBallJointMotor() {}

	// ----- PHNDJointMotor�̔h���N���X�Ŏ�������@�\
	/// propV���v�Z����
	virtual PTM::TVector<3,double> GetPropV();

	/// �p�����[�^���擾����
	virtual void GetParams(PHNDJointMotorParam<3>& p);

	/// �p�����[�^�𔽉f����
	virtual void SetParams(PHNDJointMotorParam<3>& p);
};

///	�o�l�_���p�̃R���g���[��
class PHSpringMotor : public PHNDJointMotor<6> {
public:
	/// �R���X�g���N�^
	PHSpringMotor() {}

	// ----- PHNDJointMotor�̔h���N���X�Ŏ�������@�\
	/// propV���v�Z����
	virtual PTM::TVector<6,double> GetPropV();

	/// �p�����[�^���擾����
	virtual void GetParams(PHNDJointMotorParam<6>& p);

	/// �p�����[�^�𔽉f����
	virtual void SetParams(PHNDJointMotorParam<6>& p);
};

}

#endif//PH_JOINT_MOTOR_H
