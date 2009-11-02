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
#include <Base/Affine.h>
namespace Spr{;

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
	SPR_IFDEF(FWObject);
	/// PHSolid���擾����
	PHSolidIf* GetPHSolid();
	/// PHSolid��ݒ肷��
	void SetPHSolid(PHSolidIf* s);
	/// GRFrame���擾����
	GRFrameIf* GetGRFrame();
	/// GRFrame��ݒ肷��
	void SetGRFrame(GRFrameIf* f);

	///�{�[���t��X�t�@�C����p
	/// solidLength���擾����
	double GetSolidLength();
	/// solidLength��ݒ肷��
	void SetSolidLength(double l);
};

struct FWBoneObjectDesc {
	bool modifyShapeLength;		///< ���̂�Shape�̒�����Bone�ɂ��킹�ĕύX����
	bool modifyShapeWidth;		///< ���̂�Shape�̑������X�L�����b�V���ɍ��킹�ĕύX����i�������j
	bool modifyShapePose;		///< ���̂ɑ΂���Shape�̈ʒu��(0,0,0)�ɕύX����ifalse�ɂ���Ƃ��Ƃ��Ƃ�shapePose���ێ�����j
	bool modifyPlugSocketPose;	///< �֐߂̎��t���ʒu��Bone�ɂ��킹�ĕύX����
};

struct FWStructureDesc {
};

/** @brief �{�[�����f���ƍ��̊֐ߌn���Ȃ��邽�߂�Framework�I�u�W�F�N�g
 */
struct FWBoneObjectIf : FWObjectIf {
	SPR_IFDEF(FWBoneObject);

	/// PHJoint���擾����
	PHJointIf* GetPHJoint();
	/// PHJoint��ݒ肷��
	void SetPHJoint(PHJointIf* j);

	/// �{�[���̏I�[�_������GRFrame���擾����
	GRFrameIf* GetEndFrame();
	/// �{�[���̏I�[�_������GRFrame��ݒ肷��
	void SetEndFrame(GRFrameIf* f);
};

/** @brief �{�[�����W���̂Ƃ��ĊǗ����邽�߂�Framework�I�u�W�F�N�g
 */
struct FWStructureIf : SceneObjectIf {
	SPR_IFDEF(FWStructure);

	/// BoneObject���i���o�[�Ŏw�肵�Ď擾����
	FWBoneObjectIf* GetBone(int n);
	/// BoneObject�𖼑O�Ŏw�肵�Ď擾����
	FWBoneObjectIf* GetBone(char* n);
	/// BoneObject�̃T�C�Y���擾����
	int GetBoneSize();
	/// FWBoneObject��ǉ�����
	void AddBone(FWBoneObjectIf* o);
};

}

#endif
