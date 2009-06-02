/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_FWBONEIF_H
#define SPR_FWBONEIF_H

#include <Foundation/SprScene.h>
#include <Physics/SprPHSolid.h>
#include <Graphics/SprGRFrame.h>
#include <Base/Affine.h>
namespace Spr{

	struct FWSceneIf;

	struct FWBoneDesc{
	};

	/** @brief Framework�̃I�u�W�F�N�g
		�������������{�[���̍��́iPhysics���W���[����PHSolid�j�ƃV�[���O���t�̃t���[���iGraphics���W���[����GRFrame�j��
		�֘A�t����I�u�W�F�N�g�D
		����ɂ���ăV�~�����[�V�����ɂ�鍄�̂̈ʒu�̕ω����V�[���O���t�Ɏ����I�ɔ��f�����D
		Framework�𗘗p����ꍇ��PHSceneIf::CreateSolid��GRSceneIf::CreateFrame�̑����
		FWSceneIf::CreateObject���g�p����D
	 */
	struct FWBoneIf : SceneObjectIf {
		SPR_IFDEF(FWBone);

		/// PHSolid���擾����
		PHSolidIf* GetPHSolid();
		/// PHSolid��ݒ肷��
		void SetPHSolid(PHSolidIf* s);
		/// GRFrame���擾����
		GRFrameIf* GetGRFrame();
		/// GRFrame��ݒ肷��
		void SetGRFrame(GRFrameIf* f);
		/// CDRoundConeIf���擾����
		CDRoundConeIf* GetShape();
		/// CDRoundConeIf��Radius��ݒ肷��
		void		   SetShapeRadius(double r1,double r2);
		///�@joint���擾����
		PHJointIf*	   GetJoint();
		///  joint��K,D,D2��ݒ肷��
		void		   SetJointKDD2(double K,double D, double D2);
	};

}

#endif
