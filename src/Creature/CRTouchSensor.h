/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRTOUCHSENSOR_H
#define CRTOUCHSENSOR_H

#include <Springhead.h>

#include <Foundation/Object.h>

#include <vector>

#include "CREngine.h"

//@{
namespace Spr{;
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief ���o����
*/
class CRTouchSensor : public CREngine, public CRTouchSensorDesc {
	std::vector<CRContactInfo> contactList;

public:
	SPR_OBJECTDEF(CRTouchSensor);
	ACCESS_DESC(CRTouchSensor);

	CRTouchSensor(){}
	CRTouchSensor(const CRTouchSensorDesc& desc) 
		: CRTouchSensorDesc(desc) 
	{
	}

	/** @brief ���s���������߂�v���C�I���e�B�l�D�������قǑ���
	*/
	virtual int GetPriority() const { return CREngineDesc::CREP_SENSOR; }

	/** @brief ���o���͂̏��������s����
	*/
	virtual void Step();

	/** @brief ���݂̐ڐG�̌���Ԃ�
	*/
	virtual int NContacts(){ return contactList.size(); }

	/** @brief �ڐG����Ԃ�
	*/
	virtual CRContactInfo GetContact(int n){ return contactList[n]; }
};
}
//@}

#endif//CRSENSOR_H
