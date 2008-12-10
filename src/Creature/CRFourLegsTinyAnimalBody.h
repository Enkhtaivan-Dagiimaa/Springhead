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
	void CreateBody();
	void CreateHead();
	void CreateFrontLegs(LREnum lr);
	void CreateRearLegs(LREnum lr);
	void InitBody();
	void InitLegs();

	void InitContact();
	void InitControlMode(PHJointDesc::PHControlMode m = PHJointDesc::MODE_POSITION);	//< �{�f�B�̐��䃂�[�h��ݒ肷��D

public:
	SPR_OBJECTDEF(CRFourLegsTinyAnimalBody);
	ACCESS_DESC(CRFourLegsTinyAnimalBody);

	CRFourLegsTinyAnimalBody(){}
	CRFourLegsTinyAnimalBody(const CRFourLegsTinyAnimalBodyDesc& desc, CRCreatureIf* c=NULL) 
		: CRFourLegsTinyAnimalBodyDesc(desc) 
		, CRBody((const CRBodyDesc&)desc, c)
	{
		solids.resize(CRFourLegsTinyAnimalBodyDesc::SO_NSOLIDS);
		joints.resize(CRFourLegsTinyAnimalBodyDesc::JO_NJOINTS);
	
		InitBody();
		InitLegs();
		InitContact();
		InitControlMode();
		Init();
	}

	/** @brief �����������s����
	*/
	virtual void Init();

};

}
//@}

#endif//CRFourLegsTinyAnimalBODY_H
