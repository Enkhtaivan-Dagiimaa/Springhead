/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRGRABCONTROLLER_H
#define CRGRABCONTROLLER_H

#include <Springhead.h>

#include <Foundation/Object.h>
#include "IfStubCreature.h"

#include "CRController.h"

//@{
namespace Spr{;
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief �c���R���g���[��
*/
class CRGrabController : public CRController, public CRGrabControllerIfInit, public CRGrabControllerDesc {
private:
	/// ���ʂ̃R���g���[���FReachingController
	CRReachingControllerIf *reachLeft, *reachRight;

	/// �c���Ώۂ�Solid
	PHSolidIf* grabSolid;

	/// �c���Ώۂ̑傫���i���a�j
	float radius;

	/// �c���������̂��ړ����鎞�̖ڕW�ʒu
	Vec3f movetoPos;

	/// ���݂̔c�����
	CRGrabControllerIf::GrabState grabState;

	/// �c��������Ɉړ����s�����ǂ���
	bool bMoveto;

public:
	OBJECTDEF(CRGrabController, CRController);
	ACCESS_DESC(CRGrabController);

	CRGrabController(){}
	CRGrabController(const CRGrabControllerDesc& desc, CRCreatureIf* c=NULL) 
		: CRGrabControllerDesc(desc) 
		, CRController((const CRControllerDesc&)desc, c)
	{
	}

	/** @brief ������
	*/
	virtual void Init();

	/** @brief �P�X�e�b�v
	*/
	virtual void Step();

	/** @brief �͂�
	*/
	virtual void Grab(PHSolidIf* solid, float radius);

	/** @brief ���݉����͂�ł���΁C����
	*/
	virtual void Ungrab();

	/** @brief ���݉����͂�ł���΁C����������Ĉړ�����
	*/
	virtual void MoveTo(Vec3f pos);

	/** @brief ���݂̔c���̏�Ԃ�Ԃ�
	*/
	virtual CRGrabControllerIf::GrabState GetGrabState();
};
}
//@}

#endif//CRGRABCONTROLLER_H
