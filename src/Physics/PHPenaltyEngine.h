#ifndef PHPENALTYENGINE_H
#define PHPENALTYENGINE_H

#include <SprPhysics.h>
#include <Foundation/Object.h>
#include <Physics/PHScene.h>
#include <Physics/PHEngine.h>
#include <Base/Combination.h>
#include <Collision/CDDetectorImp.h>

namespace Spr{;


class PHPenaltyEngine: public PHEngine{
	OBJECTDEF(PHPenaltyEngine);

	
	class PHShapePair:public CDShapePair{
	public:
		///@name	�R�͂̌v�Z
		//@{
		float area;							///<	�������̖ʐ�
		Vec3f reflexSpringForce;			///<	�΂˂ɂ��R��
		Vec3f reflexDamperForce;			///<	�_���p�[�ɂ��R��
		Vec3f reflexSpringTorque;			///<	�΂˂ɂ��R�g���N(commonPoint�n)
		Vec3f reflexDamperTorque;			///<	�_���p�[�ɂ��R�g���N(commonPoint�n)
		Vec3f reflexForcePoint;				///<	�R�͂̍�p�_(commonPoint�n)
		//@}

		///@name���C�̌v�Z
		//@{
		Vec3f transFrictionBase[2];			///<	���i�Î~���C�p�o�l�̒[�_(���[�J���t���[���n)
		float rotSpring;					///<	��]�΂˂̐L��(���W�A��)
		Vec3f dynaFric;						///<	�����C��
		Vec3f dynaFricMom;					///<	�����C�͂̃��[�����g(commonPoint�n)
		Vec3f frictionForce;				///<	���C��
		Vec3f frictionTorque;				///<	���C�g���N(���C�͂̍�p�_ �� (reflexForcePoint+commonPoint)�n)
		enum FrictionState{ STATIC, DYNAMIC };
		FrictionState frictionState;		///<	���C�̏��
		//@}

		PHShapePair(CDShape* s1, CDShape* s2):CDShapePair(s1, s2){
		}
		void Clear();

		///@name	��΍��W�n�ł̓ǂݏo��
		//@{
		///	�o�l�ɂ��R�͂̍�p�_
		Vec3f GetReflexForcePoint(){ return reflexForcePoint + commonPoint;}
		///	�o�l�ɂ��R��
		Vec3f GetReflexSpringForce(){ return reflexSpringForce;}
		///	�_���p�ɂ��R��
		Vec3f GetReflexDamperForce(){ return reflexDamperForce;}
		///	�R��
		Vec3f GetReflexForce(){ return reflexSpringForce + reflexDamperForce;}

		///	���C��
		Vec3f GetFrictionForce(){ return frictionForce; }
		///	���C�g���N
		Vec3f GetFrictionTorque(){ return frictionTorque + (GetReflexForcePoint()^frictionForce); }
		//@}
	};


	class PHSolidInfo{
	public:
		PHSolid* solid;
		///@name	UpdateChache�ōX�V����ϐ�
		//@{
		/**	�Ō�ɐڐG�������� = �L���b�V���p�����[�^���Ō�ɍX�V���������D
			�ڐG���ɁC�L���b�V�����X�V����̂ŁCcount �����݂̎����Ɠ��������
			�Փ˂��N�������Ƃ��Ӗ�����D	*/
		unsigned count;
		//	�t���[��(����)�P�ʂ̃p�����[�^
		Vec3f cog, vel, angVel;			///<	�d�S�C���x�C�p���x
		Vec3f pos, lastPos;				///<	�ʒu�C�Ō�̈ʒu
		Quaternionf ori, lastOri;		///<	�����C�O��̌���
		//@}
		void UpdateCache(int c);		///<	�L���b�V���ϐ������̂Ȃǂ������Ă���D
	};


	class PHSolidPair: public UTRefCount{
	public:
		PHSolidInfo solid[2];
		UTCombination<UTRef< PHShapePair> > shapePairs;

		Vec3f cocog;					///<	2���̂̏d�S�̒��_(��Όn)
		Vec3f reflexForce;				///<	�R��
		Vec3f reflexTorque;				///<	�R�͂ɂ��g���N(cocog�n)
		Vec3f frictionForce;			///<	���C��
		Vec3f frictionTorque;			///<	���C�͂ɂ��g���N(cocog�n)
		float area;

		PHSolidPair(PHSolid* s0, PHSolid* s1){
			solid[0].solid = s0;
			solid[1].solid = s1;
		}
		void Init();
		void Clear();
		bool Detect(PHPenaltyEngine* engine);

		///@name	��Όn�ł̓ǂݏo��
		//@{
		///	�R��
		Vec3f GetReflexForce(){ return reflexForce; }
		///	�R�͂̃g���N
		Vec3f GetReflexTorque(){ return reflexTorque + (cocog^reflexForce); }
		///	���C��
		Vec3f GetFrictionForce(){ return frictionForce; }
		///	���C�͂̃g���N
		Vec3f GetFrictionTorque(){ return frictionTorque + (cocog^frictionForce); }
		//@}
		
		///	�͂̍ő�l�𐧖񂷂�D
		void LimitForces(){
			bool b = false;
			b |= LimitForce(reflexForce);
			b |= LimitForce(frictionForce);
			b |= LimitForce(reflexTorque);
			b |= LimitForce(frictionTorque);
			if (b){
				reflexForce = frictionForce = reflexTorque = frictionTorque = Vec3f();
			}
		}
	protected:
		bool LimitForce(Vec3f& f){
			float sq = f.square();
			const float MF = 200000;
			if (!(sq < MF*MF)){
				return true;
			}
			return false;
		}
		void CalcReflexForce(PHShapePair* sp, CDContactAnalysis* analyzer);
		void CalcTriangleReflexForce(PHShapePair* cp, Vec3f p0, Vec3f p1, Vec3f p2, Vec3f cog, Vec3f vel, Vec3f angVel);
		void CalcFriction(PHShapePair* cp);
	};


protected:
	bool	 ready;
	PHSolids solids;
	UTCombination<UTRef<PHSolidPair> > solidPairs;
public:
	void Add(PHSolid* s);	/// Solid��o�^����
	void Invalidate(){ready = false;}		/// ready�t���O�����Z�b�g����
	void Init();			/// ���������Cready�t���O���Z�b�g����
	///
	int GetPriority() const {return SGBP_PENALTYENGINE;}
	///	���x���ʒu�A�����x�����x�̐ϕ�
	virtual void Step();
	virtual void Clear(PHScene* s){ solids.clear(); }

	PHPenaltyEngine():ready(false){}
};

}	//	namespace Spr
#endif
