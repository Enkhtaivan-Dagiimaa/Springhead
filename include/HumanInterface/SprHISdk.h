/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 *	@file SprHISdk.h
 *	@brief �q���[�}���C���^�t�F�[�XSDK
*/
#ifndef SPR_HISdkIF_H
#define SPR_HISdkIF_H

#include <HumanInterface/SprHIBase.h>
#include <Foundation/SprScene.h>

namespace Spr{;

/** \addtogroup gpHumanInterface �q���[�}���C���^�t�F�[�XSDK */
//@{

//	�������[�h�̂��߂̒��g�Ȃ��̃f�X�N���v�^
struct HISdkDesc{
};

///	�����V�~�����[�V����SDK
struct HISdkIf: public NameManagerIf{
	SPR_IFDEF(HISdk);

	///	SDK�̏�����
	//void Init();
	
	/** @brief	���z�f�o�C�X���؂�Ă���	Rent virtual device from device pool
		@param	type	�f�o�C�X�̎��
		@param	name	�f�o�C�X�̖��O
		@param	portNo	�|�[�g�ԍ�

		��ށC���O����у|�[�g�ԍ����̍��v���関�g�p�̃f�o�C�X��T���C
		�ŏ��Ɍ��������f�o�C�X���g�p��Ԃɂ��Ă���Ԃ��D
		name��NULL�̏ꍇ�͖��O�͔C�ӁD
		portNo��-1�̏ꍇ�̓|�[�g�ԍ��͔C�ӁD
		�Y������f�o�C�X��������Ȃ��ꍇ��NULL���Ԃ����D
	 */
	HIVirtualDeviceIf* RentVirtualDevice(const IfInfo* ii, const char* name=NULL, int portNo = -1);
	//HIVirtualDeviceIf* RentVirtualDeviceNo(const char* type, int No, const char* name=NULL );
	
	/** @brief	���z�f�o�C�X��ԋp����	Return virtual device to device pool
		�f�o�C�X�𖢎g�p��Ԃɂ���
	 */
	bool ReturnVirtualDevice(HIVirtualDeviceIf* dev);
	
	/**	@brief ���f�o�C�X��o�^	Create and register real device.
		@param IfInfo	�쐬����f�o�C�X��IfInfo
		@param desc		�쐬����f�o�C�X�̃f�B�X�N���v�^
	 */
	HIRealDeviceIf* AddRealDevice(const IfInfo* keyInfo, const void* desc=NULL);
	/** @brief ���L�[�{�[�h�E�}�E�X��o�^
		@param IfInfo	
		@param win		�֘A�Â���E�B���h�E
		@param desc
	 */
	//bool AddRealKeyMouse(const IfInfo* keyInfo, void* win, const void* desc=NULL);

	/**	@brief ���f�o�C�X���擾	Get real device.
	 */
	HIRealDeviceIf* FindRealDevice(const char* name);
	HIRealDeviceIf*	FindRealDevice(const IfInfo* ii);

	/**	@brief	�q���[�}���C���^�t�F�[�X�̍쐬
		@param	info	�쐬����HI�̌^���
	 **/
	HIBaseIf* CreateHumanInterface(const IfInfo* info);
	/**	@brief	�q���[�}���C���^�t�F�[�X�̍쐬
		@param	name	�쐬����HI�̌^��
	 **/
	HIBaseIf* CreateHumanInterface(const char* name);

	//	SDK�����A�o�^�֐�
	///	HISdk�̃C���X�^���X���쐬
	static HISdkIf* SPR_CDECL CreateSdk();
	///	HISdk���t�@�C�����[�_�[�Ȃǂɓo�^
	static void SPR_CDECL RegisterSdk();
};

//@}

}	//	namespace Spr
#endif
