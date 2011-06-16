/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_CRBODYIF_H
#define SPR_CRBODYIF_H

#include <Foundation/SprObject.h>

namespace Spr{;

//@{

struct PHSolidIf;
struct PHJointIf;
struct PHIKActuatorIf;
struct PHIKEndEffectorIf;

// ------------------------------------------------------------------------------

/// �N���[�`���̃{�f�B���\������v�f
struct CRBodyPartIf : SceneObjectIf {
	SPR_IFDEF(CRBodyPart);

	/** @brief ���x���i����������������FHand�Ȃǁj�̎擾
	 */
	const char* GetLabel() const;

	/** @brief ���x���i����������������FHand�Ȃǁj�̐ݒ�
	 */
	void SetLabel(const char* str);
};
struct CRBodyPartDesc {
	SPR_DESCDEF(CRBodyPart);
	std::string	label;			///<	���x���i����������������FHand�Ȃǁj
};

/// �N���[�`���̃{�f�B���\�����鍄��
struct CRSolidIf : CRBodyPartIf {
	SPR_IFDEF(CRSolid);

	/** @brief PHSolid���擾
	 */
	PHSolidIf* GetPHSolid();

	/** @brief PHSolid��ݒ�
	 */
	void SetPHSolid(PHSolidIf* so);
};
struct CRSolidDesc : CRBodyPartDesc {
	SPR_DESCDEF(CRSolid);
};

/// �N���[�`���̃{�f�B���\������IK�G���h�G�t�F�N�^�t���̍���
struct CRIKSolidIf : CRSolidIf {
	SPR_IFDEF(CRIKSolid);

	/** @brief IK�G���h�G�t�F�N�^���擾
	 */
	PHIKEndEffectorIf* GetIKEndEffector();

	/** @brief IK�G���h�G�t�F�N�^��ݒ�
	 */
	void SetIKEndEffector(PHIKEndEffectorIf* ikEE);
};
struct CRIKSolidDesc : CRSolidDesc {
	SPR_DESCDEF(CRSolid);
};

/// �N���[�`���̃{�f�B���\������֐�
struct CRJointIf : CRBodyPartIf {
	SPR_IFDEF(CRJoint);

	/** @brief PHJoint���擾
	 */
	PHJointIf* GetPHJoint();

	/** @brief PHJoint��ݒ�
	 */
	void SetPHJoint(PHJointIf* jo);

	/** @brief �o�l�_���p�W���̔{����ݒ�
	 */
	void SetSpringRatio(double springRatio, double damperRatio);
};
struct CRJointDesc : CRBodyPartDesc {
	SPR_DESCDEF(CRJoint);
};

/// �N���[�`���̃{�f�B���\������IK�A�N�`���G�[�^�t���̊֐�
struct CRIKJointIf : CRJointIf {
	SPR_IFDEF(CRIKJoint);

	/** @brief IK�A�N�`���G�[�^���擾
	 */
	PHIKActuatorIf* GetIKActuator();

	/** @brief IK�A�N�`���G�[�^��ݒ�
	 */
	void SetIKActuator(PHIKActuatorIf* ikAct);

	/** @brief �o�l�_���p�W���̔{����ݒ�
	 */
	void SetIKSpringRatio(double springRatio, double damperRatio);
};
struct CRIKJointDesc : CRJointDesc {
	SPR_DESCDEF(CRJoint);
};

// ------------------------------------------------------------------------------

/// �N���[�`���̃{�f�B���f���̃C���^�[�t�F�C�X
struct CRBodyIf : SceneObjectIf{
	SPR_IFDEF(CRBody);

	/** @brief �\�����̂̐��𓾂�
	*/
	int NSolids();

	/** @brief i�Ԗڂ̍\�����̂𓾂�
	*/
	CRSolidIf* GetSolid(int i);

	/** @brief �\���֐߂̐��𓾂�
	*/
	int NJoints();

	/** @brief i�Ԗڂ̍\���֐߂𓾂�
	*/
	CRJointIf* GetJoint(int i);

	/** @brief �{�f�B�̏d�S���W�𓾂�
	*/
	Vec3d GetCenterOfMass();

	/** @brief �{�f�B�̑����ʂ𓾂�
	*/
	double GetSumOfMass();
};

/// �N���[�`���̃{�f�B���f���̃f�X�N���v�^
struct CRBodyDesc{
	SPR_DESCDEF(CRBody);

	CRBodyDesc(){
	}
};

}

#endif//SPR_CRBODY_H
