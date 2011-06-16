/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include <Collision/CDCapsule.h>

namespace Spr{;

//----------------------------------------------------------------------------
//	CDCapsule
CDCapsule::CDCapsule(const CDCapsuleDesc& desc){
	material	= desc.material;
	radius		= desc.radius;
	length		= desc.length;
}

bool CDCapsule::IsInside(const Vec3f& p){
	return (p - Vec3f(0.0f, 0.0f, 0.5f * -length)).square() < radius * radius ||
		   (p - Vec3f(0.0f, 0.0f, 0.5f * +length)).square() < radius * radius ||
		   (p.x*p.x + p.y*p.y < radius * radius && -0.5f*length < p.z && p.z < 0.5f*length);
}
float CDCapsule::CalcVolume(){
	return  4.0f/3.0f * (float)M_PI * radius * radius * radius + 
			(float)M_PI * radius * radius * length;
}
	
// �T�|�[�g�|�C���g�����߂�
int CDCapsule::Support(Vec3f& w, const Vec3f& v) const {
	float n = v.norm();
	if(n < 1.0e-10f){
		w = Vec3f();
	}else{
		w = (radius / n) * v;
	}
	if (v.Z() >= 0){
		w.Z() += length*0.5f;
	}else{
		w.Z() -= length*0.5f;
	}
	return -1;
}


// �؂�������߂�. �ڐG��͂��s��.
bool CDCapsule::FindCutRing(CDCutRing& ring, const Posed& toW) {
	//	�؂��(ring.local)�n�ł� �J�v�Z���̌���
	Vec3f dir = ring.localInv.Ori() * toW.Ori() * Vec3f(0,0,1);
	Vec3f center = ring.localInv * toW.Pos();
	float sign = center.X() > 0.0f ? 1.0f : -1.0f;
	if (dir.X()*sign < 0) dir = -dir;
	center -= sign*dir * length/2;
	
	if (dir.X()*sign < 0.3f){		//	�J�v�Z�����ڐG�ʂɑ�̕��s�ȏꍇ
		float shrink = sqrt(1-dir.X()*dir.X());	//	�X���Ă��邽�߂ɋ������k�ފ���
		float start = -0.0f*length*shrink;
		float end = 1.0f*length*shrink;
		if (dir.X()*sign > 1e-4){	//	���S�ɕ��s�łȂ��ꍇ
			float is = (radius/shrink-sign*center.X()) / (sign*dir.X()) * shrink;	//	�ڐG�ʂƒ��S���𔼌a���炵�����Ƃ̌�_
			if (is < end) end = is;

			if (end+1e-4 < start){//0.001 < start){
				DSTR << "CDCapsule::FindCutRing() may have a problem" << std::endl;
			}
//			assert(end + 0.001 >= start);

			if (end <= start) return false;
		}

		//	ring�ɐ�����ǉ�
		float lenInv = 1/sqrt(dir.Y()*dir.Y() + dir.Z()*dir.Z());
		ring.lines.push_back(CDCutLine(Vec2f(-dir.Y(), -dir.Z())*lenInv, -start));
		ring.lines.push_back(CDCutLine(Vec2f(dir.Y(), dir.Z())*lenInv, end));
		ring.lines.push_back(CDCutLine(Vec2f(dir.Z(), -dir.Y())*lenInv, 0));
		ring.lines.push_back(CDCutLine(Vec2f(-dir.Z(), dir.Y())*lenInv, 0));
		return true;
	}else{
		return false;
	}
}

Matrix3f CDCapsule::CalcMomentOfInertia(){
	Matrix3f ans;
	
	// http://www12.plala.or.jp/ksp/mechanics/inertiaTable1/
	// http://www.dynamictouch.matrix.jp/tensormodel.php
	ans[0][0] = ((radius * radius)/4.0f + (length*length)/12.0f + 2.0f * 83.0f/320.0f * radius * radius) + length * length / 2.0f;
	ans[0][1] = 0.0f;
	ans[0][2] = 0.0f;
	ans[1][0] = 0.0f;
	ans[1][1] = ((radius * radius)/4.0f + (length*length)/12.0f+ 2.0f * 83.0f/320.0f * radius * radius) + length * length / 2.0f;
	ans[1][2] = 0.0f;
	ans[2][0] = 0.0f;
	ans[2][1] = 0.0f;
	ans[2][2] = (9.0f/5.0f * radius * radius);

	return ans;
}

int CDCapsule::LineIntersect(const Vec3f& origin, const Vec3f& dir, Vec3f* result, float* offset){
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
		if(po.norm()<GetRadius()){
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
		// �~���̓����ɂ��邩
		if(pr.norm()<GetRadius() && abs(p.z)<=GetLength()*0.5){
			result[num] = p;
			offset[num] = s;
			num++;
		}
	}
	return num;
}

}	//	namespace Spr
