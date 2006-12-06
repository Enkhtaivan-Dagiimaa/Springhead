/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PHCONTACTPOINT_H
#define PHCONTACTPOINT_H

#include <SprPhysics.h>
#include "PHConstraint.h"

class PHShapePairForLCP;

namespace Spr{;

class PHContactPoint : public PHConstraint, PHContactPointIfInit{
public:
	OBJECT_DEF(PHContactPoint);
	PHShapePairForLCP* shapePair;
	Vec3d pos;
	virtual PHConstraintDesc::ConstraintType GetConstraintType(){return PHConstraintDesc::CONTACT;}
	//virtual void CompConstraintJacobian();
	//virtual void CompError(double dt);
	virtual void SetConstrainedIndex(bool* con);
	virtual void CompBias();
	virtual void Projection(double& f, int k);
	//virtual void ProjectionCorrection(double& F, int k);
	PHContactPoint(){}
	PHContactPoint(PHShapePairForLCP* sp, Vec3d p, PHSolid* s0, PHSolid* s1);
	PHContactPoint(const Matrix3d& local, PHShapePairForLCP* sp, Vec3d p, PHSolid* s0, PHSolid* s1);
};

}

#endif
