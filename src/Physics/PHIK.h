/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PH_INVERSEKINEMATICS_H
#define PH_INVERSEKINEMATICS_H

#include <SprPhysics.h>
#include <Foundation/Object.h>
#include <Physics/PHEngine.h>

#include <vector>
#include <set>
#include <map>

namespace Spr{;

class PHIKNode;
class PHIKControlPoint;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
// IK�G���W��
// 
class PHIKEngine : public PHEngine{
public:
	SPR_OBJECTDEF(PHIKEngine);

	std::vector<PHIKNodeIf*>			nodes;
	std::vector<PHIKControlPointIf*>	controlpoints;

	std::vector<int>  nDOFsInCol;
	std::vector<int>  nDOFsInRow;
	PTM::VMatrixRow<double>   Jc;
	PTM::VMatrixRow<double> piJc;
	PTM::VVector<double>      v_;
	PTM::VVector<double>      w_;

	bool bEnabled;

	size_t numIter;
	PHIKEngine():numIter(25){ bEnabled = true; }

	int GetPriority() const {return SGBP_INVERSEKINEMATICS;}
	void Step();
	void Clear();
	void Enable(bool bEnabled){ this->bEnabled = bEnabled; };
	void SetNumIter(int numIter){ this->numIter = numIter; };

	PHIKNode* CreateIKNode(const IfInfo* ii, const PHIKNodeDesc& desc);
	PHIKControlPoint* CreateIKControlPoint(const IfInfo* ii, const PHIKControlPointDesc& desc);
	virtual bool AddChildObject(ObjectIf* o);
};

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
// IKNode�FIK�Ő��䂷��m�[�h
// 
class PHIKNode : public SceneObject{
protected:
	/// �ȗ��\�L�ptypedef
	typedef std::set<PHIKControlPointIf*>	CSet;
	typedef CSet::iterator					CSetIter;
	typedef std::set<PHIKNodeIf*>			NSet;
	typedef NSet::iterator					NSetIter;
	
	/// �ԍ�
	int number;

	/// ���R�x
	int ndof;

	/// �������ɂ���
	float bias;

	/// �쓮�p�̃o�l�_���p�i�֐ߖ{���̃o�l�_���p�͌��_�ւ̈����߂��Ƃ��č�p����j
	double spring, damper;

	/// �L���E����
	enum ENABLE_STATUS {
		ES_NONE=0,
		ES_NOORI,
		ES_ORI,
	} enableStat;

	// �v�Z�p�̈ꎞ�ϐ�
	PTM::VVector<double>     iDx, iD;
	PTM::VMatrixRow<double>  F;
	std::vector< PTM::VMatrixRow<double> >  K;

	/** @brief �֌W���邷�ׂĂ̐���_�Ƃ̃��R�r�A�������ꂼ�ꋁ�߂�
	*/
	virtual void CalcAllJacobian();

	/** @brief ���R�r�A�����N���A����
	*/
	virtual void ClearJacobian();

public:
	SPR_OBJECTDEF(PHIKNode);

	/// ����Node�𓮂�����ControlPoint
	CSet linkedControlPoints;

	/// ����Node�ƘA������Node
	NSet linkedNodes;

	// ���R�r�A��
	std::map< int,PTM::VMatrixRow<double> > Mj;

	/// IK��Iteration�̈��O�̌v�Z���ʁi��������p�j
	PTM::VVector<double> dTheta_prev;

	/// IK�̌v�Z���ʁi�p�x�j
	PTM::VVector<double> dTheta;

	/// ID�̌v�Z���ʁi�g���N�j
	PTM::VVector<double> tau;

	/** @brief �f�t�H���g�R���X�g���N�^
	*/
	PHIKNode(){};

	/** @brief �R���X�g���N�^
	*/
	PHIKNode(const PHIKNodeDesc& desc){
		SetDesc(&desc);
	}

	/** @brief �f�X�N���v�^��ݒ肷��
	*/
	virtual void SetDesc(const void* d){
		this->bias		= ((PHIKNodeDesc*)d)->bias;
		this->spring	= ((PHIKNodeDesc*)d)->spring;
		this->damper	= ((PHIKNodeDesc*)d)->damper;
	}

	/** @brief IK�̌v�Z����������
	*/
	virtual void PrepareSolve();

	/** @brief IK�̌v�Z�J�Ԃ��̂P�X�e�b�v�����s����
	*/
	virtual void ProceedSolve();

	/** @brief �v�Z���ʂ��擾����
	*/
	virtual PTM::VVector<double> GetDTheta(){ return dTheta; }

	/** @brief �v�Z���ʂɏ]���Đ���Ώۂ𓮂���
	*/
	virtual void Move(){}

	/** @brief ���R�ʒu�ɖ߂�
	*/
	virtual void MoveNatural(){}

	/** @brief ����_��ǉ�����
	*/
	virtual void AddControlPoint(PHIKControlPointIf* control);

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
	virtual PTM::VMatrixRow<double> CalcJacobian(PHIKControlPointIf* control);

	/** @brief �������ɂ�����ݒ肷��
	*/
	void SetBias(float bias){ this->bias = bias; }

	/** @brief �������ɂ������擾����
	*/
	float GetBias(){ return bias; }

	/** @brief �쓮�̂��߂̃o�l�W����ݒ肷��
	*/
	void SetSpring(double spring){ this->spring = spring; }

	/** @brief �쓮�̂��߂̃o�l�W�����擾
	*/
	double GetSpring(){ return spring; }

	/** @brief �쓮�̂��߂̃_���p�W����ݒ肷��
	*/
	void SetDamper(double damper){ this->damper = damper; }

	/** @brief �쓮�̂��߂̃_���p�W�����擾
	*/
	double GetDamper(){ return damper; }
};

class PHIKSolid : public PHIKNode{
protected:
	/// ����Ώۂ̍���
	PHSolidIf* solid;

public:
	SPR_OBJECTDEF(PHIKSolid);

	/** @brief �f�t�H���g�R���X�g���N�^
	*/
	PHIKSolid(){SetNDOF(3);}

	/** @brief �R���X�g���N�^
	*/
	PHIKSolid(const PHIKSolidDesc& desc) {
		SetNDOF(3);
		SetDesc(&desc);
	}

	/** @brief �f�X�N���v�^��ݒ肷��
	*/
	virtual void SetDesc(const void* d){
		PHIKNode::SetDesc(d);
		this->solid = ((PHIKSolidDesc*)d)->solid;
	}

	/** @brief �w�肵������_�Ƃ̊Ԃ̃��R�r�A�����v�Z����
	*/
	virtual PTM::VMatrixRow<double> CalcJacobian(PHIKControlPointIf* control);
};

class PHIKBallJoint : public PHIKNode{
protected:
	/// ����Ώۂ̊֐�
	PHBallJointIf* joint;

	/// ����Ώۊ֐߂̃o�l�_���p�����l�Ɗ�p��
	double			jSpring;
	double			jDamper;
	Quaterniond		jGoal;

	/// IK�̉�]��
	Vec3d e[3];

public:
	SPR_OBJECTDEF(PHIKBallJoint);

	/** @brief �f�t�H���g�R���X�g���N�^
	*/
	PHIKBallJoint(){SetNDOF(2);}

	/** @brief �R���X�g���N�^
	*/
	PHIKBallJoint(const PHIKBallJointDesc& desc) {
		SetNDOF(2);
		SetDesc(&desc);
	}

	/** @brief �f�X�N���v�^��ݒ肷��
	*/
	virtual void SetDesc(const void* d){
		PHIKNode::SetDesc(d);
		this->joint = ((PHIKBallJointDesc*)d)->joint;
		PHBallJointDesc dJ; DCAST(PHBallJointIf,this->joint)->GetDesc(&dJ);
		this->jSpring = dJ.spring;
		this->jDamper = dJ.damper;
		this->jGoal   = dJ.goal;
	}

	/** @brief �v�Z���ʂɏ]���Đ���Ώۂ𓮂���
	*/
	virtual void Move();

	/** @brief ���R�ʒu�ɖ߂�
	*/
	virtual void MoveNatural();

	/** @brief �w�肵������_�Ƃ̊Ԃ̃��R�r�A�����v�Z����
	*/
	virtual PTM::VMatrixRow<double> CalcJacobian(PHIKControlPointIf* control);

	/** @brief ����_��ǉ�����
	    ���Ɏp���w�萧��_���ǉ����ꂽ�Ƃ���NDOF��2����3�ɏグ�邽�߂ɃI�[�o�[���C�h
	*/
	virtual void AddControlPoint(PHIKControlPointIf* control);

	/** @brief �֌W���邷�ׂĂ̐���_�Ƃ̃��R�r�A�������ꂼ�ꋁ�߂�
	*/
	virtual void CalcAllJacobian();

	/** @brief ��]�����v�Z����
	*/
	virtual void CalcAxis();
};

class PHIKHingeJoint : public PHIKNode{
protected:
	/// ����Ώۂ̊֐�
	PHHingeJointIf *joint;

	/// ����Ώۊ֐߂̃o�l�_���p�����l�Ɗ�p��
	double		jSpring;
	double		jDamper;
	double		jGoal;

public:
	SPR_OBJECTDEF(PHIKHingeJoint);

	/** @brief �f�t�H���g�R���X�g���N�^
	*/
	PHIKHingeJoint(){SetNDOF(1);}

	/** @brief �R���X�g���N�^
	*/
	PHIKHingeJoint(const PHIKHingeJointDesc& desc) {
		SetNDOF(1);
		SetDesc(&desc);
	}

	/** @brief �f�X�N���v�^��ݒ肷��
	*/
	virtual void SetDesc(const void* d){
		PHIKNode::SetDesc(d);
		this->joint = ((PHIKHingeJointDesc*)d)->joint;
		this->jSpring = DCAST(PHHingeJointIf,this->joint)->GetSpring();
		this->jDamper = DCAST(PHHingeJointIf,this->joint)->GetDamper();
		this->jGoal   = DCAST(PHHingeJointIf,this->joint)->GetSpringOrigin();
	}

	/** @brief �v�Z���ʂɏ]���Đ���Ώۂ𓮂���
	*/
	virtual void Move();

	/** @brief ���R�ʒu�ɖ߂�
	*/
	virtual void MoveNatural();

	/** @brief �w�肵������_�Ƃ̊Ԃ̃��R�r�A�����v�Z����
	*/
	virtual PTM::VMatrixRow<double> CalcJacobian(PHIKControlPointIf* control);
};


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
// IKControlPint�FIK�̐���_
// 

class PHIKControlPoint : public SceneObject{
public:
	/// �ȗ��\�L�ptypedef
	typedef std::set<PHIKControlPointIf*>	CSet;
	typedef CSet::iterator					CSetIter;
	typedef std::set<PHIKNodeIf*>			NSet;
	typedef NSet::iterator					NSetIter;

	/// �ԍ�
	int number;

	/// ����_�̂��鍄��
	PHSolidIf* solid;

	/// ��
	Vec3d force;

	/// �L���E����
	bool isEnabled;

public:
	SPR_OBJECTDEF(PHIKControlPoint);

	/// ����ControlPoint�ƘA������Node
	NSet linkedNodes;

	/** @brief �f�t�H���g�R���X�g���N�^
	*/
	PHIKControlPoint(){}

	/** @brief �R���X�g���N�^
	*/
	PHIKControlPoint(const PHIKControlPointDesc& desc){
		SetDesc(&desc);
		force	= Vec3d(0,0,0);
		isEnabled = true;
	}

	/** @brief �f�X�N���v�^��ݒ肷��
	*/
	virtual void SetDesc(const void* d){
		this->solid = ((PHIKPosCtlDesc*)d)->solid;
	}

	/** @brief �͂�ݒ肷��
	*/
	virtual void SetForce(Vec3d force){ this->force = force; }

	/** @brief �͂��擾����
	*/
	Vec3d GetForce(){ return force; }

	/** @brief �b��ڕW�n�_���擾����
	*/
	virtual Vec3d GetTmpGoal(){ return Vec3d(); }

	/** @brief �ԍ���ݒ肷��
	*/
	void SetNumber(int i){ number = i; }

	/** @brief �ԍ����擾����
	*/
	int  GetNumber(){ return number; }

	/** @brief �L���E������ݒ肷��
	*/
	void Enable(bool enable);

	/** @brief ����Ώۂ̍��̂�ݒ肷��
	*/
	void SetSolid(PHSolidIf* solid){ this->solid = solid; }

	/** @brief ����Ώۂ̍��̂��擾����
	*/
	PHSolidIf* GetSolid(){ return this->solid; }
};

class PHIKPosCtl : public PHIKControlPoint{
	/// �ڕW�l
	Vec3d goal;

public:
	/// ����_�̈ʒu�i���̃��[�J�����W�n�j
	Vec3d pos;

	SPR_OBJECTDEF(PHIKPosCtl);

	/** @brief �f�t�H���g�R���X�g���N�^
	*/
	PHIKPosCtl(){}

	/** @brief �R���X�g���N�^
	*/
	PHIKPosCtl(const PHIKPosCtlDesc& desc) {
		SetDesc(&desc);
		goal	= Vec3d(0,0,0);
	}

	/** @brief �f�X�N���v�^��ݒ肷��
	*/
	virtual void SetDesc(const void* d){
		PHIKControlPoint::SetDesc(d);
		this->pos = ((PHIKPosCtlDesc*)d)->pos;
	}

	/** @brief �b��ڕW�n�_���擾����
	*/
	virtual Vec3d GetTmpGoal();

	/** @brief �ڕW�n�_��ݒ肷��
	*/
	virtual void SetGoal(Vec3d goal){ this->goal = goal; }

	/** @brief �ڕW�n�_���擾����
	*/
	virtual Vec3d GetGoal(){ return goal; }

	/** @brief ����_�̈ʒu��ݒ肷��
	*/
	virtual void SetPos(Vec3d pos){ this->pos = pos; }

	/** @brief ����_�̈ʒu���擾����
	*/
	virtual Vec3d GetPos(){ return pos; }
};

class PHIKOriCtl : public PHIKControlPoint{
	/// �ڕW�l
	Quaterniond goal;

public:
	SPR_OBJECTDEF(PHIKOriCtl);

	/** @brief �f�t�H���g�R���X�g���N�^
	*/
	PHIKOriCtl(){
		goal	= Quaterniond();
	}

	/** @brief �R���X�g���N�^
	*/
	PHIKOriCtl(const PHIKOriCtlDesc& desc) {
		SetDesc(&desc);
	}

	/** @brief �f�X�N���v�^��ݒ肷��
	*/
	virtual void SetDesc(const void* d){
		PHIKControlPoint::SetDesc(d);
	}

	/** @brief �b��ڕW�n�_���擾����
	*/
	virtual Vec3d GetTmpGoal();

	/** @brief �ڕW�n�_��ݒ肷��
	*/
	virtual void SetGoal(Quaterniond goal){ this->goal = goal; }

	/** @brief �ڕW�n�_���擾����
	*/
	virtual Quaterniond GetGoal(){ return goal; }
};

}

#endif
