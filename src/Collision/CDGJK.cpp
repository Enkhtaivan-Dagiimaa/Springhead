/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include "Collision.h"
#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

namespace Spr{;
double relError = 1e-6;
double absError = 1e-10;


static Vec3d p[4];			// A�̃T�|�[�g�|�C���g(���[�J���n)
static Vec3d q[4];			// B�̃T�|�[�g�|�C���g(���[�J���n)
static Vec3d p_q[4];		// �~���R�X�L�[�a��ł̃T�|�[�g�|�C���g(���[���h�n)

//	�ߋ���SupportPoint������`��(�_�E�����E3�p�`�C4�ʑ�)�D�ő�4�_�܂ŁD
static int usedPoints;		//	����4�r�b�g���C�ǂ�4�_�Ō��݂̌`�󂪍\������邩��\���D
static int lastPoint;		//	�Ō�Ɍ�����SupportPoint�����܂����ꏊ(0�`3)
static int lastUsed;		//	lastUsed = 1<<lastPoint (�r�b�g�ŕ\�������́j
static int allUsedPoints;	//	lastUsed������������
static double det[16][4];	//	�W��
//	det[3][0] * p[0] + det[3][1]*p[1] + det[3][2]*p[2] / sum �ōŋߖT�_�����܂�
//	p_q�x�N�g������������`��̑̐�(or����or�ʐ�)�D4�_�������16�ʂ�ɂ���

/*
      a   |
     /    | v = (1-k) a + k b
    /     | va = (1-k)aa + kba = vb = (1-k)ab + kbb
   /v     | -kaa + kba + kab - kbb = -aa + ab
  b       | k = a^2-ab / (a^2+b^2-2ab), 1-k = b^2-ab / (a^2+b^2-2ab)
----------+ �Ȃ̂ŁC k = a^2-ab �� �� (k-1) = b^2-ab���L�^���Ă���	*/


inline void CalcDet() {
	static double dotp[4][4];	//	p_q[i] * p_q[j] 

	//	�V�����������_(lastPoint)�ɂ��āC���ς��v�Z
	for (int i = 0, curPoint = 1; i < 4; ++i, curPoint <<=1){
		if (usedPoints & curPoint) 
			dotp[i][lastPoint] = dotp[lastPoint][i] = p_q[i] * p_q[lastPoint];
	}
	dotp[lastPoint][lastPoint] = p_q[lastPoint] * p_q[lastPoint];

	det[lastUsed][lastPoint] = 1;
	for (int j = 0, sj = 1; j < 4; ++j, sj <<= 1) {
		if (usedPoints & sj) {		
			int s2 = sj|lastUsed;	//	�V�����������_�ɂ��ČW���̌v�Z
			det[s2][j] = dotp[lastPoint][lastPoint] - dotp[lastPoint][j];	//	a^2-ab
			det[s2][lastPoint] = dotp[j][j] - dotp[j][lastPoint];			//	a^2-ab
			for (int k = 0, sk = 1; k < j; ++k, sk <<= 1) {	//	3�_�̏ꍇ
				if (usedPoints & sk) {
					int s3 = sk|s2;
					det[s3][k] = det[s2][j] * (dotp[j][j] - dotp[j][k]) + 
						det[s2][lastPoint] * (dotp[lastPoint][j] - dotp[lastPoint][k]);
					det[s3][j] = det[sk|lastUsed][k] * (dotp[k][k] - dotp[k][j]) + 
						det[sk|lastUsed][lastPoint] * (dotp[lastPoint][k] - dotp[lastPoint][j]);
					det[s3][lastPoint] = det[sk|sj][k] * (dotp[k][k] - dotp[k][lastPoint]) + 
						det[sk|sj][j] * (dotp[j][k] - dotp[j][lastPoint]);
				}
			}
		}
	}
	if (allUsedPoints == 15) {	//	4�_�̏ꍇ
		det[15][0] =	det[14][1] * (dotp[1][1] - dotp[1][0]) + 
						det[14][2] * (dotp[2][1] - dotp[2][0]) + 
						det[14][3] * (dotp[3][1] - dotp[3][0]);
		det[15][1] =	det[13][0] * (dotp[0][0] - dotp[0][1]) + 
						det[13][2] * (dotp[2][0] - dotp[2][1]) + 
						det[13][3] * (dotp[3][0] - dotp[3][1]);
		det[15][2] =	det[11][0] * (dotp[0][0] - dotp[0][2]) + 
						det[11][1] * (dotp[1][0] - dotp[1][2]) +	
						det[11][3] * (dotp[3][0] - dotp[3][2]);
		det[15][3] =	det[ 7][0] * (dotp[0][0] - dotp[0][3]) + 
						det[ 7][1] * (dotp[1][0] - dotp[1][3]) + 
						det[ 7][2] * (dotp[2][0] - dotp[2][3]);
	}	
}

//	�ŋߖT�_���n����Simplex�̒��ɂ��邩�ǂ����D�Ȃ���� false
inline bool IsVaildPoint(int s) {
	for (int i = 0, curPoint = 1; i < 4; ++i, curPoint <<= 1) {
		if (allUsedPoints & curPoint) {
			if (s & curPoint) { if (det[s][i] <= 0) return false; }
			else if (det[s|curPoint][i] > 0) return false;	//	>eplision�ɂ��������悢����
		}
	}
	return true;
}

//	�W������C�ŋߖT�_ v ���v�Z
inline void CalcVector(int usedPoints, Vec3d& v) {
	double sum = 0;
	v = Vec3d(0, 0, 0);
	for (int i = 0, curPoint = 1; i < 4; ++i, curPoint <<= 1) {
		if (usedPoints & curPoint) {
			sum += det[usedPoints][i];
			v += p_q[i] * det[usedPoints][i];
		}
	}
	v *= 1 / sum;
}

//	�W������C�ŋߖT�_�ƁC����2�̐}�`��ł́C���̓_�̈ʒu���v�Z
inline void CalcPoints(int usedPoints, Vec3d& p1, Vec3d& p2) {
	double sum = 0;
	p1 = Vec3d(0.0, 0.0, 0.0);
	p2 = Vec3d(0.0, 0.0, 0.0);
	for (int i = 0, curPoint = 1; i < 4; ++i, curPoint <<= 1) {
		if (usedPoints & curPoint) {
			sum += det[usedPoints][i];
			p1 += p[i] * det[usedPoints][i];
			p2 += q[i] * det[usedPoints][i];
		}
	}
	if (sum){
		double s = 1 / sum;
		p1 *= s;
		p2 *= s;
	}else{
		p1 = p[0];
		p2 = q[0];
	}
}

//	
inline bool CalcClosest(Vec3d& v) {
	CalcDet();
	for (int s = usedPoints; s; --s) {
		if ((s & usedPoints) == s) {		//	���݂̌`�󂩂璸�_�����炵�Ă��������̂ɂ��āC
			if (IsVaildPoint(s|lastUsed)) {	//	���ɍŋߖT�_������Simplex������������
				usedPoints = s|lastUsed;
				CalcVector(usedPoints, v);	//	�ŋߖT�_���v�Z���ĕԂ��D
				return true;
			}
		}
	}
	if (IsVaildPoint(lastUsed)) {			//	�Ō�Ɍ������_�̐^�ゾ������
		usedPoints = lastUsed;
		v = p_q[lastPoint];
		return true;
	}
	return false;
}

//	�k�ނ��Ă���ꍇ��4�_�̂����������������ʒu�ɂ���Ƃ�
inline bool IsDegenerate(const Vec3d& w) {
	for (int i = 0, curPoint = 1; i < 4; ++i, curPoint <<= 1){
//	hase	2003.10.24
//		if ((allUsedPoints & curPoint) && p_q[i] == w) return true;
		if ((allUsedPoints & curPoint) && (p_q[i]-w).square() < 1e-6){
//			DSTR << "T:" << (p_q[i]-w).norm() << p_q[i] << w << std::endl;
			return true;
		}
	}
	return false;
}

bool FindCommonPoint(const CDConvex* a, const CDConvex* b,
					 const Posed& a2w, const Posed& b2w,
					 Vec3d& v, Vec3d& pa, Vec3d& pb) {
	Vec3d w;

	usedPoints = 0;
	allUsedPoints = 0;

	do {
		lastPoint = 0;
		lastUsed = 1;
		while (usedPoints & lastUsed) { ++lastPoint; lastUsed <<= 1; }
		p[lastPoint] = a->Support(a2w.Ori().Conjugated() * (-v));
		q[lastPoint] = b->Support(b2w.Ori().Conjugated() * v);
		w = a2w * p[lastPoint]  -  b2w * q[lastPoint];
		if (v*w > 0) return false;
		if (IsDegenerate(w)) return false;
		p_q[lastPoint] = w;
		allUsedPoints = usedPoints|lastUsed;
		if (!CalcClosest(v)) return false;
	} while ( usedPoints < 15 && !(v.square() < 1e-10f) ) ;
	CalcPoints(usedPoints, pa, pb);
	return true;
}

void FindClosestPoints(const CDConvex* a, const CDConvex* b,
					  const Posed& a2w, const Posed& b2w,
					  Vec3d& pa, Vec3d& pb) {
	Vec3d v; 				//	���݂́CA��B�̃~���R�X�L�[�a�̌`�󒆂�1�_
	v = a2w * a->Support(Vec3d()) - b2w * b->Support(Vec3d());	
	double dist = v.norm();	//	���_����̋���
	Vec3d w;				//	�T�|�[�g�|�C���g
	double maxSupportDist = 0.0f;

	usedPoints = 0;
	allUsedPoints = 0;

	while (usedPoints < 15 && dist > absError) {
		lastPoint = 0;
		lastUsed = 1;
		while (usedPoints & lastUsed) { ++lastPoint; lastUsed <<= 1; }
		p[lastPoint] = a->Support(a2w.Ori().Conjugated() * (-v));
		q[lastPoint] = b->Support(b2w.Ori().Conjugated() * v);
		w = a2w * p[lastPoint]  -  b2w * q[lastPoint];
		double supportDist = w*v/dist;
		if (maxSupportDist < supportDist) maxSupportDist= supportDist;
		if (dist - maxSupportDist <= dist * relError) break;
		if (IsDegenerate(w)) break;
		p_q[lastPoint] = w;
		allUsedPoints = usedPoints|lastUsed;
		if (!CalcClosest(v)) break;
		dist = v.norm();
	}
	CalcPoints(usedPoints, pa, pb);
}

}
