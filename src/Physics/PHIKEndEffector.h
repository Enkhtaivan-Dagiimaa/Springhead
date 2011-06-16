/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PH_IKENDEFFECTOR_H
#define PH_IKENDEFFECTOR_H

#include <Foundation/Object.h>
#include <Physics/SprPHIK.h>
#include <set>

namespace Spr{;

class PHIKActuator;
class PHIKEndEffector;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
// IKEndEffector�FIK�̃G���h�G�t�F�N�^�i���B������Ώہj
// 

class PHIKEndEffector : public SceneObject, public PHIKEndEffectorDesc {
public:
	SPR_OBJECTDEF(PHIKEndEffector);
	ACCESS_DESC(PHIKEndEffector)

	/// �ȗ��\�L�ptypedef
	typedef std::set<PHIKEndEffector*>	ESet;
	typedef ESet::iterator				ESetIter;
	typedef std::set<PHIKActuator*>		ASet;
	typedef ASet::iterator				ASetIter;

	/// ���R�x
	int ndof;

	/// ���R�x�ω��t���O
	bool bNDOFChanged;

	/// �ԍ�
	int number;

	/// ����_�̂��鍄��
	PHSolidIf* solid;

	/// ���̃G���h�G�t�F�N�^���A������A�N�`���G�[�^
	ASet linkedActuators;

	/** @brief ����������
	*/
	void Init() {
		solid = NULL;
		ndof = 3;
		bNDOFChanged = true;
		number = -1;
	}

	/** @brief �f�t�H���g�R���X�g���N�^
	*/
	PHIKEndEffector() {
		Init();
	}

	/** @brief �R���X�g���N�^
	*/
	PHIKEndEffector(const PHIKEndEffectorDesc& desc) {
		Init();
		SetDesc(&desc);
	}

	// --- --- --- --- ---

	/** @brief ����Ώۂ̍��̂�ݒ肷��i�P�G���h�G�t�F�N�^�ɂ��P���̂��K���Ή�����j
	*/
	void SetSolid(PHSolidIf* solid) { this->solid = solid; }

	/** @brief ����ΏۂƂ��Đݒ肳�ꂽ���̂��擾����
	*/
	PHSolidIf* GetSolid() { return this->solid; }

	// --- --- --- --- ---

	/** @brief �L���E������ݒ肷��
	*/
	void Enable(bool enable);

	/** @brief �L���E�������擾����
	*/
	bool IsEnabled() { return bEnabled; }

	// --- --- --- --- ---

	void ChangeNDOF() {
		if (bPosition && bOrientation) {
			if (ndof != 6) {
				ndof = 6;
				bNDOFChanged = true;
			}
		} else if (bPosition || bOrientation) {
			if (ndof != 3) {
				ndof = 3;
				bNDOFChanged = true;
			}
		}
	}

	virtual void AfterSetDesc() {
		ChangeNDOF();
	}

	// --- --- --- --- ---

	/** @brief �ʒu�̐���̗L���E������؂�ւ���
	*/
	void EnablePositionControl(bool enable) {
		bPosition = enable;
		ChangeNDOF();
	}

	/** @brief �ʒu����̃v���C�I���e�B��ݒ肷��
	*/
	void SetPositionPriority(double priority){ positionPriority = priority; }

	/** @brief �ʒu�̖ڕW�l��ݒ肷��
	*/
	void SetTargetPosition(Vec3d position, Vec3d localPosition) {
		targetPosition		= position;
		targetLocalPosition	= localPosition;
	}

	/** @brief �ݒ肳�ꂽ�ʒu�̖ڕW�l���擾����
	*/
	Vec3d GetTargetPosition() { return targetPosition; }

	/** @brief �G���h�G�t�F�N�^�ɂ����铞�B���������ʒu�̐ݒ肳�ꂽ�ڕW�l���擾����
	*/
	Vec3d GetTargetLocalPosition() { return targetLocalPosition; }

	// --- --- --- --- ---

	/** @brief �p���̐���̗L���E������؂�ւ���
	*/
	void EnableOrientationControl(bool enable) {
		bOrientation = enable;
		ChangeNDOF();
	}

	/** @brief �p������̃v���C�I���e�B��ݒ肷��
	*/
	void SetOrientationPriority(double priority){ orientationPriority = priority; }

	/** @brief �p���̖ڕW�l��ݒ肷��
	*/
	void SetTargetOrientation(Quaterniond orientation) { targetOrientation = orientation; }

	/** @brief �ݒ肳�ꂽ�p���̖ڕW�l���擾����
	*/
	Quaterniond GetTargetOrientation() { return targetOrientation; }

	// --- --- --- --- ---

	/** @brief �͂̐���̗L���E������؂�ւ���
	*/
	void EnableForceControl(bool enable) { bForce = enable; }

	/** @brief �̖͂ڕW�l��ݒ肷��
	*/
	void SetTargetForce(Vec3d force, Vec3d workingPoint) {
		targetForce				= force;
		targetForceWorkingPoint	= workingPoint;
	}

	/** @brief �ݒ肳�ꂽ�̖͂ڕW�l���擾����
	*/
	Vec3d GetTargetForce() { return targetForce; }

	/** @brief �ݒ肳�ꂽ�͂̍�p�_���擾����
	*/
	Vec3d GetTargetForceWorkingPoint() { return targetForceWorkingPoint; }

	// --- --- --- --- ---

	/** @brief �g���N�̐���̗L���E������؂�ւ���
	*/
	void EnableTorqueControl(bool enable) { bTorque = enable; }

	/** @brief �g���N�̖ڕW�l��ݒ肷��
	*/
	void SetTargetTorque(Vec3d torque) { targetTorque = torque; }

	/** @brief �ݒ肳�ꂽ�g���N�̖ڕW�l���擾����
	*/
	Vec3d GetTargetTorque() { return targetTorque; }

	// --- --- --- --- ---

	virtual bool		AddChildObject(ObjectIf* o);
	virtual ObjectIf*	GetChildObject(size_t pos);
	virtual	size_t		NChildObject() const;
	
	// --- --- --- --- --- --- --- --- --- ---
	// Non API Methods

	/** @brief �b��ڕW�n�_���擾����
	*/
	PTM::VVector<double> GetTempTarget();

};

}

#endif
