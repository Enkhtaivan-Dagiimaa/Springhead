/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRESEWALKINGCONTROLLER_H
#define CRESEWALKINGCONTROLLER_H

#include <Springhead.h>

#include <Foundation/Object.h>
#include "IfStubCreature.h"

#include "CRController.h"

//@{
namespace Spr{;
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief �U���s�R���g���[��
*/
class CREseWalkingController : public CRController, public CREseWalkingControllerIfInit, public CREseWalkingControllerDesc {
private:
	/// ����ΏۂƂȂ�{�f�B�̍��̍���
	PHSolidIf* soWaist;

	/// �{�f�B���Œ肷�邽�߂̕s�����̂Ɗ֐�
	PHSolidIf* soFixpoint;
	PHJointIf* joFixpoint;

	/// ���s�X�s�[�h
	float speed;

	/// �]��p�x
	float rotation;

public:
	OBJECTDEF(CREseWalkingController, CRController);
	ACCESS_DESC(CREseWalkingController);

	CREseWalkingController(){
		speed    = 0.0f;
		rotation = 0.0f;
	}
	CREseWalkingController(const CREseWalkingControllerDesc& desc, CRCreatureIf* c=NULL) 
		: CREseWalkingControllerDesc(desc) 
		, CRController((const CRControllerDesc&)desc, c)
	{
		speed    = 0.0f;
		rotation = 0.0f;
	}

	/** @brief ���������s��
	*/
	virtual void Init();

	/** @brief ��������s����
	*/
	virtual void Step();

	/** @brief ���s�̑��x��ݒ肷��
	*/
	virtual void SetSpeed(float speed);

	/** @brief �]��p�x��ݒ肷��
	*/
	virtual void SetRotationAngle(float rot);

	/** @brief �ʒu��ݒ肷��i������Ԍ���̂��߁j
	*/
	virtual void SetPos(Vec3f pos);
};
}
//@}

#endif//CRESEWALKINGCONTROLLER_H
