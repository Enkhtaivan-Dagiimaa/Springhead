#ifndef PHSOLID_H
#define PHSOLID_H

#include <Springhead.h>
#include <Foundation/Object.h>
#include <Physics/PHScene.h>
#include <Physics/PHEngine.h>

namespace Spr{;

enum PHIntegrationMode{
		PHINT_NONE,			//�ϕ����Ȃ�
		PHINT_ARISTOTELIAN,	//f = mv
		PHINT_EULER,		//�I�C���[�@
		PHINT_SIMPLETIC,
		PHINT_ANALYTIC,
		PHINT_RUNGEKUTTA2,	//�Q�������Q�N�b�^�@
		PHINT_RUNGEKUTTA4	//�S�������Q�N�b�^�@
};

///	����
class PHSolid : public Object, public PHSolidIf{
	Vec3d	_angvel[4];			///<	���l�ϕ��W��
	Vec3d	_angacc[4];
protected:
	double		mass;			///<	����
	Matrix3d	inertia;		///<	�����e���\��
	Matrix3d	inertia_inv;
	Vec3d		force;			///<	��				(World)
	Vec3d		torque;			///<	�g���N			(World)
	Vec3d		velocity;		///<	���x			(World)
	Vec3d		angVelocity;	///<	�p���x			(World)
	Vec3d		center;			///<	���ʒ��S�̈ʒu	(Local..frame��posture�n)
	Vec3d		pos;			///<	�ʒu			(World)
	Quaterniond quat;			///<	����			(World)

	///	�ϕ�����
	PHIntegrationMode integrationMode;

	//�I�C���[�̉^��������
	//�����s��͑Ίp�s���O��D
	Vec3d	Euler(const Matrix3d& I, const Vec3d& t, const Vec3d& w){
		return Vec3d(
			(t[0] - (I[2][2] - I[1][1]) * w.Y() * w.Z()) / I[0][0],
			(t[1] - (I[0][0] - I[2][2]) * w.Z() * w.X()) / I[1][1],
			(t[2] - (I[1][1] - I[0][0]) * w.X() * w.Y()) / I[2][2]);
	}
public:
	OBJECTDEF(PHSolid);
	BASEIMP_OBJECT(Object);
	void Print(std::ostream& os)const{Object::Print(os);}
	PHSolid();											///< �\�z
	
	bool		AddChildObject(Object* o, PHScene* s);///< ���[�h���Ɏg�p�D
	size_t		NReferenceObjects();					///< 1
	Object*		ReferenceObject(size_t i);				///< �t���[����Ԃ��D
	void		Step();									///< ������i�߂�D
	
	void		AddForce(Vec3d f);						///< �͂����ʒ��S�ɉ�����
	void		AddTorque(Vec3d t){ torque += t; }		///< �g���N��������
	void		AddForce(Vec3d f, Vec3d r);				///< �͂� �ʒur(World�n) �ɉ�����
	//�������邵GravityEngine�����g���ĂȂ������̂Ŕp�����
	//void		AddForceLocal(Vec3d f, Vec3d r);		///< �͂� �ʒur(Local�n) �ɉ�����
	void		ClearForce();							///< �͂ƃg���N���N���A
	Vec3d		GetForce() const {return force;}		///< ������ꂽ��
	Vec3d		GetTorque() const {return torque;}		///< ������ꂽ�g���N
	void		SetForce(Vec3d f){force = f;}			///< �͂�ݒ肷��
	void		SetTorque(Vec3d t){torque = t;}			///< �g���N���Z�b�g����

	double		GetMass(){return mass;}					///< ����
	double		GetMassInv(){return 1.0 / mass;}		///< ���ʂ̋t��
	void		SetMass(double m){mass = m;}			///< ���ʂ̐ݒ�
	void		SetMassInv(double minv){mass = 1.0 / minv;}
														///< ���ʂ̋t���̐ݒ�
	Matrix3d	GetInertia(){return inertia;}			///< �����e���\��
	Matrix3d	GetInertiaInv(){return inertia_inv;}	///< �����e���\���̋t��
	void		SetInertia(const Matrix3d& I){			///< �����e���\���̐ݒ�
		inertia = I;
		inertia_inv = I.inv();
	}
	void		SetInertiaInv(const Matrix3d& Iinv){	///< �����e���\�����t���Őݒ�
		inertia_inv = Iinv;
		//inertia = Iinv.inv();
	}

	///	�ϕ������̎擾
	PHIntegrationMode GetIntegrationMode(){ return integrationMode; }
	///	�ϕ������̐ݒ�
	void SetIntegrationMode(PHIntegrationMode m){ integrationMode=m; }

	Vec3d		GetFramePosition() const {return pos;}
	void		SetFramePosition(const Vec3d& p){pos = p;}
	Vec3d		GetCenterPosition() const {return quat*center + pos;}
														///< �d�S�ʒu�̎擾
	void		SetCenterPosition(const Vec3d& p){		///< �d�S�ʒu�̐ݒ�
		pos = p - quat*center;
	}

	///	�����̎擾
	Matrix3d	GetRotation() const { Matrix3d rv; quat.to_matrix(rv); return rv; }
	///	�����̐ݒ�
	void		SetRotation(const Matrix3d& r){
		quat.from_matrix(r);
	}

	///	�����̎擾
	Quaterniond GetOrientation() const {return quat;}
	///	�����̐ݒ�
	void		SetOrientation(const Quaterniond& q){
		quat = q;
		Matrix3f m;
		quat.to_matrix(m);
	}

	///	���ʒ��S�̑��x�̎擾
	Vec3d		GetVelocity() const {return velocity;}
	///	���ʒ��S�̑��x�̐ݒ�
	void		SetVelocity(const Vec3d& v){velocity = v;}

	///	�p���x�̎擾
	Vec3d		GetAngularVelocity() const {return angVelocity;}
	///	�p���x�̐ݒ�
	void		SetAngularVelocity(const Vec3d& av){angVelocity = av;}

	///	���[�J���t���[�����猩���C���̂̎��ʒ��S�ʒu�̐ݒ�
	Vec3d		GetCenter() const {return center;}
	///	���[�J���t���[�����猩���C���̂̎��ʒ��S�ʒu�̎擾
	void		SetCenter(const Vec3d& c){center = c;}		

};

class PHSolids:public std::vector< UTRef<PHSolid> >{
public:
	UTRef<PHSolid> Erase(const UTRef<PHSolid>& s){
		iterator it = std::find(begin(), end(), s);
		if (it == end()) return NULL;
		UTRef<PHSolid> rv = *it;
		erase(it);
		return *it;
	}
	UTRef<PHSolid>* Find(const UTRef<PHSolid>& s){
		iterator it = std::find(begin(), end(), s);
		if (it == end()) return NULL;
		else return &*it;
	}
	UTRef<PHSolid>* Find(const UTRef<PHSolid>& s) const {
		return ((PHSolids*)this)->Find(s);
	}
};

///	Solid�̐ϕ����s���N���X�̃x�[�X
class PHSolverBase:public PHEngine{
	OBJECTDEFABST(PHSolverBase);
public:
	virtual void ClearForce()=0;
};

/**	Solid��ێ�����N���X�DSolid�̍X�V���s���D	*/
class PHSolidContainer:public PHSolverBase{
	OBJECTDEF(PHSolidContainer);
public:
	PHSolids solids;
	bool AddChildObject(Object* o, PHScene* s);
	bool DelChildObject(Object* o, PHScene* s);
	///
	int GetPriority() const {return SGBP_SOLIDCONTAINER;}
	///	���x���ʒu�A�����x�����x�̐ϕ�
	virtual void Step();
	///	���̂ɂ��������͂̃N���A
	virtual void ClearForce();
	
	virtual void Clear(PHScene* s){ solids.clear(); }
	///	���L���Ă���solid�̐�
	virtual size_t NChildObjects(){ return solids.size(); }
	///	���L���Ă���solid
	virtual Object* ChildObject(size_t i){ return solids[i]; }

};

/**	Solid�̗͂��N���A����N���X	*/
class PHSolidClearForce:public PHEngine{
	OBJECTDEF(PHSolidClearForce);
public:
	typedef std::vector< UTRef<PHSolverBase> > PHSolvers;
	PHSolvers solvers;
	///	�N���A����
	virtual void Step();
	virtual int GetPriority() const { return SGBP_CLEARFORCE; }
};

struct SolidInfo{
	float mass;				///<	����
	Matrix3f inertia;		///<	�������[�����g
	Vec3f velocity;			///<	���x
	Vec3f angularVelocity;	///<	�p���x
	Vec3f center;			///<	���S�ʒu
};

}	//	namespace Spr
#endif
