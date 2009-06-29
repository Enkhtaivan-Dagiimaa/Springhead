/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRFOURLEGSANIMALBODYGEN_H
#define CRFOURLEGSANIMALBODYGEN_H

#include <Springhead.h>

#include <Foundation/Object.h>
#include <Physics/PHConstraint.h>
#include <Physics/PhysicsDecl.hpp>

#include <vector>

#include "CRBodyGen.h"

//@{
namespace Spr{;

/// 4���������f���̃f�X�N���v�^
struct CRFourLegsAnimalBodyGenDesc : CRBodyGenDesc {

	enum CRAnimalSolids{
		// Center part of the solids
		SO_WAIST=0,
		SO_CHEST, SO_TAIL1, SO_TAIL2, SO_TAIL3,
		SO_NECK, SO_HEAD, 

		// -- Left part of the solids
		SO_LEFT_BREASTBONE, SO_LEFT_RADIUS, SO_LEFT_FRONT_CANNON_BONE, SO_LEFT_FRONT_TOE,
		SO_LEFT_FEMUR , SO_LEFT_TIBIA, SO_LEFT_REAR_CANNON_BONE, SO_LEFT_REAR_TOE,
		
		// -- Right part of the solids
		SO_RIGHT_BREASTBONE, SO_RIGHT_RADIUS, SO_RIGHT_FRONT_CANNON_BONE, SO_RIGHT_FRONT_TOE,
		SO_RIGHT_FEMUR , SO_RIGHT_TIBIA, SO_RIGHT_REAR_CANNON_BONE, SO_RIGHT_REAR_TOE,

		// -- The number of the solids
		SO_NSOLIDS
	};

	enum CRAnimalJoints{

		//////////////////////////////////////////////////////////////////
		//																//
		// < �{�f�B�̒�`�̏��� >										//
		// PHBallJoint��PHHingeJoint�ɂ��Ă���ƈ�`�q��g�񂾎���		//
		// ���܂���_�������Ȃ��Ȃ��Ă��܂��̂ō����Œ�`���Ă���		//
		//																//
		//////////////////////////////////////////////////////////////////

		// -- Center part of the joints
		JO_WAIST_CHEST=0,
		JO_CHEST_NECK,
		JO_NECK_HEAD,
		JO_WAIST_TAIL, JO_TAIL_12, JO_TAIL_23,

		// -- Left part of the joints
		JO_LEFT_SHOULDER, JO_LEFT_ELBOW, JO_LEFT_FRONT_KNEE, JO_LEFT_FRONT_ANKLE,
		JO_LEFT_HIP, JO_LEFT_STIFLE, JO_LEFT_REAR_KNEE, JO_LEFT_REAR_ANKLE,

		// -- Right part of the joints
		JO_RIGHT_SHOULDER,JO_RIGHT_ELBOW, JO_RIGHT_FRONT_KNEE, JO_RIGHT_FRONT_ANKLE,
		JO_RIGHT_HIP, JO_RIGHT_STIFLE, JO_RIGHT_REAR_KNEE, JO_RIGHT_REAR_ANKLE,

		// -- The number of the all joints (ball + hinge)
		JO_NJOINTS								//(nHingeJoints = nJoints - nBallJoints - 1)
	};

	enum CRAnimalLegs{
		LEG_RIGHT_FRONT=0, LEG_LEFT_FRONT,
		LEG_RIGHT_REAR, LEG_LEFT_REAR
	};

	int joNBallJoints;
	int joNHingeJoints;

	/// �T�C�Y�Ɋւ���p�����[�^
	double waistBreadth,	       waistHeight,			  waistThickness;
	double chestBreadth,	       chestHeight,			  chestThickness;
	double tailBreadth,		       tailHeight,		      tailThickness;
	double neckBreadth,		       neckHeight,			  neckThickness;
	double headBreadth,		       headHeight,			  headThickness;
	double breastboneBreadth,      breastboneHeight,	  breastboneThickness;
	double radiusBreadth,	       radiusHeight,		  radiusThickness;
	double frontCannonBoneBreadth, frontCannonBoneHeight, frontCannonBoneThickness;
	double frontToeBreadth,		   frontToeHeight,		  frontToeThickness;
	double femurBreadth,		   femurHeight,			  femurThickness;
	double tibiaBreadth,		   tibiaHeight,			  tibiaThickness;
	double rearCannonBoneBreadth,  rearCannonBoneHeight,  rearCannonBoneThickness;
	double rearToeBreadth,		   rearToeHeight,		  rearToeThickness;

	/// �eBallJoint�̃o�l�_���p
	double springWaistChest,   damperWaistChest;	//��-��
	double springWaistTail,	   damperWaistTail;		//��-��
	double springTail,		   damperTail;			//��
	double springChestNeck,	   damperChestNeck;     //��-��
	double springNeckHead,	   damperNeckHead;		//��-��
	double springShoulder,	   damperShoulder;		//��
	double springFrontAnkle,   damperFrontAnkle;	//�����Ɓi�O���j
	double springHip,		   damperHip;			//�K
	double springRearAnkle,    damperRearAnkle;		//�����Ɓi�㑫�j
	
	// �eHingeJoint�̃o�l�_���p
	double springElbow,		   damperElbow;			//�I�i�O���j
	double springFrontKnee,	   damperFrontKnee;		//�G�i�O���j
	double springStifle,	   damperStifle;		//�I�H�i�㑫�j
	double springRearKnee,	   damperRearKnee;		//�G�i�㑫�j
	
	/// HingeJoint���搧��
	Vec2d  rangeElbow;
	Vec2d  rangeFrontKnee;
	Vec2d  rangeStifle;
	Vec2d  rangeRearKnee;
	
	// BallJoint����ڕW
	Quaterniond goalWaistChest;
	Quaterniond goalWaistTail;
	Quaterniond goalTail;
	Quaterniond goalChestNeck;
	Quaterniond goalNeckHead;
	Quaterniond goalShoulder;
	Quaterniond goalFrontAnkle;
	Quaterniond goalHip;
	Quaterniond goalRearAnkle;

	// HingeJoint����ڕW
	double originElbow;
	double originFrontKnee;
	double originStifle;
	double originRearKnee;

	// BallJoint���搧���̒��S
	Vec3d limitDirWaistChest;
	Vec3d limitDirWaistTail;
	Vec3d limitDirTail;
	Vec3d limitDirChestNeck;
	Vec3d limitDirNeckHead;
	Vec3d limitDirShoulder;
	Vec3d limitDirFrontAnkle;
	Vec3d limitDirHip;
	Vec3d limitDirRearAnkle;

	/// BallJoint��swing����:
	Vec2d limitSwingWaistChest;
	Vec2d limitSwingWaistTail;
	Vec2d limitSwingTail;
	Vec2d limitSwingChestNeck;
	Vec2d limitSwingNeckHead;
	Vec2d limitSwingShoulder;
	Vec2d limitSwingFrontAnkle;
	Vec2d limitSwingHip;
	Vec2d limitSwingRearAnkle;

	/// BallJoint��twist����
	Vec2d limitTwistWaistChest;
	Vec2d limitTwistWaistTail;
	Vec2d limitTwistTail;
	Vec2d limitTwistChestNeck;
	Vec2d limitTwistNeckHead;
	Vec2d limitTwistShoulder;
	Vec2d limitTwistFrontAnkle;
	Vec2d limitTwistHip;
	Vec2d limitTwistRearAnkle;

	// �֐߂̏o����͂̍ő�l
	double fMaxWaistChest;
	double fMaxChestNeck;
	double fMaxNeckHead;
	double fMaxWaistTail;
	double fMaxTail12;
	double fMaxTail23;
	double fMaxLeftShoulder;
	double fMaxLeftElbow;
	double fMaxLeftFrontKnee;
	double fMaxLeftFrontAnkle;
	double fMaxLeftHip;
	double fMaxLeftStifle;
	double fMaxLeftRearKnee;
	double fMaxLeftRearAnkle;
	double fMaxRightShoulder;
	double fMaxRightElbow;
	double fMaxRightFrontKnee;
	double fMaxRightFrontAnkle;
	double fMaxRightHip;
	double fMaxRightStifle;
	double fMaxRightRearKnee;
	double fMaxRightRearAnkle;

	// ���̖̂��C�W��
	float materialMu;

	/// ���I�v�V����
	bool noLegs;
	bool noHead;

	/// �_�C�i�~�J�������邩�ǂ���
	bool dynamicalMode;
	/// �S�̂̑̏d
	double totalMass;
	/// fMax�����邩�ǂ���
	bool flagFMax;
	/// �ғ��搧�������邩�ǂ���
	bool flagRange;

	CRFourLegsAnimalBodyGenDesc(bool enableRange = false, bool enableFMax = false);
};



// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
// CRFourLegsAnimalBodyGen
// �{�[����q���W�̃W���C���g��p�����M���ރ��f���E�N���X�̎���
class CRFourLegsAnimalBodyGen : public CRBodyGen, public CRFourLegsAnimalBodyGenDesc {
private:
	
	std::vector<PHSolidIf*> upperBody;		//< ��̂��\�����鍄�̂̓o�^��̔z��
	
	void InitBody();						//< �̊����̍쐬�v��𗧂Ă�
	void CreateWaist();						//< �����쐬����
	void CreateChest();						//< �����쐬����
	void CreateTail();						//< �����쐬����

	void InitHead();						//< �����̍쐬�v��𗧂Ă�
	void CreateNeck();						//< ����쐬����
	void CreateHead();						//< ���̍쐬������

	void InitFrontLegs();					//< �O�r���̍쐬�v��𗧂Ă�
	void CreateBreastBone(LREnum lr);		//< ��r�����쐬����
	void CreateRadius(LREnum lr);			//< �O�r�����쐬����
	void CreateFrontCannonBone(LREnum lr);	//< ���荜���쐬����
	void CreateFrontToeBones(LREnum lr);	//< �w�����쐬����

	void InitRearLegs();					//< ��r���̍쐬�v��𗧂Ă�
	void CreateFemur(LREnum lr);			//< ��ڍ����쐬����
	void CreateTibia(LREnum lr);			//< �������쐬����
	void CreateRearCannonBone(LREnum lr);	//< ���������쐬����
	void CreateRearToeBones(LREnum lr);		//< �䍜���쐬����
	
	void InitEyes();						//< ���o��̍쐬�v��𗧂Ă�i�������j
	void CreateEye(LREnum lr);				//< �ڂ��쐬����i�������j

	void InitContact();						//< �{�f�B�S�Ă̍��̓��m�̐ڐG��؂�
	void InitControlMode(PHJointDesc::PHControlMode m = PHJointDesc::MODE_POSITION);	//< �{�f�B�̐��䃂�[�h��ݒ肷��D
	void SetUpperBody();					//< �{�f�B�̏�̂��\�����鍄�̂�o�^����
	
public:

	CRFourLegsAnimalBodyGen(){}
	CRFourLegsAnimalBodyGen(const CRFourLegsAnimalBodyGenDesc& desc, PHSceneIf* s=NULL) 
		: CRFourLegsAnimalBodyGenDesc(desc) 
		, CRBodyGen((const CRBodyGenDesc&)desc, s)
	{
		solids.resize(CRFourLegsAnimalBodyGenDesc::SO_NSOLIDS);
		for(unsigned int i = 0; i < solids.size(); i++) solids[i] = NULL;
		joints.resize(CRFourLegsAnimalBodyGenDesc::JO_NJOINTS);
		for(unsigned int i = 0; i < joints.size(); i++) joints[i] = NULL;

		InitBody();
		InitHead();
		InitFrontLegs();
		InitRearLegs();
		InitEyes();
		
		InitContact();
		InitControlMode();
		Init();
	}

	// �C���^�t�F�[�X�̎���
	virtual Vec3d	GetUpperCenterOfMass();		//< ��̂̍��̂̏d�S�𓾂�
	virtual int		NBallJoints();				//< �{�f�B�Ɋ܂܂�Ă���{�[���W���C���g�̐���Ԃ�
	virtual int		NHingeJoints();				//< �{�f�B�Ɋ܂܂�Ă���q���W�W���C���g�̐���Ԃ�
	virtual double	VSolid(int i);				//< i�Ԗڂ̍��̂̑̐ς�Ԃ�
	virtual double	VSolids();					//< ���̂̑̐ϑ��a��Ԃ�
	virtual double	GetTotalMass();				//< �����ʂ�Ԃ�
	virtual void	SetTotalMass(double value);	//< �����ʂ�ݒ肷��
	virtual double  GetLegLength(int i);		//< �r�̒�����Ԃ�[0]:�E�O�C[1]:���O, [2]:�E��, [3]:����
};

}
//@}

#endif//CRFOURLEGSANIMALBODYGEN_H
