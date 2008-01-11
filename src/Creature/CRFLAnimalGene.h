/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/////////////////////////////////////////////////////////
#ifndef FLANIMALGENE_H
#define FLANIMALGENE_H
/////////////////////////////////////////////////////////
#include <Springhead.h>
#include <Creature/CRFourLegsAnimalBody.h>
#include <vector>
#include <ctime>
#include <cmath>

#include "CRController.h"
#include "CRFLAnimalGeneData.h"

// --- memo --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --
// �Ƃ肠�����A�ЂƂ̃{�f�B�ɑ΂��Ċw�K�ł���悤�ɂ����Ή����Ă��܂���B
// �����̂̊w�K���������ꍇ�ɂ͗v�f�͎���Ă��Ă���̂Ŏ����ō���Ēǉ����Ă��������B
// flAnimalGenes�͈�`�q�̓����z��ƊO���猩�����̔z�񂪂ł��Ă���̂ŁA
// ��̓{�f�B���Ƃɂ������������΂����Ǝv���܂��B
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

// std::vector<CRFLAnimalGeneData>: "�����`�q�ЂƂ�"�^

/////////////////////////////////////////////////////////
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief ��`�q�֌W�̂��Ƃ����d�؂�N���X
*/
class CRFLAnimalGene: public UTRefCount{

private:
	CRCreatureIf*					creature;						//< ����Ώۂ̃N���[�`���[�Q
	std::vector<CRBodyIf*>		    crBody;							//< creature�ɓo�^����Ă���{�f�B���(X��)���i�[����z��
	
public:
	std::vector< std::vector<CRFLAnimalGeneData> > flAnimalGenes;	//< ��`�q�istd::vector<CRFLAnimalGeneData�j�̔z��istd::vector<*******>�j

	// �R���X�g���N�^
	CRFLAnimalGene(){
		//DSTR << "This Constractor is called when a object which is CRFLAnimalQL class has created." << std::endl;
	}
	CRFLAnimalGene(CRCreatureIf* charactors){
		creature = charactors;
		Init();
	}
	//�f�X�g���N�^
	~CRFLAnimalGene(){
	}

	//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	// �R���X�g���N�^�݂����Ȃ���
	/** @brief �N���X�������ɏ��������s���֐�
	*/
	virtual void Init();

	//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	//��`�q���������֐�

	/** @brief ��̃{�f�B�ɂ��č����݂̎p��������������`�q������ĕԂ�
	*/
	std::vector<CRFLAnimalGeneData> CreateGene(CRBodyIf* body);

	/** @brief ����{�f�B�̌��ݎ���Ă���p����\����`�q��Ԃ�
	*/
	std::vector<CRFLAnimalGeneData> GetGene(CRBodyIf* body);
	
	/** @brief ����{�f�B�̈�ԐV������`�q�̎p�����w�肵�����̂ɏ���������
	*/
	void SetGene(std::vector<CRFLAnimalGeneData> gene, CRBodyIf* body);
	
	/** @brief �����`�qA�ƕʂ̂����`�qB�����鏊�������ւ��āi��_�����j�V������`�q�����
	*/
	std::vector<CRFLAnimalGeneData> MixGenes(std::vector<CRFLAnimalGeneData> geneA, std::vector<CRFLAnimalGeneData> geneB);

};

#endif