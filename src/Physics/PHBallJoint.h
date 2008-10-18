/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 *  PHBallJoint��swing�ɂ��|�����݊p�x�̐����ƁAtwist�ɂ��˂���̐������ł��܂��B
 *
 */
#ifndef PHBALLJOINT_H
#define PHBALLJOINT_H

#include <SprPhysics.h>
#include <Physics/PHJoint.h>
#include <Physics/PHTreeNode.h>

namespace Spr{

struct OnLimit{
	bool onLower;
	bool onUpper;
	bool& operator [] (int i){
		return i==0 ? onLower : onUpper;
	}
};

class PHBallJoint;
///	�{�[���W���C���g�ɑΉ�����c���[�m�[�h
class PHBallJointNode : public PHTreeNodeND<3>{
public:
	SPR_OBJECTDEF1(PHBallJointNode, PHTreeNode);
	SPR_DECLMEMBEROF_PHBallJointNodeDesc;

	/// �X�C���O�E�c�C�X�g�p�̎��ԕω�������p���x�ւ̃��R�r�A��
	Matrix3d	Jst;

	PHBallJoint*		GetJoint(){return PHTreeNodeND<3>::GetJoint()->Cast();}
	virtual void		CompJointJacobian();
	virtual void		CompJointCoriolisAccel();
	virtual void		UpdateJointPosition(double dt);
	virtual void		CompRelativePosition();
	virtual void		CompRelativeVelocity();
	virtual void		ModifyJacobian();
	virtual void		CompBias();
	virtual void		Projection(double& f, int i);
	PHBallJointNode(const PHBallJointNodeDesc& desc = PHBallJointNodeDesc()){}
	
};

class PHBallJoint : public PHJointND<3>{
public:
	SPR_OBJECTDEF1(PHBallJoint, PHJoint);
	SPR_DECLMEMBEROF_PHBallJointDesc;
protected:
	
	/*********************************************************
	Socket���W�n�F�e���̂ɂ��Ă���֐߂̍��W�n
	Plug���W�n�F�q���̂ɂ��Ă���֐߂̍��W�n
	�S�����W�n�F���X�e�b�v�X�V����C�S���̏����Ɏg�����W�n
	(x���F�S���~�̐ڐ������C
	 y���FlimitDir�̉�������̈�_�ƌ���钼���C
	 z���FSocket���W�n���猩��Plug���W�n��z���̕���)
	**********************************************************/

	Vec2d			nowTheta;				///< ����Socket����Plug�ɐL�тĂ���x�N�g��(Jc.ez())�Ɖғ��搧���̒��S�x�N�g���Ƃ̂Ȃ��p�x(.x:swing, .y:swingDir, .z:twist)
	bool			anyLimit;				///< �ǂ����̃��~�b�g�ɂ������Ă��邩�ǂ����𒲂ׂ�t���O == (onLimit.onUpper || onLimit.onLower)
	Matrix3d		Jc;						///< �S�����W�n�̑��x�E�����x�@���@Jc * Socket���W�n���猩��Plug���W�n�̑��x�A�����x
	Matrix3d		Jcinv;					///< Socket���W�n���猩��Plug���W�n�̑��x�A�����x  ���@Jcinv * �S�����W�n�̑��x�E�����x
	OnLimit			onLimit[2];				///< ���搧���ɂ������Ă����true ([0]:swing, [1]:twist)	
	double			fMinDt, fMaxDt;
	
	// �O���Ǐ]����p�̕ϐ��C�����Ȃ��� by Toki Aug. 2008
	Quaterniond qd,	 preQd;		///< ���鎞���̖ڕW�ʒu�ւ̉�]���x�N�g���ƂP�����O�̖ڕW�ʒu�ւ̉�]���x�N�g��
	Vec3d qdDot, preQdDot;	///< ���鎞���̖ڕW�ʒu�ւ̉�]���x�N�g���̈�K�����ƂP�����O�̖ڕW�ʒu�ւ̉�]���x�N�g���̈�K����
	Vec3d qdWDot;			///< ���鎞���̖ڕW�ʒu�ւ̉�]���x�N�g���̓�K����

	virtual void	AfterSetDesc();
	virtual coord_t GetTorqueND(){ return (coord_t&)torque; }
	friend class PHBallJointNode;
public:
	/// �C���^�t�F�[�X�̎���
	//virtual PHConstraintDesc::ConstraintType GetConstraintType(){return PHJointDesc::BALLJOINT;}
	
	virtual void		SetSwingRange(Vec2d  range)	{ limitSwing = range; }			///< �X�C���O�p�͈̔͂�ݒ肷��֐�
	virtual void		GetSwingRange(Vec2d& range) { range = limitSwing; }			///< �X�C���O�p�͈̔͂𓾂�֐�
	virtual void		SetTwistRange(Vec2d  range)	{ limitTwist = range; }			///< �c�C�X�g�p�͈̔͂�ݒ肷��֐�
	virtual void		GetTwistRange(Vec2d& range) { range = limitTwist; }			///< �c�C�X�g�p�͈̔͂𓾂�֐�

	virtual void		SetMotorTorque(const Vec3d& t){mode = MODE_TORQUE; torque = t;}					///< ���[�^�̃g���N��ݒ肷��֐�
	virtual Vec3d		GetMotorTorque(){return torque;}							///< ���[�^�̃g���N��Ԃ��֐�
	virtual Vec3d		GetAngle(){return position;}								///< �p�x��Ԃ��֐�
	virtual Quaterniond GetPosition(){return Xjrel.q;}								///< �p�x�̃N�H�[�^�j�I����Ԃ��֐�
	virtual Vec3d		GetVelocity(){return velocity;}								///< ���x��Ԃ��֐�
	virtual void		SetTorqueMax(double max){fMax = max; fMaxDt = fMax*GetScene()->GetTimeStep(); }
	virtual double		GetTorqueMax(){return fMax;}
	virtual void		SetTorqueMin(double min){fMin = min; fMinDt = fMin*GetScene()->GetTimeStep(); }
	virtual double		GetTorqueMin(){return fMin;}
	virtual void		SetDesiredVelocity(Vec3d v){mode = MODE_VELOCITY; desiredVelocity = v;}
	virtual Vec3d		GetDesiredVelocity() const {return desiredVelocity;}
	virtual void		SetTrajectoryVelocity(Vec3d v) {desiredVelocity = v;}
	virtual Vec3d		GetTrajectoryVelocity() const {return desiredVelocity;}
	virtual void		SetGoal(Quaterniond g){goal = g;}
	virtual Quaterniond GetGoal(){return goal;}
	virtual void		SetOffset(Vec3d ofst){offset = ofst;}
	virtual Vec3d		GetOffset(){return offset;}

	/// ���z�֐��̃I�[�o���C�h
	virtual void	AddMotorTorque(){f.w() = torque * GetScene()->GetTimeStep();}	///< �g���N��������֐�
	virtual void	SetConstrainedIndex(bool* con);								///< �S�����������邩�ǂ����̔���
	virtual void	ModifyJacobian();
	virtual void	CompBias();													///< �N���ʂ̔���
	virtual void	Projection(double& f, int k);								///< LCP�ŗ͂��S������̂��ǂ����̔��������֐��ionLimit[0]��true�ɂȂ��Ă��鎞�ɓ����j 
	virtual void	UpdateJointState();											///< joint���iSocket�ɑ΂���Plug�̌����x�N�g���j�̍X�V
	virtual void	CompError();												///< �G���[����
	
	virtual PHTreeNode*		CreateTreeNode(){
		return DBG_NEW PHBallJointNode();
	}
	PHBallJoint(const PHBallJointDesc& desc = PHBallJointDesc());				/// - �R���X�g���N�^
};

}

#endif
