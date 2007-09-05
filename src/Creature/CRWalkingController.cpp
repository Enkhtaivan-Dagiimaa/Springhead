/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include "CRWalkingController.h"

#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

namespace Spr{
IF_OBJECT_IMP(CRWalkingController, CRController);

void CRWalkingController::Init(){
	CRController::Init();

	/// �ˑ����鑼�̃I�u�W�F�N�g�̎擾���@�i�����K�v�Ȃ�j�D

	/// < Body >
	/*
	body = DCAST(CRHingeHumanBodyIf, creature->GetBody());
	*/

	/// < PHScene >
	/*
	phScene = DCAST(PHSceneIf, creature->GetScene());
	*/

	/// < InternalScene >
	/*
	internalScene = creature->GetInternalScene();
	*/

	/// < �ق���Controller > 
	/*
	for (int i=0; i<creature->NControllers(); i++) {
		if (!gazeCtrl) {
			gazeCtrl = DCAST(CRGazeControllerIf, creature->GetController(i))
		}
	}
	*/

	// < �ق���Sensor >
	/*
	for (int i=0; i<creature->NSensors(); i++) {
		if (!opticalSensor) {
			opticalSensor = DCAST(CROpticalSensorIf, creature->GetSensor(i))
		}
	}
	*/
}
	
void CRWalkingController::Step(){
	CRController::Step();

	/// ����̃X�e�b�v�����s����D
}

/// ��L�ȊO��CRWalkingControllerIf�ɒǉ�����API�֐��̎����������ɒǉ��D

}
