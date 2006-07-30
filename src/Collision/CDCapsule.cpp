/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include <Springhead.h>
#include <Foundation/Object.h>
#include "Collision.h"
#include <set>
#ifdef USE_HDRSTOP
#pragma hdrstop
#endif
#include "CDQuickHull3DImp.h"

namespace Spr{;
	
//----------------------------------------------------------------------------
//	CDCapsule
IF_OBJECT_IMP(CDCapsule, CDConvex);	
	
CDCapsule::CDCapsule() {
}

CDCapsule::CDCapsule(const CDCapsuleDesc& desc) {
	radius = desc.radius;
}

// �T�|�[�g�|�C���g�����߂�
Vec3f CDCapsule::Support(const Vec3f& p) const {	
	float n = p.norm();
	Vec3f off;
	if(n < 1.0e-10f){
		off = Vec3f();
	}else{
		off = (radius / n) * p;
	}
	if (p.Z() > 0){
		off.Z() += length;
	}else if (p.Z() < 0){
		off.Z() -= length;
	}
	return off;
}

// �؂�������߂�. �ڐG��͂��s��.
bool CDCapsule::FindCutRing(CDCutRing& ring, const Posed& toW) {
	//	�؂��(ring.local)�n�ł� �J�v�Z���̌���
	Vec3f dir = ring.localInv.Ori() * toW.Ori() * Vec3f(0,0,1);
	if (abs(dir.X()) <= 1e-10f){	//	�J�v�Z�����@��(1,0,0)�Ɛ����ȏꍇ
		//	ring�ɐ�����ǉ�
		ring.lines.push_back(CDCutLine(Vec2f(dir.Y(), dir.Z()), length/2));
		ring.lines.push_back(CDCutLine(Vec2f(-dir.Y(), -dir.Z()), length/2));
		ring.lines.push_back(CDCutLine(Vec2f(dir.Y(), -dir.Z()), 0));
		ring.lines.push_back(CDCutLine(Vec2f(dir.Y(), -dir.Z()), 0));
		return true;
	}else{
		return false;
	}
}

// ���̂̔��a���擾
float CDCapsule::GetRadius() {
	return radius;
}

}	//	namespace Spr
