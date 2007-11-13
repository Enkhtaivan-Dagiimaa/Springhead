/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PHBALLJOINT_H
#define PHBALLJOINT_H

#include <SprPhysics.h>
#include <Physics/PHJoint.h>
#include <Physics/PHTreeNode.h>

namespace Spr{;

/** ��]�̃X�C���O�E�c�C�X�g�p�\�� */
struct SwingTwist : public Vec3d{
	double SwingDir(){return item(0);}
	double Swing(){return item(1);}
	double Twist(){return item(2);}
	void ToQuaternion(Quaterniond& q);
	void FromQuaternion(const Quaterniond& q);
	void Jacobian(Matrix3d& J);
	void JacobianInverse(Matrix3d& J, const Quaterniond& q);
	void Coriolis(Vec3d& c, const Vec3d& sd);
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
	
	bool		swingOnLower, swingOnUpper, twistOnLower, twistOnUpper;
	double		swingLower, swingUpper;
	double		twistLower, twistUpper;
	double      spring, damper;
	Quaterniond origin;
	
	SwingTwist	angle; //, velocity;	///< �X�C���O�E�c�C�X�g�p�\���̊p�x�Ɗp���x
	//Vec3d		torque;
	/// �p���x����X�C���O�E�c�C�X�g�p�̎��ԕω����ւ̃��R�r�A��
	Matrix3d	Jstinv;
	/// �C���^�t�F�[�X�̎���
	//virtual PHConstraintDesc::ConstraintType GetConstraintType(){return PHJointDesc::BALLJOINT;}
	virtual void	SetSwingRange(double u){swingUpper = u;}
	virtual double	GetSwingRange(){return swingUpper;}
	virtual void	SetTwistRange(double l, double u){twistLower = l; twistUpper = u;}
	virtual void	GetTwistRange(double& l, double& u){l = twistLower; u = twistUpper;}
	virtual void	SetMotorTorque(const Vec3d& t){torque = t;}
	virtual Vec3d	GetMotorTorque(){return torque;}
	virtual Vec3d	GetAngle(){return position;}
	virtual Vec3d	GetVelocity(){return velocity;}
	
	/// ���z�֐��̃I�[�o���C�h
	virtual bool GetDesc(void* desc);
	virtual void SetDesc(const void* desc);
	virtual void AddMotorTorque(){f.w() = torque * scene->GetTimeStep();}
	virtual void SetConstrainedIndex(bool* con);
	virtual void ModifyJacobian();
	virtual void CompBias();
	virtual void Projection(double& f, int k);
	virtual void UpdateJointState();
	virtual void CompError();
	
	virtual PHTreeNode* CreateTreeNode(){
		return DBG_NEW PHBallJointNode();
	}
	PHBallJoint(const PHBallJointDesc& desc = PHBallJointDesc());
};

}

#endif
