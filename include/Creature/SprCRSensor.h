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

namespace Spr{;

//@{

struct CRCreatureIf;

struct CRSensorIf;
struct CROpticalSensorIf;

// ------------------------------------------------------------------------------
/// �Z���T�̃C���^�t�F�[�X
struct CRSensorIf : SceneObjectIf{
	IF_DEF(CRSensor);

	/** @brief ���������s��
	*/
	virtual void Init()= 0;

	/** @brief ���o���͂̏��������s����
	*/
	virtual void Step()= 0;
};

/// �Z���T�̃f�X�N���v�^
struct CRSensorDesc{
	DESC_DEF_FOR_OBJECT(CRSensor);

	CRSensorDesc(){
	}
};

// ------------------------------------------------------------------------------
/// ���o�Z���T�̃C���^�t�F�[�X
struct CROpticalSensorIf : CRSensorIf{
	IF_DEF(CROpticalSensor);

};

/// ���o�Z���T�̃f�X�N���v�^
struct CROpticalSensorDesc : CRSensorDesc{
	DESC_DEF_FOR_OBJECT(CROpticalSensor);

	CROpticalSensorDesc(){
	}
};

//@}

}

#endif//SPR_CRSensorIf_H
