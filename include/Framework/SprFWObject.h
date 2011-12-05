/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_FWOBJECTIF_H
#define SPR_FWOBJECTIF_H

#include <Foundation/SprObject.h>
#include <Collision/SprCDShape.h>

namespace Spr{;

struct CDConvexMeshIf;
struct PHSolidIf;
struct PHJointIf;
struct GRMeshIf;
struct GRFrameIf;
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
	/// PHFEMMesh���擾����

	/** @brief �O���t�B�N�X�p���b�V�������[�h����w���p�֐�
		@param filename �t�@�C����
		@param ii		�t�@�C���^�C�v���w�肷��ꍇ��IfInfo
		@param frame	���b�V����ۗL����t���[���DNULL���w�肷��ƃ��[�g�t���[�����ۗL����
		@return ���[�h�ɐ���������true��Ԃ��D���s������false��Ԃ��D

		���b�V�������[�h���C����FWObject��GRFrame�̉��ɉ�����
	 */
	bool LoadMesh(const char* filename, const IfInfo* ii = NULL, GRFrameIf* frame = NULL);

	/** @brief �O���t�B�N�X�p���b�V������Փ˔���p���b�V����������������
		@param frame	�ϊ��Ώۂ̃��b�V����ۗL����GRFrame�DNULL���w�肷��ƃ��[�g�t���[�����ΏۂƂȂ�
		@param mat		�����l
		�I�u�W�F�N�g���ێ�����O���t�B�N�X���b�V���̓ʕ�Ƃ��ďՓ˔��胁�b�V���𐶐�����
	 */
	void GenerateCDMesh(GRFrameIf* frame = NULL, const PHMaterial& mat = PHMaterial());

	/// PHSolid��GRFrame�̓���
	void Sync(bool ph_to_gr);

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

	enum FWBoneObjectAdaptType{
		GRFRAME_TO_PHSOLID = 0, //PHSolid�̈ʒu��GRFrame�����킹��
		PHSOLID_TO_GRFRAME = 1	//GRFrame�̈ʒu��PHSolid�����킹��
	}AdaptType; 
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

	/// PHSolid,GRFrame�̂�����̈ʒu�ɍ��킹�邩��ݒ肷��
	void SetAdaptType(int t);
};

struct FWStructureDesc {
};
/** @brief �{�[�����W���̂Ƃ��ĊǗ����邽�߂�Framework�I�u�W�F�N�g
 */
struct FWStructureIf : SceneObjectIf {
	SPR_IFDEF(FWStructure);

	/// BoneObject���i���o�[�Ŏw�肵�Ď擾����
	FWBoneObjectIf* GetBone(int n);
	/// BoneObject�𖼑O�Ŏw�肵�Ď擾����
	FWBoneObjectIf* GetBone(const char* n);
	/// BoneObject�̃T�C�Y���擾����
	int GetBoneSize();
	/// FWBoneObject�̈ʒu��ݒ肷��
	void SetPose(Posed p);	
	/// FWBoneObject��ǉ�����
	void AddBone(FWBoneObjectIf* o);
};

}

#endif
