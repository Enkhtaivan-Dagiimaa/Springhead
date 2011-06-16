/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_HI_JOYSTICK_H
#define SPR_HI_JOYSTICK_H

#include <HumanInterface/SprHIBase.h>

namespace Spr {;

struct DVJoyStickMask{
	enum {
		BUTTON_A = 0x01,
		BUTTON_B = 0x02,
		BUTTON_C = 0x04,
		BUTTON_D = 0x08,
	};
};

/** ���z�W���C�X�e�B�b�N
	�E�g�p���@�͉��z�}�E�X�Ɠ��l
 */
struct DVJoyStickHandler{
	/** @brief �}�E�X���������Ƃ��̏���
		@param buttonMask ������Ă���{�^���̏��
		@param x		�W���C�X�e�B�b�Nx���W�i���E�j
		@param y		�W���C�X�e�B�b�Ny���W�i�O��j
		@param z		�W���C�X�e�B�b�N�Ђ˂�or�㉺
		
		zdelta�̓}�E�X�z�C�[���ɑΉ�����f�o�C�X���g�p����ꍇ�̂ݗL���D
	*/
	virtual bool OnUpdate(int buttonMask, int x, int y, int z){ return false; }
};


/**
	�W���C�X�e�B�b�N���z�f�o�C�X
 **/
struct DVJoyStickIf: public HIVirtualDeviceIf{
	SPR_IFDEF(DVJoyStick);
	
	///	�n���h���̒ǉ�
	void AddHandler(DVJoyStickHandler* h);
	/// �n���h���̍폜
	void RemoveHandler(DVJoyStickHandler* h);

	// �|�[�����O�����̐ݒ�
	void SetPollInterval(int ms);
	// �|�[�����O�����̎擾
	int  GetPollInterval();

};

/**
	GLUT�W���C�X�e�B�b�N
 **/
struct DRJoyStickGLUTIf: public HIRealDeviceIf{
	SPR_IFDEF(DRJoyStickGLUT);
};

}	//	namespace Spr
#endif
