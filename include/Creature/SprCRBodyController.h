/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_CR_BODY_CONTROLLER_IF_H
#define SPR_CR_BODY_CONTROLLER_IF_H

#include <Creature/SprCREngine.h>

namespace Spr{;

// ------------------------------------------------------------------------------

struct PHSolidIf;
struct CRIKSolidIf;

/// �S�g�^���R���g���[���̃C���^�[�t�F�[�X
struct CRBodyControllerIf : CREngineIf{
	SPR_IFDEF(CRBodyController);

	/** @brief �ڕW�ʒu�̐ݒ�
		@param effector ���삳����p�[�c�̃��x��
		@param pos �ڕW�ʒu
	*/
	void SetTargetPos(UTString effector, Vec3d pos);

	/** @brief �ڕW�p���̐ݒ�
		@param effector ���삳����p�[�c�̃��x��
		@param ori �ڕW�p��
	*/
	void SetTargetOri(UTString effector, Quaterniond ori);

	/** @brief �ڕW�ʒu�E�p���̐ݒ�
		@param effector ���삳����p�[�c�̃��x��
		@param pose �ڕW�ʒu�E�p��
	*/
	void SetTargetPose(UTString effector, Posed pose);

	/** @brief �ڕW���B�����̐ݒ�
		@param effector ���삳����p�[�c�̃��x��
		@param timeLimit ���B�܂ł̖ڕW�o�ߎ���
	*/
	void SetTimeLimit(UTString effector, float timeLimit);

	/** @brief �O�����v�悷��
	*/
	void Plan();

	/** @brief �^���J�n
		@param effector ���삳����p�[�c�̃��x��
	*/
	void Restart(UTString effector);
};

/// �S�g�^���R���g���[���̃f�X�N���v�^
struct CRBodyControllerDesc : public CREngineDesc{
	SPR_DESCDEF(CRBodyController);
};

}

#endif // SPR_CR_BODY_CONTROLLER_IF_H
