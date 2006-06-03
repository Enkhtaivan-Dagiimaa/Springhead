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
	radius = desc.radius;
}

// �T�|�[�g�|�C���g�����߂�
Vec3f CDSphere::Support(const Vec3f& p) const {	
	float n = p.norm();
	if(n < 1.0e-10f)
		return Vec3f();
	else return (radius / n) * p;
	/*Vec3f center = GetPose().Pos();		// ���̂̒��S���W���擾
	Vec3f p_ = p;
	p_ -= center;						// p����center�ւ̃x�N�g�������߂�
	float s = p_.norm();				// p����center�ւ̃x�N�g���̒���
	if (s > 1e-8f) {		
		float r = radius / s;	
		return r*p_ + center;			// �Փˌ��o�ɕK�v�ȋ��̍ŋߖT�_
	}else{
		return center;
	}*/
}

// �؂�������߂�. �ڐG��͂��s��.
bool CDSphere::FindCutRing(CDCutRing& r, const Posed& toW) {
	assert(0);
	return false;
}

// ���̂̔��a���擾
float CDSphere::GetRadius() {
	return radius;
}

}	//	namespace Spr
