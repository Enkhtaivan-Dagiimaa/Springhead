/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include "CRAttentionList.h"

#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

namespace Spr{
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
// 
bool CRAttentionList::Small(const CRAttentionItem* &a, const CRAttentionItem* &b){
	return a->attractiveness < b->attractiveness;
}

void CRAttentionList::Init(){
	// �{�������ϐ��̏����������Ă����̂��������ϐ��������Ȃ��Ă��܂����imitake�j
}

IF_OBJECT_IMP(CRAttentionList, SceneObject);

void CRAttentionList::Clear(){
	for (int i=0; i<attentionList.size(); i++) {
		delete attentionList[i];
	}
	attentionList.clear();
}

void CRAttentionList::SortAttentionList(){
	sort(attentionList.begin(), attentionList.end(), CRAttentionList::Small);
}

CRAttentionItem* CRAttentionList::GetAttentionItem(PHSolidIf* solid, Vec3f pos){
	for (int i=0; i<attentionList.size(); i++) {
		if (attentionList[i]->solid == solid && (attentionList[i]->pos - pos).norm() < 0.001) { // ���l�͕ύX�\�ɂ��邱��(mitake)
			return attentionList[i];
		}
	}
	attentionList.push_back(new CRAttentionItem(solid, pos, 0.0));
	return attentionList.back();
}

CRAttentionItem* CRAttentionList::GetAttentionItem(int i){
	return attentionList[i];
}

int CRAttentionList::NAttentionItems(){
	return attentionList.size();
}
}
