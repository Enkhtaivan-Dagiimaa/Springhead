/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRFOURLEGSTINYANIMALBODY_H
#define CRFOURLEGSTINYANIMALBODY_H

#include <Springhead.h>

#include <Foundation/Object.h>
#include <Physics/PHConstraint.h>
#include <Physics/PhysicsDecl.hpp>

#include <vector>

#include "CRBody.h"

//@{
namespace Spr{;

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
// CRFourLegsTinyAnimalBody
// �q���W�W���C���g��p�����M���ރ��f���E�N���X�̎����i�������F���g��TrunkFootHumanBody�j
class CRFourLegsTinyAnimalBody : public CRBody, public CRFourLegsTinyAnimalBodyDesc {
private:
	double massFF;		//< �O���̎���
	double massFL;		//< �O�r�̎���
	double massRF;		//< �㑫�̎���
	double massRL;		//< ��r�̎���
	double massBody;	//< �̊��̎���

	void CreateBody();
	void CreateFrontLegs(LREnum lr);
	void CreateRearLegs(LREnum lr);
	void InitBody();
	void InitFrontLeg0(LREnum lr);
	void InitFrontLeg1(LREnum lr);
	void InitRearLeg0(LREnum lr);
	void InitRearLeg1(LREnum lr);
	void InitLegs();

	void InitContact();
	void InitControlMode(PHJointDesc::PHControlMode m = PHJointDesc::MODE_POSITION);	//< �{�f�B�̐��䃂�[�h��ݒ肷��D

public:
	SPR_OBJECTDEF(CRFourLegsTinyAnimalBody);
	ACCESS_DESC(CRFourLegsTinyAnimalBody);

	CRFourLegsTinyAnimalBody(){
		massFF		= 10;
		massFL		= 20;
		massRF		= 10;
		massRL		= 20;
		massBody	= 50;
	}
	CRFourLegsTinyAnimalBody(const CRFourLegsTinyAnimalBodyDesc& desc, CRCreatureIf* c=NULL) 
		: CRFourLegsTinyAnimalBodyDesc(desc) 
		, CRBody((const CRBodyDesc&)desc, c)
	{
		solids.resize(CRFourLegsTinyAnimalBodyDesc::SO_NSOLIDS);
		joints.resize(CRFourLegsTinyAnimalBodyDesc::JO_NJOINTS);
		massFF		= 1.5;
		massFL		= 1.5;
		massRF		= 1.5;
		massRL		= 1.5;
		massBody	= 10;
		InitBody();
		InitLegs();
		InitContact();
		InitControlMode();
		Init();
	}

	/** @brief �����������s����
	*/
	virtual void Init();
	virtual Vec2d GetSwingLimit(){return rangeFrontSwing;}
	virtual Vec2d GetTwistLimit(){return rangeFrontTwist;}

};

}
//@}

#endif//CRFourLegsTinyAnimalBODY_H
