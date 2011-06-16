/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_CR_TOUCHSENSOR_IF_H
#define SPR_CR_TOUCHSENSOR_IF_H

#include <Creature/SprCREngine.h>

namespace Spr{;

struct PHSolidIf;

struct CRContactInfo {
	double		dimension;
	Vec3d		normal;
	double		pressure;
	Vec3d		pos;
	PHSolidIf*	soMe;
	PHSolidIf*	soOther;
	Vec3d		force;
};

struct CRTouchSensorIf : CREngineIf{
	SPR_IFDEF(CRTouchSensor);

	/** @brief ���݂̐ڐG�̌���Ԃ�
	*/
	int NContacts();

	/** @brief �ڐG����Ԃ�
	*/
	CRContactInfo GetContact(int n);
};

/// �G�o�Z���T�̃f�X�N���v�^
struct CRTouchSensorDesc : CREngineDesc{
	SPR_DESCDEF(CRTouchSensor);

	CRTouchSensorDesc(){
	}
};


}

#endif // SPR_CR_TOUCHSENSOR_IF_H
