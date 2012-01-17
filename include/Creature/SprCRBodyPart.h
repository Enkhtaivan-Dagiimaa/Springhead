/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_CRBODYPARTIF_H
#define SPR_CRBODYPARTIF_H

#include <Foundation/SprObject.h>

namespace Spr{;

//@{

struct PHSolidIf;
struct PHJointIf;
struct PHIKActuatorIf;
struct PHIKEndEffectorIf;

// ------------------------------------------------------------------------------
// �x�[�X

/// �N���[�`���̃{�f�B���\������v�f
struct CRBodyPartIf : SceneObjectIf {
	SPR_IFDEF(CRBodyPart);

	/** @brief ���x���i����������������FHand�Ȃǁj�̎擾
	 */
	const char* GetLabel() const;

	/** @brief ���x���i����������������FHand�Ȃǁj�̐ݒ�
	 */
	void SetLabel(const char* str);

	/** @brief �P�X�e�b�v
	*/
	void Step();
};
struct CRBodyPartDesc {
	SPR_DESCDEF(CRBodyPart);
	std::string	label;			///<	���x���i����������������FHand�Ȃǁj
};

// ------------------------------------------------------------------------------
// ����

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

	// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
	// ��{�@�\

	/** @brief IK�G���h�G�t�F�N�^���擾
	 */
	PHIKEndEffectorIf* GetIKEndEffector();

	/** @brief IK�G���h�G�t�F�N�^��ݒ�
	 */
	void SetIKEndEffector(PHIKEndEffectorIf* ikEE);

	// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
	// �O���^��

	/** @brief ���B�̑��Ί�ƂȂ鍄�̂�ݒ� NULL���Ɛ�Έʒu
		@param solid �����
	*/
	void SetOriginSolid(PHSolidIf* solid);

	/** @brief �ڕW�ʒu�̐ݒ�
		@param pos �ڕW�ʒu
	*/
	void SetTargetPos(Vec3d pos);

	/** @brief �ڕW�p���̐ݒ�
		@param ori �ڕW�p��
	*/
	void SetTargetOri(Quaterniond ori);

	/** @brief �ڕW�ʒu�E�p���̐ݒ�
		@param pose �ڕW�ʒu�E�p��
	*/
	void SetTargetPose(Posed pose);

	/** @brief �ڕW���B�����̐ݒ�
		@param timeLimit ���B�܂ł̖ڕW�o�ߎ���
	*/
	void SetTimeLimit(float timeLimit);

	/** @brief ����J�n
	*/
	void Start();

	/** @brief ����̈ꎞ��~
	*/
	void Pause();

	/** @brief �����~
	*/
	void Stop();

	/** @brief �O�����v�悷��
	*/
	void Plan();

	// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
	// ���̒T��

	/** @brief ���̒T���p�̉~���`�̃G���A��ݒu����
		@param relativePose �~����u�����΍��W�n��ݒ�F���_���~���̒��_�AZ��+���~���̎��AY��+�������
		@param horizRange ���������̕��i���W�A���j �}�C�i�X�Ȃ疳��
		@param vertRange  ���������̕��i���W�A���j �}�C�i�X�Ȃ疳��
	*/
	void SetViewArea(Posed relativePose, double horizRange, double vertRange);

	/** @brief �ݒ肳�ꂽ�~���`�̃G���A���ɑ��݂��鍄�̂̐�
	*/
	int NVisibleSolids();

	/** @brief �ݒ肳�ꂽ�~���`�̃G���A���ɑ��݂��鍄�̂�Ԃ�
	*/
	PHSolidIf* GetVisibleSolid(int i);
	
	// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
	// �ڐG�ꗗ

	/** @brief �ڐG�̐���Ԃ�
	*/
	int NContacts();

	/** @brief i�Ԗڂ̐ڐG�̐ڐG�͂�Ԃ�
	*/
	Vec3f GetContactForce(int i);

	/** @brief i�Ԗڂ̐ڐG�̖ʐς�Ԃ�
	*/
	double GetContactArea(int i);

	/** @brief i�Ԗڂ̐ڐG�̈ʒu��Ԃ�
	*/
	Vec3f GetContactPosition(int i);

	/** @brief i�Ԗڂ̐ڐG�̑��荄�̂�Ԃ�
	*/
	PHSolidIf* GetContactSolid(int i);
};
struct CRIKSolidDesc : CRSolidDesc {
	SPR_DESCDEF(CRSolid);
};

// ------------------------------------------------------------------------------
// �֐�

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

}

#endif//SPR_CRBODYPARTIF_H
