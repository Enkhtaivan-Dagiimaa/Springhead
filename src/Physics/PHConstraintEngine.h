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
	PHSolidAux* solid[2];
	Matrix3d	Rj[2];		/// �e���̂ɒ���t�����֐߃t���[��
	Vec3d		rj[2];
	Matrix3d	Rjrel;		/// �֐߃t���[���Ԃ̈ʒu�֌W
	Vec3d		rjrel;
	Matrix3d	Jvrel_v[2], Jvrel_w[2], Jwrel_v[2], Jwrel_w[2];
	void Init(PHSolidAux* lhs, PHSolidAux* rhs, const PHJointDesc& desc){
		solid[0] = lhs, solid[1] = rhs;
		for(int i = 0; i < 2; i++){
			desc.poseJoint[i].Ori().ToMatrix(Rj[i]);
			rj[i] = desc.poseJoint[i].Pos();
		}
	}
	void CompRelativeVelJacobian();
	virtual void SetupDynamics(double dt) = 0;
	virtual void SetupCorrection(double dt) = 0;
	virtual void IterateDynamics() = 0;
	virtual void IterateCorrection() = 0;
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

template<int N>
class PHConstraintND : public PHConstraint{
public:
	typedef PTM::TVector<N, double> VecNd;
	VecNd	f, F, b, B;
	PTM::TMatrixCol<N, 3, double>	Jvrel, Jwrel;
	PTM::TMatrixCol<N, 3, double>	Jv[2], Jw[2];
	PTM::TMatrixCol<3, N, double>	Tv[2], Tw[2];
	PTM::TMatrixCol<N, N, double>	A, Ainv;
	PTM::TVector<6-N, double>		u;
	virtual void CompJacobian();
	virtual void SetupDynamics(double dt);
	virtual void SetupCorrection(double dt);
	virtual void IterateDynamics();
	virtual void IterateCorrection();
	virtual void CompJointJacobian(){}
	virtual void ProjectionDynamics(VecNd& f){}
	virtual void ProjectionCorrection(VecNd& F){}
	virtual void CompError() = 0;
};

class PHContactPoint : public PHConstraintND<3>{
public:
	CDShapePair* shapePair;
	Vec3d pos;
	virtual void CompJacobian();
	virtual void CompError();
	virtual void ProjectionDynamics(VecNd& f);
	virtual void ProjectionCorrection(VecNd& F);
	PHContactPoint(CDShapePair* sp, Vec3d p, PHSolidAux* s0, PHSolidAux* s1):shapePair(sp), pos(p){
		solid[0] = s0, solid[1] = s1;
	}
};
	
class PHHingeJoint : public PHConstraintND<5>{
public:
	virtual void CompJointJacobian();
	virtual void CompError();
	PHHingeJoint();
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
