/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
// <memo>
// human�Ɋւ���ϐ��╡��body�ɂ��ď����Ă��镔��������܂����A��{�I�ɖ�����
// �Ȃ̂ŁA�g�������l���K�X�\�͂��g�����Ă�������(by Toki 2008.01.05)
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

//////////////////////////////////////////////////////
#ifndef CRTRYSTANDINGUPCONTROLLER_H
#define CRTRYSTANDINGUPCONTROLLER_H
//////////////////////////////////////////////////////

#include <Springhead.h>
#include <Foundation/Object.h>

#include "IfStubCreature.h"
#include "CRController.h"
#include "CRFLAnimalGene.h"
#include "CRFLAnimalQL.h"

//////////////////////////////////////////////////////

//@{
namespace Spr{;
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief �|�ꂽ�痧�Ƃ��Ɠw�͂���R���g���[��
*/
class CRTryStandingUpController : public CRController,
								  public CRTryStandingUpControllerIfInit, 
								  public CRTryStandingUpControllerDesc {


private:
	//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	// �ϐ��錾

	///////////////////////////////////////////////////////////////////
	//																 //
	// <<<CRController����p������Ă��̒��ł��łɎg����ϐ�>>>		 //
	//																 //
	// CRCreatureIf*	creature;   //< ����Ώۂ̃N���[�`���[�Q	 //
	// PHSceneIf*		phScene;    //< ��������V�[��				 //
	// bool				isEnabled;  //< �L�����ǂ���				 //
	//																 //
	///////////////////////////////////////////////////////////////////

	// ���̃N���X�Ŏg����ϐ�
	std::vector<PHSolidIf*>							foot;					//< �����\�����鍄�̂��i�[���Ă����z��
	std::vector<CRBodyIf*>							body;					//< ����Ώۂ̃{�f�B�������z��
	UTRef<CRFLAnimalGene>							animalGeneIf;			//< ��`�q��������邽�߂̃C���^�t�F�[�X
	UTRef<CRFLAnimalQL>								animalQLIf;				//< QLearning�����邽�߂̃C���^�t�F�[�X
	std::vector< std::vector<CRFLAnimalGeneData> >	animalGenes;			//< ���鎞�̈�`�qstd::vector<CRFLAnimalGeneData>���W�܂�����`�q�n��std::vector<********>

	unsigned long			totalStep;				//< �V�~�����[�V�����J�n������̃X�e�b�v��
	Vec3d					centerOfMass;			//< �{�f�B�S�̂̏d�S
	
	std::vector<Vec3d>		footPositions;			//< ���̈ʒu���i�[�����z��
	// human foot positions	
	Vec3d					rightFootPos;			//< �E���̈ʒu
	Vec3d					leftFootPos;			//< �����̈ʒu	
	// animal foot positions	
	Vec3d					rightFrontFootPos;		//< �E�O���̈ʒu
	Vec3d					rightRearFootPos;		//< �E�㑫�̈ʒu
	Vec3d					leftFrontFootPos;		//< ���O���̈ʒu
	Vec3d					leftRearFootPos;		//< ���㑫�̈ʒu

	std::vector<Vec3d>		footForces;				//< ���ɂ�����͂��i�[�����z��
	// human foot forces
	Vec3d					rightFootForce;			//< �E���̗�
	Vec3d					leftFootForce;			//< �����̗�	
	// animal foot forces
	Vec3d					rightFrontFootForce;	//< �E�O���̗�
	Vec3d					rightRearFootForce;		//< �E�㑫�̗�
	Vec3d					leftFrontFootForce;		//< ���O���̗�
	Vec3d					leftRearFootForce;		//< ���㑫�̗�
	
	//���J�E���^�[
	int						qLearningStep;			//< GA+QLeaning��1���s���ł�QLearning�̃X�e�b�v��

	//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	// �֐��錾

	std::vector<PHSolidIf*>		SetFootSolid(CRBodyIf* body);			//< ���̍��̂��w�肷��B
	Vec3d						GetFootPos(PHSolidIf* footSolid);		//< ���̍��W��Ԃ�
	Vec3d						CalcFootForce(PHSolidIf* footSolid);	//< ���ɂ������Ă���͂��v�Z����
	Vec3d						CalcFootTorque(PHSolidIf* footSolid);	//< ���ɂ������Ă���g���N���v�Z����
	void						UpdateBodyState();						//< �N���[�`���[�B�̃{�f�B�̏��i�d�S, ���̈ʒu, ���ɂ�����́j���X�V����
	void						MakeLatestGene();						//< ��ԐV������`�q����쐬����
	void						CalcQL();								//< Q�w�K����
	void						CalcGA();								//< GA���v�Z����

public:
	//----------------------------------------------------------
	// DCAST���ł���悤�ɂ��邽�߂̎d�g�݂������Ă���B
	// ����������Ȃ���LNK2019 Error�ɂȂ�B
	OBJECTDEF(CRTryStandingUpController, CRController);

	//----------------------------------------------------------
	// DESC�̏��������瑤�Ƀ��[�h���邽�߂̎d�g�݂������Ă���B
	// ����������Ȃ���LNK2019 Error�ɂȂ�B
	// ��`����N���X�̃R���X�g���N�^�ƍ��킹�ď����K�v������B
	// �܂��A�����̂������ȃR���X�g���N�^���K�v�ł���B
	ACCESS_DESC(CRTryStandingUpController);

	//----------------------------------------------------------
	// �R���X�g���N�^
	CRTryStandingUpController(){
	}
	CRTryStandingUpController(const CRTryStandingUpControllerDesc&		desc, CRCreatureIf*		c = NULL) 
		: CRTryStandingUpControllerDesc(desc) 
		, CRController((const CRControllerDesc&)	desc, c)
	{
		Init();
	}
	
	//----------------------------------------------------------
	//�֐��̃I�[�o�[���C�h��
	////////////////////////////////////////////////////////////////////////
	//																	  //
	// <<CRController�Ŏ�������Ă���֐�>>								  //
	//																	  //
	// virtual void		SetEnable(bool enable); //< �L��/������؂�ւ��� //
	// virtual bool		IsEnabled();			//< �L��/�����̏�Ԃ�Ԃ� //
	//																	  //
	////////////////////////////////////////////////////////////////////////

	//�C���^�t�F�[�X�̎���
	virtual void		Init();							//< ���������s��
	virtual void		Step();							//< ����̃V�~�����[�V����������

};

}
//@}

#endif//CRTRYSTANDINGUPCONTROLLER_H