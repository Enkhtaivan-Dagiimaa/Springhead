/*
 *  Copyright (c) 2003-2007, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRTRYTOSTANDUPCONTROLLER_H
#define CRTRYTOSTANDUPCONTROLLER_H

#include <Springhead.h>

#include <Foundation/Object.h>
#include "IfStubCreature.h"

#include "CRController.h"

//@{
namespace Spr{;
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief �|�ꂽ�痧�Ƃ��Ɠw�͂���R���g���[��
*/
class CRTryToStandUpController : public CRController, public CRTryToStandUpControllerIfInit, public CRTryToStandUpControllerDesc {
	
public:
	// DCAST���ł���悤�ɂ��邽�߂̎d�g�݂������Ă���B
	// ����������Ȃ���LNK2019 Error�ɂȂ�B
	OBJECTDEF(CRTryToStandUpController, CRController);

	// DESC�̏��������瑤�Ƀ��[�h���邽�߂̎d�g�݂������Ă���B
	// ����������Ȃ���LNK2019 Error�ɂȂ�B
	// ��`����N���X�̃R���X�g���N�^�ƍ��킹�ď����K�v������B
	// �܂��A�����̂������ȃR���X�g���N�^���K�v�ł���B
	ACCESS_DESC(CRTryToStandUpController);


	// �R���X�g���N�^
	CRTryToStandUpController(){
	}
	CRTryToStandUpController(const CRTryToStandUpControllerDesc& desc, CRCreatureIf* c=NULL) 
		: CRTryToStandUpControllerDesc(desc) 
		, CRController((const CRControllerDesc&)desc, c)
	{
	}

};
}
//@}

#endif//CRTRYTOSTANDUPCONTROLLER_H