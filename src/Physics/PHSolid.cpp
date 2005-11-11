#include "Physics.h"
#pragma hdrstop
#include <float.h>

using namespace PTM;
namespace Spr{

///////////////////////////////////////////////////////////////////
//	PHSolid
OBJECTIMP(PHSolid, Object);

PHSolid::PHSolid(){
	mass = 1.0;
	inertia = Matrix3d::Unit();
	integrationMode = PHINT_SIMPLETIC;
}

void PHSolid::Step(){
	PHScene* s = ACAST(PHScene, scene);
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
	Vec3d dv, dw;
	switch(GetIntegrationMode()){
	case PHINT_EULER:
		//���s�ړ��ʂ̐ϕ�
		SetCenterPosition(GetCenterPosition() + velocity * dt);
		velocity += force * (dt / mass);
		//�p���x����N�E�H�[�^�j�I���̎��Ԕ��������߁A�ϕ��A���K��
		quat += quat.derivative(angVelocity) * dt;
		quat.unitize();
		torque		= quat.conjugated() * torque;			//�g���N�Ɗp���x�����[�J�����W��
		angVelocity = quat.conjugated() * angVelocity;
		angVelocity += Euler(inertia, torque, angVelocity) * dt;	//�I�C���[�̉^��������
		torque = quat * torque;						//�g���N�Ɗp���x�����[���h��
		angVelocity = quat * angVelocity;
		break;
	case PHINT_ARISTOTELIAN:{
		SetCenterPosition(GetCenterPosition() + velocity * dt);
		velocity = force / mass;		//���x�͗͂ɔ�Ⴗ��
		Vec3d tq = quat.conjugated() * torque;	//�g���N�����[�J����
		angVelocity = quat * (inertia_inv * tq);	//�p���x�̓g���N�ɔ�Ⴗ��
		//�N�E�H�[�^�j�I����ϕ��A���K��
		quat += quat.derivative(angVelocity) * dt;
		quat.unitize();
		}break;
	case PHINT_SIMPLETIC:{
		//	x(dt) = x(0) + dt*v(0)/m
		//	v(dt) = v(0) + dt*f(dt)
		//��]�ʂ̐ϕ�
		torque		= quat.conjugated() * torque;				//	�g���N�Ɗp���x�����[�J�����W��
		angVelocity = quat.conjugated() * angVelocity;

		dw = Euler(inertia, torque, angVelocity) * dt;			//�p���x�ω���
		angVelocity += dw;
		Quaterniond dq = Quaterniond::Rot(angVelocity * dt);
		Vec3d dp = quat * (dq*(-center) - (-center));
		quat = quat * dq;
		quat.unitize();
		angVelocity += dw;										//�p���x�̐ϕ�
		torque = quat * torque;									//�g���N�Ɗp���x�����[���h��
		angVelocity = quat * angVelocity;
		//���s�ړ��ʂ̐ϕ�
		velocity += force * (dt / mass);								//	���x�̐ϕ�
		SetCenterPosition(GetCenterPosition() + velocity * dt + dp);	//	�ʒu�̐ϕ�
		}break;
	case PHINT_ANALYTIC:{
		//��]�ʂ̐ϕ�
		//��]�͉�͓I�ɐϕ��ł��Ȃ��̂ŁA�`���I�Ɂ��̌�������]�̏ꍇ�ɓ��Ă͂߂�
		torque		= quat.conjugated() * torque;					//�g���N�Ɗp���x�����[�J�����W��
		angVelocity = quat.conjugated() * angVelocity;
		dw = Euler(inertia, torque, angVelocity) * dt;			//�p���x�ω���

		Quaterniond dq = Quaterniond::Rot((angVelocity+0.5*dw) * dt);
		Vec3d dp = quat * (dq*(-center) - (-center));
		quat = quat * dq;
		quat.unitize();

		angVelocity += dw;										//�p���x�̐ϕ�
		torque = quat * torque;									//�g���N�Ɗp���x�����[���h��
		angVelocity = quat * angVelocity;
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
		torque		= quat.conjugated() * torque;					//�g���N�Ɗp���x�����[�J�����W��
		angVelocity = quat.conjugated() * angVelocity;
		_angvel[0]	= angVelocity;
		_angacc[0]	= Euler(inertia, torque, _angvel[0]);
		_angvel[1]	= _angvel[0] + _angacc[0] * dt;
		_angacc[1]	= Euler(inertia, torque, _angvel[1]);
		quat += quat.derivative(quat * (_angvel[0] + _angvel[1]) / 2.0) * dt;
		quat.unitize();
		angVelocity = quat * (angVelocity + ((_angacc[0] + _angacc[1]) * (dt / 2.0)));
		torque = quat * torque;
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
		quat += quat.derivative(quat * (_angvel[0] + 2.0 * (_angvel[1] + _angvel[2]) + _angvel[3]) / 6.0) * dt;
		quat.unitize();
		angVelocity = quat * (angVelocity + (_angacc[0] + 2.0 * (_angacc[1] + _angacc[2]) + _angacc[3]) * (dt / 6.0));
		torque = quat * torque;
		
		break;
	}
}

void PHSolid::AddForce(Vec3d f)
{
	force += f;
}

void PHSolid::AddForce(Vec3d f, Vec3d r){
	torque += (r - (quat*center+pos)) ^ f;
	force += f;
}

/*void PHSolid::AddForceLocal(Vec3d f, Vec3d r){
	torque += cross(quat * (r - center), f);
	force += f;
}*/

void PHSolid::ClearForce(){
	force.clear();
	torque.clear();
}

//----------------------------------------------------------------------------
//	PHSolverBase
//
OBJECTIMPABST(PHSolverBase, PHEngine);

//----------------------------------------------------------------------------
//	PHSolidContainer
//
OBJECTIMP(PHSolidContainer, PHSolverBase);
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
	for(PHSolids::iterator it = solids.begin(); it != solids.end(); ++it){
		(*it)->Step();
	}
}
void PHSolidContainer::ClearForce(){
	for(PHSolids::iterator it = solids.begin(); it != solids.end(); ++it){
		(*it)->ClearForce();
	}
}

//----------------------------------------------------------------------------
//	PHSolidClearForce
//
OBJECTIMP(PHSolidClearForce, PHEngine);
void  PHSolidClearForce::Step(){
	for(PHSolvers::iterator it = solvers.begin(); it!=solvers.end(); ++it){
		(*it)->ClearForce();
	}
}


}
