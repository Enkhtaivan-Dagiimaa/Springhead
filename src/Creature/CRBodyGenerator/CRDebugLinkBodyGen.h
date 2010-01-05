/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRDEBUGLINKBODYGEN_H
#define CRDEBUGLINKBODYGEN_H

#include <Springhead.h>
#include <Foundation/Object.h>
#include <Physics/PHConstraint.h>
#include <Physics/PhysicsDecl.hpp>

#include <vector>
#include "CRBodyGen.h"

//@{
namespace Spr{;

// ------------------------------------------------------------------------------
/// �f�o�b�O�p�̃w�r�󃂃f���̃f�X�N���v�^
struct CRDebugLinkBodyGenDesc : CRBodyGenDesc {
	
	/* 
		MODE_SPHEERE �S�ċ�
		MODE_CAPSULE �S�ăJ�v�Z��
		MODE_BOX	 �S�ă{�b�N�X
		MODE_MIX	 ���ƃ{�b�N�X������
	*/
	enum CDShapeMode{
		MODE_SPHERE=0, MODE_CAPSULE, MODE_BOX, MODE_MIX,
	} shapeMode;

	enum PHJointMode{
		MODE_BALL=0, MODE_HINGE, MODE_MIXED
	} jointMode;

	unsigned int soNSolids;
	unsigned int joNBallJoints;
	unsigned int joNHingeJoints;
	unsigned int joNJoints;

	/// �T�C�Y�Ɋւ���p�����[�^(radius, length)
	double radius, length;

	/// �����N��̏d��
	double mass;

	/// �eBallJoint�̃o�l�_���p
	double spring, damper;

	/// HingeJoint���搧��
	Vec2d  range;
	
	// BallJoint����ڕW
	Quaterniond SetTargetPosition;

	/// BallJoint��swing����:
	Vec2d limitSwing;

	/// BallJoint��twist����
	Vec2d limitTwist;

	// �֐߂̏o����͂̍ő�l
	double fMax;

	/// �Î~���C
	double mu0;

	/// �����C
	double mu;


	CRDebugLinkBodyGenDesc(bool enableRange = false, bool enableFMax = false);
};



class CRDebugLinkBodyGen: public CRBodyGen, public CRDebugLinkBodyGenDesc{

private:
	PHBallJointDesc InitBallJointDesc();
	PHHingeJointDesc InitHingeJointDesc();
	void SolidFactory(CDShapeMode m);
	void JointFactory(PHJointMode m);
	void CreateBody();
	void InitBody();
	void InitContact();
	// void InitControlMode(PHJointDesc::PHControlMode m = PHJointDesc::MODE_POSITION);
public:

	CRDebugLinkBodyGen(){}
	CRDebugLinkBodyGen(const CRDebugLinkBodyGenDesc& desc, PHSceneIf* s =NULL)
		: CRDebugLinkBodyGenDesc(desc)
		, CRBodyGen((const CRBodyGenDesc&)desc, s)
	{
		CreateBody();
		InitBody();
		InitContact();
	}
	int NBallJoints(){return joNBallJoints;}
	int NHingeJoints(){return joNHingeJoints;}

};

}
//@}

#endif //< end of CRDEBUGLINKBODYGEN_H