/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRHINGEHUMAN_H
#define CRHINGEHUMAN_H

#include <Springhead.h>

#include <Foundation/Object.h>
#include "IfStubCreature.h"

#include <vector>

//@{
namespace Spr{;

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
// CRHingeHuman
// �q���W�W���C���g��p�����l�̃��f���E�N���X�̎���
class CRHingeHuman : public SceneObject, public CRHingeHumanIfInit, public CRHingeHumanDesc {
private:
	enum LREnum{LEFTPART=-1, RIGHTPART=+1};

	std::vector<PHSolidIf*> solids;
	std::vector<PHJointIf*> joints;

	CREyeControllerIf *eyeCtrl;

	PHSceneIf *phScene;
	PHSdkIf   *phSdk;

	PHJointIf* CreateJoint(PHSolidIf* soChild, PHSolidIf* soParent, PHHingeJointDesc desc);

	void InitBody();
	void CreateWaist();
	void CreateAbdomen();
	void CreateChest();

	void InitHead();
	void CreateNeck();
	void CreateHead();

	void InitArms();
	void CreateUpperArm(LREnum lr);
	void CreateLowerArm(LREnum lr);
	void CreateHand(LREnum lr);

	void InitEyes();
	void CreateEye(LREnum lr);

	void InitLegs();
	void CreateUpperLeg(LREnum lr);
	void CreateLowerLeg(LREnum lr);
	void CreateFoot(LREnum lr);

public:
	OBJECTDEF(CRHingeHuman, SceneObject);
	ACCESS_DESC(CRHingeHuman);

	CRHingeHuman(){eyeCtrl=NULL;}
	CRHingeHuman(const CRHingeHumanDesc& desc, SceneIf* s=NULL) : CRHingeHumanDesc(desc), solids(SO_NSOLIDS), joints(JO_NJOINTS) {
		eyeCtrl=NULL;
		if(s){
			SetScene(s);Init();
		}
	}

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

	/** @brief �l�̃��f���̏������i�\�z�܂ށj���s��
	*/
	void Init();
};

}
//@}

#endif//CRHINGEHUMAN_H
