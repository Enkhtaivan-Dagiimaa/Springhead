#ifndef PHSOLID_H
#define PHSOLID_H

#include <Springhead.h>
#include <Foundation/Object.h>
#include <Physics/PHScene.h>
#include <Physics/PHEngine.h>

namespace Spr{;

///	�o�E���f�B���O�{�b�N�X�̎���
class PHBBox{
	Vec3f bboxCenter;	///<	BBox�̒��S(���[�J���n)
	Vec3f bboxExtent;	///<	BBox�̑傫��(���[�J���n)
public:
	///	�o�E���f�B���O�{�b�N�X�̐ݒ�
	void SetBBoxCenterExtent(Vec3f c, Vec3f e){
		bboxCenter = c;
		bboxExtent = e;
	}
	///	�o�E���f�B���O�{�b�N�X�̐ݒ�
	void SetBBoxMinMax(Vec3f bmin, Vec3f bmax){
		bboxCenter = (bmin+bmax)*0.5f;
		bboxExtent = (bmax-bmin)*0.5f;
	}
	///	�o�E���f�B���O�{�b�N�X�̒ǉ�
	void AddBBox(Vec3f bmin, Vec3f bmax){
		Vec3f bboxMin = GetBBoxMin();
		Vec3f bboxMax = GetBBoxMax();
		bboxMin.element_min(bmin);
		bboxMax.element_max(bmax);
		SetBBoxMinMax(bboxMin, bboxMax);
	}
	///	���S
	Vec3f GetBBoxCenter(){ return bboxCenter; }
	///	�傫��
	Vec3f GetBBoxExtent(){ return bboxExtent; }
	///	�������[�_
	Vec3f GetBBoxMin(){ return bboxCenter-bboxExtent; }
	///	�傫���[�_
	Vec3f GetBBoxMax(){ return bboxCenter+bboxExtent; }
	///	�^����ꂽ�x�N�g���Ƃ̓��ς��ő�ƍŏ��̓_
	void GetSupport(const Vec3f& dir, float& minS, float& maxS){
		Vec3f ext0( bboxExtent.X(),  bboxExtent.Y(),  bboxExtent.Z());
		Vec3f ext1(-bboxExtent.X(),  bboxExtent.Y(),  bboxExtent.Z());
		Vec3f ext2( bboxExtent.X(), -bboxExtent.Y(),  bboxExtent.Z());
		Vec3f ext3( bboxExtent.X(),  bboxExtent.Y(), -bboxExtent.Z());
		float d = abs(dir*ext0);
		float d1 = abs(dir*ext1);
		if (d < d1) d = d1;
		float d2 = abs(dir*ext2);
		if (d < d2) d = d2;
		float d3 = abs(dir*ext3);
		if (d < d3) d = d3;
		float c = dir * bboxCenter;
		minS = c-d;
		maxS = c+d;
	}
};


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
	typedef std::vector<CDShape*> CDShapes;
	CDShapes shapes;
	PHBBox bbox;

	OBJECTDEF(PHSolid);
	BASEIMP_OBJECT(Object);
	void Print(std::ostream& os)const{Object::Print(os);}
	PHSolid(const PHSolidDesc& desc=PHSolidDesc());

	void CalcBBox();
	void GetBBoxSupport(const Vec3f& dir, float& minS, float& maxS);
	
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

	Posed		GetPose() const { return pose; }
	Vec3d		GetFramePosition() const {return pose.pos;}
	void		SetFramePosition(const Vec3d& p){pose.pos = p;}
	Vec3d		GetCenterPosition() const {return pose*center;}
														///< �d�S�ʒu�̎擾
	void		SetCenterPosition(const Vec3d& p){		///< �d�S�ʒu�̐ݒ�
		pose.pos = p - pose.ori*center;
	}

	///	�����̎擾
	Matrix3d	GetRotation() const { Matrix3d rv; pose.ori.ToMatrix(rv); return rv; }
	///	�����̐ݒ�
	void		SetRotation(const Matrix3d& r){
		pose.ori.FromMatrix(r);
	}

	///	�����̎擾
	Quaterniond GetOrientation() const {return pose.ori;}
	///	�����̐ݒ�
	void		SetOrientation(const Quaterniond& q){
		pose.ori = q;
		Matrix3f m;
		pose.ori.ToMatrix(m);
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

	void		SetGravity(bool bOn);

	int			GetNShapes();
	CDShapeIf**	GetShapes();
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
