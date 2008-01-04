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
#include <vector>
#include <ctime>
#include <cmath>

#include "CRController.h"
#include "CRFLAnimalGene.h"

////////////////////////////////////////////////////////
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief Q�w�K���s���N���X
*/
class CRFLAnimalQL: public CRFLAnimalGene{

private:
	//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	//���ϐ��̒�`
	// CRFLAnimalGene����p�������ϐ��iprotected��private���j
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//																										//
	// CRCreatureIf*			creature;		//< �w�K���s���N���[�`���[�Q								//
	// std::vector<CRBodyIf*>	crBody;			//< creature�ɓo�^����Ă���{�f�B���iX�́j���i�[����z��	//
	//																										//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	std::vector<int>					action;			//< �e�֐߂̎w�߂�����z��
	std::vector<int>					actionNumber;	//< �e�֐߂̎�蓾��A�N�V������������z��
	std::vector<CRFLAnimalGeneData>		thisTermGene;	//< ����Step()�ł̈�`�q
	
	double					learningRate;	//<	�w�K��
	double					decreaseRate;	//< �������i�G�[�W�F���g�̖Y�p���j
	double					reward;			//< ��V
	double					qValue;			//< Q�l
	double					qValueDash;		//< �K�i�����ꂽQ�l
	double					penalty;		//< �K�i���̍ۂɎg�p���锱�̒l

	//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	//�B�������֐�
	void		BoltzmannSelection();									//< �{���c�}���I��
	void		EpsilonGreedySelection();								//< ��-greedy�I��

	void		SetActionNumber(std::vector<CRFLAnimalGeneData> aGene); //< �����`�q�̊e�֐߂���蓾��s���̃p�^�[��������ݒ肷��
	void		SelectAction(std::vector<CRFLAnimalGeneData> aGene);	//< �s�������肷��
	void		TakeAction(std::vector<CRFLAnimalGeneData> aGene);		//< �A�N�V���������s����	
	void		EvaluativeFunc();										//< �]���֐�
	void		CalcQValueMax();										//< action��ϓ�����������Q�l�̍ő�l�𒲂ׂ�
	void		CalcQValueMin();										//< action��ϓ�����������Q�l�̍ŏ��l�𒲂ׂ�
	void		UpdateQValues();										//< Q�l�̍X�V���s��
	void		StateValueMax();										//< �e��Ԃ̉��l�̍ő�l�����߂�
	void		StateValueMin();										//< �e��Ԃ̉��l�̍ŏ��l�����߂�
	void		CalcQValueDash();										//< Q�l�̋K�i�������邱�Ƃ�Q'�����߂�
	void		FitnessFromQValue();									//< Q�l�̍������̂̓K���x����������
	void		FitnessFromTimesOfUse();								//< �g�p�p�x�̍������̂̓K���x����������

public:

	// ���ϐ��̒�`
	//CRFLAnimalGene����p�������ϐ�(public)
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//																																			   //
	// std::vector< std::vector<CRFLAnimalGeneData> > flAnimalGenes;	//< ��`�q�istd::vector<CRFLAnimalGeneData�j�̔z��istd::vector<*******>�j //
	//																																			   //
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	//�R���X�g���N�^
	CRFLAnimalQL(CRCreatureIf* charactors){
		creature = charactors;
		Init();
	}
	//�f�X�g���N�^
	~CRFLAnimalQL(){
	}
	
	//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
		
	//�C���^�t�F�[�X�̎���

	// CRFLAnimalGene����p������Ă���C���^�t�F�[�X
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//																																//
	// <<��`�q���������֐�>>																										//
	//																																//
	// @brief ��̃{�f�B�ɂ��č����݂̎p��������������`�q������ĕԂ�														//
	// std::vector<CRFLAnimalGeneData> CreateGene(CRBodyIf* body);																	//
	//																																//
	// @brief ����{�f�B�̌��ݎ���Ă���p����\����`�q��Ԃ�																		//
	// std::vector<CRFLAnimalGeneData> GetGene(CRBodyIf* body);																		//
	//																																//
	// @brief ����{�f�B�̈�ԐV������`�q�̎p�����w�肵�����̂ɏ���������															//
	// void SetGene(std::vector<CRFLAnimalGeneData> gene, CRBodyIf* body);															//
	//																																//
	// @brief �����`�qA�ƕʂ̂����`�qB�����鏊�������ւ��āi��_�����j�V������`�q�����										//
	// std::vector<CRFLAnimalGeneData> MixGenes(std::vector<CRFLAnimalGeneData> geneA, std::vector<CRFLAnimalGeneData> geneB);		//
	//																																//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/** @brief ���������s��
	*/
	void Init();
	
	/** @brief Q�w�K��i�߂�
	*/
	void Step();	
	
};

#endif