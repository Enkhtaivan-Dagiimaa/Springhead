#ifndef PHCONSTRAINTENGINE_H
#define PHCONSTRAINTENGINE_H

#include <SprPhysics.h>
#include <Foundation/Object.h>
#include <Physics/PHScene.h>
#include <Physics/PHEngine.h>
#include <Collision/CDDetectorImp.h>
#include <vector>

namespace Spr{;

/// 1�̐ڐG�_
struct PHContactPoint{
	int	contact;				/// ������ڐG
	Vec3d pos;					/// �ڐG�_�̈ʒu
	double depth;				/// �����[�x
	Matrix3d Jlin[2], Jang[2];	/// J�s��̃u���b�N		�ڐG���Ă��鍄�̂̑��x����ڐG�_�ł̑��x��^���郄�R�r�s��
	Matrix3d Tlin[2], Tang[2];	/// T�s��̃u���b�N		�ڐG�͂��獄�̂̑��x�ω���^����s��
	Matrix3d A;					/// A�s��Ίp�u���b�N	���̐ڐG�_�̊����s��
	Vec3d b;					/// b�x�N�g���̃u���b�N	�ڐG�͂�0�Ƃ����ꍇ��dt��̐ڐG�_�ł̑��Α��x
	double B;					/// B�x�N�g���̗v�f		�ڐG�͂��l������dt��̌����[�x
	Vec3d f;					/// �ڐG��(LCP�̑���ϐ�)
	Vec3d f0;					/// �����ł̏����l(��������ΑO��̉�)
	Vec3d df;					/// �e�����ł̐ڐG�͂̕ω���(��������Ɏg�p)

	PHContactPoint(int c, Vec3d p){
		contact = c;
		pos = p;
	}
};
typedef std::vector<PHContactPoint> PHContactPoints;

/// Shape���m�̐ڐG
class PHContact{
public:
	int solid[2];				/// �ڐG���Ă��鍄��
	int shape[2];				/// �ڐG���Ă���`��
	Vec3d normal;				/// �@��
	Vec3d center;				/// �����`��̏d�S
	double depth;
	double mu;					/// ���C�W��
	
	PHContact(){}
	/*PHContact(int solid0, int solid1, int shape0, int shape1, Vec3d n, Vec3d c, double _mu){
		solid[0] = solid0;
		solid[1] = solid1;
		shape[0] = shape0;
		shape[1] = shape1;
		normal = n;
		center = c;
		mu = _mu;
	}*/
};

/// �S�Ă̐ڐG��ێ�����R���e�i
class PHContacts : public std::vector<PHContact>{
public:
};

class PHConstraintEngine: public PHEngine{
	friend class PHSolidPair;

	OBJECTDEF(PHConstraintEngine);

	/// �ڐG�Ɋ֗^���鍄�̂̏��
	struct PHSolidAux{
		double		minv;				/// �e���̂̎��ʂ̋t��
		Matrix3d	Iinv;				/// �e���̂̊����s��̋t�s��
		Vec3d		Vlin0, Vang0;		/// �ڐG�͂�0�̏ꍇ��dt��̑��x
		Vec3d		dVlin, dVang;		/// �ڐG�͂��l���������x�ω���(LCP�������ċ��߂�)
	};
	typedef std::vector<PHSolidAux> PHSolidAuxs;

	/// Solid�̑g�ݍ��킹�̔z��
	class PHSolidPair{
	public:
		/// Shape�̑g�ݍ��킹�̔z��
		typedef UTCombination<CDShapePair> CDShapePairs;
		CDShapePairs	shapePairs;

		void Init(PHSolid* s0, PHSolid* s1);	/// ������
		bool Detect(int s0, int s1, PHConstraintEngine* engine);	
		
	};
	typedef UTCombination<PHSolidPair> PHSolidPairs;
	
protected:
	bool			ready;		/// 
	PHSolids		solids;		/// �S���͌v�Z�̑ΏۂƂȂ鍄��
	PHSolidAuxs		solidAuxs;	/// ���̂̕t�����
	PHSolidPairs	solidPairs;
	PHContacts		contacts;	/// ���̓��m�̐ڐG�̔z��
	PHContactPoints	points;		///	�ڐG�_�̔z��
	int max_iter_dynamics;		/// Dynamics()�̔�����
	int max_iter_correction;	/// Correction()�̔�����

	bool Detect();				/// �S�̂̌����̌��m
	void PrintContacts();
	void Setup(double dt);	/// LCP�̏���
	//void SetInitialValue();		/// LCP�̌���ϐ��̏����l��ݒ�
	bool CheckConvergence();	/// �����@�ɂ������������
	void Dynamics(double dt);			/// ���x�ƈʒu�̍X�V
	void Correction();			/// �덷�̏C��
	void IterateDynamics();	/// Dynamics()�ł̈�x�̔���		
	void IterateCorrection();	/// Correction()�ł̈�x�̔���
	void UpdateSolids(double dt);		/// ���ʂ�Solid�ɔ��f����

public:
	void Add(PHSolid* s);		/// Solid ��o�^����
	void Remove(PHSolid* s);	/// �o�^����Ă���Solid���폜����
	void Invalidate(){ready = false;}	/// ready�t���O�����Z�b�g
	void Init();				/// ���������Cready�t���O���Z�b�g
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
