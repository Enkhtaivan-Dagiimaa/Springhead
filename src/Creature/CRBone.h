/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRBONE_H
#define CRBONE_H

#include <Foundation/Object.h>
#include <Creature/SprCRBone.h>
#include <Physics/SprPHSolid.h>
#include <Physics/SprPHJoint.h>
#include <Physics/SprPHIK.h>

//@{
namespace Spr{;

class CRBone : public SceneObject, public CRBoneDesc {
	PHSolidIf*			solid;
	PHJointIf*			joint;
	PHIKEndEffectorIf*	endeffector;
	PHIKActuatorIf*		actuator;

	void InitVars() {
		solid		= NULL;
		endeffector	= NULL;
		joint		= NULL;
		actuator	= NULL;
	}

public:
	SPR_OBJECTDEF(CRBone);

	CRBone() { InitVars(); }
	CRBone(const CRBoneDesc& desc) : CRBoneDesc(desc) { InitVars(); }

	/** @brief ���x���i����������������FHand�Ȃǁj�̎擾
	 */
	virtual const char* GetLabel() const { return label.c_str(); }

	/** @brief ���x���i����������������FHand�Ȃǁj�̐ݒ�
	 */
	void SetLabel(const char* str) { label = std::string(str); }

	/** @brief PHSolid���擾
	 */
	PHSolidIf* GetPHSolid() { return solid; }

	/** @brief PHSolid��ݒ�
	 */
	void SetPHSolid(PHSolidIf* so) { solid = so; }

	/** @brief PHJoint���擾
	 */
	PHJointIf* GetPHJoint() { return joint; }

	/** @brief PHJoint��ݒ�
	 */
	void SetPHJoint(PHJointIf* jo) { joint = jo; }

	/** @brief IK�G���h�G�t�F�N�^���擾
	 */
	PHIKEndEffectorIf* GetIKEndEffector() { return endeffector; }

	/** @brief IK�G���h�G�t�F�N�^��ݒ�
	 */
	void SetIKEndEffector(PHIKEndEffectorIf* ikEE) { endeffector = ikEE; }

	/** @brief IK�A�N�`���G�[�^���擾
	 */
	PHIKActuatorIf* GetIKActuator() { return actuator; }

	/** @brief IK�A�N�`���G�[�^��ݒ�
	 */
	void SetIKActuator(PHIKActuatorIf* ikAct) { actuator = ikAct; }

	/** @brief �q�v�f�̈���
	*/
	virtual size_t NChildObject() const {
		return( ((solid==NULL)?0:1) + ((joint==NULL)?0:1) + ((endeffector==NULL)?0:1) + ((actuator==NULL)?0:1) );
	}

	virtual ObjectIf* GetChildObject(size_t i) {
		ObjectIf* objs[] = {solid->Cast(), joint->Cast(), endeffector->Cast(), actuator->Cast()};
		int pos = -1;
		for (int n=0; n<4; ++n) {
			if (objs[n]!=NULL) { pos++; }
			if (pos==i) { return objs[n]; }
		}
		return NULL;
	}

	virtual bool AddChildObject(ObjectIf* o) {
		if (!solid)			{ solid			= o->Cast(); if (solid)			{ return true; } }
		if (!joint)			{ joint			= o->Cast(); if (joint)			{ return true; } }
		if (!endeffector)	{ endeffector	= o->Cast(); if (endeffector)	{ return true; } }
		if (!actuator)		{ actuator		= o->Cast(); if (actuator)		{ return true; } }
		return false;
	}

	virtual bool DelChildObject(ObjectIf* o) {
		if (o==solid)		{ solid			= NULL; return true; }
		if (o==joint)		{ joint			= NULL; return true; }
		if (o==endeffector)	{ endeffector	= NULL; return true; }
		if (o==actuator)	{ actuator		= NULL; return true; }
		return false;
	}
};

}
//@}

#endif//CRBONE_H
