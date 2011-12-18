/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef UTPRECISETIMER_H
#define UTPRECISETIMER_H
#pragma once

namespace Spr{;

/** Precise timer using CPU clock
    Modified from PreciseTimer.h written by Kajimoto in Tachi Lab. at Univ. of Tokyo. */

// �}���`�X���b�h�ł̌v���̏ꍇ�́CUTQPTimer���g��

union UTLargeInteger{
	struct {
		unsigned long lowPart;
		long highPart;  
	};  
	long long quadPart;
};

class UTPreciseTimer{
private:
	static unsigned long freq;
	UTLargeInteger cycles2;
	unsigned long stopWatch;		///<�v������us�P��
	bool startFlag;
public:
	UTPreciseTimer();
	void Init(int period = 100);	///< �J�E���^�̏�����
	void WaitUS(int time);			///< us�P�ʂő҂�
	int CountUS();					///< �O�񂩂�̌o�ߎ��Ԃ�us�P�ʂŌv��
	void CountAndWaitUS(int time);	///< �O�񂩂�̌o�ߎ��Ԃ�us�P�ʂŌv�����C�S�̂Ƃ���us�P�ʂő҂i��胋�[�v�����p�j
	
	/// �X�g�b�v�E�H�b�`�@�\.
	unsigned long Start();	///< �v���J�n�C�J�n���ԁistopWatch�̒l�j��us�P�ʂŕԂ�
	unsigned long Stop();	///< �v����~�C�o�ߎ��ԁistopWatch�̒l�j��us�P�ʂŕԂ�
	unsigned long Clear();	///< �N���A�CstopWatch��0�ɂ���
	static unsigned long CPUFrequency(){	///< CPU�̎��g����Ԃ�
		return freq;
	}
};

}

#endif
