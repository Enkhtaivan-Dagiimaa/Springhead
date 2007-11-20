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

	/// �ʒu�E�p���̐���_�̏W��
	std::vector<CRIKControlIf*> controlsPosOri;

	/// �ʒu�E�p���̐���Ώۂ̏W��
	std::vector<CRIKMovableIf*> movablesPosOri;

	/// �ʒu�E�p���̉����s��
	PTM::VMatrixCol<bool> movabilityPosOri;

	/// �ʒu�E�p���̃��R�r�A��
	PTM::VMatrixCol<Matrix3d> jacobianPosOri;

	/// �ʒu�E�p���̃��R�r�A���i�]�u���|���Đ������j
	PTM::VMatrixCol<Matrix3d> sqjacobianPosOri;

	/** @brief �֐߂����
	*/
	PHJointIf* CreateJoint(PHSolidIf* soChild, PHSolidIf* soParent, PHHingeJointDesc desc);
	PHJointIf* CreateJoint(PHSolidIf* soChild, PHSolidIf* soParent, PHBallJointDesc desc);

	/** @brief �s������T�C�Y����
	*/
	void ResizeMatrix(int hInc, int wInc);

	
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
	virtual CRIKControlIf* AddIKControl(const IfInfo* ii, const CRIKControlDesc& desc);
	
	/** @brief IK�p�̉�����ǉ�����
	*/
	virtual CRIKMovableIf* AddIKMovable(const IfInfo* ii, const CRIKMovableDesc& desc);
};
}
//@}

#endif//CRBODY_H
