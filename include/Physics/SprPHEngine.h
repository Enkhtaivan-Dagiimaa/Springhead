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
	IF_DEF(PHEngine);
};

struct PHConstraintEngineIf : PHEngineIf{
public:
	IF_DEF(PHConstraintEngine);

	/** @brief �ڐG�_�̏W����Ԃ�
	*/
	virtual PHConstraintsIf* GetContactPoints() = 0;

	/** @brief velCorrectionRate������������
		@param value ����������̒l
	*/
	virtual void SetVelCorrectionRate(double value) = 0;

	/** @brief posCorrectionRate������������
		@param value ����������̒l
	*/
	virtual void SetPosCorrectionRate(double value) = 0;

};

struct PHGravityEngineIf : PHEngineIf{
public:
	IF_DEF(PHGravityEngine);
};

struct PHPenaltyEngineIf : PHEngineIf{
public:
	IF_DEF(PHPenaltyEngine);
};
//@}
}
#endif
