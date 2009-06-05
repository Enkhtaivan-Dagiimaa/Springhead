/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRREACHINGCONTROLLER_H
#define CRREACHINGCONTROLLER_H

#include <Springhead.h>

#include <Foundation/Object.h>

#include "CRController.h"

//@{
namespace Spr{;
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief ���B�^���R���g���[��
*/
class CRReachingController : public CRController, public CRReachingControllerDesc {
private:
	/// �o�ߎ���
	float time;

	/// ���B�ڕW����
	float period;

	/// �ŏI�ڕW�ʒu�E���x�E�p���E�p���x
	Vec3f fP, fV;

	/// �L�����ǂ���
	bool bActive;

public:
	SPR_OBJECTDEF(CRReachingController);
	ACCESS_DESC(CRReachingController);

	/// ����Ώۂ�ikcp
	PHIKPosCtlIf* ikcp;

	CRReachingController(){}
	CRReachingController(const CRReachingControllerDesc& desc, CRCreatureIf* c=NULL) 
		: CRController((const CRControllerDesc&)desc, c)
		, CRReachingControllerDesc(desc)
	{
		bActive = false;
	}

	/** @ brief �����������s����
	*/
	virtual void Init();

	/** @ brief ����̃X�e�b�v�����s����
	*/
	virtual void Step();

	/** @brief �ʒu�𓞒B������
	*/
	virtual void Start(Vec3d pos, Vec3d v, float t);

	/** @brief ���B��Ԃ���������
	*/
	virtual void Stop();

	/** @brief �ڕW�ʒu�̍Đݒ�
	*/
	virtual void SetPos(Vec3f pos){ fP = pos; }

	/** @brief IK����_�̐ݒ�
	*/
	virtual void SetIKCP(PHIKPosCtlIf* ikcp){ this->ikcp = ikcp; }

	/** @brief IK����_�̎擾
	*/
	virtual PHIKPosCtlIf* GetIKCP(){ return this->ikcp; }
};

/** @brief ���B�^���R���g���[���̏W����
*/
class CRReachingControllers : public CRController, public CRReachingControllersDesc {
private:
	std::vector<CRReachingControllerIf*> controllers;

public:
	SPR_OBJECTDEF(CRReachingControllers);
	ACCESS_DESC(CRReachingControllers);

	CRReachingControllers(){}
	CRReachingControllers(const CRReachingControllersDesc& desc, CRCreatureIf* c=NULL) 
		: CRController((const CRControllerDesc&)desc, c)
		, CRReachingControllersDesc(desc)
	{
	}

	/** @brief ���B�^���R���g���[�����擾����
	*/
	CRReachingControllerIf* GetReachingController(PHSolidIf* solid){
		for (size_t i=0; i<controllers.size(); ++i) {
			CRReachingController* ct = controllers[i]->Cast();
			if (ct && ct->ikcp->GetSolid() == solid) {
				return controllers[i];
			}
		}

		{
			CRReachingControllerDesc reachDesc;
			CRReachingControllerIf*  reachCtl = creature->CreateController(reachDesc)->Cast();
			controllers.push_back(reachCtl);
			
			for (int i=0; i<creature->NBodies(); ++i) {
				CRBodyIf* body = creature->GetBody(i);
				for (int j=0; j<body->NControlPoints(); ++j) {
					PHIKPosCtlIf* posCtl = body->GetControlPoint(j)->Cast();
					if (posCtl && posCtl->GetSolid() == solid) {
						reachCtl->SetIKCP(posCtl);
						return reachCtl;
					}
				}
			}
		}

		return NULL;
	}

};
}
//@}

#endif//CRREACHINGCONTROLLER_H
