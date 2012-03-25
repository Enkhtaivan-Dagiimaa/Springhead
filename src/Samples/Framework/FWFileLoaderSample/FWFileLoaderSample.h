/*
 *  Copyright (c) 2003-2012, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

#ifndef FW_FILELOADER_SAMPLE_H						// �w�b�_�t�@�C������鎞�̂��܂��Ȃ�
#define FW_FILELOADER_SAMPLE_H						// ����

#include <Springhead.h>								// Springhead��API���g�����߂̃w�b�_�t�@�C��
#include <Framework/SprFWApp.h>						// Framework�N���X���錾����Ă���w�b�_�t�@�C��

using namespace Spr;								// Springhead�̖��O���

class FWFileLoaderSample : public FWApp{
public:
	std::string fileName;							///< ���[�h����t�@�C����
	FWFileLoaderSample();							///< �R���X�g���N�^�i���̃N���X�̃C���X�^���X���������ꂽ���ɌĂ΂��֐��j
	virtual void Init(int argc, char* argv[]);		///< �A�v���P�[�V�����̏�����
	virtual void Keyboard(int key, int x, int y);	///< �L�[�{�[�h�֐�

	void InitCameraView();							///< �����J�����ʒu�̐ݒ�		
	void Reset();									///< ���Z�b�g
}; 

#endif