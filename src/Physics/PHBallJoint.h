/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

// ����������---tquaternion.h-----------------------------------------
//fromQuaternion()��0�x�ߕӂœ����ꍇ�A360�x�Ƃ��ĔF������̂��A
//0�x�Ƃ��ĔF������̂��ł��̂��Ɣ��U���Ȃ��悤�ȃA���S���Y���𓱓�����K�v������

#ifndef PHBALLJOINT_H
#define PHBALLJOINT_H

#include <SprPhysics.h>
#include <Physics/PHJoint.h>
#include <Physics/PHTreeNode.h>

namespace Spr{;


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
	struct OnLimit{
		bool onLower;
		bool onUpper;
		bool& operator [] (int i){
			return i==0 ? onLower : onUpper;
		}
	};
	LimitST limit;		//	����
	OnLimit onLimit[3];				//	���搧���ɂ������Ă���� true
// ������-------------------------------------------------------------------------------------
//	Swing���FonLimit[0]
//	Swing�p�FonLimit[1]
//	Twist�p�FonLimit[2]
//	�ł���B�Ⴆ��Swing����onLower�𓾂邽�߂ɂ�onLimit[0][0] or onLimit[0].onLower�Ǝw�肷��
// -------------------------------------------------------------------------------------------	

	Vec3d		spring, damper;			/// �o�l�A�_���p�W��
	Vec3f		goalVector;				/// �S�[���x�N�g���iSocket���W���p�^�j
	Vec3f		currentVector;			/// ���A�\�P�b�g����v���O�ɂ����ĐL�тĂ���x�N�g��
	Vec3f		nowTheta;				/// Corn�̊֐��p��angle
	Vec3f		velocity;				/// Corn�̊֐��p��velocity

	//Vec3d		torque;
	/// �p���x����X�C���O�E�c�C�X�g�p�̎��ԕω����ւ̃��R�r�A��
	Matrix3d	Jstinv;
	/// �C���^�t�F�[�X�̎���
	//virtual PHConstraintDesc::ConstraintType GetConstraintType(){return PHJointDesc::BALLJOINT;}

	virtual void	SetSwingDirRange(double l, double u){limit.upper.SwingDir() = u; limit.lower.SwingDir() = l;}
	virtual void	GetSwingDirRange(double& l, double& u){l = limit.lower.SwingDir(); u = limit.upper.SwingDir();}

	virtual void	SetSwingRange(double l, double u){limit.upper.Swing() = u; limit.lower.Swing() = l;}
	virtual void	GetSwingRange(double& l, double& u){l = limit.lower.Swing(); u = limit.upper.Swing();}

	virtual void	SetTwistRange(double l, double u){limit.lower.Twist() = l; limit.upper.Twist() = u;}
	virtual void	GetTwistRange(double& l, double& u){l = limit.lower.Twist(); u = limit.upper.Twist();}

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
