/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_FWSCENEIF_H
#define SPR_FWSCENEIF_H

#include <Foundation/SprScene.h>
#include <Physics/SprPHScene.h>
#include <Graphics/SprGRScene.h>
#include <Graphics/SprGRRender.h>
#include <Framework/SprFWBone.h>
#include <Framework/SprFWObject.h>

namespace Spr{

	struct FWObjectDesc;
	struct FWObjectIf;

	struct FWSceneDesc{
	};

	class HIForceDevice6D;

	struct FWSceneIf : SceneIf {
		SPR_IFDEF(FWScene);
		
		/// Physics���W���[���̃V�[�����擾����
		PHSceneIf*	GetPHScene();
		/** @brief Physics���W���[���̃V�[����ݒ肷��
			FWScene�̍쐬�Ɠ����Ɏ����I�ɍ쐬�����̂ŁA�ʏ�͌Ăяo���K�v�͖����D
		*/	
		void		SetPHScene(PHSceneIf* s);

		/// Graphics���W���[���̃V�[�����擾����
		GRSceneIf*	GetGRScene();
		/** @brief Graphics���W���[���̃V�[�����擾�E�ݒ肷��
			FWScene�̍쐬�Ɠ����Ɏ����I�ɍ쐬�����̂ŁA�ʏ�͌Ăяo���K�v�͖����D
		*/
		void		SetGRScene(GRSceneIf* s);

		/** @brief ���Framework�I�u�W�F�N�g���쐬����
		 */
		FWObjectIf*	CreateFWObject();
		int NObject()const;
		FWObjectIf** GetObjects();

		/** @brief �I�u�W�F�N�g�𓯊�����
			�eFWObject�ɑ΂��āAPHSolid�̈ʒu��GRFrame�ɔ��f������B
			�ʏ��FWSceneIf::Step�ɂ���ăV�~�����[�V���������s���ꂽ��ɌĂԁB
		 */
		void Sync();

		/** @brief �V�~�����[�V���������s����
		 */
		void Step();

		/** @brief �`�悷��
			@param grRender	�����_��
			@param debug	�f�o�b�O�`�悩
			debug��true�̏ꍇ�i�f�o�b�O�`��j�A�e���̂̏Փ˔���p�`��f�[�^�ɂ��
			�`�悪�s����B����Adebug��false�̏ꍇ�̓V�[���O���t�̌`��f�[�^��
			�}�e���A���A�e�N�X�`���𗘗p���ĕ`�悪�s����B
		 */
		void Draw(GRRenderIf* grRender, bool debug=false);

		void AddHumanInterface(HIForceDevice6D* d);

		//�{�[���̐ݒ�
		void SetFWBones(FWBoneIf* b);
		/** @brief �쐬����FWBone���V�[���ɕۑ�����
		 */
		std::vector< UTRef<FWBoneIf> > GetFWBones();

		/** @brief �{�[���̏W���̂��쐬����
		 */
		void CreateFWStructure();
		/** @brief �{�[���̏W���̂Ƀ{�[����ǉ�����
		 */
		void AddFWStructure(FWStructureIf* o);
		/** @brief �Ō�ɍ쐬�����{�[���̏W���̂��擾����
		 */
		FWStructureIf* GetFWStructure();
		
		/** @brief n�Ԗڂ̃{�[���̏W���̂��擾����
		 */
		FWStructureIf* GetFWStructure(int n);

		/** @brief �{�[���̏W���̂̌����擾����
		 */
		size_t NFWStructure();


	};

	FWSceneIf* SPR_CDECL CreateFWScene();
	FWSceneIf* SPR_CDECL CreateFWScene(const void* desc);
}

#endif
