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
	IF_DEF(HIBase);
	///	�L�����u���[�V�����̑O�ɍs������
	virtual bool BeforeCalibration()=0;
	///	�L�����u���[�V��������
	virtual bool Calibration()=0;
	///	�L�����u���[�V�����̂��Ƃɍs������
	virtual bool AfterCalibration()=0;
	///	�q���[�}���C���^�t�F�[�X�̈ˑ��֌W��ǉ�����
	virtual void AddDeviceDependency(HIRealDeviceIf* rd)=0;
	///	�q���[�}���C���^�t�F�[�X�̈ˑ��֌W���N���A����
	virtual void ClearDeviceDependency()=0;
	///	�q���[�}���C���^�t�F�[�X�̏�Ԃ��X�V����D
	virtual void Update(float dt)=0;
	///	�q���[�}���C���^�t�F�[�X���g�p�\�Ȃ�� true�D
	virtual bool IsGood()=0;
	/**	�q���[�}���C���^�t�F�[�X�̏������D
		�K�v�ȃf�o�C�X��sdk����؂�Ă��āC�q���[�}���C���^�t�F�[�X���\������	*/
	virtual bool Init(HISdkIf* sdk, const void* desc)=0;
};

/**	@brief	�ʒu���̓C���^�t�F�[�X	*/
struct HIPositionIf: public HIBaseIf{
	IF_DEF(HIPosition);
	virtual Vec3f GetPosition()=0;
};

/**	@brief	�p�x���̓C���^�t�F�[�X	*/
struct HIOrientationIf: public HIBaseIf{
	IF_DEF(HIOrientation);
	virtual Quaternionf GetOrientation()=0;
};

/**	@brief	�p��(=�ʒu�{�p�x)���̓C���^�t�F�[�X	*/
struct HIPoseIf: public HIPositionIf{
	IF_DEF(HIPose);
	virtual Quaternionf GetOrientation()=0;
	virtual Posef GetPose()=0;
};

//@}
}
#endif
