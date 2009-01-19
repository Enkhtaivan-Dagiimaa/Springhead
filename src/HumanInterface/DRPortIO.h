/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef DRPORTIO_H
#define DRPORTIO_H
#include <base/BaseUtility.h>

namespace Spr {
class SPR_DLL DRLoadWindowsDriver;

/**	WindowsNT,2000 ��I/O�|�[�g���ڃA�N�Z�X�����邽�߂ɌĂԊ֐�.
	������Ă�ł����΁A_inp, _outp ���s���Ă��ی�ᔽ�������Ȃ��Ȃ�B
	giveio.sys �� exe�Ɠ����t�H���_���AWindows�̃t�H���_���A�p�X�̒ʂ���
	�t�H���_�ɓ���Ă������Ƃ��K�v�B
*/
void WBGetPortIO();
}	//	namespace Spr;

#if defined _MSC_VER
 #include <conio.h>
#endif

#if defined __BORLANDC__
#define _inp bcb_inp
#define _outp bcb_outp
#define _inpw bcb_inpw
#define _outpw bcb_outpw
namespace Spr {
 unsigned char SPR_DLL bcb_inp(unsigned short adr);
 void SPR_DLL bcb_outp(unsigned short adr, unsigned char v);
 unsigned short SPR_DLL bcb_inpw(unsigned short adr);
 void SPR_DLL bcb_outpw(unsigned short adr, unsigned short v);
}	//	namespace Spr;
#endif



#endif
