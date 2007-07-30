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

//@{
namespace Spr{;
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief ��i�����j�^���R���g���[��
*/
class CRNeckController : public SceneObject, public CRNeckControllerIfInit, public CRNeckControllerDesc {
private:
	/** @brief �����_
	*/
	Vec3d pos, vel;

public:
	OBJECTDEF(CRNeckController, SceneObject);
	ACCESS_DESC_STATE(CRNeckController);

	CRNeckController(){}
	CRNeckController(const CRNeckControllerDesc& desc, SceneIf* s=NULL) : CRNeckControllerDesc(desc) {
		if(s){SetScene(s);}
	}

	/** @brief �����_��ݒ肷��
		@param pos �����_�̂R�������W
		@param vel �����_�̈ړ����x�x�N�g��
	*/
	virtual void LookAt(Vec3f pos, Vec3f vel);

	/** @brief ���䏈�������s����
	*/
	virtual void Step();
};
}
//@}

#endif//CREYECONTROLLER_H