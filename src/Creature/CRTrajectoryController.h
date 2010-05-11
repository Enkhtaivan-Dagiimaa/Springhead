/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRTRAJECTORYCONTROLLER_H
#define CRTRAJECTORYCONTROLLER_H

#include <Springhead.h>

#include <Foundation/Object.h>

#include "CREngine.h"

//@{
namespace Spr{;
/** @brief �O���^���R���g���[��
*/
class CRTrajectoryController : public CREngine, public CRTrajectoryControllerDesc {
public:
	/// �\���V�~�����[�V�����̂��߂Ɋi�[���Ă������
	UTRef<ObjectStatesIf> states;

	/// �v�Z���ꂽ�֐ߊp
	struct JointTargetValue {
		PHJointIf*	joint;
		double		hingePosition;
		double		hingeVelocity;
		double		hingeOffset;
		Quaterniond ballPosition;
		Vec3d		ballVelocity;
		Vec3d		ballOffset;
		double		spring;
		double		damper;
	};
	std::vector<JointTargetValue> jointTargetValues;

	/// �N���[�`���g��
	CRBodyIf* crBody;

	// --- --- --- --- --- --- --- --- --- ---

	SPR_OBJECTDEF(CRTrajectoryController);
	ACCESS_DESC(CRTrajectoryController);

	CRTrajectoryController()
	{
		Init();
	}
	CRTrajectoryController(const CRTrajectoryControllerDesc& desc) 
		: CRTrajectoryControllerDesc(desc)
	{
		Init();
	}

	// --- --- --- --- --- --- --- --- --- ---
	// API Methods

	/** @brief �v���C�I���e�B
	*/
	virtual int GetPriority() {
		return CREngineDesc::CREP_CONTROLLER_ACT;
	}

	/** @brief �P�X�e�b�v����
	*/
	virtual void Step();

	// --- --- --- --- --- --- --- --- --- ---
	// Non-API Methods
	/** @brief ������
	*/
	void Init();

	/** @brief �֐ߊp�v�Z
	*/
	void CompJointAngle();

	/** @brief �O���Ǐ]�p�I�t�Z�b�g�̌v�Z
	*/
	void CompOffsetForce();
};
}
//@}

#endif//CRTRAJECTORYCONTROLLER_H
