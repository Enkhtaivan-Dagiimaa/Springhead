/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRENGINE_H
#define CRENGINE_H

#include <Foundation/Object.h>
#include <Creature/SprCREngine.h>
#include <Creature/SprCRController.h>
#include <Framework/SprFWObject.h>

#include <vector>

namespace Spr {;

///	�N���[�`���̃G���W���̊�{�N���X�D
class CREngine : public SceneObject {
	/// �L�����E������
	bool enabled;

public:
	SPR_OBJECTDEF(CREngine);

	CREngine() { enabled = true; }

	///	���s���������߂�v���C�I���e�B�l�D�������قǑ���
	virtual int GetPriority() const { return CREngineDesc::CREP_CONTROLLER; }

	///	����������
	virtual void Init(){}

	///	�������P�X�e�b�v���s����
	virtual void Step(){}

	/// �L���E������؂�ւ���
	virtual void Enable(bool enable) { enabled = enable; }

	/// �L���E��������Ԃ�
	virtual bool IsEnabled() { return enabled; }
};

inline bool operator < (const CREngine& e1, const CREngine& e2){
	return e1.GetPriority() < e2.GetPriority();
}

///	�N���[�`���̃R���g���[���̊�{�N���X�D
class CRController : public CREngine {
public:
	SPR_OBJECTDEF(CRController);

	CRController() {}

	///	��Ԃ����Z�b�g����
	virtual void Reset(){}

	///	�����Ԃ�
	virtual int GetStatus(){ return CRControllerDesc::CS_WAITING; }
};

}

#endif // CRENGINE_H
