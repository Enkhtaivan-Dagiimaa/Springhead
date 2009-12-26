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
const double sqEpsilon = 1e-4;
const double epsilon = 1e-8;
const double epsilon2 = epsilon*epsilon;

//----------------------------------------------------------------------------
//	CDBox
CDQuadFaces CDBox::qfaces;		// �ʁi�l�p�`:quadrangular face�j
std::vector<int> CDBox::neighbor[8];	// ���_�ׂ̗̓_

CDBox::CDBox() {
}

CDBox::CDBox(const CDBoxDesc& desc) {
	material = desc.material;
	boxsize = desc.boxsize;
	Recalc();
}
bool CDBox::IsInside(const Vec3f& p){
	Vec3f half = 0.5f * boxsize;
	return -half.x < p.x && p.x < half.x &&
		   -half.y < p.y && p.y < half.y &&
		   -half.z < p.z && p.z < half.z;
}
float CDBox::CalcVolume(){
	return boxsize.x * boxsize.y * boxsize.z;
}
Matrix3f CDBox::CalcMomentOfInertia(){
	Vec3f sz = boxsize;
	Vec3f sz2 = Vec3f(sz.x*sz.x, sz.y*sz.y, sz.z*sz.z);
	return 1.0f/12.0f * Matrix3f::Diag(sz2.y + sz2.z, sz2.x + sz2.z, sz2.x + sz2.y);
}

void CDBox::Recalc(){
	// ���[�J�����W�n�ŁAbox�̈ʒu��ݒ�
	Vec3f halfsize = Vec3f(boxsize.x/2.0, boxsize.y/2.0, boxsize.z/2.0);
	base.clear();
	base.push_back(Vec3f( halfsize.x,  halfsize.y, -halfsize.z));
	base.push_back(Vec3f( halfsize.x,  halfsize.y,  halfsize.z));
	base.push_back(Vec3f( halfsize.x, -halfsize.y,  halfsize.z));
	base.push_back(Vec3f( halfsize.x, -halfsize.y, -halfsize.z));
	base.push_back(Vec3f(-halfsize.x,  halfsize.y, -halfsize.z));
	base.push_back(Vec3f(-halfsize.x,  halfsize.y,  halfsize.z));
	base.push_back(Vec3f(-halfsize.x, -halfsize.y,  halfsize.z));
	base.push_back(Vec3f(-halfsize.x, -halfsize.y, -halfsize.z));
	//      ^z              ^z        
	//      |               |         
	//   6  |  5         2  |  1      
	//      |               |         
	//------o------>y ------+------>y 
	//      |x-             |x+       
	//   7  |  4         3  |  0      
	//      |               |         
	if (!neighbor[0].size()){
		for(int i=0; i<8; ++i) neighbor[i].resize(3);
		neighbor[0][0] = 3; neighbor[0][1] = 1; neighbor[0][2] = 4;
		neighbor[1][0] = 2; neighbor[1][1] = 0; neighbor[1][2] = 5;
		neighbor[2][0] = 1; neighbor[2][1] = 3; neighbor[2][2] = 6;
		neighbor[3][0] = 0; neighbor[3][1] = 2; neighbor[3][2] = 7;
		neighbor[4][0] = 7; neighbor[4][1] = 5; neighbor[4][2] = 0;
		neighbor[5][0] = 6; neighbor[5][1] = 4; neighbor[5][2] = 1;
		neighbor[6][0] = 5; neighbor[6][1] = 7; neighbor[6][2] = 2;
		neighbor[7][0] = 4; neighbor[7][1] = 6; neighbor[7][2] = 3;

		qfaces.clear();
		for (int nface=0; nface<6; ++nface){	// �����̂�8��
			qfaces.push_back(CDQuadFace());
		}
		// box�̊e�ʂ̖@����ݒ�Abox�̊e�ʂ̒��_�̃C���f�b�N�X��ݒ�
		qfaces[0].normal  = Vec3f(1.0, 0.0, 0.0);
		qfaces[0].vtxs[0] = 0;		
		qfaces[0].vtxs[1] = 1;
		qfaces[0].vtxs[2] = 2;
		qfaces[0].vtxs[3] = 3;
		qfaces[1].normal  = Vec3f(0.0, 1.0, 0.0);
		qfaces[1].vtxs[0] = 0;		
		qfaces[1].vtxs[1] = 4;
		qfaces[1].vtxs[2] = 5;
		qfaces[1].vtxs[3] = 1;
		qfaces[2].normal  = Vec3f(-1.0, 0.0, 0.0);
		qfaces[2].vtxs[0] = 4;		
		qfaces[2].vtxs[1] = 7;
		qfaces[2].vtxs[2] = 6;
		qfaces[2].vtxs[3] = 5;
		qfaces[3].normal  = Vec3f(0.0, -1.0, 0.0);
		qfaces[3].vtxs[0] = 3;		
		qfaces[3].vtxs[1] = 2;
		qfaces[3].vtxs[2] = 6;
		qfaces[3].vtxs[3] = 7;
		qfaces[4].normal  = Vec3f(0.0, 0.0, 1.0);
		qfaces[4].vtxs[0] = 1;		
		qfaces[4].vtxs[1] = 5;
		qfaces[4].vtxs[2] = 6;
		qfaces[4].vtxs[3] = 2;
		qfaces[5].normal  = Vec3f(0.0, 0.0, -1.0);
		qfaces[5].vtxs[0] = 0;		
		qfaces[5].vtxs[1] = 3;
		qfaces[5].vtxs[2] = 7;
		qfaces[5].vtxs[3] = 4;
	}
	curPos = 0;
}

// �T�|�[�g�|�C���g�����߂�
int CDBox::Support(Vec3f& w, const Vec3f& v) const {
	// �^����ꂽ����p�Ɉ�ԉ����_�i���ύő�̓_���T�|�[�g�|�C���g�Ƃ���j
	float d1=0.0, d2=0.0;
	for (unsigned int i=0; i<8; ++i){		// 8���_
		d1 = base[i] * v;
		if (d1 > d2) { 
			d2 = d1;
			curPos = i;
		}
	}
	w = base[curPos];
	return curPos;
}
std::vector<int>& CDBox::FindNeighbors(int vtx){
	return neighbor[vtx];
}

// �؂�������߂�. �ڐG��͂��s��.
bool CDBox::FindCutRing(CDCutRing& ring, const Posed& toW) {
	const int qfaces_size = 6;	// qfaces.size()=6�� 
	const int base_size = 8;

	Posed toL	  = toW.Inv();
	// ���_���ǂ������ɂ��邩���ׂ�
	Vec3d planePosL = toL * ring.local.Pos();
	Vec3d planeNormalL = toL.Ori() * ring.local.Ori() * Vec3d(1, 0, 0);
	int sign[base_size];
	double d = planeNormalL * planePosL;
	for (int i=0; i<base_size; ++i){
		double vtxDist = planeNormalL * base[i];
		if (vtxDist > d + sqEpsilon) sign[i] = 1;
		else if (vtxDist < d - sqEpsilon) sign[i] = -1;
		else sign[i] = 0;
	}
	bool rv = false;
	//	�܂������Ă���ʂ̏ꍇ�C��������߂�
	for(int i=0; i<qfaces_size; ++i){		// face.size()=6��
		//�@�S���_��plane�ɑ΂��ē��������ɂ���ꍇ�̓p�X
		if (sign[qfaces[i].vtxs[0]] == sign[qfaces[i].vtxs[1]] &&
			sign[qfaces[i].vtxs[0]] == sign[qfaces[i].vtxs[2]] &&
			sign[qfaces[i].vtxs[0]] == sign[qfaces[i].vtxs[3]]) continue;
		
		//	�ڐG��(plane,��1)��box�̖�(qface,��2)�̌�������߂�
		/*	�������Ƃ���1�_��������̂�
						|��2
						|n2
						|d2
				   O	|
			-------+----+----��1 n1,d1=0
						|P
			P = a*n1 + b*n2;
				a = (d1 - d2*(n1*n2)) / (1-(n1*n2)^2)
				b = (d2 - d1*(n1*n2)) / (1-(n1*n2)^2)
			����1(plane)�Ɩ�2(qface)����钼����ʂ�1�_
			O:		planePosL
			n1,d1	plane�̖@��(planeNormalL)�CO����̋���=0
			n2,d2	qface�̖@��(qfaceNormal)�CO����̋���			
		*/
		Vec3d qfaceNormal = qfaces[i].normal;
		double qfaceDist = qfaceNormal * (base[qfaces[i].vtxs[0]] - planePosL);
		Vec3d lineDirection = (planeNormalL ^ qfaceNormal).unit();
		double ip = planeNormalL * qfaceNormal;
		double l_ipip = 1-(ip*ip);
		if (l_ipip < epsilon2) continue;	//	���s�Ȗʂ͖���
		double a = -qfaceDist*ip / (1-(ip*ip));
		double b = qfaceDist / (1-(ip*ip));
		Vec3d lineOff = a*planeNormalL + b*qfaceNormal;
		Vec3d lineNormal = planeNormalL ^ lineDirection;
		double lineDist = lineNormal * lineOff;
		if (finite(lineDist)) {	
			// local -> world -> ring2�����n�ɕϊ�
			Posed to2D = ring.localInv * toW;
			Vec2d lineNormal2D = (to2D.Ori() * lineNormal).sub_vector(1, Vec2d());
			//	���͓����������������̂ŁC normal, dist �𔽓]���� ring.lines �ɒǉ�
			ring.lines.push_back(CDCutLine(-lineNormal2D, -lineDist));
			rv = true;
		} 
	}
	//bool �Փ˂̗L��
	return rv;
}

// �����̂̃T�C�Y���擾
Vec3f CDBox::GetBoxSize() {
	return boxsize;
}

Vec3f* CDBox::GetVertices(){
	return &*base.begin();
}

CDFaceIf* CDBox::GetFace(size_t i){
	return qfaces.at(i).Cast();
}

// �����̂̃T�C�Y��ݒ�
Vec3f CDBox::SetBoxSize(Vec3f boxSize){
	boxsize = boxSize;
	Recalc();
	return boxsize;
}

int CDBox::LineIntersect(const Vec3f& origin, const Vec3f& dir, Vec3f* result, float* offset){
	const float eps = 1.0e-10f;
	Vec3f p;
	int num = 0;

	for(int i = 0; i < qfaces.size(); i++){
		const CDQuadFace& f = qfaces[i];
		const Vec3f& n = f.normal;	//�ʂ̖@��
		float tmp = n * dir;		//�ʂ̖@���ƃ|�C���^�̃x�N�g���Ƃ̓���
		if(abs(tmp) < eps)	//���ς��������ꍇ�͔��肵�Ȃ�
			continue;

		float s = ((base[f.vtxs[0]] - origin) * n) / tmp; //�J�����Ɩʂ̋��� 
		if(s < 0.0)
			continue;
		p = origin + dir * s;	//�����Ɩʂ̌�_p = �J�������W�n�̌��_+�J�������W�n����ʂւ̃x�N�g��*���� (Shape���W�n)

		// 4�p�`�̓����ɂ��邩
		Vec3d b =GetBoxSize()*0.5; //CDBox��x,y,z���ɑ΂���ő�l
		if(-b.x<=p.x && p.x <=b.x && -b.y<=p.y && p.y <=b.y && -b.z<=p.z && p.z <=b.z ){
			result[num] = p;
			offset[num] = s;
			num++;
		}
		if(num == 2)		// �������3�ȏ�͂��蓾�Ȃ����O�̂���
			break;
	}
	return num;
}

}	//	namespace Spr
