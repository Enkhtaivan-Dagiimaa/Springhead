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
#include <Physics/PHJointLimit.h>
#include <Physics/PHMotor.h>

namespace Spr{

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
	//virtual void		ModifyJacobian();
	//virtual void		CompBias();
	//virtual void		Projection(double& f, int i);
	PHBallJointNode(const PHBallJointNodeDesc& desc = PHBallJointNodeDesc()){}
	
};

enum PHDeformationType;
class PHBallJoint : public PHJointND<3>{
public:
	SPR_OBJECTDEF1(PHBallJoint, PHJoint);
	SPR_DECLMEMBEROF_PHBallJointDesc;
protected:
	friend class PHBallJointLimit;
	friend class PHBallJointMotor;
	PHBallJointLimit	limit;		///< ���͈͍S��
	PHBallJointMotor	motor;		///< �֐߃R���g���[��

	Matrix3d		Jc;						///< �S�����W�n�̑��x�E�����x�@���@Jc * Socket���W�n���猩��Plug���W�n�̑��x�A�����x
	Matrix3d		Jcinv;					///< Socket���W�n���猩��Plug���W�n�̑��x�A�����x  ���@Jcinv * �S�����W�n�̑��x�E�����x
	
	// �O���Ǐ]����p�̕ϐ��C�����Ȃ��� by Toki Aug. 2008
	Quaterniond qd,	 preQd;		///< ���鎞���̖ڕW�ʒu�ւ̉�]���x�N�g���ƂP�����O�̖ڕW�ʒu�ւ̉�]���x�N�g��
	Vec3d qdDot, preQdDot;	///< ���鎞���̖ڕW�ʒu�ւ̉�]���x�N�g���̈�K�����ƂP�����O�̖ڕW�ʒu�ւ̉�]���x�N�g���̈�K����
	Vec3d qdWDot;			///< ���鎞���̖ڕW�ʒu�ւ̉�]���x�N�g���̓�K����

	//virtual void	ControlCheck(double dtinv);	//< ����������邩�̃`�F�b�N
	//virtual void	MovableCheck(double dtinv);	//< ����������邩�̃`�F�b�N
	//virtual void	AfterSetDesc();
	//virtual coord_t GetTorqueND(){ return (coord_t&)torque; }
	friend class PHBallJointNode;
public:
	/// �C���^�t�F�[�X�̎���
	//virtual PHConstraintDesc::ConstraintType GetConstraintType(){return PHJointDesc::BALLJOINT;}
	
	virtual void		SetSwingRange(Vec2d  range)	{ limitSwing = range; }			///< �X�C���O�p�͈̔͂�ݒ肷��֐�
	virtual void		GetSwingRange(Vec2d& range) { range = limitSwing; }			///< �X�C���O�p�͈̔͂𓾂�֐�
	virtual void		SetTwistRange(Vec2d  range)	{ limitTwist = range; }			///< �c�C�X�g�p�͈̔͂�ݒ肷��֐�
	virtual void		GetTwistRange(Vec2d& range) { range = limitTwist; }			///< �c�C�X�g�p�͈̔͂𓾂�֐�

	virtual void		SetMotorTorque(const Vec3d& t){offsetForce = t;}			///< ���[�^�̃g���N��ݒ肷��֐�
	virtual Vec3d		GetMotorTorque(){return offsetForce;}						///< ���[�^�̃g���N��Ԃ��֐�
	virtual Vec3d		GetAngle(){return position;}								///< �p�x��Ԃ��֐�
	virtual Quaterniond GetPosition(){return Xjrel.q;}								///< �p�x�̃N�H�[�^�j�I����Ԃ��֐�
	virtual Vec3d		GetVelocity(){return velocity;}								///< ���x��Ԃ��֐�
	virtual void		SetTorqueMax(double max){fMax = fabs(max); }
	virtual double		GetTorqueMax(){return fMax;}
	virtual void		SetTargetVelocity(Vec3d v){ targetVelocity = v;}
	virtual Vec3d		GetTargetVelocity() const {return targetVelocity;}
	virtual void		SetTrajectoryVelocity(Vec3d v) { targetVelocity = v;}
	virtual Vec3d		GetTrajectoryVelocity() const {return targetVelocity;}
	virtual void		SetTargetPosition(Quaterniond p){ targetPosition = p;}
	virtual Quaterniond GetTargetPosition(){return targetPosition;}

	virtual void		SetSpring(double spring){ this->spring = spring; }
	virtual double		GetSpring(){ return spring; }
	virtual void		SetDamper(double damper){ this->damper = damper; }
	virtual double		GetDamper(){ return damper; }

	virtual void		SetOffsetForce(Vec3d ofst){offsetForce = ofst;}
	virtual Vec3d		GetOffsetForce(){ return offsetForce;
							// API���Ə������e�ɐH���Ⴂ������Ǝv����
							/*Vec3d ans, tmp;
							if(anyLimit)
								ans = Vec3d();
							else
								GetConstraintForce(tmp, ans);
							return ans;*/
						}
	virtual bool		IsLimit(){return limit.anyLimit;}
	/// �S���_�̓���
	virtual bool		SetConstLine(char* fileName , int i){ return limit.SetConstLine(fileName, i); }
	/// �S���_�̍��W�̎擾
	virtual double		GetConstLine(int num, int way){ return limit.GetConstLine(num, way); }
	/// ConstLine�Ǝg��Twist�p�͈̔͐���
	virtual void		SetTwistPole(Vec2d range){ poleTwist = range; }

	/// �C���^�t�F�[�X�̎���
	double  GetSecondDamper()				{return secondDamper;}
	void	SetSecondDamper(double input)	{secondDamper = input;}
	double  GetYieldStress()				{return yieldStress;}
	void	SetYieldStress(double input)	{yieldStress = input;}
	double  GetHardnessRate()				{return hardnessRate;}
	void	SetHardnessRate(double input)	{hardnessRate = input;}
	void	SetInertia(const Vec3d i)		{Inertia = i;}
	Vec3d	GetInertia()					{return Inertia;}
	void	SetDefomationType(int t)		{type = (PHJointDesc::PHDeformationType)t;}
	int 	GetDefomationType()				{return (int)type;}
	double 	GetmotorfNorm()					{return motor.fNorm;}
	PHJointDesc::PHDeformationType 	GetDeformationMode();

	/// ���z�֐��̃I�[�o���C�h
	virtual void	SetupLCP();
	virtual	void	IterateLCP();
	virtual void	CompBias();							///< �N���ʂ̔���
	virtual void	UpdateJointState();					///< joint���iSocket�ɑ΂���Plug�̌����x�N�g���j�̍X�V
	virtual void	CompError();						///< �G���[����
	
	virtual PHTreeNode*		CreateTreeNode(){
		return DBG_NEW PHBallJointNode();
	}
	PHBallJoint(const PHBallJointDesc& desc = PHBallJointDesc());				/// - �R���X�g���N�^
};

}

#endif
