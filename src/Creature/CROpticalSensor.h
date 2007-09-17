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
#include <set>

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

	/// ���Ȃ̍��̂̃Z�b�g�i���Ȃɑ����鍄�̂��ǂ����𔻒肷��̂Ɏg���j
	std::set<PHSolidIf*> selfSolids;

	/// ������O����̊�ՂƂȂ�֐�
	bool  IsInside(PHSolidIf* solid, double rangeIn, double rangeOut, double rangeVert);

	/// �����x�N�g���𐅕��E�����p�x�ɕϊ�
	Vec2d Vec3ToAngle(Vec3d v);

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

	/** @brief ������O������s��
	*/
	virtual bool IsVisible(PHSolidIf* solid);

	/** @brief ���S������O������s��
	*/
	virtual bool IsInCenter(PHSolidIf* solid);
};
}
//@}

#endif//CROPTICALSENSOR_H
