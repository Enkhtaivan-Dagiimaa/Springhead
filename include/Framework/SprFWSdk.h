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
struct FWSdkIf : SdkIf {
	SPR_IFDEF(FWSdk);

	/** @brief SDK�I�u�W�F�N�g���쐬����
	 */
	static FWSdkIf* SPR_CDECL CreateSdk();

	/** @brief ��̃V�[�����쐬����
		��̃V�[�����쐬���A�V�[�����X�g�ɒǉ�����D
		�쐬���ꂽ�V�[���̓A�N�e�B�u�ƂȂ�D
	 */
	FWSceneIf* CreateScene(const PHSceneDesc& phdesc = PHSceneDesc(), const GRSceneDesc& grdesc = GRSceneDesc());

	/** @brief �V�[�����t�@�C�����烍�[�h����
		@param filename �t�@�C����
		�w�肳�ꂽ�t�@�C������V�[�������[�h���A�V�[�����X�g�ɒǉ�����D
		���[�h��́C���[�h���ꂽ�V�[�����A�N�e�B�u�ƂȂ�D
		�t�@�C�����ɕ����̃V�[�����܂܂�Ă����ꍇ�́C�Ō�Ƀ��[�h���ꂽ�V�[�����A�N�e�B�u�ƂȂ�D
	 */
	bool LoadScene(UTString filename);

	/** @brief �V�[�����t�@�C���փZ�[�u����
		@param filename �t�@�C����
	 */
	bool SaveScene(UTString filename);

	/// �V�[���̌����擾����
	int NScene() const;

	/** @brief �V�[����؂�ւ���
		@param scene
		�V�[��scene���A�N�e�B�u�ɂ���
	 */
	void	SwitchScene(FWSceneIf* scene);

	/** @brief �V�[�����擾����
		@param index �擾����V�[���̃C���f�b�N�X�D�ȗ�����ƃA�N�e�B�u�V�[�����擾����D
	 */
	FWSceneIf* GetScene(int index = -1);
	/** @brief �V�[����������
		scene1�Ɋ܂܂��S�Ă̗v�f��scene0�Ɉړ����Cscene1���폜����D
	 */
	void	MergeScene(FWSceneIf* scene0, FWSceneIf* scene1);

	/** @brief �����_�����쐬����
		�����_�����쐬���C�����_�����X�g�ɒǉ�����D
		
		��������Device��GL��D3D�����w��ł���悤�ɂ���ׂ�
	 */
	GRRenderIf*	CreateRender();
	/** @brief �����_���̌����擾����
	 */
	int NRender() const;
	/** @brief �����_�����擾����
	 */
	GRRenderIf* GetRender(int index = -1);
	/** @brief �����_�����A�N�e�B�u�ɂ���
	 */
	void SwitchRender(GRRenderIf* render);
	
	/// PHSdk�I�u�W�F�N�g���擾����
	PHSdkIf* GetPHSdk();

	/// GRSdk�I�u�W�F�N�g���擾����
	GRSdkIf* GetGRSdk();

	/// FISdk�I�u�W�F�N�g���擾����
	FISdkIf* GetFISdk();

	/** @brief �f�o�b�O�`�惂�[�h�̎擾
	 */
	bool GetDebugMode();

	/** @brief �f�o�b�O�`�惂�[�h�̐ݒ�
	 */
	void SetDebugMode(bool debug = true);

	/** @brief �V�~�����[�V�����̎��s
	 */
	void Step();

	/** @brief �`������s
		�A�N�e�B�u�ȃV�[���ƃA�N�e�B�u�ȃ����_�����g���ĕ`����s���D
	 */
	void Draw();

	/** @brief �`��̈�̃T�C�Y��ݒ肷��
		@param w �`��̈�̉���
		@param h �`��̈�̏c��
		�A�N�e�B�u�ȃ����_���̕`��̈��ݒ肷��
	 */
	void Reshape(int w, int h);

	static void SPR_CDECL RegisterSdk();
};

}

#endif
