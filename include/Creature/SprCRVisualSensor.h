/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_CR_VISUALSENSOR_IF_H
#define SPR_CR_VISUALSENSOR_IF_H

#include <Creature/SprCREngine.h>

namespace Spr{;

struct PHSolidIf;

struct CRVisualSensorIf : CREngineIf{
	SPR_IFDEF(CRVisualSensor);

	/** @brief ������O������s��
	*/
	bool IsVisible(PHSolidIf* solid);
	bool IsVisible(Vec3f pos);

	/** @brief ���S������O������s��
	*/
	bool IsInCenter(PHSolidIf* solid);
	bool IsInCenter(Vec3f pos);

	/** @brief �������g�̍��̂��ǂ����𔻒肷��
	*/
	bool IsSelfSolid(PHSolidIf* solid);
};

/// ���o�Z���T�̃f�X�N���v�^
struct CRVisualSensorDesc : CREngineDesc{
	SPR_DESCDEF(CRVisualSensor);

	CRVisualSensorDesc(){
	}
};


}

#endif // SPR_CR_VISUALSENSOR_IF_H
