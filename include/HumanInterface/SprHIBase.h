/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 *	@file SprHIBase.h
 *	@brief �q���[�}���C���^�t�F�[�X�̊�{�N���X
*/


#ifndef SPR_HIBase_H
#define SPR_HIBase_H
#include <HumanInterface/SprHIDevice.h>
#include <HumanInterface/SprHumanInterfaceDecl.hpp>

namespace Spr{;

/**	\addtogroup	gpHumanInterface	*/
//@{

/**	@brief	�q���[�}���C���^�t�F�[�X�̊�{�N���X
 **/
struct HIBaseIf : NamedObjectIf{
	SPR_IFDEF(HIBase);
	///	�L�����u���[�V�����̑O�ɍs������
	bool BeforeCalibration();
	///	�L�����u���[�V��������
	bool Calibration();
	///	�L�����u���[�V�����̂��Ƃɍs������
	bool AfterCalibration();
	///	�q���[�}���C���^�t�F�[�X�̈ˑ��֌W��ǉ�����
	void AddDeviceDependency(HIRealDeviceIf* rd);
	///	�q���[�}���C���^�t�F�[�X�̈ˑ��֌W���N���A����
	void ClearDeviceDependency();
	///	�q���[�}���C���^�t�F�[�X�̏�Ԃ��X�V����D
	void Update(float dt);
	///	�q���[�}���C���^�t�F�[�X���g�p�\�Ȃ�� true�D
	bool IsGood();
	/**	�q���[�}���C���^�t�F�[�X�̏������D
		�K�v�ȃf�o�C�X��sdk����؂�Ă��āC�q���[�}���C���^�t�F�[�X���\������	*/
	bool Init(const void* desc);
};

/**	@brief	�ʒu���̓C���^�t�F�[�X	*/
/*struct HIPositionIf : HIBaseIf{
	SPR_VIFDEF(HIPosition);
	Vec3f GetPosition();
};*/

/**	@brief	�p�x���̓C���^�t�F�[�X	*/
/*struct HIOrientationIf : HIBaseIf{
	SPR_VIFDEF(HIOrientation);
	Quaternionf GetOrientation();
};*/

/**	@brief	�p��(=�ʒu�{�p�x)���̓C���^�t�F�[�X	*/
struct HIPoseIf : HIBaseIf{
	SPR_VIFDEF(HIPose);
	//SPR_OVERRIDEMEMBERFUNCOF(HIBaseIf, HIPositionIf);
	
	Vec3f			GetPosition();
	Quaternionf		GetOrientation();
	Posef			GetPose();
	Affinef			GetAffine();
	Vec3f			GetVelocity();
	Vec3f			GetAngularVelocity();
};

/** @brief	6���R�x�͊o�C���^�t�F�[�X�̊�{�N���X
 **/
struct HIHapticIf : HIPoseIf{
	SPR_VIFDEF(HIHaptic);
	///	�f�o�C�X�̎��ۂ̒񎦃g���N��Ԃ�
	Vec3f GetTorque();
	///	�f�o�C�X�̎��ۂ̒񎦗͂�Ԃ�
	Vec3f GetForce();
	///	�f�o�C�X�̖ڕW�o�͂ƃg���N�o�͂�ݒ肷��
	void SetForce(const Vec3f& f, const Vec3f& t = Vec3f());
};

//@}
}
#endif
