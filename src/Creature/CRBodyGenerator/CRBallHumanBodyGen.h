/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CR_BALLHUMANBODYGEN_H
#define CR_BALLHUMANBODYGEN_H

#include <Foundation/Object.h>
#include <Creature/CRBodyGenerator/CRBodyGen.h>

//@{
namespace Spr{;

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/// �{�[���W���C���g�l�̃��f���̃f�X�N���v�^
struct CRBallHumanBodyGenDesc : CRBodyGenDesc {
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
	
	///Joint�̐�
	int joNBallJoints;
	int joNHingeJoints;
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

	// �`��Ɋւ���ݒ�
	enum CRHumanShapeType {
		HST_ROUNDCONE=0,
		HST_BOX,
	} shapeType;

	CRBallHumanBodyGenDesc();
};

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
// CRBallHumanBodyGen
// �{�[���W���C���g��p�����l�̃��f���E�N���X�̎���
class CRBallHumanBodyGen : public CRBodyGen, public CRBallHumanBodyGenDesc {
private:
	void GenerateBody();
	void CreateWaist();
	void CreateAbdomen();
	void CreateChest();

	void GenerateHead();
	void CreateNeck();
	void CreateHead();

	void GenerateArms();
	void CreateUpperArm(LREnum lr);
	void CreateLowerArm(LREnum lr);
	void CreateHand(LREnum lr);

	void GenerateEyes();
	void CreateEye(LREnum lr);

	void GenerateLegs();
	void CreateUpperLeg(LREnum lr);
	void CreateLowerLeg(LREnum lr);
	void CreateFoot(LREnum lr);

	void InitContact();
	void InitSolidPose();
	void InitIK();

	void SetJointSpringDamper(PHBallJointDesc  &ballDesc,  double springOrig, double damperOrig, double actuatorMass);
	void SetJointSpringDamper(PHHingeJointDesc &hingeDesc, double springOrig, double damperOrig, double actuatorMass);

	void CreateIKActuator(int n);
	void CreateIKEndEffector(int n);

public:
	CRBallHumanBodyGen(){}
	CRBallHumanBodyGen(const CRBallHumanBodyGenDesc& desc) 
		: CRBallHumanBodyGenDesc(desc) 
		, CRBodyGen((const CRBodyGenDesc&)desc)
	{
		solids.resize(CRBallHumanBodyGenDesc::SO_NSOLIDS);
		joints.resize(CRBallHumanBodyGenDesc::JO_NJOINTS);
	}

	/** @brief �����������s����
	*/
	virtual CRBodyIf* Generate(CRCreatureIf* crCreature);
};

}
//@}

#endif // CR_BALLHUMANBODYGEN_H
