/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_CRAttentionListIf_H
#define SPR_CRAttentionListIf_H

#include <SprFoundation.h>
#include <SprPhysics.h>

namespace Spr{;

//@{

struct PHSolidIf;

// ------------------------------------------------------------------------------
/// ���ӑΏۂ�ێ�����\����
struct CRAttentionItem{
	/// ���ӑΏۂ̂ƂȂ鍄��
	PHSolidIf* solid;

	/// ���̃��[�J�����W�n�ɂ����钍�ӑΏۈʒu
	Vec3f      pos;

	/// ���ӂ������t����x��
	float      attractiveness;

	CRAttentionItem(){
		solid          = NULL;
		pos            = Vec3f(0,0,0);
		attractiveness = 0.0f;
	}
	CRAttentionItem(PHSolidIf* s, Vec3f p, float a){
		solid          = s;
		pos            = p;
		attractiveness = a;
	}
};

// ------------------------------------------------------------------------------
/// ���ӕ��̃��X�g�̃f�X�N���v�^
struct CRAttentionListDesc{

	CRAttentionListDesc(){
	}
};

/// ���ӕ��̃��X�g�̃C���^�[�t�F�[�X
struct CRAttentionListIf : SceneObjectIf{
	IF_DEF(CRAttentionList);

	/** @brief ���X�g���N���A����
	*/
	virtual void Clear()= 0;

	/** @brief ���ӑΏۃ��X�g���\�[�g����
	*/
	virtual void SortAttentionList();

	/** @brief ����solid�̍��Wpos�ɐݒ肳�ꂽ���ӂ��擾����
		@param solid ���́i���̂ł͂Ȃ��ꏊ�݂̂ɐݒ肳�ꂽ���ӂ̏ꍇNULL�j
		@param pos ���̂̃��[�J�����W�n�ɂ�������W
	*/
	virtual CRAttentionItem* GetAttentionItem(PHSolidIf* solid, Vec3f pos)= 0;

	/** @brief i�Ԗڂ�Attractiveness�̑傫�����ӂ��擾����
	*/
	virtual CRAttentionItem* GetAttentionItem(int i)= 0;

	/** @brief AttractiveList�ɒǉ����ꂽ�Ώۂ̐�
	*/
	virtual int NAttentionItems()= 0;
};

//@}

}

#endif//SPR_CRReachingMovementIf_H
