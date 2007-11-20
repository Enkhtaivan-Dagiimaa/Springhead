/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRBODY_H
#define CRBODY_H

#include <Springhead.h>

#include <Foundation/Object.h>
#include "IfStubCreature.h"

//@{
namespace Spr{;
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief IK�̐���_
*/
class CRControlPoint{
public:
	/// ����_�̎��
	CRBodyIf::CRIKCPType cpType;

	/// ����_�̂���Solid
	PHSolidIf* cpSolid;

	/// ����_�̈ʒu(SolidLocal���W�n)
	Vec3d cpPos;

	/// ����_�̖ڕW�l
	Vec3d cpGoal;

	CRControlPoint(){
		cpType = CRBodyIf::IKCP_ORI;
		cpSolid = NULL;
		cpPos = Vec3d(0,0,0);
		cpGoal = Vec3d(0,0,0);
	}
};
/** @brief IK�̂��߂̉���
*/
class CRMovableObject{
public:
	/// ���ȃ{�[���W���C���g
	PHBallJointIf* movableBallJoint;

	/// ���ȂR�A�q���W�W���C���g
	PHHingeJointIf *movableHinge1, *movableHinge2, *movableHinge3;

	/// ���ȍ���
	PHSolidIf* movableSolid;

	CRMovableObject(){
		movableBallJoint = NULL;
		movableHinge1 = NULL;
		movableHinge2 = NULL;
		movableHinge3 = NULL;
		movableSolid = NULL;
	}
};
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief �{�f�B
*/
class CRBody : public SceneObject, public CRBodyIfInit, public CRBodyDesc {
protected:
	enum LREnum{LEFTPART=-1, RIGHTPART=+1};

	/// �{�f�B���\�����鍄��
	std::vector<PHSolidIf*> solids;
	
	/// �{�f�B���\������֐�
	std::vector<PHJointIf*> joints;

	/// ���̃Z���T�̎����傽��Creature
	CRCreatureIf* creature;

	/// ��������V�[��
	PHSceneIf* phScene;

	/// ��������Sdk
	PHSdkIf* phSdk;

	/// �{�f�B�S�̂̃��R�r�A�����Ǘ�����s��
	PTM::VMatrixCol<Matrix3d> jacobian;

	/// �������Ǘ�����s��
	PTM::VMatrixCol<bool>     movability;

	/// �s�ԍ��Ɛ���_�̑Ή�
	std::vector<CRControlPoint>  cpNums;

	/// ��ԍ��Ɖ����̑Ή�
	std::vector<CRMovableObject> moNums;

	/** @brief �֐߂����
	*/
	PHJointIf* CreateJoint(PHSolidIf* soChild, PHSolidIf* soParent, PHHingeJointDesc desc);
	PHJointIf* CreateJoint(PHSolidIf* soChild, PHSolidIf* soParent, PHBallJointDesc desc);

	

public:
	OBJECTDEF(CRBody, SceneObject);
	ACCESS_DESC(CRBody);

	CRBody(){}
	CRBody(const CRBodyDesc& desc, CRCreatureIf* c=NULL) 
		: CRBodyDesc(desc)
	{
		if(c!=NULL && c->GetScene()){SetScene(c->GetScene());}
		creature = c;
		phScene = DCAST(PHSceneIf, c->GetScene());
		phSdk   = phScene->GetSdk();
	}

	/** @brief ���������s��
	*/
	virtual void Init();

	/** @brief ���̂̐��𓾂�
	*/
	virtual int NSolids();

	/** @brief i�Ԗڂ̍��̂𓾂�
	*/
	virtual PHSolidIf* GetSolid(int i);

	/** @brief �֐߂̐��𓾂�
	*/
	virtual int NJoints();

	/** @brief i�Ԗڂ̊֐߂𓾂�
	*/
	virtual PHJointIf* GetJoint(int i);

	/** @brief IK�p�̐���_��ǉ�����
	*/
	virtual int AddIKControlPoint(CRBodyIf::CRIKCPType type, PHSolidIf* solid, Vec3d pos);
	
	/** @brief IK�p�̉�����ǉ�����
	*/
	virtual int AddIKMovableJoint(int cpnum, PHBallJointIf* ballJoint);
	virtual int AddIKMovableJoint(int cpnum, PHHingeJointIf* jo1, PHHingeJointIf* jo2, PHHingeJointIf* jo3);
	virtual int AddIKMovableSolid(int cpnum, PHSolidIf* solid);
};
}
//@}

#endif//CRBODY_H
