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
struct CRContactInfo {
	double		dimension;
	Vec3d		normal;
	double		pressure;
	Vec3d		pos;
	PHSolidIf*	soMe;
	PHSolidIf*	soOther;
	Vec3d		force;
};

struct CRTouchSensorIf : CRSensorIf{
	SPR_IFDEF(CRTouchSensor);

	/** @brief ���݂̐ڐG�̌���Ԃ�
	*/
	int NContacts();

	/** @brief �ڐG����Ԃ�
	*/
	CRContactInfo GetContact(int n);
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
