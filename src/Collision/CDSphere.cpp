/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include <Springhead.h>
#include <Foundation/Scene.h>
#include "Collision.h"
#include <set>
#ifdef USE_HDRSTOP
#pragma hdrstop
#endif
#include "CDQuickHull3DImp.h"

namespace Spr{;
	
//----------------------------------------------------------------------------
//	CDSphere
	
CDSphere::CDSphere() {
}

CDSphere::CDSphere(const CDSphereDesc& d){
	radius = d.radius;
	material = d.material;
	
}

// �T�|�[�g�|�C���g�����߂�
Vec3f CDSphere::Support(const Vec3f& p) const {	
	float n = p.norm();
	if(n < 1.0e-10f)
		return Vec3f();
	else return (radius / n) * p;
	/*
	Vec3f center = GetPose().Pos();		// ���̂̒��S���W���擾
	Vec3f p_ = p;
	p_ -= center;						// p����center�ւ̃x�N�g�������߂�
	float s = p_.norm();				// p����center�ւ̃x�N�g���̒���
	if (s > 1e-8f) {		
		float r = radius / s;	
		return r*p_ + center;			// �Փˌ��o�ɕK�v�ȋ��̍ŋߖT�_
	}else{
		return center;
	}
	*/
}

// �؂�������߂�. �ڐG��͂��s��.
bool CDSphere::FindCutRing(CDCutRing& r, const Posed& toW) {
	return false;
}

// ���̂̔��a���擾
float CDSphere::GetRadius() {
	return radius;
}
bool CDSphere::IsInside(const Vec3f& p){
	return p.square() < radius * radius;
}
float CDSphere::CalcVolume(){
	return float(4.0/3.0*M_PI*radius*radius*radius);
}
Matrix3f CDSphere::CalcMomentOfInertia(){
	return 0.4f * radius * radius * Matrix3f::Unit();
}

}	//	namespace Spr
