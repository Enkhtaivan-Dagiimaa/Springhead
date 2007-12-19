/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
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
struct CRIKControlIf;
struct CRIKMovableIf;
struct CRIKControlDesc;
struct CRIKMovableDesc;

// ------------------------------------------------------------------------------
/// �N���[�`���̃{�f�B���f���̃C���^�[�t�F�C�X
struct CRBodyIf : SceneObjectIf{
	IF_DEF(CRBody);

	/** @brief ���������s��
	*/
	virtual void Init()= 0;

	/** @brief ���̂̐��𓾂�
	*/
	virtual int NSolids()= 0;

	/** @brief i�Ԗڂ̍��̂𓾂�
	*/
	virtual PHSolidIf* GetSolid(int i)= 0;

	/** @brief �֐߂̐��𓾂�
	*/
	virtual int NJoints()= 0;

	/** @brief i�Ԗڂ̊֐߂𓾂�
	*/
	virtual PHJointIf* GetJoint(int i)= 0;

	/** @brief IK�p�̐���_��ǉ�����
	*/
	virtual CRIKControlIf* CreateIKControl(const IfInfo* ii, const CRIKControlDesc& desc)= 0;
	template <class T> CRIKControlIf* CreateIKControl(const T& desc){
		return CreateIKControl(T::GetIfInfo(), desc);
	}
	
	/** @brief IK�p�̉�����ǉ�����
	*/
	virtual CRIKMovableIf* CreateIKMovable(const IfInfo* ii, const CRIKMovableDesc& desc)= 0;
	template <class T> CRIKMovableIf* CreateIKMovable(const T& desc){
		return CreateIKMovable(T::GetIfInfo(), desc);
	}

	/** @brief IK���v�Z����
	*/
	virtual void CalcIK() = 0;

	/** @brief �{�f�B�̏d�S���W�𓾂�
	*/
	virtual Vec3d GetCenterOfMass() = 0;

	
};

/// �N���[�`���̃{�f�B���f���̃f�X�N���v�^
struct CRBodyDesc{
	DESC_DEF_FOR_OBJECT(CRBody);

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
	IF_DEF(CRHingeHumanBody);

	/** @brief �㔼�g�̊֐߂̌�����ς���
		@param stifness �����F�f�t�H���g�̌����ɑ΂���{���Ŏw��
	*/
	virtual void SetUpperBodyStiffness(float stiffness)=0;

	/** @brief �㔼�g�̃|�[�Y��ێ�����
	*/
	virtual void KeepUpperBodyPose()=0;

	/** @brief �㔼�g�̃|�[�Y�������ʒu�ɖ߂�
	*/
	virtual void ResetUpperBodyPose()=0;
};

/// �q���W�W���C���g�l�̃��f���̃f�X�N���v�^
struct CRHingeHumanBodyDesc : CRBodyDesc {
	DESC_DEF_FOR_OBJECT(CRHingeHumanBody);

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
/// �S���������f���̃C���^�[�t�F�C�X
struct CRFourLegsAnimalBodyIf : CRBodyIf {
	IF_DEF(CRFourLegsAnimalBody);

	/** @brief ��Ԃ̏d�S���W��Ԃ�
	*/
	virtual Vec3d GetUpperCenterOfMass() = 0;
};

/// 4���������f���̃f�X�N���v�^
struct CRFourLegsAnimalBodyDesc : CRBodyDesc {
	DESC_DEF_FOR_OBJECT(CRFourLegsAnimalBody);

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
		
		// -- Undefined members (the reality is NULL)
		SO_RIGHT_EYE,SO_LEFT_EYE,
		SO_RIGHT_EYE_XY,SO_LEFT_EYE_XY,

		// -- The number of the solids
		SO_NSOLIDS
	};

	enum CRAnimalJoints{
		// -- Center part of the ball joints
		JO_WAIST_CHEST=0,
		JO_CHEST_NECK,
		JO_NECK_HEAD,
		JO_WAIST_TAIL, JO_TAIL_12, JO_TAIL_23,

		// -- Left part of the ball joints
		JO_LEFT_SHOULDER,JO_LEFT_FRONT_ANKLE,
		JO_LEFT_HIP, JO_LEFT_REAR_ANKLE,

		// -- Right part of the ball joints
		JO_RIGHT_SHOULDER, JO_RIGHT_FRONT_ANKLE,
		JO_RIGHT_HIP, JO_RIGHT_REAR_ANKLE,

		// -- The number of the ball joints
		JO_NBALLJOINTS,
		
		// -- Left part of the hinge joints
		JO_LEFT_ELBOW,JO_LEFT_FRONT_KNEE,
		JO_LEFT_STIFLE,JO_LEFT_REAR_KNEE,

		// -- Right part of the hinge joints
		JO_RIGHT_ELBOW,JO_RIGHT_FRONT_KNEE,
		JO_RIGHT_STIFLE,JO_RIGHT_REAR_KNEE,

		// --�@Undefined members (its reality is NULL)
		JO_RIGHT_EYE_Y, JO_RIGHT_EYE_X,
		JO_LEFT_EYE_Y, JO_LEFT_EYE_X,

		// -- The number of the all joints (ball + hinge)
		JO_NJOINTS								//(nHingeJoints = nJoints - nBallJoints - 1)
	};

	/// �T�C�Y�Ɋւ���p�����[�^
	float waistBreadth,	       waistHeight,			  waistThickness;
	float chestBreadth,	       chestHeight,			  chestThickness;
	float tailBreadth,		       tailHeight,		      tailThickness;
	float neckBreadth,		       neckHeight,			  neckThickness;
	float headBreadth,		       headHeight,			  headThickness;
	float breastboneBreadth,      breastboneHeight,	  breastboneThickness;
	float radiusBreadth,	       radiusHeight,		  radiusThickness;
	float frontCannonBoneBreadth, frontCannonBoneHeight, frontCannonBoneThickness;
	float frontToeBreadth,		   frontToeHeight,		  frontToeThickness;
	float femurBreadth,		   femurHeight,			  femurThickness;
	float tibiaBreadth,		   tibiaHeight,			  tibiaThickness;
	float rearCannonBoneBreadth,  rearCannonBoneHeight,  rearCannonBoneThickness;
	float rearToeBreadth,		   rearToeHeight,		  rearToeThickness;

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
	Range<double> limitSwingWaistChest;
	Range<double> limitSwingWaistTail;
	Range<double> limitSwingTail;
	Range<double> limitSwingChestNeck;
	Range<double> limitSwingNeckHead;
	Range<double> limitSwingShoulder;
	Range<double> limitSwingFrontAnkle;
	Range<double> limitSwingHip;
	Range<double> limitSwingRearAnkle;

	/// BallJoint��twist����
	Range<double> limitTwistWaistChest;
	Range<double> limitTwistWaistTail;
	Range<double> limitTwistTail;
	Range<double> limitTwistChestNeck;
	Range<double> limitTwistNeckHead;
	Range<double> limitTwistShoulder;
	Range<double> limitTwistFrontAnkle;
	Range<double> limitTwistHip;
	Range<double> limitTwistRearAnkle;

	// 

	/// ���I�v�V����
	bool noLegs;
	/// �_�C�i�~�J�������邩�ǂ���
	bool dynamicalMode;

	CRFourLegsAnimalBodyDesc();
};





// ------------------------------------------------------------------------------
/// �����ɓ��̂Ƒ��݂̂̐l�̃��f���̃C���^�[�t�F�C�X�i���s����p�j
struct CRTrunkFootHumanBodyIf : CRBodyIf {
	IF_DEF(CRTrunkFootHumanBody);

};

/// ���́E���l�̃��f���̃f�X�N���v�^
struct CRTrunkFootHumanBodyDesc : CRBodyDesc {
	DESC_DEF_FOR_OBJECT(CRTrunkFootHumanBody);

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
struct CRTrunkFootAnimalBodyIf : CRBodyIf{
	IF_DEF(CRTrunkFootAnimalBody);
};

/// ���̂Ƒ��݂̂̚M���ރ��f���̃f�B�X�N���v�^
struct CRTrunkFootAnimalBodyDesc : CRBodyDesc{
	DESC_DEF_FOR_OBJECT(CRTrunkFootAnimalBody);

	enum CRAnimalSolids{
		// ����
		SO_WAIST=0, SO_CHEST, SO_HEAD, SO_RIGHT_FOOT, SO_LEFT_FOOT,
		// ���̂̐�
		SO_NSOLIDS
	};

	enum CRAnimalJoints{
		// �֐�
		JO_WAIST_CHEST=0, JO_CHEST_HEAD,
		// �֐߂̐�
		JO_NJOINTS
	};

	/// �T�C�Y�Ɋւ���p�����[�^
	double waistHeight, waistBreadth, waistThickness;
	double chestHeight, chestBreadth, chestThickness;
	double neckLength;
	float  headDiameter;
	double footLength, footBreadth, footThickness;

	/// �e�֐߂̃o�l�_���p
	double springWaistChest, damperWaistChest;
	double springChestHead,    damperChestHead;

	/// ���搧��
	Vec2d rangeWaistChest;
	Vec2d rangeChestHead;

	CRTrunkFootAnimalBodyDesc();
};

}

#endif//SPR_CRHingeHumanBodyIf_H
