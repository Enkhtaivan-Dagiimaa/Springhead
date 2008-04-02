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

		/** @brief Framework�I�u�W�F�N�g���쐬����
			PHSolid��GRFrame���֘A�t����I�u�W�F�N�g���쐬����D
			�����I�ɂ́A�܂�desc�ɂ��ƂÂ��ĐV����PHSolid�����GRFrame���A���ꂼ��
			PHSceneIf::CreateSolid�����GRSceneIf::CreateFrame�ɂ��쐬�����B
			���ɗ��҂��Q�Ƃ���FWObject���쐬����FWScene�ɓo�^�����B
		 */
		FWObjectIf*	CreateObject(const PHSolidDesc& soliddesc = PHSolidDesc(), const GRFrameDesc& framedesc = GRFrameDesc());
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
	};

	FWSceneIf* SPR_CDECL CreateFWScene();
	FWSceneIf* SPR_CDECL CreateFWScene(const void* desc);
}

#endif
