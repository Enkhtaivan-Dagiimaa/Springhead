/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_FWOBJECTIF_H
#define SPR_FWOBJECTIF_H

#include <Foundation/SprScene.h>
#include <Physics/SprPHSolid.h>
#include <Graphics/SprGRFrame.h>

namespace Spr{

	struct FWSceneIf;

	struct FWObjectDesc{
	};

	/** @brief Framework�̃I�u�W�F�N�g
		���́iPhysics���W���[����PHSolid�j�ƃV�[���O���t�̃t���[���iGraphics���W���[����GRFrame�j��
		�֘A�t����I�u�W�F�N�g�D
		����ɂ���ăV�~�����[�V�����ɂ�鍄�̂̈ʒu�̕ω����V�[���O���t�Ɏ����I�ɔ��f�����D
		Framework�𗘗p����ꍇ��PHSceneIf::CreateSolid��GRSceneIf::CreateFrame�̑����
		FWSceneIf::CreateObject���g�p����D
	 */
	struct FWObjectIf : SceneObjectIf {
		IF_DEF(FWObject);
		/// PHSolid���擾����
		virtual PHSolidIf* GetPHSolid() = 0;
		/// PHSolid��ݒ肷��
		virtual void SetPHSolid(PHSolidIf* s) = 0;
		/// GRFrame���擾����
		virtual GRFrameIf* GetGRFrame() = 0;
		/// GRFrame��ݒ肷��
		virtual void SetGRFrame(GRFrameIf* f) = 0;
	};

}

#endif
