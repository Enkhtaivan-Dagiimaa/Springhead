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

	/** @brief ���̂�Ԃ�
	*/
	virtual PHSolidIf* GetSolid()= 0;

	/** @brief �ʒu��Ԃ�
	*/
	virtual Vec3f GetPos()= 0;
};

/// �����V�[�����\�����镨�̂̃f�X�N���v�^
struct CRInternalSceneObjectDesc{
	DESC_DEF_FOR_OBJECT(CRInternalSceneObject);

	/// �ʒu�̊�ƂȂ鍄��
	PHSolidIf* solid;

	/// ���̃��[�J�����W�n�ɂ�����ʒu
	Vec3f position;

	CRInternalSceneObjectDesc(){
		solid          = NULL;
		position       = Vec3f(0,0,0);
	}
};

// ------------------------------------------------------------------------------
/// ���ӂ��Ђ����镨�̂̃C���^�t�F�[�X
struct CRISAttractiveObjectIf : CRInternalSceneObjectIf{
	IF_DEF(CRISAttractiveObject);

	/** @brief ���ӂ��Ђ�����x���𓾂�
	*/
	virtual float GetAttractiveness()= 0;

	/** @brief ���ӂ��Ђ�����x����ݒ肷��
	*/
	virtual void SetAttractiveness(float attractiveness)= 0;
};

/// ���ӂ��Ђ����镨�̂̃f�X�N���v�^
struct CRISAttractiveObjectDesc : CRInternalSceneObjectDesc{
	DESC_DEF_FOR_OBJECT(CRISAttractiveObject);

	/// ���ӂ������t����x��
	float  attractiveness;

	CRISAttractiveObjectDesc(){
		attractiveness = 0.0f;
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
	virtual CRInternalSceneObjectIf* GetObject(int i)= 0;

	/** @brief ���̂̐�
	*/
	virtual int NObjects()= 0;

	/** @brief �I�u�W�F�N�g���쐬����
	*/
	virtual CRInternalSceneObjectIf* CreateInternalSceneObject(const IfInfo* ii, const CRInternalSceneObjectDesc& desc)= 0;
	template <class T> CRInternalSceneObjectIf* CreateInternalSceneObject(const T& desc){
		return CreateInternalSceneObject(T::GetIfInfo(), desc);
	}
};

/// �����V�[���̃f�X�N���v�^
struct CRInternalSceneDesc{
	DESC_DEF_FOR_OBJECT(CRInternalScene);

	CRInternalSceneDesc(){
	}
};

//@}

}

#endif//SPR_CRReachingMovementIf_H
