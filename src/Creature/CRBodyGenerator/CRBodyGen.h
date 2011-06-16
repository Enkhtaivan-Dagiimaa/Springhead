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
#include <vector>

//@{
namespace Spr{;

struct PHSdkIf;
struct PHIKActuatorIf;
struct PHIKEndEffectorIf;

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief �{�f�B
*/
struct CRBodyGenDesc{
	/// �v���O����e���̂ɂ��邩�\�P�b�g����e���̂ɂ��邩
	//enum CRHumanJointOrder{
	enum CRCreatureJointOrder{
		SOCKET_PARENT,
		PLUG_PARENT,
	} jointOrder;

	CRBodyGenDesc(){
	}
};

class CRBodyGen : public CRBodyGenDesc {
protected:
	enum LREnum{LEFTPART=-1, RIGHTPART=+1};

	/// �{�f�B���\�����鍄��
	std::vector< UTRef<PHSolidIf> > solids;	
	
	/// �{�f�B���\������֐�
	std::vector< UTRef<PHJointIf> > joints;

	/// IK�m�[�h
	std::vector< UTRef<PHIKActuatorIf> > ikActuators;

	/// ����_
	std::vector< UTRef<PHIKEndEffectorIf> > ikEndEffectors;

	/// ��������V�[��
	PHSceneIf*		phScene;

	/// ��������Sdk
	PHSdkIf*		phSdk;
	
	/** @brief �֐߂����
	*/
	PHJointIf* CreateJoint(PHSolidIf* soChild, PHSolidIf* soParent, const PHHingeJointDesc& desc);
	PHJointIf* CreateJoint(PHSolidIf* soChild, PHSolidIf* soParent, const PHBallJointDesc& desc);

	/** @brief CDBox��Inertia���v�Z����i�����e���\���̑Ίp�v�f�ɂ͒l�������āC������ς�0�j
		@param CDBoxDesc::boxsize, PHSolidDesc::mass
		@return �����e���\��
	*/
	Matrix3d CalcBoxInertia(Vec3d boxsize, double mass);

	/**
		@brief child�̃|�e���V�����G�l���M�[���v�Z����
	*/
	double CalcPotential(Posed parentPos, PHSolidIf* parentSolid, PHJointIf* childJoint);

	/**
		@brief ���ׂĂ̍��̂̏����p����ڕW�p�Ɏ����������̂Ƃ��ď���������
	*/
	void SetInitPosition(PHSolidIf* parentSolid, PHJointIf* childJoint);

public:
	CRBodyGen(const CRBodyGenDesc& desc = CRBodyGenDesc(), PHSceneIf* s = 0);

	/** @brief ���������s��
	*/
	virtual void				Init();

	/** @brief ���̂̐��𓾂�
	*/
	virtual int					NSolids();

	/** @brief i�Ԗڂ̍��̂𓾂�
	*/
	virtual PHSolidIf*			GetSolid(int i);

	/** @brief �֐߂̐��𓾂�
	*/
	virtual int					NJoints();

	/** @brief i�Ԗڂ̊֐߂𓾂�
	*/
	virtual PHJointIf*			GetJoint(int i);

	/** @brief �{�[���W���C���g�̐���Ԃ�
	*/
	virtual int NBallJoints();

	/** @brief �q���W�W���C���g�̐���Ԃ�
	*/
	virtual int NHingeJoints();

	/** @brief IK�m�[�h�̐��𓾂�
	*/
	int NIKActuators();

	/** @brief i�Ԗڂ�IK�m�[�h�𓾂�
	*/
	PHIKActuatorIf* GetIKActuator(int i);

	/** @brief IK����_�̐��𓾂�
	*/
	int NControlPoints();

	/** @brief i�Ԗڂ�IK����_�𓾂�
	*/
	PHIKEndEffectorIf* GetControlPoint(int i);

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

};
}
//@}

#endif // CR_BODYGEN_H
