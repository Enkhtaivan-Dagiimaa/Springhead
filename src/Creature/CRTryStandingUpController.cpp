#include "CRTryStandingUpController.h"

namespace Spr{;

IF_OBJECT_IMP(CRTryStandingUpController, CRController);

std::vector<PHSolidIf*> CRTryStandingUpController::SetFootSolid(CRBodyIf* body){
	
	if(DCAST(CRHingeHumanBodyIf, body)!=NULL){
		foot.resize(sizeof(PHSolidIf*) +1);
		foot.back() = body->GetSolid(CRHingeHumanBodyDesc::SO_LEFT_FOOT);
		foot.resize(sizeof(PHSolidIf*) +1);
		foot.back() = body->GetSolid(CRHingeHumanBodyDesc::SO_RIGHT_FOOT);	
	}
	else if(DCAST(CRFourLegsAnimalBodyIf, body)!=NULL){
		foot.resize(sizeof(PHSolidIf*) +1);
		foot.back() = body->GetSolid(CRFourLegsAnimalBodyDesc::SO_LEFT_FRONT_TOE);
		foot.resize(sizeof(PHSolidIf*) +1);
		foot.back() = body->GetSolid(CRFourLegsAnimalBodyDesc::SO_LEFT_REAR_TOE);
		foot.resize(sizeof(PHSolidIf*) +1);
		foot.back() = body->GetSolid(CRFourLegsAnimalBodyDesc::SO_RIGHT_FRONT_TOE);
		foot.resize(sizeof(PHSolidIf*) +1);
		foot.back() = body->GetSolid(CRFourLegsAnimalBodyDesc::SO_RIGHT_REAR_TOE);
	}
	return foot;
}

Vec3d CRTryStandingUpController::GetFootPos(PHSolidIf* footSolid){
	Vec3d pos;
	pos = footSolid->GetPose();
	return pos;
}

void CRTryStandingUpController::TransitionPoseModel(CRBodyIf* crBody){
	
	;
}

void CRTryStandingUpController::Init(){	
	CRController::Init();

//���ϐ��̏�����
	totalStep = 0;
	
	// body[i]:i�̖ڂ̃N���[�`���[�̃{�f�B
	for(int i=0; i<creature->NBodies(); i++){
		body.resize(sizeof(CRBodyIf*) +1);	
		body.back() = creature->GetBody(i);
	}
}

void CRTryStandingUpController::Step(){	
	totalStep += 1;
	CRController::Step();

	// �e�{�f�B�̏d�S���o��
	for(int i=0; i<creature->NBodies(); i++){
		centerOfMass = body[i]->GetCenterOfMass();
		DSTR << centerOfMass << std::endl;

		SetFootSolid(body[i]);
		if(DCAST(CRHingeHumanBodyIf, body[i])!=NULL){
			rightFootPos = GetFootPos(foot[0]);
			leftFootPos	 = GetFootPos(foot[1]);
		}
		else if(DCAST(CRFourLegsAnimalBodyIf, body[i])!=NULL){
			rightFrontFootPos = GetFootPos(foot[0]);
			rightRearFootPos  = GetFootPos(foot[1]);
			leftFrontFootPos  = GetFootPos(foot[2]);
			leftRearFootPos	  = GetFootPos(foot[3]);
		}
	}
}

}