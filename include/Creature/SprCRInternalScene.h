/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_CRInternalSceneIf_H
#define SPR_CRInternalSceneIf_H

#include <SprFoundation.h>
#include <SprPhysics.h>

namespace Spr{;

//@{

struct PHSolidIf;

// ------------------------------------------------------------------------------
/// �����V�[�����\�����镨�̂̃C���^�t�F�[�X
struct CRInternalSceneObjectIf : SceneObjectIf{
	SPR_IFDEF(CRInternalSceneObject);

	/** @brief �w�肵��type�����肷��
	*/
	bool IsA(const char* typestr);

	/** @brief ��ނ�Ԃ�
	*/
	const char* GetISObjType();

	/** @brief ���̂�Ԃ�
	*/
	PHSolidIf* GetSolid();

	/** @brief �ʒu��Ԃ�
	*/
	Vec3f GetPos();

	/** @brief �ʒu��ݒ肷��
	*/
	void SetPos(Vec3d pos);
};

/// �����V�[�����\�����镨�̂̃f�X�N���v�^
struct CRInternalSceneObjectDesc{
	SPR_DESCDEF(CRInternalSceneObject);

	/// ���
	char* type;

	/// �ʒu�̊�ƂȂ鍄��
	PHSolidIf* solid;

	/// ���̃��[�J�����W�n�ɂ�����ʒu
	Vec3f position;

	CRInternalSceneObjectDesc(){
		type           = NULL;
		solid          = NULL;
		position       = Vec3f(0,0,0);
	}
};

// ------------------------------------------------------------------------------
/// ���ӂ��Ђ����镨�̂̃C���^�t�F�[�X
struct CRISAttractiveObjectIf : CRInternalSceneObjectIf{
	SPR_IFDEF(CRISAttractiveObject);

	/** @brief �����I�ɂ��̕��̂����ӂ��Ђ�����x���𓾂�
	*/
	float GetTotalAttractiveness();

	/** @brief �{�g���A�b�v���ӂ̓x�������Z����
	*/
	void AddBottomupAttr(float attr);

	/** @brief �{�g���A�b�v���ӂ̓x�������Z�b�g����
	*/
	void ClearBottomupAttr();

	/** @brief �g�b�v�_�E�����ӂ̓x����ݒ肷��
	*/
	void SetTopdownAttr(float attr);

	/** @brief �s�m�����𑝂�
	*/
	void IncUncertainty();

	/** @brief �s�m���������炷
	*/
	void DecUncertainty();

	/** @brief �s�m�������擾����
	*/
	float GetUncertainty();
};

/// ���ӂ��Ђ����镨�̂̃f�X�N���v�^
struct CRISAttractiveObjectDesc : CRInternalSceneObjectDesc{
	SPR_DESCDEF(CRISAttractiveObject);

	/// �{�g���A�b�v���̒��ӂ̓x��
	float bottomupAttr;

	/// �g�b�v�_�E�����̒��ӂƂ��Ċ��蓖�Ă�ꂽ�l
	float topdownAttr;

	/// ���̕��̂��s�m���Ȏ��ɔ�����Attention
	float unknownAttr;

	/// �s�m�������x���i���̂�������ƌ������C�����Ȃ��Ƒ�������C0.0�`1.0�̒l�j
	float uncertainty;

	/// �s�m�������x���̑�����
	float uncertaintyIncRate;

	/// �s�m�������x���̌�����
	float uncertaintyDecRate;

	CRISAttractiveObjectDesc(){
		bottomupAttr = 0.0f;
		topdownAttr  = 0.0f;
		unknownAttr  = 1.0f;
		uncertainty  = 1.0f;
		uncertaintyIncRate = 0.99f;
		uncertaintyDecRate = 0.2f;
	}
};

// ------------------------------------------------------------------------------
/// ���s�̃|�e���V��������ɂ�����镨�̂̃C���^�t�F�[�X
struct CRISTravelPotentialObjectIf : CRInternalSceneObjectIf{
	SPR_IFDEF(CRISTravelPotentialObject);

	/*
	** ����r�ɂ����郌�i�[�h�E�W���[���Y�|�e���V����
	**     U = -A / (r^n) + B / (r^m)
	*/

	/** @brief �|�e���V�����̋����̌W���iA, B�j�𓾂�
	*/
	Vec2f GetStrengthCoeff();

	/** @brief �|�e���V�����̋����̌W���iA, B�j��ݒ肷��
	*/
	void SetStrengthCoeff(Vec2f strength);

	/** @brief �|�e���V�����̌����̌W���in, m�j�𓾂�
	*/
	Vec2f GetDecayCoeff();

	/** @brief �|�e���V�����̌����̌W���in, m�j��ݒ肷��
	*/
	void SetDecayCoeff(Vec2f decay);

	/** @brief ����ʒu�ɑΉ�����|�e���V�������v�Z����
	*/
	Vec2f GetPotential(Vec2f currPos);
};

/// ���s�̃|�e���V��������ɂ�����镨�̂̃f�X�N���v�^
struct CRISTravelPotentialObjectDesc : CRInternalSceneObjectDesc{
	SPR_DESCDEF(CRISTravelPotentialObject);

	/// �����̌v��
	Vec2f  strength;
	/// �����̌v��
	Vec2f  decay;

	CRISTravelPotentialObjectDesc(){
		strength = Vec2f(0.0f, 0.0f);
		decay    = Vec2f(0.0f, 0.0f);
	}
};

// ------------------------------------------------------------------------------
/// �����V�[���̃C���^�[�t�F�[�X
struct CRInternalSceneIf : SceneObjectIf{
	SPR_IFDEF(CRInternalScene);

	/** @brief �X�e�b�v���Ƃ̏��������s����
	*/
	void Step();

	/** @brief Attractiveness���N���A����i�{����Bottomup�����j
	*/
	void ClearAttractiveness();

	/** @brief Attractiveness�̑傫�����Ƀ\�[�g����
	*/
	void SortByAttractiveness();

	/** @brief ����solid�̍��Wpos�ɐݒ肳�ꂽ���ӂ��擾����
		@param solid ���́i���̂ł͂Ȃ��ꏊ�݂̂ɐݒ肳�ꂽ���ӂ̏ꍇNULL�j
		@param pos ���̂̃��[�J�����W�n�ɂ�������W
	*/
	CRInternalSceneObjectIf* FindObject(PHSolidIf* solid, Vec3f pos);

	/** @brief i�Ԗڂ̕��̂��擾����iSort��ɗp�����Sort���ꂽ�����Ŏ擾�ł���j
	*/
	CRInternalSceneObjectIf* GetISObject(int i);

	/** @brief ���̂̐�
	*/
	int NObjects();

	/** @brief �I�u�W�F�N�g���쐬����
	*/
	CRInternalSceneObjectIf* CreateInternalSceneObject(const IfInfo* ii, const CRInternalSceneObjectDesc& desc);
	template <class T> CRInternalSceneObjectIf* CreateInternalSceneObject(const T& desc){
		return CreateInternalSceneObject(T::GetIfInfo(), desc);
	}
};

/// �����V�[���̃f�X�N���v�^
struct CRInternalSceneDesc{
	SPR_DESCDEF(CRInternalScene);

	CRInternalSceneDesc(){
	}
};

//@}

}

#endif//SPR_CRReachingMovementIf_H
