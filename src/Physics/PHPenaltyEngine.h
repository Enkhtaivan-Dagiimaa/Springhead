/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PHPENALTYENGINE_H
#define PHPENALTYENGINE_H

#include <Physics/PHContactDetector.h>

namespace Spr{;

class PHShapePairForPenalty : public CDShapePair{
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

class PHSolidInfoForPenalty : public PHSolidInfo{
public:
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
	PHSolidInfoForPenalty(PHSolid* solid):PHSolidInfo(solid){}
};

class PHPenaltyEngine;
class PHSolidPairForPenalty : public PHSolidPair<PHSolidInfoForPenalty, PHShapePairForPenalty, PHPenaltyEngine>{
public:
	typedef PHSolidPair<PHSolidInfoForPenalty, PHShapePairForPenalty, PHPenaltyEngine> base_type;
	typedef base_type::shapepair_type shapepair_type;
	typedef base_type::engine_type engine_type;

	Vec3f cocog;					///<	2���̂̏d�S�̒��_(��Όn)
	Vec3f reflexForce;				///<	�R��
	Vec3f reflexTorque;				///<	�R�͂ɂ��g���N(cocog�n)
	Vec3f frictionForce;			///<	���C��
	Vec3f frictionTorque;			///<	���C�͂ɂ��g���N(cocog�n)
	float convertedMass;			///<	���̂̑g�̊��Z����
	float area;						///<	�ڐG�ʐ�

	virtual void OnDetect(shapepair_type* sp, engine_type* engine, unsigned ct, double dt);	///< ���������m���ꂽ�Ƃ��̏���

	void Setup(unsigned int ct, double dt);
	void GenerateForce();

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
	void CalcReflexForce(PHShapePairForPenalty* sp, CDContactAnalysis* analyzer);
	void CalcTriangleReflexForce(PHShapePairForPenalty* cp, Vec3f p0, Vec3f p1, Vec3f p2, Vec3f cog, Vec3f vel, Vec3f angVel);
	void CalcFriction(PHShapePairForPenalty* cp);
};

class PHPenaltyEngine : public PHContactDetectorImp<PHSolidInfoForPenalty, PHShapePairForPenalty, PHSolidPairForPenalty, PHPenaltyEngine>{
	OBJECT_DEF_NOIF(PHPenaltyEngine);
public:
	int GetPriority() const {return SGBP_PENALTYENGINE;}
	virtual void Step();
};

}	//	namespace Spr
#endif
