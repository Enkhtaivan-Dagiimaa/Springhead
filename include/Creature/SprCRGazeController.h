/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_CR_EYECONTROLLER_IF_H
#define SPR_CR_EYECONTROLLER_IF_H

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

	CREyeControllerDesc(){
	}
};

// ------------------------------------------------------------------------------

/// ��^���R���g���[���̃C���^�[�t�F�C�X
struct CRNeckControllerIf : CREngineIf{
	SPR_IFDEF(CRNeckController);

	/** @brief �����_��ݒ肷��
		@param pos �����_�̂R�������W
		@param vel �����_�̈ړ����x�x�N�g��
	*/
	void LookAt(Vec3f pos, Vec3f vel, float attractiveness);
};

/// ��^���R���g���[���̃X�e�[�g
struct CRNeckControllerState{
};

/// ��^���R���g���[���̃f�X�N���v�^
struct CRNeckControllerDesc: public CRNeckControllerState, public CREngineDesc{
	SPR_DESCDEF(CRNeckController);

	/// ��𓮂����͂��߂�Attractiveness��臒l
	float lowerAttractiveness;
	/// ������S�ɑΏۂɌ�����Attractiveness��臒l
	float upperAttractiveness;

	CRNeckControllerDesc(){
		lowerAttractiveness = -1.0;
		upperAttractiveness =  0.0;
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

// ------------------------------------------------------------------------------

/// ���ӃR���g���[���̃C���^�t�F�[�X
struct CRAttentionControllerIf : CREngineIf{
	SPR_IFDEF(CRAttentionController);

};

/// ���ӃR���g���[���̃f�X�N���v�^
struct CRAttentionControllerDesc : public CREngineDesc{
	SPR_DESCDEF(CRAttentionController);

	CRAttentionControllerDesc(){
	}
};

}

#endif // SPR_CR_EYECONTROLLER_IF_H
