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
	/// �X�C���O�E�c�C�X�g�p�̎��ԕω�������p���x�ւ̃��R�r�A��
	Matrix3d	Jst;

	PHBallJoint* GetJoint(){return PHTreeNodeND<3>::GetJoint()->Cast();}
	virtual void CompJointJacobian();
	virtual void CompJointCoriolisAccel();
	virtual void UpdateJointPosition(double dt);
	virtual void CompRelativePosition();
	virtual void CompRelativeVelocity();
	virtual void ModifyJacobian();
	virtual void CompBias();
	virtual void Projection(double& f, int i);
	PHBallJointNode(const PHBallJointNodeDesc& desc = PHBallJointNodeDesc()){}
	
};

class PHBallJoint : public PHJointND<3>{
public:
	SPR_OBJECTDEF1(PHBallJoint, PHJoint);

	double			spring, damper;			///< �o�l�A�_���p�W��
	Vec3d			limitDir;				///< ����̒��S�x�N�g���i��������Z���j
	Vec2d			nowTheta;				///< ����Socket����Plug�ɐL�тĂ���x�N�g��(Jc.ez())�Ɖғ��搧���̒��S�x�N�g���Ƃ̂Ȃ��p�x(.x:swing, .y:swingDir, .z:twist)
	Vec2d			limitSwing;				///< swing�p�͈̔�
	Vec2d			limitTwist;				///< twist�p�͈̔�
	bool			anyLimit;				///< �ǂ����̃��~�b�g�ɂ������Ă��邩�ǂ����𒲂ׂ�t���O == (onLimit.onUpper || onLimit.onLower)
	Matrix3d		Jc;						///< Socket���W�n�̑��x�E�����x�@���@Jc * �S�����W�n�̑��x�A�����x
	Matrix3d		Jcinv;					///< �S�����W�n�̑��x�A�����x    ���@Jcinv * Socket���W�n�̑��x�E�����x
	OnLimit			onLimit[2];				///< ���搧���ɂ������Ă����true ([0]:swing, [1]:twist)	
	Quaterniond		goal;					///< ����ڕW
	double			fMax, fMin;				///< �֐߂̏o����͂̍ő�l�A�ŏ��l

public:
	/// �C���^�t�F�[�X�̎���
	//virtual PHConstraintDesc::ConstraintType GetConstraintType(){return PHJointDesc::BALLJOINT;}
	
	virtual void	SetSwingRange(Vec2d  range)	{ limitSwing = range; }			///< �X�C���O�p�͈̔͂�ݒ肷��֐�
	virtual void	GetSwingRange(Vec2d& range) { range = limitSwing; }			///< �X�C���O�p�͈̔͂𓾂�֐�
	virtual void    SetTwistRange(Vec2d  range)	{ limitTwist = range; }			///< �c�C�X�g�p�͈̔͂�ݒ肷��֐�
	virtual void	GetTwistRange(Vec2d& range) { range = limitTwist; }			///< �c�C�X�g�p�͈̔͂𓾂�֐�

	virtual void	SetMotorTorque(const Vec3d& t){torque = t;}					///< ���[�^�̃g���N��ݒ肷��֐�
	virtual Vec3d	GetMotorTorque(){return torque;}							///< ���[�^�̃g���N��Ԃ��֐�
	virtual Vec3d	GetAngle(){return position;}								///< �p�x��Ԃ��֐�
	virtual Vec3d	GetVelocity(){return velocity;}								///< ���x��Ԃ��֐�
	virtual void	SetTorqueMax(double max){fMax = max * scene->GetTimeStep();}///< f���t���ő�͐ϒl[N�sec]�Ƃ��ēo�^����i������f[N]�j
	virtual double	GetTorqueMax(){return fMax / scene->GetTimeStep();}			///< f���t����͂̍ő�l[N]�ɖ߂��ĕԂ�
	virtual void	SetTorqueMin(double min){fMin = min * scene->GetTimeStep();}///< f���t���ŏ��͐ϒl[N�sec]�Ƃ��ēo�^����i������f[N]�j
	virtual double	GetTorqueMin(){return fMin / scene->GetTimeStep();}			///< f���t����͂̍ŏ��l[N]�ɖ߂��ĕԂ�

	/// ���z�֐��̃I�[�o���C�h
	virtual bool	GetDesc(void* desc) const ;									///< �f�X�N���v�^�̏��𓾂邽�߂̊֐�
	virtual void	SetDesc(const void* desc);									///< �f�X�N���v�^��ݒ肷��֐�
	virtual void	AddMotorTorque(){f.w() = torque * scene->GetTimeStep();}	///< �g���N��������֐�
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
