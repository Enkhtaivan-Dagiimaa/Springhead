/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PHJOINT_H
#define PHJOINT_H

#include <SprPhysics.h>
#include <Physics/PHConstraint.h>

namespace Spr{;

class PHJoint : public PHConstraint{
public:
	SPR_OBJECTDEF_ABST(PHJoint);
	
	/// ABA�őΉ�����PHTreeNode�̔h���N���X�𐶐����ĕԂ�
	virtual PHTreeNode* CreateTreeNode(){return NULL;}
	
	/// �R���X�g���N�^
	PHJoint();

};

template<int NDOF>
class PHJointND : public PHJoint{
public:
	typedef	PTM::TVector<NDOF, double> coord_t;

	int		axisIndex[NDOF];
	coord_t position, velocity, torque;
	
	//virtual void	SetMotorTorque(coord_t t){torque = t;}
	//virtual coord_t GetMotorTorque(){return torque;}
	PHJointND(){
		position.clear();
		velocity.clear();
		torque.clear();
	}
};

class PHJoint1D : public PHJointND<1>{
public:
	SPR_OBJECTDEF_ABST(PHJoint1D);
	
	bool	onLower, onUpper;			///< ���͈͂̉����A����ɒB���Ă���ꍇ��true
	double	lower, upper;				///< ���͈͂̉����A���
	double	pos_d, vel_d;				///< �ڕW�ψʁA�ڕW���x
	double	spring, origin, damper;		///< �o�l�W���A�o�l���_�A�_���p�W��
	double  fMax, fMin;					///< �֐߂̏o����͂̍ő�l�A�ŏ��l

	/// �C���^�t�F�[�X�̎���
	virtual double	GetPosition(){return position[0];}
	virtual double	GetVelocity(){return velocity[0];}
	virtual void	SetMotorTorque(double t){mode = MODE_TORQUE; torque[0] = t;}
	virtual double	GetMotorTorque(){return torque[0];}
	virtual void	SetRange(double l, double u){lower = l, upper = u;}
	virtual void	GetRange(double& l, double& u){l = lower, u = upper;}
	//virtual void SetDesiredPosition(double p){mode = MODE_POSITION; pos_d = p;}
	//virtual double GetDesiredPosition(){return pos_d;}
	virtual void	SetDesiredVelocity(double v){mode = MODE_VELOCITY; vel_d = v;}
	virtual double	GetDesiredVelocity(){return vel_d;}
	virtual void	SetSpring(double K){spring = K;}
	virtual double	GetSpring(){return spring;}
	virtual void	SetSpringOrigin(double org){origin = org;}
	virtual double	GetSpringOrigin(){return origin;}
	virtual void	SetDamper(double D){damper = D;}
	virtual double	GetDamper(){return damper;}
	virtual void	SetTorqueMax(double max){fMax = max * scene->GetTimeStep();}	///< f���t���ő�͐ϒl[N�sec]�Ƃ��ēo�^����i������f[N]�j
	virtual double	GetTorqueMax(){return fMax / scene->GetTimeStep();}				///< f���t����͂̍ő�l[N]�ɖ߂��ĕԂ�
	virtual void	SetTorqueMin(double min){fMin = min * scene->GetTimeStep();}	///< f���t���ŏ��͐ϒl[N�sec]�Ƃ��ēo�^����i������f[N]�j
	virtual double	GetTorqueMin(){return fMin / scene->GetTimeStep();}				///< f���t����͂̍ŏ��l[N]�ɖ߂��ĕԂ�

	/// �I�[�o���C�h
	virtual bool	GetDesc(void* desc);
	virtual void	SetDesc(const void* desc);
	virtual void	AddMotorTorque(){f[axisIndex[0]] = torque[0] * scene->GetTimeStep();}
	virtual void	SetConstrainedIndex(bool* con);
	virtual void	SetConstrainedIndexCorrection(bool* con);
	virtual void	Projection(double& f, int k);
	virtual void	ProjectionCorrection(double& F, int k);
	PHJoint1D();
};

}

#endif
