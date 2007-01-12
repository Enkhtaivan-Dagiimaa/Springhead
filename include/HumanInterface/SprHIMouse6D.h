/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 *	@file SprDRDevice.h
 *	@brief �q���[�}���C���^�t�F�[�X����邽�߂̍ޗ��ɂȂ�f�o�C�X�̃N���X�D���Ƃ��΁CD/A,A/D,PIO�ȂǁD
*/

#ifndef SPR_HIMouse6D_H
#define SPR_HIMouse6D_H
#include <SprFoundation.h>

namespace Spr{;
/**	\addtogroup	gpHumanInterface	*/
//@{

struct DVKeyMouseIf;
///	���f�o�C�X�D�C���^�t�F�[�X�J�[�h�Ƃ��CUSB�f�o�C�X�Ƃ��C
struct HIMouse6DIf: public HIPoseIf{
	IF_DEF(HIMouse6D);
	virtual DVKeyMouseIf* GetKeyMouse()=0;
};


//@}
}
#endif
