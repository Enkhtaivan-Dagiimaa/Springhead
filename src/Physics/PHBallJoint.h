/*
 *  Copyright (c) 2003-2007, Shoichi Hasegawa and Springhead development team 
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
#include <fstream>

namespace Spr{

struct OnLimit{
	bool onLower;
	bool onUpper;
	bool& operator [] (int i){
		return i==0 ? onLower : onUpper;
	}
};

///	�{�[���W���C���g�ɑΉ�����c���[�m�[�h
class PHBallJointNode : public PHTreeNodeND<3>{
public:
	OBJECTDEF(PHBallJointNode, PHTreeNode);
	/// �X�C���O�E�c�C�X�g�p�̎��ԕω�������p���x�ւ̃��R�r�A��
	Matrix3d	Jst;

	PHBallJoint* GetJoint(){return DCAST(PHBallJoint, PHTreeNodeND<3>::GetJoint());}
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

class PHBallJoint : public PHJointND<3>, public PHBallJointIfInit{
public:
	OBJECTDEF(PHBallJoint, PHJoint);

	double			spring, damper;			///< �o�l�A�_���p�W��
	Vec3d			limitDir;				///< ����̒��S�x�N�g���i��������Z���j
	Vec2d			nowTheta;				///< ����Socket����Plug�ɐL�тĂ���x�N�g��(Jc.ez())�Ɖғ��搧���̒��S�x�N�g���Ƃ̂Ȃ��p�x(.x:swing, .y:swingDir, .z:twist)
	Range<double>	limitSwing;				///< swing�p�͈̔�
	Range<double>	limitTwist;				///< twist�p�͈̔�
	bool			anyLimit;				///< �ǂ����̃��~�b�g�ɂ������Ă��邩�ǂ����𒲂ׂ�t���O == (onLimit.onUpper || onLimit.onLower)
	Matrix3d		Jc;						///< Socket���W�n�̑��x�E�����x�@���@Jc * �S�����W�n�̑��x�A�����x
	Matrix3d		Jcinv;					///< �S�����W�n�̑��x�A�����x    ���@Jcinv * Socket���W�n�̑��x�E�����x
	OnLimit			onLimit[2];				///< ���搧���ɂ������Ă����true ([0]:swing, [1]:twist)	
	Quaterniond		goal;					///< ����ڕW

	/// �C���^�t�F�[�X�̎���
	//virtual PHConstraintDesc::ConstraintType GetConstraintType(){return PHJointDesc::BALLJOINT;}
	
	virtual void	SetSwingRange(Range<double>  range)	{ limitSwing = range; }		///< �X�C���O�p�͈̔͂�ݒ肷��֐�
	virtual void	GetSwingRange(Range<double>& range) { range = limitSwing; }		///< �X�C���O�p�͈̔͂𓾂�֐�
	virtual void    SetTwistRange(Range<double>  range)	{ limitTwist = range; }		///< �c�C�X�g�p�͈̔͂�ݒ肷��֐�
	virtual void	GetTwistRange(Range<double>& range) { range = limitTwist; }		///< �c�C�X�g�p�͈̔͂𓾂�֐�

	virtual void	SetMotorTorque(const Vec3d& t){torque = t;}										///< ���[�^�̃g���N��ݒ肷��֐�
	virtual Vec3d	GetMotorTorque(){return torque;}												///< ���[�^�̃g���N��Ԃ��֐�
	virtual Vec3d	GetAngle(){return position;}													///< �p�x��Ԃ��֐�
	virtual Vec3d	GetVelocity(){return velocity;}													///< ���x��Ԃ��֐�

	/// ���z�֐��̃I�[�o���C�h
	virtual bool GetDesc(void* desc);																///< �f�X�N���v�^�̏��𓾂邽�߂̊֐�
	virtual void SetDesc(const void* desc);															///< �f�X�N���v�^��ݒ肷��֐�
	virtual void AddMotorTorque(){f.w() = torque * scene->GetTimeStep();}							///< �g���N��������֐�
	virtual void SetConstrainedIndex(bool* con);													///< �S�����������邩�ǂ����̔���
	virtual void ModifyJacobian();
	virtual void CompBias();																		///< �N���ʂ̔���
	virtual void Projection(double& f, int k);														///< LCP�ŗ͂��S������̂��ǂ����̔��������֐��ionLimit[0]��true�ɂȂ��Ă��鎞�ɓ����j 
	virtual void UpdateJointState();																///< joint���iSocket�ɑ΂���Plug�̌����x�N�g���j�̍X�V
	virtual void CompError();																		///< �G���[����
	
	virtual PHTreeNode* CreateTreeNode(){
		return DBG_NEW PHBallJointNode();
	}
	PHBallJoint(const PHBallJointDesc& desc = PHBallJointDesc());									/// - �R���X�g���N�^
};

}

#endif
