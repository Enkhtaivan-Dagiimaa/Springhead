/*
 *  Copyright (c) 2003-2012, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

#ifndef FWHAPTIC_SAMPLE_H
#define FWHAPTIC_SAMPLE_H

#include <Springhead.h>
#include <Framework/SprFWApp.h>

using namespace Spr;

class FWHapticSample : public FWApp{
public:
	enum HumanInterface{
		SPIDAR,				
		XBOX,				
		FALCON,
	} humanInterface;
	enum EngineType{
		SINGLE = 0,				// �V���O���������
		MULTI,					// �}���`�X���b�h
		LD,						// �}���`+�Ǐ��V�~�����[�V����
	} engineType;
	PHSceneIf* phscene;			// PHScene�ւ̃|�C���^
	PHHapticPointerIf* pointer; // �͊o�|�C���^�ւ̃|�C���^
	float pdt;					// �����X���b�h�̍���
	float hdt;					// �͊o�X���b�h�̍���
	int physicsTimerID;			// �����X���b�h�̃^�C�}
	int hapticTimerID;			// �͊o�X���b�h�̃^�C�}
	UTRef<HIBaseIf> spg;		// �͊o�C���^�t�F�[�X�ւ̃|�C���^

	FWHapticSample();
	void InitInterface();						// �͊o�C���^�t�F�[�X�̏�����
	void Init(int argc = 0, char* argv[] = 0);	// �A�v���P�[�V�����̏�����
	virtual void BuildScene();						// �I�u�W�F�N�g�̍쐬
	virtual void Keyboard(int key, int x, int y);	// �L�[�{�[�h�֐�
	void TimerFunc(int id);							// �e�^�C�}���ĂԃR�[���o�b�N�֐�
};


#endif