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

#define SUBMAT(r, c, h, w) sub_matrix(PTM::TSubMatrixDim<r, c, h, w>())
#define SUBVEC(o, l) sub_vector(PTM::TSubVectorDim<o, l>())

namespace Spr{;

typedef PTM::TMatrixRow<6, 6, double> Matrix6d;
typedef PTM::TMatrixRow<6, 3, double> Matrix63d;
typedef PTM::TVector<6, double> Vec6d;

/// ���̂̏��
struct PHSolidInfoForLCP : public PHSolidInfo{
	double		minv;		/// ���ʂ̋t��
	Matrix3d	Iinv;		/// �����s��̋t�s��
	Vec3d		f, t;		/// ���[�J�����W�ł̊O��
	Vec3d		v, w;		/// ���[�J�����W�ł̌��݂̑��x
	Vec3d		dv0, dw0;	/// �S���͈ȊO�̊O�͂ɂ�鑬�x�ω���
	Vec3d		dv, dw;		/// �S���͂ɂ�鑬�x�ω���
	//Vec3d		dV, dW;		/// Correction�ɂ��ړ��ʁC��]��
	void SetupDynamics(double dt);
	void SetupCorrection();
	PHSolidInfoForLCP(PHSolid* s):PHSolidInfo(s){}
};

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
	
	bool		bEnabled;			/// �L��������Ă���ꍇ��true
	bool		bFeasible;			/// �����̍��̂�undynamical�ȏꍇtrue

	PHSolidInfoForLCP* solid[2];
	Matrix3d	Rj[2];				/// �e���̂ɒ���t�����֐߃t���[��
	Vec3d		rj[2];
	Matrix3d	Rjrel;				/// �֐߃t���[���Ԃ̈ʒu�֌W
	Quaterniond	qjrel;
	Vec3d		rjrel;
	Vec3d		vjrel, wjrel;		/// �֐߃t���[���Ԃ̑��Α��x
	/// Jvv, Jvw : �e���̂̑��x�C�p���x���瑊�Α��x�ւ̃��R�r�s��
	/// Jwv, Jww : �e���̂̑��x�C�p���x���瑊�Ίp���x�ւ̃��R�r�s��
	/// Jqv, Jqw : �e���̂̑��x�C�p���x���瑊��quaternion�̎��Ԕ����ւ̃��R�r�s��
	/**
				|	  v[0]	  w[0]	  v[1]	  w[1]
	  ----------+---------------------------------
		vjrel	|	Jvv[0]	Jvw[0]	Jvv[1]	Jvw[1]
		wjrel	|	Jwv[0]	Jww[0]	Jwv[1]	Jww[1]
		qjrel	|	Jqv[0]	Jqw[0]	Jqv[1]	Jqw[1]
	*/
	Matrix3d	Jvv[2], Jvw[2], Jwv[2], Jww[2];	//, Jqv[2], Jqw[2];
	Matrix3d	Tvv[2], Tvw[2], Twv[2], Tww[2];
	
	//int		dim_d, dim_c;	// �S���̎���
	bool		constr[6];	// �S�����鎩�R�x�Dtrue�Ȃ�΍S������D

	//Matrix63d	Jdv[2], Jdw[2];	// Dynamics�p�̍S�����R�r�s��
	//Matrix63d	Tdv[2], Tdw[2];
	
	//Matrix63d	Jcv[2], Jcw[2];	// Correction�p�̍S�����R�r�s��
	//Matrix63d	Tcv[2], Tcw[2];

	Vec3d		fv, fw;	/// dynamics�ɂ�����֐ߗ̗͂͐�
	//Vec3d		Fv, Fq; /// correction�ɂ�����֐ߗ�

	//Vec6d		b, B;	// b�x�N�g���Ddynamics�p��correction�p
	Vec3d		bv, bw;	/// dynamics�ɂ�����S�����x
	//Vec3d		bv_bias, bw_bias;	/// ���x������������邽�߂�bv, bw�̃I�t�Z�b�g��
	//Vec3d		Bv, Bq; /// correction�ɂ�����S���덷
	//Vec6d		Ad, Ac;	// A�s��̑Ίp�����Ddynamics�p��correction�p
	Vec3d		Av, Aw; //, Aq;
	Vec3d		dbv, dbw;	//�o�l�_���p�������Ȃǂɂ�����b�̕␳�l
	Vec3d		dAv, dAw;	//��A�̑Ίp�����̕␳�l
	
	virtual void SetDesc(const PHConstraintDesc& desc);
	virtual bool AddChildObject(ObjectIf* o);
	virtual void Enable(bool bEnable = true){bEnabled = bEnable;}
	virtual bool IsEnabled(){return bEnabled;}
	void Init();
	virtual void CompJacobian(bool bCompAngular);
	void SetupDynamics(double dt, double correction_rate, double shrink_rate);
	void IterateDynamics();
	//void SetupCorrection(double dt, double max_error);
	//void IterateCorrection();
	//virtual void CompDof(){}					/// dim_v, dim_w, dim_q��ݒ肷��
	//virtual void CompMotorForce(){}				/// fv, fw�Ƀ��[�^�ɂ��e����ݒ肷��
	//virtual void CompConstraintJacobian()=0;
	virtual void CompBias(double dt, double correction_rate){}	/// 
	//virtual void CompError(double dt)=0;			/// Bv, Bq��ݒ肷��
	virtual void Projection(double& f, int k){}
	//virtual void ProjectionCorrection(double& F, int k){}
	PHConstraint();
};
class PHConstraints : public std::vector< UTRef<PHConstraint> >{
public:
	void SetupDynamics(double dt, double correction_rate, double shrink_rate){
		for(iterator it = begin(); it != end(); it++)
			(*it)->SetupDynamics(dt, correction_rate, shrink_rate);
	}
	/*void SetupCorrection(double dt, double max_error){
		for(iterator it = begin(); it != end(); it++)
			(*it)->SetupCorrection(dt, max_error);
	}*/
	void IterateDynamics(){
		for(iterator it = begin(); it != end(); it++)
			(*it)->IterateDynamics();
	}
	/*void IterateCorrection(){
		for(iterator it = begin(); it != end(); it++)
			(*it)->IterateCorrection();
	}*/
};

}

#endif
