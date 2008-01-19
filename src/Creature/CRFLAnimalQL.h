/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
///////////////////////////////////////////////////////
#ifndef CRFLANIMALQL_H
#define CRFLANIMALQL_H
///////////////////////////////////////////////////////

#include <Springhead.h>
#include <Creature/CRFourLegsAnimalBody.h>
#include <Creature/CRHingeHumanBody.h>
#include <vector>
#include <ctime>
#include <cmath>

#include "CRController.h"
#include "CRFLAnimalGene.h"

////////////////////////////////////////////////////////
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief Q�w�K���s���N���X
*/
class CRFLAnimalQL: public UTRefCount{

protected:
	//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	//���ϐ��̒�`	
	CRCreatureIf*			creature;
	std::vector<CRBodyIf*>  crBody;
	std::vector< std::vector<double> >	qValue;
	std::vector< std::vector<double> >	qValueDash;
	
	std::vector<int>					action;			//< �e�֐߂̎w�߂�����z��
	std::vector<int>					actionNumber;	//< �e�֐߂̎�蓾��A�N�V������������z��
	
	double					learningRate;	//<	�w�K��
	double					decreaseRate;	//< �������i�G�[�W�F���g�̖Y�p���j
	double					reward;			//< ��V
	double					penalty;		//< �K�i���̍ۂɎg�p���锱�̒l
	double					moveRatio;		//< �s���̕��idegree�j

	Vec3d					vEvaluateHC;	//< ��̂̎p����]�����邽�߂̃x�N�g���i�����狹�ւ̃x�N�g���̔w�������ւ̐��������x�N�g���j
	Vec3d					vEvaluateMF;	//< �x���r���p�`���d�S�̒��ɂ��邩�ǂ����𔻕ʂ���x�N�g���i�d�S����4���̑����̂̒��S����镽�ʂւ̖@���x�N�g���j
	
	//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	//CRTryStandingUpController����͌ĂׂȂ��֐�
	void		Init();			//< ���������s��

public:
	//�R���X�g���N�^
	CRFLAnimalQL(){
		Init();
	}
	CRFLAnimalQL(CRCreatureIf* charactors){
		creature = charactors;
		Init();
	}
	//�f�X�g���N�^
	~CRFLAnimalQL(){
	}
		
	//�C���^�t�F�[�X�̎���
	//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	//CRTryStandingUpController����Ăׂ�C���^�t�F�[�X�֐�
	void		BoltzmannSelection();										//< �{���c�}���I��
	void		EpsilonGreedySelection();									//< ��-greedy�I��
	void		SetActionNumber(std::vector<CRFLAnimalGeneData> *aGene);	//< �����`�q�̊e�֐߂���蓾��s���̃p�^�[��������ݒ肷��
	void		SelectAction(std::vector<CRFLAnimalGeneData> *aGene);		//< �s�������肷��
	void		TakeAction(std::vector<CRFLAnimalGeneData> *aGene);			//< �A�N�V���������s����		
	void		EvaluativeFunc(Vec3d centerOfMass, 
							   std::vector<Vec3d>footPositions, 
							   std::vector<Vec3d> footForces);				//< �]���֐�
	void		CalcQValueMax();											//< action��ϓ�����������Q�l�̍ő�l�𒲂ׂ�
	void		CalcQValueMin();											//< action��ϓ�����������Q�l�̍ŏ��l�𒲂ׂ�
	void		UpdateQValues();											//< Q�l�̍X�V���s��
	void		StateValueMax();											//< �e��Ԃ̉��l�̍ő�l�����߂�
	void		StateValueMin();											//< �e��Ԃ̉��l�̍ŏ��l�����߂�
	void		CalcQValueDash();											//< Q�l�̋K�i�������邱�Ƃ�Q'�����߂�
	void		FitnessFromQValue();										//< Q�l�̍������̂̓K���x����������
	void		FitnessFromTimesOfUse();									//< �g�p�p�x�̍������̂̓K���x����������
	
};

#endif