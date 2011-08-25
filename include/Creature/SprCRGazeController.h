/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_CR_GAZECONTROLLER_IF_H
#define SPR_CR_GAZECONTROLLER_IF_H

namespace Spr{;

// ------------------------------------------------------------------------------

/// �ዅ�^���R���g���[���̃C���^�[�t�F�C�X
struct CREyeControllerIf : CREngineIf{
	SPR_IFDEF(CREyeController);

	/** @brief �����_��ݒ肷��
		@param pos �����_�̂R�������W
		@param vel �����_�̈ړ����x�x�N�g��
	*/
	void LookAt(Vec3f pos, Vec3f vel);

	/** @brief �T�b�P�[�h�����ǂ�����Ԃ�
	*/
	bool IsSaccading();
};

/// �ዅ�^���R���g���[���̃X�e�[�g
struct CREyeControllerState{
	/// ����̏��
	enum ControlState{
		CS_SACCADE=0, CS_PURSUIT,
	} controlState; 
};

/// �ዅ�^���R���g���[���̃f�X�N���v�^
struct CREyeControllerDesc: public CREyeControllerState, public CREngineDesc{
	SPR_DESCDEF(CREyeController);

	/// �������̂��w�肷�郉�x��
	std::string labelHeadSolid;
	/// �ዅ���̂��w�肷�郉�x��
	std::string labelLEyeSolid;
	std::string labelREyeSolid;
	/// �ዅ�֐߂��w�肷�郉�x��
	std::string labelLEyeJoint;
	std::string labelREyeJoint;

	CREyeControllerDesc(){
		labelHeadSolid = "head";
		labelLEyeSolid = "left_eye";
		labelREyeSolid = "right_eye";
		labelLEyeJoint = "left_eye_joint";
		labelREyeJoint = "right_eye_joint";
	}
};

// ------------------------------------------------------------------------------

/// ��^���R���g���[���̃C���^�[�t�F�C�X
struct CRNeckControllerIf : CREngineIf{
	SPR_IFDEF(CRNeckController);

	/** @brief �����_��ݒ肷��
		@param pos   �����_�̂R�������W
		@param vel   �����_�̈ړ����x�x�N�g��
		@param ratio �^�������i1:���S�ɑΏۂ̕������� 0:�S�������Ȃ��j
	*/
	void LookAt(Vec3f pos, Vec3f vel, float ratio);
};

/// ��^���R���g���[���̃X�e�[�g
struct CRNeckControllerState{
};

/// ��^���R���g���[���̃f�X�N���v�^
struct CRNeckControllerDesc: public CRNeckControllerState, public CREngineDesc{
	SPR_DESCDEF(CRNeckController);

	/// �������̂��w�肷�郉�x��
	std::string labelHeadSolid;

	CRNeckControllerDesc(){
		labelHeadSolid = "head";
	}
};

// ------------------------------------------------------------------------------

/// �����R���g���[���i�ዅ�^���E��^���̃R���g���[���̏�ʋ@�\�j�̃C���^�[�t�F�C�X
struct CRGazeControllerIf : CREngineIf{
	SPR_IFDEF(CRGazeController);

	/** @brief �����_��ݒ肷��
		@param pos �����_�̂R�������W
		@param vel �����_�̈ړ����x�x�N�g��
		@param attractiveness ���ӂ������x����
	*/
	void LookAt(Vec3f pos, Vec3f vel, float attractiveness);
};

/// �����R���g���[���̃X�e�[�g
struct CRGazeControllerState{
};

/// �����R���g���[���̃f�X�N���v�^
struct CRGazeControllerDesc: public CRGazeControllerState, public CREngineDesc{
	SPR_DESCDEF(CRGazeController);

	CRGazeControllerDesc(){
	}
};

}

#endif // SPR_CR_GAZECONTROLLER_IF_H
