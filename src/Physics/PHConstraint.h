/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
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

struct PHConstraintStatePrivate{
	SpatialVector f;				///< �S���̗͂͐�
	SpatialVector F;				///< �S���덷���ʒu��LCP�ŕ␳����ꍇ�̕␳��*����
};

class PHConstraint : public SceneObject, public PHConstraintDesc, public PHConstraintStatePrivate{
public:
	SPR_OBJECTDEF_ABST(PHConstraint);
	ACCESS_DESC(PHConstraint);

	enum PHControlMode{
		MODE_TORQUE=0,
		MODE_POSITION,
		MODE_VELOCITY,
	} mode;
	PHConstraintEngine* engine;

	bool		bFeasible;				///< �����̍��̂�undynamical�ȏꍇtrue
	bool		bArticulated;			///< �֐ߌn���\�����Ă���ꍇtrue
	bool		bInactive[2];			///< ���̂���͖@�ɏ]���ꍇtrue	
	PHSolid*			solid[2];		///< �S�����鍄��
	SpatialTransform    X[2];			///< ���[���h���W�n�̒��S�ɑ΂���e(�q)���̂̈ʒu�ƌ���
	SpatialTransform	Xj[2];			///< ���̂̎��ʒ��S�ɑ΂���\�P�b�g�C�v���O�̈ʒu�ƌ���
	SpatialTransform	Xjrel;			///< �\�P�b�g�ɑ΂���v���O�̈ʒu�ƌ���
	SpatialVector		vjrel;			///< �\�P�b�g�ɑ΂���v���O�̑��Α��x,�p���x
	
	SpatialTransform	Js[2];			///< �S�����R�r�A�� SpatialTranform�`���C
												//[0]�F�e���̒��S����Socket���W�n�֕ϊ����郄�R�r�A��
												//[1]�F�q���̒��S����Socket���W�n�֕ϊ�����̃��R�r�A��
	SpatialMatrix		J[2];			///< �S�����R�r�A�� �s��`��
												//[0]�F�e���̂̎��ʒ��S����Socket���W�n�ւ̃��R�r�A��
												//[1]�F�q���̂̎��ʒ��S����Plug���W�n�o�R��Socket���W�n�ւ̃��R�r�A��
	SpatialMatrix		AinvJ[2];
	SpatialMatrix		T[2];
	
	
	SpatialVector b, db, B;				///< LCP��b�x�N�g���Ƃ��̕␳��
	SpatialVector A, dA, Ainv;			///< LCP��A�s��̑Ίp�����Ƃ��̕␳�ʁC�t��
	SpatialVector scale;
	
	SpatialVector xs[2];					///(3�v�f���f����p)�΂˕��̋���
	SpatialVector ws;					///(3�v�f���f����p)�΂˕��̑���
	SpatialVector fs;					///(3�v�f���f����p)�΂˕��̗�
	bool		constr[6];				///< ���x���S�����鎩�R�x. ���͈́C�o�l�E�_���p���L���ȏꍇ��true�ƂȂ�
	bool		constrCorrection[6];	///< �ʒu���S�����鎩�R�x. ���͈͂��L���ȏꍇ��true�ƂȂ�

	///�R���X�g���N�^
	PHConstraint();

	///���̃N���X���̋@�\
	void		CompJacobian();
	void		SetupLCP();
	void		IterateLCP();
	void		UpdateState();
	void		CompResponseMatrix();
	void		CompResponseMatrixABA();
	void		SetupCorrectionLCP();
	void		IterateCorrectionLCP();
	virtual		PHSceneIf* GetScene() const;
	
	///�h���N���X�̋@�\
	virtual void		 AddMotorTorque(){}							///< �S���͂Ɋ֐߃g���N�������Z
	virtual void		 SetConstrainedIndex(bool* con){}			///< �ǂ̎��R�x�𑬓x�S�����邩��ݒ�
	virtual void		 SetConstrainedIndexCorrection(bool* con){	///< �ǂ̎��R�x���ʒu�S�����邩��ݒ�
		SetConstrainedIndex(con);
	}														
	virtual void		 ModifyJacobian(){}							///< �Ǝ����W�n���g���ꍇ�̃��R�r�A���̏C��
	virtual void		 CompBias(){}								///< 
	virtual void		 Projection(double& f, int k){}				///< �S���͂̎ˉe
	virtual void		 UpdateJointState(){}						///< �֐ߍ��W�̈ʒu�E���x���X�V����
	virtual void		 CompError(){}								///< Correction�p�̍S���덷��ݒ肷��
	virtual void		 ProjectionCorrection(double& F, int k){}	///< 
	
	/// �C���^�t�F�[�X�̎���
	//virtual PHConstraintDesc::ConstraintType GetConstraintType(){ assert(0); return PHConstraintDesc::INVALID_CONSTRAINT; }
	virtual void GetSocketPose(Posed& pose){pose = poseSocket;}
	virtual void SetSocketPose(const Posed& pose){
		poseSocket = pose;
		Xj[0].q = pose.Ori();
		Xj[0].r = pose.Pos();
	}
	virtual void GetPlugPose(Posed& pose){pose = posePlug;}
	virtual void SetPlugPose(const Posed& pose){
		posePlug = pose;
		Xj[1].q = pose.Ori();
		Xj[1].r = pose.Pos();
	}
	virtual void		 Enable(bool bEnable = true){bEnabled = bEnable;}
	virtual bool		 IsEnabled(){return bEnabled;}
	virtual void		 SetInactive(int index = 0, bool Inaction = true){bInactive[index] = Inaction;}
	virtual bool		 IsInactive(int index = 0){return bInactive[index];}
	virtual void		 GetRelativePose(Posed& p){UpdateState(); p.Pos() = Xjrel.r; p.Ori() = Xjrel.q;}
	virtual Vec3d		 GetRelativePoseR(){UpdateState();return Xjrel.r;}
	virtual Quaterniond	 GetRelativePoseQ(){UpdateState();return Xjrel.q;}
	virtual Quaterniond  GetAbsolutePoseQ(){UpdateState();return Xjrel.q * X[0].q;}	//< Socket������ʒu���C�ɂ���ׂ����H
	virtual void		 GetRelativeVelocity(Vec3d& v, Vec3d& w);//{UpdateCacheLCP(GetScene()->GetTimeStep()); UpdateState();v = vjrel.v(); w = vjrel.w();}
	virtual void		 GetConstraintForce(Vec3d& _f, Vec3d& _t){_f = f.v() / GetScene()->GetTimeStep(); _t = f.w() / GetScene()->GetTimeStep();}
	virtual bool		 AddChildObject(ObjectIf* o);
	virtual size_t		 NChildObject();
	virtual ObjectIf*	 GetChildObject(size_t i);
	virtual void		 DisableTrajectoryVelocity(){mode = MODE_POSITION;}

protected:
	virtual void AfterSetDesc();
};

class PHConstraints : public std::vector< UTRef<PHConstraint> >, public SceneObject{
	
public:
	SPR_OBJECTDEF(PHConstraints);
	virtual PHConstraintIf* FindBySolidPair(PHSolidIf* lhs, PHSolidIf* rhs){
		for(iterator it = begin(); it != end(); it++) {
			if((*it)->solid[0] == DCAST(PHSolid, lhs) && (*it)->solid[1] == DCAST(PHSolid, rhs)) {
				return XCAST(*it);
			} else if((*it)->solid[0] == DCAST(PHSolid, rhs) && (*it)->solid[1] == DCAST(PHSolid, lhs)) {
				return XCAST(*it);
			}
		}

		return NULL;
	}
	
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
