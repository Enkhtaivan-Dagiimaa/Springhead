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
	/// �{�[���̓������Έʒu�ōs���i�e�t���[������̍����łȂ����[���h���W�n�ɑ΂���ϊ����Z�b�g����j
	bool bAbsolute;

	enum FWObjectSyncSource {
		PHYSICS=0,  // Physics�̏�Ԃ�Graphics�����������
		GRAPHICS    // Graphics�̏�Ԃ�Physics�����������
	} syncSource;

	FWObjectDesc() {
		bAbsolute  = false;
		syncSource = PHYSICS;
	}
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

	/// PHJoint���擾����
	PHJointIf* GetPHJoint();
	/// PHJoint��ݒ肷��
	void SetPHJoint(PHJointIf* j);

	/// �Z�b�g���ꂽ�qFrame���擾����
	GRFrameIf* GetChildFrame();
	/// �qFrame���Z�b�g����i��Frame�ƎqFrame�Ԃ̋������킩��̂�Solid�̑傫���������ݒ�\�ɂȂ�j
	void SetChildFrame(GRFrameIf* f);

	/// PHSolid,GRFrame�̂�����̈ʒu�ɍ��킹�邩��ݒ肷��
	void SetSyncSource(FWObjectDesc::FWObjectSyncSource syncSrc);
	/// PHSolid,GRFrame�̂�����̈ʒu�ɍ��킹�邩���擾����
	FWObjectDesc::FWObjectSyncSource GetSyncSource();

	/// �{�[�����Έʒu�w��œ������邩��ݒ肷��
	void EnableAbsolute(bool bAbs);
	/// �{�[�����Έʒu�w��œ������邩���擾����
	bool IsAbsolute();

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
	void Sync();
};

struct FWObjectGroupDesc {
};
/** @brief �{�[�����W���̂Ƃ��ĊǗ����邽�߂�Framework�I�u�W�F�N�g
 */
struct FWObjectGroupIf : SceneObjectIf {
	SPR_IFDEF(FWObjectGroup);

	/// FWObject���擾����
	FWObjectIf* GetObject(int n);

	/// FWObject�̐���Ԃ�
	int NObjects();

	// FWObject�����x���ŒT��
	FWObjectIf* FindByLabel(const char* name);
};

}

#endif
