/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRBODY_H
#define CRBODY_H

#include <Springhead.h>

#include <Foundation/Object.h>
#include "IfStubCreature.h"

//@{
namespace Spr{;
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief �{�f�B
*/
class CRBody : public SceneObject, public CRBodyIfInit, public CRBodyDesc {
protected:
	/// �{�f�B���\�����鍄��
	std::vector<PHSolidIf*> solids;

	/// �{�f�B���\������֐�
	std::vector<PHJointIf*> joints;

	/// ���̃Z���T�̎����傽��Creature
	CRCreatureIf* creature;

	/// ��������V�[��
	PHSceneIf* phScene;

	/// ��������Sdk
	PHSdkIf* phSdk;

	/** @brief �֐߂����
	*/
	PHJointIf* CreateJoint(PHSolidIf* soChild, PHSolidIf* soParent, PHHingeJointDesc desc);

public:
	OBJECTDEF(CRBody, SceneObject);
	ACCESS_DESC(CRBody);

	CRBody(){}
	CRBody(const CRBodyDesc& desc, CRCreatureIf* c=NULL) 
		: CRBodyDesc(desc)
		, solids(CRHingeHumanBodyDesc::SO_NSOLIDS)
		, joints(CRHingeHumanBodyDesc::JO_NJOINTS)  
	{
		if(c!=NULL && c->GetScene()){SetScene(c->GetScene());}
		creature = c;
	}

	/** @brief ���������s��
	*/
	virtual void Init();

	/** @brief ���̂̐��𓾂�
	*/
	virtual int NSolids();

	/** @brief i�Ԗڂ̍��̂𓾂�
	*/
	virtual PHSolidIf* GetSolid(int i);

	/** @brief �֐߂̐��𓾂�
	*/
	virtual int NJoints();

	/** @brief i�Ԗڂ̊֐߂𓾂�
	*/
	virtual PHJointIf* GetJoint(int i);
};
}
//@}

#endif//CRBODY_H
