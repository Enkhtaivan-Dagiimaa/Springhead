/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRTRUNKFOOTANIMALBODY_H
#define CRTRUNKFOOTANIMALBODY_H

#include <Springhead.h>

#include <Foundation/Object.h>
#include <vector>

#include "CRBody.h"

//@{
namespace Spr{;

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
// CRTrunkFootAnimalBody
// �q���W�W���C���g��p�����M���ރ��f���E�N���X�̎����i�������F���g��TrunkFootHumanBody�j
class CRTrunkFootAnimalBody : public CRBody, public CRTrunkFootAnimalBodyDesc {
private:
	void InitBody();
	void CreateWaist();
	void CreateChest();
	void CreateHead();

	void InitLegs();
	void CreateFoot(LREnum lr);

	void InitContact();

public:
	SPR_OBJECTDEF(CRTrunkFootAnimalBody);
	ACCESS_DESC(CRTrunkFootAnimalBody);

	CRTrunkFootAnimalBody(){}
	CRTrunkFootAnimalBody(const CRTrunkFootAnimalBodyDesc& desc, CRCreatureIf* c=NULL) 
		: CRTrunkFootAnimalBodyDesc(desc) 
		, CRBody((const CRBodyDesc&)desc, c)
	{
		solids.resize(CRTrunkFootAnimalBodyDesc::SO_NSOLIDS);
		joints.resize(CRTrunkFootAnimalBodyDesc::JO_NJOINTS);

		InitBody();
		InitLegs();

		InitContact();
	}

	/** @brief �����������s����
	*/
	virtual void Init();

};

}
//@}

#endif//CRTRUNKFOOTANIMALBODY_H
