/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

/*
	3��Xfile��3�̃E�B���h�E�ŕ\��
	�R�[���o�b�N�^�C�}��3�ł��ꂼ��̃V�[�����V�~�����[�V����
	q, ESC	: �A�v���P�[�V�����I��
	r		: Xfile�̃����[�h

*/

#include "FWMultiWinSample2.h"

FWMultiWinSample2 app;
int __cdecl main(int argc, char* argv[]){	
	app.Init(argc, argv);	
	app.StartMainLoop();
	return 0;
}