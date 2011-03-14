/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

/** @file SprUTTimer.h
	�����^�C�}
 */

#ifndef SPR_UTTIMER_H
#define SPR_UTTIMER_H

#include <Foundation/SprObject.h>

namespace Spr{;

struct UTTimerIf : ObjectIf{
	SPR_IFDEF(UTTimer);

	///	�^�C�}�[�̃��[�h
	enum Mode{
		/// 1ms ���x�̐��x�����}���`���f�B�A�^�C�}�[
		MULTIMEDIA,
		/// �X���b�h���g�p�����[���^�C�}�[
		THREAD,
		/// GLUT�Ȃǂ̃t���[�����[�N�Ɉˑ��������b�Z�[�W�p�b�V���O�^�̃^�C�}�[
		FRAMEWORK,
		/// �A�C�h������
		IDLE
	};

	///	�R�[���o�b�N�֐��̌^
	typedef void (SPR_CDECL *TimerFunc)(void* arg);

	///	�^�C�}�̐����擾
	static unsigned		SPR_CDECL NTimers();

	/// �^�C�}���擾
	static UTTimerIf*	SPR_CDECL Get(unsigned id);

	/// �^�C�}���쐬
	static UTTimerIf*	SPR_CDECL Create();

	/// MULTIMEDIA���̃^�C�}�[���x�̎擾
	unsigned GetResolution();
	/// MULTIMEDIA���̃^�C�}�[���x�̐ݒ�iMULTIMEDIA���T�|�[�g���Ȃ��ꍇ�A�ݒ肵���l���珑������邱�Ƃ�����B
	bool SetResolution(unsigned r);					
	
	/// �^�C�}�[�̎��ԊԊu(ms)
	unsigned GetInterval();
	/// �^�C�}�[�̎��ԊԊu(ms)�̐ݒ�
	bool SetInterval(unsigned i);
	
	/// �R�[���o�b�N�֐��̐ݒ�
	bool SetCallback(TimerFunc f, void* arg);
	/// �^�C�}�[�̓��샂�[�h�̎擾
	Mode GetMode();
	/// �^�C�}�[�̓��샂�[�h�̐ݒ�
	bool SetMode(Mode m);

	/// �^�C�}�[��ON�i����I�ɃR�[���o�b�N���Ăяo����ԁj�Ȃ� true
	bool IsStarted();
	/// �R�[���o�b�N�Ăяo�����Ȃ�� true
	bool IsRunning();
	
	/// �^�C�}�[����J�n
	bool Start();
	/// �^�C�}�[�̒�~
	bool Stop();
	/// �R�[���o�b�N�֐����Ăяo��
	void Call();

};

}	//	namespace Spr
#endif
