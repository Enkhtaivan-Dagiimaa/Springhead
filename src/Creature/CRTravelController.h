/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRTRAVELCONTROLLER_H
#define CRTRAVELCONTROLLER_H

#include <Springhead.h>

#include <Foundation/Object.h>
#include "IfStubCreature.h"

#include "CRController.h"

//@{
namespace Spr{;
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief �ړ��R���g���[���i�ڕW�n�_�ւ̈ړ��C��Q������j
*/
class CRTravelController : public CRController, public CRTravelControllerIfInit, public CRTravelControllerDesc {
private:
	/// ���ʂ̃R���g���[���F���s�R���g���[��
	// CREseWalkingControllerIf* walkCtrl;
	CRWalkingControllerIf* walkCtrl;

	/// ��񌹁F�����V�[��
	CRInternalSceneIf* internalScene;

	/// ���L�����̌��݈ʒu�𓾂邽�߂̍���
	PHSolidIf* soWaist;

	/// �ڕW�n�_�i���ʓ����W�j
	Vec3f goal;

	/// ���s�R���g���[���ɏo�͉\�ȍő�̃X�s�[�h
	float maxSpeed;

public:
	OBJECTDEF(CRTravelController, CRController);
	ACCESS_DESC(CRTravelController);

	CRTravelController(){
		goal = Vec3f(0,0,0);
	}
	CRTravelController(const CRTravelControllerDesc& desc, CRCreatureIf* c=NULL) 
		: CRTravelControllerDesc(desc) 
		, CRController((const CRControllerDesc&)desc, c)
	{
		walkCtrl = NULL;
		goal = Vec3f(0,0,0);
	}

	/** @brief ���������s��
	*/
	virtual void Init();

	/** @brief ��������s����
	*/
	virtual void Step();

	/** @brief �ڕW�n�_��ݒ肷��
	*/
	virtual void SetGoal(Vec3f goal);

	virtual float GetMaxSpeed() { return maxSpeed; }
};
}
//@}

#endif//CRTRAVELCONTROLLER_H
