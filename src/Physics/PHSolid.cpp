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
#include <float.h>

using namespace PTM;
namespace Spr{

///////////////////////////////////////////////////////////////////
//	PHSolid
IF_OBJECT_IMP(PHSolid, SceneObject);

PHSolid::PHSolid(const PHSolidDesc& desc, SceneIf* s):PHSolidDesc(desc){
	integrationMode = PHINT_SIMPLETIC;
	inertia_inv = inertia.inv();
	treeNode = NULL;
	bFrozen = false;
	if (s){ SetScene(s); }
}
void PHSolid::SetGravity(bool bOn){
	PHScene* s = DCAST(PHScene, GetScene());
	PHGravityEngine* ge;
	s->engines.Find(ge);
	if (bOn) ge->AddChildObject(s->Cast());
	else ge->solids.Erase(this);
}

CDShapeIf* PHSolid::CreateShape(const IfInfo*  info, const CDShapeDesc& desc){
	CDShapeIf* rv = DCAST(PHScene, GetScene())->CreateShape(info, desc);
	if (rv){
		AddShape(rv);
	}
	return rv;
}
ObjectIf* PHSolid::CreateObject(const IfInfo* info, const void* desc){
	ObjectIf* rv = SceneObject::CreateObject(info, desc);
	if (!rv){
		if (info->Inherit(CDShapeIf::GetIfInfoStatic())){
			rv = CreateShape(info, *(CDShapeDesc*)desc);
		}
	}
	return rv;
}
bool PHSolid::AddChildObject(ObjectIf* obj){
	if (DCAST(CDShapeIf, obj)){
		AddShape(DCAST(CDShapeIf, obj));
		return true;
	}
	return false;
}

Vec3d PHSolid::GetDeltaPosition() const {
	PHScene* s = DCAST(PHScene, nameManager);
	return velocity * s->GetTimeStep();
}
Vec3d PHSolid::GetDeltaPosition(const Vec3d& p) const {
	PHScene* s = DCAST(PHScene, nameManager);
	double dt = s->GetTimeStep();
	Quaterniond rot = Quaterniond::Rot(angVelocity*dt);
	return velocity*dt + rot*(p-center);
}
void PHSolid::CalcBBox(){
	Vec3f bboxMin = Vec3f(FLT_MAX, FLT_MAX, FLT_MAX);
	Vec3f bboxMax = Vec3f(-FLT_MAX,-FLT_MAX,-FLT_MAX);
	for(int i = 0; i < (int)shapes.size(); i++){
		shapes[i].shape->CalcBBox(bboxMin, bboxMax, shapes[i].pose);
	}
	if (bboxMin.X() == FLT_MAX){
		bbox.SetBBoxMinMax(Vec3f(0,0,0), Vec3f(-1,-1,-1));
	}else{
		bbox.SetBBoxMinMax(bboxMin, bboxMax);
	}
//	DSTR << GetName() << bbox.GetBBoxMin() << bbox.GetBBoxMax() << std::endl;
}
void PHSolid::GetBBoxSupport(const Vec3f& dir, float& minS, float& maxS){
		bbox.GetSupport(GetOrientation().Inv()*dir, minS, maxS);
		float c = Vec3f(GetFramePosition()) * dir;
		minS += c;
		maxS += c;
}

void PHSolid::UpdateCacheLCP(double dt){
	if(mass != 0)
		minv = GetMassInv();
	Iinv = GetInertiaInv();
	Quaterniond qc = GetOrientation().Conjugated();
	f.v() = qc * nextForce;
	f.w() = qc * nextTorque;
	v.v() = qc * GetVelocity();
	v.w() = qc * GetAngularVelocity();
	
	// �c���[�ɑ�����ꍇ��PHRootNode::SetupDynamics��dv���v�Z�����
	if(treeNode)return;
	
	if(IsDynamical() && !IsFrozen()){
		dv.v() = minv * f.v() * dt;
		dv.w() = Iinv * (f.w() - v.w() % (GetInertia() * v.w())) * dt;
	}
	else{
		dv.clear();
	}
	dV.clear();
}

void PHSolid::UpdateCachePenalty(int c){
	if ((unsigned)c == count) return;
	count = c;
	vel = GetVelocity();
	angVel = GetAngularVelocity();
	lastPos = pos;
	pos = GetFramePosition();
	lastOri = ori;
	ori = GetOrientation();
	cog = ori * GetCenterOfMass() + pos;
}

void PHSolid::UpdateVelocity(double dt){
	SpatialVector vold = v;
	if(IsDynamical() && !IsFrozen()){
		v += dv;
		//oldVel = GetVelocity();
		//oldAngVel = GetAngularVelocity();
		SetVelocity       (GetOrientation() * v.v());
		SetAngularVelocity(GetOrientation() * v.w());
	}
	// �t���[�Y����
	/*	�����蔻�肪�s����ɂȂ�̂ŁA�R�����g�A�E�g���܂����B(by hase)
	
	if(!IsFrozen()){
		if(vold.square() < engine->freezeThreshold && v.square() < engine->freezeThreshold){
			SetFrozen(true);
		}
	}
*/
}
void PHSolid::UpdatePosition(double dt){
	if(IsFrozen())return;
	SetCenterPosition(GetCenterPosition() + GetVelocity() * dt + GetOrientation() * dV.v());
	SetOrientation((GetOrientation() * Quaterniond::Rot(v.w() * dt + dV.w())).unit());
	//solid->SetOrientation((solid->GetOrientation() + solid->GetOrientation().Derivative(solid->GetOrientation() * is->dW)).unit());
	//solid->SetOrientation((solid->GetOrientation() * Quaterniond::Rot(/*solid->GetOrientation() * */info->dW)).unit());
}

void PHSolid::Step(){
	force = nextForce;
	torque = nextTorque;
	nextForce.clear();
	nextTorque.clear();

	//���ɑ��̃G���W���ɂ���čX�V�������ꂽ�ꍇ�͐ϕ����s��Ȃ�
	if(IsUpdated())
		return;

	PHScene* s = DCAST(PHScene, GetScene());
	double dt = s->GetTimeStep();
	assert(GetIntegrationMode() != PHINT_NONE);
#ifdef _DEBUG
	if (!finite(velocity.norm()) || velocity.norm() > 100 || angVelocity.norm() > 100){	
		DSTR << "Warning: solid '" << GetName() << "' has a very fast velocity. v:" << velocity << "w:" << angVelocity << std::endl;
	}
#endif
	//k1 = f(y);
	//k2 = f(y + k1 * h / 2);
	//k3 = f(y + k2 * h / 2);
	//k4 = f(y + k3);
	//y += (k1 + 2 * k2 + 2 * k3 + k4) * h / 6;
	//	�ϕ��v�Z
	Vec3d dv, dw;				//<	���x�E�p���x�̕ω���
	Vec3d	_angvel[4];			//<	���l�ϕ��W��
	Vec3d	_angacc[4];
	switch(GetIntegrationMode()){
	case PHINT_EULER:
		//���s�ړ��ʂ̐ϕ�
		SetCenterPosition(GetCenterPosition() + velocity * dt);
		velocity += force * (dt / mass);
		//�p���x����N�E�H�[�^�j�I���̎��Ԕ��������߁A�ϕ��A���K��
		pose.Ori() += pose.Ori().Derivative(angVelocity) * dt;
		pose.Ori().unitize();
		torque		= pose.Ori().Conjugated() * torque;			//�g���N�Ɗp���x�����[�J�����W��
		angVelocity = pose.Ori().Conjugated() * angVelocity;
		angVelocity += Euler(inertia, torque, angVelocity) * dt;	//�I�C���[�̉^��������
		torque = pose.Ori() * torque;						//�g���N�Ɗp���x�����[���h��
		angVelocity = pose.Ori() * angVelocity;
		break;
	case PHINT_ARISTOTELIAN:{
		SetCenterPosition(GetCenterPosition() + velocity * dt);
		velocity = force / mass;		//���x�͗͂ɔ�Ⴗ��
		Vec3d tq = pose.Ori().Conjugated() * torque;	//�g���N�����[�J����
		angVelocity = pose.Ori() * (inertia_inv * tq);	//�p���x�̓g���N�ɔ�Ⴗ��
		//�N�E�H�[�^�j�I����ϕ��A���K��
		pose.Ori() += pose.Ori().Derivative(angVelocity) * dt;
		pose.Ori().unitize();
		}break;
	case PHINT_SIMPLETIC:{
		//	x(dt) = x(0) + dt*v(0)/m
		//	v(dt) = v(0) + dt*f(dt)
		//��]�ʂ̐ϕ�
		torque		= pose.Ori().Conjugated() * torque;				//	�g���N�Ɗp���x�����[�J�����W��
		angVelocity = pose.Ori().Conjugated() * angVelocity;

		dw = Euler(inertia, torque, angVelocity) * dt;			//�p���x�ω���
		angVelocity += dw;										//�p���x�̐ϕ�
		Quaterniond dq = Quaterniond::Rot(angVelocity * dt);
		Vec3d dp = pose.Ori() * (dq*(-center) - (-center));
		pose.Ori() = pose.Ori() * dq;
		pose.Ori().unitize();
		torque = pose.Ori() * torque;									//�g���N�Ɗp���x�����[���h��
		angVelocity = pose.Ori() * angVelocity;
		//���s�ړ��ʂ̐ϕ�
		velocity += force * (dt / mass);								//	���x�̐ϕ�
		SetCenterPosition(GetCenterPosition() + velocity * dt + dp);	//	�ʒu�̐ϕ�
		}break;
	case PHINT_ANALYTIC:{
		//��]�ʂ̐ϕ�
		//��]�͉�͓I�ɐϕ��ł��Ȃ��̂ŁA�`���I�Ɂ��̌�������]�̏ꍇ�ɓ��Ă͂߂�
		torque		= pose.Ori().Conjugated() * torque;					//�g���N�Ɗp���x�����[�J�����W��
		angVelocity = pose.Ori().Conjugated() * angVelocity;
		dw = Euler(inertia, torque, angVelocity) * dt;			//�p���x�ω���

		Quaterniond dq = Quaterniond::Rot((angVelocity+0.5*dw) * dt);
		Vec3d dp = pose.Ori() * (dq*(-center) - (-center));
		pose.Ori() = pose.Ori() * dq;
		pose.Ori().unitize();

		angVelocity += dw;										//�p���x�̐ϕ�
		torque = pose.Ori() * torque;									//�g���N�Ɗp���x�����[���h��
		angVelocity = pose.Ori() * angVelocity;
		//���s�ړ��ʂ̐ϕ��i��͉��Ɉ�v�j
		dv = force * (dt / mass);									//���x�ω���
		SetCenterPosition(GetCenterPosition() + (velocity+0.5*dv) * dt + dp);	//	�ʒu�̐ϕ�
		velocity += dv;												//���x�̐ϕ�
		}break;
	case PHINT_RUNGEKUTTA2:
		//���s�ړ��ʂ̐ϕ�(�܂��߂Ƀ����Q�N�b�^�����ɏ]���Ă����ʂ͓�������͉�)
		dv = force * (dt / mass);
		SetCenterPosition(GetCenterPosition() + (velocity + 0.5 * dv) * dt);
		velocity += dv;
		//��]�ʂ̌v�Z
		//��]�͉�͓I�ɐϕ��ł��Ȃ��̂ŁA�����Q�N�b�^�������g��
		torque		= pose.Ori().Conjugated() * torque;					//�g���N�Ɗp���x�����[�J�����W��
		angVelocity = pose.Ori().Conjugated() * angVelocity;
		_angvel[0]	= angVelocity;
		_angacc[0]	= Euler(inertia, torque, _angvel[0]);
		_angvel[1]	= _angvel[0] + _angacc[0] * dt;
		_angacc[1]	= Euler(inertia, torque, _angvel[1]);
		pose.Ori() += pose.Ori().Derivative(pose.Ori() * (_angvel[0] + _angvel[1]) / 2.0) * dt;
		pose.Ori().unitize();
		angVelocity = pose.Ori() * (angVelocity + ((_angacc[0] + _angacc[1]) * (dt / 2.0)));
		torque = pose.Ori() * torque;
		break;
	case PHINT_RUNGEKUTTA4:
		//���s�ړ��ʂ̐ϕ�(�܂��߂Ƀ����Q�N�b�^�����ɏ]���Ă����ʂ͓�������͉�)
		dv = force * (dt / mass);
		SetCenterPosition(GetCenterPosition() + (velocity + 0.5 * dv) * dt);
		velocity += dv;
		//��]�ʂ̌v�Z
		_angvel[0]	= angVelocity;
		_angacc[0]	= Euler(inertia, torque, _angvel[0]);
		_angvel[1]	= _angvel[0] + _angacc[0] * (dt / 2.0);
		_angacc[1]	= Euler(inertia, torque, _angvel[1]);
		_angvel[2]	= _angvel[0] + _angacc[1] * (dt / 2.0); 
		_angacc[2]	= Euler(inertia, torque, _angvel[2]);
		_angvel[3]	= _angvel[0] + _angacc[2] * dt;
		_angacc[3]	= Euler(inertia, torque, _angvel[3]);
		pose.Ori() += pose.Ori().Derivative(pose.Ori() * (_angvel[0] + 2.0 * (_angvel[1] + _angvel[2]) + _angvel[3]) / 6.0) * dt;
		pose.Ori().unitize();
		angVelocity = pose.Ori() * (angVelocity + (_angacc[0] + 2.0 * (_angacc[1] + _angacc[2]) + _angacc[3]) * (dt / 6.0));
		torque = pose.Ori() * torque;
		break;
	default:			
		/* DO NOTHING */		
		break;
	}
}
void PHSolid::AddTorque(Vec3d t){
	nextTorque += t; 
}
void PHSolid::AddForce(Vec3d f){
	nextForce += f;
}

void PHSolid::AddForce(Vec3d f, Vec3d r){
	nextTorque += (r - pose*center) ^ f;
	nextForce += f;
}

/*void PHSolid::AddForceLocal(Vec3d f, Vec3d r){
	torque += cross(pose.Ori() * (r - center), f);
	force += f;
}*/


void PHSolid::AddShape(CDShapeIf* shape){
	CDShape* sh = DCAST(CDShape, shape);
	shapes.push_back(CDShapeRefWithPose());
	shapes.back().shape = sh;
	CalcBBox();
	//�ڐG�G���W����shapePairs���X�V����
	PHScene* scene = DCAST(PHScene,GetScene());
	scene->penaltyEngine->UpdateShapePairs(this);
	scene->constraintEngine->UpdateShapePairs(this);
}

Posed	PHSolid::GetShapePose(int i){
	if(0 <= i && i < (int)shapes.size())
		return shapes[i].pose;
	return Posed();
}

void	PHSolid::SetShapePose(int i, const Posed& pose){
	if(0 <= i && i < (int)shapes.size()){
		shapes[i].pose = pose;
		CalcBBox();
	}
}
int PHSolid::NShape(){
	return shapes.size();
}
CDShapeIf* PHSolid::GetShape(int i){
	return shapes[i].shape->Cast();
}


//----------------------------------------------------------------------------
//	PHSolidContainer
//
OBJECT_IMP(PHSolidContainer, PHEngine);

PHSolidContainer::PHSolidContainer(){
}

bool PHSolidContainer::AddChildObject(ObjectIf* o){
	PHSolidIf* s = DCAST(PHSolidIf, o);
	if (s && std::find(solids.begin(), solids.end(), s) == solids.end()){
		solids.push_back(s);
		return true;
	}
	return false;
}
bool PHSolidContainer::DelChildObject(ObjectIf* o){
	PHSolidIf* so = DCAST(PHSolidIf, o);
	if (so){
		solids.Erase(so);
		return true;
	}
	return false;
}

void PHSolidContainer::Reset(){
	for(PHSolidIfs::iterator it = solids.begin(); it != solids.end(); ++it){
		PHSolid* s = XCAST(*it);
		s->SetUpdated(false);
	}
}

void PHSolidContainer::Step(){
	for(PHSolidIfs::iterator it = solids.begin(); it != solids.end(); ++it){
		PHSolid* s = XCAST(*it);
		s->Step();
	}
}

//----------------------------------------------------------------------------
//	PHSolidInitializer
//
OBJECT_IMP(PHSolidInitializer, PHEngine);
void PHSolidInitializer::Step(){
		container->Reset();	
}

}
