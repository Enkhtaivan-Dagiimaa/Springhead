/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 *  PHHingeJoint��swing�ɂ��|�����݊p�x�̐����ƁAtwist�ɂ��˂���̐������ł��܂��B
 *
 */
#ifndef PH3ELEMENTHINGEJOINT_H
#define PH3ELEMENTHINGEJOINT_H

#include <SprPhysics.h>
#include <Physics/PHHingeJoint.h>

namespace Spr{

class PH3ElementHingeJoint : public PHHingeJoint {
public:
	SPR_OBJECTDEF1(PH3ElementHingeJoint, PHJoint);
	SPR_DECLMEMBEROF_PH3ElementHingeJointDesc;
protected:
public:
		
	/// �C���^�t�F�[�X�̎���
	double  GetSecondDamper()				{return secondDamper;}
	void	SetSecondDamper(double input)	{secondDamper = input;}

	// ���̃N���X���ōĒ�`����i�I�[�o�[���C�h����j�֐�
	void	CompBias();		///< �N���ʂ̔���(�I�[�o�[���C�h)

	// �R���X�g���N�^�i�����t���j
	PH3ElementHingeJoint(const PH3ElementHingeJointDesc& desc = PH3ElementHingeJointDesc());				/// - �R���X�g���N�^
};

}

#endif
