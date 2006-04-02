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
	friend class PHSolid;

	OBJECTDEF(PHConstraintEngine);

	/// Solid�̑g�ݍ��킹�̔z��
	class PHSolidPair{
	public:
		bool bEnabled;
		UTRef<PHSolidAux> solid[2];
		/// Shape�̑g�ݍ��킹�̔z��
		typedef UTCombination<PHShapePair> PHShapePairs;
		PHShapePairs	shapePairs;

		void Init(PHSolidAux* s0, PHSolidAux* s1);	/// ������
		bool Detect(PHConstraintEngine* engine);	

	};
	typedef UTCombination<PHSolidPair> PHSolidPairs;
	
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
	
public:
	void Add(PHSolid* s);			/// Solid ��o�^����
	void Remove(PHSolid* s);		/// �o�^����Ă���Solid���폜����
	PHJoint* AddJoint(PHSolid* lhs, PHSolid* rhs, const PHJointDesc& desc);	/// �֐߂̒ǉ�����
	void EnableContact(PHSolid* lhs, PHSolid* rhs, bool bEnable);
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
