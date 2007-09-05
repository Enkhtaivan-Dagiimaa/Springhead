/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRWALKINGCONTROLLER_H
#define CRWALKINGCONTROLLER_H

#include <Springhead.h>

#include <Foundation/Object.h>
#include "IfStubCreature.h"

#include "CRController.h"

//@{
namespace Spr{;
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief ���ӃR���g���[��
*/
class CRWalkingController : public CRController, public CRWalkingControllerIfInit, public CRWalkingControllerDesc {
private:
	/// �ˑ����鑼�̃I�u�W�F�N�g�̗�D�K�v�Ȃ��̂�I��ŋL�q�D
	/*
	PHSceneIf*          phScene;
	CRHingeHumanBodyIf* body;
	CRInternalSceneIf*  internalScene;
	CRGazeControllerIf* gazeController;
	CROpticalSensorIf*  opticalSensor;
	*/

	/// �ق���Private�ȕϐ��⃁�\�b�h���K�v�Ȃ炱���ɒ�`��ǉ��D

public:
	OBJECTDEF(CRWalkingController, CRController);
	ACCESS_DESC(CRWalkingController);

	CRWalkingController(){
	}
	CRWalkingController(const CRWalkingControllerDesc& desc, CRCreatureIf* c=NULL) 
		: CRWalkingControllerDesc(desc) 
		, CRController((const CRControllerDesc&)desc, c)
	{
	}

	/** @brief ���������s��
	*/
	virtual void Init();

	/** @brief ��������s����
	*/
	virtual void Step();

	/// ��L�ȊO��CRWalkingControllerIf�ɒǉ�����API�֐��̒�`�������ɒǉ��D
};
}
//@}

#endif//CRWALKINGCONTROLLER_H
