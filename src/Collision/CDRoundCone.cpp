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
#pragma hdrstop
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
			(float)M_PI * (radius[0]*radius[0] + radius[0]*radius[1] + radius[1]*radius[1]) * length / 3.0f;
}
	
// �T�|�[�g�|�C���g�����߂�
int CDRoundCone::Support(Vec3f&w, const Vec3f& v) const{
	float normal_Z = (radius[0] - radius[1]) / length;
	float n = v.norm();
	Vec3f dir;
	if(n < 1.0e-10f){
		dir = Vec3f();
	}else{
		dir = v / n;
	}

	if (-1 < normal_Z && normal_Z < 1) {
		if (normal_Z < dir.Z()) {
			// v�̕���Z���O�� �� radius[1]���g�p
			w = dir*radius[1] + Vec3f(0,0, length/2.0);
			return 1;
		} else {
			// v�̕���Z����� �� radius[0]���g�p
			w = dir*radius[0] + Vec3f(0,0,-length/2.0);
			return 0;
		}
	} else {
		// �ǂ��炩�̋��ɕ�܂���Ă���
		if (radius[0] < radius[1]) {
			w = dir*radius[1] + Vec3f(0,0, length/2.0);
		} else {
			w =  dir*radius[0] + Vec3f(0,0,-length/2.0);
		}
		return -1;
	}
}

// �؂�������߂�. �ڐG��͂��s��.
bool CDRoundCone::FindCutRing(CDCutRing& ring, const Posed& toW) {
	//	�؂��(ring.local)�n�ł� �J�v�Z���̌���
	Vec3f dir = ring.localInv.Ori() * toW.Ori() * Vec3f(0,0,1);
	Vec3f center = ring.localInv * toW.Pos();
	float sign = center.X() > 0.0f ? 1.0f : -1.0f;

	//	sinA : (r1-r0)/length �ɂȂ�B
	//	sinB : Cutring�ʂƉ~���ʂ̐��Ƃ̂Ȃ��p�� B
	//	sinA+B = dir.X() �ɂȂ�B
	float sinA = (radius[1]-radius[0]) / length;
	if (-1 > (-sinA) || (-sinA) > 1) { return false; } // ���̂ɂȂ��Ă���
	float sinB = dir.X()*sign * sqrt(1-sinA*sinA)  -  sqrt(1-dir.X()*dir.X()) * sinA;

	float r = radius[0];
	if (sinB < 0){
		dir = -dir;
		sinA *= -1;
		sinB *= -1;
		r = radius[1];
	}
	center = center - (length/2) * dir;
	if (sinB < 0.3f) { // ���ʂ��ڐG�ʂɂقڕ��s�ȏꍇ
		float shrink = sqrt(1-dir.X()*dir.X());	//	�X���Ă��邽�߂ɋ������k�ފ���
		float start = -0.0f*length*shrink;
		float end = 1.0f*length*shrink;

		if (sinB > 1e-4){	//	���S�ɕ��s�łȂ��ꍇ
			float depth = r/shrink - sign*center.X();
			float cosB = sqrt(1-sinB*sinB);
			float is = depth / sinB * cosB;	//	�ڐG�ʂƒ��S���𔼌a���炵�����Ƃ̌�_
			if (is < end) end = is;
			if (end+1e-4 < start){//0.001 < start){
				DSTR << "CDRoundCone::FindCutRing() may have a problem" << std::endl;
			}
			if (end <= start) return false;
		}
		//	ring�ɐ�����ǉ�
		float lenInv = 1/sqrt(dir.Y()*dir.Y() + dir.Z()*dir.Z());
		ring.lines.push_back(CDCutLine(Vec2f(-dir.Y(), -dir.Z())*lenInv, -start));
		ring.lines.push_back(CDCutLine(Vec2f(dir.Y(), dir.Z())*lenInv, end));
		ring.lines.push_back(CDCutLine(Vec2f(dir.Z(), -dir.Y())*lenInv, 0));
		ring.lines.push_back(CDCutLine(Vec2f(-dir.Z(), dir.Y())*lenInv, 0));
		return true;
	}
	return false;
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

Matrix3f CDRoundCone::CalcMomentOfInertia(){
	Matrix3f ans;
	//�~����̕����͉~���ߎ�
	// http://www12.plala.or.jp/ksp/mechanics/inertiaTable1/
	// http://www.dynamictouch.matrix.jp/tensormodel.php

	float r = (radius[0] + radius[1]) * 0.5f;	 //�~���̔��a
	
	ans[0][0] = ((r * r)/4.0f + (length*length)/12.0f + 83.0f/320.0f * ( radius[0] * radius[0] +  radius[1] * radius[1]))+ length * length / 2.0f; 
	ans[0][1] = 0.0f;
	ans[0][2] = 0.0f;
	ans[1][0] = 0.0f;
	ans[1][1] = ((r * r)/4.0f + (length*length)/12.0f + 83.0f/320.0f * ( radius[0] * radius[0] +  radius[1] * radius[1]))+ length * length / 2.0f;
	ans[1][2] = 0.0f;
	ans[2][0] = 0.0f;
	ans[2][1] = 0.0f;
	ans[2][2] = (9.0f/5.0f * r * r);

	return ans;
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

int CDRoundCone::LineIntersect(const Vec3f& origin, const Vec3f& dir, Vec3f* result, float* offset){
	const float eps = 1.0e-10f;
	Vec3f p;
	int num = 0;
	Vec3f sCenter[2];	//���̒��S�ʒu
	sCenter[0] = Vec3f(0.0f,0.0f,length);
	sCenter[1] = Vec3f(0.0f,0.0f,-length);

	//�������̔���
	for(int i=0; i<2; i++){
		const Vec3f n = sCenter[i] - origin;		 //�ʂ̖@�� = �J�����Ƌ��̌��_�����ԃx�N�g��
		float tmp = n * dir;						 //�ʂ̖@���ƃ|�C���^�̃x�N�g���Ƃ̓���
		if(abs(tmp) < eps)							 //���ς��������ꍇ�͔��肵�Ȃ�
			continue;
		float s = ((sCenter[i] - origin) * n) / tmp; //�J�����Ɩʂ̋��� 
		if(s < 0.0)
			continue;
		p = origin + dir * s;						 //�����Ɩʂ̌�_p = �J�������W�n�̌��_+�J�������W�n����ʂւ̃x�N�g��*���� (Shape���W�n)
		Vec3f po = p-sCenter[i];					 //���̒��S�����_�Ƃ������̌�_�̈ʒu
		// �~�̓����ɂ��邩
		if(po.norm()<GetRadius()[i]){
			result[num] = p;
			offset[num] = s;
			num++;
		}
	}
	//�~�������̔���
	const Vec3f n =  origin;						 //�J���������ւ̐����Ȓf�ʂ̖@�� = �J������shape�̌��_�����ԃx�N�g��
	float tmp = n * dir;							 //�ʂ̖@���ƃ|�C���^�̃x�N�g���Ƃ̓���

	for(int i=0; i<1; i++){
		if(abs(tmp) < eps)							 //���ς��������ꍇ�͔��肵�Ȃ�
			continue;
		float s = ((- origin) * n) / tmp;			 //�J�����Ɩʂ̋��� 
		if(s < 0.0)
			continue;
		p = origin + dir * s;						 //�����Ɩʂ̌�_p = �J�������W�n�̌��_+�J�������W�n����ʂւ̃x�N�g��*���� (Shape���W�n)
		
		Vec2f pr = Vec2f(p.x,p.y);					 //xy���ʂ�p
		Vec2f r = GetRadius();
		if(r[0]<r[1]) r[0] = r[1]; 
		// �~���̓����ɂ��邩
		if(pr.norm()<r[0] && abs(p.z)<=GetLength()*0.5){
			result[num] = p;
			offset[num] = s;
			num++;
		}
		/*����
		���݂͉~���Ŕ�����s���Ă��邪�A�{���͉~���̓����ɂ��邩���肷��K�v������B
		�C�����K�v
		*/
	}
	return num;
}

}	//	namespace Spr
