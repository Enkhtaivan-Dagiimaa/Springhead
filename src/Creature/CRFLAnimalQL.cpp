/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include "CRFLAnimalQL.h"
//-----------------------------------------------------------------------------------
//publicFunc:
void CRFLAnimalQL::Init(){
	for(int i=0; i<creature->NBodies(); i++){
		crBody.push_back(creature->GetBody(i));
	}
	learningRate =	  0.2;
	decreaseRate =    0.8;
	penalty		 = -100.0;
	moveRatio	 =	  0.1;

}

void CRFLAnimalQL::BoltzmannSelection(){
	
	//////////////////////////////////////////////////////////////
	//															//
	// �ǂ��������ɃG�l���M�[���Ⴂ���l����						//
	//															//
	//////////////////////////////////////////////////////////////

}
void CRFLAnimalQL::EpsilonGreedySelection(){
	
}

void CRFLAnimalQL::SetActionNumber(std::vector<CRFLAnimalGeneData> *aGene){
	actionNumber.resize((*aGene).size());
	for(unsigned int i = 0; i < (*aGene).size(); i++){
		if((*aGene)[i].geneType == CRFLAnimalGeneData::GEN_QUATERNIOND)
			actionNumber[i] = 9;
		else if((*aGene)[i].geneType == CRFLAnimalGeneData::GEN_DOUBLE)
			actionNumber[i] = 3;
	}

}
void CRFLAnimalQL::SelectAction(std::vector<CRFLAnimalGeneData> *aGene){

	////////////////////////////////////
	//								  //
	// �{���c�}���I������񂾂����H //
	//								  //
	////////////////////////////////////

	srand((unsigned) time(NULL));
	action.resize(actionNumber.size());
	for(unsigned int i = 0; i < crBody.size(); i++){
		for(unsigned int j = 0; j < actionNumber.size(); j++){
			//�K���ɃA�N�V���������߂Ă��邪�A�����Ƀ{���c�}���I�����-greedy�I�������ݍ��ޕK�v������
			action[j] = rand()%actionNumber[j];
//			DSTR << action[j]  << std::endl;

			//////////////////////////////////////////////////////////////////////////
			//																		//
			// �s�������搧���̊O�ɏo�Ă��܂����ꍇ�̏������l���Ȃ��Ɣ��U����		//
			//																		//
			//////////////////////////////////////////////////////////////////////////

			// BallJoint�������ꍇ�Ɏ��s��
			if((*aGene)[j].geneType == CRFLAnimalGeneData::GEN_QUATERNIOND){
				if(action[j] == 0){
					// Quaternion�̌v�Z�͍����珇�Ԃɍl����
					(*aGene)[j].goalDir = Quaterniond::Rot(Rad( moveRatio), 'x') * Quaterniond::Rot(Rad(-moveRatio), 'y') * (*aGene)[j].goalDir;
				}
				else if(action[j] == 1){
					(*aGene)[j].goalDir = Quaterniond::Rot(Rad(-moveRatio), 'y') * (*aGene)[j].goalDir;
				}
				else if(action[j] == 2){
					(*aGene)[j].goalDir = Quaterniond::Rot(Rad(-moveRatio), 'x') * Quaterniond::Rot(Rad(-moveRatio), 'y') * (*aGene)[j].goalDir;
				}
				else if(action[j] == 3){
					(*aGene)[j].goalDir = Quaterniond::Rot(Rad( moveRatio), 'x') * (*aGene)[j].goalDir;
				}
				else if(action[j] == 4){
//					DSTR << "The joint " << j << " is keep its position" << std::endl;
				}
				else if(action[j] == 5){
					(*aGene)[j].goalDir = Quaterniond::Rot(Rad(-moveRatio), 'x') * (*aGene)[j].goalDir;
				}
				else if(action[j] == 6){
					(*aGene)[j].goalDir = Quaterniond::Rot(Rad( moveRatio), 'x') * Quaterniond::Rot(Rad( moveRatio), 'y') * (*aGene)[j].goalDir;
				}
				else if(action[j] == 7){
					(*aGene)[j].goalDir = Quaterniond::Rot(Rad( moveRatio), 'y') * (*aGene)[j].goalDir;
				}
				else if(action[j] == 8){
					(*aGene)[j].goalDir = Quaterniond::Rot(Rad(-moveRatio), 'x') * Quaterniond::Rot(Rad( moveRatio), 'y') * (*aGene)[j].goalDir;
				}
				else{
				}
			}
			
			// HingeJoint�������ꍇ�Ɏ��s��
			else if((*aGene)[j].geneType == CRFLAnimalGeneData::GEN_DOUBLE){
				if(action[j] == 0){
					//if(crBody[0]->GetJoint(j)->
					(*aGene)[j].goalDir[0] += Rad(moveRatio);
				}
				else if(action[j] == 1){
//					DSTR << "The joint " << j << " is keep its position" << std::endl;
				}
				else if(action[j] == 2){
					(*aGene)[j].goalDir[0] -= Rad(moveRatio);
				}
				else{
				}
			}

		}
	}
/*
	for(unsigned int i = 0; i < actionNumber.size(); i++){
		DSTR << (*aGene)[i].goalDir << std::endl;
	}
*/
}

void CRFLAnimalQL::TakeAction(std::vector<CRFLAnimalGeneData> *aGene){

	for(unsigned int i = 0; i < crBody.size(); i++){
//		DSTR << "crBody.size() : " << crBody.size() << std::endl;
		for(unsigned int j = 0; j < (*aGene).size(); j++){
			if((*aGene)[j].geneType == CRFLAnimalGeneData::GEN_QUATERNIOND){
				PHBallJointDesc ballDesc;
				crBody[i]->GetJoint(j)->GetDesc(&ballDesc);
				ballDesc.goal = (*aGene)[j].goalDir;
				crBody[i]->GetJoint(j)->SetDesc(&ballDesc);
			}
			else if((*aGene)[j].geneType == CRFLAnimalGeneData::GEN_DOUBLE){
				PHHingeJointDesc hingeDesc;
				crBody[i]->GetJoint(j)->GetDesc(&hingeDesc);
				hingeDesc.origin = (*aGene)[j].goalDir[0];
				crBody[i]->GetJoint(j)->SetDesc(&hingeDesc);
			}
			else DSTR << "Unknown type." << std::endl;
		}
	}

}

void CRFLAnimalQL::EvaluativeFunc(Vec3d centerOfMass, 
								  std::vector<Vec3d>footPositions, 
								  std::vector<Vec3d> footForces){

	if(footPositions.size() == 2){		//< �n����Ă����f�[�^�͐l�^�̂��̂�
		
		if(crBody[0]->GetSolid(CRHingeHumanBody::SO_HEAD)){
			DSTR << "human body is not up to date." << std::endl;
		}
	}

	else if(footPositions.size() == 4){		//< �n����Ă����f�[�^�͓����^�̂��̂�

		//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
		//[check 1]��̂̌����͂ǂ��Ȃ��Ă���́H
		if(crBody[0]->GetSolid(CRFourLegsAnimalBody::SO_HEAD) && crBody[0]->GetSolid(CRFourLegsAnimalBody::SO_CHEST)){
			Vec3d		 headPos	=	crBody[0]->GetSolid(CRFourLegsAnimalBody::SO_HEAD)->GetPoseR();
			Vec3d		 chestPos	=	crBody[0]->GetSolid(CRFourLegsAnimalBody::SO_CHEST)->GetPoseR();
			Quaterniond  chestOri	=   crBody[0]->GetSolid(CRFourLegsAnimalBody::SO_CHEST)->GetPoseQ();

			// vEvaluateHC  : �����狹�֐L�т��x�N�g���̔w�������ւ̖@���x�N�g���i�]���x�N�g���j, localTheta  : �w���Ɠ����x�N�g���̂Ȃ��p
			//�y���_�z	: ������ƒP�ʃx�N�g���ɂ��Ă�����ς��Ƃ��Ă���̂�1.0�𒴂��Ă��܂����Ƃ�����B�Ȃ����낤�B
			double localTheta	=	acos(min (dot((chestPos - headPos).unit(), (chestOri * chestPos).unit() ), 1.0));
			vEvaluateHC			=	(chestPos - headPos) * sin(localTheta);
			//DSTR << localTheta * 180 / M_PI << std::endl;
		}
		//thetaHC : �]���x�N�g���Əd�͕����̂Ȃ��p
		double thetaHC = acos(min(dot(vEvaluateHC, Vec3d(0.0, -1.0, 0.0)), 1.0));
		//DSTR << "HC: " << vEvaluateHC << " theta: " << thetaHC *180/3.14 << " " << dot(vEvaluateHC, Vec3d(0, -1, 0)) << std::endl;

		//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
		// [check 2]�d�S�͎x���r���p�`�̒��ɂ���́H
		// footSolid[0] : rightFront, footSolid[1] : rightRear, footSolid[2] : leftFront, footSolid[3] : leftRear
		// <<�x���r���p�`����1>>
		Vec3d normalLine = cross((footPositions[0] - footPositions[1]), (footPositions[0] - footPositions[2]));

		//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
		// [check 3]�e�֐߂��{�f�B�̏o����ő�g���N���ɂ��邩���m�F����
		for(int i = 0; i < crBody[0]->NJoints(); i++){
			if(DCAST(PHBallJointIf, crBody[0]->GetJoint(i)))
				DCAST(PHBallJointIf, crBody[0]->GetJoint(i))->GetTorqueMax();
			else if(DCAST(PHHingeJointIf, crBody[0]->GetJoint(i)))
				DCAST(PHHingeJointIf, crBody[0]->GetJoint(i))->GetTorqueMax();
		}

	}

	


}

void CRFLAnimalQL::CalcQValueMax(){

}

void CRFLAnimalQL::CalcQValueMin(){

}

void CRFLAnimalQL::UpdateQValues(){

}

void CRFLAnimalQL::StateValueMax(){

}

void CRFLAnimalQL::StateValueMin(){

}

void CRFLAnimalQL::CalcQValueDash(){

}

void CRFLAnimalQL::FitnessFromQValue(){

}

void CRFLAnimalQL::FitnessFromTimesOfUse(){

}

