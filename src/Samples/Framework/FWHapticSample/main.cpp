/*
 *  Copyright (c) 2003-2012, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

/*
	�͊o�C���^�t�F�[�X���g���A�o�[�`�������E�Ɨ͊o�C���^���N�V�������ł���T���v��
	�L�[����
	1:�͊o�����_�����O���[�h���y�i���e�B
	2:�͊o�����_�����O���[�h���v���L�V��
	c:�C���^�t�F�[�X�̃L�����u���[�V����
	f:�͊o�񎦂�ON/OFF
	v:�U���񎦂�ON/OFF
	d:�f�o�b�N�\��
	a:�o�l�W����100up
	z:�o�l�W����100down
	space:���̂𐶐�����
*/

#include "FWHapticSample.h"

FWHapticSample app;

int _cdecl main(int argc, char* argv[]){
	app.Init(argc, argv);
	app.StartMainLoop();
	return 0;
}