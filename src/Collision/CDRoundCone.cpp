/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
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
//	CDRoundCone
CDRoundCone::CDRoundCone() {
}

CDRoundCone::CDRoundCone(const CDRoundConeDesc& desc){
	radius = desc.radius;
	length = desc.length;
	material = desc.material;
}

bool CDRoundCone::IsInside(const Vec3f& p){
	return (p - Vec3f(0.0f, 0.0f, 0.5f * -length)).square() < radius[0] * radius[0] ||
		   (p - Vec3f(0.0f, 0.0f, 0.5f * +length)).square() < radius[1] * radius[1] ||
		   (p.x*p.x + p.y*p.y < radius * radius && -0.5f*length < p.z && p.z < 0.5f*length);
}
float CDRoundCone::CalcVolume(){
	return  2.0f/3.0f * (float)M_PI * radius[0] * radius[0] * radius[0] +
			2.0f/3.0f * (float)M_PI * radius[1] * radius[1] * radius[1] +
			(float)M_PI * (radius[0]*radius[0] + radius[0]*radius[1] + radius[1]*radius[1]) * length / 3.0;
}
	
// �T�|�[�g�|�C���g�����߂�
Vec3f CDRoundCone::Support(const Vec3f& p) const {
	// Z���O����radius[0]�A�����radius[1]
	float normal_Z = (radius[1] - radius[0]) / length;
	Vec3f dir;
	if (p.norm()!=0) {
		dir = p / p.norm();
	} else {
		dir = Vec3f(0,0,0);
	}

	if (-1.0 < normal_Z && normal_Z < 1.0) {
		if (normal_Z < dir.Z()) {
			// p�̕���Z���O�� �� radius[0]���g�p
			return dir*radius[0] + Vec3f(0,0, length/2.0);
		} else {
			// p�̕���Z����� �� radius[1]���g�p
			return dir*radius[1] + Vec3f(0,0,-length/2.0);
		}
	} else {
		// �ǂ��炩�̋��ɕ�܂���Ă���
		if (radius[1] < radius[0]) {
			return dir*radius[0] + Vec3f(0,0, length/2.0);
		} else {
			return dir*radius[1] + Vec3f(0,0,-length/2.0);
		}
	}
}

// �؂�������߂�. �ڐG��͂��s��.
bool CDRoundCone::FindCutRing(CDCutRing& ring, const Posed& toW) {
	//	�؂��(ring.local)�n�ł� �J�v�Z���̌���
	Vec3f dir = ring.localInv.Ori() * toW.Ori() * Vec3f(0,0,1);
	std::cout << dir << std::endl;
	float normal_Z = (radius[1] - radius[0]) / length;
	if (-1.0 < normal_Z && normal_Z < 1.0) {
		float d = abs(-dir.X() - normal_Z);
		if (d < 0.3f) { // ���ʂ��ڐG�ʂɂقڕ��s�ȏꍇ
			float shrink = sqrt(1-d*d);	//	�X���Ă��邽�߂ɋ������k�ފ���
			// float l_lat  = length * cos( Rad(90) - acos(normal_Z) );
			float l_lat  = length;
			float start  = -0.5f*l_lat*shrink;
			float end    =  0.5f*l_lat*shrink;
			
			if (d > 1e-4) { // ���S�ȕ��s�ł͂Ȃ��ꍇ
				// ������
			}

			//	ring�ɐ�����ǉ�
			ring.lines.push_back(CDCutLine(Vec2f(-dir.Y(), -dir.Z()), -start));
			ring.lines.push_back(CDCutLine(Vec2f(dir.Y(), dir.Z()), end));
			ring.lines.push_back(CDCutLine(Vec2f(dir.Z(), -dir.Y()), 0));
			ring.lines.push_back(CDCutLine(Vec2f(-dir.Z(), dir.Y()), 0));
			return true;
		} else {
			return false;
		}

	} else {
		// �ǂ��炩����܂���Ăɋ��̂ɂȂ��Ă���
		return false;
	}
}

Vec3d CDRoundCone::Normal(Vec3d p){
	Vec2d	r = radius;
	double	l = length;

	// RoundCone�̑��ʂ̊p�x�i���ʂ�Z���ɐ����ȂƂ�0���A���s(�܂�J�v�Z���^)�̂Ƃ�90���j
	double theta = acos((r[1]-r[0])/l);

	if ( (p[2] > (r[0]*cos(theta) + l/2.0)) || (p[2] < (r[1]*cos(theta) - l/2.0)) ) {
		// �ڐG�_���ǂ��炩�̋��̂ɂ���ꍇ�F
		return p.unit();
	} else {
		// �ڐG�_�����̂Ƌ��̂̊Ԃɂ���ꍇ�F
		Vec3d pNormal = p;
		pNormal[2] = 0; pNormal = pNormal.unit() * sin(theta);
		pNormal[2] = cos(theta);
		return pNormal;
	}
}

double CDRoundCone::CurvatureRadius(Vec3d p){
	/// ���L�̌v�Z�͌����_�Ńo�O�����B
	/// RoundCone�̗��[�̔��a�������ł���Ƃ���IND�ɂȂ�
	/// ������C�����邱�ƁI ('09/02/17, mitake)

	Vec2d	r = radius;
	double	l = length;

	// RoundCone�̑��ʂ̊p�x�i���ʂ�Z���ɐ����ȂƂ�0���A���s(�܂�J�v�Z���^)�̂Ƃ�90���j
	double theta = acos((r[1]-r[0])/l);
	// �ڐG�_��Z���W
	double Zc = p[2];

	/*
	�ڐG�_���ǂ��炩�̋��̂ɂ���ꍇ�F
	*/
	if (Zc > (r[0]*cos(theta) + l/2.0)) {
		return r[0];
	} else if (Zc < (r[1]*cos(theta) - l/2.0)) {
		return r[1];
	}

	/*
	�ڐG�_�����̂Ƌ��̂̊Ԃɂ���ꍇ�F
	�ڐG�_��ʂ�A�ڐG�_�̈ʒu�ő��ʂƒ�������f�ʂɂ�����RoundCone�̐ؒf���l����B
	���̒f�ʂ͑ȉ~�ƂȂ�A���̑ȉ~�̍ŏ��ȗ����a�i�����̒[�ɂ�����ȗ����a�j�����߂���̂ƂȂ�B
	*/

	/// -- RoundCone�����������~���̒��_��Z���W
	double Z0 = l/2*(r[1]+r[0])/(r[1]-r[0]);

	/// -- Z-R���W�n�Ō����Ƃ��̑��ʂ̌X��
	double M1 = tan(Rad(90) - theta);
	/// -- Z-R���W�n�Ō����Ƃ��̒f�ʂ̌X��
	double M2 = tan(theta);

	/// -- �ؒf�ʂ̌��������̓_
	double Za = (M1*Z0-M2*Zc)/(M1-M2);
	Vec2d pA = Vec2d(Za, -M1*Za + M1*Z0);
	/// -- �ؒf�ʂ̂��������̓_
	double Zb = (M1*Z0+M2*Zc)/(M1+M2);
	Vec2d pB = Vec2d(Zb,  M1*Zb - M1*Z0);
	/// -- RoundCone�����������~���̒��_
	Vec2d p0 = Vec2d(Z0, 0);

	/// -- �ؒf�ȉ~�̑傫�����l���邽�߂̎O�p�`�̎O��
	double Ta = (pB - p0).norm(), Tb = (pA - p0).norm(), Tc = (pA - pB).norm();
	/// -- �ؒf�ȉ~�̒������a
	double eL = Tc/2;
	/// -- �ؒf�ȉ~�̒��S����œ_�܂ł̋����i�������a����Ta,Tb,Tc����Ȃ�O�p�`�̓��ډ~���a�����������̂ɂȂ�j
	double f = eL - (Ta*Tc)/(Ta+Tb+Tc);
	/// -- �ؒf�ȉ~�̒Z�����a
	double eS = sqrt(eL*eL - f*f);
	/// -- �ؒf�ȉ~�̍ŏ��ȗ����a
	double Rmin = eS*eS / eL;

	return Rmin;
}

Vec2f CDRoundCone::GetRadius() {
	return radius;
}
float CDRoundCone::GetLength() {
	return length;
}
void CDRoundCone::SetRadius(Vec2f r) {
	radius=r;
}
void CDRoundCone::SetLength(float l) {
	length=l;
}
void CDRoundCone::SetWidth(Vec2f r) {
	Vec2f radiusMargin=r-radius;
	
	length-=(radiusMargin.x+radiusMargin.y);
	radius=r;
}

}	//	namespace Spr
