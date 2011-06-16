/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_FWSDKIF_H
#define SPR_FWSDKIF_H

#include <Physics/SprPHScene.h>
#include <Graphics/SprGRScene.h>
#include <Framework/SprFWInteractScene.h>

namespace Spr{;

struct ImportIf;
struct FWSceneIf;
struct FISdkIf;
struct HISdkIf;

struct FWSdkDesc{};

/** @brief Framework SDK
	FWSdk -
		PHSdk
		GRSdk
		FISdk
		HISdk
		FWScene[]
		GRRender[]
 */
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
		@param ii		�t�@�C���̎�ނ�����IfInfo
		@param objs		�e�I�u�W�F�N�g�̔z��
		�w�肳�ꂽ�t�@�C������V�[�������[�h���A�V�[�����X�g�ɒǉ�����D
		���[�h��́C���[�h���ꂽ�V�[�����A�N�e�B�u�ƂȂ�D
		�t�@�C�����ɕ����̃V�[�����܂܂�Ă����ꍇ�́C�Ō�Ƀ��[�h���ꂽ�V�[�����A�N�e�B�u�ƂȂ�D

		ii��NULL�̏ꍇ�͊g���q����t�@�C���^�C�v�����肳���D
		�t�@�C���^�C�v�𖾎��������ꍇ��ii�ɑΉ�����IfInfo���w�肷��(FIFileXIf::GetIfInfoStatic()�Ȃ�)
		
		�e�I�u�W�F�N�g���w�肵�����ꍇ�́Cobjs�ɐݒ肷��D
		objs��NULL�̏ꍇ�C�V�[����FWSdk�̎q�I�u�W�F�N�g�Ƃ��ă��[�h�����D

		�����ŃC���|�[�g�����t�@�C���̃^�C�v�͕K���g���q��蔻�肳���D
	 */
	bool LoadScene(UTString filename, ImportIf* ex=NULL, const IfInfo* ii = NULL, ObjectIfs* objs = NULL);

	/** @brief �V�[�����t�@�C���փZ�[�u����
		@param filename �t�@�C����
		@param ex		�G�N�X�|�[�g���
		@param ii		�t�@�C���̎�ނ�����IfInfo
		@param objs		�ۑ�����I�u�W�F�N�g�̔z��
		�t�@�C���̎�ނ̎w��ɂ��Ă�LoadScene���Q�ƁD

		�ۑ�����I�u�W�F�N�g���w�肵�����ꍇ�́C������objs�ɐݒ肷��D
		objs��NULL�̏ꍇ�͑S�Ă�FWScene���ۑ������D

		�����I�ɕʃt�@�C���ɕۑ��������ꍇ�̓G�N�X�|�[�g����ex�Ɏw�肷��D
		ex��NULL�̏ꍇ�͑S�Ă�1�̃t�@�C���ɕۑ������D
	 */
	bool SaveScene(UTString filename, ImportIf* ex = NULL, const IfInfo* ii = NULL, ObjectIfs* objs = NULL);

	/// �V�[���̌����擾����
	int NScene() const;

	/** @brief �V�[����؂�ւ���
		@param scene
		�V�[��scene���A�N�e�B�u�ɂ���
	 */
	//void	SwitchScene(FWSceneIf* scene);

	/** @brief �V�[�����擾����
		@param index �擾����V�[���̃C���f�b�N�X
	 */
	FWSceneIf* GetScene(int index);

	/** @brief �V�[����������
		scene1�Ɋ܂܂��S�Ă̗v�f��scene0�Ɉړ����Cscene1���폜����D
	 */
	void	MergeScene(FWSceneIf* scene0, FWSceneIf* scene1);

	/// �C���^���N�V�����V�[�����쐬����
	FWInteractSceneIf*	CreateIAScene(const FWInteractSceneDesc& desc);
	/// �C���^���N�V�����V�[�����擾����
	FWInteractSceneIf*	GetIAScene(int index);
	/// �C���^���N�V�����V�[���̐�
	int		NIAScenes();
	/// �C���^���N�V�����V�[�����N���A����
	void	ClearIAScenes();

	/** @brief �����_�����쐬����
		�����_�����쐬���C�����_�����X�g�ɒǉ�����D
		
		��������Device��GL��D3D�����w��ł���悤�ɂ���ׂ�
	 */
	//GRRenderIf*	CreateRender();

	/** @brief �����_���̌����擾���� */
	//int NRender() const;
	
	/** @brief �����_�����擾���� */
	//GRRenderIf* GetRender(int index = -1);
	
	/** @brief �����_�����A�N�e�B�u�ɂ��� */
	//void SwitchRender(GRRenderIf* render);

	
	/// Physics SDK(PHSdk)�I�u�W�F�N�g���擾����
	PHSdkIf* GetPHSdk();

	/// Graphics SDK(GRSdk)�I�u�W�F�N�g���擾����
	GRSdkIf* GetGRSdk();

	/// FileIO SDK(FISdk)�I�u�W�F�N�g���擾����
	FISdkIf* GetFISdk();

	/// HumanInterface SDK(HISdk)�I�u�W�F�N�g���擾����
	HISdkIf* GetHISdk();

	/** @brief �f�o�b�O�`�惂�[�h�̎擾
	 */
	//bool GetDebugMode();

	/** @brief �f�o�b�O�`�惂�[�h�̐ݒ�
	 */
	//void SetDebugMode(bool debug = true);

	/** @brief �V�~�����[�V�����̎��s
	 */
	//void Step();

	/** @brief �`������s
		�A�N�e�B�u�ȃV�[���ƃA�N�e�B�u�ȃ����_�����g���ĕ`����s���D

		�f�o�b�O�`�惂�[�h�ł�Graphics�̃V�[���O���t�����g�킸�ɁC
		Physics�V�[���̍��̂����Փ˔���p�`��f�[�^��C�S���͂Ȃǂ̕`�悪�s����D
		
		�ʏ�`�惂�[�h�ł͂܂�Sync�֐��ɂ��Physics�V�[���̏�Ԃ�Graphics�V�[���O���t�ɔ��f���ꂽ��C
		�V�[���O���t�̌`��f�[�^��}�e���A���A�e�N�X�`���𗘗p���ĕ`�悪�s����B
	 */
	//void Draw();

	/** @brief �`��̈�̃T�C�Y��ݒ肷��
		@param w �`��̈�̉���
		@param h �`��̈�̏c��
		�A�N�e�B�u�ȃ����_���̕`��̈��ݒ肷��
	 */
	//void Reshape(int w, int h);

	/** @brief DSTR�o�͂̃f�o�b�O���̕\���̉ۂ�ݒ�
	 */
	//void SetDSTR(bool f);

	static void SPR_CDECL RegisterSdk();
};

}

#endif
