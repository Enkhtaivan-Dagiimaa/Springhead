/*
 *  Copyright (c) 2003-2010, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PHCONSTRAINT_H
#define PHCONSTRAINT_H

#include <Physics/PHContactDetector.h>
#include <Physics/PHSpatial.h>
#include <Physics/PHAxisIndex.h>

#include <vector>

namespace Spr{;

class PHTreeNode;

/// �S���̃X�e�[�g
struct PHConstraintState {
	/// �S���̗͂͐�
	SpatialVector f;

	/// �S���덷���ʒu��LCP�ŕ␳����ꍇ�̕␳��*����
	SpatialVector F;

	// ----- PHJoint�Ɋ֘A����ϐ�

	/// �΂˕��̋����i�O�v�f���f���p�j
	SpatialVector xs;

	/// �S���͂Ƀ��[�p�X������������
	SpatialVector fAvg;

	/// �~���������ǂ���
	bool   bYielded;

	PHConstraintState() {
		xs       = SpatialVector();
		fAvg     = SpatialVector();
		bYielded = false;
	}
};

/// �S��
class PHConstraint : public SceneObject, public PHConstraintDesc, public PHConstraintState {
public:

	SPR_OBJECTDEF_ABST(PHConstraint);
	ACCESS_DESC_STATE(PHConstraint);

	/// �S�����Ǘ�����G���W��
	PHConstraintEngine* engine;

	/// �S�����鍄��
	PHSolid* solid[2];

	// ----- �t���O

	/// �����̍��̂�undynamical�ȏꍇtrue
	bool bFeasible;				

	/// �֐ߌn���\�����Ă���ꍇtrue
	bool bArticulated;			

	/// ���̂���͖@�ɏ]���ꍇtrue	
	bool bInactive[2];			

	// ----- �v�Z�p�ϐ�

	/// ���[���h���W�n�̒��S�ɑ΂���e(�q)���̂̈ʒu�ƌ���   #* ���̂��疈�����Ă���l
	SpatialTransform X[2];

	/// ���̂̎��ʒ��S�ɑ΂���\�P�b�g�C�v���O�̈ʒu�ƌ���   #* �֐߂����Ƃ��ɐݒ肷��l
	SpatialTransform Xj[2];

	/// �\�P�b�g�ɑ΂���v���O�̈ʒu�ƌ���                   #* X����v�Z
	SpatialTransform Xjrel;

	/// �\�P�b�g�ɑ΂���v���O�̑��Α��x,�p���x              #* ���̂���v�Z
	SpatialVector vjrel;			
	
	/// �S�����R�r�A�� Pose(Vec+Quaternion)�`��              #* Xj Xjrel����v�Z
	///   [0]�F�e���̒��S����Socket���W�n�֕ϊ����郄�R�r�A��
	///   [1]�F�q���̒��S����Socket���W�n�֕ϊ�����̃��R�r�A��
	SpatialTransform Js[2];
	
	/// �S�����R�r�A�� �s��`��                              #[n_c x 6] Js����v�Z�B�s��^���K�v
	///   [0]�F�e���̂̎��ʒ��S����Socket���W�n�ւ̃��R�r�A��
	///   [1]�F�q���̂̎��ʒ��S����Plug���W�n�o�R��Socket���W�n�ւ̃��R�r�A��
	SpatialMatrix J[2];

	/// T = M.inv() * J^t �K�E�X�U�C�f���Ŏg�p               #[6 x n_c] �S���̂���s�����ŗǂ�
	SpatialMatrix T[2];
	
	/// LCP��b�x�N�g���Ƃ��̕␳��                           #[n_c]     �S���̂���s�����ŗǂ�
	SpatialVector b, db, B;
	
	/// LCP��A�s��̑Ίp�����Ƃ��̕␳�ʁC�t��               #[n_c]
	SpatialVector A, dA, Ainv;	

	/// Projection�p�̊e����Min/Max
	double fMaxDt[6], fMinDt[6];

	// ----- �S�����Ǘ�

	/// �S�����Ǘ��N���X
	AxisIndex<6> axes;

	/// �S���̑ΏۂƂȂ肤�鎲�ԍ����X�g
	int targetAxes[6];

	/// targetAxes�̗v�f��
	int nTargetAxes;

	/// ���i���{���S�����Ȃ��j���ԍ����X�g
	int movableAxes[6];

	/// movableAxes�̗v�f��
	int nMovableAxes;

	// -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  ----- 

	/// �R���X�g���N�^
	PHConstraint();

	/// �S���Ώێ��ԍ����X�g�̍\�z�D�R���X�g���N�^��movableAxes�����߂���ɌĂ�
	void InitTargetAxes();

	// ----- �G���W������Ăяo�����֐�

	/// ��Ԃ̍X�V
	void UpdateState();

	/// LCP�������O�i�K�̌v�Z
	virtual void SetupLCP();

	/// LCP�̌J��Ԃ��v�Z
	virtual	void IterateLCP();

	/// �ʒuLCP�������O�i�K�̌v�Z
	virtual void SetupCorrectionLCP();

	/// �ʒuLCP�̌J��Ԃ��v�Z
	virtual void IterateCorrectionLCP();

	// ----- ���̃N���X�Ŏ�������@�\

	/// �S������2�̍��̂̊e���x���瑊�Α��x�ւ̃��R�r�A�����v�Z
	virtual void CompJacobian();

	/// A�̑Ίp�������v�Z����DA = J * M^-1 * J^T
	void CompResponseMatrix();

	/// �S���͕ω���(df)�ɑ΂�������x�ω���(dv�̍���)���v�Z���Ĕ��f
	void CompResponse(double df, int j);

	// ----- �h���N���X�Ŏ�������@�\

	/// �֐ߍ��W�̈ʒu�E���x���X�V����
	virtual void	UpdateJointState(){}

	/// �ǂ̎��R�x�𑬓x�S�����邩��ݒ�
	virtual void	SetupAxisIndex();

	/// �Ǝ����W�n���g���ꍇ�̃��R�r�A���̏C��
	virtual void	ModifyJacobian(){}							

	/// LCP�̕␳�l�̌v�Z�D�덷�C���p
	virtual void	CompBias(){}

	/// �S���͂̎ˉe
	virtual void	Projection(double& f_, int i);

	/// Correction�p�̍S���덷��ݒ肷��
	virtual void	CompError(){}

	/// Correction�p�̎ˉe
	virtual void	ProjectionCorrection(double& F, int k){}

	// ----- �C���^�t�F�[�X�̎���

	virtual PHSceneIf*	 GetScene() const { return DCAST(PHSceneIf, SceneObject::GetScene()); }
	virtual PHSolidIf*	 GetSocketSolid() { return solid[0]->Cast(); }
	virtual PHSolidIf*	 GetPlugSolid()   { return solid[1]->Cast(); }

	virtual void		 Enable(bool bEnable = true){ bEnabled = bEnable; }
	virtual bool		 IsEnabled(){return bEnabled;}

	virtual void		 SetInactive(int index = 0, bool Inaction = true){ bInactive[index]=Inaction; }
	virtual bool		 IsInactive(int index = 0){ return bInactive[index]; }

	virtual void		 GetSocketPose(Posed& pose){ pose=poseSocket; }
	virtual void		 SetSocketPose(const Posed& pose){ poseSocket=pose; Xj[0].q=pose.Ori(); Xj[0].r=pose.Pos(); }
	virtual void		 GetPlugPose(Posed& pose){ pose=posePlug; }
	virtual void		 SetPlugPose(const Posed& pose){ posePlug=pose; Xj[1].q=pose.Ori(); Xj[1].r=pose.Pos(); }

	virtual void		 GetRelativePose(Posed& p){ UpdateState(); p.Pos()=Xjrel.r; p.Ori()=Xjrel.q; }
	virtual Vec3d		 GetRelativePoseR(){ UpdateState(); return Xjrel.r; }
	virtual Quaterniond	 GetRelativePoseQ(){ UpdateState(); return Xjrel.q; }
	virtual Quaterniond  GetAbsolutePoseQ(){ UpdateState(); return Xjrel.q * X[0].q; } // Socket������ʒu���C�ɂ���ׂ����H

	virtual void		 GetRelativeVelocity(Vec3d& v, Vec3d& w);
	virtual void		 GetConstraintForce(Vec3d& _f, Vec3d& _t);

	virtual bool		 AddChildObject(ObjectIf* o);
	virtual size_t		 NChildObject() const;
	virtual ObjectIf*	 GetChildObject(size_t i);

protected:
	virtual void		 AfterSetDesc();

};

/// �S���R���e�i
class PHConstraints : public std::vector< UTRef<PHConstraint> >, public SceneObject {
public:

	SPR_OBJECTDEF(PHConstraints);

	/// �w�肳�ꂽ���̂̑g�ɍ�p���Ă���S����Ԃ��iIf�Łj
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
		for(iterator it = begin(); it != end(); it++) {
			if((*it)->solid[0] == lhs && (*it)->solid[1] == rhs) {
				return *it;
			}
		}

		return NULL;
	}

	/// �w�肳�ꂽ���̂̑g�ɍ�p���Ă��鑍���I�ȗ͂�Ԃ�
	Vec3d GetTotalForce(PHSolidIf* lhs, PHSolidIf* rhs){
		Vec3d total = Vec3f();
		for(iterator it = begin(); it != end(); it++){
			if((*it)->solid[0] == DCAST(PHSolid, lhs) && (*it)->solid[1] == DCAST(PHSolid, rhs)){
				Vec3d _f , _t;
				(*it)->GetConstraintForce(_f, _t);
				SpatialVector __f;
				__f.v() = _f; __f.w() = _t;
				PTM::TVector<6,double> Jf = ((*it)->J[0].trans() * __f);
				Vec3d Jfv;
				for(int i=0; i<3; ++i){ Jfv[i] = Jf[i]; }
				total += Jfv;
			}
		}
		return total;
	}

};

}

#endif
