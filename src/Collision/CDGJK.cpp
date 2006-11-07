/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/*	���̃t�@�C���̃��C�Z���X�ɂ��Ă̒���
	���̃\�[�X�́C�ꕔ SOLID (Software Library for Interference Detection) 2.0
	http://www.win.tue.nl/~gino/solid �� src/Convex.cpp ���Q�l�ɏ����Ă��܂��D

	��҂́C���̃\�[�X��Solid2.0�̈ꕔ�̔h�����ł͂Ȃ��ƐM���Ă��܂��D
	�������C���Ă���ӏ�������܂��D�����h�������ƔF�肳�ꂽ�ꍇ�C
	���C�Z���X��GPL�ƂȂ�܂��D�����ӂ��������D
*/

#include "Collision.h"
#include <Foundation/Scene.h>
#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

namespace Spr{;
const double sqEpsilon = 1e-4;
const double epsilon = 1e-8;
const double epsilon2 = epsilon*epsilon;

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

///	det(�����E�ʐρE�̐�)�̌v�Z�D�����̑������_����:���ɓ������邩���v�Z
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
			det[s2][lastPoint] = dotp[j][j] - dotp[j][lastPoint];			//	b^2-ab
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
		if ((allUsedPoints & curPoint) && (p_q[i]-w).square() < epsilon2) return true;
	}
	return false;
}

bool FindCommonPoint(const CDConvex* a, const CDConvex* b,
	const Posed& a2w, const Posed& b2w, Vec3d& v, Vec3d& pa, Vec3d& pb) {
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
	} while ( usedPoints < 15 && !(v.square() < epsilon2) ) ;
	CalcPoints(usedPoints, pa, pb);
	return true;
}


struct CDGJKIds{
	char i[3];		//	���_��ID
	char nVtx;		//	�g�p���钸�_�̐�
	double dist;
	Vec3d normal;
	double k[4];
	CDGJKIds(){
		i[0] = i[1] = i[2] = -1;
		nVtx = -1;
		dist = -1;
	}
};
const char vacants[] = {
	0, 1, 0, 2, 0, 1, 0, 3,
	0, 1, 0, 2, 0, 1, 0, 4,
};
inline char VacantId(char a, char b){
	char bits = (1<<a) | (1<<b);
	return vacants[bits];
}
inline char VacantId(char a, char b, char c){
	char bits = (1<<a) | (1<<b) | (1<<c);
	return vacants[bits];
}
#define XY()	sub_vector( PTM::TSubVectorDim<0,2>() )
#define CalcSupport(v, n)										\
	p[n] = a->Support(a2z.Ori().Conjugated() * (v));			\
	q[n] = b->Support(b2z.Ori().Conjugated() * -(v));			\
	w[n] = b2z * (q[n]) - a2z * (p[n]);							\

int ContFindCommonPoint(const CDConvex* a, const CDConvex* b,
	const Posed& a2w, const Posed& b2w, Vec3d& range, Vec3d& normal, Vec3d& pa, Vec3d& pb, double& dist){
	//	range ��+Z�ɂȂ�悤�ȍ��W�n�����߂�D
	Quaterniond w2z;
	double endLength = range.norm();
	range /= endLength;
	Vec3d u = -range;	//	u: ���̂ł͂Ȃ����_�̑��x�̌����Ȃ̂� - �����D
	if (u.Z() < -1+epsilon){
		w2z = Quaterniond::Rot(Rad(180), 'x');
	}else if (u.Z() < 1-epsilon){
		Matrix3d matW2z = Matrix3d::Rot(u, Vec3f(0,0,1), 'z');
		w2z.FromMatrix(matW2z);
		w2z = w2z.Inv();
	}
	Posed a2z;
	a2z.Ori() = w2z * a2w.Ori();
	a2z.Pos() = w2z * a2w.Pos();
	Posed b2z;
	b2z.Ori() = w2z * b2w.Ori();
	b2z.Pos() = w2z * b2w.Pos();

	
	//	GJK�Ǝ������@�ŁC��_�����߂�
	
	static CDGJKIds ids[2];
	static Vec3d w[4], p[4], q[4];
	CDGJKIds* cur = ids;
	CDGJKIds* last = ids+1;

	//	w0�����߂�
	CalcSupport(Vec3d(0,0,1), 0);
	if (w[0].Z() > endLength) return -1;	//	range���ł͐ڐG���Ȃ����C�����ڐG���邩������Ȃ��D
	if (w[0].Z() < 0){						//	���Α���support�����߂Ă݂āC�͈͊O���m�F
		CalcSupport(Vec3d(0,0,-1), 3);
		if (w[3].Z() <0) return -2;			//	range���ł͐ڐG���Ȃ����C�ߋ�(���ɉ��������)�ڐG���Ă�����������Ȃ��D
	}
	//	w1�����߂�
	Vec3d v(w[0].X(), w[0].Y(), 0);
	if (v.XY().square() < epsilon2){	//	w0=�Փ˓_�̏ꍇ
		normal = u.unit();
		pa = p[0]; pb = q[0];
		dist = w[0].Z();
		return 1;
	}
	CalcSupport(v, 1);
	if (w[1].XY() * v.XY() > 0) return 0;	//	w[1]�̊O����O������̂ŐG���ĂȂ�
	
	
	//	w[0]-w[1]-w[0] ���O�p�`�ƍl���ăX�^�[�g���āCo���O�p�`�̓����ɓ���܂ŌJ��Ԃ�
	cur->i[0] = 1;	//	�V�������_
	cur->i[1] = 0;	//	���Ƃ̐���
	cur->i[2] = 0;	//	���Ƃ̐���
	while(1){
		double s = w[cur->i[0]].XY() ^ w[cur->i[1]].XY();
		if (s > epsilon){		//	1-0����͂ݏo���Ă���
			Vec2d l = w[cur->i[1]].XY() - w[cur->i[0]].XY();
			assert(l.square() >= epsilon2);		//	w0=w1�Ȃ�΁C���łɔ����Ă���͂��D
			double ll_inv = 1/l.square();
			v.XY() = (w[cur->i[1]].XY()*l*ll_inv) * w[cur->i[0]].XY() - (w[cur->i[0]].XY()*l*ll_inv) * w[cur->i[1]].XY();
			v.Z() = 0;
			cur->i[2] = cur->i[0];
			cur->i[0] = VacantId(cur->i[1], cur->i[2]);
		}else{
			s = w[cur->i[2]].XY() ^ w[cur->i[0]].XY();
			if (s > epsilon){	//	0-2����͂ݏo���Ă���
				Vec2d l = w[cur->i[2]].XY() - w[cur->i[0]].XY();
				assert(l.square() >= epsilon2);		//	w0=w1�Ȃ�΁C���łɔ����Ă���͂��D
				double ll_inv = 1/l.square();
				v.XY() = (w[cur->i[2]].XY()*l*ll_inv) * w[cur->i[0]].XY() - (w[cur->i[0]].XY()*l*ll_inv) * w[cur->i[2]].XY();
				v.Z() = 0;
				cur->i[1] = cur->i[0];
				cur->i[0] = VacantId(cur->i[1], cur->i[2]);
			}else{				//	����
				break;
			}
		}
		CalcSupport(v, cur->i[0]);
		if (w[cur->i[0]].XY() * v.XY() > -epsilon2){	//	0�̊O����o������̂ŐG���ĂȂ�
			return 0;
		}
		if(	(w[cur->i[0]].XY()-w[cur->i[1]].XY()).square() < epsilon2 || 
			(w[cur->i[2]].XY()-w[cur->i[1]].XY()).square() < epsilon2){
			return 0;								//	����w: �ӂ̍X�V�Ȃ���O�͕ӂ̊O��
		}
	}
	last->nVtx = 0;
	//	�O�p�` cur 0-1-2 �̒���o������D	cur 0���Ō�ɍX�V�������_w
	while(1){
		static Vec3d s;		//	�O�p�`�̗L���ʐ�
		s = (w[cur->i[1]]-w[cur->i[0]]) % (w[cur->i[2]]-w[cur->i[0]]);
		//	���_�̕��я������낦��D
		if (s.Z() < 0){		//	�t����
			std::swap(cur->i[1], cur->i[2]);
			s *= -1;
		}
		if (s.Z() < epsilon){	//	�����ɂȂ�ꍇ
			cur->nVtx = 2;		//	�g���̂�2�_�C�ǂ����2�_���g�������肷��D
			double ip1 = w[cur->i[0]].XY() * w[cur->i[1]].XY();
			double ip2 = w[cur->i[0]].XY() * w[cur->i[2]].XY();
			if (ip1 < epsilon && ip2 < epsilon){	//	0-1��0-2��o���܂�
				cur->k[cur->i[0]] = w[cur->i[0]].XY().norm();
				cur->k[cur->i[1]] = w[cur->i[1]].XY().norm();
				cur->dist = cur->k[cur->i[0]]*w[cur->i[0]].Z() + cur->k[cur->i[1]]*w[cur->i[1]].Z();
				double l1 = cur->k[cur->i[0]] + cur->k[cur->i[1]];
				cur->dist /= l1;
				
				cur->k[cur->i[2]] = w[cur->i[1]].XY().norm();
				double d2 = cur->k[cur->i[0]]*w[cur->i[0]].Z() + cur->k[cur->i[2]]*w[cur->i[2]].Z();
				double l2 = cur->k[cur->i[0]] + cur->k[cur->i[2]];
				d2 /= l2;
				if (d2 < cur->dist){
					std::swap(cur->i[1], cur->i[2]);
					cur->dist = d2;
				}
			}else{
				if (ip2 < ip1) std::swap(cur->i[1], cur->i[2]);
				cur->k[cur->i[0]] = w[cur->i[0]].XY().norm();
				cur->k[cur->i[1]] = w[cur->i[1]].XY().norm();
				cur->dist = cur->k[cur->i[0]]*w[cur->i[0]].Z() + cur->k[cur->i[1]]*w[cur->i[1]].Z();
				double l1 = cur->k[cur->i[0]] + cur->k[cur->i[1]];
				cur->dist /= l1;
			}
			if (last->nVtx){
				double approach = last->normal * (w[cur->i[0]] - w[last->i[0]]);
				if (approach > -epsilon || cur->dist <= last->dist) break;	//	return last
			}
			Vec3d l = w[cur->i[0]] - w[cur->i[1]];
			cur->normal = Vec3d(0,0,1) - l.Z() / l.square() * l;
			cur->normal.unitize();
			std::swap(cur, last);
			cur->i[1] = last->i[0];
			cur->i[2] = last->i[1];
			cur->i[0] = VacantId(cur->i[1], cur->i[2]);
			CalcSupport(last->normal, 0);
		}else{	//	�O�p�`�ɂȂ�ꍇ
			cur->nVtx = 3;		//	�g���̂�3�_�D
			cur->normal = s.unit();
			cur->dist = w[cur->i[0]] * cur->normal / cur->normal.Z();
			if (last->nVtx){
				double approach = last->normal * (w[cur->i[0]] - w[last->i[0]]);
				if (approach > -epsilon || (approach > -sqEpsilon && cur->dist <= last->dist)) break;	//	return last;
			}

			int newVtx = VacantId(cur->i[0], cur->i[1], cur->i[2]);
			CalcSupport(cur->normal, newVtx);
			
			//	�V�����_ newVtx�ƌ��́���2�_w�Ō��_���͂ށ������
			//	(w[cur->i]-w[newVtx])^(o-w[newVtx]) �� + ���� - �ɕω�����Ƃ����T���D
			bool bPlus = false;
			bool bMinus = false;
			double ow[3];
			int i;
			for(i=0; i<3;++i){
				Vec2d wn = w[cur->i[i]].XY()-w[newVtx].XY();
				ow[i] = wn % (-w[newVtx].XY());
				if (ow[i] < 0){
					bMinus = true;
					if (bPlus) break;
				}else if(ow[i] > 0) {
					bPlus= true;
				}
			}
			int replace = -100;
			if (bPlus && bMinus){
				//	+-���o�������ꍇ�F+����-�Ɉڂ������̒��_���u�������钸�_
				replace = (i+1)%3;
			}else{
				//	+-���o����Ȃ��ꍇ�C�S��0�̏ꍇ�Fw[newVtx]�ɋ߂�w[i]��u������
				double minDist = DBL_MAX;
				for(int i=0; i<3; ++i){
					double d = (w[cur->i[i]].XY() - w[newVtx].XY()).square();
					if (d<minDist){
						minDist = d; replace = i;
					}
				}
			}
			std::swap(cur, last);
			cur->i[0] = newVtx;
			cur->i[1] = last->i[(replace+1)%3];
			cur->i[2] = last->i[(replace+2)%3];
		}
	}
	if (last->nVtx == 2){	//	�����̏ꍇ�C�v�Z�ς݂�k���g����pa,pb���v�Z
		double sumInv = 1 / (last->k[0]+last->k[1]);
		last->k[0] *= sumInv; last->k[1] *= sumInv;
		pa = last->k[0] * p[last->i[0]] + last->k[1] * p[last->i[1]];
		pb = last->k[0] * q[last->i[0]] + last->k[1] * q[last->i[1]];
	}else{					//	�O�p�`�̏ꍇ�Ck�̌v�Z�����Ă��Ȃ��̂ł����Ōv�Z
		Matrix2d m;
		m.Ex() = w[last->i[0]].XY()-w[last->i[1]].XY();
		m.Ey() = w[last->i[0]].XY()-w[last->i[2]].XY();
		Vec2d k = m.inv() * w[last->i[0]].XY();
		double kz = 1-k.x-k.y;
		pa = k.x*p[last->i[1]] + k.y*p[last->i[2]] + kz*p[last->i[0]];
		pb = k.x*q[last->i[1]] + k.y*q[last->i[2]] + kz*q[last->i[0]];
	}
	dist = last->dist;
	normal = w2z.Conjugated() * last->normal;
	return last->nVtx;
}

}
