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
class CRFLAnimalQL: public UTRefCount{

private:
	//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	//���ϐ��̒�`
	CRCreatureIf*			creature;		//< �w�K���s���N���[�`���[�Q
	std::vector<CRBodyIf*>	crBody;			//< creature�ɓo�^����Ă���{�f�B���iX�́j���i�[����z��
	std::vector<int>		actions;		//< �e�֐߂̎w�߂�����z��
	std::vector<int>		actionNumber;	//< �e�֐߂̎�蓾��A�N�V������������z��
	
	double					learningRate;	//<	�w�K��
	double					decreaseRate;	//< �������i�G�[�W�F���g�̖Y�p���j
	double					qValue;			//< Q�l
	double					qValueDash;		//< �K�i�����ꂽQ�l
	double					penalty;		//< �K�i���̍ۂɎg�p���锱�̒l

	//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	//�B�������֐�
	void		BoltzmannSelection();		//< �{���c�}���I��
	void		EpsilonGreedySelection();	//< ��-greedy�I��
	void		SelectAction();				//< �s�������肷��
	void		TakeAction();				//< �A�N�V���������s����
	void		EvaluativeFunc();			//< �]���֐�
	void		UpdateQValues();			//< Q�l�̍X�V���s��
	void		StateValueMax();			//< �e��Ԃ̉��l�̍ő�l�����߂�
	void		StateValueMin();			//< �e��Ԃ̉��l�̍ŏ��l�����߂�
	void		CalcQValueDash();			//< Q�l�̋K�i�������邱�Ƃ�Q'�����߂�
	void		FitnessFromQValue();		//< Q�l�̍������̂̓K���x����������
	void		FitnessFromTimesOfUse();	//< �g�p�p�x�̍������̂̓K���x����������

public:
	
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
	/** @brief ���������s��
	*/
	void Init();
	
	/** @brief Q�w�K��i�߂�
	*/
	void Step();
	
	
};

#endif