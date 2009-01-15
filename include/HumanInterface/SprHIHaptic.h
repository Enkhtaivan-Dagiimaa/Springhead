/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 *	@file SprHIHaptic.h
 *	@brief �q���[�}���C���^�t�F�[�X�̊�{�N���X
*/


#ifndef SPR_HIHaptic_H
#define SPR_HIHaptic_H
#include <HumanInterface/SprHIBase.h>

namespace Spr{;

/**	\addtogroup	gpHumanInterface	*/
//@{

struct DRRealDeviceIf;

/**	@brief	6���R�x�͊o�C���^�t�F�[�X�̊�{�N���X	*/
struct HIForceInterface6DIf: public HIPoseIf{
	SPR_VIFDEF(HIForceInterface6D);
	///�f�o�C�X�̑��x��Ԃ�
	Vec3f GetVelocity();
	///�f�o�C�X�̊p���x��Ԃ�
	Vec3f GetAngularVelocity();
	///	�f�o�C�X�̎��ۂ̒񎦃g���N��Ԃ�
	Vec3f GetTorque();
	///	�f�o�C�X�̎��ۂ̒񎦗͂�Ԃ�
	Vec3f GetForce();
	///	�f�o�C�X�̖ڕW�o�͂ƃg���N�o�͂�ݒ肷��
	void SetForce(const Vec3f& f, const Vec3f& t);
};

/**	@brief	3���R�x�͊o�C���^�t�F�[�X�̊�{�N���X	*/
struct HIForceInterface3DIf: public HIPoseIf{
	SPR_VIFDEF(HIForceInterface3D);
	///�f�o�C�X�̑��x��Ԃ�
	Vec3f GetVelocity();
	///	�f�o�C�X�̎��ۂ̒񎦗͂�Ԃ�
	Vec3f GetForce();
	///	�f�o�C�X�̖ڕW�o�͂ƃg���N�o�͂�ݒ肷��
	void SetForce(const Vec3f& f);
};

//@}
}
#endif
