/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef UTQTTIMER_H
#define UTQTTIMER_H
#pragma once
#include <Foundation/UTPreciseTimer.h> //UTLargeInteger�̓ǂݍ���
namespace Spr{;

//���{�搶�̃\�[�X�R�[�h
//http://kaji-lab.jp/ja/index.php?people/kaji
//QueryPerformanceCounter���g������D(windows.h���Ő錾���ꂽ�֐��j
//�ȑO��RDTSC���g�����o�[�W�����Ő����Ă�������
//QueryPerformanceCounter���g���C
//�X���b�h�����b�N����
//���Ƃɂ���āC���Ԃ�C�قډ����D
//����ȏ�����߂�Ȃ獡�̂Ƃ���̓C���^�t�F�[�X�{�[�h�̃^�C�}�J�E���^���g�������Ȃ��D

class UTQPTimer{
private:
	UTLargeInteger freq;
	UTLargeInteger lasttime;//�O��̏�Ԃ�ۑ�����
	unsigned long stopWatch;		///<�v������us�P��
	bool startFlag;

public:
	UTQPTimer();
	void WaitUS(int time); //��s�P�ʂő҂�
	int CountUS();//�O�񂩂�̌o�ߎ��Ԃ���s�P�ʂŌv��
	void CountAndWaitUS(int time);//�O�񂩂�̌o�ߎ��Ԃ���s�P�ʂŌv�����C�S�̂Ƃ���us�P�ʂő҂i��胋�[�v�����p�j
	int Freq(); //�^�C�}�[�̎��g�����o��

	/// �X�g�b�v�E�H�b�`�@�\.
	unsigned long Start();		///< �v���J�n�C�J�n���ԁistopWatch�̒l�j��us�P�ʂŕԂ�
	unsigned long Stop();		///< �v����~�C�o�ߎ��ԁistopWatch�̒l�j��us�P�ʂŕԂ�
	unsigned long GetTime();	///< ���݂̎��ԁistopWatch�̒l�j��us�P�ʂŕԂ�
	unsigned long Clear();		///< �N���A�CstopWatch��0�ɂ���
};

}
#endif

