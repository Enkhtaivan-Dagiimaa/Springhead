#ifndef PHCONSTRAINTENGINE_H
#define PHCONSTRAINTENGINE_H

#include <Physics/PHContactDetector.h>
#include <Physics/PHConstraint.h>

namespace Spr{;

class PHSolid;
class PHJoint;
class PHConstraintEngine;

///	�`��̑g
class PHShapePairForLCP : public CDShapePair{
public:
	///	�R���X�g���N�^
	//PHShapePairForLCP(){}
	//PHShapePair(CDShape* s0, CDShape* s1):CDShapePair(s0, s1){}
	///	�ڐG����D�ڐG�̒��S(center �ŐN���_�̒��_)�Ɩ@���x�N�g���܂ŋ��߂�
	//bool Detect(unsigned ct, PHSolid* s0, PHSolid* s1, int ishape0, int ishape1);
	void CalcNormal(PHSolid* solid0, PHSolid* solid1);
	///	�ڐG��́D�ڐG�����̐؂�������߂āC�؂�����\������ʑ��p�`�̒��_��engine�ɍS���Ƃ��Ēǉ�����D
	void EnumVertex(PHConstraintEngine* engine, unsigned ct, PHSolidInfoForLCP* solid0, PHSolidInfoForLCP* solid1);
	//void SetState(const PHShapePairState& s);
};

/// Solid�̑g
class PHConstraintEngine;
class PHSolidPairForLCP : public PHSolidPair<PHSolidInfoForLCP, PHShapePairForLCP, PHConstraintEngine>{
public:
	//virtual void Clear();
	//virtual void Init(PHSolidInfoFor* s0, PHSolidInfo* s1);	/// ������
	virtual void OnDetect(PHShapePairForLCP* cp, PHConstraintEngine* engine, unsigned ct, double dt);
};

class PHConstraintEngine : public PHContactDetectorImp<PHSolidInfoForLCP, PHShapePairForLCP, PHSolidPairForLCP, PHConstraintEngine>{
	typedef PHContactDetectorImp<PHSolidInfoForLCP, PHShapePairForLCP, PHSolidPairForLCP, PHConstraintEngine> base_type;
	/*friend class PHSolidPair;
	friend class PHSolid;*/
	friend class PHConstraint;

	OBJECT_DEF_NOIF(PHConstraintEngine);

	PHJoint* CreateJoint(const PHJointDesc& desc);
	
public:
	PHConstraintEngine();
	~PHConstraintEngine();
	//void Add(PHSolid* s);			///< Solid ��o�^����
	//void Remove(PHSolid* s);		///< �o�^����Ă���Solid���폜����
	PHJoint* AddJoint(const PHJointDesc& desc);	///< �֐߂�ǉ�����i�t�@�C�����[�_�p�j
	PHJoint* AddJoint(PHSolid* lhs, PHSolid* rhs, const PHJointDesc& desc);	///< �֐߂̒ǉ�����
	
	virtual int GetPriority() const {return SGBP_CONSTRAINTENGINE;}
	virtual void Step();	///<	���x���ʒu�A�����x�����x�̐ϕ�
	virtual void Clear();

protected:
	//PHSolidAuxs		solids;			/// ���̂̔z��
	//PHSolidPairs	solidPairs;	
	PHConstraints	points;			///	�ڐG�_�̔z��
	PHConstraints	joints;			///	�֐߂̔z��
	int max_iter_dynamics;			/// Dynamics()�̔�����
	int max_iter_correction;		/// Correction()�̔�����
	//double step_size;				/// LCP�̃X�e�b�v��
	//double converge_criteria;		/// ���������臒l
	double max_error;

	void SetupDynamics(double dt);	/// LCP�̏���
	void SetupCorrection(double dt);/// �덷�̏C��
	void IterateDynamics();			/// Correction()�ł̈�x�̔���
	void IterateCorrection();		/// Correction()�ł̈�x�̔���
	void UpdateSolids(double dt);	/// ���ʂ�Solid�ɔ��f����
	//void SetInitialValue();		/// LCP�̌���ϐ��̏����l��ݒ�
	
	friend class PHShapePairForLCP;
	//friend struct PHConstraintEngineState;
};


}	//	namespace Spr
#endif
