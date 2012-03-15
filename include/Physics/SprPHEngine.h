/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_PHENGINEIF_H
#define SPR_PHENGINEIF_H

#include <Foundation/SprObject.h>

namespace Spr{;

struct PHConstraintsIf;

/** \addtogroup gpPhysics */
//@{

///	�G���W���̊�{�N���X
struct PHEngineIf : public SceneObjectIf{
public:
	SPR_IFDEF(PHEngine);
};

struct PHConstraintEngineIf : PHEngineIf{
public:
	SPR_IFDEF(PHConstraintEngine);

	/** @brief �ڐG�_�̏W����Ԃ�
	*/
	PHConstraintsIf* GetContactPoints();

	/** @brief velCorrectionRate������������
		@param value ����������̒l
	*/
	void SetVelCorrectionRate(double value);

	/** @brief velCorrectionRate���擾����
		@return value velCorrectionRate�̒l
	*/
	double GetVelCorrectionRate();

	/** @brief posCorrectionRate������������
		@param value ����������̒l
	*/
	void SetPosCorrectionRate(double value);

	/** @brief posCorrectionRate���擾����
		@return value posCorrectionRate�̒l
	*/
	double GetPosCorrectionRate();

	/** @brief contactCorrectionRate������������
		@param value ����������̒l
	*/
	void SetContactCorrectionRate(double value);

	/** @brief contactCorrectionRate���擾����
		@return value contactCorrectionRate�̒l
	*/
	double GetContactCorrectionRate();

	/** @brief bSaveConstraints������������
		@return value SaveState, LoadState�ɁC constraints ���܂߂邩�ǂ����D�{���s�v�����Cf, F���ω�����D
	*/
	void	SetBSaveConstraints(bool value);

	/** @brief ���̂̑��x�����X�V����t���O��ݒ肷��
	*/
	void SetUpdateAllSolidState(bool flag);

	/** @brief �ʐڐG���g���t���O��ݒ肷��
	*/
	void SetUseContactSurface(bool flag);

	/** @brief shrinkRate�̒l��ݒ肷��
	*/
	void SetShrinkRate(double data);
	
	/** @brief shrinkRate���擾����
		@return value shrinkCorrectionRate�̒l
	*/
	double GetShrinkRate();

	/** @brief shrinkRateCorrection�̒l��ݒ肷��
	*/
	void SetShrinkRateCorrection(double data);

	/** @brief shrinkRateCorrection���擾����
		@return value shrinkRateCorrection�̒l
	*/
	double GetShrinkRateCorrection();

};

struct PHGravityEngineIf : PHEngineIf{
public:
	SPR_IFDEF(PHGravityEngine);
};

struct PHPenaltyEngineIf : PHEngineIf{
public:
	SPR_IFDEF(PHPenaltyEngine);

};

struct PHIKEngineIf : PHEngineIf{
public:
	/** @brief �L�������������擾����
	*/
	bool IsEnabled();

	/** @brief �L���E������؂�ւ���
	*/
	void Enable(bool bEnabled);

	/** @brief �J��Ԃ��񐔂��Z�b�g����
	*/
	void SetNumIter(int numIter);

	SPR_IFDEF(PHIKEngine);
};

struct PHFemEngineIf : PHEngineIf{
public:
	SPR_IFDEF(PHFemEngine);
};

struct PHHapticEngineDesc{
	enum HapticEngineMode{
		SINGLE_THREAD,
		MULTI_THREAD,
		LOCAL_DYNAMICS,
	};
	PHHapticEngineDesc();
};
struct PHHapticEngineIf : public PHHapticEngineDesc, PHEngineIf{
public:
	SPR_IFDEF(PHHapticEngine);
	void EnableHapticEngine(bool b);
	void SetHapticEngineMode(HapticEngineMode mode);
	void StepPhysicsSimulation();
};
//@}
}
#endif
