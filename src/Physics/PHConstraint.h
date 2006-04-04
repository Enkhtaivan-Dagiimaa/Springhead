#ifndef PHCONSTRAINT_H
#define PHCONSTRAINT_H

#include <SprPhysics.h>

#define SUBMAT(r, c, h, w) sub_matrix(TSubMatrixDim<r, c, h, w>())
#define SUBVEC(o, l) sub_vector(TSubVectorDim<o, l>())

namespace Spr{;

/// ���̂̏��
struct PHSolidAux : public Object{
	PHSolid*	solid;		/// PHSolid�ւ̎Q��
	double		minv;		/// ���ʂ̋t��
	Matrix3d	Iinv;		/// �����s��̋t�s��
	Vec3d		f, t;		/// ���[�J�����W�ł̊O��
	Vec3d		v, w;		/// ���[�J�����W�ł̌��݂̑��x
	Vec3d		dv0, dw0;	/// �S���͈ȊO�̊O�͂ɂ�鑬�x�ω���
	Vec3d		dv, dw;		/// �S���͂ɂ�鑬�x�ω���
	Vec3d		dV, dW;		/// Correction�ɂ��ړ��ʁC��]��
	void SetupDynamics(double dt);
	void SetupCorrection();
	PHSolidAux::PHSolidAux(PHSolid* s):solid(s){}
};
class PHSolidAuxs : public std::vector< UTRef<PHSolidAux> >{
public:
	iterator Find(PHSolid* s){
		iterator is;
		for(is = begin(); is != end(); is++)
			if((*is)->solid == s)
				break;
		return is;
	};
	void SetupDynamics(double dt){
		for(iterator is = begin(); is != end(); is++)
			(*is)->SetupDynamics(dt);
	}
	void SetupCorrection(){
		for(iterator is = begin(); is != end(); is++)
			(*is)->SetupCorrection();
	}
};

/// PHConstraint�h���N���X�̂��߂̃��[�^
template<class inf, class base>
class InheritConstraint : public InheritSceneObject<inf, base>{
public:
	void Enable(bool bEnable = true){base::Enable(bEnable);}
	bool IsEnabled(){return base::IsEnabled();}
};
template<class inf, class base>
class InheritJoint1D : public InheritConstraint<inf, base>{
public:
	void	SetRange(double lower, double upper){base::SetRange(lower, upper);}
	void	GetRange(double& lower, double& upper){base::GetRange(lower, upper);}
	void	SetMotorTorque(double t){base::SetMotorTorque(t);}
	double	GetMotorTorque(){return base::GetMotorTorque();}
	// void	SetDesiredPosition(double p, double t) = 0;	/// �ڕW�ψʂ�ݒ肷��
	// double	GetDesiredPosition() = 0;				/// �ڕW�ψʂ��擾����
	void	SetDesiredVelocity(double v){base::SetDesiredVelocity(v);}
	double	GetDesiredVelocity(){return base::GetDesiredVelocity();}
	void	SetSpring(double K){base::SetSpring(K);}
	double	GetSpring(){return base::GetSpring();}
	void	SetSpringOrigin(double org){base::SetSpringOrigin(org);}
	double	GetSpringOrigin(){return base::GetSpringOrigin();}
	void	SetDamper(double D){base::SetDamper(D);}
	double	GetDamper(){return base::GetDamper();}
};

///
class PHConstraint : public InheritSceneObject<PHConstraintIf, SceneObject>{
public:
	//OBJECTDEF(PHConstraint);
	enum PHControlMode{
		MODE_TORQUE,
		MODE_POSITION,
		MODE_VELOCITY
	} mode;
	
	bool		bEnabled;			/// �L��������Ă���ꍇ��true
	bool		bFeasible;			/// �����̍��̂�undynamical�ȏꍇtrue

	PHSolidAux* solid[2];
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
	Matrix3d	Jvv[2], Jvw[2], Jwv[2], Jww[2], Jqv[2], Jqw[2];
	
	int			dim_d, dim_c;	// �S���̎���

	typedef PTM::TMatrixCol<6, 3, double> Matrix63d;
	typedef PTM::TVector<6, double> Vec6d;
	Matrix63d	Jdv[2], Jdw[2];	// Dynamics�p�̍S�����R�r�s��
	Matrix63d	Tdv[2], Tdw[2];
	
	Matrix63d	Jcv[2], Jcw[2];	// Correction�p�̍S�����R�r�s��
	Matrix63d	Tcv[2], Tcw[2];

	Vec6d		f, F;
	//Vec3d		fv, fw;	/// dynamics�ɂ�����֐ߗ�
	//Vec6d		F;
	//Vec3d		Fv, Fq; /// correction�ɂ�����֐ߗ�

	Vec6d		b, B;	// b�x�N�g���Ddynamics�p��correction�p
	//Vec3d		bv, bw;	/// dynamics�ɂ�����S�����x
	//Vec3d		bv_bias, bw_bias;	/// ���x������������邽�߂�bv, bw�̃I�t�Z�b�g��
	//Vec3d		Bv, Bq; /// correction�ɂ�����S���덷
	Vec6d		Ad, Ac;	// A�s��̑Ίp�����Ddynamics�p��correction�p
	//Vec3d		Av, Aw, Aq;
	
	virtual void Enable(bool bEnable = true){bEnabled = bEnable;}
	virtual bool IsEnabled(){return bEnabled;}
	virtual void Init(PHSolidAux* lhs, PHSolidAux* rhs, const PHJointDesc& desc);
	void CompJacobian(bool bCompAngular);
	void SetupDynamics(double dt);
	void SetupCorrection(double dt, double max_error);
	void IterateDynamics();
	void IterateCorrection();
	//virtual void CompDof(){}					/// dim_v, dim_w, dim_q��ݒ肷��
	//virtual void CompMotorForce(){}				/// fv, fw�Ƀ��[�^�ɂ��e����ݒ肷��
	virtual void CompConstraintJacobian()=0;
	virtual void CompBias(double dt){}	/// 
	virtual void CompError();			/// Bv, Bq��ݒ肷��
	virtual void ProjectionDynamics(double& f, int k){}
	virtual void ProjectionCorrection(double& F, int k){}
	PHConstraint();
};
class PHConstraints : public std::vector< UTRef<PHConstraint> >{
public:
	void SetupDynamics(double dt){
		for(iterator it = begin(); it != end(); it++)
			(*it)->SetupDynamics(dt);
	}
	void SetupCorrection(double dt, double max_error){
		for(iterator it = begin(); it != end(); it++)
			(*it)->SetupCorrection(dt, max_error);
	}
	void IterateDynamics(){
		for(iterator it = begin(); it != end(); it++)
			(*it)->IterateDynamics();
	}
	void IterateCorrection(){
		for(iterator it = begin(); it != end(); it++)
			(*it)->IterateCorrection();
	}
};

}

#endif