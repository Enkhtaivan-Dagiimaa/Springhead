/*
 *  Copyright (c) 2003-2010, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include <Physics/PHConstraint.h>
#include <Physics/PHTreeNode.h>
#include <Physics/PHConstraintEngine.h>

using namespace std;
using namespace PTM;
namespace Spr{;

// -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  ----- 
// PHConstraint

// �R���X�g���N�^
PHConstraint::PHConstraint() {
	solid[0] = solid[1] = NULL;

	f.clear();
	F.clear();

	bEnabled = true;
	bInactive[0] = true;
	bInactive[1] = true;
	bArticulated = false;
	bProhibitUpdateSolidCacheLCP = false;

	for(int i=0; i<6; i++){
		fMaxDt[i] =  FLT_MAX;
		fMinDt[i] = -FLT_MAX;
	}
}

void PHConstraint::InitTargetAxes() {
	// movableAxes�̎c���targetAxes�����D
	nTargetAxes = 0;
	for (int i=0; i<6; ++i) {
		bool bMovable = false;
		for (int n=0; n<nMovableAxes; ++n) { if (i==movableAxes[n]) { bMovable = true; }  }
		if (bMovable) { continue; }
		targetAxes[nTargetAxes++] = i;
	}
}

// ----- �G���W������Ăяo�����֐�

void PHConstraint::UpdateState() {
	// ���̂̏�Ԃ��X�V����i�����ł��ׂ����͗v�����I���Ȃ��Ƃ������ȑO��Update����ĂȂ��Ɛ������l���o�Ȃ� <!!>�j
	if (!bProhibitUpdateSolidCacheLCP) {
		for (int i=0; i<2; i++) { solid[i]->UpdateCacheLCP(GetScene()->GetTimeStep()); }
	}

	// ���̂̑��Έʒu���烄�R�r�A���C�֐ߑ��x�E�ʒu���t�Z����
	CompJacobian();

	// �e���̂̒��S���猩�����x�ƁC�q���̂̒��S���猩�����x��
	// Socket���W�n���猩�����x�ɗ��������āC���Α��x�����D
	vjrel = Js[1] * solid[1]->v - Js[0] * solid[0]->v;

	// �֐ߍ��W�̈ʒu�E���x���X�V����
	UpdateJointState();
}

void PHConstraint::SetupLCP() {
	bProhibitUpdateSolidCacheLCP = true;

	// �����\�ȍS���ł��邩
	bFeasible = solid[0]->IsDynamical() || solid[1]->IsDynamical();
	if(!bEnabled || !bFeasible) { return; }
	
	// �S�����t���O�̃N���A <<��������axes.CreateList()�܂�axes[n]�͎g���Ȃ��DEnable/Disable/IsEnabled�̂�>>
	axes.Clear();

	// Projection�p�̍ő�E�ŏ��l�����Z�b�g����
	for (int i=0; i<6; i++) { fMinDt[i] = -FLT_MAX; fMaxDt[i] =  FLT_MAX; }

	// �S�����鎩�R�x�̌���
	SetupAxisIndex();

	// LCP�̍��W�̎���������Ȋ֐߂̓��R�r�A���ɍ��W�ϊ���������
	ModifyJacobian();

	// LCP�̌W��A, b�̕␳�ldA, db���v�Z
	dA.clear();
	db.clear();
	CompBias();

	// LCP��A�s��̑Ίp�������v�Z
	CompResponseMatrix();

	// LCP��b�x�N�g�� == �_������w[t]���v�Z
	b = J[0] * solid[0]->v + J[1] * solid[1]->v;  //vjrel�ł͂Ȃ� �l�ɂ���Ă͕ς���čS�����Ȃ��Ȃ�

	// �����܂łŌ��肳�ꂽ�S�����t���O���g���Ď��ԍ����X�g���쐬�@<<���������axes[n]���g�p�\>>
	axes.CreateList();

	// �S���͂̏����l���X�V
	//   �S���͂͑O��̒l���k���������̂������l�Ƃ���D
	//   �O��̒l���̂܂܂������l�ɂ���ƁC�S���͂�����ɑ��傷��Ƃ������ۂ�������D
	//   ����́CLCP��L����i���ۂɂ�10����x�j�̔����őł��؂邽�߂��Ǝv����D
	//   0�x�N�g���������l�ɗp���Ă��ǂ����C���̏ꍇ��r�I�����̔����񐔂�v����D
	for (int n=0; n<axes.size(); ++n) {
		f[axes[n]] *= axes.IsContinued(axes[n]) ? engine->shrinkRate : 0;
	}

	// �S���͏����l�ɂ�鑬�x�ω��ʂ��v�Z
	SpatialVector fs;
	for (int i=0; i<2; ++i) {
		if (!solid[i]->IsDynamical() || !IsInactive(i)) { continue; }
		if (solid[i]->IsArticulated()) {
			(Vec6d&)fs = J[i].trans() * f;
			solid[i]->treeNode->CompResponse(fs, true, false);
		} else {
			solid[i]->dv += T[i].trans() * f;
		}
	}
}

void PHConstraint::IterateLCP() {
	if (!bEnabled || !bFeasible) { return; }
	
	SpatialVector fnew, df;

	for (int n=0; n<axes.size(); ++n) {
		// Gauss-Seidel Iteration
		int i = axes[n];
		fnew[i] = f[i] - engine->accelSOR * Ainv[i] * (dA[i]*f[i] + b[i] + db[i] 
				+ J[0].row(i)*solid[0]->dv + J[1].row(i)*solid[1]->dv);

		// Projection
		Projection(fnew[i], i);

		// Comp Response & Update f
		df[i] = fnew[i] - f[i];
		CompResponse(df[i], i);
		f[i] = fnew[i];
	}
}

void PHConstraint::SetupCorrectionLCP() {
	if(!bEnabled || !bFeasible || bArticulated) { return; }

	for (int n=0; n<axes.size(); ++n) {
		F[axes[n]] *= engine->shrinkRateCorrection;
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
		if (!solid[i]->IsDynamical() || !IsInactive(i)) { continue; }
		if (solid[i]->IsArticulated()) {
			(Vec6d&)Fs = J[i].trans() * F;
			solid[i]->treeNode->CompResponse(Fs, true, true);
		} else {
			solid[i]->dV += T[i].trans() * F;
		}
	}
}

void PHConstraint::IterateCorrectionLCP() {
	if(!bEnabled || !bFeasible || bArticulated) { return; }
	
	SpatialVector Fnew, dF, dFs;

	for (int n=0; n<axes.size(); ++n) {
		int k = axes[n];
		Fnew[k] = F[k] - Ainv[k] * (B[k] + J[0].row(k) * solid[0]->dV + J[1].row(k) * solid[1]->dV);
		ProjectionCorrection(Fnew[k], k);
		dF[k] = Fnew[k] - F[k];
		for (int i=0; i<2; i++) {
			if(!solid[i]->IsDynamical() || !IsInactive(i))continue;
			if(solid[i]->IsArticulated()){
				(Vec6d&)dFs = J[i].row(k) * dF[k];
				solid[i]->treeNode->CompResponse(dFs, true, true);			
			} else {
				solid[i]->dV += T[i].row(k) * dF[k];
			}
		}
		F[k] = Fnew[k];
	}
}

// ----- ���̃N���X�Ŏ�������@�\

void PHConstraint::CompJacobian() {
	// �S������2�̍��̂̊e���x���瑊�Α��x�ւ̃��R�r�A�����v�Z
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
}

void PHConstraint::CompResponseMatrix() {
	// A�̑Ίp�������v�Z����DA = J * M^-1 * J^T
	// A�s��͍S���͂��瑬�x�ω��ւ̉e���̋�����\���s��Ȃ̂ŁC
	// ���̑Ίp�����͂���S���͐������玩�����g�̍S�����x�����ւ̉e���̋�����\��

	A.clear();
	PHRootNode* root[2] = {
		solid[0]->IsArticulated() ? solid[0]->treeNode->GetRootNode() : NULL,
		solid[1]->IsArticulated() ? solid[1]->treeNode->GetRootNode() : NULL,
	};

	SpatialVector df;
	for (int i=0; i<2; ++i) {
		if (solid[i]->IsDynamical()) {
			if (solid[i]->IsArticulated()) {
				// -- ABA --
				for (int j=0; j<6; ++j) {
					(Vec6d&)df = J[i].row(j);
					solid[i]->treeNode->CompResponse(df, false, false);
					A[j] += J[i].row(j) * solid[i]->treeNode->da;
					//�����Е��̍��̂�����̃c���[�ɑ�����ꍇ�͂��̉e���������Z
					if(solid[!i]->IsArticulated() && root[i] == root[!i]) {
						A[j] += J[!i].row(j) * solid[!i]->treeNode->da;
					}
				}
			} else {
				// -- LCP --
				// T = M^-1 * J^T
				T[i].vv() = J[i].vv() * solid[i]->minv;
				T[i].vw() = J[i].vw() * solid[i]->Iinv;
				T[i].wv() = J[i].wv() * solid[i]->minv;
				T[i].ww() = J[i].ww() * solid[i]->Iinv;

				// A == �_������J * M^-1 * J^T, Gauss Seidel�@��D
				for(int j=0; j<6; ++j) {
					A[j] += J[i].row(j) * T[i].row(j);
				}
			}
		}
	}

	// �ő�̑Ίp�v�f�Ƃ̔䂪eps�����������Ίp�v�f������ꍇ�C
	// ���l�I�s���萫�̌����ƂȂ�̂ł��̐����͍S���Ώۂ��珜�O����
	// ��eps��傫���Ƃ�ƁC�K�v�ȍS���܂Ŗ���������Ă��܂��̂ŁA�����͐T�d�ɁB
	const double eps = 1.0e-6, epsabs = 1.0e-10;
	double Amax=0.0, Amin;
	for(int i=0; i<6; ++i) {
		if(axes.IsEnabled(i) && A[i] > Amax) { Amax = A[i]; }
	}
	Amin = Amax * eps;

	for(int i=0; i<6; ++i) {
		if (axes.IsEnabled(i)) {
			if(A[i] < Amin || A[i] < epsabs){
				axes.Disable(i);
				DSTR << this->GetName() << ": Axis " << i << " ill-conditioned! Disabled.  A[" << i << "]= " << A[i] << endl;
			} else {
				Ainv[i] = 1.0 / (A[i] + dA[i]);
			}
		}
	}
}

void PHConstraint::CompResponse(double df, int i) {
	SpatialVector dfs;
	for (int k=0; k<2; ++k) {
		if (!solid[k]->IsDynamical() || !IsInactive(k)) { continue; }
		if (solid[k]->IsArticulated()) {
			(Vec6d&)dfs = J[k].row(i) * df;
			solid[k]->treeNode->CompResponse(dfs, true, false);
		} else {
			solid[k]->dv += T[k].row(i) * df;
		}
	}
}

// ----- �{���͔h���N���X�Ŏ�������@�\�́C�f�t�H���g����

void PHConstraint::SetupAxisIndex() {
	if (!bArticulated) {
		for (int n=0; n<nTargetAxes; ++n) {
			axes.Enable(targetAxes[n]);
		}
	}
}

/// �S���͂̎ˉe
void PHConstraint::Projection(double& f_, int i) {
	f_ = min(max(fMinDt[i], f_), fMaxDt[i]);
}

// ----- �C���^�t�F�[�X�̎���

void PHConstraint::GetRelativeVelocity(Vec3d &v, Vec3d &w) {
	for (int i=0; i<2; i++) {
		solid[i]->UpdateCacheLCP(GetScene()->GetTimeStep());
	}
	UpdateState();
	v = vjrel.v(); 
	w = vjrel.w();
}

void PHConstraint::GetConstraintForce(Vec3d& _f, Vec3d& _t) {
	_f = f.v() / GetScene()->GetTimeStep();
	_t = f.w() / GetScene()->GetTimeStep();
}

Vec3d PHConstraint::GetForce() {
	return( f.v() / GetScene()->GetTimeStep() );
}

Vec3d PHConstraint::GetTorque() {
	return( f.w() / GetScene()->GetTimeStep() );
}

bool PHConstraint::AddChildObject(ObjectIf* o) {
	PHSolid* s = DCAST(PHSolid, o);
	if (s) {
		if (!solid[0]) {
			solid[0] = s;
			return true;
		}
		if (!solid[1]) {
			solid[1] = s;
			return true;
		}
	}
	return false;
}

size_t PHConstraint::NChildObject() const {
	return (solid[0] ? 1 : 0) + (solid[1] ? 1 : 0);
}

ObjectIf* PHConstraint::GetChildObject(size_t i) {
	return solid[i]->Cast();
}

void PHConstraint::AfterSetDesc() {
	SceneObject::AfterSetDesc();
}

}
