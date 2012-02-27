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

namespace Spr{;

struct CDShapeIf;
struct PHSolidIf;
struct PHConstraintIf;
struct PHContactPointIf;
struct PHIKEngineIf;
struct PHHapticEngineIf;
struct PHSceneIf;
struct GRSceneIf;
struct GRRenderIf;
struct FWObjectDesc;
struct FWObjectIf;
struct FWBoneIf;
struct FWStructureIf;

struct FWSceneDesc{
};

class HIForceDevice6D;

struct FWSceneIf : SceneIf {
	SPR_IFDEF(FWScene);

	enum AxisStyle{
		AXIS_LINES,
		AXIS_ARROWS,
	};

public:
		
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
		�쐬���ꂽ���_��PHSolid��GRFrame�͊��蓖�Ă��Ă��Ȃ��̂ŁC
		SetPHSolid, SetGRFrame�Ŋ��蓖�Ă�K�v������D
	 */
	FWObjectIf*	CreateFWObject();

	/// �I�u�W�F�N�g�̐����擾����
	int NObject()const;

	/// �I�u�W�F�N�g�̔z����擾����
	FWObjectIf** GetObjects();

	/** @brief �I�u�W�F�N�g�𓯊�����
		�eFWObject�ɑ΂��āAPHSolid�̈ʒu��GRFrame�ɔ��f������B
		�ʏ��FWSceneIf::Step�ɂ���ăV�~�����[�V���������s���ꂽ��ɌĂԁB
	 */
	void Sync(bool ph_to_gr = true);

	/** @brief �V�~�����[�V���������s����
	 */
	void Step();

	/** @brief �`�悷��
		@param grRender	�����_��
		@param debug	�f�o�b�O�`�悩

	 */
	void Draw(GRRenderIf* grRender, bool debug);

	/** @brief	Physics�V�[���������_�����O����
		@param	render	�����_��
		@param  scene	�V�[��
		�V�[���Ɋ܂܂��I�u�W�F�N�g�i���́C�S���C�ڐG�CIK�Ȃǁj�����ׂĕ`�悷��D
	 */
	void DrawPHScene(GRRenderIf* render);

	/** @brief ���̂������_�����O����
	    @param	render			�����_��
		@param	solid			����
		@param	solid_or_wire
		�w�肵�����̂̏��i���W���C�Փ˔���`��C�́C���[�����g�j��`�悷��
	 */
	void DrawSolid(GRRenderIf* render, PHSolidIf* solid, bool solid_or_wire);

	/** @brief �Փ˔���p�`���`�悷��
		@param	render	�����_��
		@param	shape	�`��
		@param	solid	true�Ȃ�\���b�h�`��Cfalse�Ȃ烏�C���t���[���`��
		shape�̎�ނ𔻕ʂ��Č`���`�悷��D
	 */
	void DrawShape(GRRenderIf* render, CDShapeIf* shape, bool solid);

	/** @brief �S����`�悷��
		@param	render	�����_��
		@param	con		�S��
		�w�肵���S���̏��i�\�P�b�g�ƃv���O�̍��W���C�S���́C���[�����g�j��`�悷��
	 */
	void DrawConstraint(GRRenderIf* render, PHConstraintIf* con);

	/**	@brief �ڐG��`�悷��
		@param	render	�����_��
		@param	con		�ڐG
		�w�肵���ڐG�̐ڐG�f�ʂ�`�悷��
	 */
	void DrawContact(GRRenderIf* render, PHContactPointIf* con);

	/**	@brief	IK����`�悷��
		@param	render		�����_��
		@param	ikEngine	IK�G���W��
	 */
	void DrawIK(GRRenderIf* render, PHIKEngineIf* ikEngine);

	/** @brief �֐߉����`�悷��
		@param	render	�����_��
		@param	con		�֐�
	 */
	void DrawLimit(GRRenderIf* render, PHConstraintIf* con);

	/**	@brief	Haptic����`�悷��
		@param	render			�����_��
		@param	hapticEngine	Haptic�G���W��
	 */
	void DrawHaptic(GRRenderIf* render, PHHapticEngineIf* hapticEngine);

	/**	@brief �`�惂�[�h�̐ݒ�
		@param ph_or_gr	PHScene��`�悷�邩GRScene��`�悷�邩
		@param solid	�\���b�h�`��̃I��/�I�t
		@param wire		���C���t���[���`��̃I��/�I�t
		solid��wire��PHSceen��`�悷��Ƃ��̂ݗL���D
	 */
	void SetRenderMode(bool solid = true, bool wire = false);

	/** @brief	�I�u�W�F�N�g�P�ʂ̕`��̃I��/�I�t
	 */
	void EnableRender(ObjectIf* obj, bool enable = true);
	
	/** @brief �`��}�e���A���̐ݒ�
		@param	mat			�}�e���A��
		@param	solid		�ݒ�Ώۂ̍���
		�f�o�b�O�`��ɂ�����}�e���A����ݒ肷��D
		mat�ɂ�GRRenderIf::TMaterialSample�̒l���w��ł���D
		solid��NULL���w�肷��ƑS�Ă̍��̂��ΏۂƂȂ�D
	 */
	void SetSolidMaterial(int mat, PHSolidIf* solid = 0);
	void SetWireMaterial (int mat, PHSolidIf* solid = 0);

	/// ���W���̕`��؂�ւ�
	void EnableRenderAxis(bool world = true, bool solid = true, bool con = true);
	/// ���W���̕`��I�v�V����
	void SetAxisMaterial(int matX, int matY, int matZ);
	void SetAxisScale(float scaleWorld, float scaleSolid, float scaleCon);
	void SetAxisStyle(int style);

	/// �͂̕`��؂�ւ�
	void EnableRenderForce(bool solid = true, bool constraint = true);
	/// �͂̕`��I�v�V����
	void SetForceMaterial(int matForce, int matMoment);
	void SetForceScale(float scaleForce, float scaleMoment);

	/**  @brief �ڐG�ʂ̕`�� */
	void EnableRenderContact(bool enable = true);
	void SetContactMaterial(int mat);

	/**  @brief �O���b�h�̕`�� */
	void EnableRenderGrid(bool x = true, bool y = true, bool z = true);
	/** @brief	�O���b�h�̐ݒ�
		@param	axis	�ݒ肷�鎲��'x', 'y', 'z'�Ŏw��
		@param	offset	�I�t�Z�b�g
		@param	size	�O���b�h�̕�
		@param	slice	������
	 */
	void SetGridOption(char axis, float offset, float size, int slice);
	void SetGridMaterial(int matX, int matY, int matZ);

	/**  @brief IK�̌v�Z���ʂ̕`�� */
	void EnableRenderIK(bool enable = true);
	void SetIKMaterial(int mat);
	void SetIKScale(float scale);

	/**  @brief �֐߉���̕`�� */
	void EnableRenderLimit(bool enable = true);

	/** @brief Haptic�̌v�Z���ʂ̕`�� */
	void EnableRenderHaptic(bool enable = true);
	
	void AddHumanInterface(HIForceDevice6D* d);

	/// �{�[���̐ݒ�
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
