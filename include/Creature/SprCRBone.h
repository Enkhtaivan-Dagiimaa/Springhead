/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_CRBONEIF_H
#define SPR_CRBONEIF_H

#include <Foundation/SprObject.h>

namespace Spr{;

struct PHSolidIf;
struct PHJointIf;
struct PHIKActuatorIf;
struct PHIKEndEffectorIf;

/// �N���[�`���̃{�[���i���̈�{�e�{�[���ւ̊֐߈�j
struct CRBoneIf : SceneObjectIf {
	SPR_IFDEF(CRBone);

	/** @brief ���x���i����������������FHand�Ȃǁj�̎擾
	 */
	const char* GetLabel() const;

	/** @brief ���x���i����������������FHand�Ȃǁj�̐ݒ�
	 */
	void SetLabel(const char* str);

	/** @brief PHSolid���擾
	 */
	PHSolidIf* GetPHSolid();

	/** @brief PHSolid��ݒ�
	 */
	void SetPHSolid(PHSolidIf* so);

	/** @brief IK�G���h�G�t�F�N�^���擾
	 */
	PHIKEndEffectorIf* GetIKEndEffector();

	/** @brief IK�G���h�G�t�F�N�^��ݒ�
	 */
	void SetIKEndEffector(PHIKEndEffectorIf* ikEE);

	/** @brief PHJoint���擾
	 */
	PHJointIf* GetPHJoint();

	/** @brief PHJoint��ݒ�
	 */
	void SetPHJoint(PHJointIf* jo);

	/** @brief IK�A�N�`���G�[�^���擾
	 */
	PHIKActuatorIf* GetIKActuator();

	/** @brief IK�A�N�`���G�[�^��ݒ�
	 */
	void SetIKActuator(PHIKActuatorIf* ikAct);
};

struct CRBoneDesc {
	SPR_DESCDEF(CRBone);
	std::string	label;			///<	���x���i����������������FHand�Ȃǁj
};

}

#endif//SPR_CRBONEIF_H