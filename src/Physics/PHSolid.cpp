#include "Physics.h"
#pragma hdrstop
#include <float.h>

#define _DEBUG

using namespace PTM;
namespace Spr{

///////////////////////////////////////////////////////////////////
//	PHSolid
OBJECTIMP(PHSolid, Object);
IF_IMP(PHSolid, Object);

PHSolid::PHSolid(const PHSolidDesc& desc):PHSolidDesc(desc){
	integrationMode = PHINT_SIMPLETIC;
	inertia_inv = inertia.inv();
}
CDShapeIf* PHSolid::CreateShape(const CDShapeDesc& desc){
	CDShapeIf* rv = ICAST(PHSceneIf, GetScene())->CreateShape(desc);
	AddShape(rv);
	return rv;
}
ObjectIf* PHSolid::CreateObject(const IfInfo* info, const void* desc){
	if (CDConvexMeshIf::GetIfInfoStatic() == info){
		return CreateShape(*(CDConvexMeshDesc*)desc);
	}
	return NULL;
}

void PHSolid::CalcBBox(){
	Vec3f bboxMin = Vec3f(FLT_MAX, FLT_MAX, FLT_MAX);
	Vec3f bboxMax = Vec3f(-FLT_MAX,-FLT_MAX,-FLT_MAX);
	for(CDShapes::iterator it=shapes.begin(); it != shapes.end(); ++it){
		(*it)->CalcBBox(bboxMin, bboxMax);
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

void PHSolid::Step(){
	force = nextForce;
	torque = nextTorque;
	nextForce.clear();
	nextTorque.clear();

	//���ɑ��̃G���W���ɂ���čX�V�������ꂽ�ꍇ�͐ϕ����s��Ȃ�
	if(IsUpdated())
		return;

	PHScene* s = OCAST(PHScene, GetScene());
	double dt = s->GetTimeStep();
	assert(GetIntegrationMode() != PHINT_NONE);
#ifdef _DEBUG
	if (!_finite(velocity.norm()) || velocity.norm() > 100 || angVelocity.norm() > 100){	
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
	shapes.push_back(OCAST(CDShape,shape));
	CalcBBox();
	//�ڐG�G���W����invalidate
	PHScene* scene = OCAST(PHScene,GetScene());
	assert(scene);
	switch(scene->contactSolver){
	case PHScene::SOLVER_PENALTY:{
		PHPenaltyEngine* pe;
		scene->engines.Find(pe);
		if(pe) pe->Invalidate();
	}break;
	case PHScene::SOLVER_CONSTRAINT:{
		PHConstraintEngine* ce;
		scene->engines.Find(ce);
		if(ce) ce->Invalidate();
	}break;
	}
}

void PHSolid::SetGravity(bool bOn){
	PHScene* ps = OCAST(PHScene,GetScene());
	PHGravityEngine* ge;
	ps->engines.Find(ge);
	if (bOn == true){
		if (ge->solids.Find(this)){
			ge->solids.push_back(this);
		}
	}else{
		ge->solids.Erase(this);
	}
}
int PHSolid::NShape(){
	return shapes.size();
}
CDShapeIf** PHSolid::GetShapes(){
	return (CDShapeIf**)(void*)&shapes.front();
}

//----------------------------------------------------------------------------
//	PHSolidContainer
//
OBJECTIMP(PHSolidContainer, PHEngine);
bool PHSolidContainer::AddChildObject(Object* o, PHScene* s){
	if (DCAST(PHSolid, o)){
		solids.push_back((PHSolid*)o);
		return true;
	}
	return false;
}
bool PHSolidContainer::DelChildObject(Object* o, PHScene* s){
	PHSolid* so = DCAST(PHSolid, o);
	if (so){
		solids.Erase(so);
		return true;
	}
	return false;
}

void  PHSolidContainer::Step(){
	for(PHSolids::iterator it = solids.begin(); it != solids.end(); ++it)
		(*it)->Step();
}

//----------------------------------------------------------------------------
//	PHSolidInitializer
//
OBJECTIMP(PHSolidInitializer, PHEngine);
bool PHSolidInitializer::AddChildObject(Object* o, PHScene* s){
	if (DCAST(PHSolid, o)){
		solids.push_back((PHSolid*)o);
		return true;
	}
	return false;
}
bool PHSolidInitializer::DelChildObject(Object* o, PHScene* s){
	PHSolid* so = DCAST(PHSolid, o);
	if (so){
		solids.Erase(so);
		return true;
	}
	return false;
}
void PHSolidInitializer::Step(){
	for(PHSolids::iterator it = solids.begin(); it != solids.end(); it++)
		(*it)->SetUpdated(false);
}	

}
