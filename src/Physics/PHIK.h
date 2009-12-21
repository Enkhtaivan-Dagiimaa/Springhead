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
	PHIKEngine();

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
public:
	/// �ȗ��\�L�ptypedef
	typedef std::set<PHIKControlPointIf*>	CSet;
	typedef CSet::iterator					CSetIter;
	typedef std::set<PHIKNodeIf*>			NSet;
	typedef NSet::iterator					NSetIter;
	
	/// �ԍ�
	int number;

	/// �������ɂ���
	float bias;

	/// �쓮�p�̃o�l�_���p�i�֐ߖ{���̃o�l�_���p�͌��_�ւ̈����߂��Ƃ��č�p����j
	double spring, damper;

	/// �A����������_�̗L���E�������
	enum ENABLE_STATUS {
		ES_NONE=0,
		ES_NOORI,
		ES_ORI,
	} enableStat;

	/// ���̊֐ߎ��g�̗L���E����
	bool isEnabled;

	// �v�Z�p�̈ꎞ�ϐ�
	PTM::VVector<double>     alpha, beta;
	std::map< int, PTM::VMatrixRow<double> >  gamma;

public:
	SPR_OBJECTDEF(PHIKNode);

	/// ���R�x
	int ndof;

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
	PHIKNode(){ bias=1.0f; }

	/** @brief �R���X�g���N�^
	*/
	PHIKNode(const PHIKNodeDesc& desc){
		SetDesc(&desc);
	}

	/** @brief �f�X�N���v�^��ݒ肷��
	*/
	virtual void SetDesc(const void* d){
		this->isEnabled = ((PHIKNodeDesc*)d)->isEnabled;
		this->bias		= ((PHIKNodeDesc*)d)->bias;
		this->spring	= ((PHIKNodeDesc*)d)->spring;
		this->damper	= ((PHIKNodeDesc*)d)->damper;
	}

	/** @brief �֌W���邷�ׂĂ̐���_�Ƃ̃��R�r�A�������ꂼ�ꋁ�߂�
	*/
	virtual void CalcAllJacobian();

	/** @brief ���R�r�A�����N���A����
	*/
	virtual void ClearJacobian();

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

	/** @brief �L���E������ݒ肷��
	*/
	void Enable(bool enable){ isEnabled = enable; if (!enable) { MoveNatural(); } }

	/** @brief �L���E�������擾����
	*/
	bool IsEnabled(){ return isEnabled; }

	virtual bool AddChildObject(ObjectIf* o){
		PHIKControlPointIf* cp = o->Cast();
		if (cp) {
			AddControlPoint(cp);
			return true;
		}
		return false;
	}
	virtual ObjectIf* GetChildObject(size_t pos){
		for (CSetIter it=linkedControlPoints.begin(); it!=linkedControlPoints.end(); ++it) {
			if (pos == 0) {
				return *it;
			}
			pos--;
		}
		return NULL;
	}
	size_t NChildObject() {
		int cnt = 0;
		for (CSetIter it=linkedControlPoints.begin(); it!=linkedControlPoints.end(); ++it) {
			cnt++;
		}
		return cnt;
	}
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
	}

	/** @brief �w�肵������_�Ƃ̊Ԃ̃��R�r�A�����v�Z����
	*/
	virtual PTM::VMatrixRow<double> CalcJacobian(PHIKControlPointIf* control);

	virtual bool AddChildObject(ObjectIf* o){
		PHSolidIf* so = o->Cast();
		if (so) { this->solid = so; return true; }
		return false;
	}
	virtual ObjectIf* GetChildObject(size_t pos){
		if (pos == 0 && this->solid != NULL) { return this->solid; }
		if (this->solid != NULL) {
			return PHIKNode::GetChildObject(pos - 1);
		} else {
			return PHIKNode::GetChildObject(pos);
		}
		return NULL;
	}
	size_t NChildObject() {
		if (this->solid != NULL) { return 1 + PHIKNode::NChildObject(); }
		return PHIKNode::NChildObject();
	}
};

class PHIKBallJoint : public PHIKNode{
public:
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
		joint = NULL;
	}

	/** @brief �f�X�N���v�^��ݒ肷��
	*/
	virtual void SetDesc(const void* d){
		PHIKNode::SetDesc(d);
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

	virtual bool AddChildObject(ObjectIf* o){
		PHBallJointIf* jo = o->Cast();
		if (jo) {
			this->joint = jo;
			PHBallJointDesc dJ; DCAST(PHBallJointIf,this->joint)->GetDesc(&dJ);
			this->jSpring = dJ.spring;
			this->jDamper = dJ.damper;
			this->jGoal   = dJ.targetPosition;
			return true;
		}
		return PHIKNode::AddChildObject(o);
	}
	virtual ObjectIf* GetChildObject(size_t pos){
		if (pos == 0 && this->joint != NULL) { return this->joint; }
		if (this->joint != NULL) {
			return PHIKNode::GetChildObject(pos - 1);
		} else {
			return PHIKNode::GetChildObject(pos);
		}
		return NULL;
	}
	size_t NChildObject() {
		if (this->joint != NULL) { return 1 + PHIKNode::NChildObject(); }
		return PHIKNode::NChildObject();
	}
};

class PHIKHingeJoint : public PHIKNode{
public:
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
	PHIKHingeJoint(){
		joint = NULL;
		SetNDOF(1);
	}

	/** @brief �R���X�g���N�^
	*/
	PHIKHingeJoint(const PHIKHingeJointDesc& desc) {
		SetNDOF(1);
		SetDesc(&desc);
		joint = NULL;
	}

	/** @brief �f�X�N���v�^��ݒ肷��
	*/
	virtual void SetDesc(const void* d){
		PHIKNode::SetDesc(d);
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

	virtual bool AddChildObject(ObjectIf* o){
		PHHingeJointIf* jo = o->Cast();
		if (jo) {
			this->joint = jo;
			this->jSpring = DCAST(PHHingeJointIf,this->joint)->GetSpring();
			this->jDamper = DCAST(PHHingeJointIf,this->joint)->GetDamper();
			this->jGoal   = DCAST(PHHingeJointIf,this->joint)->GetTargetPosition();
			return true;
		}
		return PHIKNode::AddChildObject(o);
	}
	virtual ObjectIf* GetChildObject(size_t pos){
		if (pos == 0 && this->joint != NULL) { return this->joint; }
		if (this->joint != NULL) {
			return PHIKNode::GetChildObject(pos - 1);
		} else {
			return PHIKNode::GetChildObject(pos);
		}
		return NULL;
	}
	size_t NChildObject() {
		if (this->joint != NULL) { return 1 + PHIKNode::NChildObject(); }
		return PHIKNode::NChildObject();
	}
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
		solid = NULL;
	}

	/** @brief �f�X�N���v�^��ݒ肷��
	*/
	virtual void SetDesc(const void* d){
		isEnabled = ((PHIKControlPointDesc*)d)->isEnabled;
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

	/** @brief �L���E�������擾����
	*/
	bool IsEnabled(){ return isEnabled; }

	/** @brief ����Ώۂ̍��̂�ݒ肷��
	*/
	void SetSolid(PHSolidIf* solid){ this->solid = solid; }

	/** @brief ����Ώۂ̍��̂��擾����
	*/
	PHSolidIf* GetSolid(){ return this->solid; }

	virtual bool AddChildObject(ObjectIf* o){
		PHSolidIf* so = o->Cast();
		if (so) { this->solid = so; return true; }
		return false;
	}
	virtual ObjectIf* GetChildObject(size_t pos){
		if (pos == 0 && this->solid != NULL) { return this->solid; }
		return NULL;
	}
	size_t NChildObject() const {
		if (this->solid != NULL) { return 1; }
		return 0;
	}
};

class PHIKPosCtl : public PHIKControlPoint{
	/// �ڕW�l
	Vec3d targetPosition;

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
		targetPosition	= Vec3d(0,0,0);
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
	virtual void SetTargetPosition(Vec3d targetPosition){ this->targetPosition = targetPosition; }

	/** @brief �ڕW�n�_���擾����
	*/
	virtual Vec3d GetTargetPosition(){ return targetPosition; }

	/** @brief ����_�̈ʒu��ݒ肷��
	*/
	virtual void SetPos(Vec3d pos){ this->pos = pos; }

	/** @brief ����_�̈ʒu���擾����
	*/
	virtual Vec3d GetPos(){ return pos; }
};

class PHIKOriCtl : public PHIKControlPoint{
	/// �ڕW�l
	Quaterniond targetPosition;

public:
	SPR_OBJECTDEF(PHIKOriCtl);

	/** @brief �f�t�H���g�R���X�g���N�^
	*/
	PHIKOriCtl(){
		targetPosition	= Quaterniond();
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
	virtual void SetTargetPosition(Quaterniond targetPosition){ this->targetPosition = targetPosition; }

	/** @brief �ڕW�n�_���擾����
	*/
	virtual Quaterniond GetTargetPosition(){ return targetPosition; }
};

}

#endif
