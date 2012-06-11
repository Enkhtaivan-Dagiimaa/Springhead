/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_CRENGINEIF_H
#define SPR_CRENGINEIF_H

#include <Foundation/SprObject.h>

namespace Spr{;

//@{

struct CRCreatureIf;

///	�N���[�`���E�G���W���̊�{�N���X
struct CREngineIf : public SceneObjectIf{
	SPR_IFDEF(CREngine);

	/** @brief ���s���������߂�v���C�I���e�B�l�D�������قǑ���
	 */
	int GetPriority() const;

	/** @brief ����������
	 */
	void Init();

	/** @brief �������P�X�e�b�v�����s����
	 */
	void Step();

	/** @breif �L���E������؂�ւ���
	*/
	void Enable(bool enable);

	/** @brief �L���E��������Ԃ�
	*/
	bool IsEnabled();
};

struct CREngineDesc{
	SPR_DESCDEF(CREngine);

	/// �N���[�`���̃��W���[���̕��ނƗD�揇��
	///	100�̔{���͕��ނ�\���A�ȉ��̈ʂ͂��̒��ł̎��s�������K�肷��
	enum CREnginePriority {
		// ���o�n
		CREP_SENSOR			= 100,
		// �s������
		CREP_DECISION		= 200,
		// �^������
		CREP_CONTROLLER		= 300,
	};

	CREngineDesc(){
	}
};

//@}

}

#endif//SPR_CRENGINEIF_H
