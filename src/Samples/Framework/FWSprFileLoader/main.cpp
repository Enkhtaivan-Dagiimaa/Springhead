/*
 *  Copyright (c) 2003-2012, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

#include "FWSprFileLoader.h"
/**
	�V���v����Sprfile���[�_�[
	�V�~�����[�V������glutIdleFunc�Ői�߂�
	q, ESC	: �A�v���P�[�V�����I��
	r		: �t�@�C���̃����[�h
	w		: �J�����s��̏�����
	d		: �f�o�b�N�\���؂�ւ�
*/

FWSprfileLoader app;
int _cdecl main(int argc, char* argv[]){
	app.Init(argc, argv);
	app.StartMainLoop();
	return 0;
}
