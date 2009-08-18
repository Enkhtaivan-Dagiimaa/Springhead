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
		bSim = false;
		sceneSolid = NULL;
	}
}; 
typedef std::vector<FWInteractSolid> FWInteractSolids;

}

#endif