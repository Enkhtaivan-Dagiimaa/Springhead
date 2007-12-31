/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_HIKEYMOUSE_H
#define SPR_HIKEYMOUSE_H

#include <HumanInterface/SprHIBase.h>
namespace Spr {;

struct DVKeyMouseHandler{
	/// ������Ă���L�[�C�{�^���̏�Ԃ������t���O
	enum ButtonState{
		NONE,
		LBUTTON = 0x01,
		RBUTTON = 0x02,
		SHIFT   = 0x04,
		CONTROL = 0x08,
		MBUTTON = 0x10,
	};
	/** �}�E�X���������Ƃ��̏���
		@param keyState ������Ă���L�[�C�{�^���̏��
	*/
	virtual bool OnMouseMove(int keyState, int x, int y, int zdelta){ return false; }
	///
	virtual bool OnClick(int keyState, int x, int y){ return false; }
	///
	virtual bool OnDoubleClick(int keyState, int x, int y){ return false; }
	///	�L�[����
	virtual bool OnKey(bool isDown, int vkey, int keyState, int x, int y){
		return false;
	}
};


///	DVKeyMouse
struct DVKeyMouseIf:public HIVirtualDeviceIf{
	///	�}�E�X�̈ʒu��Ԃ����߂̍\����
	struct DVMousePosition{
		int x;
		int y;
		unsigned time;
	};
	///	�L�[�̏��
	enum DVKeyState{
		NONE	= 0,
		PRESS	= 1,
		TOGGLE_ON = 2,
		BOTH	= PRESS|TOGGLE_ON,
	};
	IF_DEF(DVKeyMouse);
	///	�f�o�C�X�̖��O
	virtual const char* Name() const =0;
	///	�n���h���̐ݒ�
	virtual void SetHandler(DVKeyMouseHandler* h)=0;
	///	�}�E�X�{�^���E�L�[�{�[�h��Ԏ擾
	virtual int GetKeyState(int key) = 0;
	///	�}�E�X�ʒu�擾�֐�	0���ŐV�C1�ȏ�͉ߋ��̗������擾
	virtual DVKeyMouseIf::DVMousePosition GetMousePosition(int count=0)=0;
};

///	DRKeyMouseWin32
struct DRKeyMouseWin32If:public HIRealDeviceIf{
	IF_DEF(DRKeyMouseWin32);
};

}	//	namespace Spr
#endif
