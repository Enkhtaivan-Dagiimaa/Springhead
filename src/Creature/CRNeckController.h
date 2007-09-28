/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRNECKCONTROLLER_H
#define CRNECKCONTROLLER_H

#include <Springhead.h>

#include <Foundation/Object.h>
#include "IfStubCreature.h"

#include "CRController.h"

//@{
namespace Spr{;
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief ��i�����j�^���R���g���[��
*/
class CRNeckController : public CRController, public CRNeckControllerIfInit, public CRNeckControllerDesc {
private:
	/// �����_
	Vec3d pos, vel;

	/// ���ӂ̓x��
	float attractiveness;

	/// ����ɂ�������
	PHSolidIf *soHead, *soNeck, *soChest, *soAbdomen;

	/// ����ɂ����֐�
	PHHingeJointIf *joNeckHeadX, *joChestNeckY, *joAbdomenChest;

	/// ������i�����𒆐S�Ɋ����ړ�����Cattractiveness�����l�𒴂���Ƃ��̕����֕ύX�����j
	double origX, origZ;

public:
	OBJECTDEF(CRNeckController, CRController);
	ACCESS_DESC_STATE(CRNeckController);

	CRNeckController(){}
	CRNeckController(const CRNeckControllerDesc& desc, CRCreatureIf* c=NULL)
		: CRNeckControllerDesc(desc)
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

#endif//CRNECKCONTROLLER_H
