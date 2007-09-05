/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRCONTROLLER_H
#define CRCONTROLLER_H

#include <Springhead.h>

#include <Foundation/Object.h>
#include "IfStubCreature.h"

//@{
namespace Spr{;
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief �R���g���[��
*/
class CRController : public SceneObject, public CRControllerIfInit, public CRControllerDesc {
protected:
	/// ����Ώۂ̃N���[�`��
	CRCreatureIf* creature;

	/// ��������V�[��
	PHSceneIf* phScene;

public:
	OBJECTDEF(CRController, SceneObject);
	ACCESS_DESC(CRController);

	CRController(){}
	CRController(const CRControllerDesc& desc, CRCreatureIf* c=NULL) : CRControllerDesc(desc) {
		if(c!=NULL && c->GetScene()){SetScene(c->GetScene());}
		creature = c;
	}

	/** @brief ���������s��
	*/
	virtual void Init();

	/** @brief �^������̏��������s����
	*/
	virtual void Step();
};
}
//@}

#endif//CRCONTROLLER_H
