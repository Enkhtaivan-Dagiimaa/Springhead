/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PHHINGEJOINT_H
#define PHHINGEJOINT_H

#include <SprPhysics.h>
#include <Physics/PHConstraint.h>
#include <Physics/PHTreeNode.h>

namespace Spr{;

///	�q���W�֐߂̃c���[�m�[�h
class PHHingeJointNode : public PHTreeNode1D{
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
class PHHingeJoint : public PHJoint1D{


private:
	// �O���Ǐ]����p�ϐ��CPHBallJoint�Ƃ͕ϐ��̌^���قȂ�
	double qd,		preQd;		///< ���鎞���̖ڕW�ʒu�ւ̊p�x�ƂP�����O�̖ڕW�ʒu�ւ̊p�x
	double qdDot,	preQdDot;	///< ���鎞���̖ڕW�ʒu�ւ̊p�x�̈�K�����ƂP�����O�̖ڕW�ʒu�ւ̊p�x�̈�K����
	double qdWDot;				///< ���鎞���̖ڕW�ʒu�ւ̊p�x�̓�K����
		
public:
	SPR_OBJECTDEF(PHHingeJoint);
	SPR_DECLMEMBEROF_PHHingeJointDesc;

	virtual void CompBias();
	virtual void UpdateJointState();
	virtual PHTreeNode* CreateTreeNode(){
		return DBG_NEW PHHingeJointNode();
	}
	virtual void	CompError();
	PHHingeJoint(const PHHingeJointDesc& desc = PHHingeJointDesc());
};

}

#endif
