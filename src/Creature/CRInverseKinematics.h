/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRINVERSEKINEMATICS_H
#define CRINVERSEKINEMATICS_H

#include <Springhead.h>

#include <Foundation/Object.h>
#include "IfStubCreature.h"

#include <set>
#include <vector>

//@{
namespace Spr{;
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief ����_
*/
class CRIKControl : public SceneObject, public CRIKControlIfInit {
public:
	/// �ȗ��\�L�ptypedef
	typedef std::set<CRIKControlIf*>           CSet;
	typedef std::set<CRIKControlIf*>::iterator CSetIter;
	typedef std::set<CRIKMovableIf*>           MSet;
	typedef std::set<CRIKMovableIf*>::iterator MSetIter;

	/// �ԍ�
	int number;

	/// ����_�̂��鍄��
	PHSolidIf* solid;

	/// �ڕW�l
	Vec3d goal;

public:
	OBJECTDEF(CRIKControl, SceneObject);

	/// ����Control�ƘA������Movable
	MSet linkedMovables;

	/** @brief �f�t�H���g�R���X�g���N�^
	*/
	CRIKControl(){}

	/** @brief �R���X�g���N�^
	*/
	CRIKControl(const CRIKControlDesc& desc){
		this->solid = desc.solid;
	}

	/** @brief �ڕW�n�_��ݒ肷��
	*/
	virtual void SetGoal(Vec3d goal){ this->goal = goal; }

	/** @brief �ڕW�n�_���擾����
	*/
	Vec3d GetGoal(){ return goal; }

	/** @brief �b��ڕW�n�_���擾����
	*/
	virtual Vec3d GetTmpGoal(){ return goal; }

	/** @brief �ԍ���ݒ肷��
	*/
	void SetNumber(int i){ number = i; }

	/** @brief �ԍ����擾����
	*/
	int  GetNumber(){ return number; }
};

/** @brief �ʒu����_
*/
class CRIKControlPos : public CRIKControl, CRIKControlPosIfInit {
public:
	/// ����_�̈ʒu�i���̃��[�J�����W�n�j
	Vec3d pos;

public:
	OBJECTDEF(CRIKControlPos, CRIKControl);

	/** @brief �f�t�H���g�R���X�g���N�^
	*/
	CRIKControlPos(){}

	/** @brief �R���X�g���N�^
	*/
	CRIKControlPos(const CRIKControlPosDesc& desc) : CRIKControl(desc) {
		this->pos = desc.pos;
	}

	/** @brief �b��ڕW�n�_���擾����
	*/
	virtual Vec3d GetTmpGoal();
};

/** @brief �p������_
*/
class CRIKControlOri : public CRIKControl, CRIKControlOriIfInit {
public:
public:
	OBJECTDEF(CRIKControlOri, CRIKControl);

	/** @brief �f�t�H���g�R���X�g���N�^
	*/
	CRIKControlOri(){}

	/** @brief �R���X�g���N�^
	*/
	CRIKControlOri(const CRIKControlOriDesc& desc) : CRIKControl(desc) {
	}

	/** @brief �b��ڕW�n�_���擾����
	*/
	virtual Vec3d GetTmpGoal();
};

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief ����Ώ�
*/
class CRIKMovable : public SceneObject, public CRIKMovableIfInit {
protected:
	/// �ȗ��\�L�ptypedef
	typedef std::set<CRIKControlIf*>           CSet;
	typedef std::set<CRIKControlIf*>::iterator CSetIter;
	typedef std::set<CRIKMovableIf*>           MSet;
	typedef std::set<CRIKMovableIf*>::iterator MSetIter;
	
	/// �ԍ�
	int number;

	/// ���R�x
	int ndof;

	/// �������₷��
	float bias;

	// �v�Z�p�̈ꎞ�ϐ�
	PTM::VVector<double>     iDx, iD;
	PTM::VMatrixRow<double>  F;
	std::vector< PTM::VMatrixRow<double> >  K;

public:
	OBJECTDEF(CRIKMovable, SceneObject);

	/// ����Movable�𓮂�����Control
	CSet linkedControls;

	/// ����Movable�ƘA������Movable
	MSet linkedMovables;

	/// �v�Z����
	PTM::VVector<double> value;

	/** @brief �f�t�H���g�R���X�g���N�^
	*/
	CRIKMovable(){};

	/** @brief �R���X�g���N�^
	*/
	CRIKMovable(const CRIKMovableDesc& desc){
		this->bias = desc.bias;
	}

	/** @brief IK�̌v�Z����������
	*/
	virtual void PrepareSolve();

	/** @brief IK�̌v�Z�J�Ԃ��̂P�X�e�b�v�����s����
	*/
	virtual void ProceedSolve();

	/** @brief �v�Z���ʂ��擾����
	*/
	virtual PTM::VVector<double> GetValue(){ return value; }

	/** @brief �v�Z���ʂɏ]���Đ���Ώۂ𓮂���
	*/
	virtual void Move(){}

	/** @brief ����_��ǉ�����
	*/
	virtual void AddIKControl(CRIKControlIf* control);

	/** @brief �ԍ���ݒ肷��
	*/
	void SetNumber(int i){ number = i; }

	/** @brief �ԍ����擾����
	*/
	int  GetNumber(){ return number; }

	/** @brief ���R�x��ݒ肷��
	*/
	void SetNDOF(int n);

	/** @brief �w�肵������_�Ƃ̊Ԃ̃��R�r�A�����v�Z����
	*/
	virtual PTM::VMatrixRow<double> CalcJacobian(CRIKControlIf* control);
};

/** @brief �ʒu����\�ȍ���
*/
class CRIKMovableSolidPos : public CRIKMovable, public CRIKMovableSolidPosIfInit {
protected:
	/// ����Ώۂ̍���
	PHSolidIf* solid;

public:
	OBJECTDEF(CRIKMovableSolidPos, CRIKMovable);

	/** @brief �f�t�H���g�R���X�g���N�^
	*/
	CRIKMovableSolidPos(){SetNDOF(3);}

	/** @brief �R���X�g���N�^
	*/
	CRIKMovableSolidPos(const CRIKMovableSolidPosDesc& desc) : CRIKMovable(desc){
		SetNDOF(3);
		this->solid = desc.solid;
	}

	/** @brief �w�肵������_�Ƃ̊Ԃ̃��R�r�A�����v�Z����
	*/
	virtual PTM::VMatrixRow<double> CalcJacobian(CRIKControlIf* control);
};

/** @brief �p������\�ȍ���
*/
class CRIKMovableSolidOri : public CRIKMovable, public CRIKMovableSolidOriIfInit {
protected:
	/// ����Ώۂ̍���
	PHSolidIf* solid;

public:
	OBJECTDEF(CRIKMovableSolidOri, CRIKMovable);

	/** @brief �f�t�H���g�R���X�g���N�^
	*/
	CRIKMovableSolidOri(){SetNDOF(3);}

	/** @brief �R���X�g���N�^
	*/
	CRIKMovableSolidOri(const CRIKMovableSolidOriDesc& desc) : CRIKMovable(desc){
		SetNDOF(3);
		this->solid = desc.solid;
	}

	/** @brief �w�肵������_�Ƃ̊Ԃ̃��R�r�A�����v�Z����
	*/
	virtual PTM::VMatrixRow<double> CalcJacobian(CRIKControlIf* control);
};

/** @brief �p�x����\�ȃ{�[���W���C���g
*/
class CRIKMovableBallJointOri : public CRIKMovable, public CRIKMovableBallJointOriIfInit {
protected:
	/// ����Ώۂ̊֐�
	PHBallJointIf* joint;

public:
	OBJECTDEF(CRIKMovableBallJointOri, CRIKMovable);

	/** @brief �f�t�H���g�R���X�g���N�^
	*/
	CRIKMovableBallJointOri(){SetNDOF(3);}

	/** @brief �R���X�g���N�^
	*/
	CRIKMovableBallJointOri(const CRIKMovableBallJointOriDesc& desc) : CRIKMovable(desc){
		SetNDOF(3);
		this->joint = desc.joint;
	}

	/** @brief �w�肵������_�Ƃ̊Ԃ̃��R�r�A�����v�Z����
	*/
	virtual PTM::VMatrixRow<double> CalcJacobian(CRIKControlIf* control);
};

/** @brief �p�x����\�ȎO�A�q���W�W���C���g
*/
class CRIKMovable3HingeJointOri : public CRIKMovable, public CRIKMovable3HingeJointOriIfInit {
protected:
	/// ����Ώۂ̊֐�
	PHHingeJointIf *joint1, *joint2, *joint3;

public:
	OBJECTDEF(CRIKMovable3HingeJointOri, CRIKMovable);

	/** @brief �f�t�H���g�R���X�g���N�^
	*/
	CRIKMovable3HingeJointOri(){SetNDOF(3);}

	/** @brief �R���X�g���N�^
	*/
	CRIKMovable3HingeJointOri(const CRIKMovable3HingeJointOriDesc& desc) : CRIKMovable(desc){
		SetNDOF(3);
		this->joint1 = desc.joint1;
		this->joint2 = desc.joint2;
		this->joint3 = desc.joint3;
	}

	/** @brief �v�Z���ʂɏ]���Đ���Ώۂ𓮂���
	*/
	virtual void Move();

	/** @brief �w�肵������_�Ƃ̊Ԃ̃��R�r�A�����v�Z����
	*/
	virtual PTM::VMatrixRow<double> CalcJacobian(CRIKControlIf* control);
};

/** @brief �p�x����\�ȃq���W�W���C���g
*/
class CRIKMovableHingeJointOri : public CRIKMovable, public CRIKMovableHingeJointOriIfInit {
protected:
	/// ����Ώۂ̊֐�
	PHHingeJointIf *joint;

public:
	OBJECTDEF(CRIKMovableHingeJointOri, CRIKMovable);

	/** @brief �f�t�H���g�R���X�g���N�^
	*/
	CRIKMovableHingeJointOri(){SetNDOF(1);}

	/** @brief �R���X�g���N�^
	*/
	CRIKMovableHingeJointOri(const CRIKMovableHingeJointOriDesc& desc) : CRIKMovable(desc){
		SetNDOF(1);
		this->joint = desc.joint;
	}

	/** @brief �v�Z���ʂɏ]���Đ���Ώۂ𓮂���
	*/
	virtual void Move();

	/** @brief �w�肵������_�Ƃ̊Ԃ̃��R�r�A�����v�Z����
	*/
	virtual PTM::VMatrixRow<double> CalcJacobian(CRIKControlIf* control);
};
}
//@}
#endif//CRINVERSEKINEMATICS_H
