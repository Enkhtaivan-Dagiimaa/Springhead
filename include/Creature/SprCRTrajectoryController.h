/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_CR_TRAJECTORY_CONTROLLER_IF_H
#define SPR_CR_TRAJECTORY_CONTROLLER_IF_H

namespace Spr{;

// ------------------------------------------------------------------------------

struct CRSolidIf;

/// �X�̎��̋O��
struct CRTrajectoryIf : CREngineIf {
	SPR_IFDEF(CRTrajectory);

	/** @brief ���݂̈ʒu�̎擾
	*/
	Vec3f GetPosition();

	/** @brief ���݂̑��x�̎擾
	*/
	Vec3f GetVelocity();

	/** @brief ���݂̎p���̎擾
	*/
	Quaternionf GetOrientation();

	/** @brief ���݂̊p���x�̎擾
	*/
	Vec3f GetAngularVelocity();

	/** @brief �O���^���J�n
	*/
	void Start();

	/** @brief ������1�X�e�b�v�i�߂�
	*/
	void Step();

	/** @brief ���B�ڕW���Ԃ��Z�b�g
	*/
	void SetTimeLimit(double tL);

	/** @brief ���B�ڕW�ʒu���Z�b�g
	*/
	void SetTargetPosition(Vec3f pos);
	void EnablePositionControl(bool enable);

	/** @brief �퓞�B�ڕW�ʒu���Z�b�g
	*/
	void SetTargetPositionInSolid(Vec3f pos);

	/** @brief ���B�ڕW���x���Z�b�g
	*/
	void SetTargetVelocity(Vec3f vel);
	void EnableVelocityControl(bool enable);

	/** @brief ���B�ڕW�p�����Z�b�g
	*/
	void SetTargetOrientation(Vec3f ori);
	void EnableOrientationControl(bool enable);

	/** @brief ���B�ڕW�p���x���Z�b�g
	*/
	void SetTargetAngularVelocity(Vec3f angvel);
	void EnableAngularVelocityControl(bool enable);

	/** @brief �L���E�����Z�b�g
	*/
	void Enable(bool enable);
};
/// �X�̎��̋O���̃X�e�[�g
struct CRTrajectoryState {
	bool		posEnabled;
	bool		velEnabled;
	bool		oriEnabled;
	bool		angvelEnabled;
	double		time;
	Vec3f		posStart;
	Vec3f		posEnd;
	Vec3f		velStart;
	Vec3f		velEnd;
	Quaternionf	oriStart;
	Quaternionf	oriEnd;
	Vec3f		angvelStart;
	Vec3f		angvelEnd;
	Vec3f		currentPosition;
	Quaternionf	currentOrientation;

	CRTrajectoryState() {
		time = 0;
	}
};
/// �X�̎��̋O���̃f�X�N���v�^
struct CRTrajectoryDesc : public CRTrajectoryState, public CREngineDesc {
	SPR_DESCDEF(CRTrajectory);

	Vec3f		posInSolid;
	Vec3f		posInWorld;
	double		timeLimit;
	bool		bEnabled;

	CRTrajectoryDesc() {
		timeLimit = 0;
		bEnabled = false;
	}
};


/// �O���^���^���R���g���[���̃C���^�[�t�F�[�X
struct CRTrajectoryControllerIf : CREngineIf{
	SPR_IFDEF(CRTrajectoryController);
};

/// �O���^���R���g���[���̃f�X�N���v�^
struct CRTrajectoryControllerDesc : public CREngineDesc{
	SPR_DESCDEF(CRTrajectoryController);
};


}

#endif // SPR_CR_TRAJECTORY_CONTROLLER_IF_H
