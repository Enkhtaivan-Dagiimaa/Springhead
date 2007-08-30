/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_CRSensorIf_H
#define SPR_CRSensorIf_H

#include <SprFoundation.h>
#include <SprPhysics.h>

namespace Spr{;

//@{

struct CRAttentionListIf;
struct PHSolidIf;

// ------------------------------------------------------------------------------
/// ���o�Z���T�̃f�X�N���v�^
struct CROpticalSensorDesc{
	/// ���͑Ώۂ̒��Ӄ��X�g
	CRAttentionListIf* attentionList;

	/// �ڂ�Solid
	PHSolidIf *soLEye, *soREye;

	CROpticalSensorDesc(){
		attentionList = NULL;
	}
};

/// ���o�Z���T�̃C���^�t�F�[�X
struct CROpticalSensorIf : SceneObjectIf{
	IF_DEF(CROpticalSensor);

	/** @brief ���o���͂̏��������s����
	*/
	virtual void Step()= 0;
};

//@}

}

#endif//SPR_CRReachingMovementIf_H
