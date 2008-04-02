/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include "Physics.h"
#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

using namespace PTM;
using namespace std;
namespace Spr{;

//----------------------------------------------------------------------------
// PHContactPoint
PHContactPoint::PHContactPoint(const Matrix3d& local, PHShapePairForLCP* sp, Vec3d p, PHSolid* s0, PHSolid* s1){
	shapePair = sp;
	pos = p;
	solid[0] = s0, solid[1] = s1;

	Vec3d rjabs[2];
	for(int i = 0; i < 2; i++){
		rjabs[i] = pos - solid[i]->GetCenterPosition();	//���̂̒��S����ڐG�_�܂ł̃x�N�g��
	}
	// local: �ڐG�_�̊֐߃t���[�� �́Cx����@��, y,z����ڐ��Ƃ���
	Quaterniond qlocal;
	qlocal.FromMatrix(local);
	for(int i = 0; i < 2; i++){
		(i == 0 ? poseSocket : posePlug).Ori() = Xj[i].q = solid[i]->GetOrientation().Conjugated() * qlocal;
		(i == 0 ? poseSocket : posePlug).Pos() = Xj[i].r = solid[i]->GetOrientation().Conjugated() * rjabs[i];
	}
}

PHContactPoint::PHContactPoint(PHShapePairForLCP* sp, Vec3d p, PHSolid* s0, PHSolid* s1){
	shapePair = sp;
	pos = p;
	solid[0] = s0, solid[1] = s1;

	Vec3d rjabs[2], vjabs[2];
	for(int i = 0; i < 2; i++){
		rjabs[i] = pos - solid[i]->GetCenterPosition();	//���̂̒��S����ڐG�_�܂ł̃x�N�g��
		vjabs[i] = solid[i]->GetVelocity() + solid[i]->GetAngularVelocity() % rjabs[i];	//�ڐG�_�ł̑��x
	}
	//�ڐ��x�N�g��t[0], t[1] (t[0]�͑��Α��x�x�N�g���ɕ��s�ɂȂ�悤�ɂ���)
	Vec3d n, t[2], vjrel, vjrelproj;
	n = shapePair->normal;
	vjrel = vjabs[1] - vjabs[0];
	vjrelproj = vjrel - (n * vjrel) * n;		//���Α��x�x�N�g����@���ɒ������镽�ʂɎˉe�����x�N�g��
	double vjrelproj_norm = vjrelproj.norm();
	if(vjrelproj_norm < 1.0e-10){
		t[0] = n % Vec3d(1.0, 0.0, 0.0);	
		if(t[0].norm() < 1.0e-10)
			t[0] = n % Vec3d(0.0, 1.0, 0.0);
		t[0].unitize();
	}
	else{
		t[0] = vjrelproj / vjrelproj_norm;
	}
	t[1] = n % t[0];
	Matrix3d Rjabs;
	// �ڐG�_�̊֐߃t���[����x��, y����ڐ��Cz����@���Ƃ���
	Rjabs.col(0) = n;
	Rjabs.col(1) = t[0];
	Rjabs.col(2) = t[1];
	Quaterniond qjabs;
	qjabs.FromMatrix(Rjabs);
	for(int i = 0; i < 2; i++){
		(i == 0 ? poseSocket : posePlug).Ori() = Xj[i].q = solid[i]->GetOrientation().Conjugated() * qjabs;
		(i == 0 ? poseSocket : posePlug).Pos() = Xj[i].r = solid[i]->GetOrientation().Conjugated() * rjabs[i];
	}
}

void PHContactPoint::SetConstrainedIndex(bool* con){
	con[0] = con[1] = con[2] = true;
	con[3] = con[4] = con[5] = false;
}

// double PHContactPoint::correctionSpring = 100.0;
// double PHContactPoint::correctionDamper = 10.0;
double PHContactPoint::correctionSpring = 1000.0;
double PHContactPoint::correctionDamper = 1000.0;


void PHContactPoint::CompBias(){
	//	correction���ʒuLCP�ŕʂɍs���ꍇ�́A���x��ύX���Ă̈ʒu�␳�͂��Ȃ��B
	if (engine->numIterContactCorrection) return;
	double dtinv = 1.0 / scene->GetTimeStep();
	double overlap = 0.01;

#if 1
	//	���˕Ԃ�W��: 2���̂̕��ϒl���g��
	double e = 0.5 * (shapePair->shape[0]->material.e + shapePair->shape[1]->material.e);
	
	//	�ڐG�p�� correctionRate
	double contactCorrectionRate = 0;
	//	���x���������ꍇ�́A���˕Ԃ�Ȃ��B
//	if (vjrel[0]*scene->GetTimeStep() > -0.001){
	if (vjrel[0]*scene->GetTimeStep() > -0.1){
		e=0;
		contactCorrectionRate = engine->contactCorrectionRate;
	}
	if (overlap > shapePair->depth) overlap = shapePair->depth;
	db[0] = - contactCorrectionRate * (shapePair->depth - overlap) / scene->GetTimeStep() + e * vjrel[0];
#endif

#if 0
//	db.v.x = 0.1*engine->correctionRate * (-shapePair->depth * dtinv + vjrel.v.x);
	/*	hase	�{���� 1e-3�͈�������
		depth ������Correction������ƐڐG���s�A���ɂȂ�̂ŁCdepth-epsilon�ŗǂ����C
		epsilon���傫������D

		����́C�ڐG�ʂ��l�p�`�ŐڐG�_��4�_����ꍇ�ȂǁC
		4�_�Ԃ̗͂̕��U���܂�ɕs�ψ�ɂȂ��]���n�܂�C
		�ڐG�ʂ��E���̏�Ԃƍ����̏�Ԃ̊Ԃōׂ������U���邱�Ƃ����邽�߁D
		����́C���̏ꍇ�ɂ��ڐG�ʂ̌`���ς��Ȃ��悤�ɂ��邱�ƂŁC���艻���Ă���D

		2���̊Ԃ̐ڐG���ЂƂ̐���Ƃ��Ă���킹��悤�ɂȂ�Ή�������Ǝv���D	
	*/
	double err = (shapePair->depth - 1e-3)*dtinv - 0.2*vjrel.v().x;
	if (err < 0) err = 0;
	if (err){
//		HASE_REPORT
//		DSTR << shapePair->state;
//		DSTR << shapePair->shapePoseW[1].Pos();
//		DSTR << "err: " << err << std::edl;
		db.v().x = -err * engine->velCorrectionRate;
	}
#endif

#if 0
	double tmp = 1.0 / (correctionDamper + correctionSpring * scene->GetTimeStep());
	dA[0] = tmp * dtinv;
	db[0] = -correctionSpring * max(0.0, shapePair->depth - overlap) * tmp;
#endif
}

void PHContactPoint::Projection(double& f, int k){
	static double flim;
	if(k == 0){	//�����R�� >= 0�̐���
		f = max(0.0, f);
		//	�ő�Î~���C
		flim = 0.5 * (shapePair->shape[0]->material.mu0 + shapePair->shape[1]->material.mu0) * f;	}
	else{
		//	�����C�������Ɏ������Ă݂�B
		double fu = (shapePair->shape[0]->material.mu + shapePair->shape[1]->material.mu)
			/ (shapePair->shape[0]->material.mu0 + shapePair->shape[1]->material.mu0)
			* flim;	
		if (-0.01 < vjrel[1] && vjrel[1] < 0.01){	//	�Î~���C
			if (f > flim) f = fu;
			else if (f < -flim) f = -fu;
		}else{					//	�����C
			if (f > fu) f = fu;
			else if (f < -fu) f = -fu;		
		}
#if 0
		//|���C��| <= �ő�Î~���C�̐���
		//	�E���C�͂̊e�������ő�Î~���C�����������Ă����͂͒�����\��������̂Ŗ{���͂��������B
		//	�E�Î~���C�Ɠ����C�������l�łȂ��ƈ����Ȃ��B
		//���C�W���͗��҂̐Î~���C�W���̕��ςƂ���
		f = min(max(-flim, f), flim);
#endif
	}
}

void PHContactPoint::CompError(){
	const double eps = 0.001;
	//�Փ˔���A���S���Y���̓s����ACorrection�ɂ���Ċ��S�ɍ��̂�����Ă��܂��͍̂���̂�
	//�덷��eps����������������
	B.v().x = min(0.0, -shapePair->depth + eps);

	// �덷��S����������ƐU������̂ŁA���Ȃ߂�
	B.v().x *= engine->posCorrectionRate;
}

void PHContactPoint::ProjectionCorrection(double& F, int k){
	if(k == 0){	//�����R�� >= 0�̐���
		F = max(0.0, F);
	}
	else if(k == 1 || k == 2)
		F = 0;
}

}
