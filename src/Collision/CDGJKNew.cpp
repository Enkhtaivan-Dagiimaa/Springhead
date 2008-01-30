/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/*	���̃t�@�C���̃��C�Z���X�ɂ��Ă̒���
	���̃\�[�X�́C�ꕔ SOLID (Software Library for Interference Detection) 2.0
	http://www.win.tue.nl/~gino/solid �� src/Convex.cpp ���Q�l�ɏ����Ă��܂��D
	�_���ƃ\�[�X�����J����Ă��� Bergen ����Ɋ��ӂ��܂��D

	���J��́C���̃\�[�X��Solid2.0�̈ꕔ�̔h�����ł͂Ȃ��ƐM���Ă��܂��D
	�������C���Ă���ӏ�������܂��D�����h�������ƔF�肳�ꂽ�ꍇ�C
	���C�Z���X��LGPL�ƂȂ�܂��D�����ӂ��������D
*/
static bool bDebug;


#include "Collision.h"
#include <Foundation/Scene.h>
#include <fstream>
#include <windows.h>
#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

namespace Spr{;

void SaveMaterial(std::ostream& file, PHMaterial& m){
	file << m.e << " ";
	file << m.mu << " " << m.mu0 << std::endl;
}
void LoadMaterial(std::istream& file, PHMaterial& m){
	file >> m.e;
	file >> m.mu >> m.mu0;
}
void SaveShape(std::ostream& file, CDShape* a){
	CDConvexMesh* mesh = a->Cast();
	if (mesh){
		file << "mesh" << std::endl;
		CDConvexMeshDesc desc;
		mesh->GetDesc(&desc);
		SaveMaterial(file, desc.material);
		file << desc.vertices.size() << " ";
		for(unsigned i=0; i<desc.vertices.size(); ++i){
			file << desc.vertices[i] << std::endl;
		}
	}
	CDBox* box = a->Cast();
	if (box){
		file << "box" << std::endl;
		CDBoxDesc desc;
		mesh->GetDesc(&desc);
		SaveMaterial(file, desc.material);
		file << desc.boxsize << std::endl;
	}		
	CDCapsule* cap = a->Cast();
	if (cap){
		file << "cap" << std::endl;
		CDCapsuleDesc desc;
		cap->GetDesc(&desc);
		SaveMaterial(file, desc.material);
		file << desc.radius << std::endl;
		file << desc.length << std::endl;
	}		
}
CDConvex* LoadShape(std::istream& file, PHSdkIf* sdk){
	CDConvex* rv = NULL;
	char type[100];
	file >> type;
	if( strcmp(type, "mesh") == 0 ){
		CDConvexMeshDesc desc;
		LoadMaterial(file, desc.material);			
		int size;
		file >> size;
		desc.vertices.resize(size);
		for(int i=0; i<size; ++i){
			file >> desc.vertices[i];
		}
		rv = sdk->CreateShape(CDConvexMeshIf::GetIfInfoStatic(), desc)->Cast();
	}
	if( strcmp(type, "box") == 0){
		CDBoxDesc desc;
		LoadMaterial(file, desc.material);			
		file >> desc.boxsize;
		rv = sdk->CreateShape(CDBoxIf::GetIfInfoStatic(), desc)->Cast();
	}
	if( strcmp(type, "cap") == 0){
		CDCapsuleDesc desc;
		LoadMaterial(file, desc.material);			
		file >> desc.radius;
		file >> desc.length;
		rv = sdk->CreateShape(CDCapsuleIf::GetIfInfoStatic(), desc)->Cast();
	}
	return rv;
}

int FASTCALL ContFindCommonPoint(const CDConvex* a, const CDConvex* b,
	const Posed& a2w, const Posed& b2w, Vec3d& range, Vec3d& normal, 
	Vec3d& pa, Vec3d& pb, double& dist);
void ContFindCommonPointSaveParam(const CDConvex* a, const CDConvex* b,
	const Posed& a2w, const Posed& b2w, Vec3d& range, Vec3d& normal, 
	Vec3d& pa, Vec3d& pb, double& dist){
	std::ofstream file("ContFindCommonPointSaveParam.txt");
	SaveShape(file, (CDConvex*)a);
	SaveShape(file, (CDConvex*)b);
	file << a2w << std::endl;
	file << b2w << std::endl;
	file << range << std::endl;
	file << normal << std::endl;
	file << pa << std::endl;
	file << pb << std::endl;
	file << dist << std::endl;
}
void ContFindCommonPointCall(std::istream& file, PHSdkIf* sdk){
	bDebug = true;
	const CDConvex* a;
	const CDConvex* b;
	Posed a2w, b2w;
	Vec3d range, normal, pa, pb;
	double dist;
	a = LoadShape(file, sdk);
	b = LoadShape(file, sdk);
	file >> a2w;
	file >> b2w;
	file >> range;
	file >> normal;
	file >> pa;
	file >> pb;
	file >> dist;

	Vec3f dir = b2w.Ori() * Vec3f(0,0,1);
	DSTR << "dir of capsule = " << dir << std::endl;
	DSTR << "center of capsule = " << b2w.Pos()  << std::endl;
	ContFindCommonPoint(a, b, a2w, b2w, range, normal, pa, pb, dist);
	DSTR << "normal = " << normal << std::endl;
	DSTR << "pa and pb in W = "<< std::endl;
	DSTR << a2w * pa << std::endl;
	DSTR << b2w * pb << std::endl;
}


const double sqEpsilon = 1e-3;
const double epsilon   = 1e-6;  // s��2e-6�ɂȂ邱�Ƃ��������D�܂����߂�������Ȃ��D�imitake�j
const double epsilon2  = epsilon*epsilon;

static Vec3d p[4];			// A�̃T�|�[�g�|�C���g(���[�J���n)
static Vec3d q[4];			// B�̃T�|�[�g�|�C���g(���[�J���n)
static Vec3d p_q[4];		// �~���R�X�L�[�a��ł̃T�|�[�g�|�C���g(���[���h�n)

const char vacants[] = {
	0, 1, 0, 2, 0, 1, 0, 3,
	0, 1, 0, 2, 0, 1, 0, 4,
};
inline char FindVacantId(char a, char b){
	char bits = (1<<a) | (1<<b);
	return vacants[(int)bits];
}
inline char FindVacantId(char a, char b, char c){
	char bits = (1<<a) | (1<<b) | (1<<c);
	return vacants[(int)bits];
}

#if 1	//	���ʂ���Ȃ�GJK
#define XY()	sub_vector( PTM::TSubVectorDim<0,2>() )
#define CalcSupport(n)											\
	p[n] = a->Support(a2z.Ori().Conjugated() * (v[n]));			\
	q[n] = b->Support(b2z.Ori().Conjugated() * -(v[n]));		\
	w[n] = b2z * (q[n]) - a2z * (p[n]);

int FASTCALL ContFindCommonPoint(const CDConvex* a, const CDConvex* b,
	const Posed& a2w, const Posed& b2w, Vec3d& range, Vec3d& normal, 
	Vec3d& pa, Vec3d& pb, double& dist){
	Vec3d rangeOrg = range;
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
	//	�܂��A2�����Ō����Ƃ��ɁA���_���܂܂��悤�ȎO�p�`�܂��͐��������
	int ids[4];
	Vec3d w[4], p[4], q[4], v[4];

	//	w0�����߂�
	v[0] = Vec3d(0,0,1);
	CalcSupport(0);
	if (w[0].Z() > endLength) 
		return -1;	//	range���ł͐ڐG���Ȃ����C�����ڐG���邩������Ȃ��D
	if (w[0].Z() < 0){						//	���Α���support�����߂Ă݂āC�͈͊O���m�F
		v[3] = Vec3d(0,0,-1);
		CalcSupport(3);
		if (w[3].Z() <0){
			//	range���ł͐ڐG���Ȃ����C�ߋ�(���ɉ��������)�ڐG���Ă�����������Ȃ��D
			return -2;			
		}
	}
	//	w1�����߂�
	v[1] = Vec3d(w[0].X(), w[0].Y(), 0);
	if (v[1].XY().square() < epsilon2){		//	w0=�Փ˓_�̏ꍇ
		normal = u.unit();
		pa = p[0]; pb = q[0];
		dist = w[0].Z();
		return 1;
	}
	CalcSupport(1);
	if (w[1].XY() * v[1].XY() > 0) 
		return 0;	//	w[1]�̊O����O������̂ŐG���ĂȂ�
	
	
	//	w[0]-w[1]-w[0] ���O�p�`�ƍl���ăX�^�[�g���āCo���O�p�`�̓����ɓ���܂ŌJ��Ԃ�
	ids[0] = 1;	//	�V�������_
	ids[1] = 0;	//	���Ƃ̐���
	ids[2] = 0;	//	���Ƃ̐���
	while(1){
		double s;
		Vec3d vNew;
		if ((s = w[(int)ids[0]].XY() ^ w[(int)ids[1]].XY()) > epsilon){
			//	�_O���A����1-0����A�O�p�`�̊O�ɂ͂ݏo���Ă���ꍇ  
			//		... epsilon=1e-8���Ɩ������[�v�C1e-7�ł��H�ɖ������[�v
			//	1-0�̖@���̌���vNew��support point��T���A�V�����O�p�`�ɂ���B
			Vec2d l = w[(int)ids[1]].XY() - w[(int)ids[0]].XY();
			assert(l.square() >= epsilon2);		//	w0=w1�Ȃ�΁C���łɔ����Ă���͂��D
			double ll_inv = 1/l.square();
			vNew.XY() = (w[(int)ids[1]].XY()*l*ll_inv) * w[(int)ids[0]].XY()
				   - (w[(int)ids[0]].XY()*l*ll_inv) * w[(int)ids[1]].XY();
			vNew.Z() = 0;
			ids[2] = ids[0];
			ids[0] = FindVacantId(ids[1], ids[2]);
		}else if ((s = w[(int)ids[2]].XY() ^ w[(int)ids[0]].XY()) > epsilon){
			//	�_O���A����2-0����A�O�p�`�̊O�ɂ͂ݏo���Ă���ꍇ
			//	2-0�̖@���̌���v��support point��T���A�V�����O�p�`�ɂ���B
			Vec2d l = w[(int)ids[2]].XY() - w[(int)ids[0]].XY();
			assert(l.square() >= epsilon2);		//	w0=w1�Ȃ�΁C���łɔ����Ă���͂��D
			double ll_inv = 1/l.square();
			vNew.XY() = (w[(int)ids[2]].XY()*l*ll_inv) * w[(int)ids[0]].XY()
				   - (w[(int)ids[0]].XY()*l*ll_inv) * w[(int)ids[2]].XY();
			vNew.Z() = 0;
			ids[1] = ids[0];
			ids[0] = FindVacantId(ids[1], ids[2]);
		}else{
			//	�_O�͎O�p�`�̓����ɂ���B
			if (ids[1] == ids[2]){
				//	1��2�������_=�ŏ�����online���������߁A3�p�`���ł��Ȃ������B
				assert(ids[0] == 1);
				assert(ids[1] == 0);
				ids[2] = 2;
				w[2] = w[0];
				v[2] = v[0];
				p[2] = p[0];
				q[2] = q[0];
			}
			break;
		}
		v[ids[0]] = vNew;
		CalcSupport(ids[0]);	//	�@���̌���vNew�ŃT�|�[�g�|�C���g��T��
		if (w[ids[0]].XY() * v[ids[0]].XY() > -epsilon2){	//	0�̊O����o������̂ŐG���ĂȂ�
			return 0;
		}
		//	�V����support��1��O�̐�������܂����������Ă��Ȃ� �� �_O�͊O��
		double d1 = -vNew.XY() * (w[(int)ids[0]].XY()-w[(int)ids[1]].XY());
		double d2 = -vNew.XY() * (w[(int)ids[0]].XY()-w[(int)ids[2]].XY());
		if (d1 < epsilon2 || d2 < epsilon2) 
			return 0;
	}
	ids[3] = 3;
	//	�O�p�` ids[0-1-2] �̒���o������Dids[0]���Ō�ɍX�V�������_w
	//	�O�p�`�����������Ă���
	int notuse;
	int count = 0;
	while(1){
		count ++;
		if (count > 1000) {
#if USERNAME==hase
			//	���J���p�f�o�b�O�R�[�h�B���ݓ����蔻��Debug���B			
			DSTR << "Too many loop in CCDGJK." << std::endl;
			ContFindCommonPointSaveParam(a, b, a2w, b2w, rangeOrg, normal, pa, pb, dist);
			
			DebugBreak();
			bDebug=true;
			ContFindCommonPoint(a, b, a2w, b2w, rangeOrg, normal, pa, pb, dist);
#else
			if (!bDebug) return -1;
#endif
		}
		Vec3d s;		//	�O�p�`�̗L���ʐ�
		s = (w[ids[1]]-w[ids[0]]) % (w[ids[2]]-w[ids[0]]);
		if (s.Z() < 0){		//	�t�����̏ꍇ�A�Ђ�����Ԃ�
			std::swap(ids[1], ids[2]);
			s *= -1;
		}
		notuse = -1;
		if (s.Z() > epsilon){
			if (bDebug) DSTR << "TRI ";
			//	�O�p�`�ɂȂ�ꍇ
			v[ids[3]] = s.unit();	//	3�p�`�̖@�����g��
		}else{
			if (bDebug) DSTR << "LINE";
			//	�����ɂȂ�ꍇ�A�@�������܂�Ȃ��̂ŁAo����߂�2�_�����߂��ۂ̖@���̕��ς��g��
			//	���_O���܂݁A��ԒZ��������I�� -> o����߂�2�_��I��	x--o--x----x
			double sqMax = 0;
			for(int i=0; i<3; ++i){
				double sq = w[ids[i]].XY().square();
				if (sq > sqMax){
					sq = sqMax;
					notuse = i;
				}
			}
			int id0 = (notuse+1)%3;
			int id1 = (notuse+2)%3;
			v[ids[3]] = (v[ids[id0]] + v[ids[id1]]).unit();
		}
		//	�V���� w w[3] �����߂�
		CalcSupport(ids[3]);
		

		if (bDebug){
			DSTR << "v:" << v[ids[3]];
			for(int i=0; i<4; ++i){
				DSTR << "  w[" << (int) ids[i] << "] = " << w[ids[i]];
			}
			DSTR << std::endl;
		}
		//	�V����w���A����܂ł̓_���߂����`�F�b�N����B
		double improvement = DBL_MAX;
		for(int i=0; i<3; ++i){
			double delta = -(w[ids[3]] - w[ids[i]]) * v[ids[3]];
			if (improvement > delta) improvement = delta;
		}
		if (bDebug) {
			DSTR << "notuse" << notuse;
			for(int i=0; i<4; ++i) DSTR << " " << ids[i];
			DSTR << std::endl;
			DSTR << "Improvement: " << improvement << std::endl;
		}
		if (improvement < epsilon) goto final;

		if (notuse>=0){	//	�����̏ꍇ�A�g����2�_�ƐV�����_�ŎO�p�`�����
			std::swap(ids[notuse], ids[3]);
		}else{
			//	�ǂ�2�_��w[3]�ŎO�p�`�����邩�m�F���� -> w[3] ���A�c��2�ŕ\������
			double min = 1e10;
			int minId;
			for(int i=0; i<3; ++i){
				Matrix2d m;	//	(w[i] w[i+1])  t(k1,k2) = w[3]
				m.Ex() = w[ids[i]].XY();
				m.Ey() = w[ids[(i+1)%3]].XY();
				Vec2d k = m.inv() * w[ids[3]].XY();
				if (k.x > k.y){
					if (k.x < min){
						min = k.x;
						minId = i;
					}
				}else{
					if (k.y < min){
						min = k.y;
						minId = i;
					}
				}
			}
			std::swap(ids[(minId+2)%3], ids[3]);
			if (min > 1e-5){	//	�ُ�B���_���܂ގO�p�`��������Ȃ�
				//	�f�o�b�O�p����
				DSTR << "No including traiangle found." << std::endl;
				if (bDebug){
					ContFindCommonPoint(a, b, a2w, b2w, rangeOrg, 
						normal, pa, pb, dist);
				}
			}
		}
	}
	//	������~
final:
	if (notuse >=0){
		int id0 = ids[(notuse+1)%3];
		int id1 = ids[(notuse+2)%3];
		double l0 = w[ids[id0]].XY().norm();
		double l1 = w[ids[id1]].XY().norm();
		double kx = l1 / (l0+l1);
		double ky = l0 / (l0+l1);
		pa = kx*p[ids[id0]] + ky*p[ids[id1]];
		pb = kx*q[ids[id0]] + ky*q[ids[id1]];
		dist = kx*w[ids[id0]].z + ky*w[ids[id1]].z;
		normal = w2z.Conjugated() * v[ids[3]];
	}else{
		Matrix2d m;
		m.Ex() = w[ids[0]].XY()-w[ids[1]].XY();
		m.Ey() = w[ids[0]].XY()-w[ids[2]].XY();
		double det = m.det();
		Vec2d k;
		if (-epsilon2 < det && det < epsilon2){
			k = Vec2f(0.5, 0.5);
		}else{
			k = m.inv() * w[ids[0]].XY();
		}
		double kz = 1-k.x-k.y;
		pa = k.x*p[ids[1]] + k.y*p[ids[2]] + kz*p[ids[0]];
		pb = k.x*q[ids[1]] + k.y*q[ids[2]] + kz*q[ids[0]];
		dist = k.x*w[ids[1]].z + k.y*w[ids[2]].z + kz*w[ids[0]].z;
		normal = w2z.Conjugated() * v[ids[3]];
	}
	//	HASE_REPORT
	//	DSTR << "CCDGJK dist:" << dist << "  " << pa << pb << std::endl;
	static bool bSave = false;
	if (bSave){
		ContFindCommonPointSaveParam(a, b, a2w, b2w, rangeOrg, normal, pa, pb, dist);
	}
	return 1;
}
#else	//	���ʂ�GJK

#endif

#define USE_NON_CONTINUOUS_DETECTORS
#ifdef USE_NON_CONTINUOUS_DETECTORS
/*	GJK�A���S���Y��(FindCommonPoint(), FindClosestPoint())�̎����D
	SOLID�̃\�[�X���Q�l�ɂ��ď����������ł��D
	FindClosestPoint�͎g���Ă��܂���D
	
	FindCommonPoint�́C
		CDShapePair::Detect()
		CDContactAnalysis::CalcNormal(CDShapePair* cp)
	�Ŏg���Ă��܂��D�y�i���e�B�@���g���ƌĂяo����܂��D	*/

//	�ߋ���SupportPoint������`��(�_�E�����E3�p�`�C4�ʑ�)�D�ő�4�_�܂ŁD
static int usedBits;		//	����4�r�b�g���C�ǂ�4�_�Ō��݂̌`�󂪍\������邩��\���D
static int lastId;			//	�Ō�Ɍ�����SupportPoint�����܂����ꏊ(0�`3)
static int lastBit;			//	lastBit = 1<<lastId (�r�b�g�ŕ\�������́j
static int allUsedBits;		//	usedBits��lastBit������������
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

	//	�V�����������_(lastId)�ɂ��āC���ς��v�Z
	for (int i = 0, curPoint = 1; i < 4; ++i, curPoint <<=1){
		if (usedBits & curPoint) 
			dotp[i][lastId] = dotp[lastId][i] = p_q[i] * p_q[lastId];
	}
	dotp[lastId][lastId] = p_q[lastId] * p_q[lastId];

	det[lastBit][lastId] = 1;
	for (int j = 0, sj = 1; j < 4; ++j, sj <<= 1) {
		if (usedBits & sj) {		
			int s2 = sj|lastBit;	//	�V�����������_�ɂ��ČW���̌v�Z
			det[s2][j] = dotp[lastId][lastId] - dotp[lastId][j];	//	a^2-ab
			det[s2][lastId] = dotp[j][j] - dotp[j][lastId];			//	b^2-ab
			for (int k = 0, sk = 1; k < j; ++k, sk <<= 1) {	//	3�_�̏ꍇ
				if (usedBits & sk) {
					int s3 = sk|s2;
					det[s3][k] = det[s2][j] * (dotp[j][j] - dotp[j][k]) + 
						det[s2][lastId] * (dotp[lastId][j] - dotp[lastId][k]);
					det[s3][j] = det[sk|lastBit][k] * (dotp[k][k] - dotp[k][j]) + 
						det[sk|lastBit][lastId] * (dotp[lastId][k] - dotp[lastId][j]);
					det[s3][lastId] = det[sk|sj][k] * (dotp[k][k] - dotp[k][lastId]) + 
						det[sk|sj][j] * (dotp[j][k] - dotp[j][lastId]);
				}
			}
		}
	}
	if (allUsedBits == 15) {	//	4�_�̏ꍇ
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


//	�W������C�ŋߖT�_ v ���v�Z
inline void CalcVector(int usedBits, Vec3d& v) {
	double sum = 0;
	v.clear();
	for (int i = 0; i < 4; ++i) {
		if (usedBits & (1<<i)) {
			sum += det[usedBits][i];
			v += p_q[i] * det[usedBits][i];
		}
	}
	v *= 1 / sum;
}

//	�W������C�ŋߖT�_�ƁC����2�̐}�`��ł́C���̓_�̈ʒu���v�Z
inline void CalcPoints(int usedBits, Vec3d& p1, Vec3d& p2) {
	double sum = 0;
	p1.clear();
	p2.clear();
	for (int i = 0, curPoint = 1; i < 4; ++i, curPoint <<= 1) {
		if (usedBits & curPoint) {
			sum += det[usedBits][i];
			p1 += p[i] * det[usedBits][i];
			p2 += q[i] * det[usedBits][i];
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

//	�ŋߖT�_��Ԃ��D
inline bool CalcClosest(Vec3d& v) {
	CalcDet();
	if (!usedBits){
		usedBits = lastBit;
		v = p_q[lastId];
		return true;
	}
	int simplex[5][4];
	int nSimplex[5];
	const char numVertices[] = {
		0, 1, 1, 2, 1, 2, 2, 3,
		1, 2, 2, 3, 2, 3, 3, 4
	};
	int nVtx = numVertices[allUsedBits];
	simplex[nVtx][0] = allUsedBits;
	nSimplex[nVtx] = 1;
	
	for(; nVtx>1; --nVtx){
		for(int sid=0; sid<nSimplex[nVtx]; ++sid){
			int s = simplex[nVtx][sid];
			nSimplex[nVtx-1]=0;
			for(int i = 0; i!=4; ++i){
				int bit = 1<<i;
				if ((s&bit) && det[s][i] <= 0){
					simplex[nVtx-1][nSimplex[nVtx-1]] = s & ~bit;
					++ nSimplex[nVtx-1];
				}
			}
			if (nSimplex[nVtx-1] == 0){
				usedBits = s;
				CalcVector(usedBits, v);	//	�ŋߖT�_���v�Z���ĕԂ��D			
				return true;
			}
		}
	}
	return false;
}

//	�V�����_w���C���܂ł̓_�Ɠ������ꍇ
inline bool HasSame(const Vec3d& w) {
	for (int i = 0; i < 4; ++i){
		if ((allUsedBits & (1<<i)) && (p_q[i]-w).square() < epsilon2) return true;
	}
	return false;
}
inline char VacantIdFromBits(char bits){
	return vacants[(int)bits];
}

bool FASTCALL FindCommonPoint(const CDConvex* a, const CDConvex* b,
	const Posed& a2w, const Posed& b2w, Vec3d& v, Vec3d& pa, Vec3d& pb) {
	Vec3d w;

	usedBits = 0;
	allUsedBits = 0;
	int count = 0;
	do {
		lastId = VacantIdFromBits(usedBits);
		lastBit = 1 << lastId;

		p[lastId] = a->Support(a2w.Ori().Conjugated() * (-v));
		q[lastId] = b->Support(b2w.Ori().Conjugated() * v);
		w = a2w * p[lastId]  -  b2w * q[lastId];
		if (v*w > 0) return false;			//	���_��support�ʂ̊O��
		if (HasSame(w)) return false;		//	support��1�_�ɏW�������_�͊O�ɂ���D
		p_q[lastId] = w;					//	�V�����_����
		allUsedBits = usedBits|lastBit;//	�g�p�����_���X�g�ɒǉ�
		if (!CalcClosest(v)) return false;

		count ++;
		if (count > 100){
			Vec3d vtxs[3];
			Vec3d notUsed[3];
			int nUsed=0, nNotUsed=0;
			for(int i=0; i<4; ++i){
				if (usedBits & (1<<i)){
					vtxs[nUsed] = p_q[i];
					nUsed++;
				}else{
					notUsed[nNotUsed] = p_q[i];
					nNotUsed++;
				}
			}
			if (nUsed == 3 && allUsedBits == 15){
				Vec3d n = (vtxs[2]-vtxs[0]) ^ (vtxs[1]-vtxs[0]);
				n.unitize();
				if (n * (notUsed[0] - vtxs[0]) > 0){
					n *= -1;
				}
				double dist = n * (-vtxs[0]);
/*				DSTR << "n:\t" << nUsed << "Dist:\t" << dist << std::endl;
				DSTR << "v:\t" << v.x << "\t" << v.y << "\t" << v.z << std::endl; 
				DSTR << "Normal:\t" << n.x << "\t" << n.y << "\t" << n.z << std::endl;
				for(int i=0; i<nUsed; ++i){
					DSTR << "Use:\t" << vtxs[i].x << "\t" << vtxs[i].y << "\t" << vtxs[i].z << std::endl;
				}
				for(int i=0; i<nNotUsed; ++i){
					DSTR << "Not:\t" << notUsed[i].x << "\t" << notUsed[i].y << "\t" << notUsed[i].z << std::endl;
				}
				*/
			}else{
				/*	DSTR << "n:\t" << nUsed << std::endl;
				DSTR << "v:\t" << v.x << "\t" << v.y << "\t" << v.z << std::endl;
				for(int i=0; i<nUsed; ++i){
					DSTR << "Use:\t" << vtxs[i].x << "\t" << vtxs[i].y << "\t" << vtxs[i].z << std::endl;
				}
				for(int i=0; i<nNotUsed; ++i){
					DSTR << "Not:\t" << notUsed[i].x << "\t" << notUsed[i].y << "\t" << notUsed[i].z << std::endl;
				}
				*/
			}
		}
	} while ( usedBits < 15 && !(v.square() < epsilon2) ) ;
	CalcPoints(usedBits, pa, pb);
	return true;
}


#define USE_FIND_CLOSEST_POINT
#ifdef USE_FIND_CLOSEST_POINT

inline bool IsDegenerate(const Vec3d& w) {
	for (int i = 0, curPoint = 1; i < 4; ++i, curPoint <<= 1){
//		if ((allUsedBits & curPoint) && (p_q[i]-w).square() < 1e-6){
		if ((allUsedBits & curPoint) && (p_q[i]-w).square() < epsilon2){
			return true;
		}
	}
	return false;
}
void FASTCALL FindClosestPoints(const CDConvex* a, const CDConvex* b,
					  const Posed& a2w, const Posed& b2w,
					  Vec3d& pa, Vec3d& pb) {
	Vec3d v; 				
	v = a2w * a->Support(Vec3d()) - b2w * b->Support(Vec3d());	
	double dist = v.norm();	
	Vec3d w;				
	double maxSupportDist = 0.0f;

	usedBits = 0;
	allUsedBits = 0;

	while (usedBits < 15 && dist > epsilon) {
		lastId = 0;
		lastBit = 1;
		while (usedBits & lastBit) { ++lastId; lastBit <<= 1; }
		p[lastId] = a->Support(a2w.Ori().Conjugated() * (-v));
		q[lastId] = b->Support(b2w.Ori().Conjugated() * v);
		w = a2w * p[lastId]  -  b2w * q[lastId];
		double supportDist = w*v/dist;
		if (maxSupportDist < supportDist) maxSupportDist= supportDist;
		if (dist - maxSupportDist <= dist * epsilon) break;
		if (IsDegenerate(w)) break;
		p_q[lastId] = w;
		allUsedBits = usedBits|lastBit;
		if (!CalcClosest(v)) break;
		dist = v.norm();
	}
	CalcPoints(usedBits, pa, pb);
}
#endif
#endif

}
