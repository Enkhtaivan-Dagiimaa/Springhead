/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRENGINE_H
#define CRENGINE_H

#include <SprFoundation.h>
#include <Foundation/Object.h>

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
	///	�������P�X�e�b�v���s����
	virtual void Step(){}

	/// �L���E������؂�ւ���
	virtual void SetEnable(bool enable) { enabled = enable; }

	/// �L���E��������Ԃ�
	virtual bool IsEnabled() { return enabled; }

	/// �f�o�b�O����`�悷��
	virtual void Render(GRRenderIf* render){}
};

inline bool operator < (const CREngine& e1, const CREngine& e2){
	return e1.GetPriority() < e2.GetPriority();
}

}

#endif // CRENGINE_H
