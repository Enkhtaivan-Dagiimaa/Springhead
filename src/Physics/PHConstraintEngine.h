#ifndef PHCONSTRAINTENGINE_H
#define PHCONSTRAINTENGINE_H

#include <SprPhysics.h>
//#include <Foundation/Object.h>
#include <Physics/PHScene.h>
#include <Physics/PHEngine.h>
#include <Collision/CDDetectorImp.h>
//#include <vector>
#include "PHConstraint.h"

namespace Spr{;

class PHSolid;
class PHJoint;
class PHConstraintEngine;

struct PHShapePairState;
///	Shape�̑g
class PHShapePair: public CDShapePair{
public:
	///	
	PHShapePair(){}
	PHShapePair(CDShapeRefWithPose* s0, CDShapeRefWithPose* s1):CDShapePair(s0, s1){}
	///	�ڐG����D�ڐG�̒��S(center �ŐN���_�̒��_)�Ɩ@���x�N�g���܂ŋ��߂�
	bool Detect(unsigned ct, PHSolidAux* solid0, PHSolidAux* solid1);
	///	�ڐG��́D�ڐG�����̐؂�������߂āC�؂�����\������ʑ��p�`�̒��_��engine�ɍS���Ƃ��Ēǉ�����D
	void EnumVertex(PHConstraintEngine* engine, unsigned ct, PHSolidAux* solid0, PHSolidAux* solid1);
	void SetState(const PHShapePairState& s);
};
/// Shape�̑g�̏��
struct PHShapePairState{
	unsigned lastContactCount;
	Vec3d normal;
	double depth;
	PHShapePairState(const PHShapePair& s):
		lastContactCount(s.lastContactCount),normal(s.normal), depth(s.depth){}
};

/// Solid�̑g�̏�ԕ���
struct PHSolidPairState{
	bool bEnabled;
};
/// Solid�̑g
class PHSolidPair: public PHSolidPairState{
public:
	UTRef<PHSolidAux> solid[2];
	/// Shape�̑g�ݍ��킹�̔z��
	typedef UTCombination<PHShapePair> PHShapePairs;
	PHShapePairs	shapePairs;

	void Init(PHSolidAux* s0, PHSolidAux* s1);	/// ������
	bool Detect(PHConstraintEngine* engine);
	void SetState(const PHSolidPairState& s){
		*((PHSolidPairState*)this) = s;
	}
};


class PHConstraintEngine;
///	PHConstraintEngine�̏��
struct PHConstraintEngineState{
	size_t nSolidPair;	//	SolidPair�̐�
	size_t nShapePair;	//	ShapePair�̐�
	PHSolidPairState* SolidStates(){
		char* ptr = ((char*)this) + sizeof(*this);
		return (PHSolidPairState*)ptr;
	}
	PHShapePairState* ShapeStates(){
		char* ptr = ((char*)this) + sizeof(*this) + nSolidPair*sizeof(PHSolidPairState);
		return (PHShapePairState*)ptr;
	}
	size_t GetSize(){
		return sizeof(*this) + nSolidPair*sizeof(PHSolidPairState)
			+ nShapePair*sizeof(PHShapePairState);
	}
	PHConstraintEngineState(const PHConstraintEngine& ce);
};
class PHConstraintEngine: public PHEngine{
	friend class PHSolidPair;
	friend class PHSolid;
	friend class PHConstraint;

	OBJECT_DEF(PHConstraintEngine);

	typedef UTCombination<PHSolidPair> PHSolidPairs;

	PHJoint* CreateJoint(const PHJointDesc& desc);
	
public:
	PHConstraintEngine();
	~PHConstraintEngine();
	void Add(PHSolid* s);			///< Solid ��o�^����
	void Remove(PHSolid* s);		///< �o�^����Ă���Solid���폜����
	PHJoint* AddJoint(const PHJointDesc& desc);	///< �֐߂�ǉ�����i�t�@�C�����[�_�p�j
	PHJoint* AddJoint(PHSolid* lhs, PHSolid* rhs, const PHJointDesc& desc);	///< �֐߂̒ǉ�����
	void EnableContact(PHSolid* lhs, PHSolid* rhs, bool bEnable);
	///
	int GetPriority() const {return SGBP_CONSTRAINTENGINE;}
	///	���x���ʒu�A�����x�����x�̐ϕ�
	virtual void Step();
	virtual void Clear(PHScene* s);

protected:
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
	
	/// Solid��Shape���ǉ����ꂽ�Ƃ���Solid����Ă΂��
	void UpdateShapePairs(PHSolid* solid); 

	virtual size_t GetStateSize() const ;
	virtual void ConstructState(void* m) const ;
	virtual void DestructState(void* m) const ;
	virtual bool GetState(void* s) const;
	virtual void SetState(const void* s);

	friend class PHShapePair;
	friend struct PHConstraintEngineState;
};


}	//	namespace Spr
#endif
