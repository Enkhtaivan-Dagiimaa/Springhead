/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PHSOLID_H
#define PHSOLID_H

#include <SprPhysics.h>
#include <Foundation/Object.h>
#include <Collision/CDShape.h>
#include <Physics/PHScene.h>
#include <Physics/PHEngine.h>
#include <Physics/PHSpatial.h>

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
	void GetSupport(const Vec3f& dir, float& minS, float& maxS);
	
	///	Solid��Pose�������邱�ƂŁCworld���W�n�̍ŏ��l,�ő�l���擾
	/// (���Ӂj�o�E���f�B���O�{�b�N�X�����傫�ȃ{�b�N�X�Ŕ��肳��Ă��܂��D
	//
	void GetBBoxWorldMinMax(Posed& pos , Vec3d& min, Vec3d& max);
};


enum PHIntegrationMode{
		PHINT_NONE,				///	�ϕ����Ȃ�
		PHINT_ARISTOTELIAN,		///	f = mv
		PHINT_EULER,			///	�I�C���[�@
		PHINT_SIMPLETIC,		/// �V���v���N�e�B�b�N�@
		PHINT_ANALYTIC,			
		PHINT_RUNGEKUTTA2,		///	�Q�������Q�N�b�^�@
		PHINT_RUNGEKUTTA4		///	�S�������Q�N�b�^�@
};

class PHSolid;
class PHFrame: public SceneObject, public PHFrameDesc{
	friend class PHSolid;
protected:
	PHSolid* solid;
	CDShape* shape;

	double		mass;
	Vec3d		center;
	Matrix3d	inertia;
public:
	SPR_OBJECTDEF(PHFrame);
	ACCESS_DESC(PHFrame);

	PHFrame();
	PHFrame(const PHFrameDesc& desc);
	void	CompInertia();
	virtual ObjectIf* GetChildObject(size_t pos);
	virtual bool AddChildObject(ObjectIf * o);
	virtual size_t NChildObject() const;
	virtual Posed GetPose();
	virtual void SetPose(Posed p);
};

class PHTreeNode;
class PHScene;
class PHConstraintEngine;

struct PHSolidStatePrivate{
protected:
	bool		bFrozen;		///<	�t���[�Y��Ԃ�
	bool		bUpdated;		///<	�����̃G���W����Solid�̍X�V���Ǘ����邽�߂̃t���O
};

///	����
class PHSolid : public SceneObject, public PHSolidDesc, public PHSolidStatePrivate{
protected:
	Matrix3d	inertia_inv;	///<	�����e���\���̋t��(Local�n�E�L���b�V��)

	///	�ϕ�����
	PHIntegrationMode integrationMode;

	/// �I�C���[�̉^��������
	/// �����s��͑Ίp�s���O��D
	Vec3d	Euler(const Matrix3d& I, const Vec3d& t, const Vec3d& w){
		return Vec3d(
			(t[0] - (I[2][2] - I[1][1]) * w.Y() * w.Z()) / I[0][0],
			(t[1] - (I[0][0] - I[2][2]) * w.Z() * w.X()) / I[1][1],
			(t[2] - (I[1][1] - I[0][0]) * w.X() * w.Y()) / I[2][2]);
	}
public:
	///@name LCP�֘A�⏕�ϐ�
	//@{
	PHScene*	scene;
	PHConstraintEngine* engine;
	PHTreeNode*	treeNode;	///< �֐ߌn���\�����Ă���ꍇ�̑Ή�����m�[�h
	double		minv;		///< ���ʂ̋t��
	Matrix3d	Iinv;		///< �����s��̋t�s��
	SpatialVector f;		///< ���[�J�����W�ł̊O��
	SpatialVector v;		///< ���[�J�����W�ł̌��݂̑��x
	//Vec3d		dv0, dw0;	///< �S���͈ȊO�̊O�͂ɂ�鑬�x�ω���
	SpatialVector dv;		///< �S���͂ɂ�鑬�x�ω���
	SpatialVector dV;		///< Correction�ɂ��ړ��ʁC��]��
	void UpdateCacheLCP(double dt);
	//void SetupCorrection();
	void UpdateVelocity(double dt);
	void UpdatePosition(double dt);
	bool IsArticulated();
	//@}
		
public:
	std::vector< UTRef<PHFrame> > frames;
	PHBBox bbox;

	SPR_OBJECTDEF(PHSolid);
	PHSolid(const PHSolidDesc& desc=PHSolidDesc(), SceneIf* s=NULL);

	virtual SceneObjectIf* CloneObject();
	CDShapeIf* CreateAndAddShape(const IfInfo* info, const CDShapeDesc& desc);
	ObjectIf* CreateObject(const IfInfo* info, const void* desc);
	bool AddChildObject(ObjectIf* obj);
	size_t NChildObject() const{
		return frames.size();
	}
	ObjectIf* GetChildObject(size_t pos) {
		return frames[pos]->Cast();
	}

	void CalcBBox();
	void GetBBoxSupport(const Vec3f& dir, float& minS, float& maxS);
	
	void		SetUpdated(bool set){bUpdated = set;}	///< 
	bool		IsUpdated(){return bUpdated;}			///< 
	void		Step();									///< ������i�߂�D
	
	void		AddForce(Vec3d f);						///< �͂����ʒ��S�ɉ�����
	void		AddTorque(Vec3d t);						///< �g���N��������
	void		AddForce(Vec3d f, Vec3d r);				///< �͂� �ʒur(World�n) �ɉ�����
	/// �S���͈ȊO�̍��̂ɉ�������O�́B�S���͂�PHConstraint����擾���Ȃ���΂Ȃ�Ȃ��B
	Vec3d		GetForce() const {return force;}
	/// �S���͈ȊO�̍��̂ɉ�������O�͂ɂ��g���N�B�S���g���N��PHConstraint����擾���Ȃ���΂Ȃ�Ȃ��B
	Vec3d		GetTorque() const {return torque;}

	double		GetMass(){return mass;}					///< ����
	double		GetMassInv(){return 1.0 / mass;}		///< ���ʂ̋t��
	void		SetMass(double m){mass = m;}			///< ���ʂ̐ݒ�
	void		SetMassInv(double minv){mass = 1.0 / minv;}	///< ���ʂ̋t���̐ݒ�
	Vec3d		GetCenterOfMass(){return center;}		///< ���[�J���t���[�����猩���C���̂̎��ʒ��S�ʒu�̐ݒ�
	void		SetCenterOfMass(const Vec3d& c){center = c;}	///< ���[�J���t���[�����猩���C���̂̎��ʒ��S�ʒu�̎擾
	Matrix3d	GetInertia(){return inertia;}			///< �����e���\��
	Matrix3d	GetInertiaInv(){return inertia_inv;}	///< �����e���\���̋t��
	void		SetInertia(const Matrix3d& I){			///< �����e���\���̐ݒ�
		inertia = I;
		inertia_inv = I.inv();
	}
	void		SetInertiaInv(const Matrix3d& Iinv){	///< �����e���\�����t���Őݒ�
		inertia_inv = Iinv;
		inertia = Iinv.inv();
	}
	void		CompInertia();

	///	�ϕ������̎擾
	PHIntegrationMode GetIntegrationMode(){ return integrationMode; }
	///	�ϕ������̐ݒ�
	void SetIntegrationMode(PHIntegrationMode m){ integrationMode=m; }

	Posed		GetPose() const { return pose; }
	void		SetPose(const Posed& p) { pose = p;}
	Vec3d		GetFramePosition() const {return pose.Pos();}
	void		SetFramePosition(const Vec3d& p){pose.Pos() = p;}
	Vec3d		GetDeltaPosition() const ;
	Vec3d		GetDeltaPosition(const Vec3d& pos) const ;
	Vec3d		GetPrevFramePosition() const { return pose.Pos()-GetDeltaPosition(); }
	Vec3d		GetCenterPosition() const {return pose*center;} 	///< �d�S�ʒu�̎擾
	void		SetCenterPosition(const Vec3d& p){					///< �d�S�ʒu�̐ݒ�
		pose.Pos() = p - pose.Ori()*center;
	}

	///	�����̎擾
	Matrix3d	GetRotation() const { Matrix3d rv; pose.Ori().ToMatrix(rv); return rv; }
	///	�����̐ݒ�
	void		SetRotation(const Matrix3d& r){
		pose.Ori().FromMatrix(r);
	}

	///	�����̎擾
	Quaterniond GetOrientation() const {return pose.Ori();}
	///	�����̐ݒ�
	void		SetOrientation(const Quaterniond& q){
		pose.Ori() = q;
		//Matrix3f m;
		//pose.Ori().ToMatrix(m);
	}

	///	���ʒ��S�̑��x�̎擾
	Vec3d		GetVelocity() const {return velocity;}
	///	���ʒ��S�̑��x�̐ݒ�
	void		SetVelocity(const Vec3d& v){
		velocity = v;
		SetFrozen(false);
	}

	///	�p���x�̎擾
	Vec3d		GetAngularVelocity() const {return angVelocity;}
	///	�p���x�̐ݒ�
	void		SetAngularVelocity(const Vec3d& av){
		angVelocity = av;
		SetFrozen(false);
	}

	///	�C�ӂ̈ʒu�ł̑��x�̎擾
	Vec3d		GetPointVelocity(Vec3d posW) const {
		return velocity + (angVelocity^(posW - pose*center));
	}

	///	shape�̐��B
	int			NFrame();
	///	shape���A�ʒu�w�荞�݂Œǉ�����D
	void		AddFrame(PHFrameIf* frame);	
	///	frame�Ŏ擾
	PHFrameIf*	GetFrame(int i);

	///	���̍��̂����� Spr::CDShape �̐�
	int			NShape();
	///	���̍��̂����� i�Ԗڂ� SPR::CDShape �̎擾
	CDShapeIf*	GetShape(int i);
	///	shape �� ���̍��̂�����Spr::CDShape�̃��X�g �̍Ō�ɒǉ�����D
	void		AddShape(CDShapeIf* shape);
	///	���̍��̂����� i�Ԗڂ� SPR::CDShape �̂��̍��̂��猩���p�����擾
	Posed		GetShapePose(int i);
	///	���̍��̂����� i�Ԗڂ� SPR::CDShape �̂��̍��̂��猩���p����ݒ�
	void		ClearShape();
	/// ���̍��̂�����SPR::CDShape ���폜
	void		SetShapePose(int i, const Posed& pose);
	/// �d�͂������邩�ǂ����ݒ�
	void		SetGravity(bool bOn);
	/// �����@���ɏ]�����ǂ����ݒ�
	void		SetDynamical(bool bOn){dynamical = bOn;}
	/// �����@���ɏ]���Ă��邩�ǂ������擾
	bool		IsDynamical(){return dynamical;}
	/// ���x��ϕ����邩�ǂ�����ݒ�
	void		SetIntegrate(bool bOn){integrate = bOn;}
	/// ���x��ϕ����邩�ǂ������擾
	bool		IsIntegrate(){return integrate;}
	/// ���x�����ȉ��̎��C�ϕ����s��Ȃ��悤�ɐݒ�
	void		SetFrozen(bool bOn){bFrozen = bOn;}
	/// ���x�����ȉ��Őϕ����s��Ȃ����ǂ������擾
	bool		IsFrozen(){return bFrozen;}
	PHTreeNodeIf* GetTreeNode();

	ACCESS_DESC_STATE_PRIVATE(PHSolid);

protected:
	virtual void AfterSetDesc();
};

class PHSolidIfs : public std::vector< UTRef<PHSolidIf> >{
public:
	UTRef<PHSolidIf> Erase(const PHSolidIf* s){
		iterator it = std::find(begin(), end(), s);
		if (it == end()) return NULL;
		UTRef<PHSolidIf> rv = *it;
		erase(it);
		return *it;
	}
	UTRef<PHSolidIf>* Find(const PHSolidIf* s){
		iterator it = std::find(begin(), end(), s);
		if (it == end()) return NULL;
		else return &*it;
	}
	UTRef<PHSolidIf>* Find(const PHSolidIf* s) const {
		return ((PHSolidIfs*)this)->Find(s);
	}
};
class PHSolids : public std::vector< UTRef<PHSolid> >{
public:
	UTRef<PHSolid> Erase(const PHSolid* s){
		iterator it = std::find(begin(), end(), s);
		if (it == end()) return NULL;
		UTRef<PHSolid> rv = *it;
		erase(it);
		return rv;
	}
	UTRef<PHSolid>* Find(const PHSolid* s){
		iterator it = std::find(begin(), end(), s);
		if (it == end()) return NULL;
		else return &*it;
	}
	UTRef<PHSolid>* Find(const PHSolid* s) const {
		return ((PHSolids*)this)->Find(s);
	}
};

/**	Solid��ێ�����N���X�DSolid�̍X�V���s���D	*/
class PHSolidContainer:public PHEngine{
	SPR_OBJECTDEF_NOIF(PHSolidContainer);
public:
	PHSolidIfs solids;
	///
	int GetPriority() const {return SGBP_SOLIDCONTAINER;}
	/// solid�̃��Z�b�g
	void Reset();
	///	���x���ʒu�A�����x�����x�̐ϕ�
	virtual void Step();
	
	virtual void Clear(){ solids.clear(); }
	bool AddChildObject(ObjectIf* o);
	bool DelChildObject(ObjectIf* o);
	///	���L���Ă���solid�̐�
	virtual size_t NChildObject() const { return solids.size(); }
	///	���L���Ă���solid
	virtual ObjectIf* GetChildObject(size_t i){ return solids[i]; }

	PHSolidContainer();
};

/** SolidContainer��Reset���ĂԂ��߂����̃G���W�� */
class PHSolidInitializer : public PHEngine{
	SPR_OBJECTDEF_NOIF(PHSolidInitializer);
public:
	PHSolidContainer* container;
	
	///
	int GetPriority() const {return SGBP_SOLIDINITIALIZER;}
	///	updated�t���O�����Z�b�g����
	virtual void Step();
};


}	//	namespace Spr
#endif
