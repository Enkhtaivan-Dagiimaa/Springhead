/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_CRREACHCONTROLLERIF_H
#define SPR_CRREACHCONTROLLERIF_H

#include <Foundation/SprObject.h>
#include <Creature/SprCRController.h>

namespace Spr{;

///	�O���^���R���g���[��
struct CRReachControllerIf : public CRControllerIf{
	SPR_IFDEF(CRReachController);

	// ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
	// �����x��API

	/** @brief �ŏI���B�ڕW�ʒu���Z�b�g����
	*/
	void SetTargetPos(Vec3d pos);

	/** @brief ���ϓ��B���x���Z�b�g����i��������ڕW���B���Ԃ��v�Z�����j
	*/
	void SetAverageSpeed(double speed);
	double GetAverageSpeed();

	/** @brief Wait�X�e�[�g����Traj�X�e�[�g�ֈڍs����ۂ̍Œ�҂����Ԃ��Z�b�g����
	*/
	void SetMinWait(double t);

	/** @brief �}�[�W�����Z�b�g����
	*/
	void SetPosInnerMargin(double margin);
	void SetPosOuterMargin(double margin);
	void SetDirInnerMargin(double margin);
	void SetDirOuterMargin(double margin);

	/** @brief �O���̒ʉߓ_��Ԃ� s=0.0�`1.0
	*/
	Vec3d GetTrajectory(float s);

	/** @brief �ڕW���B���Ԃ�Ԃ�
	*/
	float GetReachTime();

	/** @brief ���ݎ�����Ԃ�
	*/
	float GetCurrentTime();

	/** @brief ���B�Ɏg���G���h�G�t�F�N�^��ݒ�E�擾����
	*/
	void SetIKEndEffector(PHIKEndEffectorIf* ikEff);
	PHIKEndEffectorIf* GetIKEndEffector();
};

//@{
///	�O���^���R���g���[����State
struct CRReachControllerState{
	/// �O���^���J�n����̌o�ߎ���
	float time;
	
	/// ���B�\�莞���i�^���J�n����0�Ƃ���j
	float reachTime;

	/// �o�R�_�ʉߗ\�莞���i�o�R�_���g��Ȃ��ꍇ�͕��̒l�Ƃ���j
	float viaTime;

	/// �O���n�_�E�I�_�̈ʒu�E���x�E�p��
	Vec3d pos0, vel0, pos1, vel1, pos1AtStartTime;
	Quaterniond ori0, ori1;

	/// �o�R�_�̖ڕW�ʒu
	Vec3d viaPos;

	/// ���݂̈ʒu�E���x
	Vec3d currPos, currVel;

	/// �ڕW�ʒu
	Vec3d targetPos, lastTargetPos, tempTargetPos, lastTempTargetPos, tempTargetPosAtStartTime;

	// �}�[�W��
	double posInnerMargin, posOuterMargin, dirInnerMargin, dirOuterMargin;

	// �}�[�W�����O���
	enum CRReachInOut { CRREACH_IN, CRREACH_OUT };
	CRReachInOut posInOut, dirInOut;

	// ������
	enum CRReachStat { CRREACH_TRAJ, CRREACH_FOLLOW, CRREACH_WAIT };
	CRReachStat posStat, dirStat;

	// WAIT��Ԃɓ���������
	double posWaitStarted, dirWaitStarted;

	CRReachControllerState(){
		time = 0; reachTime = -1; viaTime = -1;

		pos0  = Vec3d(); vel0  = Vec3d(); ori0  = Quaterniond();
		pos1  = Vec3d(); vel1  = Vec3d(); ori1  = Quaterniond(); pos1AtStartTime = Vec3d();

		currPos = Vec3d(); currVel = Vec3d();
		targetPos = Vec3d(); lastTargetPos = Vec3d(); tempTargetPos = Vec3d(); lastTempTargetPos = Vec3d(); tempTargetPosAtStartTime = Vec3d();
		viaPos = Vec3d();

		posInnerMargin = 0.0;    dirInnerMargin = Rad(0);
		posOuterMargin = 0.1;    dirOuterMargin = Rad(10);
		posInOut = CRREACH_OUT;  dirInOut = CRREACH_OUT;
		posStat  = CRREACH_WAIT; dirStat  = CRREACH_WAIT;
		posWaitStarted = 0;      dirWaitStarted = 0;
	}
};

/// �O���^���R���g���[���̃f�X�N���v�^
struct CRReachControllerDesc : public CRControllerDesc, public CRReachControllerState {
	SPR_DESCDEF(CRReachController);

	// ���ϓ��B���x
	double averageSpeed;

	// Wait->Traj�Œ�҂�����
	double minWait;

	CRReachControllerDesc() {
		averageSpeed       = 0.2;
		minWait            = 0.0;
	}
};

//@}

}

#endif//SPR_CRREACHCONTROLLERIF_H
