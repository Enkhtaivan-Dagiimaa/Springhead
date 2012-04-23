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

/** ���z�L�[�{�[�h�E�}�E�X
	�E�L�[�E�}�E�X�C�x���g����������N���X��DVKeyMouseCallback���p�����C�x���g�n���h������������
	�E���̏��DVKeyMouseBaseIf::AddCallback�Ŏ��g��o�^����
	�E�n���h���͕����o�^�\�D
 */

/// ������Ă���L�[�C�{�^���̏�Ԃ������t���O
struct DVButtonMask{
	enum{
		LBUTTON		= 0x01,
		RBUTTON		= 0x02,
		MBUTTON		= 0x04,
		SHIFT		= 0x08,
		CONTROL		= 0x10,
		ALT			= 0x20,
	};
};
struct DVButtonSt{
	enum{
		DOWN = 0,
		UP	 = 1,
	};
};
struct DVKeySt{
	enum{
		PRESSED		= 0x01,		///< ������Ă���
		TOGGLE_ON	= 0x02,		///< �g�O������Ă���(caps lock�Ȃ�)
	};
};
/** ascii�ȊO�̓���L�[�R�[�h�D�l��GLUT�̓���L�[�R�[�h��256�����Z�������́D
 **/
struct DVKeyCode{
	enum{
		BACKSPACE				= 8,
		TAB						= 9,
		CR						= 13,
		ENTER					= CR,
		ESC						= 27,
		DEL						= 127,
		OFFSET					= 256,
		F1, 
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		LEFT					= 100 + OFFSET,
		UP,
		RIGHT,
		DOWN,
		PAGE_UP,
		PAGE_DOWN,
		HOME,
		END,
		INSERT,
	};
};

struct DVKeyMouseCallback{
	/** @brief �}�E�X�N���b�N���̏���
		@param button	DVButtonMask�񋓎q�̑g�ݍ��킹
		@param state	DVButtonSt�񋓎q�̒l
		@param x		�J�[�\����x���W
		@param y		�J�[�\����y���W
	 */
	virtual bool OnMouse(int button, int state, int x, int y){ return false; }
	/** @brief �_�u���N���b�N���̏���
		@param button	ButtonMask�񋓎q�̑g�ݍ��킹
		@param x		�J�[�\����x���W
		@param y		�J�[�\����y���W
	 */
	virtual bool OnDoubleClick(int button, int x, int y){ return false; }
	/** @brief �}�E�X���������Ƃ��̏���
		@param button	ButtonMask�񋓎q�̑g�ݍ��킹
		@param x		�J�[�\����x���W
		@param y		�J�[�\����y���W
		@param zdelta	�}�E�X�z�C�[���̕ω���
		
		zdelta�̓}�E�X�z�C�[���ɑΉ�����f�o�C�X���g�p����ꍇ�̂ݗL���D
	*/
	virtual bool OnMouseMove(int button, int x, int y, int zdelta){ return false; }
	/** @brief �L�[���͏���
		@param state	DVKeySt�񋓎q�̒l
		@param key		ascii�R�[�h��DVKeyCode�񋓎q�̒l
		@param x		�J�[�\����x���W
		@param y		�J�[�\����y���W
	 */
	virtual bool OnKey(int state, int key, int x, int y){ return false; }
};

/**
	�L�[�{�[�h�E�}�E�X���z�f�o�C�X
 **/
struct DVKeyMouseIf: public HIVirtualDeviceIf{
	SPR_IFDEF(DVKeyMouse);
	
	///	�n���h���̒ǉ�
	void AddCallback(DVKeyMouseCallback* cb);
	/// �n���h���̍폜
	void RemoveCallback(DVKeyMouseCallback* cb);

	/**	@brief �}�E�X�{�^���E�L�[�{�[�h��Ԏ擾
		@param	key		DVKeyCode�̒l
		@return			DVKeySt�̒l
		win32�}�E�X����D
	 */
	int GetKeyState(int key);

	/**	@brief ���݂���щߋ��̃}�E�X�ʒu���擾
		@param x		x���W
		@param y		y���W
		@param time		�^�C���X�^���v
		@param count	�����O�̏����擾���邩
		count��[0,63]�͈̔͂��L���D0�Ȃ�΍ŐV�C1�ȏ�͉ߋ��̗������擾����D
		Win32��GLUT�ŋ������قȂ�̂Œ��ӁDGLUT�ł�time�l�͖����D
	 */
	void GetMousePosition(int& x, int& y, int& time, int count=0);
};

/**
	Win32�}�E�X
 **/
struct DRKeyMouseWin32If: public HIRealDeviceIf{
	SPR_IFDEF(DRKeyMouseWin32);
};
struct DRKeyMouseWin32Desc{
	SPR_DESCDEF(DRKeyMouseWin32);
};

/**
	GLUT�}�E�X
 **/
struct DRKeyMouseGLUTIf: public HIRealDeviceIf{
	SPR_IFDEF(DRKeyMouseGLUT);
};
struct DRKeyMouseGLUTDesc{
	SPR_DESCDEF(DRKeyMouseGLUT);
};

}	//	namespace Spr
#endif
