/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include <ctime>
#include <cmath>
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
					(*aGene)[j].goalDir[0] += Rad(3 * moveRatio);
				}
				else if(action[j] == 1){
//					DSTR << "The joint " << j << " is keep its position" << std::endl;
				}
				else if(action[j] == 2){
					(*aGene)[j].goalDir[0] -= Rad(3 * moveRatio);
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
	
	  double valueY = centerOfMass[1];
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

