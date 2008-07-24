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
	bool OnMouseMove(int keyState, int x, int y, int zdelta);
	///
	bool OnClick(int keyState, int x, int y);
	///
	bool OnDoubleClick(int keyState, int x, int y);
	///	�L�[����
	bool OnKey(bool isDown, int vkey, int keyState, int x, int y);
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
	SPR_IFDEF(DVKeyMouse);
	///	�f�o�C�X�̖��O
	const char* Name() const ;
	///	�n���h���̐ݒ�
	void SetHandler(DVKeyMouseHandler* h);
	///	�}�E�X�{�^���E�L�[�{�[�h��Ԏ擾
	int GetKeyState(int key);
	///	�}�E�X�ʒu�擾�֐�	0���ŐV�C1�ȏ�͉ߋ��̗������擾
	DVKeyMouseIf::DVMousePosition GetMousePosition(int count=0);
};

///	DRKeyMouseWin32
struct DRKeyMouseWin32If:public HIRealDeviceIf{
	SPR_IFDEF(DRKeyMouseWin32);
};

///	DRKeyMouseGLUT
struct DRKeyMouseGLUTIf:public HIRealDeviceIf{
	SPR_IFDEF(DRKeyMouseGLUT);
};

}	//	namespace Spr
#endif
