/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CR_BODYGEN_H
#define CR_BODYGEN_H

#include <Foundation/Object.h>
#include <Physics/SprPHJoint.h>
#include <Physics/SprPHSolid.h>
#include <Creature/SprCRBody.h>
#include <Creature/SprCRBone.h>
#include <Creature/SprCRCreature.h>
#include <vector>

//@{
namespace Spr{;

struct PHSdkIf;
struct PHIKActuatorIf;
struct PHIKEndEffectorIf;

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief �{�f�B�\�z�N���X�̃x�[�X
*/
struct CRBodyGenDesc{
	CRBodyGenDesc(){
	}
};

class CRBodyGen : public CRBodyGenDesc {
protected:
	enum LREnum{LEFTPART=-1, RIGHTPART=+1};

	// �Ώ�Creature
	CRCreatureIf* crCreature;

	// �쐬�����Body
	CRBodyIf* crBody;

	/// �{�f�B���\�����鍄��
	std::vector< UTRef<CRBoneIf> > solids;	
	
	/// �{�f�B���\������֐�
	std::vector< UTRef<CRBoneIf> > joints;

	/** @brief �֐߂����
	*/
	CRBoneIf* CreateJoint(CRBoneIf* soChild, CRBoneIf* soParent, const PHHingeJointDesc& desc);
	CRBoneIf* CreateJoint(CRBoneIf* soChild, CRBoneIf* soParent, const PHBallJointDesc& desc);

	/** @brief ���̂����
	*/
	CRBoneIf* CreateSolid(const PHSolidDesc& desc);

	/** @brief ���ׂĂ̍��̂̏����p����ڕW�p�Ɏ����������̂Ƃ��ď���������	
	*/
	void SetInitPosition(PHSolidIf* parentSolid, PHJointIf* childJoint);

public:
	CRBodyGen(const CRBodyGenDesc& desc = CRBodyGenDesc());

	/** @brief �{�f�B�𐶐�����
	*/
	virtual CRBodyIf* Generate(CRCreatureIf* crCreature);


	// �����̊֐���CRBody�Ɉړ��\��imitake�j
	#if 0
	/** @brief CDBox��Inertia���v�Z����i�����e���\���̑Ίp�v�f�ɂ͒l�������āC������ς�0�j
		@param CDBoxDesc::boxsize, PHSolidDesc::mass
		@return �����e���\��
	*/
	Matrix3d CalcBoxInertia(Vec3d boxsize, double mass);

	/**
		@brief child�̃|�e���V�����G�l���M�[���v�Z����
	*/
	double CalcPotential(Posed parentPos, PHSolidIf* parentSolid, PHJointIf* childJoint);

	/** @brief �{�f�B�̏d�S���W�𓾂�
	*/
	virtual Vec3d GetCenterOfMass();
	
	/** @brief �{�f�B�̎��ʂ𓾂�
	*/
	virtual double GetSumOfMass();

	/**
		@brief �ڕW�p�E�ڕW�p���x�Ɏ��������ꍇ�̗͊w�I�G�l���M�[��Ԃ�
	*/
	virtual double GetTargetMechanicalEnergy(PHSolidIf* rootSolid);

	/**
		@brief �ڕW�p�E�ڕW�p���x�Ɏ��������ꍇ�̉^���G�l���M�[��Ԃ�
	*/
	virtual double GetTargetKineticEnergy();

	/**
		@brief �ڕW�p�E�ڕW�p���x�Ɏ��������ꍇ�̈ʒu�G�l���M�[��Ԃ�
	*/
	virtual double GetTargetPotentialEnergy(PHSolidIf* rootSolid);

	/**
		@breif �e���̖̂ڕW��ԂɎ��������ꍇ�̈ʒu�G�l���M�[��Ԃ�
	*/
	 virtual double CalcTargetPotential(Posed parentPos, PHSolidIf* parentSolid, PHJointIf* childJoint);

	/**
		@brief ���݂̗͊w�I�G�l���M�[��Ԃ�
	*/
	virtual double GetMechanicalEnergy();

	/**
		@brief ���݂̉^���G�l���M�[��Ԃ�
	*/
	virtual double GetKineticEnergy();

	/**
		@brief ���݂̈ʒu�G�l���M�[��Ԃ�
	*/
	virtual double GetPotentialEnergy();
	#endif
};
}
//@}

#endif // CR_BODYGEN_H
