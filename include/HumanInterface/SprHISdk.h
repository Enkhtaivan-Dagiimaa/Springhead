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

	/** @brief	���z�f�o�C�X���؂�Ă���	Rent virtual device from device pool
		@param	type	���z�f�o�C�X�̎��
		@param	name	���f�o�C�X�̖��O
		@param	portNum	���f�o�C�X���̉��ڂ̉��z�f�o�C�X���g�p���邩�w��

		���z�f�o�C�X�̎�ށC���f�o�C�X�������v����f�o�C�X��T���B
		�f�o�C�X��������A���g�p�̏ꍇ�́A�g�p��Ԃɂ��ĕԂ��B
		�f�o�C�X��������Ȃ����A�g�p���̏ꍇ��NULL��Ԃ��B
		name��NULL�̏ꍇ���f�o�C�X���͔C�ӁB
		portNum��-1�̎��͍ŏ��̋󂢂Ă���f�o�C�X�B	*/
	HIVirtualDeviceIf* RentVirtualDevice(const IfInfo* ii, const char* rname=NULL, int portNum = -1);
	HIVirtualDeviceIf* RentVirtualDevice(const char* itype, const char* rname=NULL, int portNum = -1);
	
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
