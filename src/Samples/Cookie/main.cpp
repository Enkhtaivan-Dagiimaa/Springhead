/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/***********************************************************************************************
 *	<<Application>>	Cookie
 *	<<Author>>		T. Tokizaki (Programing)�CH. Matsuyama (Modeling) 
 *	<<Date>>		Feb. 23, 2008
 *	<<Overview>>	Press 'm'				: Make a new cookie.
 *					Press '1'				: Zoom in  of the top view.
 *					Press '2'				: Zoom out of the top view.
 *					Press '3'				: Rotate the top view.
 *					Press 'Esc', 'q', 'Q'	: Quit the program.
 ***********************************************************************************************/

#include <Springhead.h>		//	Springhead�̃C���^�t�F�[�X
#include <GL/glut.h>
#include "MYApp.h"
#include "resource.h"
#include <ctime>

#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

using namespace Spr;

/**
 brief		���C���֐�
 param		<in/--> argc�@�@�R�}���h���C�����͂̌�
 param		<in/--> argv�@�@�R�}���h���C������
 return		0 (����I��)
 */
MYApp app;
int main(int argc, char* argv[]){
	srand((unsigned) time(NULL));
	app.Init(argc, argv);
	Sleep(10);
	app.Start();
	return 0;
}
