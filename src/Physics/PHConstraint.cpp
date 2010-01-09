/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include "Physics.h"
#include <Base/VectorDebug.h>
#pragma hdrstop

using namespace std;
using namespace PTM;
namespace Spr{;

//----------------------------------------------------------------------------
// PHConstraint
//�R���X�g���N�^
PHConstraint::PHConstraint(){
	solid[0] = solid[1] = NULL;
	f.clear();
	F.clear();
	bEnabled = true;
	bInactive[0] = true;
	bInactive[1] = true;
	bArticulated = false;	
}

PHSceneIf* PHConstraint::GetScene() const{
	return DCAST(PHSceneIf, SceneObject::GetScene());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
// �C���^�t�F�[�X(PHConstraintIf�̋@�\)�̎���,�I�[�o�[���C�h   cf.SprPHJoint.h

bool PHConstraint::AddChildObject(ObjectIf* o){
	PHSolid* s = DCAST(PHSolid, o);
	if(s){
		//PHSolids::iterator it = (PHSolids::iterator) GetScene()->constraintEngine->solids.Find(s);
		//if(it == GetScene()->constraintEngine->solids.end())
		//	return false;
		if(!solid[0]){
			solid[0] = s;
			return true;
		}
		if(!solid[1]){
			solid[1] = s;
			return true;
		}
	}
	return false;
}
size_t PHConstraint::NChildObject(){
	return (solid[0] ? 1 : 0) + (solid[1] ? 1 : 0);
}
ObjectIf* PHConstraint::GetChildObject(size_t i){
	return solid[i]->Cast();
}

void PHConstraint::AfterSetDesc(){
	SceneObject::AfterSetDesc();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
// ���̃N���X���̋@�\.

void PHConstraint::UpdateState(){
	// ���̂̑��Έʒu���烄�R�r�A���C�֐ߑ��x�E�ʒu���t�Z����
	CompJacobian();
	//�e���̂̒��S���猩�����x�ƁC�q���̂̒��S���猩�����x��Socket���W�n���猩�����x�ɗ��������āC���Α��x�����D
	vjrel = Js[1] * solid[1]->v - Js[0] * solid[0]->v;
	UpdateJointState();
}


void PHConstraint::CompJacobian(){ // �S������2�̍��̂̊e���x���瑊�Α��x�ւ̃��R�r�A�����v�Z
	// Xj[i] : ���̂̎��ʒ��S����\�P�b�g/�v���O�ւ̕ϊ�
	Xj[0].r    = poseSocket.Pos() - solid[0]->center;
	Xj[0].q    = poseSocket.Ori();
	Xj[1].r    = posePlug.Pos() - solid[1]->center;
	Xj[1].q	   = posePlug.Ori();

	X[0].r = solid[0]->GetCenterPosition();
	X[0].q = solid[0]->GetOrientation();
	X[1].r = solid[1]->GetCenterPosition();
	X[1].q = solid[1]->GetOrientation();
	Xjrel =  Xj[1] * X[1] * X[0].inv() * Xj[0].inv();

	Js[0] = Xj[0];
	Js[1] = Xjrel.inv() * Xj[1];
	J[0] = Js[0];
	J[0] *= -1.0;	//����p
	J[1] = Js[1];
	
	/*
	//�p���x�̍����炩�����quaternion�̎��Ԕ�����������s��
	Matrix3d E(
		 qjrel.W(),  qjrel.Z(), -qjrel.Y(),
		-qjrel.Z(),  qjrel.W(),  qjrel.X(),
		 qjrel.Y(), -qjrel.X(),  qjrel.W());
	E *= 0.5;
	Jqv[0].clear();
	Jqw[0] = E * Jww[0];
	Jqv[1].clear();
	Jqw[1] = E * Jww[1];
	*/
}

/*	A�̑Ίp�������v�Z����DA = J * M^-1 * J^T
	A�s��͍S���͂��瑬�x�ω��ւ̉e���̋�����\���s��Ȃ̂ŁC
	���̑Ίp�����͂���S���͐������玩�����g�̍S�����x�����ւ̉e���̋�����\��
 */
void PHConstraint::CompResponseMatrix(){
	int i, j;
	A.clear();
	PHRootNode* root[2];
	if(solid[0]->IsArticulated())
		root[0] = solid[0]->treeNode->GetRootNode();
	if(solid[1]->IsArticulated())
		root[1] = solid[1]->treeNode->GetRootNode();

	SpatialVector df;
	for(i = 0; i < 2; i++){
		if(solid[i]->IsDynamical()){
			if(solid[i]->IsArticulated()){
				for(j = 0; j < 6; j++){
					(Vec6d&)df = J[i].row(j);
					solid[i]->treeNode->CompResponse(df, false, false);
					A[j] += J[i].row(j) * solid[i]->treeNode->da;
					int ic = !i;
					//�����Е��̍��̂�����̃c���[�ɑ�����ꍇ�͂��̉e���������Z
					if(solid[ic]->IsArticulated() && root[i] == root[ic])
						A[j] += J[ic].row(j) * solid[ic]->treeNode->da;
				}
			}
			else{
				// T = M^-1 * J^T
				T[i].vv() = J[i].vv() * solid[i]->minv;
				T[i].vw() = J[i].vw() * solid[i]->Iinv;
				T[i].wv() = J[i].wv() * solid[i]->minv;
				T[i].ww() = J[i].ww() * solid[i]->Iinv;
				for(j = 0; j < 6; j++)
					// A == �_������J * M^-1 * J^T, Gauss Seidel�@��D
					A[j] += J[i].row(j) * T[i].row(j);
			}
		}
	}
	/** �ő�̑Ίp�v�f�Ƃ̔䂪eps�����������Ίp�v�f������ꍇ�C
		���l�I�s���萫�̌����ƂȂ�̂ł��̐����͍S���Ώۂ��珜�O����
		��eps��傫���Ƃ�ƁC�K�v�ȍS���܂Ŗ���������Ă��܂��̂ŁA�����͐T�d�ɁB
	 */
	const double eps = 0.000001, epsabs = 1.0e-10;
	double Amax = 0.0, Amin;
	for(j = 0; j < 6; j++)
		if(constr[j] && A[j] > Amax)
			Amax = A[j];
	Amin = Amax * eps;
	for(j = 0; j < 6; j++){
		if(!constr[j])continue;
		if(A[j] < Amin || A[j] < epsabs){
			constr[j] = false;
			DSTR <<this->GetName()<<":"<< j << "-th constraint ill-conditioned! disabled." << endl;
		}
		else
			Ainv[j] = 1.0 / (A[j] + dA[j]);
	}
}

void PHConstraint::CompResponse(double df, int j){
	SpatialVector dfs;
	for(int i = 0; i < 2; i++){
		if(!solid[i]->IsDynamical() || !IsInactive(i))continue;
		if(solid[i]->IsArticulated()){
			(Vec6d&)dfs = J[i].row(j) * df;
			solid[i]->treeNode->CompResponse(dfs, true, false);
		}
		else solid[i]->dv += T[i].row(j) * df;
	}
}

void PHConstraint::SetupLCP(){
	bFeasible = solid[0]->IsDynamical() || solid[1]->IsDynamical();
	if(!bEnabled || !bFeasible)
		return;

	/* �S���͂͑O��̒l���k���������̂������l�Ƃ���D
	   �O��̒l���̂܂܂������l�ɂ���ƁC�S���͂�����ɑ��傷��Ƃ������ۂ�������D
	   ����́CLCP��L����i���ۂɂ�10����x�j�̔����őł��؂邽�߂��Ǝv����D
	   0�x�N�g���������l�ɗp���Ă��ǂ����C���̏ꍇ��r�I�����̔����񐔂�v����D
	  */
	
	// �S�����鎩�R�x�̌���C�S���͂̏�����
	//bool con[6];
	SetConstrainedIndex(constr);
	for(int i = 0; i < 6; i++){
		//if(con[i] && constr[i]){				// �p�����čS�������ꍇ
			f[i] *= engine->shrinkRate;
		//}else{
		//	f[i] = 0.0;							// �V�K�ɍS������� or �S������Ȃ�
		//}
		//constr[i] = con[i];
	}

	FPCK_FINITE(f.v());

	// LCP�̍��W�̎���������Ȋ֐߂̓��R�r�A���ɍ��W�ϊ���������
	ModifyJacobian();

	// LCP�̌W��A, b�̕␳�ldA, db���v�Z
	dA.clear();
	db.clear();
	
	// LCP��A�s��̑Ίp�������v�Z
	CompResponseMatrix();

	// ABA�̏ꍇ�͂����܂�
	if(bArticulated)return;

	CompBias();	// �덷�C���̂��߂̕␳�l�D
	
	// LCP��b�x�N�g�� == �_������w[t], �o�l�E�_���p��db�ŕ␳����
	b = J[0] * solid[0]->v + J[1] * solid[1]->v;	//vjrel�ł͂Ȃ�	�l�ɂ���Ă͕ς���čS�����Ȃ��Ȃ�

	// �S���͏����l�ɂ�鑬�x�ω��ʂ��v�Z
	SpatialVector fs;
	for(int i = 0; i < 2; i++){
		if(!solid[i]->IsDynamical() || !IsInactive(i))continue;
		if(solid[i]->IsArticulated()){
			(Vec6d&)fs = J[i].trans() * f;
			solid[i]->treeNode->CompResponse(fs, true, false);
		}
		else solid[i]->dv += T[i].trans() * f;
	}

}

void PHConstraint::IterateLCP(){
	if(!bEnabled || !bFeasible || bArticulated)
		return;
	FPCK_FINITE(f.v());

	SpatialVector fnew, df;
	for(int j = 0; j < 6; j++){
		if(!constr[j])continue;
		
		fnew[j] = f[j] - engine->accelSOR * Ainv[j] * (dA[j] * f[j] + b[j] + db[j] 
				+ J[0].row(j) * solid[0]->dv + J[1].row(j) * solid[1]->dv);

		// �Ƃ肠���������Ȃ��悤�ɊԂɍ��킹�̃R�[�h
		if (!FPCK_FINITE(fnew[j])) fnew[j] = f[j]; //naga ����������ł͊Ԃɍ��킹�̃R�[�h�ł�������

		if (!FPCK_FINITE(fnew[0])){
			FPCK_FINITE(b[0]);
//			DSTR << AinvJ[0].vv << AinvJ[1].vv;
//			DSTR << AinvJ[0].vw << AinvJ[1].vw;
//			DSTR << dA.v[j];
//			DSTR << std::endl;
//			DSTR << "f.v:" << f.v << "b.v:" << b.v << std::endl;
			DSTR << "s0:" << (solid[0]->dv) << std::endl;
			DSTR << "s1:" << (solid[1]->dv)  << std::endl;
		}
		Projection(fnew[j], j);
		df[j] = fnew[j] - f[j];
		CompResponse(df[j], j);
		f[j] = fnew[j];
	}
}

void PHConstraint::SetupCorrectionLCP(){
	if(!bEnabled || !bFeasible || bArticulated)
		return;
	//	�S�����鎩�R�x�̌���
	//bool con[6];
	//SetConstrainedIndexCorrection(con);
	for(int i = 0; i < 6; i++){
		//if(con[i] && constrCorrection[i]){		// �p�����čS�������ꍇ
			 F[i] *= engine->shrinkRateCorrection;
		//}else{
		//	F[i] = 0.0;							// �V�K�ɍS������� or �S������Ȃ�
		//}
		//constrCorrection[i] = con[i];
	}
	B.clear();
	CompError();
	
	// velocity update�ɂ��e�������Z
	B += (J[0] * (solid[0]->v + solid[0]->dv)
			+ J[1] * (solid[1]->v + solid[1]->dv)) * GetScene()->GetTimeStep();
	B *= engine->posCorrectionRate;
		
	// �S���͏����l�ɂ��ʒu�ω��ʂ��v�Z
	SpatialVector Fs;
	for(int i = 0; i < 2; i++){
		if(!solid[i]->IsDynamical() || !IsInactive(i))continue;
		if(solid[i]->IsArticulated()){
			(Vec6d&)Fs = J[i].trans() * F;
			solid[i]->treeNode->CompResponse(Fs, true, true);
		}
		else solid[i]->dV += T[i].trans() * F;
	}
}

void PHConstraint::IterateCorrectionLCP(){
	if(!bEnabled || !bFeasible || bArticulated)
		return;
	
	SpatialVector Fnew, dF, dFs;
	int i, j;
	for(j = 0; j < 6; j++){
		if(!constrCorrection[j]) continue;
		Fnew[j] = F[j] - Ainv[j] * (B[j] + J[0].row(j) * solid[0]->dV + J[1].row(j) * solid[1]->dV);
		ProjectionCorrection(Fnew[j], j);
		dF[j] = Fnew[j] - F[j];
		for(i = 0; i < 2; i++){
			if(!solid[i]->IsDynamical() || !IsInactive(i))continue;
			if(solid[i]->IsArticulated()){
				(Vec6d&)dFs = J[i].row(j) * dF[j];
				solid[i]->treeNode->CompResponse(dFs, true, true);			
			}
			else solid[i]->dV += T[i].row(j) * dF[j];
		}
		F[j] = Fnew[j];
	}
}

void PHConstraint::GetRelativeVelocity(Vec3d &v, Vec3d &w){
	for(int i = 0; i < 2; i++){
		solid[i]->UpdateCacheLCP(GetScene()->GetTimeStep());
	}
	UpdateState();
	v = vjrel.v(); 
	w = vjrel.w();
}

void PHConstraint::GetConstraintForce(Vec3d& _f, Vec3d& _t){
	_f = f.v() / GetScene()->GetTimeStep();
	_t = f.w() / GetScene()->GetTimeStep();
}

}