/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_FWAPPGL_H
#define SPR_FWAPPGL_H
#include <Framework/SprFWApp.h>

namespace Spr{;

/** OpenGL�̃R���e�L�X�g�쐬�@�\�����A�v���P�[�V�����N���X
 */
class FWAppGL : public FWApp{
public:
	/** @brief �V����Window�̂��߂́AGRRender, GRDevice�����B
	 */
	FWSdkIf::FWWin* CreateRender(int wid);
};

}

#endif
