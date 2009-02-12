/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_CRSensorIf_H
#define SPR_CRSensorIf_H

#include <SprFoundation.h>

namespace Spr{;

//@{

struct CRCreatureIf;

struct CRSensorIf;
struct CROpticalSensorIf;

// ------------------------------------------------------------------------------
/// �Z���T�̃C���^�t�F�[�X
struct CRSensorIf : SceneObjectIf{
	SPR_IFDEF(CRSensor);

	/** @brief ���������s��
	*/
	void Init();

	/** @brief ���o���͂̏��������s����
	*/
	void Step();
};

/// �Z���T�̃f�X�N���v�^
struct CRSensorDesc{
	SPR_DESCDEF(CRSensor);

	CRSensorDesc(){
	}
};

// ------------------------------------------------------------------------------
/// ���o�Z���T�̃C���^�t�F�[�X
struct CROpticalSensorIf : CRSensorIf{
	SPR_IFDEF(CROpticalSensor);

	/** @brief ������O������s��
	*/
	bool IsVisible(PHSolidIf* solid);

	/** @brief ���S������O������s��
	*/
	bool IsInCenter(PHSolidIf* solid);

	/** @brief ������O������s��
	*/
	bool IsVisible(Vec3f pos);

	/** @brief ���S������O������s��
	*/
	bool IsInCenter(Vec3f pos);

	/** @brief �������g�̍��̂��ǂ����𔻒肷��
	*/
	bool IsSelfSolid(PHSolidIf* solid);
};

/// ���o�Z���T�̃f�X�N���v�^
struct CROpticalSensorDesc : CRSensorDesc{
	SPR_DESCDEF(CROpticalSensor);

	CROpticalSensorDesc(){
	}
};

// ------------------------------------------------------------------------------
/// �G�o�Z���T�̃C���^�t�F�[�X
struct CRTouchSensorIf : CRSensorIf{
	SPR_IFDEF(CRTouchSensor);

	/** @brief ���݂̐ڐG�̌���Ԃ�
	*/
	int NContacts();

	/** @brief �ڐG���Ă��鍄�́i�����́j��Ԃ�
	*/
	PHSolidIf* GetContactSolidMe(int n);

	/** @brief �ڐG���Ă��鍄�́i����́j��Ԃ�
	*/
	PHSolidIf* GetContactSolidOther(int n);

	/** @brief �ڐG���͂̑傫����Ԃ�
	*/
	double GetContactPressure(int n);

	/** @brief �ڐG�n�_�i�O���[�o�����W�j��Ԃ�
	*/
	Vec3d GetContactPos(int n);

	/** @brief �ڐG���Ă��鍄�́i�����́j��Ԃ�
	*/
	PHSolidIf* GetContactMySolid(int n);

	/** @brief �ڐG���Ă��鍄�́i����́j��Ԃ�
	*/
	PHSolidIf* GetContactOthersSolid(int n);

	/** @brief �ڐG�͂�Ԃ�
	*/
	Vec3f GetContactForce(int n);
};

/// �G�o�Z���T�̃f�X�N���v�^
struct CRTouchSensorDesc : CRSensorDesc{
	SPR_DESCDEF(CRTouchSensor);

	CRTouchSensorDesc(){
	}
};

//@}

}

#endif//SPR_CRSensorIf_H
