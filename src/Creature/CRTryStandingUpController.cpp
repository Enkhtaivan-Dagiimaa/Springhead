#include "CRTryStandingUpController.h"

namespace Spr{;

IF_OBJECT_IMP(CRTryStandingUpController, CRController);

Vec3d GetFootPos(PHSolidIf* footSolid){
	Vec3d pos;
	return pos;
}

void TransitionPoseModel(CRBodyIf* crBody){
	
	;
}

void CRTryStandingUpController::Init(){	
	CRController::Init();

	//���ϐ��̏�����
	totalStep = 0;
}

void CRTryStandingUpController::Step(){	
	totalStep += 1;
	CRController::Step();

	// �e�{�f�B�̏d�S���o��
	for(int i=0; i<creature->NBodies(); i++){
	centerOfMass = creature->GetBody(i)->GetCenterOfMass();
	DSTR << centerOfMass << std::endl;

/*
	rightFrontFootPos = GetFootPos(footSolid);
	rightRearFootPos  = GetFootPos(footSolid);
	leftFrontFootPos  = GetFootPos(footSolid);
	lefrRearFootPos	  = GetFootPos(footSolid);
*/
	}
}

}