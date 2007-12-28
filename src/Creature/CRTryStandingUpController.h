/*
 *  Copyright (c) 2003-2007, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
//////////////////////////////////////////////////////

#ifndef CRTRYSTANDINGUPCONTROLLER_H
#define CRTRYSTANDINGUPCONTROLLER_H

//////////////////////////////////////////////////////

#include <Springhead.h>
#include <Foundation/Object.h>

#include "IfStubCreature.h"
#include "CRController.h"
#include "CRFLAnimalGene.h"

//////////////////////////////////////////////////////

//@{
namespace Spr{;
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief �|�ꂽ�痧�Ƃ��Ɠw�͂���R���g���[��
*/
class CRTryStandingUpController : public CRController, public CRTryStandingUpControllerIfInit, public CRTryStandingUpControllerDesc {

private:

	//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	// �ϐ��錾

	///////////////////////////////////////////////////////////////////
	//																 //
	// <<<CRController����p������Ă��̒��ł��łɎg����ϐ�>>>>	 //
	//																 //
	// CRCreatureIf*	creature;   //< ����Ώۂ̃N���[�`���[�Q	 //
	// PHSceneIf*		phScene;    //< ��������V�[��				 //
	// bool				isEnabled;  //< �L�����ǂ���				 //
	//																 //
	///////////////////////////////////////////////////////////////////

	// ���̃N���X�Ƃ��̔h���N���X�i���͂܂��Ȃ��j�Ŏg����ϐ�
	std::vector<PHSolidIf*>							foot;				//< �����\�����鍄�̂��i�[���Ă����z��
	std::vector<CRBodyIf*>							body;				//< ����Ώۂ̃{�f�B�������z��
	CRFLAnimalGene*									animalGeneIf;		//< ��`�q��������邽�߂̃C���^�t�F�[�X
	std::vector< std::vector<CRFLAnimalGeneData> >	animalGenes;		//< �����̈�`�q�������z��

	unsigned long									totalStep;			//< �V�~�����[�V�����J�n������̃X�e�b�v��
	Vec3d											centerOfMass;		//< �{�f�B�S�̂̏d�S
	
	// human foot positions	
	Vec3d											rightFootPos;		//< �E���̈ʒu
	Vec3d											leftFootPos;		//< �����̈ʒu
	
	// animal foot positions	
	Vec3d											rightFrontFootPos;	//< �E�O���̈ʒu
	Vec3d											rightRearFootPos;	//< �E�㑫�̈ʒu
	Vec3d											leftFrontFootPos;	//< ���O���̈ʒu
	Vec3d											leftRearFootPos;	//< ���㑫�̈ʒu

	// human foot forces
	Vec3d											rightFootForce;
	Vec3d											leftFootForce;
	
	// animal foot forces
	Vec3d											rightFrontFootForce;
	Vec3d											rightRearFootForce;
	Vec3d											leftFrontFootForce;
	Vec3d											leftRearFootForce;
	
	//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	// �֐��錾

	/** @brief ���̍��̂��w�肷��B
		@param body �����t���Ă���̂̃C���^�t�F�[�X�^
	*/
	std::vector<PHSolidIf*>		SetFootSolid(CRBodyIf* body);

	/** @brief ���̍��W��Ԃ�
		@param footSolid ���̍��̂̃C���^�t�F�[�X�^
	*/
	Vec3d						GetFootPos(PHSolidIf* footSolid);
	
	/** @briaf ���ɂ������Ă���͂��v�Z����
		@param footSolid ���̍��̂̃C���^�t�F�[�X�^
	*/
	Vec3d						CalcFootForce(PHSolidIf* footSolid);

	/** @brief ��`�q�Ɋi�[����Ă���ڕW�p�x��񂩂�{�f�B�����ۂɓ�����
		@param gene �����`�q�i�S�Ă̊֐߂̖ڕW�p���i�[���ꂽvector�z��j
	*/
	void						TransitionPoseModel(std::vector<CRFLAnimalGeneData> gene);

	/** @brief �N���[�`���[�B�̃{�f�B�̏��i�d�S, ���̈ʒu, ���ɂ�����́j���X�V����
	*/
	void						UpdateBodyState();

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
	
	/** @brief ���������s��
	*/
	virtual void				Init();

	/** @brief ����̃V�~�����[�V����������
	*/
	virtual void				Step();

	/** @brief Q�w�K����
	*/
	void						QLearning();

};

}
//@}

#endif//CRTRYSTANDINGUPCONTROLLER_H