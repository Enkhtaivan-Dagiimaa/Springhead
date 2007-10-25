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
const double sqEpsilon = 1e-4;
const double epsilon = 1e-8;
const double epsilon2 = epsilon*epsilon;

//----------------------------------------------------------------------------
//	CDBox
IF_OBJECT_IMP(CDBox, CDConvex);	

CDBox::CDBox() {
}

CDBox::CDBox(const CDBoxDesc& desc) {
	material = desc.material;

	boxsize = desc.boxsize;
	// ���[�J�����W�n�ŁAbox�̈ʒu��ݒ�
	Vec3f halfsize = Vec3f(boxsize.x/2.0, boxsize.y/2.0, boxsize.z/2.0);
	base.push_back(Vec3f( halfsize.x,  halfsize.y, -halfsize.z));
	base.push_back(Vec3f( halfsize.x,  halfsize.y,  halfsize.z));
	base.push_back(Vec3f( halfsize.x, -halfsize.y,  halfsize.z));
	base.push_back(Vec3f( halfsize.x, -halfsize.y, -halfsize.z));
	base.push_back(Vec3f(-halfsize.x,  halfsize.y, -halfsize.z));
	base.push_back(Vec3f(-halfsize.x,  halfsize.y,  halfsize.z));
	base.push_back(Vec3f(-halfsize.x, -halfsize.y,  halfsize.z));
	base.push_back(Vec3f(-halfsize.x, -halfsize.y, -halfsize.z));

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

// �T�|�[�g�|�C���g�����߂�
Vec3f CDBox::Support(const Vec3f& p) const {
	// �^����ꂽ����p�Ɉ�ԉ����_�i���ύő�̓_���T�|�[�g�|�C���g�Ƃ���j
	float d1=0.0, d2=0.0;
	for (unsigned int i=0; i<8; ++i){		// 8���_
		d1 = base[i] * p;
		if (d1 > d2) { 
			d2 = d1;
			curPos = i;
		}
	}
	return base[curPos];
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
//		if ((ip < 1.0-epsilon2) && (ip > -1.0+epsilon2)){	//	���s�Ȗʂ͖���
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
/*		}else{
			DSTR << "parallel plane" << std::endl;
			DSTR << "ip:" << ip << std::endl;
			DSTR << "pn:" << planeNormalL<<std::endl;
			DSTR << "fn:" << qfaceNormal<<std::endl;
			DSTR << "vtx:";
			for(int j=0; j<4; ++j){
				int id = qfaces[i].vtxs[j];
				double vtxDist = planeNormalL * base[id];
				DSTR << sign[id] << " " << vtxDist-d << " " << base[id];
			}
			DSTR << std::endl;
			exit(0);
		}
*/	}
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
	return &qfaces.at(i);
}

}	//	namespace Spr
