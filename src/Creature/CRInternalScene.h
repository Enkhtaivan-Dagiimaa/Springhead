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
private:
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

	/*
	/// �f�X�N���v�^�֘A
protected:
	PHSolidIf* solid;
	Vec3f position;
public:
	virtual void SetDesc(const void* desc){
		solid    = ((CRInternalSceneObjectDesc*)desc)->solid;
		position = ((CRInternalSceneObjectDesc*)desc)->position;
	}
	virtual bool GetDesc(void* desc){
		((CRInternalSceneObjectDesc*)desc)->solid    = solid;
		((CRInternalSceneObjectDesc*)desc)->position = position;
		return true;
	}
	*/

	/** @brief ���̂�Ԃ�
	*/
	virtual PHSolidIf* GetSolid();

	/** @brief �ʒu��Ԃ�
	*/
	virtual Vec3f GetPos();
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

	/*
	/// �f�X�N���v�^�֘A
protected:
	float  attractiveness;
public:
	virtual void SetDesc(const void* desc){
		CRInternalSceneObject::SetDesc(desc);
		attractiveness = ((CRISAttractiveObjectDesc*)desc)->attractiveness;
	}
	virtual bool GetDesc(void* desc){
		CRInternalSceneObject::GetDesc(desc);
		((CRISAttractiveObjectDesc*)desc)->attractiveness = attractiveness;
		return true;
	}
	*/

	/** @brief ���ӂ��Ђ�����x���𓾂�
	*/
	virtual float GetAttractiveness();

	/** @brief ���ӂ��Ђ�����x����ݒ肷��
	*/
	virtual void SetAttractiveness(float attractiveness);
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
	static bool LargerAttention(const CRInternalSceneObject* &a, const CRInternalSceneObject* &b);

public:
	OBJECTDEF(CRInternalScene, SceneObject);
	ACCESS_DESC(CRInternalScene);

	CRInternalScene(){}
	CRInternalScene(const CRInternalSceneDesc& desc, CRCreatureIf* c=NULL) : CRInternalSceneDesc(desc) {
		if(c!=NULL && c->GetScene()){SetScene(c->GetScene());}
		creature = c;
	}

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
	virtual CRInternalSceneObjectIf* GetObject(int i);

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
