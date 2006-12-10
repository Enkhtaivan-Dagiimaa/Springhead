/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PHSHAPE_H
#define PHSHAPE_H

#include <SprCollision.h>
#include <Foundation/Object.h>
#include "IfStubCollision.h"

namespace Spr{;

/// �`��
/// �`�󎩑͈̂ʒu���������Ȃ��D�ʒu���͌`����Q�Ƃ���e���̂����D
class CDShape : public NamedObject, CDShapeIfInit{
public:
	PHMaterial material;

	OBJECTDEF_ABST(CDShape, NamedObject);
	//Posef GetPose() const { return pose; }
	//void SetPose(const Posef& p){ pose = p; }
	
	virtual void CalcBBox(Vec3f& bbmin, Vec3f& bbmax, const Posed& pose)=0;
};

/// �`��ւ̎Q�Ƃƍ��̂ɑ΂��鑊�Έʒu
struct CDShapeRefWithPose{
	CDShape* shape;
	Posed pose;
};

}	//	namespace Spr
#endif
