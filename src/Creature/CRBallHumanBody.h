/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRBallHUMANBODY_H
#define CRBallHUMANBODY_H

#include <Springhead.h>

#include <Foundation/Object.h>

#include <vector>

#include "CRBody.h"

//@{
namespace Spr{;

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
// CRBallHumanBody
// �{�[���W���C���g��p�����l�̃��f���E�N���X�̎���
class CRBallHumanBody : public CRBody, public CRBallHumanBodyDesc {
private:
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

	void InitContact();

	void SetJointSpringDamper(PHBallJointDesc  &ballDesc,  double springOrig, double damperOrig, double actuatorMass);
	void SetJointSpringDamper(PHHingeJointDesc &hingeDesc, double springOrig, double damperOrig, double actuatorMass);

public:
	SPR_OBJECTDEF(CRBallHumanBody);
	ACCESS_DESC(CRBallHumanBody);

	CRBallHumanBody(){}
	CRBallHumanBody(const CRBallHumanBodyDesc& desc, CRCreatureIf* c=NULL) 
		: CRBallHumanBodyDesc(desc) 
		, CRBody((const CRBodyDesc&)desc, c)
	{
		solids.resize(CRBallHumanBodyDesc::SO_NSOLIDS);
		joints.resize(CRBallHumanBodyDesc::JO_NJOINTS);

		InitBody();
		InitHead();
		InitArms();
		InitEyes();
		InitLegs();

		InitContact();
	}

	/** @brief �����������s����
	*/
	virtual void Init();
};

}
//@}

#endif//CRBallHUMANBODY_H