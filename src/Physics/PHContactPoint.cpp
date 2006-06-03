#include "Physics.h"
#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

using namespace PTM;
using namespace std;
namespace Spr{;

//----------------------------------------------------------------------------
// PHContactPoint
//OBJECTIMP(PHContactPoint, PHConstraint);
IF_IMP(PHContactPoint, PHConstraint);

PHContactPoint::PHContactPoint(const Matrix3d& local, CDShapePair* sp, Vec3d p, PHSolidAux* s0, PHSolidAux* s1){
	shapePair = sp;
	pos = p;
	solid[0] = s0, solid[1] = s1;

	Vec3d rjabs[2];
	for(int i = 0; i < 2; i++){
		rjabs[i] = pos - solid[i]->solid->GetCenterPosition();	//���̂̒��S����ڐG�_�܂ł̃x�N�g��
	}
	// local: �ڐG�_�̊֐߃t���[�� �́Cx����@��, y,z����ڐ��Ƃ���
	for(int i = 0; i < 2; i++){
		Rj[i] = solid[i]->solid->GetRotation().trans() * local;
		rj[i] = solid[i]->solid->GetRotation().trans() * rjabs[i];
	}
}

PHContactPoint::PHContactPoint(CDShapePair* sp, Vec3d p, PHSolidAux* s0, PHSolidAux* s1){
	shapePair = sp;
	pos = p;
	solid[0] = s0, solid[1] = s1;

	Vec3d rjabs[2], vjabs[2];
	for(int i = 0; i < 2; i++){
		rjabs[i] = pos - solid[i]->solid->GetCenterPosition();	//���̂̒��S����ڐG�_�܂ł̃x�N�g��
		vjabs[i] = solid[i]->solid->GetVelocity() + solid[i]->solid->GetAngularVelocity() % rjabs[i];	//�ڐG�_�ł̑��x
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
	t[1] = t[0] % n;
	Matrix3d Rjabs;
	// �ڐG�_�̊֐߃t���[����x��, y����ڐ��Cz����@���Ƃ���
	Rjabs.col(0) = n;
	Rjabs.col(1) = t[0];
	Rjabs.col(2) = t[1];
	
	for(int i = 0; i < 2; i++){
		Rj[i] = solid[i]->solid->GetRotation().trans() * Rjabs;
		rj[i] = solid[i]->solid->GetRotation().trans() * rjabs[i];
	}
}

void PHContactPoint::CompConstraintJacobian(){
	dim_d = 3;
	dim_c = 1;
	
	Ad.clear();
	for(int i = 0; i < 2; i++){
		Jdv[i].SUBMAT(0, 0, 3, 3) = Jvv[i];
		Jdw[i].SUBMAT(0, 0, 3, 3) = Jvw[i];
		Jcv[i].row(0) = Jdv[i].row(0);
		Jcw[i].row(0) = Jdw[i].row(0);
		if(solid[i]->solid->IsDynamical()){
			Tdv[i].SUBMAT(0, 0, 3, 3) = Jdv[i].SUBMAT(0, 0, 3, 3) * solid[i]->minv;
			Tdw[i].SUBMAT(0, 0, 3, 3) = Jdw[i].SUBMAT(0, 0, 3, 3) * solid[i]->Iinv;
			Tcv[i].row(0) = Tdv[i].row(0);
			Tcw[i].row(0) = Tdw[i].row(0);
			for(int j = 0; j < 3; j++)
				Ad[j] += Jdv[i].row(j) * Tdv[i].row(j) + Jdw[i].row(j) * Tdw[i].row(j);
		}
	}
	Ac[0] = Ad[0];
}

void PHContactPoint::ProjectionDynamics(double& f, int k){
	static double flim;
	if(k == 0){	//�����R�� >= 0�̐���
		f = max(0.0, f);
		flim = 0.5 * (shapePair->shape[0]->shape->material.mu0 + shapePair->shape[1]->shape->material.mu0) * f;	//�ő�Î~���C
	}
	else{
		//|���C��| <= �ő�Î~���C�̐���
		//	�E���C�͂̊e�������ő�Î~���C�����������Ă����͂͒�����\��������̂Ŗ{���͂��������B
		//	�E�Î~���C�Ɠ����C�������l�łȂ��ƈ����Ȃ��B
		//���C�W���͗��҂̐Î~���C�W���̕��ςƂ���
		f = min(max(-flim, f), flim);
	}
}

void PHContactPoint::CompError(double dt){
	const double eps = 0.0;
	//�Փ˔���A���S���Y���̓s����ACorrection�ɂ���Ċ��S�ɍ��̂�����Ă��܂��͍̂���̂�
	//�덷��eps����������������
	B[0] = min(0.0, -shapePair->depth + eps);
}

void PHContactPoint::ProjectionCorrection(double& F, int k){
	//�����R�� >= 0�̐���
	F = Spr::max(0.0, F);
}

}
