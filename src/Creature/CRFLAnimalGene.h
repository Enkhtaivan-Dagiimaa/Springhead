#ifndef FLANIMALGENE_H
#define FLANIMALGENE_H

#include <Springhead.h>
#include <Creature/CRFourLegsAnimalBody.h>
#include <vector>

#include "CRController.h"
#include "CRFLAnimalGeneData.h"

// --- memo --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --
// �Ƃ肠�����A�ЂƂ̃{�f�B�ɑ΂��Ċw�K�ł���悤�ɂ����Ή����Ă��܂���B
// �����̂̊w�K���������ꍇ�ɂ͗v�f�͎���Ă��Ă���̂Ŏ����ō���Ēǉ����Ă��������B
// flAnimalGenes�͈�`�q�̓����z��ƊO���猩�����̔z�񂪂ł��Ă���̂ŁA��̓{�f�B���Ƃ�
// �������������΂����Ǝv���܂��B
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- 
class CRFLAnimalGene{

private:
	CRCreatureIf*					creature;					//< ����Ώۂ̃N���[�`���[�Q
	std::vector<CRBodyIf*>		    crBody;						//< creature�ɓo�^����Ă���{�f�B�����i�[����z��
	std::vector< std::vector<CRFLAnimalGeneData> > flAnimalGenes; //< ��`�q�istd::vector<CRFLAnimalGeneData�j�̔z��istd::vector<*******>�j
public:
	
	// �R���X�g���N�^
	
	CRFLAnimalGene(CRCreatureIf* charactors){
		creature = charactors;
		Init();
	}
	~CRFLAnimalGene(){
	}

	std::vector<CRFLAnimalGeneData> CreateGene(CRBodyIf* body);
	std::vector<CRFLAnimalGeneData> GetGene(CRBodyIf* body);
	
	void SetGene(std::vector<CRFLAnimalGeneData> gene, CRBodyIf* body);
	void Init();
	void MixGenes(std::vector<CRFLAnimalGeneData> geneA, std::vector<CRFLAnimalGeneData> geneB);

};

#endif