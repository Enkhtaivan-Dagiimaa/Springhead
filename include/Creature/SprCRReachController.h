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
	void SetFinalPos(Vec3d pos);

	/** @brief �ŏI���B�ڕW���x���Z�b�g����i�f�t�H���g�� (0,0,0)�j
	*/
	void SetFinalVel(Vec3d vel);
	
	/** @brief ���ϓ��B���x���Z�b�g����i��������ڕW���B���Ԃ��v�Z�����j
	*/
	void SetAverageSpeed(double speed);
	double GetAverageSpeed();

	/** @brief ���e�ʒu�덷���Z�b�g����i����ȏ�̌덷���������ēx���킷��j
	*/
	void SetAcceptablePosError(double err);

	/** @brief �ŏI���B�ڕW�ʒu�����݂̖ڕW���B�ʒu���炱��ȏ㉓������Ɠ��B�^���������I�ɍĎn������
	*/
	void SetRestartDistance(double dist);

	/** @brief ���B�\�莞���o�ߌ�̑҂����Ԃ��Z�b�g����
	*/
	void SetReachTimeMargin(double margin);


	// ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
	// �჌�x��API

	/** @brief ���B�^�����J�n����
	*/
	void Start(float reachTime);

	/** @brief �o�R�n�_�ʉߎ������Z�b�g����i���̏ꍇ�A�o�R�n�_��p���Ȃ��j
	*/
	void SetViaTime(float time);


	// �i���j�ʒu����E�p�����䂻�ꂼ��̗L���E������IKEndEffector�ŃZ�b�g���邱�ƁB

	/** @brief ���B�ڕW�ʒu���Z�b�g����
	*/
	void SetTargetPos(Vec3d pos);

	/** @brief ���B�ڕW���x���Z�b�g����i�f�t�H���g�� (0,0,0)�j
	*/
	void SetTargetVel(Vec3d vel);

	/** @brief �o�R�n�_���Z�b�g����
	*/
	void SetViaPos(Vec3d pos);


	/** @brief ���B�ڕW�p�����Z�b�g����
	*/
	void SetTargetOri(Quaterniond ori);

	/** @brief ���B�ڕW�p���x���Z�b�g����i�f�t�H���g�� (0,0,0)�j
	*/
	void SetTargetAVel(Vec3d avel);

	/** @brief �o�R�p�����Z�b�g����
	*/
	void SetViaOri(Quaterniond ori);


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
	
	/// �ڕW���B�����i�^���J�n����0�Ƃ���j
	float reachTime;

	/// �ڕW�o�R�_�ʉߎ����i�o�R�_���g��Ȃ��ꍇ�͕��̒l�Ƃ���j
	float viaTime;

	/// �^���J�n���̈ʒu�E���x�E�p���E�p���x
	Vec3d initPos, initVel, initAVel;
	Quaterniond initOri;

	/// �o�R�_�̖ڕW�ʒu�E�p��
	Vec3d viaPos;
	Quaterniond viaOri;

	/// ���ݓ��B�ڕW�Ƃ��Ă���ʒu�E���x�E�p���E�p���x
	Vec3d targPos, targVel, targAVel;
	Quaterniond targOri;

	/// ���݂̈ʒu�E���x�E�p���E�p���x
	Vec3d currPos, currVel, currAVel;
	Quaterniond currOri;

	/// �ŏI�I�Ȃ̖ڕW�ʒu�E���x�E�p���E�p���x
	Vec3d finalPos, finalVel, finalAVel;
	Quaterniond finalOri;

	CRReachControllerState(){
		time = 0; reachTime = -1; viaTime  = -1;
		initPos  = Vec3d(); initVel  = Vec3d(); initOri  = Quaterniond(); initAVel  = Vec3d();
		targPos  = Vec3d(); targVel  = Vec3d(); targOri  = Quaterniond(); targAVel  = Vec3d();
		currPos  = Vec3d(); currVel  = Vec3d(); currOri  = Quaterniond(); currAVel  = Vec3d();
		finalPos = Vec3d(); finalVel = Vec3d(); finalOri = Quaterniond(); finalAVel = Vec3d();
		viaPos   = Vec3d(); viaOri   = Quaterniond();
	}
};

/// �O���^���R���g���[���̃f�X�N���v�^
struct CRReachControllerDesc : public CRControllerDesc, public CRReachControllerState {
	SPR_DESCDEF(CRReachController);

	// ���ϓ��B���x
	double averageSpeed;

	// ���e�ʒu�덷
	double acceptablePosError;

	// ���B�^�������Ȃ������x
	double restartDistance;

	// ���B�ڕW�����o�ߌ�̑҂�����
	double reachTimeMargin;

	CRReachControllerDesc() {
		averageSpeed       = 0.2;
		acceptablePosError = 0.05;
		restartDistance    = 0.5;
		reachTimeMargin    = 2.0;
	}
};

//@}

}

#endif//SPR_CRREACHCONTROLLERIF_H
