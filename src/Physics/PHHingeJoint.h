/*
 *  Copyright (c) 2003-2012, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PHHINGEJOINT_H
#define PHHINGEJOINT_H

#include <Physics/SprPHJoint.h>
#include <Physics/PHConstraint.h>
#include <Physics/PHTreeNode.h>

namespace Spr{;

///	�q���W�֐߂ɑΉ�����c���[�m�[�h
class PHHingeJointNode : public PHTreeNode1D {
public:
	SPR_OBJECTDEF(PHHingeJointNode);
	SPR_DECLMEMBEROF_PHHingeJointNodeDesc;

	virtual void CompJointJacobian();
	virtual void CompJointCoriolisAccel();
	virtual void CompRelativePosition();
	virtual void CompRelativeVelocity();
	PHHingeJointNode(const PHHingeJointNodeDesc& desc = PHHingeJointNodeDesc()){}
};

///	�q���W�֐�
class PHHingeJoint : public PH1DJoint {
public:
	SPR_OBJECTDEF(PHHingeJoint);
	SPR_DECLMEMBEROF_PHHingeJointDesc;

	/// �R���X�g���N�^
	PHHingeJoint(const PHHingeJointDesc& desc = PHHingeJointDesc());

	/// ABA�őΉ�����PHTreeNode�̔h���N���X�𐶐����ĕԂ�
	virtual PHTreeNode* CreateTreeNode(){ return DBG_NEW PHHingeJointNode(); }

	// ----- PHConstraint�̔h���N���X�Ŏ�������@�\

	virtual void UpdateJointState();
	virtual void CompBias();
	virtual void CompError();

	// ----- �C���^�t�F�[�X�̎���

	virtual double GetDeviation();

};

}

#endif
