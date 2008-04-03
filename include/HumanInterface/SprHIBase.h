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

namespace Spr{;

/**	\addtogroup	gpHumanInterface	*/
//@{

struct DRRealDeviceIf;

/**	@brief	�q���[�}���C���^�t�F�[�X�̊�{�N���X	*/
struct HIBaseIf: public NamedObjectIf{
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
	bool Init(HISdkIf* sdk, const void* desc);
};

/**	@brief	�ʒu���̓C���^�t�F�[�X	*/
struct HIPositionIf: public HIBaseIf{
	SPR_IFDEF(HIPosition);
	Vec3f GetPosition();
};

/**	@brief	�p�x���̓C���^�t�F�[�X	*/
struct HIOrientationIf: public HIBaseIf{
	SPR_IFDEF(HIOrientation);
	Quaternionf GetOrientation();
};

/**	@brief	�p��(=�ʒu�{�p�x)���̓C���^�t�F�[�X	*/
struct HIPoseIf: public HIPositionIf{
	SPR_IFDEF(HIPose);
	Quaternionf GetOrientation();
	Posef GetPose();
};

//@}
}
#endif
