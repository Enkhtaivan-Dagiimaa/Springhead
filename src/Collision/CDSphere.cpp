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
//	CDSphere
OBJECTIMP(CDSphere, CDConvex);	
IF_IMP(CDSphere, CDConvex);	
	
CDSphere::CDSphere() {
}

CDSphere::CDSphere(const CDSphereDesc& desc) {
	center = desc.center;
	radius = desc.radius;
}

// �T�|�[�g�|�C���g�����߂�
Vec3f CDSphere::Support(const Vec3f& p) const {	
	Vec3f p_ = p;
	p_ -= center;					// center����p�ւ̃x�N�g�������߂�
	float s = p_.norm();			// center����p�ւ̃x�N�g����length
	if (s > 1e-8f) {		
		float r = radius / s;	
		return r*p_ + center;		// �Փˌ��o�ɕK�v�ȋ��̍ŋߖT�_
	}else{
		return center;
	}	
}

// �؂�������߂�. �ڐG��͂��s��.
void CDSphere::FindCutRing(CDCutRing& r, const Posed& toW) {
	assert(0);	
}

// ���̂̒��S���W���擾
Vec3f CDSphere::GetCenter() {
	return center;
}	

// ���̂̔��a���擾
float CDSphere::GetRadius() {
	return radius;
}

}	//	namespace Spr
