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
class PHSolid : public Object, public PHSolidDesc, public PHSolidIf{
	Vec3d	_angvel[4];			///<	���l�ϕ��W��
	Vec3d	_angacc[4];
protected:
	Vec3d		force;			///<	��				(World)
	Vec3d		torque;			///<	�g���N			(World)
	Vec3d		nextForce;
	Vec3d		nextTorque;
	Matrix3d	inertia_inv;

	std::vector< UTRef<CDShape> > shapes;

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
	PHSolid(const PHSolidDesc& desc=PHSolidDesc());
	
	bool		AddChildObject(Object* o, PHScene* s);///< ���[�h���Ɏg�p�D
	size_t		NReferenceObjects();					///< 1
	Object*		ReferenceObject(size_t i);				///< �t���[����Ԃ��D
	void		Step();									///< ������i�߂�D
	
	void		AddForce(Vec3d f);						///< �͂����ʒ��S�ɉ�����
	void		AddTorque(Vec3d t);						///< �g���N��������
	void		AddForce(Vec3d f, Vec3d r);				///< �͂� �ʒur(World�n) �ɉ�����
	//�������邵GravityEngine�����g���ĂȂ������̂Ŕp�����
	//void		AddForceLocal(Vec3d f, Vec3d r);		///< �͂� �ʒur(Local�n) �ɉ�����
	Vec3d		GetForce() const {return force;}		///< ������ꂽ��
	Vec3d		GetTorque() const {return torque;}		///< ������ꂽ�g���N

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

	Vec3d		GetFramePosition() const {return position;}
	void		SetFramePosition(const Vec3d& p){position = p;}
	Vec3d		GetCenterPosition() const {return orientation*center + position;}
														///< �d�S�ʒu�̎擾
	void		SetCenterPosition(const Vec3d& p){		///< �d�S�ʒu�̐ݒ�
		position = p - orientation*center;
	}

	///	�����̎擾
	Matrix3d	GetRotation() const { Matrix3d rv; orientation.to_matrix(rv); return rv; }
	///	�����̐ݒ�
	void		SetRotation(const Matrix3d& r){
		orientation.from_matrix(r);
	}

	///	�����̎擾
	Quaterniond GetOrientation() const {return orientation;}
	///	�����̐ݒ�
	void		SetOrientation(const Quaterniond& q){
		orientation = q;
		Matrix3f m;
		orientation.to_matrix(m);
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

	void		AddShape(CDShapeIf* shape);
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

/**	Solid��ێ�����N���X�DSolid�̍X�V���s���D	*/
class PHSolidContainer:public PHEngine{
	OBJECTDEF(PHSolidContainer);
public:
	PHSolids solids;
	bool AddChildObject(Object* o, PHScene* s);
	bool DelChildObject(Object* o, PHScene* s);
	///
	int GetPriority() const {return SGBP_SOLIDCONTAINER;}
	///	���x���ʒu�A�����x�����x�̐ϕ�
	virtual void Step();
	
	virtual void Clear(PHScene* s){ solids.clear(); }
	///	���L���Ă���solid�̐�
	virtual size_t NChildObjects(){ return solids.size(); }
	///	���L���Ă���solid
	virtual Object* ChildObject(size_t i){ return solids[i]; }

};


}	//	namespace Spr
#endif
