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

CDRoundCone::CDRoundCone(const CDRoundConeDesc& desc):CDRoundConeDesc(desc){
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
	// std::cout << Deg(acos(normal_Z)) << std::endl;
	Vec3f dir;
	if (p.norm()!=0) {
		dir = p / p.norm();
	} else {
		dir = Vec3f(1,0,0);
	}
	// std::cout << normal_Z << ", " << dir.Z() << std::endl;

	if (-M_PI/2.0 < normal_Z && normal_Z < M_PI/2.0) {
		if (normal_Z < dir.Z()) {
			// p�̕���Z���O�� �� radius[0]���g�p
			// std::cout << "[0] : " << dir*radius[0] + Vec3f(0,0, length/2.0) << std::endl;
			return dir*radius[0] + Vec3f(0,0, length/2.0);
		} else {
			// p�̕���Z����� �� radius[1]���g�p
			// std::cout << "[1] : " << dir*radius[1] + Vec3f(0,0,-length/2.0) << std::endl;
			return dir*radius[1] + Vec3f(0,0,-length/2.0);
		}
	} else {
		// �ǂ��炩�̋��ɕ�܂���Ă���
		if (radius[1] < radius[0]) {
			// std::cout << "[0]* : " << dir*radius[0] + Vec3f(0,0, length/2.0) << std::endl;
			return dir*radius[0] + Vec3f(0,0, length/2.0);
		} else {
			// std::cout << "[1]* : " << dir*radius[1] + Vec3f(0,0,-length/2.0) << std::endl;
			return dir*radius[1] + Vec3f(0,0,-length/2.0);
		}
	}

	/*
	// std::cout << "p : " << p << std::endl;
	float dr = (radius[1]-radius[0]);
	Vec3d d = Vec3d(length,0,dr); d = d / d.norm();
	float n = p.norm();
	Vec3d q;
	Vec3f off;
	if(n < 1.0e-10f){
		off = Vec3f();
		q = Vec3f();
	}else{
		off = n * p;
		q = p / n;
	}
	// std::cout << "q : " << q << std::endl;
	// std::cout << "d : " << d << std::endl;
	if (q.Z() >= d.Z()){
		off *= radius[0];
		off.Z() += length*0.5f;
		// std::cout << "r[0] : " << radius[0] << std::endl;
	}else{
		off *= radius[1];
		off.Z() -= length*0.5f;
		// std::cout << "r[1] : " << radius[1] << std::endl;
	}
	// std::cout << "off : " << off << std::endl;
	return off;
	*/
}


// �؂�������߂�. �ڐG��͂��s��.
bool CDRoundCone::FindCutRing(CDCutRing& ring, const Posed& toW) {
	//	�؂��(ring.local)�n�ł� �J�v�Z���̌���
	Vec3f dir = ring.localInv.Ori() * toW.Ori() * Vec3f(0,0,1);
	float normal_Z = (radius[1] - radius[0]) / length;
	if (M_PI/2.0 < normal_Z && normal_Z < M_PI/2.0) {
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

	/*
	//	�؂��(ring.local)�n�ł� �J�v�Z���̌���
	Vec3f dir = ring.localInv.Ori() * toW.Ori() * Vec3f(0,0,1);
	if (dir.X() < 0) dir = -dir;
	
	if (dir.X() < 0.3f){		//	�J�v�Z�����ڐG�ʂɑ�̕��s�ȏꍇ
		float shrink = sqrt(1-dir.X()*dir.X());	//	�X���Ă��邽�߂ɋ������k�ފ���
		float start = -0.5f*length*shrink;
		float end = 0.5f*length*shrink;
		if (dir.X() > 1e-4){	//	���S�ɕ��s�łȂ��ꍇ
			Vec3f center = ring.localInv * toW.Pos();
			float is = -(center.X()-radius[0]/shrink) / dir.X() * shrink;	//	�ڐG�ʂƒ��S���𔼌a���炵�����Ƃ̌�_
			if (is < end) end = is;

			if (end+0.001 < start){
				DSTR << "CDRoundCone::FindCutRing() may have a problem" << std::endl;
			}
//			assert(end + 0.001 >= start);

			if (end <= start) return false;
		}

		//	ring�ɐ�����ǉ�
		ring.lines.push_back(CDCutLine(Vec2f(-dir.Y(), -dir.Z()), -start));
		ring.lines.push_back(CDCutLine(Vec2f(dir.Y(), dir.Z()), end));
		ring.lines.push_back(CDCutLine(Vec2f(dir.Z(), -dir.Y()), 0));
		ring.lines.push_back(CDCutLine(Vec2f(-dir.Z(), dir.Y()), 0));
		return true;
	}else{
		return false;
	}
	*/
}

Vec2f CDRoundCone::GetRadius() {
	return radius;
}
float CDRoundCone::GetLength() {
	return length;
}

}	//	namespace Spr
