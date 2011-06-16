/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include <Collision/CDSphere.h>

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
int CDSphere::Support(Vec3f&w, const Vec3f& v) const{
	float n = v.norm();
	if(n < 1.0e-10f){
		w = Vec3f();
		return -1;
	}
	w = (radius / n) * v;
	return -1;
}

// �؂�������߂�. �ڐG��͂��s��.
bool CDSphere::FindCutRing(CDCutRing& r, const Posed& toW) {
	return false;
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

int CDSphere::LineIntersect(const Vec3f& origin, const Vec3f& dir, Vec3f* result, float* offset){
	const float eps = 1.0e-10f;
	Vec3f p;
	int num = 0;

	const Vec3f n = origin;	//�ʂ̖@�� = �J������shape�̌��_�����ԃx�N�g��
	float tmp = n * dir;	//�ʂ̖@���ƃ|�C���^�̃x�N�g���Ƃ̓���
	if(abs(tmp) < eps)	//���ς��������ꍇ�͔��肵�Ȃ�
		return num;

	float s = ((Vec3f(0.0,0.0,0.0) - origin) * n) / tmp; //�J�����Ɩʂ̋��� 
	if(s < 0.0)
		return num;
	p = origin + dir * s;	//�����Ɩʂ̌�_p = �J�������W�n�̌��_+�J�������W�n����ʂւ̃x�N�g��*���� (Shape���W�n)

	// �~�̓����ɂ��邩
	if(p.norm()<GetRadius()){
		//result[num] = p;
		result[num] = Vec3f(0.0,0.0,0.0); //Sphere�͒��S�ʒu�ɃW���C���g��ڑ����������g���₷����
		offset[num] = s;
		num++;
	}
	return num;
}

}	//	namespace Spr
