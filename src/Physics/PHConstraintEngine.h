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
struct PHSolidAux{
	PHSolid*	solid;		/// PHSolid�ւ̎Q��
	double		minv;		/// ���ʂ̋t��
	Matrix3d	Iinv;		/// �����s��̋t�s��
	Vec3d		v0, w0;		/// �ڐG�͂�0�̏ꍇ��dt��̑��x
	Vec3d		dv, dw;		/// �ڐG�͂��l���������x�ω���
	Vec3d		dV, dW;		/// Correction�ɂ��ړ��ʁC��]��
	void SetupDynamics(double dt);
	void SetupCorrection();
};
class PHSolidAuxs : public std::vector<PHSolidAux>{
public:
	iterator Find(PHSolid* s){
		iterator is;
		for(is = begin(); is != end(); is++)
			if(is->solid == s)
				break;
		return is;
	};
	void SetupDynamics(double dt){
		for(iterator is = begin(); is != end(); is++)
			is->SetupDynamics(dt);
	}
	void SetupCorrection(){
		for(iterator is = begin(); is != end(); is++)
			is->SetupCorrection();
	}
};

///
class PHConstraint : public InheritSceneObject<PHConstraintIf, SceneObject>{
public:
	PHJointDesc::JointType	type;
	int			dim_v, dim_w, dim_q;
	int			idx_v[3], idx_w[3], idx_q[3];
	bool		bFeasible;			/// �����̍��̂�undynamical�ȏꍇtrue

	PHSolidAux* solid[2];
	Matrix3d	Rj[2];				/// �e���̂ɒ���t�����֐߃t���[��
	Vec3d		rj[2];
	Matrix3d	Rjrel;				/// �֐߃t���[���Ԃ̈ʒu�֌W
	Vec3d		rjrel;
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
	Vec3d		fv, fw, Fv, Fq, bv, bw, Bv, Bq;
	Vec3d		Av, Aw, Aq;
	
	void Init(PHSolidAux* lhs, PHSolidAux* rhs, const PHJointDesc& desc){
		solid[0] = lhs, solid[1] = rhs;
		for(int i = 0; i < 2; i++){
			desc.poseJoint[i].Ori().ToMatrix(Rj[i]);
			rj[i] = desc.poseJoint[i].Pos();
		}
	}
	void CompJacobian(bool bCompAngular);
	void SetupDynamics(double dt);
	void SetupCorrection(double dt);
	void IterateDynamics();
	void IterateCorrection();
	virtual void Projectionfv(double& f, int k){}
	virtual void Projectionfw(double& f, int k){}
	virtual void ProjectionFv(double& F, int k){}
	virtual void ProjectionFq(double& F, int k){}
	virtual void CompError() = 0;

	PHConstraint(PHJointDesc::JointType t);
};
class PHConstraints : public std::vector< UTRef<PHConstraint> >{
public:
	void SetupDynamics(double dt){
		for(iterator it = begin(); it != end(); it++)
			(*it)->SetupDynamics(dt);
	}
	void SetupCorrection(double dt){
		for(iterator it = begin(); it != end(); it++)
			(*it)->SetupCorrection(dt);
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

class PHContactPoint : public PHConstraint{
public:
	CDShapePair* shapePair;
	Vec3d pos;
	virtual void CompError();
	virtual void Projectionfv(double& f, int k);
	virtual void ProjectionFv(double& F, int k);
	PHContactPoint(CDShapePair* sp, Vec3d p, PHSolidAux* s0, PHSolidAux* s1);
};
	
class PHHingeJoint : public PHConstraint{
public:
	virtual void CompError();
	PHHingeJoint();
};
class PHSliderJoint : public PHConstraint{
	virtual void CompError();
	PHSliderJoint();

};
class PHBallJoint : public PHConstraint{
	virtual void CompError();
	PHBallJoint();
};

class PHConstraintEngine: public PHEngine{
	friend class PHSolidPair;

	OBJECTDEF(PHConstraintEngine);

	/// Solid�̑g�ݍ��킹�̔z��
	class PHSolidPair{
	public:
		PHSolidAux* solid[2];
		/// Shape�̑g�ݍ��킹�̔z��
		typedef UTCombination<CDShapePair> CDShapePairs;
		CDShapePairs	shapePairs;

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
	double step_size;				/// LCP�̃X�e�b�v��
	double converge_criteria;		/// ���������臒l

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
	void AddJoint(PHSolid* lhs, PHSolid* rhs, const PHJointDesc& desc);	/// �֐߂̒ǉ�����
	void EnableContact(PHSolid* lhs, PHSolid* rhs, bool bEnable);
	void Invalidate(){ready = false;}	/// ready�t���O�����Z�b�g
	void Init();						/// ���������Cready�t���O���Z�b�g
	///
	int GetPriority() const {return SGBP_CONSTRAINTENGINE;}
	///	���x���ʒu�A�����x�����x�̐ϕ�
	virtual void Step();
	virtual void Clear(PHScene* s){ solids.clear(); }

	PHConstraintEngine();
	~PHConstraintEngine();
};

}	//	namespace Spr
#endif
