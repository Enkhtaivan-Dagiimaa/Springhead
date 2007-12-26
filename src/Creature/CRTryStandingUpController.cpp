#include "CRTryStandingUpController.h"

namespace Spr{;

IF_OBJECT_IMP(CRTryStandingUpController, CRController);

//------------------------------------------------------------------------------------------
// private Func:

std::vector<PHSolidIf*> CRTryStandingUpController::SetFootSolid(CRBodyIf* body){
	
	if(DCAST(CRHingeHumanBodyIf, body)!=NULL){
		foot.push_back(body->GetSolid(CRHingeHumanBodyDesc::SO_RIGHT_FOOT));	
		foot.push_back(body->GetSolid(CRHingeHumanBodyDesc::SO_LEFT_FOOT));
	
	}
	else if(DCAST(CRFourLegsAnimalBodyIf, body)!=NULL){
		foot.push_back(body->GetSolid(CRFourLegsAnimalBodyDesc::SO_RIGHT_FRONT_TOE));
		foot.push_back(body->GetSolid(CRFourLegsAnimalBodyDesc::SO_RIGHT_REAR_TOE));
		foot.push_back(body->GetSolid(CRFourLegsAnimalBodyDesc::SO_LEFT_FRONT_TOE));
		foot.push_back(body->GetSolid(CRFourLegsAnimalBodyDesc::SO_LEFT_REAR_TOE));		
	}
	return foot;
}

Vec3d CRTryStandingUpController::GetFootPos(PHSolidIf* footSolid){
	Vec3d pos;
	pos = footSolid->GetPose().Pos();
	return pos;
}

void CRTryStandingUpController::TransitionPoseModel(std::vector<CRFLAnimalGeneData> gene){
	for(unsigned int i=0; i<gene.size(); i++){
		//BDSTR << gene[i].goalDir << std::endl;
		if(i==0){
			PHBallJointDesc ballDesc;
			body[0]->GetJoint(CRFourLegsAnimalBodyDesc::JO_WAIST_CHEST)->GetDesc(&ballDesc);
			ballDesc.goal = gene[i].goalDir;
			body[0]->GetJoint(CRFourLegsAnimalBodyDesc::JO_WAIST_CHEST)->SetDesc(&ballDesc);
			Quaterniond dd = body[0]->GetJoint(CRFourLegsAnimalBodyDesc::JO_WAIST_CHEST)->GetRelativePoseQ();
		
		}	
	}
}

void CRTryStandingUpController::UpdateBodyState(){
	// �e�{�f�B�̏����X�V����
	for(int i=0; i<creature->NBodies(); i++){
		if(body[i] != 0){
			centerOfMass = body[i]->GetCenterOfMass();
			//DSTR << centerOfMass << std::endl;

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
				//DSTR << "rightFrontFoot: " << rightFrontFootPos << "rightRearFoot: " << rightRearFootPos << "leftFrontFoot: "  << leftFrontFootPos  << "leftRearFoot: "  << leftRearFootPos << std::endl;
			}
		}
	}
}




//------------------------------------------------------------------------------------------
// public Func:
void CRTryStandingUpController::Init(){	
	CRController::Init();

//���ϐ��̏�����
	totalStep = 0;
	animalGeneIf = DBG_NEW CRFLAnimalGene(creature);			//< animalGene�̈�ԍŌ�ɃN���[�`���[�̕������\��t����

	// body[i]:i�̖ڂ̃N���[�`���[�̃{�f�B�ɂȂ�悤�ɓo�^����
	for(int i=0; i<creature->NBodies(); i++){
		body.push_back(creature->GetBody(i));							//< creature�̒��ɂ���{�f�B�������ԂɊi�[���Ă���
	}	
}

void CRTryStandingUpController::Learning(){
	
	
	
}

void CRTryStandingUpController::Step(){	
	totalStep += 1;
	CRController::Step();
	UpdateBodyState();

	for(int i=0; i<creature->NBodies(); i++){
		animalGenes.push_back(animalGeneIf->CreateGene(body[i]));		
	}
	DSTR << animalGenes.size() << std::endl;
	
	//���X�e�b�v�ł��Ă���ŐV��animalGenes���m�F����
	for(unsigned int i=0; i<animalGenes.back().size(); i++){
		DSTR << animalGenes.back()[i].goalDir << std::endl;
	}	
	std::vector<CRFLAnimalGeneData> gene = animalGeneIf->MixGenes(animalGeneIf->flAnimalGenes[0], animalGenes.back());
	
	/*
	for(int i=0; i<gene.size(); i++){
		DSTR << gene[i].goalDir << std::endl;
	}
	*/
	TransitionPoseModel(gene);
	
}

}