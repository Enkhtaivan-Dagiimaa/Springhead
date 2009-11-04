/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PHMOTOR_H
#define PHMOTOR_H

#include <SprPhysics.h>
#include <Physics/PhysicsDecl.hpp>

namespace Spr{;

class PHScene;
class PHConstraintEngine;
class PHJoint1D;

/* ��`�s�v�H
	struct SpatialVectorArray2{
	SpatialVector v[2];
	SpatialVector& operator[](int i){
		return v[i];
	}
};*/
struct PH3ElementState{
	SpatialVector xs[2];			///< (3�v�f���f����p)�΂˕��̋���
};

struct PH3ElementCommonData: public PH3ElementState{
	SpatialVector xd;				///< (3�v�f���f����p)�_���p���̋���
	SpatialVector ws;				///< (3�v�f���f����p)�΂˕��̑���
	//SpatialVector fs;				///< (3�v�f���f����p)�΂˕��̗�
};

/**
	�֐߃R���g���[���D�C���^�t�F�[�X�͎����Ȃ��D
 */

class PHMotor1D : public PH3ElementCommonData{
public:
	PHJoint1D*				joint;

	double  A, Ainv, dA, b, db;
	double	f;	
	double  fMaxDt, fMinDt;			///< �֐߂̏o�����*dt�̍ő�l�A�ŏ��l
	
	virtual void	SetupLCP();
	virtual void	IterateLCP();
	PHMotor1D();
};

class PHBallJointMotor : public PH3ElementCommonData{
public:
	PHBallJoint*			joint;

	Vec3d	A, Ainv, dA, b, db;
	Vec3d	f;
	Quaterniond	propQ;
	Vec3d	propV, I;
	double	fMaxDt, fMinDt;
	double	fNorm;
	double	dt, dtinv, K, D, D2;
	bool	yieldFlag;
	std::vector<Vec3d> fs;	

	void    ElasticDeformation();
	void	PlasticDeformation();

	virtual void	SetupLCP();
	virtual void	IterateLCP();

	PHBallJointMotor();
};

}

#endif