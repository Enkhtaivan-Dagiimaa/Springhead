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

#include "CRSensor.h"

//@{
namespace Spr{;
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief ���o����
*/
class CRTouchSensor : public CRSensor, public CRTouchSensorDesc {
	struct Contact {
		double		dimension;
		Vec3d		normal;
		double		pressure;
		Vec3d		pos;
		PHSolidIf*	soMe;
		PHSolidIf*	soOther;
	};

	std::vector<Contact> contactList;

public:
	SPR_OBJECTDEF(CRTouchSensor);
	ACCESS_DESC(CRTouchSensor);

	CRTouchSensor(){}
	CRTouchSensor(const CRTouchSensorDesc& desc, CRCreatureIf* c=NULL) 
		: CRTouchSensorDesc(desc) 
		, CRSensor((const CRSensorDesc&)desc, c)
	{
	}

	/** @brief ���������s��
	*/
	virtual void Init();

	/** @brief ���o���͂̏��������s����
	*/
	virtual void Step();

	/** @brief ���݂̐ڐG�̌���Ԃ�
	*/
	virtual int NContacts(){ return contactList.size(); }

	/** @brief �ڐG���Ă��鍄�́i�����́j��Ԃ�
	*/
	virtual PHSolidIf* GetContactSolidMe(int n){ return contactList[n].soMe; }

	/** @brief �ڐG���Ă��鍄�́i����́j��Ԃ�
	*/
	virtual PHSolidIf* GetContactSolidOther(int n){ return contactList[n].soOther; }

	/** @brief �ڐG���͂̑傫����Ԃ�
	*/
	virtual double GetContactPressure(int n){ return contactList[n].pressure; }

	/** @brief �ڐG�n�_�i�O���[�o�����W�j��Ԃ�
	*/
	virtual Vec3d GetContactPos(int n){ return contactList[n].pos; }

	/** @brief �ڐG���Ă��鍄�́i�����́j��Ԃ�
	*/
	virtual PHSolidIf* GetContactMySolid(int n){ return contactList[n].soMe; }

	/** @brief �ڐG���Ă��鍄�́i����́j��Ԃ�
	*/
	virtual PHSolidIf* GetContactOthersSolid(int n){ return contactList[n].soOther; }
};
}
//@}

#endif//CRSENSOR_H
