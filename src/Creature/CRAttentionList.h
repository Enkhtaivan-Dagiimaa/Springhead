/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRATTENTIONLIST_H
#define CRATTENTIONLIST_H

#include <Springhead.h>

#include <Foundation/Object.h>
#include "IfStubCreature.h"

#include <vector>
#include <algorithm>

//@{
namespace Spr{;
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief ���ӕ��̃��X�g
*/
class CRAttentionList : public SceneObject, public CRAttentionListIfInit, public CRAttentionListDesc {
private:
	/** @brief ���ӑΏۂ̃��X�g
	*/
	std::vector<CRAttentionItem*> attentionList;

	/** @brief �\�[�g�Ɏg��
	*/
	static bool Small(const CRAttentionItem* &a, const CRAttentionItem* &b);

	/** @brief ����������
	*/
	void Init();

public:
	OBJECTDEF(CRAttentionList, SceneObject);
	ACCESS_DESC(CRAttentionList);

	CRAttentionList(){ Init(); }
	CRAttentionList(const CRAttentionListDesc& desc, SceneIf* s=NULL) : CRAttentionListDesc(desc) {
		if(s){SetScene(s);}
		Init();
	}

	/** @brief ���X�g���N���A����
	*/
	virtual void Clear();

	/** @brief ���ӑΏۃ��X�g���\�[�g����
	*/
	virtual void SortAttentionList();

	/** @brief ����solid�̍��Wpos�ɐݒ肳�ꂽ���ӂ��擾����
		@param solid ���́i���̂ł͂Ȃ��ꏊ�݂̂ɐݒ肳�ꂽ���ӂ̏ꍇNULL�j
		@param pos ���̂̃��[�J�����W�n�ɂ�������W
	*/
	virtual CRAttentionItem* GetAttentionItem(PHSolidIf* solid, Vec3f pos);

	/** @brief i�Ԗڂ̒��ӂ��擾����i����Sort��Ɏg���j
	*/
	virtual CRAttentionItem* GetAttentionItem(int i);

	/** @brief AttractiveList�ɒǉ����ꂽ�Ώۂ̐�
	*/
	virtual int NAttentionItems();
};
}
//@}

#endif//CRATTENTIONLIST_H
