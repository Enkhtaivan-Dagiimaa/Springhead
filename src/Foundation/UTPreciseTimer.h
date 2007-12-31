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
/**	CPU�N���b�N�𗘗p�������m�ȃ^�C�}�[	
	����ڌ��̊��{���񂪍��ꂽ PreciseTimer.h �����ς��܂����D	*/

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
	unsigned long stopWatch;
	bool startFlag;
public:
	UTPreciseTimer();
	void Init(int period = 100);	///<	�J�E���^�̏�����
	void WaitUS(int time);			///<	��s�P�ʂő҂�
	int CountUS();					///<	�O�񂩂�̌o�ߎ��Ԃ���s�P�ʂŌv��
	void CountAndWaitUS(int time);	///<	�O�񂩂�̌o�ߎ��Ԃ���s�P�ʂŌv�����C�S�̂Ƃ���us�P�ʂő҂i��胋�[�v�����p�j
	//�X�g�b�v�E�H�b�`�@�\	
	unsigned long Start();					///<	�v���J�n
	unsigned long Stop();					///<	�v����~
	unsigned long Clear();					///<	�N���A
	static unsigned long CPUFrequency(){	///<	CPU�̎��g����Ԃ��D
		return freq;
	}
};

}

#endif
