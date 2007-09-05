/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CROPTICALSENSOR_H
#define CROPTICALSENSOR_H

#include <Springhead.h>

#include <Foundation/Object.h>
#include "IfStubCreature.h"

#include <vector>
#include <algorithm>

#include "CRSensor.h"

//@{
namespace Spr{;
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief ���o�Z���T
*/
class CROpticalSensor : public CRSensor, public CROpticalSensorIfInit, public CROpticalSensorDesc {
private:
	/// ���p���鍄��
	PHSolidIf *soLEye, *soREye;

	/// �������ݐ�̓����V�[��
	CRInternalSceneIf* internalScene;

	/** @brief ��������ǂ���
	*/
	bool IsVisible(PHSolidIf* solid);

public:
	OBJECTDEF(CROpticalSensor, CRSensor);
	ACCESS_DESC(CROpticalSensor);

	CROpticalSensor(){}
	CROpticalSensor(const CROpticalSensorDesc& desc, CRCreatureIf* c=NULL) 
		: CROpticalSensorDesc(desc) 
		, CRSensor((const CRSensorDesc&)desc, c)
	{
	}

	/** @brief ����������
	*/
	virtual void Init();

	/** @brief �P�X�e�b�v���̏���
	*/
	virtual void Step();

};
}
//@}

#endif//CROPTICALSENSOR_H
