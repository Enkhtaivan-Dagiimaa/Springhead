/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PH_IKACTUATOR_H
#define PH_IKACTUATOR_H

#include <Physics/SprPHIK.h>
#include <Physics/SprPHJoint.h>
#include <Foundation/Object.h>
#include <Physics/PHIKEndEffector.h>
#include "PhysicsDecl.hpp"
#include <set>
#include <map>

namespace Spr{;

class PHIKActuator;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
// IKActuator�FIK�Ɏg�p�ł���A�N�`���G�[�^�i�쓮���i�j
// 

class PHIKActuator : public SceneObject{
public:
	SPR_OBJECTDEF(PHIKActuator);
	SPR_DECLMEMBEROF_PHIKActuatorDesc;

	/// �ȗ��\�L�ptypedef
	typedef std::set<PHIKEndEffector*>	ESet;
	typedef std::set<PHIKActuator*>		ASet;
	
	/// ���̃A�N�`���G�[�^�œ�������G���h�G�t�F�N�^�̃��X�g
	ESet linkedEndEffectors;

	/// �u���̃A�N�`���G�[�^�œ������邢���ꂩ�̃G���h�G�t�F�N�^�v�𓮂����鑼�̃A�N�`���G�[�^
	ASet linkedActuators;

	// --- --- --- --- ---

	/// �ԍ�
	int number;

	/// ���R�x
	int ndof;

	/// ���R�x�ω��t���O
	bool bNDOFChanged;

	/// �G���h�G�t�F�N�^�ǉ��t���O
	bool bEndEffectorAdded;

	/// �A�N�`���G�[�^�ǉ��t���O
	bool bActuatorAdded;

	// --- --- --- --- ---

	/// �v�Z�p�̈ꎞ�ϐ�
	PTM::VVector<double>						alpha, beta;
	std::map< int, PTM::VMatrixRow<double> >	gamma;

	/// ���R�r�A��
	std::map< int,PTM::VMatrixRow<double> > Mj;

	/// IK��Iteration�̈��O�̌v�Z���ʁi��������p�j
	PTM::VVector<double> omega_prev;

	/// IK�̌v�Z���ʁi�p�x�j
	PTM::VVector<double> omega;

	/// ID�̌v�Z���ʁi�g���N�j
	PTM::VVector<double> tau;

	// --- --- --- --- --- --- --- --- --- ---

	/** @brief ������
	*/
	virtual void Init() {
		number = -1;
		bNDOFChanged = true;
		bEndEffectorAdded = false;
		bActuatorAdded = false;
	}

	/** @brief �f�t�H���g�R���X�g���N�^
	*/
	PHIKActuator(){
		Init();
	}

	/** @brief �R���X�g���N�^
	*/
	PHIKActuator(const PHIKActuatorDesc& desc){
		Init();
		SetDesc(&desc);
	}

	// --- --- --- --- ---

	/** @brief IK�̌v�Z����������
	*/
	virtual void PrepareSolve();

	/** @brief IK�̌v�Z�J�Ԃ��̂P�X�e�b�v�����s����
	*/
	virtual void ProceedSolve();

	/** @brief �[���t�����������܂܂�"����"�v�Z���ʂ��擾����
	*/
	virtual PTM::VVector<double> GetRawSolution(){ return omega; }

	// --- --- --- --- ---

	/** @brief �v�Z���ʂɏ]���Đ���Ώۂ𓮂���
	*/
	virtual void Move(){}

	// --- --- --- --- ---

	/** @brief ���̃A�N�`���G�[�^���g���ē�������G���h�G�t�F�N�^�A��o�^����
	*/
	void RegisterEndEffector(PHIKEndEffectorIf* endeffector);

	// --- --- --- --- ---

	/** @brief �������ɂ�����ݒ�E�擾����
	*/
	void  SetBias(float bias){ this->bias = bias; }
	float GetBias()          { return bias; }

	/** @brief ���x����̔��W����ݒ�E�擾����
	*/
	void   SetVelocityGain(double velocityGain){ this->velocityGain = velocityGain; }
	double GetVelocityGain()                   { return velocityGain; }

	/** @brief �L���E������ݒ�E�擾����
	*/
	void Enable(bool enable){ bEnabled = enable; }
	bool IsEnabled()        { return bEnabled; }

	// --- --- --- --- ---

	virtual bool		AddChildObject(ObjectIf* o);
	virtual ObjectIf*	GetChildObject(size_t pos);
	virtual	size_t		NChildObject()const;

	// --- --- --- --- --- --- --- --- --- ---
	// Non API Methods

	/** @brief ���R�r�A���v�Z�O�̏���
	*/
	virtual void BeforeCalcAllJacobian() {}

	/** @brief �֌W���邷�ׂĂ̐���_�Ƃ̃��R�r�A�������ꂼ�ꋁ�߂�
	*/
	void CalcAllJacobian();

	/** @brief �v�Z�p�ϐ�����������O�̏���
	*/
	virtual void BeforeSetupMatrix() {}

	/** @brief �v�Z�p�ϐ�����������
	*/
	void SetupMatrix();

	/** @brief �w�肵������_�Ƃ̊Ԃ̃��R�r�A�����v�Z����
	*/
	virtual void CalcJacobian(PHIKEndEffector* endeffector) {}

	/** @brief �J�Ԃ��v�Z�̈�X�e�b�v�̌�ɍs������
	*/
	virtual void AfterProceedSolve() {}
};

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

class PHIKBallActuator : public PHIKActuator{
public:
	SPR_OBJECTDEF(PHIKBallActuator);
	SPR_DECLMEMBEROF_PHIKBallActuatorDesc;

	/// ����Ώۂ̊֐�
	PHBallJointIf* joint;

	/// IK�̉�]��
	Vec3d e[3];

	// --- --- --- --- --- --- --- --- --- ---

	/** @brief ������
	*/
	virtual void Init() {
		ndof = 2;
		joint = NULL;
		PHIKActuator::Init();
	}

	/** @brief �f�t�H���g�R���X�g���N�^
	*/
	PHIKBallActuator(){
		Init();
	}

	/** @brief �R���X�g���N�^
	*/
	PHIKBallActuator(const PHIKBallActuatorDesc& desc) {
		Init();
		SetDesc(&desc);
	}

	// --- --- --- --- ---

	/** @brief �v�Z���ʂɏ]���Đ���Ώۂ𓮂���
	*/
	virtual void Move();

	// --- --- --- --- ---

	/** @brief ����Ώۂ̊֐߂�ݒ肷��i�P�A�N�`���G�[�^�ɂ��P�֐߂��K���Ή�����j
	*/
	virtual void SetJoint(PHBallJointIf* joint) { this->joint = joint; }

	/** @brief ����ΏۂƂ��Đݒ肳�ꂽ�֐߂��擾����
	*/
	virtual PHBallJointIf* GetJoint() { return this->joint; }

	// --- --- --- --- ---

	virtual bool		AddChildObject(ObjectIf* o);
	virtual ObjectIf*	GetChildObject(size_t pos);
	virtual	size_t		NChildObject()const;

	// --- --- --- --- --- --- --- --- --- ---
	// Non API Methods

	/** @brief ��]�����v�Z����
	*/
	virtual void CalcAxis();

	/** @brief ���R�r�A���v�Z�O�̏���
	*/
	virtual void BeforeCalcAllJacobian();

	/** @brief �v�Z�p�ϐ�����������O�̏���
	*/
	virtual void BeforeSetupMatrix();

	/** @brief �w�肵������_�Ƃ̊Ԃ̃��R�r�A�����v�Z����
	*/
	virtual void CalcJacobian(PHIKEndEffector* endeffector);
};

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

class PHIKHingeActuator : public PHIKActuator{
public:
	SPR_OBJECTDEF(PHIKHingeActuator);
	SPR_DECLMEMBEROF_PHIKBallActuatorDesc;

	/// ����Ώۂ̊֐�
	PHHingeJointIf *joint;

	// --- --- --- --- --- --- --- --- --- ---

	/** @brief ������
	*/
	virtual void Init() {
		ndof = 1;
		joint = NULL;
		PHIKActuator::Init();
	}

	/** @brief �f�t�H���g�R���X�g���N�^
	*/
	PHIKHingeActuator(){
		Init();
	}

	/** @brief �R���X�g���N�^
	*/
	PHIKHingeActuator(const PHIKHingeActuatorDesc& desc) {
		Init();
		SetDesc(&desc);
	}

	// --- --- --- --- ---

	/** @brief �v�Z���ʂɏ]���Đ���Ώۂ𓮂���
	*/
	virtual void Move();

	// --- --- --- --- ---

	/** @brief ����Ώۂ̊֐߂�ݒ肷��i�P�A�N�`���G�[�^�ɂ��P�֐߂��K���Ή�����j
	*/
	virtual void SetJoint(PHHingeJointIf* joint) { this->joint = joint; }

	/** @brief ����ΏۂƂ��Đݒ肳�ꂽ�֐߂��擾����
	*/
	virtual PHHingeJointIf* GetJoint() { return this->joint; }

	// --- --- --- --- ---

	virtual bool		AddChildObject(ObjectIf* o);
	virtual ObjectIf*	GetChildObject(size_t pos);
	virtual	size_t		NChildObject()const;

	// --- --- --- --- --- --- --- --- --- ---
	// Non API Methods

	/** @brief �w�肵������_�Ƃ̊Ԃ̃��R�r�A�����v�Z����
	*/
	virtual void CalcJacobian(PHIKEndEffector* endeffector);
};

}

#endif
