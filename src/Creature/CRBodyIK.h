/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRBODYIK_H
#define CRBODYIK_H

#include <Springhead.h>

#include <Foundation/Object.h>
#include "IfStubCreature.h"

//@{
namespace Spr{;
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief ����_
*/
class CRIKControl : public SceneObject, public CRIKControlIfInit {
protected:
	PHSolidIf* solid;
	Vec3d goal;
	int number;

public:
	OBJECTDEF(CRIKControl, SceneObject);

	CRIKControl(){}
	CRIKControl(const CRIKControlDesc& desc){
		this->solid = desc.solid;
	}

	virtual void SetGoal(Vec3d goal){ this->goal = goal; }
	virtual Vec3d GetGoal(){ return goal; }

	virtual void SetNumber(int i){ number = i; }
	virtual int  GetNumber(){ return number; }
};

/** @brief �ʒu����_
*/
class CRIKControlPos : public CRIKControl, CRIKControlPosIfInit {
protected:
	Vec3d pos;

public:
	OBJECTDEF(CRIKControlPos, CRIKControl);

	CRIKControlPos(){}
	CRIKControlPos(const CRIKControlPosDesc& desc) : CRIKControl(desc) {
		this->pos = desc.pos;
	}
};

/** @brief �p������_
*/
class CRIKControlOri : public CRIKControl, CRIKControlOriIfInit {
protected:
public:
	OBJECTDEF(CRIKControlOri, CRIKControl);

	CRIKControlOri(){}
	CRIKControlOri(const CRIKControlOriDesc& desc) : CRIKControl(desc) {
	}
};

/** @brief �͐���_
*/
class CRIKControlForce : public CRIKControl, CRIKControlForceIfInit {
protected:
public:
	OBJECTDEF(CRIKControlForce, CRIKControl);

	CRIKControlForce(){}
	CRIKControlForce(const CRIKControlForceDesc& desc) : CRIKControl(desc) {
	}
};

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief ����Ώ�
*/
class CRIKMovable : public SceneObject, public CRIKMovableIfInit {
protected:
	float bias;
	Vec3d value;
	int number;

	Matrix3d CrossMatrix(Vec3d v){
		Matrix3d c = Matrix3d();
		c[0][1] = -v[2];
		c[0][2] =  v[1];
		c[1][0] =  v[2];
		c[1][2] = -v[0];
		c[2][0] = -v[1];
		c[2][1] =  v[0];
	}

public:
	OBJECTDEF(CRIKMovable, SceneObject);

	CRIKMovable(){};
	CRIKMovable(const CRIKMovableDesc& desc){
		this->bias = desc.bias;
	}

	virtual void SetValue(Vec3d value){ this->value = value; }
	virtual Vec3d GetValue(){ return value; }

	virtual void SetNumber(int i){ number = i; }
	virtual int  GetNumber(){ return number; }

	virtual Matrix3d CalcJacobian(CRIKControlIf* control);
};

/** @brief �ʒu����\�ȍ���
*/
class CRIKMovableSolidPos : public CRIKMovable, public CRIKMovableSolidPosIfInit {
protected:
	PHSolidIf* solid;

public:
	OBJECTDEF(CRIKMovableSolidPos, CRIKMovable);

	CRIKMovableSolidPos(){}
	CRIKMovableSolidPos(const CRIKMovableSolidPosDesc& desc) : CRIKMovable(desc){
		this->solid = desc.solid;
	}

	virtual Matrix3d CalcJacobian(CRIKControlIf* control);
};

/** @brief �p������\�ȍ���
*/
class CRIKMovableSolidOri : public CRIKMovable, public CRIKMovableSolidOriIfInit {
protected:
	PHSolidIf* solid;

public:
	OBJECTDEF(CRIKMovableSolidOri, CRIKMovable);

	CRIKMovableSolidOri(){}
	CRIKMovableSolidOri(const CRIKMovableSolidOriDesc& desc) : CRIKMovable(desc){
		this->solid = desc.solid;
	}

	virtual Matrix3d CalcJacobian(CRIKControlIf* control);
};

/** @brief �p�x����\�ȃ{�[���W���C���g
*/
class CRIKMovableBallJointOri : public CRIKMovable, public CRIKMovableBallJointOriIfInit {
protected:
	PHBallJointIf* joint;

public:
	OBJECTDEF(CRIKMovableBallJointOri, CRIKMovable);

	CRIKMovableBallJointOri(){}
	CRIKMovableBallJointOri(const CRIKMovableBallJointOriDesc& desc) : CRIKMovable(desc){
		this->joint = desc.joint;
	}

	virtual Matrix3d CalcJacobian(CRIKControlIf* control);
};

/** @brief �g���N����\�ȃ{�[���W���C���g
*/
class CRIKMovableBallJointTorque : public CRIKMovable, public CRIKMovableBallJointTorqueIfInit {
protected:
	PHBallJointIf* joint;

public:
	OBJECTDEF(CRIKMovableBallJointTorque, CRIKMovable);

	CRIKMovableBallJointTorque(){}
	CRIKMovableBallJointTorque(const CRIKMovableBallJointTorqueDesc& desc) : CRIKMovable(desc){
		this->joint = desc.joint;
	}

	virtual Matrix3d CalcJacobian(CRIKControlIf* control);
};

/** @brief �p�x����\�ȎO�A�q���W�W���C���g
*/
class CRIKMovable3HingeJointOri : public CRIKMovable, public CRIKMovable3HingeJointOriIfInit {
protected:
	PHHingeJointIf *joint1, *joint2, *joint3;

public:
	OBJECTDEF(CRIKMovable3HingeJointOri, CRIKMovable);

	CRIKMovable3HingeJointOri(){}
	CRIKMovable3HingeJointOri(const CRIKMovable3HingeJointOriDesc& desc) : CRIKMovable(desc){
		this->joint1 = desc.joint1;
		this->joint2 = desc.joint2;
		this->joint3 = desc.joint3;
	}

	virtual Matrix3d CalcJacobian(CRIKControlIf* control);
};

/** @brief �g���N����\�ȎO�A�q���W�W���C���g
*/
class CRIKMovable3HingeJointTorque : public CRIKMovable, public CRIKMovable3HingeJointTorqueIfInit {
protected:
	PHHingeJointIf *joint1, *joint2, *joint3;

public:
	OBJECTDEF(CRIKMovable3HingeJointTorque, CRIKMovable);

	CRIKMovable3HingeJointTorque(){}
	CRIKMovable3HingeJointTorque(const CRIKMovable3HingeJointTorqueDesc& desc) : CRIKMovable(desc){
		this->joint1 = desc.joint1;
		this->joint2 = desc.joint2;
		this->joint3 = desc.joint3;
	}

	virtual Matrix3d CalcJacobian(CRIKControlIf* control);
};
}
//@}
#endif//CRBODYIK_H
