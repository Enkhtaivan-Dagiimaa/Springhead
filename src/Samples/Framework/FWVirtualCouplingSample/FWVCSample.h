/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

#ifndef FWLDHAPTIC_SAMPLE_H
#define FWLDHAPTIC_SAMPLE_H

#include <Springhead.h>
#include <Framework/SprFWApp.h>

using namespace Spr;

class FWLDHapticSample : public FWApp{
public:
	FWLDHapticSample();
	virtual void Init(int argc, char* argv[]);			///< �A�v���P�[�V�����̏�����
	virtual void InitCameraView();						///< �J�����s��̏�����
	virtual void InitHumanInterface();					///< HapticInterface�̏�����
	virtual void Reset();
	virtual void TimerFunc(int id);
	virtual void IdleFunc();							
	virtual void Display();								///< �`��֐�	
	virtual void BuildScene();							///< ���̂̍쐬
	virtual void BuildPointer();						///< ���̂̍쐬
								
	virtual void Keyboard(int key, int x, int y);		///< �L�[�{�[�h�֐�

	/// �I�v�V����
	bool bStep;									///< �V�~�����[�V������i�߂邩�ǂ���
	bool bOneStep;								///< �X�e�b�v���s
	bool bDrawInfo;								///< �f�o�b�N�\�����邩�ǂ���
	double dt;									///< �X�V�X�e�b�v[s]
}; 

#endif