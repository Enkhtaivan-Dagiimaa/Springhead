#include "Physics.h"
#ifdef USE_HDRSTOP
#pragma hdrstop
#endif
#include <float.h>
#include <Collision/CDDetectorImp.h>
#include <Collision/CDQuickHull2D.h>
#include <Collision/CDQuickHull2DImp.h>
#include <Collision/CDCutRing.h>

using namespace PTM;
using namespace std;
namespace Spr{;


#define SUBMAT(r, c, h, w) sub_matrix(TSubMatrixDim<r, c, h, w>())
#define SUBVEC(o, l) sub_vector(TSubVectorDim<o, l>())

inline double rowtimes(const Matrix3d& M, int k, const Vec3d& v){
	return M.item(k, 0) * v[0] + M.item(k, 1) * v[1] + M.item(k, 2) * v[2];
}

//----------------------------------------------------------------------------
// PHSolidAux
void PHSolidAuxs::Init(){
	for(iterator it = begin(); it != end(); it++){
		PHSolidAux* s = *it;
		s->minv = s->solid->GetMassInv();
		s->Iinv = s->solid->GetInertiaInv();
	}
}
void PHSolidAux::SetupDynamics(double dt){
	Quaterniond q = solid->GetOrientation();
	v = q.Conjugated() * solid->GetVelocity();
	w = q.Conjugated() * solid->GetAngularVelocity();
	if(solid->IsDynamical()){
		f = q.Conjugated() * solid->nextForce;
		t = q.Conjugated() * solid->nextTorque;
		dv0 = minv * f * dt;
		dw0 = Iinv * (t - w % (solid->GetInertia() * w)) * dt;
	}
	else{
		dv0.clear();
		dw0.clear();
	}
	dv.clear();
	dw.clear();
}
void PHSolidAux::SetupCorrection(){
	dV.clear();
	dW.clear();
}

//----------------------------------------------------------------------------
// PHConstraint
//OBJECTIMP(PHConstraint, SceneObject);
IF_IMP(PHConstraint, SceneObject);
PHConstraint::PHConstraint(){
	fv.clear();
	fw.clear();
	Fv.clear();
	Fq.clear();
	bEnabled = true;
}
void PHConstraint::Init(PHSolidAux* lhs, PHSolidAux* rhs, const PHJointDesc& desc){
	solid[0] = lhs, solid[1] = rhs;
	for(int i = 0; i < 2; i++){
		desc.poseJoint[i].Ori().ToMatrix(Rj[i]);
		rj[i] = desc.poseJoint[i].Pos();
	}
	bEnabled = desc.bEnabled;
}
void PHConstraint::CompJacobian(bool bCompAngular){
	Matrix3d	R[2], Rjabs[2];
	Vec3d		r[2];
	R[0] = solid[0]->solid->GetRotation();
	R[1] = solid[1]->solid->GetRotation();
	r[0] = solid[0]->solid->GetCenterPosition();
	r[1] = solid[1]->solid->GetCenterPosition();
	Rjabs[0] = R[0] * Rj[0];
	Rjabs[1] = R[1] * Rj[1];
	Rjrel = Rjabs[0].trans() * Rjabs[1];
	qjrel.FromMatrix(Rjrel);
	rjrel = Rjabs[0].trans() * ((R[1] * rj[1] + r[1]) - (R[0] * rj[0] + r[0]));
	Jvv[0] = -Rj[0].trans();
	Jvw[0] = -Rj[0].trans() * (-Matrix3d::Cross(rj[0]));
	Jvv[1] =  Rjabs[0].trans() * R[1];
	Jvw[1] =  Jvv[1] * (-Matrix3d::Cross(rj[1]));
	vjrel = Jvv[0] * solid[0]->v + Jvw[0] * solid[0]->w + Jvv[1] * solid[1]->v + Jvw[1] * solid[1]->w;
	
	if(bCompAngular){
		Jwv[0].clear();
		Jww[0] = Jvv[0];
		Jwv[1].clear();
		Jww[1] = Jvv[1];
		wjrel = Jwv[0] * solid[0]->v + Jww[0] * solid[0]->w + Jwv[1] * solid[1]->v + Jww[1] * solid[1]->w;
		
		Matrix3d E(
			 qjrel.W(),  qjrel.Z(), -qjrel.Y(),
			-qjrel.Z(),  qjrel.W(),  qjrel.X(),
			 qjrel.Y(), -qjrel.X(),  qjrel.W());
		E *= 0.5;
		Jqv[0].clear();
		Jqw[0] = E * Jww[0];
		Jqv[1].clear();
		Jqw[1] = E * Jww[1];
	}
}

void PHConstraint::SetupDynamics(double dt){
	bFeasible = solid[0]->solid->IsDynamical() || solid[1]->solid->IsDynamical();
	if(!bEnabled || !bFeasible)return;

	CompJacobian(GetJointType() != PHJointDesc::JOINT_CONTACT);		//�ڐG�S���̏ꍇ�͉�]�֌W�̃��R�r�s��͕K�v�Ȃ�
	CompDof();
	CompMotorForce();

	int i, j;
	Av.clear();
	Aw.clear();
	Aq.clear();
	for(i = 0; i < 2; i++){
		if(solid[i]->solid->IsDynamical()){
			for(j = 0; j < dim_v; j++){
				Tvv[i].row(j) = Jvv[i].row(j) * solid[i]->minv;
				Tvw[i].row(j) = Jvw[i].row(j) * solid[i]->Iinv;
				Av[j] += Jvv[i].row(j) * Tvv[i].row(j) + Jvw[i].row(j) * Tvw[i].row(j);
			}
			if(mode == MODE_TORQUE){
				for(; j < 3; j++){
					solid[i]->dv += Tvv[i].row(j) * fv[j];
					solid[i]->dw += Tvw[i].row(j) * fv[j];
				}
			}
			for(j = 0; j < dim_w; j++){
				Twv[i].row(j) = Jwv[i].row(j) * solid[i]->minv;
				Tww[i].row(j) = Jww[i].row(j) * solid[i]->Iinv;
				Aw[j] += Jwv[i].row(j) * Twv[i].row(j) + Jww[i].row(j) * Tww[i].row(j);
			}
			if(mode == MODE_TORQUE){
				for(; j < 3; j++){
					solid[i]->dv += Twv[i].row(j) * fw[j];
					solid[i]->dw += Tww[i].row(j) * fw[j];
				}
			}
			for(j = 0; j < dim_q; j++){
				Tqv[i].row(j) = Jqv[i].row(j) * solid[i]->minv;
				Tqw[i].row(j) = Jqw[i].row(j) * solid[i]->Iinv;
				Aq[j] += Jqv[i].row(j) * Tqv[i].row(j) + Jqw[i].row(j) * Tqw[i].row(j);
			}
		}
	}
	
	for(j = 0; j < dim_v; j++){
		bv[j] = vjrel[j] +
			rowtimes(Jvv[0], j, solid[0]->dv0) + rowtimes(Jvw[0], j, solid[0]->dw0) +
			rowtimes(Jvv[1], j, solid[1]->dv0) + rowtimes(Jvw[1], j, solid[1]->dw0);
	}
	for(int j = 0; j < dim_w; j++){
		bw[j] = wjrel[j] + 
			rowtimes(Jwv[0], j, solid[0]->dv0) + rowtimes(Jww[0], j, solid[0]->dw0) +
			rowtimes(Jwv[1], j, solid[1]->dv0) + rowtimes(Jww[1], j, solid[1]->dw0);
	}

	CompBias(dt);

	if(dim_v)for(i = 0; i < 3; i++)
		Av[i] = 1.0 / Av[i];
	if(dim_w)for(i = 0; i < 3; i++)
		Aw[i] = 1.0 / Aw[i];
	if(dim_q)for(i = 0; i < 3; i++)
		Aq[i] = 1.0 / Aq[i];

	for(j = 0; j < dim_v; j++){
		bv[j] *= Av[j];
		Jvv[0].row(j) *= Av[j];
		Jvw[0].row(j) *= Av[j];
		Jvv[1].row(j) *= Av[j];
		Jvw[1].row(j) *= Av[j];
	}
	for(int j = 0; j < dim_w; j++){
		bw[j] *= Aw[j];
		Jwv[0].row(j) *= Aw[j];
		Jww[0].row(j) *= Aw[j];
		Jwv[1].row(j) *= Aw[j];
		Jww[1].row(j) *= Aw[j];
	}
	//fv.clear();
	//fw.clear();
}
void PHConstraint::CompError(){
	Bv = rjrel;
	Quaterniond qrel;
	qrel.FromMatrix(Rjrel);
	Bq = qrel.V();
}
void PHConstraint::SetupCorrection(double dt, double max_error){
	if(!bEnabled || !bFeasible)return;

	int j;
	Vec3d v[2], w[2];
	for(int i = 0; i < 2; i++){
		v[i] = solid[i]->v + solid[i]->dv0 + solid[i]->dv;
		w[i] = solid[i]->w + solid[i]->dw0 + solid[i]->dw;
	}
	CompError();
	// velocity update�ɂ��e�������Z
	double tmp;
	for(j = 0; j < dim_v; j++){
		Bv[j] *= Av[j];
		Bv[j] += (rowtimes(Jvv[0], j, v[0]) + rowtimes(Jvw[0], j, w[0]) +
				  rowtimes(Jvv[1], j, v[1]) + rowtimes(Jvw[1], j, w[1])) * dt;
	}
	Bv *= 0.2;	//��x�Ɍ덷��0�ɂ��悤�Ƃ���Ƃ��U���I�ɂȂ�̂œK���Ɍ덷��������������
	tmp = max(max(Bv[0], Bv[1]), Bv[2]);
	if(tmp > max_error)
		Bv *= (max_error / tmp);
	for(j = 0; j < dim_q; j++){
		Bq[j] *= Aq[j];
		Bq[j] += (rowtimes(Jqv[0], j, v[0]) + rowtimes(Jqw[0], j, w[0]) +
				  rowtimes(Jqv[1], j, v[1]) + rowtimes(Jqw[1], j, w[1])) * dt;
	}
	Bq *= 0.2;
	tmp = max(max(Bq[0], Bq[1]), Bq[2]);
	if(tmp > max_error)
		Bq *= (max_error / tmp);
	//DSTR << B.norm() << endl;
	//Fv.clear();
	//Fq.clear();
}
void PHConstraint::IterateDynamics(){
	if(!bEnabled || !bFeasible)return;

	Vec3d fvnew, fwnew, dfv, dfw;
	int i, j;
	for(j = 0; j < dim_v; j++){
		fvnew[j] = fv[j] - (bv[j] + 
			rowtimes(Jvv[0], j, solid[0]->dv) + rowtimes(Jvw[0], j, solid[0]->dw) +
			rowtimes(Jvv[1], j, solid[1]->dv) + rowtimes(Jvw[1], j, solid[1]->dw));
		Projectionfv(fvnew[j], j);
		dfv[j] = fvnew[j] - fv[j];
		for(i = 0; i < 2; i++){
			if(solid[i]->solid->IsDynamical()){
				solid[i]->dv += Tvv[i].row(j) * dfv[j];
				solid[i]->dw += Tvw[i].row(j) * dfv[j];
			}
		}
		fv[j] = fvnew[j];
	}
	for(j = 0; j < dim_w; j++){
		fwnew[j] = fw[j] - (bw[j] + 
			rowtimes(Jwv[0], j, solid[0]->dv) + rowtimes(Jww[0], j, solid[0]->dw) +
			rowtimes(Jwv[1], j, solid[1]->dv) + rowtimes(Jww[1], j, solid[1]->dw));
		Projectionfw(fwnew[j], j);
		dfw[j] = fwnew[j] - fw[j];
		for(i = 0; i < 2; i++){
			if(solid[i]->solid->IsDynamical()){
				solid[i]->dv += Twv[i].row(j) * dfw[j];
				solid[i]->dw += Tww[i].row(j) * dfw[j];
			}
		}
		fw[j] = fwnew[j];
	}
}
void PHConstraint::IterateCorrection(){
	if(!bEnabled || !bFeasible)return;

	Vec3d Fvnew, Fqnew, dFv, dFq;
	int i, j;
	for(j = 0; j < dim_v; j++){
		Fvnew[j] = Fv[j] - (Bv[j] + 
			rowtimes(Jvv[0], j, solid[0]->dV) + rowtimes(Jvw[0], j, solid[0]->dW) +
			rowtimes(Jvv[1], j, solid[1]->dV) + rowtimes(Jvw[1], j, solid[1]->dW));
		ProjectionFv(Fvnew[j], j);
		dFv[j] = Fvnew[j] - Fv[j];
		for(i = 0; i < 2; i++){
			if(solid[i]->solid->IsDynamical()){
				solid[i]->dV += Tvv[i].row(j) * dFv[j];
				solid[i]->dW += Tvw[i].row(j) * dFv[j];
			}
		}
		Fv[j] = Fvnew[j];
	}
	for(j = 0; j < dim_q; j++){
		Fqnew[j] = Fq[j] - (Bq[j] + 
			rowtimes(Jqv[0], j, solid[0]->dV) + rowtimes(Jqw[0], j, solid[0]->dW) +
			rowtimes(Jqv[1], j, solid[1]->dV) + rowtimes(Jqw[1], j, solid[1]->dW));
		ProjectionFq(Fqnew[j], j);
		dFq[j] = Fqnew[j] - Fq[j];
		for(i = 0; i < 2; i++){
			if(solid[i]->solid->IsDynamical()){
				solid[i]->dV += Tqv[i].row(j) * dFq[j];
				solid[i]->dW += Tqw[i].row(j) * dFq[j];
			}
		}
		Fq[j] = Fqnew[j];
	}
}

//----------------------------------------------------------------------------
// PHContactPoint
//OBJECTIMP(PHContactPoint, PHConstraint);
IF_IMP(PHContactPoint, PHConstraint);
PHContactPoint::PHContactPoint(const Matrix3d& local, CDShapePair* sp, Vec3d p, PHSolidAux* s0, PHSolidAux* s1){
	shapePair = sp;
	pos = p;
	solid[0] = s0, solid[1] = s1;

	dim_v = 3;
	dim_w = 0;
	dim_q = 0;
	
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

	dim_v = 3;
	dim_w = 0;
	dim_q = 0;
	
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
void PHContactPoint::Projectionfv(double& f, int k){
	static double flim;
	if(k == 0){	//�����R�� >= 0�̐���
		f = Spr::max(0.0, f);
		flim = 0.5 * (shapePair->shape[0]->material.mu0 + shapePair->shape[1]->material.mu0) * f;	//�ő�Î~���C
	}
	else{
		//|���C��| <= �ő�Î~���C�̐���
		//	�E���C�͂̊e�������ő�Î~���C�����������Ă����͂͒�����\��������̂Ŗ{���͂��������B
		//	�E�Î~���C�Ɠ����C�������l�łȂ��ƈ����Ȃ��B
		//���C�W���͗��҂̐Î~���C�W���̕��ςƂ���
		f = Spr::min(Spr::max(-flim, f), flim);
	}
}
void PHContactPoint::CompError(){
	const double eps = 0.0001;
	//�Փ˔���A���S���Y���̓s����ACorrection�ɂ���Ċ��S�ɍ��̂�����Ă��܂��͍̂���̂�
	//�덷��eps����������������
	double error = min(0.0, -shapePair->depth + eps);
	Bv = Vec3d(eps, 0.0, 0.0);
}
void PHContactPoint::ProjectionFv(double& F, int k){
	//�����R�� >= 0�̐���
	if(k == 0)
		 F = Spr::max(0.0, F);
	else F = 0.0;
}

//----------------------------------------------------------------------------
// PHJoint
IF_IMP(PHJoint, PHConstraint);
//----------------------------------------------------------------------------
// PHJoint1D
IF_IMP(PHJoint1D, PHJoint);
//----------------------------------------------------------------------------
// PHHingeJoint
//OBJECTIMP(PHHingeJoint, PHJoint1D)
IF_IMP(PHHingeJoint, PHJoint1D)
double PHHingeJoint::GetPosition(){
	//�������̍S���͍��v���Ă�����̂Ɖ��肵�Ċp�x������
	double theta = qjrel.Theta();
	if(qjrel.Axis().Z() < 0.0)
		theta = -theta;
	return theta;
}
double PHHingeJoint::GetVelocity(){
	return wjrel[2];
}
void PHHingeJoint::CompDof(){
	on_lower = on_upper = false;
	if(lower < upper){
		double theta = GetPosition();
		on_lower = (theta <= lower);
		on_upper = (theta >= upper);
	}
	if(on_lower || on_upper || mode == MODE_POSITION){
		dim_v = 3;
		dim_w = 3;
		dim_q = 3;
	}
	if(mode == MODE_VELOCITY || spring != 0.0 || damper != 0.0){
		dim_v = 3;
		dim_w = 3;
		dim_q = 2;
	}
	else{
		dim_v = 3;
		dim_w = 2;
		dim_q = 2;
	}
}
void PHHingeJoint::CompMotorForce(){
	fw[2] = torque;
}
void PHHingeJoint::CompBias(double dt){
	if(mode == MODE_VELOCITY){
		bw[2] -= vel_d;
	}
	else if(spring != 0.0 || damper != 0.0){
		double diff = GetPosition() - origin;
		if(diff >  M_PI) diff -= 2 * M_PI;
		if(diff < -M_PI) diff += 2 * M_PI;
		double tmp = 1.0 / (damper + spring * dt);
		Aw[2] += tmp / dt;
		bw[2] += spring * (diff) * tmp;
		/*
		�o�l�_���p�ƊO�͂̉^���������F
		#mimetex(  f=kx+bv+f_e  )
		#mimetex(  v_{t+1} = f_{t+1} \Delta t + v_t  )
		#mimetex(  v_{t+1} = (kx_{t+1}+bv_{t+1}+f_e_{t+1}) \Delta t + v_t  )
		#mimetex(  x_{t+1} = v_{t+1} \Delta t + x_t  )
		~
		#mimetex(  v_{t+1} = (k(v_{t+1}\Delta t+x_t) +bv_{t+1}+f_e_{t+1}) \Delta t + v_t  )
		���x�̍X�V�̎��F
		#mimetex(  (1-k\Delta t^2 - b \Delta t)v_{t+1} = (f_e_{t+1}+k x_t)\Delta t + v_t)
		�Ȃ̂ŁC�֐߂ɂȂ���Q���̂ɉ����͂��C�֐߃g���N�ɕϊ����āC
		�S�������Ƃ��Ă������� vnext ���X�V����΁C�O�͂��l�������_���p�ɂȂ�H
		*/
	}
}
void PHHingeJoint::Projectionfw(double& f, int k){
	if(k == 2){
		if(on_lower)
			f = max(0.0, f);
		if(on_upper)
			f = min(0.0, f);
	}
}
void PHHingeJoint::ProjectionFq(double& F, int k){
	if(k == 2){
		if(on_lower)
			F = max(0.0, F);
		if(on_upper)
			F = min(0.0, F);
	}
}

//----------------------------------------------------------------------------
// PHSliderJoint
//OBJECTIMP(PHSliderJoint, PHJoint1D)
IF_IMP(PHSliderJoint, PHJoint1D)
void PHSliderJoint::CompDof(){
	dim_v = 2;
	dim_w = 3;
	dim_q = 3;
}

//----------------------------------------------------------------------------
// PHBallJoint
//OBJECTIMP(PHBallJoint, PHJoint1D)
IF_IMP(PHBallJoint, PHJoint1D)
void PHBallJoint::CompDof(){
	dim_v = 3;
	dim_w = 0;
	dim_q = 0;
}

//----------------------------------------------------------------------------
//	PHSolidPair
void PHConstraintEngine::PHSolidPair::Init(PHSolidAux* s0, PHSolidAux* s1){
	solid[0] = s0;
	solid[1] = s1;
	int ns0 = solid[0]->solid->shapes.size(), ns1 = solid[1]->solid->shapes.size();
	shapePairs.resize(ns0, ns1);
	for(int i = 0; i < ns0; i++)for(int j = 0; j < ns1; j++){
		PHShapePair& sp = shapePairs.item(i, j);
		sp.shape[0] = solid[0]->solid->shapes[i];
		sp.shape[1] = solid[1]->solid->shapes[j];
	}
	bEnabled = true;
}

class ContactVertex: public Vec3d{
public:
	static Vec3d ex, ey;
	ContactVertex(){}
	ContactVertex(const Vec3d& v):Vec3d(v){}
	Vec2d GetPos(){
		return Vec2d( (*(Vec3d*)this)*ex, (*(Vec3d*)this)*ey );
	}
};
Vec3d ContactVertex::ex;
Vec3d ContactVertex::ey;

void PHShapePair::EnumVertex(PHConstraintEngine* engine, unsigned ct, PHSolidAux* solid0, PHSolidAux* solid1){
	//	center �� normal�����ʂƌ�������ʂ����߂Ȃ��Ƃ����Ȃ��D
	//	�ʂ̒��_���ʂ̑��ɂ���ʂ������ΏہD
	//	quick hull �� n log r ������C���`���Ԃŏo���邱�Ƃ͂���Ă����ׂ��D

	//	�e3�p�`�ɂ��āC���_���ǂ��瑤�ɂ��邩���肵�C�����ɂ�����̂�ΏۂƂ���D
	//	������C�@���{���l�̌`�ŕ\������D
	//	���̏����͓ʌ`�󂪎����Ă��ėǂ��D
	//	������̕\���`���Ƃ��āC2���Ȑ��������D���̏ꍇ�C�����͕Ԃ��Ȃ�
	//	��2���Ȑ���Mullar��Preparata�ɂ͓���Ȃ��ŕʂɂ��Ă����D
	CDConvex* conv[2] = { (CDConvex*)shape[0], (CDConvex*)shape[1], };


	//2D�ւ̕ϊ�������D�ǂ�����H
	//	�K���ɑ��x�H
	Vec3d v0 = solid0->solid->GetPointVelocity(center);
	Vec3d v1 = solid1->solid->GetPointVelocity(center);
	Matrix3d local;	//	contact coodinate system �ڐG�̍��W�n
	local.Ex() = normal;
	local.Ey() = v1-v0;
	local.Ey() -= local.Ey() * normal * normal;
	if (local.Ey().square() > 1e-6){
		local.Ey().unitize(); 
	}else{
		if (Square(normal.x) < 0.5) local.Ey()= (normal ^ Vec3f(1,0,0)).unit();
		else local.Ey() = (normal ^ Vec3f(0,1,0)).unit();
	}
	local.Ez() =  local.Ex() ^ local.Ey();
	if (local.det() < 0.99) {
		DSTR << "Error: local coordinate error." << std::endl;
		assert(0);
	}

	//	�؂�������߂�P�F�؂�����\����������̗�
	CDCutRing cutRing(center, local);
	conv[0]->FindCutRing(cutRing, shapePoseW[0]);
	conv[1]->FindCutRing(cutRing, shapePoseW[1]);
	//	�؂�������߂�Q�F�������Ȃ��ŗւ����
	cutRing.MakeRing();
//	cutRing.Print(DSTR);
//	DSTR << "contact center:" << center << " normal:" << normal << "  vtxs:" << std::endl;
	for(CDQHLine<CDCutLine>* vtx = cutRing.vtxs.begin; vtx!=cutRing.vtxs.end; ++vtx){
		if (vtx->deleted) continue;
		Vec3d pos;
		pos.sub_vector(1, Vec2d()) = vtx->normal / vtx->dist;
		pos = cutRing.local * pos;
		Matrix3d local;
		cutRing.local.Ori().ToMatrix(local);
		engine->points.push_back(DBG_NEW PHContactPoint(local, this, pos, solid0, solid1));
//		DSTR << "  " << pos << std::endl;
	}
}
#define USE_VOLUME

#ifndef USE_VOLUME
bool PHShapePair::Detect(unsigned ct, PHSolidAux* solid0, PHSolidAux* solid1){
	UpdateShapePose(solid0->solid->GetPose() * shape[0]->GetPose(), solid1->solid->GetPose() * shape[1]->GetPose());
	CDConvex* conv[2] = { (CDConvex*)shape[0], (CDConvex*)shape[1], };
	Vec3d sep;
	bool r = FindCommonPoint(conv[0], conv[1], shapePoseW[0], shapePoseW[1], sep, closestPoint[0], closestPoint[1]);
	const double depthEpsilon = 0.01;
	if (r){
		commonPoint = shapePoseW[0] * closestPoint[0];
		if (lastContactCount == unsigned(ct-1)) state = CONTINUE;
		else state = NEW;
		lastContactCount = ct;

		//	�@�������߂�
		if (state == NEW){
			normal = solid0->solid->GetPointVelocity(commonPoint) - solid1->solid->GetPointVelocity(commonPoint);
			double norm = normal.norm();
			if (norm<1e-10){
				normal = solid1->solid->GetCenterPosition() - solid0->solid->GetCenterPosition();
				double norm = normal.norm();
				if (norm<1e-10){
					normal = Vec3d(1,0,0);
				}
			}
			normal.unitize();
			depth = depthEpsilon;
/*
			if (normal.y < 0.8){
				DSTR << std::endl;
				DSTR << (state == NEW ? "new" : "cont");
				DSTR << normal << std::endl;
			} 
*/
		}
		//	�O��̖@���̌����ɓ������āC�ŋߖT�_�����߂�
		if (depth < depthEpsilon) depth = depthEpsilon;
		Posed trans;
		Vec3f n;
		while(1) {
			depth *= 2;							//	�]�T�����āC�[����2�{������
			trans = shapePoseW[1];				//	�������s��
			trans.Pos() += depth * normal;
			FindClosestPoints(conv[0], conv[1], shapePoseW[0], trans, closestPoint[0], closestPoint[1]);
			center = shapePoseW[0] * closestPoint[0];
			n = trans * closestPoint[1] - center;
			if (n.square() > 1e-5) break;
		}
		depth = depth - n.norm();		//	������������ - 2�_�̋���
		normal = n.unit();
		center -= 0.5f*depth*normal;
//		DSTR << "Normal:" << normal << " center:" << center << " depth:" << depth << std::endl;
		#ifdef _DEBUG
		if (!finite(normal.norm())){
			//DSTR << "Error: Wrong normal:" << normal << std::endl;
			//DSTR << trans;
			//DSTR << closestPoint[0] << closestPoint[1] << std::endl;
			FindClosestPoints(conv[0], conv[1], shapePoseW[0], trans, closestPoint[0], closestPoint[1]);
		}
		#endif
	}
	return r;
}
#else
bool PHShapePair::Detect(unsigned ct, PHSolidAux* solid0, PHSolidAux* solid1){
	return CDShapePair::Detect(ct);
}
#endif
//--------------------------------------------------------------------------
#ifndef USE_VOLUME	//	�̐ς��g��Ȃ��ڐG����
bool PHConstraintEngine::PHSolidPair::Detect(PHConstraintEngine* engine){
	if(!bEnabled)return false;

	unsigned ct = OCAST(PHScene, engine->GetScene())->GetCount();
	// �����ꂩ��Solid�Ɍ`�󂪊��蓖�Ă��Ă��Ȃ��ꍇ�͐ڐG�Ȃ�
	PHSolid *s0 = solid[0]->solid, *s1 = solid[1]->solid;
	if(s0->NShape() == 0 || s1->NShape() == 0) return false;

	// �S�Ă�shape pair�ɂ��Č����𒲂ׂ�
	bool found = false;
	for(int i = 0; i < (int)(s0->shapes.size()); i++){
		for(int j = 0; j < (int)(s1->shapes.size()); j++){
			PHShapePair& sp = shapePairs.item(i, j);
			//����shape pair�̌�������/�@���ƐڐG�̈ʒu�����߂�D
			if(sp.Detect(ct, solid[0], solid[1])){
				found = true;
				//	��������2�̓ʌ`���ڐG�ʂŐ؂������̐؂���̌`�����߂�
				sp.EnumVertex(engine, ct, solid[0], solid[1]);
				DSTR << sp.normal;
			}
		}
	}
	return found;
}

//--------------------------------------------------------------------------
#else	//	�̐ς��g���ڐG����
bool PHConstraintEngine::PHSolidPair::Detect(PHConstraintEngine* engine){
	if(!bEnabled)return false;
	static CDContactAnalysis analyzer;

	unsigned ct = OCAST(PHScene, engine->GetScene())->GetCount();
	
	// �����ꂩ��Solid�Ɍ`�󂪊��蓖�Ă��Ă��Ȃ��ꍇ�̓G���[
	if(solid[0]->solid->NShape() == 0 || solid[1]->solid->NShape() == 0)
		return false;

	// �S�Ă�shape pair�ɂ��Č����𒲂ׂ�
	bool found = false;
	for(int i = 0; i < (int)(solid[0]->solid->shapes.size()); i++)for(int j = 0; j < (int)(solid[1]->solid->shapes.size()); j++){
		PHShapePair& sp = shapePairs.item(i, j);
		sp.UpdateShapePose(solid[0]->solid->GetPose(), solid[1]->solid->GetPose());

		if(sp.Detect(ct, solid[0], solid[1])){
			found = true;
			analyzer.FindIntersection(&sp);			//�����`��̌v�Z
			analyzer.CalcNormal(&sp);				//�����̖@���ƒ��S�𓾂�

			//�ڐG�_�̍쐬�F
			//�����`����\�����钸�_��analyzer.planes.begin����end�܂ł̓�deleted==false�̂���
			typedef CDQHPlanes<CDContactAnalysisFace>::CDQHPlane Plane;
			static std::vector<ContactVertex> isVtxs;
			isVtxs.clear();
			for(Plane* p = analyzer.planes.begin; p != analyzer.planes.end; p++){
				if(p->deleted) continue;
				isVtxs.push_back(p->normal / p->dist);
			}
			ContactVertex::ex = (-0.1 < sp.normal.z && sp.normal.z < 0.1) ?
				sp.normal ^ Vec3f(0,0,1) : sp.normal ^ Vec3f(1,0,0);
			ContactVertex::ex.unitize();
			ContactVertex::ey = sp.normal ^ ContactVertex::ex;

			//	���ׂĂ̐ڐG�_���܂ލŏ��̓ʑ��p�`
			static CDQHLines<ContactVertex> supportConvex(1000);
			supportConvex.Clear();
			supportConvex.epsilon = 0.01f;

			static std::vector<ContactVertex*> isVtxPtrs;
			isVtxPtrs.clear();
			isVtxPtrs.resize(isVtxs.size());
			for(size_t i=0; i<isVtxPtrs.size(); ++i) isVtxPtrs[i] = &isVtxs[i];
			supportConvex.CreateConvexHull(&isVtxPtrs.front(), &isVtxPtrs.back()+1);
			
			typedef CDQHLines<ContactVertex>::CDQHLine Line;
//#define DEBUG_CONTACTOUT
#ifdef DEBUG_CONTACTOUT
			int n = engine->points.size();
#endif
			for(Line* l = supportConvex.begin; l!=supportConvex.end; ++l){
				//if (l->deleted) continue;
				Vec3d v = *l->vtx[0]+sp.commonPoint;
				engine->points.push_back(DBG_NEW PHContactPoint(&sp, v, solid[0], solid[1]));
			}
#ifdef DEBUG_CONTACTOUT
			//DSTR << engine->points.size()-n << " contacts:";
			for(unsigned i=n; i<engine->points.size(); ++i){
				//DSTR << engine->points[i].pos;
			}
			//DSTR << std::endl;
#endif
		}
	}
	return found;
}
#endif

//----------------------------------------------------------------------------
// PHConstraintEngine
OBJECTIMP(PHConstraintEngine, PHEngine);

PHConstraintEngine::PHConstraintEngine(){
	ready = false;
	max_iter_dynamics = 5;
	max_iter_correction = 6;
	//step_size = 1.0;
	//converge_criteria = 0.00000001;
	max_error = 1.0;
}

PHConstraintEngine::~PHConstraintEngine(){

}

void PHConstraintEngine::Clear(PHScene* s){
	points.clear();
	joints.clear();
	solidPairs.clear();
	solids.clear();
}

void PHConstraintEngine::Add(PHSolid* s){
	if(solids.Find(s) == solids.end()){
		solids.push_back(DBG_NEW PHSolidAux());
		solids.back()->solid = s;
		Invalidate();
	}
}

void PHConstraintEngine::Remove(PHSolid* s){
	PHSolidAuxs::iterator is = solids.Find(s);
	if(is != solids.end()){
		solids.erase(is);
		Invalidate();
	}
}

PHJoint* PHConstraintEngine::AddJoint(PHSolid* lhs, PHSolid* rhs, const PHJointDesc& desc){
	PHSolidAuxs::iterator islhs, isrhs;
	islhs = solids.Find(lhs);
	isrhs = solids.Find(rhs);
	if(islhs == solids.end() || isrhs == solids.end())
		return NULL;
	
	PHJoint* joint = NULL;
	switch(desc.type){
	case PHJointDesc::JOINT_HINGE:
		joint = DBG_NEW PHHingeJoint();
		joint->Init(*islhs, *isrhs, desc);
		break;
	}
	joints.push_back(joint);

	//�֐߂łȂ���ꂽ���̊Ԃ̐ڐG�͖�����
	//EnableContact(lhs, rhs, false);

	return joint;
}

void PHConstraintEngine::EnableContact(PHSolid* lhs, PHSolid* rhs, bool bEnable){
	PHSolidAuxs::iterator ilhs = solids.Find(lhs), irhs = solids.Find(rhs);
	if(ilhs == solids.end() || irhs == solids.end())
		return;
	int i = ilhs - solids.begin(), j = irhs - solids.begin();
	if(i > j)swap(i, j);
	if(!ready || i >= solidPairs.height() || j >= solidPairs.width())
		Init();
	solidPairs.item(i, j).bEnabled = bEnable;
}

void PHConstraintEngine::Init(){
	solids.Init();

	//solidPairs�̍�蒼��
	int i, j, N = solids.size(), Nold = solidPairs.height();
	PHSolidPairs oldPairs;
	oldPairs.resize(Nold, Nold);	//private�ϐ�height_��width_��ς����������c
	oldPairs.swap(solidPairs);

    solidPairs.resize(N, N);
	for(i = 0; i < N-1; i++)for(j = i+1; j < N; j++)
		solidPairs.item(i, j).Init(solids[i], solids[j]);

	//Enable�t���O�������p��
	if(!oldPairs.empty()){
		vector<int> index_map(Nold);
		PHSolidAuxs::iterator it;
		for(i = 0; i < Nold-1; i++){
			it = find(solids.begin(), solids.end(), oldPairs.item(i, i+1).solid[0]);
			index_map[i] = (it != solids.end() ? it - solids.begin() : -1);
		}
		it = find(solids.begin(), solids.end(), oldPairs.item(Nold-2, Nold-1).solid[1]);
		index_map[Nold-1] = (it != solids.end() ? it - solids.begin() : -1);

		for(i = 0; i < Nold-1; i++){
			if(index_map[i] == -1)continue;
			for(j = i+1; j < Nold; j++){
				if(index_map[j] == -1)continue;
				solidPairs.item(index_map[i], index_map[j]).bEnabled = oldPairs.item(i, j).bEnabled;
			}
		}
	}
	
	ready = true;
}

// AABB�Ń\�[�g���邽�߂̍\����
struct Edge{
	float edge;				///<	�[�̈ʒu(�O���[�o���n)
	int	index;				///<	���� solid�̈ʒu
	bool bMin;				///<	���[�Ȃ�true
	bool operator < (const Edge& s) const { return edge < s.edge; }
};
typedef std::vector<Edge> Edges;

bool PHConstraintEngine::DetectPenetration(){
	/* �ȉ��̗���Ō��������߂�
		1. Solid��BBox���x���ł̌�������(z���\�[�g)�D�����̂�����̖����g�����O
		2. �eSolid�̑g�ɂ���
			2a. Shape��BBox���x���ł̌������� (������)
			2b. �eShape�̑g�ɂ���GJK�Ō����`��𓾂�
			2c. �����`�󂩂�@�������߁A�@���Ɋւ��Č`����ˉe���C���̒��_��ڐG�_�Ƃ���
			2d. ����ꂽ�ڐG�_����PHContactPoints�ɋl�߂Ă���
	 */
	points.clear();
	int N = solids.size();

	//1. BBox���x���̏Փ˔���
	Vec3f dir(0,0,1);
	Edges edges;
	edges.resize(2 * N);
	Edges::iterator eit = edges.begin();
	for(int i = 0; i < N; ++i){
		solids[i]->solid->GetBBoxSupport(dir, eit[0].edge, eit[1].edge);
		eit[0].index = i; eit[0].bMin = true;
		eit[1].index = i; eit[1].bMin = false;
		eit += 2;
	}
	std::sort(edges.begin(), edges.end());
	//�[���猩�Ă����āC�ڐG�̉\��������m�[�h�̔��������D
	typedef std::set<int> SolidSet;
	SolidSet cur;							//	���݂�Solid�̃Z�b�g
	bool found = false;
	for(Edges::iterator it = edges.begin(); it != edges.end(); ++it){
		if (it->bMin){						//	���[��������C���X�g���̕��̂Ɣ���
			for(SolidSet::iterator itf=cur.begin(); itf != cur.end(); ++itf){
				int f1 = it->index;
				int f2 = *itf;
				if (f1 > f2) std::swap(f1, f2);
				//2. Solid��Solid�̏Փ˔���
				found |= solidPairs.item(f1, f2).Detect(this);
			}
			cur.insert(it->index);
		}else{
			cur.erase(it->index);			//	�I�[�Ȃ̂ō폜�D
		}
	}
	return found;
}
void PHConstraintEngine::SetupDynamics(double dt){
	solids.SetupDynamics(dt);
	points.SetupDynamics(dt);
	joints.SetupDynamics(dt);
}
void PHConstraintEngine::SetupCorrection(double dt){
	solids.SetupCorrection();
	points.SetupCorrection(dt, max_error);
	joints.SetupCorrection(dt, max_error);
}
void PHConstraintEngine::IterateDynamics(){
	double dfsum = 0.0;
	int count = 0;
	while(true){
		if(count == max_iter_dynamics){
			//DSTR << "max count." << " iteration count: " << count << " dfsum: " << dfsum << endl;
			break;
		}
		points.IterateDynamics();
		joints.IterateDynamics();

		count++;
		//�I������
		/*if(dfsum < converge_criteria){
			DSTR << "converged." << " iteration count: " << count << " dfsum: " << dfsum << endl;
			break;
		}*/
	}
}
void PHConstraintEngine::IterateCorrection(){
	double dFsum = 0.0;
	int count = 0;
	while(true){
		if(count == max_iter_correction){
			//DSTR << "max count." << " iteration count: " << count << " dFsum: " << dFsum << endl;
			break;
		}
		dFsum = 0.0;
		
		points.IterateCorrection();
		joints.IterateCorrection();

		count++;
		//�I������
		/*if(dFsum < converge_criteria){
			DSTR << "converged." << " iteration count: " << count << " dFsum: " << dFsum << endl;
			break;
		}*/
	}
}

void PHConstraintEngine::UpdateSolids(double dt){
	PHSolidAuxs::iterator is;
	PHSolidAux* aux;
	PHSolid* solid;
	Vec3d vnew, wnew;
	for(is = solids.begin(); is != solids.end(); is++){
		aux = *is;
        solid = aux->solid;
		//velocity update
		vnew = aux->v + aux->dv0 + aux->dv;
		wnew = aux->w + aux->dw0 + aux->dw;
		solid->SetVelocity       (solid->GetOrientation() * vnew);
		solid->SetAngularVelocity(solid->GetOrientation() * wnew);
		//position update
		solid->SetCenterPosition(solid->GetCenterPosition() + solid->GetVelocity() * dt + solid->GetOrientation() * aux->dV);
		solid->SetOrientation(
			(solid->GetOrientation() * Quaterniond::Rot(wnew * dt + aux->dW)).unit()
		);
		//solid->SetOrientation((solid->GetOrientation() + solid->GetOrientation().Derivative(solid->GetOrientation() * is->dW)).unit());
		solid->SetOrientation((solid->GetOrientation() * Quaterniond::Rot(/*solid->GetOrientation() * */aux->dW)).unit());
		solid->SetUpdated(true);
	}
}

//#include <windows.h>

void PHConstraintEngine::Step(){
	if(!ready)
		Init();
	//LARGE_INTEGER freq, val[2];
	//QueryPerformanceFrequency(&freq);
	
	//���������m
	//QueryPerformanceCounter(&val[0]);
	DetectPenetration();
	//QueryPerformanceCounter(&val[1]);
	//DSTR << "cd " << (double)(val[1].QuadPart - val[0].QuadPart)/(double)(freq.QuadPart) << endl;

	double dt = OCAST(PHScene, GetScene())->GetTimeStep();

	//QueryPerformanceCounter(&val[0]);
	SetupDynamics(dt);
	//QueryPerformanceCounter(&val[1]);
	//DSTR << "sd " << (double)(val[1].QuadPart - val[0].QuadPart)/(double)(freq.QuadPart) << endl;

	//QueryPerformanceCounter(&val[0]);
	IterateDynamics();
	//DSTR << points.size() << endl;
	//for(PHConstraints::iterator it = points.begin(); it != points.end(); it++)
	//	DSTR << (*it)->fv << (*it)->fw << endl;
	//QueryPerformanceCounter(&val[1]);
	//DSTR << "id " << (double)(val[1].QuadPart - val[0].QuadPart)/(double)(freq.QuadPart) << endl;

	//QueryPerformanceCounter(&val[0]);
	SetupCorrection(dt);
	//QueryPerformanceCounter(&val[1]);
	//DSTR << "sc " << (double)(val[1].QuadPart - val[0].QuadPart)/(double)(freq.QuadPart) << endl;

	//QueryPerformanceCounter(&val[0]);
	IterateCorrection();
	//QueryPerformanceCounter(&val[1]);
	//DSTR << "ic " << (double)(val[1].QuadPart - val[0].QuadPart)/(double)(freq.QuadPart) << endl;

	UpdateSolids(dt);

}

#undef SUBMAT
#undef SUBVEC

}

