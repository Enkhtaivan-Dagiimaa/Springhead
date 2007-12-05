/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRINTERNALSCENE_H
#define CRINTERNALSCENE_H

#include <Springhead.h>

#include <Foundation/Object.h>
#include "IfStubCreature.h"
#include "AccessDescDumpCreature.h"

#include <vector>
#include <algorithm>

//@{
namespace Spr{;
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief �����V�[�����\������I�u�W�F�N�g
*/
class CRInternalSceneObject : public SceneObject, public CRInternalSceneObjectIfInit {

protected:
	/// ������������V�[��
	CRInternalSceneIf* internalScene;

public:
	OBJECTDEF(CRInternalSceneObject, SceneObject);
	ACCESS_DESC_CRInternalSceneObjectDesc_NOBASE;

	CRInternalSceneObject(){}
	CRInternalSceneObject(const CRInternalSceneObjectDesc& desc, CRInternalSceneIf* is) {
		SetDesc(&desc);
		internalScene = is;
	}

	/** @brief �w�肵��type�����肷��
	*/
	virtual bool IsA(const char* typestr);

	/** @brief ��ނ�Ԃ�
	*/
	virtual const char* GetISObjType();

	/** @brief ���̂�Ԃ�
	*/
	virtual PHSolidIf* GetSolid();

	/** @brief �ʒu��Ԃ�
	*/
	virtual Vec3f GetPos();

	/** @brief �ʒu��ݒ肷��
	*/
	virtual void SetPos(Vec3d pos);
};

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief ���ӂ��Ђ����镨��
*/
class CRISAttractiveObject : public CRInternalSceneObject, public CRISAttractiveObjectIfInit {

private:

public:
	OBJECTDEF(CRISAttractiveObject, CRInternalSceneObject);
	ACCESS_DESC_CRISAttractiveObjectDesc(CRInternalSceneObject);

	CRISAttractiveObject(){}
	CRISAttractiveObject(const CRISAttractiveObjectDesc& desc, CRInternalSceneIf* is) 
		: CRInternalSceneObject((const CRInternalSceneObjectDesc&)desc, is)
	{
		SetDesc(&desc);
	}

	/** @brief �����I�ɂ��̕��̂����ӂ��Ђ�����x���𓾂�
	*/
	virtual float GetTotalAttractiveness();

	/** @brief �{�g���A�b�v���ӂ̓x�������Z����
	*/
	virtual void AddBottomupAttr(float attr);

	/** @brief �{�g���A�b�v���ӂ̓x�������Z�b�g����
	*/
	virtual void ClearBottomupAttr();

	/** @brief �g�b�v�_�E�����ӂ̓x����ݒ肷��
	*/
	virtual void SetTopdownAttr(float attr);

	/** @brief �s�m�����𑝂�
	*/
	virtual void IncUncertainty();

	/** @brief �s�m���������炷
	*/
	virtual void DecUncertainty();

	/** @brief �s�m�������擾����
	*/
	virtual float GetUncertainty();
};

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief ���s�̃|�e���V�����ɂ�����镨��
*/
class CRISTravelPotentialObject : public CRInternalSceneObject, public CRISTravelPotentialObjectIfInit {

private:

public:
	OBJECTDEF(CRISTravelPotentialObject, CRInternalSceneObject);
	ACCESS_DESC_CRISTravelPotentialObjectDesc(CRInternalSceneObject);

	CRISTravelPotentialObject(){}
	CRISTravelPotentialObject(const CRISTravelPotentialObjectDesc& desc, CRInternalSceneIf* is) 
		: CRInternalSceneObject((const CRInternalSceneObjectDesc&)desc, is)
	{
		SetDesc(&desc);
	}

	/*
	/// �f�X�N���v�^�֘A
protected:
	Vec2f  strength;
	Vec2f  decay;
public:
	virtual void SetDesc(const void* desc){
		CRInternalSceneObject::SetDesc(desc);
		strength = ((CRISTravelPotentialObjectDesc*)desc)->strength;
		decay    = ((CRISTravelPotentialObjectDesc*)desc)->decay;
	}
	virtual bool GetDesc(void* desc){
		CRInternalSceneObject::GetDesc(desc);
		((CRISTravelPotentialObjectDesc*)desc)->strength = strength;
		((CRISTravelPotentialObjectDesc*)desc)->decay    = decay;
		return true;
	}
	*/

	/** @brief �|�e���V�����̋����̌W���iA, B�j�𓾂�
	*/
	virtual Vec2f GetStrengthCoeff();

	/** @brief �|�e���V�����̋����̌W���iA, B�j��ݒ肷��
	*/
	virtual void SetStrengthCoeff(Vec2f strength);

	/** @brief �|�e���V�����̌����̌W���in, m�j�𓾂�
	*/
	virtual Vec2f GetDecayCoeff();

	/** @brief �|�e���V�����̌����̌W���in, m�j��ݒ肷��
	*/
	virtual void SetDecayCoeff(Vec2f decay);

	/** @brief ����ʒu�ɑΉ�����|�e���V�������v�Z����
	*/
	virtual Vec2f GetPotential(Vec2f currPos);
};

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief �����V�[��
*/
class CRInternalScene : public SceneObject, public CRInternalSceneIfInit, public CRInternalSceneDesc {

private:
	/// �����傽��Creature
	CRCreatureIf* creature;

	/** @brief ���ӑΏۂ̃��X�g
	*/
	std::vector<CRInternalSceneObject*> sceneObjects;

	/** @brief �\�[�g�Ɏg��
	*/
	static bool LargerAttention(const CRInternalSceneObject* a, const CRInternalSceneObject* b);

public:
	OBJECTDEF(CRInternalScene, SceneObject);
	ACCESS_DESC(CRInternalScene);

	CRInternalScene(){}
	CRInternalScene(const CRInternalSceneDesc& desc, CRCreatureIf* c=NULL) : CRInternalSceneDesc(desc) {
		if(c!=NULL && c->GetScene()){SetScene(c->GetScene());}
		creature = c;
	}

	/** @brief �X�e�b�v���Ƃ̏��������s����
	*/
	virtual void Step();

	/** @brief Attractiveness���N���A����i�{����Bottomup�����j
	*/
	virtual void ClearAttractiveness();

	/** @brief Attractiveness�̑傫�����Ƀ\�[�g����
	*/
	virtual void SortByAttractiveness();

	/** @brief ����solid�̍��Wpos�ɐݒ肳�ꂽ���ӂ��擾����
		@param solid ���́i���̂ł͂Ȃ��ꏊ�݂̂ɐݒ肳�ꂽ���ӂ̏ꍇNULL�j
		@param pos ���̂̃��[�J�����W�n�ɂ�������W
	*/
	virtual CRInternalSceneObjectIf* FindObject(PHSolidIf* solid, Vec3f pos);

	/** @brief i�Ԗڂ̕��̂��擾����iSort��ɗp�����Sort���ꂽ�����Ŏ擾�ł���j
	*/
	virtual CRInternalSceneObjectIf* GetISObject(int i);

	/** @brief ���̂̐�
	*/
	virtual int NObjects();

	/** @brief �I�u�W�F�N�g���쐬����
	*/
	virtual CRInternalSceneObjectIf* CreateInternalSceneObject(const IfInfo* ii, const CRInternalSceneObjectDesc& desc);
};
}
//@}

#endif//CRINTERNALSCENE_H
