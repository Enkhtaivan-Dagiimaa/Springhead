/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

#ifndef FWINTERACT_SOLID_H
#define FWINTERACT_SOLID_H

#include <Foundation/Object.h>
#include <Physics/PHSolid.h>

namespace Spr{;

struct FWInteractSolid : public SceneObject{
public:
	bool bfirstSim;
	bool bSim;
	PHSolid* sceneSolid;
	PHSolid copiedSolid;
	SpatialVector b;				///< �\���V�~�����[�V�����ŋ��߂����r���e�B�i�d�͓��̒萔���j
	SpatialVector curb;				///< �ʏ�V�~�����[�V�����łł��萔��
	SpatialVector lastb;			///< �O��̗\���V�~�����[�V�����ŋ��߂��萔��
	FWInteractSolid(){
		bfirstSim = false; 
		bSim = false;
		sceneSolid = NULL;
	}
}; 
typedef std::vector<FWInteractSolid> FWInteractSolids;

}

#endif