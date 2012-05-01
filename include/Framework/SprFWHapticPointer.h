/*
 *  Copyright (c) 2003-2012, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

#ifndef SPR_FWHAPTICPOINTERIF_H
#define SPR_FWHAPTICPOINTERIF_H


#include <Physics/SprPHHaptic.h>
#include <HumanInterface/SprHIBase.h>
#include <Foundation/SprObject.h>

namespace Spr{;

struct FWHapticPointerIf : SceneObjectIf {
	SPR_IFDEF(FWHapticPointer);

	/** @brief PHHapticPointerIf��ݒ肷��
	*/
	void SetPHHapticPointer(PHHapticPointerIf* hpGlobal);

	/** @brief PHHapticPointerIf���擾
	*/	
	PHHapticPointerIf*	GetPHHapticPointer();

	/** @brief HumanInterfaceIf��ݒ肷��
	*/
	void SetHumanInterface(HIBaseIf* hi);

	/** @brief HumanInterfaceIf��ݒ肷��
	*/
	HIBaseIf* GetHumanInterface();

	///** @brief �v�Z�񎦗͂��擾����
	//*/
	//Vec3f GetHapticForce();

	///** @breif �v�Z�񎦃g���N���擾����
	//*/
	//Vec3f GetHapticTorque();
};


}

#endif
