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

class PHConstraint : public SceneObject, public PHConstraintIfInit{
public:
	OBJECTDEF_ABST(PHConstraint, SceneObject);
	enum PHControlMode{
		MODE_TORQUE,
		MODE_POSITION,
		MODE_VELOCITY
	} mode;

	PHScene*			scene;
	PHConstraintEngine* engine;

	bool		bEnabled;			///< �L��������Ă���ꍇ��true
	bool		bFeasible;			///< �����̍��̂�undynamical�ȏꍇtrue
	bool		bArticulated;		///< �֐ߌn���\�����Ă���ꍇtrue
	bool		bInactive[2];		///< ���̂���͖@�ɏ]���ꍇtrue	
	PHSolid*			solid[2];	///< �S�����鍄��
	SpatialTransform	Xj[2];		///< ���̂ɑ΂���\�P�b�g�C�v���O�̈ʒu�ƌ���
	SpatialTransform	Xjrel;		///< �\�P�b�g�ɑ΂���v���O�̈ʒu�ƌ���
	SpatialVector		vjrel;		///< �\�P�b�g�ɑ΂���v���O�̑��Α��x
	
	SpatialTransform	Js[2];		///< �S�����R�r�A�� SpatialTranform�`��
	SpatialMatrix		J[2];		///< �S�����R�r�A�� �s��`��
	SpatialMatrix		AinvJ[2];
	SpatialMatrix		T[2];
	
	SpatialVector f;				///< �S���̗͂͐�
	SpatialVector F;
	
	SpatialVector b, db, B;			///< LCP��b�x�N�g���Ƃ��̕␳��
	SpatialVector A, dA, Ainv;		///< LCP��A�s��̑Ίp�����Ƃ��̕␳�ʁC�t��
	SpatialVector scale;

	bool		constr[6];			///< �S�����鎩�R�x. ���͈́C�o�l�E�_���p���L���ȏꍇ��true�ƂȂ�

	/* �h���N���X�̋@�\	*/
	//virtual void AddMotorTorque(){}							///< �S���͂Ɋ֐߃g���N�������Z
	void AddMotorTorque(){}							///< �S���͂Ɋ֐߃g���N�������Z
	virtual void SetConstrainedIndex(bool* con){}			///< �ǂ̎��R�x���S�����邩��ݒ�
	virtual void ModifyJacobian(){}							///< �Ǝ����W�n���g���ꍇ�̃��R�r�A���̏C��
	virtual void CompBias(){}								///< 
	virtual void Projection(double& f, int k){}				///< �S���͂̎ˉe
	virtual void UpdateJointState(){}						///< �֐ߍ��W�̈ʒu�E���x���X�V����
	virtual void CompError(){}								///< Correction�p�̍S���덷��ݒ肷��
	virtual void ProjectionCorrection(double& F, int k){}	///< 
	
	/// �C���^�t�F�[�X�̎���
	virtual PHConstraintDesc::ConstraintType GetConstraintType(){ assert(0); return PHConstraintDesc::INVALID_CONSTRAINT; }
	virtual void Enable(bool bEnable = true){bEnabled = bEnable;}
	virtual bool IsEnabled(){return bEnabled;}
	virtual void SetInactive(int index = 0, bool Inaction = true){bInactive[index] = Inaction;}
	virtual bool IsInactive(int index = 0){return bInactive[index];}
	virtual void GetRelativePose(Posed& p){p.Pos() = Xjrel.r; p.Ori() = Xjrel.q;}
	virtual void GetRelativeVelocity(Vec3d& v, Vec3d& w){v = vjrel.v(); w = vjrel.w();}
	virtual void GetConstraintForce(Vec3d& _f, Vec3d& _t){_f = f.v(); _t = f.w();}
	virtual bool AddChildObject(ObjectIf* o);
	virtual size_t NChildObject();
	virtual ObjectIf* GetChildObject(size_t i);
	virtual void SetDesc(const void* desc);
	
	void	CompJacobian();
	void	SetupLCP();
	void	IterateLCP();
	void	UpdateState();
	void	CompResponseMatrix();
	void	CompResponseMatrixABA();
	void	SetupCorrectionLCP();
	void	IterateCorrectionLCP();
	
	PHConstraint();
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
};

}

#endif
