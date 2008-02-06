/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_FWSDKIF_H
#define SPR_FWSDKIF_H

#include <Framework/SprFWScene.h>
#include <Physics/SprPHSdk.h>
#include <Graphics/SprGRSdk.h>
#include <FileIO/SprFISdk.h>

namespace Spr{;

struct FWSdkDesc{
};

struct FWSceneIf;
struct FWSdkIf : ObjectIf {
	IF_DEF(FWSdk);

	/** @brief SDK�I�u�W�F�N�g���쐬����
	 */
	static FWSdkIf* SPR_CDECL CreateSdk();

	/** @brief ��̃V�[�����쐬����
		��̃V�[�����쐬���A�V�[�����X�g�ɒǉ�����D
		�쐬���ꂽ�V�[���̓A�N�e�B�u�ƂȂ�D
	 */
	virtual FWSceneIf* CreateScene(const PHSceneDesc& phdesc = PHSceneDesc(), const GRSceneDesc& grdesc = GRSceneDesc())=0;

	/** @brief �V�[�����t�@�C�����烍�[�h����
		@param filename �t�@�C����
		�w�肳�ꂽ�t�@�C������V�[�������[�h���A�V�[�����X�g�ɒǉ�����D
		���[�h��́C���[�h���ꂽ�V�[�����A�N�e�B�u�ƂȂ�D
		�t�@�C�����ɕ����̃V�[�����܂܂�Ă����ꍇ�́C�Ō�Ƀ��[�h���ꂽ�V�[�����A�N�e�B�u�ƂȂ�D
	 */
	virtual bool LoadScene(UTString filename)=0;

	/** @brief �V�[�����t�@�C���փZ�[�u����
		@param filename �t�@�C����
	 */
	virtual bool SaveScene(UTString filename)=0;

	/// �V�[���̌����擾����
	virtual int NScene() const=0;

	/** @brief �V�[����؂�ւ���
		@param scene
		�V�[��scene���A�N�e�B�u�ɂ���
	 */
	virtual void	SwitchScene(FWSceneIf* scene)=0;

	/** @brief �V�[�����擾����
		@param index �擾����V�[���̃C���f�b�N�X�D�ȗ�����ƃA�N�e�B�u�V�[�����擾����D
	 */
	virtual FWSceneIf* GetScene(int index = -1)=0;

	/** @brief �V�[����������
		scene1�Ɋ܂܂��S�Ă̗v�f��scene0�Ɉړ����Cscene1���폜����D
	 */
	virtual void	MergeScene(FWSceneIf* scene0, FWSceneIf* scene1) = 0;

	/** @brief �����_�����쐬����
		�����_�����쐬���C�����_�����X�g�ɒǉ�����D
		
		��������Device��GL��D3D�����w��ł���悤�ɂ���ׂ�
	 */
	virtual GRRenderIf*	CreateRender() = 0;
	/** @brief �����_���̌����擾����
	 */
	virtual int NRender() const = 0;
	/** @brief �����_�����擾����
	 */
	virtual GRRenderIf* GetRender(int index = -1) = 0;
	/** @brief �����_�����A�N�e�B�u�ɂ���
	 */
	virtual void SwitchRender(GRRenderIf* render) = 0;
	
	/// PHSdk�I�u�W�F�N�g���擾����
	virtual PHSdkIf* GetPHSdk()=0;

	/// GRSdk�I�u�W�F�N�g���擾����
	virtual GRSdkIf* GetGRSdk()=0;

	/// FISdk�I�u�W�F�N�g���擾����
	virtual FISdkIf* GetFISdk()=0;

	/** @brief �f�o�b�O�`�惂�[�h�̎擾
	 */
	virtual bool GetDebugMode() = 0;

	/** @brief �f�o�b�O�`�惂�[�h�̐ݒ�
	 */
	virtual void SetDebugMode(bool debug = true) = 0;

	/** @brief �V�~�����[�V�����̎��s
	 */
	virtual void Step()=0;

	/** @brief �`������s
		�A�N�e�B�u�ȃV�[���ƃA�N�e�B�u�ȃ����_�����g���ĕ`����s���D
	 */
	virtual void Draw() = 0;

	/** @brief �`��̈�̃T�C�Y��ݒ肷��
		@param w �`��̈�̉���
		@param h �`��̈�̏c��
		�A�N�e�B�u�ȃ����_���̕`��̈��ݒ肷��
	 */
	virtual void Reshape(int w, int h)=0;

	static void SPR_CDECL RegisterSdk();
};

}

#endif
