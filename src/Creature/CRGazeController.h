/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRGAZECONTROLLER_H
#define CRGAZECONTROLLER_H

#include <Springhead.h>

#include <Foundation/Object.h>
#include "IfStubCreature.h"

#include "CRController.h"

//@{
namespace Spr{;
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief �����^���R���g���[��
*/
class CRGazeController : public CRController, public CRGazeControllerIfInit, public CRGazeControllerDesc {
private:
	/// �����_
	Vec3d pos, vel;

	/// ���ӂ̓x��
	float attractiveness;

	/// ���ʂ̃R���g���[���F�ዅ�^��
	CREyeControllerIf* eyeCtrl;

	/// ���ʂ̃R���g���[���F��^��
	CRNeckControllerIf* neckCtrl;

public:
	OBJECTDEF(CRGazeController, CRController);
	ACCESS_DESC_STATE(CRGazeController);

	CRGazeController(){}
	CRGazeController(const CRGazeControllerDesc& desc, CRCreatureIf* c=NULL) 
		: CRGazeControllerDesc(desc) 
		, CRController((const CRControllerDesc&)desc, c)
	{
	}

	/** @brief �����������s����
	*/
	virtual void Init();

	/** @brief ���䏈�������s����
	*/
	virtual void Step();

	/** @brief �����_��ݒ肷��
		@param pos �����_�̂R�������W
		@param vel �����_�̈ړ����x�x�N�g��
	*/
	virtual void LookAt(Vec3f pos, Vec3f vel, float attractiveness);
};
}
//@}

#endif//CRGAZECONTROLLER_H
