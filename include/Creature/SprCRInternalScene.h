/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
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
	IF_DEF(CRInternalSceneObject);

	/** @brief �w�肵��type�����肷��
	*/
	virtual bool IsA(const char* typestr)= 0;

	/** @brief ��ނ�Ԃ�
	*/
	virtual const char* GetISObjType()= 0;

	/** @brief ���̂�Ԃ�
	*/
	virtual PHSolidIf* GetSolid()= 0;

	/** @brief �ʒu��Ԃ�
	*/
	virtual Vec3f GetPos()= 0;

	/** @brief �ʒu��ݒ肷��
	*/
	virtual void SetPos(Vec3d pos)= 0;
};

/// �����V�[�����\�����镨�̂̃f�X�N���v�^
struct CRInternalSceneObjectDesc{
	DESC_DEF_FOR_OBJECT(CRInternalSceneObject);

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
	IF_DEF(CRISAttractiveObject);

	/** @brief �����I�ɂ��̕��̂����ӂ��Ђ�����x���𓾂�
	*/
	virtual float GetTotalAttractiveness()= 0;

	/** @brief �{�g���A�b�v���ӂ̓x�������Z����
	*/
	virtual void AddBottomupAttr(float attr)= 0;

	/** @brief �{�g���A�b�v���ӂ̓x�������Z�b�g����
	*/
	virtual void ClearBottomupAttr()= 0;

	/** @brief �g�b�v�_�E�����ӂ̓x����ݒ肷��
	*/
	virtual void SetTopdownAttr(float attr)= 0;

	/** @brief �s�m�����𑝂�
	*/
	virtual void IncUncertainty()= 0;

	/** @brief �s�m���������炷
	*/
	virtual void DecUncertainty()= 0;

	/** @brief �s�m�������擾����
	*/
	virtual float GetUncertainty()= 0;
};

/// ���ӂ��Ђ����镨�̂̃f�X�N���v�^
struct CRISAttractiveObjectDesc : CRInternalSceneObjectDesc{
	DESC_DEF_FOR_OBJECT(CRISAttractiveObject);

	/// �{�g���A�b�v���̒��ӂ̓x��
	float bottomupAttr;

	/// �g�b�v�_�E�����̒��ӂƂ��Ċ��蓖�Ă�ꂽ�l
	float topdownAttr;

	/// �s�m�������x���i���̂�������ƌ������C�����Ȃ��Ƒ�������C0.0�`1.0�̒l�j
	float uncertainty;

	/// �s�m�������x���̑�����
	float uncertaintyIncRate;

	/// �s�m�������x���̌�����
	float uncertaintyDecRate;

	CRISAttractiveObjectDesc(){
		bottomupAttr = 0.0f;
		topdownAttr  = 0.0f;
		uncertainty  = 1.0f;
		uncertaintyIncRate = 0.99f;
		uncertaintyDecRate = 0.9f;
	}
};

// ------------------------------------------------------------------------------
/// ���s�̃|�e���V��������ɂ�����镨�̂̃C���^�t�F�[�X
struct CRISTravelPotentialObjectIf : CRInternalSceneObjectIf{
	IF_DEF(CRISTravelPotentialObject);

	/*
	** ����r�ɂ����郌�i�[�h�E�W���[���Y�|�e���V����
	**     U = -A / (r^n) + B / (r^m)
	*/

	/** @brief �|�e���V�����̋����̌W���iA, B�j�𓾂�
	*/
	virtual Vec2f GetStrengthCoeff()= 0;

	/** @brief �|�e���V�����̋����̌W���iA, B�j��ݒ肷��
	*/
	virtual void SetStrengthCoeff(Vec2f strength)= 0;

	/** @brief �|�e���V�����̌����̌W���in, m�j�𓾂�
	*/
	virtual Vec2f GetDecayCoeff()= 0;

	/** @brief �|�e���V�����̌����̌W���in, m�j��ݒ肷��
	*/
	virtual void SetDecayCoeff(Vec2f decay)= 0;

	/** @brief ����ʒu�ɑΉ�����|�e���V�������v�Z����
	*/
	virtual Vec2f GetPotential(Vec2f currPos)= 0;
};

/// ���s�̃|�e���V��������ɂ�����镨�̂̃f�X�N���v�^
struct CRISTravelPotentialObjectDesc : CRInternalSceneObjectDesc{
	DESC_DEF_FOR_OBJECT(CRISTravelPotentialObject);

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
	IF_DEF(CRInternalScene);

	/** @brief �X�e�b�v���Ƃ̏��������s����
	*/
	virtual void Step()= 0;

	/** @brief Attractiveness���N���A����i�{����Bottomup�����j
	*/
	virtual void ClearAttractiveness()= 0;

	/** @brief Attractiveness�̑傫�����Ƀ\�[�g����
	*/
	virtual void SortByAttractiveness()= 0;

	/** @brief ����solid�̍��Wpos�ɐݒ肳�ꂽ���ӂ��擾����
		@param solid ���́i���̂ł͂Ȃ��ꏊ�݂̂ɐݒ肳�ꂽ���ӂ̏ꍇNULL�j
		@param pos ���̂̃��[�J�����W�n�ɂ�������W
	*/
	virtual CRInternalSceneObjectIf* FindObject(PHSolidIf* solid, Vec3f pos)= 0;

	/** @brief i�Ԗڂ̕��̂��擾����iSort��ɗp�����Sort���ꂽ�����Ŏ擾�ł���j
	*/
	virtual CRInternalSceneObjectIf* GetISObject(int i)= 0;

	/** @brief ���̂̐�
	*/
	virtual int NObjects()= 0;

	/** @brief �I�u�W�F�N�g���쐬����
	*/
	virtual CRInternalSceneObjectIf* CreateInternalSceneObject(const IfInfo* ii, const CRInternalSceneObjectDesc& desc)= 0;
	template <class T> CRInternalSceneObjectIf* CreateInternalSceneObject(const T& desc){
		return CreateInternalSceneObject(T::GetIfInfo(), desc);
	}

	/** @brief ���̂̂킩��Ȃ����̂��f�t�H���g�Ŏ��g�b�v�_�E�����ӂ̗ʂ�Ԃ�
	*/
	virtual float GetDefaultTopdownAttr()= 0;
};

/// �����V�[���̃f�X�N���v�^
struct CRInternalSceneDesc{
	DESC_DEF_FOR_OBJECT(CRInternalScene);

	/// Uncertainty��1�̕��̂�����TopDownAttention�̗�
	float defaultTopdownAttr;

	CRInternalSceneDesc(){
		defaultTopdownAttr = 0.0f;
	}
};

//@}

}

#endif//SPR_CRReachingMovementIf_H
