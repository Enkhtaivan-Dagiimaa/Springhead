/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include <Physics/PHJointLimit.h>
#include <Physics/PHJoint.h>
#include <Physics/PHBallJoint.h>
#include <Physics/PHConstraintEngine.h>

using namespace PTM;
using namespace std;
namespace Spr{;

// -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  ----- 
// PH1DJointLimit

void PH1DJointLimit::SetupAxisIndex() {
	bOnLimit = false;
	if (range[0] >= range[1]){ return; }

	if(joint->GetPosition() <= range[0]){
		joint->axes.Enable(joint->movableAxes[0]);
		bOnLimit = true;

		diff = joint->GetPosition() - range[0];

		joint->fMaxDt[joint->movableAxes[0]] =  FLT_MAX;
		joint->fMinDt[joint->movableAxes[0]] =  0;
	}

	if(joint->GetPosition() >= range[1]){
		joint->axes.Enable(joint->movableAxes[0]);
		bOnLimit = true;
		
		diff = joint->GetPosition() - range[1];

		joint->fMaxDt[joint->movableAxes[0]] =  0;
		joint->fMinDt[joint->movableAxes[0]] = -FLT_MAX;
	}
}

void PH1DJointLimit::CompBias() {
	if (!bOnLimit) { return; }

	double tmp = 1.0 / (damper + spring * joint->GetScene()->GetTimeStep());

	joint->dA[joint->movableAxes[0]] = tmp * joint->GetScene()->GetTimeStepInv();
	joint->db[joint->movableAxes[0]] = tmp * spring * diff;

	joint->db[joint->movableAxes[0]] += tmp * (
		  joint->GetSpring() * (joint->GetPosition() - joint->GetTargetPosition())
		- joint->GetDamper() * joint->GetTargetVelocity()
		- joint->GetOffsetForce() * joint->GetScene()->GetTimeStepInv()
		);
}


// -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  ----- 
// PHBallJointLimit

PHBallJointLimit::PHBallJointLimit(){
	f.clear();
	
	for(int i=0; i<3; i++){
		fMaxDt[i] =  FLT_MAX;
		fMinDt[i] = -FLT_MAX;
	}
}

/// LCP�������O�i�K�̌v�Z
void PHBallJointLimit::SetupLCP() {
	// �S�����W�n���R�r�A�����v�Z
	CompJacobian();

	// �S�����t���O�̃N���A <<��������axes.CreateList()�܂�axes[n]�͎g���Ȃ��D>>
	axes.Clear();

	// Projection�p�̍ő�E�ŏ��l�����Z�b�g����i
	for (int i=0; i<3; i++) { fMinDt[i] = -FLT_MAX; fMaxDt[i] =  FLT_MAX; }

	// �S�����鎩�R�x�̌���
	SetupAxisIndex();
		
	// LCP�̌W��A, b�̕␳�ldA, db���v�Z
	dA.clear();
	db.clear();
	CompBias();

	// LCP��A�s��̑Ίp�������v�Z
	CompResponseMatrix();
		
	// LCP��b�x�N�g�� == �_������w[t]���v�Z
	b = J[0] * joint->solid[0]->v.w() + J[1] * joint->solid[1]->v.w();

	// �����܂łŌ��肳�ꂽ�S�����t���O���g���Ď��ԍ����X�g���쐬�@<<���������axes[n]���g�p�\>>
	axes.CreateList();

	// �S���͂̏����l���X�V
	for (int n=0; n<axes.size(); ++n) {
		f[axes[n]] *= axes.IsContinued(axes[n]) ? joint->engine->shrinkRate : 0;
	}

	// �S���͏����l�ɂ�鑬�x�ω��ʂ��v�Z
	for (int n=0; n<axes.size(); ++n) {
		int i = axes[n];
		CompResponse(f[i], i);
	}
}

/// LCP�̌J��Ԃ��v�Z
void PHBallJointLimit::IterateLCP() {
	Vec3d fnew, df;
	
	for (int n=0; n<axes.size(); ++n) {
		int i = axes[n];

		// Gauss Seidel Iteration
		fnew[i] = f[i] - joint->engine->accelSOR * Ainv[i] * (dA[i]*f[i] + b[i] + db[i] 
				+ J[0].row(i)*joint->solid[0]->dv.w() + J[1].row(i)*joint->solid[1]->dv.w());

		// Projection
		fnew[i] = min(max(fMinDt[i], fnew[i]), fMaxDt[i]);

		// Comp Response & Update f
		df[i] = fnew[i] - f[i];
		CompResponse(df[i], i);
		f[i] = fnew[i];
	}
}

/// A�̑Ίp�������v�Z����DA = J * M^-1 * J^T
void PHBallJointLimit::CompResponseMatrix() {
	A.clear();
	PHRootNode* root[2] = {
		joint->solid[0]->IsArticulated() ? joint->solid[0]->treeNode->GetRootNode() : NULL,
		joint->solid[1]->IsArticulated() ? joint->solid[1]->treeNode->GetRootNode() : NULL,
	};

	SpatialVector df;
	for (int i=0; i<2; ++i) {
		if (joint->solid[i]->IsDynamical()) {
			if (joint->solid[i]->IsArticulated()) {
				// -- ABA --
				for (int j=0; j<3; ++j) {
					df.v().clear();
					df.w() = J[i].row(j);
					joint->solid[i]->treeNode->CompResponse(df, false, false);
					A[j] += J[i].row(j) * joint->solid[i]->treeNode->da.w();
					//�����Е��̍��̂�����̃c���[�ɑ�����ꍇ�͂��̉e���������Z
					if(joint->solid[!i]->IsArticulated() && root[i] == root[!i]) {
						A[j] += J[!i].row(j) * joint->solid[!i]->treeNode->da.w();
					}
				}
			} else {
				// -- LCP --
				// T = M^-1 * J^T
				T[i] = J[i] * joint->solid[i]->Iinv;

				// A == �_������J * M^-1 * J^T, Gauss Seidel�@��D
				for(int j=0; j<3; ++j) {
					A[j] += J[i].row(j) * T[i].row(j);
				}
			}
		}
	}

	// �ő�̑Ίp�v�f�Ƃ̔䂪eps�����������Ίp�v�f������ꍇ�C
	// ���l�I�s���萫�̌����ƂȂ�̂ł��̐����͍S���Ώۂ��珜�O����
	// ��eps��傫���Ƃ�ƁC�K�v�ȍS���܂Ŗ���������Ă��܂��̂ŁA�����͐T�d�ɁB
	const double eps = 0.000001, epsabs = 1.0e-10;
	double Amax=0.0, Amin;
	for(int i=0; i<6; ++i) {
		if(axes.IsEnabled(i) && A[i] > Amax) { Amax = A[i]; }
	}
	Amin = Amax * eps;

	for(int i=0; i<3; ++i) {
		if (axes.IsEnabled(i)) {
			if(A[i] < Amin || A[i] < epsabs){
				axes.Disable(i);
				DSTR << joint->GetName() << " Limit : Axis " << i << " ill-conditioned! Disabled." << endl;
			} else {
				Ainv[i] = 1.0 / (A[i] + dA[i]);
			}
		}
	}
}

void PHBallJointLimit::CompBias() {
	double tmp = 1.0 / (damper + spring * joint->GetScene()->GetTimeStep());

	for (int i=0; i<3; ++i) {
		if (axes.IsEnabled(i)) {
			dA[i] = tmp * joint->GetScene()->GetTimeStepInv();
			db[i] = tmp * spring * diff[i];
		}
	}
}

void PHBallJointLimit::CompResponse(double df, int i) {
	SpatialVector dfs;
	for (int k=0; k<2; ++k) {
		if (!joint->solid[k]->IsDynamical() || !joint->IsInactive(k)) { continue; }
		if (joint->solid[k]->IsArticulated()) {
			dfs.v().clear();
			dfs.w() = J[k].row(i) * df;
			joint->solid[k]->treeNode->CompResponse(dfs, true, false);
		} else {
			joint->solid[k]->dv.w() += T[k].row(i) * df;
		}
	}
}

// -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  ----- 
// PHBallJointConeLimit

/// �S�����W�n��Jabocian���v�Z
void PHBallJointConeLimit::CompJacobian() {
	// �W���I�Ȋ֐�Jacobian���ŗL�̍S�����W�n�ɕϊ�����s��
	Matrix3d Jc, Jcinv;
	const double eps = 1.0e-5;
	Jc.Ez() = joint->Xjrel.q * Vec3d(0.0, 0.0, 1.0);
	Vec3d tmp = Jc.Ez() - limitDir;
	double n = tmp.square();
	Jc.Ex() = (n > eps ? cross(Jc.Ez(), tmp).unit() : joint->Xjrel.q * Vec3d(1.0, 0.0, 0.0));
	Jc.Ey() = cross(Jc.Ez(), Jc.Ex());
	Jcinv   = Jc.trans();
	
	// �֐�Jacobian���擾�E�ϊ�����Limit��Jacobian�Ƃ���
	J[0] = Jcinv * joint->J[0].ww();
	J[1] = Jcinv * joint->J[1].ww();
}

/// ���搧���ɂ������Ă��邩�m�F���ǂ̎��R�x�𑬓x�S�����邩��ݒ�
void PHBallJointConeLimit::SetupAxisIndex() {
	Vec2d limit[3] = { limitSwing, limitSwingDir, limitTwist };

	for (int i=0; i<3; ++i) {
		if ((limit[i][0] < FLT_MAX*0.1) && joint->position[i] < limit[i][0]) {
			axes.Enable(i); bOnLimit = true;
			diff[i] = joint->position[i] - limit[i][0];
			fMinDt[i] = 0;
		} else if ((limit[i][1] < FLT_MAX*0.1) && joint->position[i] > limit[i][1]) {
			axes.Enable(i);
			diff[i] = joint->position[i] - limit[i][1];
			fMaxDt[i] = 0; bOnLimit = true;
		}
	}
}

// -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  ----- 
// PHBallJointSplineLimit

// -----  -----  -----  -----  -----
// SplinePoint

void SplinePoint::Update() {
	// �͈̓`�F�b�N
	while (1.0 < t) { t -= 1.0; edge++; }
	if (edge >= spline->NEdges()) { edge = 0; }
	while (t < 0.0) { t += 1.0; edge--; }
	if (edge < 0) { edge = spline->NEdges()-1; }

	// pos�̍X�V
	Vec4d s = spline->GetEdgeSwingCoeff(edge);
	Vec4d d = spline->GetEdgeSwingDirCoeff(edge);
	pos[0] = s[0]*t*t*t + s[1]*t*t + s[2]*t + s[3];
	pos[1] = d[0]*t*t*t + d[1]*t*t + d[2]*t + d[3];
}

void SplinePoint::ReverseUpdate() {
	for (int i=0; i< spline->NEdges(); ++i) {
		SplinePoint p0 = spline->GetPointOnEdge(i, 0.0);
		SplinePoint p1 = spline->GetPointOnEdge(i, 1.0);
		if (p0.pos[1] < pos[1] && pos[1] < p1.pos[1]) { edge = i; break; }
	}
	Vec4d d = spline->GetEdgeSwingDirCoeff(edge); d[3] -= pos[1];
	Vec3d sol = SolveCubicFunction(d);
	for (int j=0; j<3; ++j) { if (0<sol[j] && sol[j]<=1) { t = sol[j]; } }
}

Matrix3d SplinePoint::CompJacobian() {
	Vec3d p0  = FromPolar(pos);

	SplinePoint pt = *this;
	Vec3d p1 = FromPolar(pt.pos);

	pt.t -= 0.01; pt.Update();
	Vec3d p2 = FromPolar(pt.pos);

	Matrix3d Jc;
	Jc.Ex() = p1 - p2; if (Jc.Ex().norm()!=0) { Jc.Ex() = Jc.Ex().unit(); }
	Jc.Ez() = p0;      if (Jc.Ez().norm()!=0) { Jc.Ez() = Jc.Ez().unit(); }
	Jc.Ey() = PTM::cross(Jc.Ez(), Jc.Ex());

	return Jc;
}

// -----  -----  -----  -----  -----
// ClosedSplineCurve

Vec4d ClosedSplineCurve::GetEdgeSwingCoeff(int i) {
	Vec4d c;
	int i0 = i-1, i1 = i;
	int sz = (int)nodes->size();
	if (i0 < 0)
		i0 += sz;
	if (i1 >= sz)
		i1 -= sz;

	double  s0 = (*nodes)[i0].swing;
	double  s1 = (*nodes)[i1].swing;
	double ds0 = (*nodes)[i0].dSwing;
	double ds1 = (*nodes)[i1].dSwing;

	c[0] = 2*s0 - 2*s1 +   ds0 + ds1;
	c[1] = 3*s1 - 3*s0 - 2*ds0 - ds1;
	c[2] = ds0;
	c[3] = s0;

	return c;
}

Vec4d ClosedSplineCurve::GetEdgeSwingDirCoeff(int i) {
	Vec4d c;
	int i0 = i-1, i1 = i;
	int sz = (int)nodes->size();
	if (i0 < 0)
		i0 += sz;
	if (i1 >= sz)
		i1 -= sz;

	double  s0 = (*nodes)[i0].swingDir;
	double  s1 = (*nodes)[i1].swingDir;
	double ds0 = (*nodes)[i0].dSwingDir;
	double ds1 = (*nodes)[i1].dSwingDir;

	c[0] = 2*s0 - 2*s1 +   ds0 + ds1;
	c[1] = 3*s1 - 3*s0 - 2*ds0 - ds1;
	c[2] = ds0;
	c[3] = s0;

	return c;
}

void ClosedSplineCurve::AddNode(PHSplineLimitNode node, int pos) {
	if (pos < 0) { pos += nodes->size(); }
	std::vector<PHSplineLimitNode>::iterator it = nodes->begin();
	for (int i=0; i<pos; ++i) { ++it; }
	nodes->insert(it, node);
}

SplinePoint ClosedSplineCurve::GetPointOnEdge(int i, double t) {
	SplinePoint sp;
	sp.spline = this;
	sp.edge   = i;
	sp.t      = t;
	sp.Update();
	return sp;
}

// -----  -----  -----  -----  -----
// PHBallJointSplineLimit

/// �S�����W�n��Jabocian���v�Z
void PHBallJointSplineLimit::CompJacobian() {
	currPos = joint->position;

	// �ϊ��O�̕W��Jacobian
	Matrix3d Jc, Jcinv=Matrix3d::Unit();
	const double eps = 1.0e-5;
	Jc.Ez() = joint->Xjrel.q * Vec3d(0.0, 0.0, 1.0);
	Vec3d tmp = Jc.Ez() - limitDir;
	double n = tmp.square();
	Jc.Ex() = (n > eps ? cross(Jc.Ez(), tmp).unit() : joint->Xjrel.q * Vec3d(1.0, 0.0, 0.0));
	Jc.Ey() = cross(Jc.Ez(), Jc.Ex());
	Jcinv = Jc.trans();

	// ����`�F�b�N
	CheckSwingLimit();
	CheckTwistLimit();

	// Jacobian��ϊ�����
	if (bOnSwing) { Jcinv = neighbor.CompJacobian().trans(); }
	J[0] = Jcinv * joint->J[0].ww();
	J[1] = Jcinv * joint->J[1].ww();
}

/// �ǂ̎��R�x�𑬓x�S�����邩��ݒ�
void PHBallJointSplineLimit::SetupAxisIndex() {
	if (bOnSwing) {
		axes.Enable(0);
	} else if (bOnTwist) {
		axes.Enable(2);
	}
}

// -----  -----  -----

/// Swing-SwingDir�̉���`�F�b�N�ibOnSwing�i��neighbor�j�����肷��j
void PHBallJointSplineLimit::CheckSwingLimit() {
	// ��_�����J�E���g���ē��O������s��
	double minDist = DBL_MAX;
	int numIntersection = CheckIntersection(Vec2d(0, 0), Vec2d(currPos[0], currPos[1]), minDist);
	bOnSwing = (numIntersection % 2 == 1);

	if (bOnSwing) {
		// ----- ����O�ɏo�Ă����ꍇ -----
		// ��������̍ŏI�n�_���猻�ݒn�_�Ɏ���ŋߖT��_�����߂�
		CheckIntersection(Vec2d(lastPos[0], lastPos[1]), Vec2d(currPos[0], currPos[1]), minDist);

		// �ŋߖT��_�����ǂ��ċߖT�_�����߂�
		double delta = 0.01; // t�̒T����
		double minNorm = 10;

		Vec3d p0 = FromPolar(Vec2d(currPos[0], currPos[1]));
		SplinePoint point = neighbor;
		
		point.t-=(delta*10.0); point.Update();
		for (int i=0; i<20; ++i) {
			point.t += delta; point.Update();
			Vec3d p1 = FromPolar(point.pos);
			if (minNorm > (p0-p1).norm()) {
				minNorm  = (p0-p1).norm();
				neighbor = point;
			}
		}
		
		// �N���ʂ����߂�
		diff.x = minNorm;
		if (diff.x > 0.5) { diff.x = 0.5; } // �l���傫������Ɣ��U����\�������邽��
	} else {
		// ----- ������������ꍇ -----
		lastPos = currPos;
	}
}

/// Twist�̉���`�F�b�N�ibOnTwist�����肷��j
void PHBallJointSplineLimit::CheckTwistLimit() {
	bOnTwist = false;
	if (poleTwist[0] < FLT_MAX*0.1 && poleTwist[0] != poleTwist[1]) {
		// ���݈ʒu�ł�Twist Limit���Ԃ��ċ��߂�
		SplinePoint ptTwist = neighbor; ptTwist.spline = &limitCurve;
		if (bOnSwing) {
			ptTwist.pos = neighbor.pos + Vec2d(currPos[0], currPos[1]);
		} else {
			ptTwist.pos = Vec2d(currPos[0], currPos[1]); ptTwist.ReverseUpdate();
		}

		Vec4d  s = limitCurve.GetEdgeSwingCoeff(ptTwist.edge);
		double t = ptTwist.t;
		double l = s[0]*t*t*t + s[1]*t*t + s[2]*t + s[3];

		std::vector<PHSplineLimitNode>* nodes = limitCurve.nodes;
		PHSplineLimitNode nd0 = (*nodes)[ptTwist.edge], nd1 = (*nodes)[ptTwist.edge+1];
		Vec2d limitTwist = Vec2d(
			nd0.twistMin * (1 - t) + nd1.twistMin*t,
			nd0.twistMax * (1 - t) + nd1.twistMax*t
			);
		if (!bOnSwing) {
			Vec2d ps = ptTwist.pos * (1/l);
			limitTwist[0] = limitTwist[0]*ps[0]*ps[0] + poleTwist[0]*(1 - ps[0]*ps[0]);
			limitTwist[1] = limitTwist[1]*ps[0]*ps[0] + poleTwist[1]*(1 - ps[0]*ps[0]);
		}

		// Twist�̉���`�F�b�N
		if (limitTwist[0] < limitTwist[1]) {
			if (limitTwist[0] < FLT_MAX*0.1 && currPos[2] < limitTwist[0]) {
				bOnTwist = true;
				diff.z = currPos[2] - limitTwist[0];
				fMinDt[2] = 0.0;
			} else if (limitTwist[1] < FLT_MAX*0.1 && currPos[2] > limitTwist[1]) {
				bOnTwist = true;
				diff.z = currPos[2] - limitTwist[1];
				fMaxDt[2] = 0.0;
			}
		}
	}
}

/// ��_�`�F�b�N
int PHBallJointSplineLimit::CheckIntersection(Vec2d base, Vec2d curr, double& minDist) {
	int numIsect = 0;

	if (base==curr) { return 0; }

	// �S���Ȑ��̕������ƂɌ�_�̗L�����`�F�b�N����
	for (int i=0; i<limitCurve.NEdges(); ++i) {
		Vec4d swg = limitCurve.GetEdgeSwingCoeff(i);
		Vec4d swd = limitCurve.GetEdgeSwingDirCoeff(i);
		
		// �O���������𗧂ĂČ�_�������߂�
		Vec4d eqn;
		if ((curr[1]-base[1]) != 0) {
			double K = (curr[0]-base[0])/(curr[1]-base[1]);
			eqn = swg - K*swd;
			eqn[3] -= K*base[1] - base[0];
		} else {
			double K = (curr[1]-base[1])/(curr[0]-base[0]);
			eqn = swd - K*swg;
			eqn[3] -= K*base[0] - base[1];
		}
		Vec3d sol = SolveCubicFunction(eqn);
		
		// ��_��₩��ŋߖT��_�����߂�
		for (int j=0; j<3; ++j) {
			if (0 <= sol[j] && sol[j] <= 1) {
				SplinePoint isect = limitCurve.GetPointOnEdge(i, sol[j]);

				// ���݂̈ʒu���Swing�p�̏�������_�𐔂��グ��
				if (isect.pos[0] < curr[0]) { numIsect++; }
					
				// �����̈�ԋ߂���_��T��
				Vec3d p0 = FromPolar(Vec2d(curr[0], curr[1]));
				Vec3d p1 = FromPolar(isect.pos);
				double dist = (p0-p1).norm();
				if (dist < minDist) {
					minDist = dist;
					neighbor = isect;
					diff.x = isect.pos[0] - curr[0];
				}
			}
		}
	}

	return numIsect;
}

// -----  -----  -----  -----  -----

/// �R���������������֐�
Vec3d SolveCubicFunction(Vec4d eq3){
	double z = 0;
	bool FlagQ = true;
	double D;
	Vec3d Para;
	double eq2[3];
	
	if(eq3[0] != 0){
		if(eq3[3] == 0) Para[0] = 0;
		else {
			double variable[2];
			variable[0] = (eq3[2]/eq3[0] - eq3[1]*eq3[1] / (3.0*eq3[0]*eq3[0])) / 3.0;//P
			variable[1] = (eq3[3]/eq3[0]-eq3[1]*eq3[2]/(3.0*eq3[0]*eq3[0])+2.0*pow(eq3[1],3)/(27.0*pow(eq3[0],3)))/2.0;//Q
			
			if(variable[1] == 0) z = 0;
			else{
				if(variable[1] < 0){
					FlagQ = false;
					variable[1] *= -1;
				}
				D = pow(variable[0],3) + pow(variable[1],2);
				if(D < 0)
					z = - 2 * sqrt( -variable[0] ) * cos(atan(sqrt(-D) / variable[1]) / 3.0);
				else if(variable[0] < 0)
					z = -pow(variable[1] + sqrt(D),0.333333) - pow(variable[1] - sqrt(D),0.333333);
				else if(variable[0] >= 0)
					z = pow(sqrt(D) - variable[1],0.333333) - pow(variable[1] + sqrt(D),0.333333);
				if(FlagQ == false)
					z *= -1;				
			}	
		Para[0] = z - eq3[1] / (3 * eq3[0]);
		}
		eq2[0] = eq3[0];
		for(int  i=0; i<2; i++){
			eq2[i+1] = eq3[i+1] + Para[0] * eq2[i];
		}
	}
	else {
		Para[0] = FLT_MAX;
		eq2[0] = eq3[1];
		eq2[1] = eq3[2];
		eq2[2] = eq3[3];
	}
	if(eq2[0] != 0){
		D = eq2[1] * eq2[1] - 4.0 * eq2[0] * eq2[2];
		if (D > 0){
			Para[1] = (- eq2[1] + sqrt(D))/ (2 * eq2[0]);
			Para[2] = (- eq2[1] - sqrt(D))/ (2 * eq2[0]);
		}
		else Para[1] = Para[2] = FLT_MAX;
	}
	else if(eq2[1] != 0){
		Para[1] = -eq2[2]/eq2[1];
		Para[2] = FLT_MAX;
	}
	else{
		Para[1] = Para[2] = FLT_MAX;
	}
	return	Para;
}

/// Swing-SwingDir�ɍ��W�����[�N���b�h���W�ɒ����֐�
Vec3d FromPolar(Vec2d pos) {
	return Vec3d(sin(pos[0])*cos(pos[1]), sin(pos[0])*sin(pos[1]), cos(pos[0]));
}

}
