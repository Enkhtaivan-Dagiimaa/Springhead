/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRFOURLEGSTINYANIMALBODYGEN_H
#define CRFOURLEGSTINYANIMALBODYGEN_H

#include <Springhead.h>

#include <Foundation/Object.h>
#include <Physics/PHConstraint.h>
#include <Physics/PhysicsDecl.hpp>

#include <vector>

#include "CRBodyGen.h"

//@{
namespace Spr{;

/// ���̂Ƒ��݂̂̚M���ރ��f���̃f�B�X�N���v�^
struct CRFourLegsTinyAnimalBodyGenDesc : CRBodyGenDesc{

	// �ǂ̎�ނ̊֐߂ŋr���\�����邩
	enum CRTinyJointsMode{
		HINGE_MODE = 0,
		BALL_MODE,
	} jointType;

	// �r�̃|���S���̃^�C�v
	enum ShapeMode{
		MODE_ROUNDCONE = 0,
		MODE_BOX, MODE_CAPSULE, MODE_SPHERE,
	} shapeMode;

	/// �̊��Ɋւ���p�����[�^
	double bodyHeight, bodyBreadth, bodyThickness;

	Vec2d upperSizes;	//< �r��RoundCone�̃T�C�Y�i�e���C�q���j
	Vec2d lowerSizes;	//< ����RoundCone�̃T�C�Y�i�e���C�q���j
	float upperLength;	//< �r��RoundCone���\������2���Ԃ̋���
	float lowerLength;	//< ����RoundCone���\������2���Ԃ̋���

	// ���ʂɊւ���p�����[�^
	double massFF;		//< �O���̎���
	double massFL;		//< �O�r�̎���
	double massRF;		//< �㑫�̎���
	double massRL;		//< ��r�̎���
	double massBody;	//< �̊��̎���

	/// �e�֐߂̃o�l�_���p
	double springFront, damperFront;
	double springRear,  damperRear;

	/// ���搧���iBallJoint�̏ꍇ�j
	Vec2d rangeFrontSwing;
	Vec2d rangeFrontTwist;
	Vec2d rangeRearSwing;
	Vec2d rangeRearTwist;
	
	/// ���搧���iHingeJoint�̏ꍇ�j
	Vec2d rangeUpperFront;
	Vec2d rangeLowerFront;
	Vec2d rangeUpperRear;
	Vec2d rangeLowerRear;

	bool rfLeg; //< �E�O�����쐬���邩
	bool lfLeg; //< ���O�����쐬���邩
	bool rrLeg; //< �E�㑫���쐬���邩
	bool lrLeg; //< ���㑫���쐬���邩

	CRFourLegsTinyAnimalBodyGenDesc();
};


// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
// CRFourLegsTinyAnimalBodyGen
// �q���W�W���C���g��p�����M���ރ��f���E�N���X�̎����i�������F���g��TrunkFootHumanBody�j
class CRFourLegsTinyAnimalBodyGen : public CRBodyGen, public CRFourLegsTinyAnimalBodyGenDesc {
private:
	PHSolidIf* CreateBody();
	void CreateFrontLegs(LREnum lr, PHSolidIf* sBody);
	void CreateRearLegs( LREnum lr, PHSolidIf* sBody);
	PHSolidIf* InitBody();
	PHSolidIf* InitFrontLeg0(LREnum lr, PHSolidIf* sBody);
	void InitFrontLeg1(LREnum lr, PHSolidIf* sLeg0);
	PHSolidIf* InitRearLeg0(LREnum lr, PHSolidIf* sBody);
	void InitRearLeg1(LREnum lr, PHSolidIf* sLeg0);
	void InitLegs(PHSolidIf* sBody);

	void InitContact();
	// void InitControlMode(PHJointDesc::PHControlMode m = PHJointDesc::MODE_POSITION);	//< �{�f�B�̐��䃂�[�h��ݒ肷��D


public:

	CRFourLegsTinyAnimalBodyGen(){}
	CRFourLegsTinyAnimalBodyGen(const CRFourLegsTinyAnimalBodyGenDesc& desc, PHSceneIf* s=NULL) 
		: CRFourLegsTinyAnimalBodyGenDesc(desc) 
		, CRBodyGen((const CRBodyGenDesc&)desc, s)
	{
		PHSolidIf* b	= InitBody();
		InitLegs(b);
		InitContact();
		// InitControlMode();
		Init();
	}

	/** @brief �����������s����
	*/
	virtual void Init();
	virtual Vec2d GetSwingLimit(){return rangeFrontSwing;}
	virtual Vec2d GetTwistLimit(){return rangeFrontTwist;}

};

}
//@}

#endif//CRFourLegsTinyAnimalBODYGEN_H
