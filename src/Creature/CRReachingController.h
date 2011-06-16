/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRREACHINGCONTROLLER_H
#define CRREACHINGCONTROLLER_H

#include <Creature/CREngine.h>
#include <Creature/SprCRReachController.h>

//@{
namespace Spr{;
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief ���B�^���R���g���[��
*/
class CRReachingController : public CREngine, public CRReachingControllerDesc {
public:
	/// �o�ߎ���
	float time;

	/// ���B�ڕW����
	float period;

	/// �ŏI�ڕW�ʒu�E���x�E�p���E�p���x
	Vec3f fP, fV;

	/// �L�����ǂ���
	bool bActive;

	/// ���݂̖ڕW
	Vec3f pos, vel;

	/// �ڕW�܂ł̏�������
	double initLen;

	/// �\���V�~�����[�V�����̂��߂Ɋi�[���Ă������
	UTRef<ObjectStatesIf> state;

public:
	SPR_OBJECTDEF(CRReachingController);
	ACCESS_DESC(CRReachingController);

	/// ����Ώۂ̃N���[�`���p����
	CRIKSolidIf* cSolid;

	CRReachingController(){}
	CRReachingController(const CRReachingControllerDesc& desc) 
		: CRReachingControllerDesc(desc)
	{
		bActive = false;
		Init();
	}

	/** @ brief �����������s����
	*/
	virtual void Init();

	/** @ brief ����̃X�e�b�v�����s����
	*/
	virtual void Step();

	/** @brief �f�o�b�O����\������
	*/
	virtual void Render(GRRenderIf* render);

	/** @brief �ʒu�𓞒B������
	*/
	virtual void Start(Vec3d pos, Vec3d v, float t);

	/** @brief ���B��Ԃ���������
	*/
	virtual void Stop();

	/** @brief �ڕW�ʒu�̍Đݒ�
	*/
	virtual void SetPos(Vec3f pos){ fP = pos; }

	/** @brief ����Ώۍ��̂̐ݒ�
	*/
	virtual void SetCRSolid(CRIKSolidIf* cso){ cSolid = cso; }

	/** @brief IK����_�̎擾
	*/
	virtual CRIKSolidIf* GetCRSolid(){ return cSolid; }
};

/** @brief ���B�^���R���g���[���̏W����
*/
class CRReachingControllers : public CREngine, public CRReachingControllersDesc {
private:
	std::vector<CRReachingControllerIf*> controllers;

public:
	SPR_OBJECTDEF(CRReachingControllers);
	ACCESS_DESC(CRReachingControllers);

	CRReachingControllers(){}
	CRReachingControllers(const CRReachingControllersDesc& desc)
		: CRReachingControllersDesc(desc)
	{
	}

	/** @ brief ����̃X�e�b�v�����s����
	*/
	virtual void Step() { /* std::cout << "CRReachingControllers::Step() called" << std::endl; */ }

	/** @brief ���B�^���R���g���[�����擾����
	*/
	CRReachingControllerIf* GetReachingController(PHSolidIf* solid){
		/*
		for (size_t i=0; i<controllers.size(); ++i) {
			CRReachingController* ct = controllers[i]->Cast();
			if (ct && ct->ikcp->GetSolid() == solid) {
				return controllers[i];
			}
		}

		{
			CRCreatureIf* creature = DCAST(SceneObject,this)->GetScene()->Cast();
			CRReachingControllerDesc reachDesc;
			CRReachingControllerIf*  reachCtl = creature->CreateEngine(reachDesc)->Cast();
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
		*/

		return NULL;
	}

};
}
//@}

#endif//CRREACHINGCONTROLLER_H
