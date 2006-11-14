/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PHCONSTRAINT_H
#define PHCONSTRAINT_H

#include <SprPhysics.h>
#include <Physics/PHContactDetector.h>
#include <Physics/PHSpatial.h>

namespace Spr{;

class PHTreeNode;

/// PHConstraint�h���N���X�̂��߂̃��[�^
template<class inf, class base>
class InheritConstraint : public InheritSceneObject<inf, base>{
public:
	void Enable(bool bEnable = true){base::Enable(bEnable);}
	bool IsEnabled(){return base::IsEnabled();}
};

///
class PHConstraint : public InheritSceneObject<PHConstraintIf, SceneObject>{
public:
	OBJECT_DEF_ABST(PHConstraint);
	enum PHControlMode{
		MODE_TORQUE,
		MODE_POSITION,
		MODE_VELOCITY
	} mode;
	
	bool		bEnabled;			///< �L��������Ă���ꍇ��true
	bool		bFeasible;			///< �����̍��̂�undynamical�ȏꍇtrue
	bool		bInactive[2];		///< ���̂���͖@�ɏ]���ꍇtrue
	bool		bArticulated;		///< �֐ߌn���\�����Ă���ꍇtrue
	
	PHSolid*			solid[2];	///< �S�����鍄��
	SpatialTransform	Xj[2];		///< ���̂ɑ΂���\�P�b�g�C�v���O�̈ʒu�ƌ���
	SpatialTransform	Xjrel;		///< �\�P�b�g�ɑ΂���v���O�̈ʒu�ƌ���
	Quaterniond	qjrel; 
	SpatialVector		vjrel;		///< �\�P�b�g�ɑ΂���v���O�̑��Α��x
	
	SpatialTransform		Js[2];		///< �S�����R�r�A�� SpatialTranform�`��
	SpatialMatrix			J[2];		///< �S�����R�r�A�� �s��`��
	SpatialMatrix			AinvJ[2];
	SpatialMatrix			T[2];
	
	bool		constr[6];			///< �S�����鎩�R�x�Dtrue�Ȃ�΍S������D

	SpatialVector f;				///< �S���̗͂͐�
	//Vec3d		Fv, Fq;				/// correction�ɂ�����֐ߗ�

	SpatialVector b, db;			///< LCP��b�x�N�g���Ƃ��̕␳��
	SpatialVector A, dA, Ainv;		///< LCP��A�s��̑Ίp�����Ƃ��̕␳�ʁC�t��
	
	void	Init();
	void	SetupDynamics();
	void	IterateDynamics();
	void	SetScene(SceneIf* s){scene = DCAST(PHScene, s);}
	void	SetEngine(PHConstraintEngine* e){engine = e;}
	void	UpdateState();
	void	CompJacobian();
	void	CompResponseMatrix();
	//void SetupCorrection(double dt, double max_error);
	//void IterateCorrection();
	
	/// �C���^�t�F�[�X�̎���
	virtual bool AddChildObject(ObjectIf* o);
	virtual void Enable(bool bEnable = true){bEnabled = bEnable;}
	virtual bool IsEnabled(){return bEnabled;}
	virtual void SetInactive(int index = 0, bool Inaction = true){bInactive[index] = Inaction;}
	virtual bool IsInactive(int index = 0){return bInactive[index];}

	/// �h���N���X�̋@�\
	virtual void SetDesc(const PHConstraintDesc& desc);
	virtual void AddMotorTorque(){}
	virtual void CompBias(){}
	virtual void Projection(double& f, int k){}
	virtual void UpdateJointState(){}			///< �֐ߍ��W�̈ʒu�E���x���X�V����
	//virtual void CompError(double dt)=0;			/// Bv, Bq��ݒ肷��
	//virtual void ProjectionCorrection(double& F, int k){}
	PHConstraint();
protected:
	PHScene* scene;
	PHConstraintEngine* engine;
};
class PHConstraints : public std::vector< UTRef<PHConstraint> >{
public:
	/// �w�肳�ꂽ���̂̑g�ɍ�p���Ă���S����Ԃ�
	PHConstraint* FindBySolidPair(PHSolid* lhs, PHSolid* rhs){
		for(iterator it = begin(); it != end(); it++)
			if((*it)->solid[0] == lhs && (*it)->solid[1] == rhs)
				return *it;
		return NULL;
	}
	void SetupDynamics(){
		for(iterator it = begin(); it != end(); it++)
			(*it)->SetupDynamics();
	}
	void IterateDynamics(){
		for(iterator it = begin(); it != end(); it++)
			(*it)->IterateDynamics();
	}
	/*void SetupCorrection(double dt, double max_error){
		for(iterator it = begin(); it != end(); it++)
			(*it)->SetupCorrection(dt, max_error);
	}*/
	/*void IterateCorrection(){
		for(iterator it = begin(); it != end(); it++)
			(*it)->IterateCorrection();
	}*/
	void UpdateState(){
		for(iterator it = begin(); it != end(); it++)
			(*it)->UpdateState();
	}
};

}

#endif
