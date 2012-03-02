/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 *	@file SprHISpaceNavigator.h
 *	@brief 3Dconnexion SpaceNavigator 6���R�x���̓f�o�C�X
*/

#ifndef SPR_HI_SPACENAVIGATOR_H
#define SPR_HI_SPACENAVIGATOR_H

#include <HumanInterface/SprHIDevice.h>
#include <HumanInterface/SprHIKeyMouse.h>

namespace Spr{;
/**	\addtogroup	gpHumanInterface	*/
//@{

/**	@brief SpaceNavigator�̃C���^�t�F�[�X */
struct HISpaceNavigatorIf: public HIPoseIf{
	SPR_VIFDEF(HISpaceNavigator);

	///	���݂̎p����ݒ�
	bool SetPose(Posef pose);

	/// �ʒu�̍������擾
	Vec3f GetTrnDelta();

	/// ��]�̍������擾
	Vec3f GetRotDelta();

	/// RAW Input Windows���b�Z�[�W�����߂���
	bool PreviewMessage(void *m);
};
struct HISpaceNavigatorDesc{
	SPR_DESCDEF(HISpaceNavigator);

	void* hWnd;
};


//@}
}
#endif
