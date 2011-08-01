/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PHJOINTLIMIT_H
#define PHJOINTLIMIT_H

#include <Base/TQuaternion.h>
#include <Physics/PhysicsDecl.hpp>

namespace Spr{;

class PHScene;
class PHConstraintEngine;
class PHJoint1D;
class PHBallJoint;
struct PH3ElementCommonData;

/**
	�֐߂̉��͈͍S���D�C���^�t�F�[�X�͎����Ȃ��D
 */

class PHJointLimit1D{
public:
	PHJoint1D*	joint;
	double  A, Ainv, dA, b, db;
	double	f, F;
	bool	onLower, onUpper;			///< ���͈͂̉����A����ɒB���Ă���ꍇ��true
	
	virtual void	SetupLCP();
	virtual void	IterateLCP();
	virtual void	SetupCorrectionLCP();
	virtual void	IterateCorrectionLCP();


	PHJointLimit1D();
};

struct OnLimit{
	bool onLower;
	bool onUpper;
	bool& operator [] (int i){
		return i==0 ? onLower : onUpper;
	}
};

struct splinePoints{
	double swingUpper[15][6];
	int limitCount;
	bool UorL;	//true:LimitUpper,,false:LimitLower
};

class PHBallJointLimit{
public:
	PHBallJoint*	joint;
	Vec3d	A, Ainv, dA, b, db;
//	SpatialMatrix	J[2];
	Matrix3d		J[2];
//	SpatialMatrix	T[2];
	Matrix3d		T[2];
	Matrix3d		Jc;
	Matrix3d		Jcinv;
	//Vec3d	f, F;
	bool	onLower, onUpper;

	/*********************************************************
	Socket���W�n�F�e���̂ɂ��Ă���֐߂̍��W�n
	Plug���W�n�F�q���̂ɂ��Ă���֐߂̍��W�n
	�S�����W�n�F���X�e�b�v�X�V����C�S���̏����Ɏg�����W�n
	(x���F�S���~�̐ڐ������C
	 y���FlimitDir�̉�������̈�_�ƌ���钼���C
	 z���FSocket���W�n���猩��Plug���W�n��z���̕���)
	**********************************************************/

	Vec3d			currentEulerAngle;				///< ����Socket����Plug�ɐL�тĂ���x�N�g��(Jc.ez())�Ɖғ��搧���̒��S�x�N�g���Ƃ̂Ȃ��p�x(.x:swing, .y:twist, .z:swingDir)
	bool			anyLimit;				///< �ǂ����̃��~�b�g�ɂ������Ă��邩�ǂ����𒲂ׂ�t���O == (onLimit.onUpper || onLimit.onLower)
	OnLimit			onLimit[2];				///< ���搧���ɂ������Ă����true ([0]:swing, [1]:twist)	
	int				limitCount[2];			///< ��̔z��̂����܂œ����Ă���̂�������
	Vec3d			penetration;			///< �N����
	Vec3d			tanDir;
	Matrix3d		limDir;					///< �����̍S�����W�n (x��,y��,z��( = limitDir))
	Vec3d			lastEulerAngle;					///< �O��̈ʒu
	splinePoints	splinePoints[2];			///< �S���͈͂̎w��
	Vec4d			neighborAngle;			///< �S�����̋ߖT�_�̏��(SDir,Swing,���,�}��ϐ��l)
	int				constNumCondition;		///< �S�����̐�
	int				constrainedAxes[3];		///< �S�������w��

	void	CheckLimit();
	void	SplineLimit();
	void	SwingTwistLimit();
	bool	SetConstLine(const char* fileName , bool i);
	void	SetConstPoint(int num, int way, double a);
	double	GetConstLine(int num, int way){ return splinePoints[0].swingUpper[num][way]; }

	void	SetupLCP();
	void	IterateLCP();
	void	Projection(double& f, int k);
	Vec3d	CompResponseMatrix();


	Vec3d	SolveQubicFunction(Vec4d coefficient);
	Vec4d	CalcParameter(int i,int j,bool flag);
	PHBallJointLimit();
};

}

#endif
