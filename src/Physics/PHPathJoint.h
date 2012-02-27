/*
 *  Copyright (c) 2003-2012, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PHPATHJOINT_H
#define PHPATHJOINT_H

#include <Physics/SprPHJoint.h>
#include <Physics/PHConstraint.h>
#include <Physics/PHTreeNode.h>
#include <Physics/PHSpatial.h>

namespace Spr{;

/*	2�̍��̊Ԃ̑��Έʒu�E�p����Ԃ��֐�
		[p; q] = f(s)
	������Ƃ���D������p�͑��Έʒu, q�͑���quaternion�Cs�͈�ʉ����W��\�킷�X�J���[�D
	��������
		[p'; q'] = (df/ds)(s)s'�D
	quaternion�̎��Ԕ�������p���x�𓾂�s���E(q)�Ƃ����
		[v; w] = [I O; O E(q(s))](df/ds)(s)s' =: J(s)s'
	��̎����s��s'�����܂�Α��Α��x[v; w]�����܂�D
	������S�������̌`�ɕς���D6�����x�N�g��J(s)�͑��Α��x�̌�����\�킵�Ă���̂ŁC
	����ɒ�������x�N�g�����s�x�N�g���Ɏ��s���J~(s)�Ƃ���ƁC�S��������
		J~(s)[v; w] = 0

	Correction�́C
	velocity update��̑��Έʒu�ɍł��߂��O��im(f)��̓_���擾�C���̓_�ɍ��킹��D
	�{���̍S��������[p; q] in im(f)��������`�Ȃ̂ň����Ȃ��D
 */

struct PHPathPointWithJacobian : public PHPathPoint {
	Matrix6d	J;
};

/// �֐߂̋O���̃L�[�t���[���������C�⊮���ĕԂ����胄�R�r�A�����v�Z�����肷��N���X
class PHPath : public SceneObject, public std::vector<PHPathPointWithJacobian> {
	bool bReady;
	bool bLoop;	//[-pi, pi]�̖�����]�֐�
	iterator Find(double &s);
public:
	SPR_OBJECTDEF_ABST(PHPath);

	virtual bool GetDesc(void* desc)const;
	virtual void SetDesc(const void* desc);

	virtual void SetLoop(bool OnOff = true){bLoop = OnOff;}
	virtual bool IsLoop(){return bLoop;}
	void AddPoint(double s, const Posed& pose);
	void CompJacobian();
	void GetPose(double s, Posed& pose);
	void GetJacobian(double s, Matrix6d& J);
	void Rollover(double& s);
	PHPath(const PHPathDesc& desc=PHPathDesc());
};

class PHPathJoint;

/// �p�X�֐߂ɑΉ�����c���[�m�[�h
class PHPathJointNode : public PHTreeNode1D {
public:
	SPR_OBJECTDEF(PHPathJointNode);
	SPR_DECLMEMBEROF_PHPathJointNodeDesc;

	PHPathJoint* GetJoint(){return PHTreeNode1D::GetJoint()->Cast();}

	virtual void CompJointJacobian();
	virtual void CompJointCoriolisAccel();
	virtual void CompRelativePosition();
	virtual void CompRelativeVelocity();
	virtual void UpdateJointPosition(double dt);
	PHPathJointNode(const PHPathJointNodeDesc& desc = PHPathJointNodeDesc()){}
};

/// �p�X�֐�
class PHPathJoint : public PH1DJoint {
public:
	SPR_OBJECTDEF(PHPathJoint);
	SPR_DECLMEMBEROF_PHPathJointDesc;

	UTRef<PHPath> path;

	/// �R���X�g���N�^
	PHPathJoint(const PHPathJointDesc& desc = PHPathJointDesc());

	/// ABA�őΉ�����PHTreeNode�̔h���N���X�𐶐����ĕԂ�
	virtual PHTreeNode* CreateTreeNode(){ return DBG_NEW PHPathJointNode(); }

	// ----- PHConstraint�̔h���N���X�Ŏ�������@�\

	virtual void UpdateJointState();
	virtual void ModifyJacobian();
	virtual void CompBias();

	// ----- �C���^�t�F�[�X�̎���

	virtual void SetPosition(double pos){position[0] = pos;}
	virtual bool AddChildObject(ObjectIf* o);

};

}

#endif
