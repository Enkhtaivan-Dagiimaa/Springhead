#ifndef PHCONSTRAINTENGINE_H
#define PHCONSTRAINTENGINE_H

#include <SprPhysics.h>
#include <Foundation/Object.h>
#include <Physics/PHScene.h>
#include <Physics/PHEngine.h>
#include <Collision/CDDetectorImp.h>
#include <vector>

namespace Spr{;

/// ���̂̏��
struct PHSolidAux : public Object{
	PHSolid*	solid;		/// PHSolid�ւ̎Q��
	double		minv;		/// ���ʂ̋t��
	Matrix3d	Iinv;		/// �����s��̋t�s��
	Vec3d		v0, w0;		/// �ڐG�͂�0�̏ꍇ��dt��̑��x
	Vec3d		dv, dw;		/// �ڐG�͂��l���������x�ω���
	Vec3d		dV, dW;		/// Correction�ɂ��ړ��ʁC��]��
	void SetupDynamics(double dt);
	void SetupCorrection();
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
	void Init();
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
	// void	SetDesiredPosition(double p, double t) = 0;	/// �ڕW�ψʂ�ݒ肷��
	// double	GetDesiredPosition() = 0;				/// �ڕW�ψʂ��擾����
	void	SetDesiredVelocity(double v){base::SetDesiredVelocity(v);}
	double	GetDesiredVelocity(){return base::GetDesiredVelocity();}
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
	int			dim_v, dim_w, dim_q;
	//int			idx_v[3], idx_w[3], idx_q[3];
	bool		bEnabled;			/// �L��������Ă���ꍇ��true
	bool		bFeasible;			/// �����̍��̂�undynamical�ȏꍇtrue

	PHSolidAux* solid[2];
	Matrix3d	Rj[2];				/// �e���̂ɒ���t�����֐߃t���[��
	Vec3d		rj[2];
	Matrix3d	Rjrel;				/// �֐߃t���[���Ԃ̈ʒu�֌W
	Quaterniond	qjrel;
	Vec3d		rjrel;
	Vec3d		vjrel, wjrel;
			/// �e���̂̑��x�C�p���x���瑊�Α��x�ւ̃��R�r�s��
			/// �e���̂̑��x�C�p���x���瑊�Ίp���x�ւ̃��R�r�s��
			/// �e���̂̑��x�C�p���x���瑊��quaternion�̎��Ԕ����ւ̃��R�r�s��
	/**
				|	  v[0]	  w[0]	  v[1]	  w[1]
	  ----------+---------------------------------
		vrel	|	Jvv[0]	Jvw[0]	Jvv[1]	Jvw[1]
		wrel	|	Jwv[0]	Jww[0]	Jwv[1]	Jww[1]
		qdrel	|	Jqv[0]	Jqw[0]	Jqv[1]	Jqw[1]
	*/
	Matrix3d	Jvv[2], Jvw[2], Jwv[2], Jww[2], Jqv[2], Jqw[2];
	Matrix3d	Tvv[2], Tvw[2], Twv[2], Tww[2], Tqv[2], Tqw[2];
	Vec3d		fv, fw;	/// dynamics�ɂ�����֐ߗ�
	Vec3d		Fv, Fq; /// correction�ɂ�����֐ߗ�
	Vec3d		bv, bw;	/// dynamics�ɂ�����S�����x
	Vec3d		bv_bias, bw_bias;	/// ���x������������邽�߂�bv, bw�̃I�t�Z�b�g��
	Vec3d		Bv, Bq; /// correction�ɂ�����S���덷
	Vec3d		Av, Aw, Aq;
	
	virtual void Enable(bool bEnable = true){bEnabled = bEnable;}
	virtual bool IsEnabled(){return bEnabled;}
	virtual void Init(PHSolidAux* lhs, PHSolidAux* rhs, const PHJointDesc& desc);
	void CompJacobian(bool bCompAngular);
	void SetupDynamics(double dt);
	void SetupCorrection(double dt, double max_error);
	void IterateDynamics();
	void IterateCorrection();
	virtual void CompDof(){}			/// dim_v, dim_w, dim_q��ݒ肷��
	virtual void CompVelocityBias(){}	/// bv_bias, bw_bias��ݒ肷��
	virtual void CompError();			/// Bv, Bq��ݒ肷��
	virtual void Projectionfv(double& f, int k){}
	virtual void Projectionfw(double& f, int k){}
	virtual void ProjectionFv(double& F, int k){}
	virtual void ProjectionFq(double& F, int k){}
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

class PHContactPoint : public InheritConstraint<PHContactPointIf, PHConstraint>{
public:
	//OBJECTDEF(PHContactPoint);
	CDShapePair* shapePair;
	Vec3d pos;
	virtual PHJointDesc::JointType GetJointType(){return PHJointDesc::JOINT_CONTACT;}
	virtual void CompError();
	virtual void Projectionfv(double& f, int k);
	virtual void ProjectionFv(double& F, int k);
	PHContactPoint(CDShapePair* sp, Vec3d p, PHSolidAux* s0, PHSolidAux* s1);
	PHContactPoint(const Matrix3d& local, CDShapePair* sp, Vec3d p, PHSolidAux* s0, PHSolidAux* s1);
};

class PHJoint : public InheritConstraint<PHJointIf, PHConstraint>{
public:

};
class PHJoint1D : public InheritConstraint<PHJoint1DIf, PHJoint>{
public:
	//OBJECTDEF(PHJoint1D);
	bool on_lower, on_upper;	/// ���͈͂̉����A����ɒB���Ă���ꍇ��true
	double lower, upper;		/// ���͈͂̉����A���
	double pos_d, vel_d;		/// �ڕW�ψʁA�ڕW���x
	
	virtual void Init(PHSolidAux* lhs, PHSolidAux* rhs, const PHJointDesc& desc){
		PHConstraint::Init(lhs, rhs, desc);
		const PHJoint1DDesc& desc1D = (const PHJoint1DDesc&)desc;
		lower = desc1D.lower;
		upper = desc1D.upper;
	}
	virtual void SetRange(double l, double u){lower = l, upper = u;}
	//virtual void SetDesiredPosition(double p){mode = MODE_POSITION; pos_d = p;}
	//virtual double GetDesiredPosition(){return pos_d;}
	virtual void SetDesiredVelocity(double v){mode = MODE_VELOCITY; vel_d = v;}
	virtual double GetDesiredVelocity(){return vel_d;}
};

class PHHingeJoint : public InheritJoint1D<PHHingeJointIf, PHJoint1D>{
public:
	//OBJECTDEF(PHHingeJoint);
	virtual PHJointDesc::JointType GetJointType(){return PHJointDesc::JOINT_HINGE;}
	virtual void SetTorque(double t){mode = MODE_TORQUE; fw[2] = t;}
	virtual double GetTorque(){return fw[2];}
	virtual double GetPosition(){return qjrel.Theta();}
	virtual double GetVelocity(){return wjrel[2];}
	virtual void CompDof();
	virtual void CompVelocityBias();
	virtual void Projectionfw(double& f, int k);
	virtual void ProjectionFq(double& F, int k);
	PHHingeJoint(){}
};
class PHSliderJoint : public InheritJoint1D<PHSliderJointIf, PHJoint1D>{
public:
	//OBJECTDEF(PHSliderJoint);
	virtual PHJointDesc::JointType GetJointType(){return PHJointDesc::JOINT_SLIDER;}
	virtual void CompDof();
	PHSliderJoint(){}
};
class PHBallJoint : public InheritConstraint<PHBallJointIf, PHConstraint>{
public:
	//OBJECTDEF(PHBallJoint);
	virtual PHJointDesc::JointType GetJointType(){return PHJointDesc::JOINT_BALL;}
	virtual void CompDof();
	PHBallJoint(){}
};

class PHConstraintEngine;
///	Shape�̑g���킹
class PHShapePair: public CDShapePair{
public:
	///	
	PHShapePair(){}
	PHShapePair(CDShape* s0, CDShape* s1):CDShapePair(s0, s1){}
	///	�ڐG����D�ڐG�̒��S(center �ŐN���_�̒��_)�Ɩ@���x�N�g���܂ŋ��߂�
	bool Detect(unsigned ct, PHSolidAux* solid0, PHSolidAux* solid1);
	///	�ڐG��́D�ڐG�����̐؂�������߂āC�؂�����\������ʑ��p�`�̒��_��engine�ɍS���Ƃ��Ēǉ�����D
	void EnumVertex(PHConstraintEngine* engine, unsigned ct, PHSolidAux* solid0, PHSolidAux* solid1);
};
class PHConstraintEngine: public PHEngine{
	friend class PHSolidPair;

	OBJECTDEF(PHConstraintEngine);

	/// Solid�̑g�ݍ��킹�̔z��
	class PHSolidPair{
	public:
		bool bEnabled;
		PHSolidAux* solid[2];
		/// Shape�̑g�ݍ��킹�̔z��
		typedef UTCombination<PHShapePair> PHShapePairs;
		PHShapePairs	shapePairs;

		void Init(PHSolidAux* s0, PHSolidAux* s1);	/// ������
		bool Detect(PHConstraintEngine* engine);	

	};
	typedef UTCombination<PHSolidPair> PHSolidPairs;
	
protected:
	bool			ready;			/// 
	PHSolidAuxs		solids;			/// ���̂̔z��
	PHSolidPairs	solidPairs;	
	PHConstraints	points;			///	�ڐG�_�̔z��
	PHConstraints	joints;			///	�֐߂̔z��
	int max_iter_dynamics;			/// Dynamics()�̔�����
	int max_iter_correction;		/// Correction()�̔�����
	//double step_size;				/// LCP�̃X�e�b�v��
	//double converge_criteria;		/// ���������臒l
	double max_error;

	bool DetectPenetration();		/// �S�̂̌����̌��m
	void PrintContacts();
	void SetupDynamics(double dt);	/// LCP�̏���
	void SetupCorrection(double dt);/// �덷�̏C��
	void IterateDynamics();			/// Correction()�ł̈�x�̔���
	void IterateCorrection();		/// Correction()�ł̈�x�̔���
	void UpdateSolids(double dt);	/// ���ʂ�Solid�ɔ��f����
	//void SetInitialValue();		/// LCP�̌���ϐ��̏����l��ݒ�
	
public:
	void Add(PHSolid* s);			/// Solid ��o�^����
	void Remove(PHSolid* s);		/// �o�^����Ă���Solid���폜����
	PHJoint* AddJoint(PHSolid* lhs, PHSolid* rhs, const PHJointDesc& desc);	/// �֐߂̒ǉ�����
	void EnableContact(PHSolid* lhs, PHSolid* rhs, bool bEnable);
	void Invalidate(){ready = false;}	/// ready�t���O�����Z�b�g
	void Init();						/// ���������Cready�t���O���Z�b�g
	///
	int GetPriority() const {return SGBP_CONSTRAINTENGINE;}
	///	���x���ʒu�A�����x�����x�̐ϕ�
	virtual void Step();
	virtual void Clear(PHScene* s);

	PHConstraintEngine();
	~PHConstraintEngine();

	friend class PHShapePair;
};


}	//	namespace Spr
#endif
