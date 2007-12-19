/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRHINGEHUMANBODY_H
#define CRHINGEHUMANBODY_H

#include <Springhead.h>

#include <Foundation/Object.h>
#include "IfStubCreature.h"

#include <vector>

#include "CRBody.h"

//@{
namespace Spr{;

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
// CRHingeHumanBody
// �q���W�W���C���g��p�����l�̃��f���E�N���X�̎���
class CRHingeHumanBody : public CRBody, public CRHingeHumanBodyIfInit, public CRHingeHumanBodyDesc {
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

	void SetJointSpringDamper(PHHingeJointDesc &hingeDesc, double springOrig, double damperOrig, double actuatorMass);

public:
	OBJECTDEF(CRHingeHumanBody, CRBody);
	ACCESS_DESC(CRHingeHumanBody);

	CRHingeHumanBody(){}
	CRHingeHumanBody(const CRHingeHumanBodyDesc& desc, CRCreatureIf* c=NULL) 
		: CRHingeHumanBodyDesc(desc) 
		, CRBody((const CRBodyDesc&)desc, c)
	{
		solids.resize(CRHingeHumanBodyDesc::SO_NSOLIDS);
		joints.resize(CRHingeHumanBodyDesc::JO_NJOINTS);

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

	/** @brief �㔼�g�̊֐߂̌�����ς���
		@param stifness �����F�f�t�H���g�̌����ɑ΂���{���Ŏw��
	*/
	virtual void SetUpperBodyStiffness(float stiffness);

	/** @brief �㔼�g�̃|�[�Y��ێ�����
	*/
	virtual void KeepUpperBodyPose();

	/** @brief �㔼�g�̃|�[�Y�������ʒu�ɖ߂�
	*/
	virtual void ResetUpperBodyPose();

};

}
//@}

#endif//CRHINGEHUMANBODY_H
