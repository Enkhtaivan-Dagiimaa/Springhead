/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_PHENGINEIF_H
#define SPR_PHENGINEIF_H

namespace Spr{;
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

	/** @brief posCorrectionRate������������
		@param value ����������̒l
	*/
	void SetPosCorrectionRate(double value);

	/** @brief contactCorrectionRate������������
		@param value ����������̒l
	*/
	void SetContactCorrectionRate(double value);

	/** @brief ���̂̑��x�����X�V����t���O��ݒ肷��
	*/
	void SetUpdateAllSolidState(bool flag);

	/** @brief shrinkRate�̒l��ݒ肷��
	*/
	void SetShrinkRate(double data);
	
	/** @brief shrinkRateCorrection�̒l��ݒ肷��
	*/
	void SetShrinkRateCorrection(double data);

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
	/** @brief �L���E������؂�ւ���
	*/
	void Enable(bool bEnabled);

	/** @brief �J��Ԃ��񐔂��Z�b�g����
	*/
	void SetNumIter(int numIter);

	SPR_IFDEF(PHIKEngine);
};
//@}
}
#endif
