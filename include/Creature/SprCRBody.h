/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_CRBodyIf_H
#define SPR_CRBodyIf_H

#include <SprFoundation.h>
#include <SprPhysics.h>

namespace Spr{;

//@{

struct PHSolidIf;
struct PHJointIf;

struct CRBodyIf;
struct CRHingeHumanBodyIf;
struct CRFourLegsAnimalBodyIf;

// ------------------------------------------------------------------------------
/// �N���[�`���̃{�f�B���f���̃C���^�[�t�F�C�X
struct CRBodyIf : SceneObjectIf{
	SPR_IFDEF(CRBody);

	/** @brief ���������s��
	*/
	void Init();

	/** @brief ���̂̐��𓾂�
	*/
	int NSolids();

	/** @brief i�Ԗڂ̍��̂𓾂�
	*/
	PHSolidIf* GetSolid(int i);

	/** @brief �֐߂̐��𓾂�
	*/
	int NJoints();

	/** @brief i�Ԗڂ̊֐߂𓾂�
	*/
	PHJointIf* GetJoint(int i);

	/** @brief IK�m�[�h�̐��𓾂�
	*/
	int NIKNodes();

	/** @brief i�Ԗڂ�IK�m�[�h�𓾂�
	*/
	PHIKNodeIf* GetIKNode(int i);

	/** @brief IK����_�̐��𓾂�
	*/
	int NControlPoints();

	/** @brief i�Ԗڂ�IK����_�𓾂�
	*/
	PHIKControlPointIf* GetControlPoint(int i);

	/** @brief �{�f�B�̏d�S���W�𓾂�
	*/
	Vec3d GetCenterOfMass();

	/** @brief �{�f�B�̎��ʂ𓾂�
	*/
	double GetSumOfMass();
};

/// �N���[�`���̃{�f�B���f���̃f�X�N���v�^
struct CRBodyDesc{
	SPR_DESCDEF(CRBody);

	/// �v���O����e���̂ɂ��邩�\�P�b�g����e���̂ɂ��邩
	//enum CRHumanJointOrder{
	enum CRCreatureJointOrder{
		SOCKET_PARENT,
		PLUG_PARENT,
	} jointOrder;

	CRBodyDesc(){
	}
};

// ------------------------------------------------------------------------------
/// �q���W�W���C���g�l�̃��f���̃C���^�[�t�F�C�X
struct CRHingeHumanBodyIf : CRBodyIf {
	SPR_IFDEF(CRHingeHumanBody);

	/** @brief �㔼�g�̊֐߂̌�����ς���
		@param stifness �����F�f�t�H���g�̌����ɑ΂���{���Ŏw��
	*/
	void SetUpperBodyStiffness(float stiffness);

	/** @brief �㔼�g�̃|�[�Y��ێ�����
	*/
	void KeepUpperBodyPose();

	/** @brief �㔼�g�̃|�[�Y�������ʒu�ɖ߂�
	*/
	void ResetUpperBodyPose();
};

/// �q���W�W���C���g�l�̃��f���̃f�X�N���v�^
struct CRHingeHumanBodyDesc : CRBodyDesc {
	SPR_DESCDEF(CRHingeHumanBody);

	enum CRHumanSolids{
		// Center
		SO_WAIST=0,
		SO_ABDOMEN, SO_CHEST, SO_NECK, SO_HEAD,

		// Right
		SO_RIGHT_UPPER_ARM, SO_RIGHT_LOWER_ARM, SO_RIGHT_HAND,
		SO_RIGHT_UPPER_LEG, SO_RIGHT_LOWER_LEG, SO_RIGHT_FOOT,
		SO_RIGHT_EYE,

		// Left
		SO_LEFT_UPPER_ARM, SO_LEFT_LOWER_ARM, SO_LEFT_HAND,
		SO_LEFT_UPPER_LEG, SO_LEFT_LOWER_LEG, SO_LEFT_FOOT,
		SO_LEFT_EYE,

		// �֐߂̒��p���p�̌`���L���Ȃ�����
		SO_CHEST_NECK_XZ, SO_CHEST_NECK_ZY,
		SO_NECK_HEAD_XZ,

		SO_RIGHT_SHOULDER_ZX, SO_RIGHT_SHOULDER_XY, 
		SO_RIGHT_WRIST_YX, SO_RIGHT_WRIST_XZ,
		SO_RIGHT_WAIST_LEG_ZX, SO_RIGHT_WAIST_LEG_XY, 
		SO_RIGHT_ANKLE_YX, SO_RIGHT_ANKLE_XZ,
		SO_RIGHT_EYE_YX,

		SO_LEFT_SHOULDER_ZX, SO_LEFT_SHOULDER_XY, 
		SO_LEFT_WRIST_YX, SO_LEFT_WRIST_XZ, 
		SO_LEFT_WAIST_LEG_ZX, SO_LEFT_WAIST_LEG_XY, 
		SO_LEFT_ANKLE_YX, SO_LEFT_ANKLE_XZ,
		SO_LEFT_EYE_YX,

		// ���̂̐�
		SO_NSOLIDS
	};

	enum CRHumanJoints{
		// -- Center
		JO_WAIST_ABDOMEN=0, JO_ABDOMEN_CHEST,
		JO_CHEST_NECK_X, JO_CHEST_NECK_Z, JO_CHEST_NECK_Y,
		JO_NECK_HEAD_X, JO_NECK_HEAD_Z,

		// -- Right
		JO_RIGHT_SHOULDER_Z, JO_RIGHT_SHOULDER_X, JO_RIGHT_SHOULDER_Y,
		JO_RIGHT_ELBOW,
		JO_RIGHT_WRIST_Y, JO_RIGHT_WRIST_X, JO_RIGHT_WRIST_Z,

		JO_RIGHT_WAIST_LEG_Z, JO_RIGHT_WAIST_LEG_X, JO_RIGHT_WAIST_LEG_Y,
		JO_RIGHT_KNEE,
		JO_RIGHT_ANKLE_Y, JO_RIGHT_ANKLE_X, JO_RIGHT_ANKLE_Z,

		JO_RIGHT_EYE_Y, JO_RIGHT_EYE_X,

		// -- Left
		JO_LEFT_SHOULDER_Z, JO_LEFT_SHOULDER_X, JO_LEFT_SHOULDER_Y,
		JO_LEFT_ELBOW,
		JO_LEFT_WRIST_Y, JO_LEFT_WRIST_X, JO_LEFT_WRIST_Z,

		JO_LEFT_WAIST_LEG_Z, JO_LEFT_WAIST_LEG_X, JO_LEFT_WAIST_LEG_Y,
		JO_LEFT_KNEE,
		JO_LEFT_ANKLE_Y, JO_LEFT_ANKLE_X, JO_LEFT_ANKLE_Z,

		JO_LEFT_EYE_Y, JO_LEFT_EYE_X,

		// �֐߂̐�
		JO_NJOINTS
	};

	/// �̏d
	double bodyMass;

	/// �T�C�Y�Ɋւ���p�����[�^
	double waistHeight, waistBreadth, waistThickness;
	double abdomenHeight, abdomenBreadth, abdomenThickness;
	double chestHeight, chestBreadth, chestThickness;
	double neckLength, neckDiameter;
	double headDiameter;
	double upperArmLength, upperArmDiameter;
	double lowerArmLength, lowerArmDiameter;
	double handLength, handBreadth, handThickness;
	double upperLegLength, upperLegDiameter, interLegDistance;
	double lowerLegLength, lowerLegDiameter;
	double footLength, footBreadth, footThickness, ankleToeDistance;
	double vertexToEyeHeight, occiputToEyeDistance;
	double eyeDiameter, interpupillaryBreadth;

	/// �֐߃o�l�_���p�W��
	double spring, damper;

	/// �e�֐߂̃o�l�_���p
	double springWaistAbdomen, damperWaistAbdomen;
	double springAbdomenChest, damperAbdomenChest;
	double springChestNeckX,   damperChestNeckX;
	double springChestNeckY,   damperChestNeckY;
	double springChestNeckZ,   damperChestNeckZ;
	double springNeckHeadX,    damperNeckHeadX;
	double springNeckHeadZ,    damperNeckHeadZ;
	double springShoulderZ,    damperShoulderZ;
	double springShoulderX,    damperShoulderX;
	double springShoulderY,    damperShoulderY;
	double springElbow,        damperElbow;
	double springWristY,       damperWristY;
	double springWristX,       damperWristX;
	double springWristZ,       damperWristZ;
	double springWaistLegZ,    damperWaistLegZ;
	double springWaistLegX,    damperWaistLegX;
	double springWaistLegY,    damperWaistLegY;
	double springKnee,         damperKnee;
	double springAnkleY,       damperAnkleY;
	double springAnkleX,       damperAnkleX;
	double springAnkleZ,       damperAnkleZ;
	double springEyeY,         damperEyeY;
	double springEyeX,         damperEyeX;

	/// �֐ߎ��t���p�x�E�ʒu
	Vec3d       posRightUpperArm;
	Quaterniond oriRightUpperArm;
	Quaterniond oriRightLowerArm;
	Quaterniond oriRightHand;

	/// ���搧��
	Vec2d rangeWaistAbdomen;
	Vec2d rangeAbdomenChest;
	Vec2d rangeChestNeckX;
	Vec2d rangeChestNeckY;
	Vec2d rangeChestNeckZ;
	Vec2d rangeNeckHeadX;
	Vec2d rangeNeckHeadZ;
	Vec2d rangeShoulderZ;
	Vec2d rangeShoulderX;
	Vec2d rangeShoulderY;
	Vec2d rangeElbow;
	Vec2d rangeWristY;
	Vec2d rangeWristX;
	Vec2d rangeWristZ;
	Vec2d rangeWaistLegZ;
	Vec2d rangeWaistLegX;
	Vec2d rangeWaistLegY;
	Vec2d rangeKnee;
	Vec2d rangeAnkleY;
	Vec2d rangeAnkleX;
	Vec2d rangeAnkleZ;
	Vec2d rangeRightEyeY;
	Vec2d rangeEyeX;

	/// ���I�v�V����
	bool noLegs;

	CRHingeHumanBodyDesc();
};


// ------------------------------------------------------------------------------
/// �{�[���W���C���g�l�̃��f���̃C���^�[�t�F�C�X
struct CRBallHumanBodyIf : CRBodyIf {
	SPR_IFDEF(CRBallHumanBody);
};

/// �{�[���W���C���g�l�̃��f���̃f�X�N���v�^
struct CRBallHumanBodyDesc : CRBodyDesc {
	SPR_DESCDEF(CRBallHumanBody);

	enum CRHumanSolids{
		// Center
		SO_WAIST=0,
		SO_ABDOMEN, SO_CHEST, SO_NECK, SO_HEAD,

		// Right
		SO_RIGHT_UPPER_ARM, SO_RIGHT_LOWER_ARM, SO_RIGHT_HAND,
		SO_RIGHT_UPPER_LEG, SO_RIGHT_LOWER_LEG, SO_RIGHT_FOOT,
		SO_RIGHT_EYE,

		// Left
		SO_LEFT_UPPER_ARM, SO_LEFT_LOWER_ARM, SO_LEFT_HAND,
		SO_LEFT_UPPER_LEG, SO_LEFT_LOWER_LEG, SO_LEFT_FOOT,
		SO_LEFT_EYE,

		// ���̂̐�
		SO_NSOLIDS
	};

	enum CRHumanJoints{
		// -- Center
		JO_WAIST_ABDOMEN=0, JO_ABDOMEN_CHEST,
		JO_CHEST_NECK, JO_NECK_HEAD,

		// -- Right
		JO_RIGHT_SHOULDER, JO_RIGHT_ELBOW, JO_RIGHT_WRIST,
		JO_RIGHT_WAIST_LEG, JO_RIGHT_KNEE, JO_RIGHT_ANKLE,
		JO_RIGHT_EYE,

		// -- Left
		JO_LEFT_SHOULDER, JO_LEFT_ELBOW, JO_LEFT_WRIST,
		JO_LEFT_WAIST_LEG, JO_LEFT_KNEE, JO_LEFT_ANKLE,
		JO_LEFT_EYE,

		// �֐߂̐�
		JO_NJOINTS
	};

	/// �̏d
	double bodyMass;

	/// �T�C�Y�Ɋւ���p�����[�^
	double waistLength, waistBreadth;
	double abdomenLength, abdomenBreadth;
	double chestLength, chestBreadth, bideltoidBreadth;
	double neckLength, neckDiameter, neckPosZ, neckPosY;
	double headBreadth, headHeight, bigonialBreadth;
	double upperArmLength, upperArmDiameter;
	double lowerArmLength, lowerArmDiameter;
	double handLength, handBreadth, handThickness;
	double upperLegLength, upperLegDiameter, interLegDistance, legPosY;
	double lowerLegLength, lowerLegDiameter, kneeDiameter, ankleDiameter;
	double calfPosY, calfPosZ, calfDiameter;
	double footLength, footBreadth, footThickness, ankleToeDistance;
	double vertexToEyeHeight, occiputToEyeDistance;
	double eyeDiameter, interpupillaryBreadth;

	/// �e�֐߂̃o�l�_���p
	double springWaistAbdomen, damperWaistAbdomen;
	double springAbdomenChest, damperAbdomenChest;
	double springChestNeck,    damperChestNeck;
	double springNeckHead,     damperNeckHead;
	double springShoulder,     damperShoulder;
	double springElbow,        damperElbow;
	double springWrist,        damperWrist;
	double springWaistLeg,     damperWaistLeg;
	double springKnee,         damperKnee;
	double springAnkle,        damperAnkle;
	double springEye,          damperEye;

	// ���搧���Ȃǁi�������j

	CRBallHumanBodyDesc();
};


// ------------------------------------------------------------------------------
/// �S���������f���̃C���^�[�t�F�C�X
struct CRFourLegsAnimalBodyIf : CRBodyIf {
	SPR_IFDEF(CRFourLegsAnimalBody);

	/** @brief �����������s����
	*/
	void Init();
	
	/** @brief ��Ԃ̏d�S���W��Ԃ�
	*/
	Vec3d GetUpperCenterOfMass();

	/** @brief �{�[���W���C���g�̐���Ԃ�
	*/
	int NBallJoints();

	/** @brief �q���W�W���C���g�̐���Ԃ�
	*/
	int NHingeJoints();
	/** @brief i�Ԗڂ̍��̂̑̐ς�Ԃ�
	*/
	double VSolid(int i);
	/** @brief ���̂̑̐ϑ��a��Ԃ�
	*/
	double VSolids();
	/** @brief �{�f�B�̑����ʂ�Ԃ�
	*/
	double	GetTotalMass();
	/** @brief �{�f�B�̑����ʂ�ݒ肷��
	*/
	void	SetTotalMass(double value);
	/** @brief �{�f�B�̋r�̒�����Ԃ�
	*/
	double GetLegLength(int i);

};

/// 4���������f���̃f�X�N���v�^
struct CRFourLegsAnimalBodyDesc : CRBodyDesc {
	SPR_DESCDEF(CRFourLegsAnimalBody);

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

	CRFourLegsAnimalBodyDesc(bool enableRange = false, bool enableFMax = false);
};

// ------------------------------------------------------------------------------
/// �f�o�b�O�p�̃w�r�󃂃f���̃C���^�[�t�F�C�X
struct CRDebugLinkBodyIf : CRBodyIf {
	SPR_IFDEF(CRDebugLinkBody);

	/** @brief �{�[���W���C���g�̐���Ԃ�
	*/
	int NBallJoints();

	/** @brief �q���W�W���C���g�̐���Ԃ�
	*/
	int NHingeJoints();
	
};

/// �f�o�b�O�p�̃w�r�󃂃f���̃f�X�N���v�^
struct CRDebugLinkBodyDesc : CRBodyDesc {
	
	SPR_DESCDEF(CRDebugLinkBody);

	enum CDShapeMode{
		MODE_SPHERE=0, MODE_CAPSULE, MODE_BOX
	} shapeMode;

	enum PHJointMode{
		MODE_BALL=0, MODE_HINGE
	} jointMode;

	int soNSolids;
	int joNBallJoints;
	int joNHingeJoints;
	int joNJoints;

	/// �T�C�Y�Ɋւ���p�����[�^(radius, length)
	double radius, length;

	/// �����N��̏d��
	double mass;

	/// �eBallJoint�̃o�l�_���p
	double spring, damper;

	/// HingeJoint���搧��
	Vec2d  range;
	
	// BallJoint����ڕW
	Quaterniond goal;

	/// BallJoint��swing����:
	Vec2d limitSwing;

	/// BallJoint��twist����
	Vec2d limitTwist;

	// �֐߂̏o����͂̍ő�l
	double fMax;

	/// �_�C�i�~�J�������邩�ǂ���
	bool dynamicalMode;

	CRDebugLinkBodyDesc(bool enableRange = false, bool enableFMax = false);
};



// ------------------------------------------------------------------------------
/// �����ɓ��̂Ƒ��݂̂̐l�̃��f���̃C���^�[�t�F�C�X�i���s����p�j
struct CRTrunkFootHumanBodyIf : CRBodyIf {
	SPR_IFDEF(CRTrunkFootHumanBody);

};

/// ���́E���l�̃��f���̃f�X�N���v�^
struct CRTrunkFootHumanBodyDesc : CRBodyDesc {
	SPR_DESCDEF(CRTrunkFootHumanBody);

	enum CRHumanSolids{
		// ����
		SO_WAIST=0, SO_CHEST, SO_HEAD, SO_RIGHT_FOOT, SO_LEFT_FOOT,
		// ���̂̐�
		SO_NSOLIDS
	};

	enum CRHumanJoints{
		// �֐�
		JO_WAIST_CHEST=0, JO_CHEST_HEAD,
		// �֐߂̐�
		JO_NJOINTS
	};

	/// �T�C�Y�Ɋւ���p�����[�^
	double waistHeight, waistBreadth, waistThickness;
	double chestHeight, chestBreadth, chestThickness;
	double neckLength;
	double headDiameter;
	double footLength, footBreadth, footThickness;

	/// �e�֐߂̃o�l�_���p
	double springWaistChest, damperWaistChest;
	double springChestHead,    damperChestHead;

	/// ���搧��
	Vec2d rangeWaistChest;
	Vec2d rangeChestHead;

	CRTrunkFootHumanBodyDesc();

};
//@}

// ------------------------------------------------------------------------------
/// �����ɓ��̂Ƒ��݂̂̚M���ރ��f���̃C���^�[�t�F�C�X�i����p�j
struct CRFourLegsTinyAnimalBodyIf : CRBodyIf{
	SPR_IFDEF(CRFourLegsTinyAnimalBody);
};

/// ���̂Ƒ��݂̂̚M���ރ��f���̃f�B�X�N���v�^
struct CRFourLegsTinyAnimalBodyDesc : CRBodyDesc{
	SPR_DESCDEF(CRFourLegsTinyAnimalBody);

	enum CRAnimalSolids{
		// ����
		SO_BODY=0, SO_HEAD,SO_RIGHT_FRONT_LEG, SO_LEFT_FRONT_LEG,
		SO_RIGHT_REAR_LEG, SO_LEFT_REAR_LEG,
		// ���̂̐�
		SO_NSOLIDS
	};

	enum CRAnimalJoints{
		// �֐�
		JO_BODY_HEAD=0,
		JO_BODY_RIGHT_FRONT_LEG, JO_BODY_LEFT_FRONT_LEG,
		JO_BODY_RIGHT_REAR_LEG,  JO_BODY_LEFT_REAR_LEG,
		// �֐߂̐�
		JO_NJOINTS
	};

	/// �T�C�Y�Ɋւ���p�����[�^
	double bodyHeight, bodyBreadth, bodyThickness;
	double frontLegsBreadth, frontLegsHeight, frontLegsThickness;
	double rearLegsBreadth, rearLegsHeight, rearLegsThickness;

	/// �e�֐߂̃o�l�_���p
	double springFront, damperFront;
	double springRear,  damperRear;

	/// ���搧��
	Vec2d rangeFrontSwing;
	Vec2d rangeFrontTwist;
	Vec2d rangeRearSwing;
	Vec2d rangeRearTwist;

	bool noLegs;
	bool noHead;
	bool onlyOneLeg;
	bool hingeDebug;
	CRFourLegsTinyAnimalBodyDesc();
};

}

#endif//SPR_CRBODY_H
