/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PHJOINTLIMIT_H
#define PHJOINTLIMIT_H

#include <SprPhysics.h>
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

struct LimitLine{
	double SwingLow[15][5];
	double SwingUp[15][5];
};

class PHBallJointLimit{
public:
	PHBallJoint*	joint;
	Vec3d	A, Ainv, dA, b, db;
	Vec3d	f, F;
	bool	onLower, onUpper;

	/*********************************************************
	Socket���W�n�F�e���̂ɂ��Ă���֐߂̍��W�n
	Plug���W�n�F�q���̂ɂ��Ă���֐߂̍��W�n
	�S�����W�n�F���X�e�b�v�X�V����C�S���̏����Ɏg�����W�n
	(x���F�S���~�̐ڐ������C
	 y���FlimitDir�̉�������̈�_�ƌ���钼���C
	 z���FSocket���W�n���猩��Plug���W�n��z���̕���)
	**********************************************************/

	Vec3d			nowTheta;				///< ����Socket����Plug�ɐL�тĂ���x�N�g��(Jc.ez())�Ɖғ��搧���̒��S�x�N�g���Ƃ̂Ȃ��p�x(.x:swing, .y:twist, .z:swingDir)
	bool			anyLimit;				///< �ǂ����̃��~�b�g�ɂ������Ă��邩�ǂ����𒲂ׂ�t���O == (onLimit.onUpper || onLimit.onLower)
	OnLimit			onLimit[2];				///< ���搧���ɂ������Ă����true ([0]:swing, [1]:twist)	
	int				limitCount[2];			///< ��̔z��̂����܂œ����Ă���̂�������
	double			Irrupt;					///< �N����
	Vec3d			tanLine;
	int				FunNum;
	Matrix3d		limDir;					///< �����̍S�����W�n (x��,y��,z��( = limitDir))
	Vec3d			BefPos;					///< �O��̈ʒu
	LimitLine		limitLine;				///< �S���͈͂̎w��

	void	CheckLimit();
	bool	SetConstLine(char* fileName , int i);
	double	GetConstLine(int num, int way){ return limitLine.SwingUp[num][way]; }

	void	SetupLCP();
	void	IterateLCP();
	void	Projection(double& f, int k);

	PHBallJointLimit();
};

}

#endif
