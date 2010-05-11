/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRTRAJECTORY_H
#define CRTRAJECTORY_H

#include <Springhead.h>

#include <Foundation/Object.h>

#include "CREngine.h"

//@{
namespace Spr{;
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief �X�̎��O��
*/
class CRTrajectory : public CREngine, public CRTrajectoryDesc {
public:
	CRIKSolidIf*	solid;
	PHSolidIf*		soCur;

	Vec3f a0, a1, a2, a3, a4, a5;
	Vec3f b0, b1, b2, b3, b4, b5;

	SPR_OBJECTDEF(CRTrajectory);
	ACCESS_DESC(CRTrajectory);

	CRTrajectory()
	{
		solid = NULL;
		soCur = NULL;
	}
	CRTrajectory(const CRTrajectoryDesc& desc) 
		: CRTrajectoryDesc(desc)
	{
		solid = NULL;
		soCur = NULL;
	}

	// --- --- --- --- --- --- ---
	// API Methods

	/** @brief �v���C�I���e�B
	*/
	virtual int GetPriority() {
		return CREngineDesc::CREP_CONTROLLER_PLAN;
	}

	/** @brief ���݂̈ʒu�̎擾
	*/
	Vec3f GetPosition() {
		double t = ((time > timeLimit) ? timeLimit : time);
		/*
		std::cout << "a0 : " << a0 << ", ";
		std::cout << "a1 : " << a1 << ", ";
		std::cout << "a2 : " << a2 << ", ";
		std::cout << "a3 : " << a3 << ", ";
		std::cout << "a4 : " << a4 << ", ";
		std::cout << "a5 : " << a5 << ", ";
		std::cout << std::endl;
		*/
		return(a0 + a1*t + a2*pow(t,2) + a3*pow(t,3) + a4*pow(t,4) + a5*pow(t,5));
	}

	/** @brief ���݂̑��x�̎擾
	*/
	Vec3f GetVelocity() {
		double t = ((time > timeLimit) ? timeLimit : time);
		return(a1 + 2*a2*t + 3*a3*pow(t,2) + 4*a4*pow(t,3) + 5*a5*pow(t,4));
	}

	/** @brief ���݂̎p���̎擾
	*/
	Quaternionf GetOrientation() {
		return Quaternionf();
	}

	/** @brief ���݂̊p���x�̎擾
	*/
	Vec3f GetAngularVelocity() {
		return Vec3f();
	}

	/** @brief �O���^���J�n
	*/
	void Start() {
		time = 0;
		bEnabled = true;
		posStart = solid->GetPHSolid()->GetPose()*posInSolid;
		velStart = solid->GetPHSolid()->GetVelocity() + PTM::cross(solid->GetPHSolid()->GetAngularVelocity(), posInSolid);
		CalcTrajectory();
		currentPosition = GetPosition();
		currentOrientation = GetOrientation();

		solid->GetIKEndEffector()->Enable(true);
	}

	/** @brief ������1�X�e�b�v�i�߂�
	*/
	void Step() {
		if (soCur==NULL) {
			/*
			PHSceneIf* phScene = DCAST(CRCreatureIf,GetScene())->GetPHScene();
			PHSolidDesc descSo;
			descSo.dynamical = false;
			soCur = phScene->CreateSolid(descSo);
			CDBoxDesc descBox;
			descBox.boxsize = Vec3f(0.2, 0.2, 0.2);
			soCur->AddShape(phScene->GetSdk()->CreateShape(descBox));
			phScene->SetContactMode(soCur, PHSceneDesc::MODE_NONE);
			*/
		}

		if (bEnabled) {
			if (posEnabled) {
				// restart if too far
				if ((GetPosition() - solid->GetPHSolid()->GetPose()*posInSolid).norm() > 2.0) {
					// Start();
				}

				// restart if jump
				if ((GetPosition() - currentPosition).norm() > 0.5) {
					Start();
				}
			}

			currentPosition = GetPosition();
			currentOrientation = GetOrientation();

			time += DCAST(CRCreatureIf,GetScene())->GetPHScene()->GetTimeStep();

			// std::cout << time << " : " << GetPosition() << std::endl;

			// std::cout << posInSolid << std::endl;
			solid->GetIKEndEffector()->SetTargetPosition(GetPosition(), posInSolid);
			if (soCur) {
				soCur->SetFramePosition(GetPosition());
			}
		}
	}

	/** @brief ���B�ڕW���Ԃ��Z�b�g
	*/
	void SetTimeLimit(double tL) { timeLimit = tL; CalcTrajectory(); }

	/** @brief ���B�ڕW�ʒu���Z�b�g
	*/
	void SetTargetPosition(Vec3f pos) { posEnd = pos; CalcTrajectory(); }
	void EnablePositionControl(bool enable) { posEnabled = enable; }

	/** @brief �퓞�B�ڕW�ʒu���Z�b�g
	*/
	void SetTargetPositionInSolid(Vec3f pos){ posInSolid = pos; }

	/** @brief ���B�ڕW���x���Z�b�g
	*/
	void SetTargetVelocity(Vec3f vel) { velEnd = vel; CalcTrajectory(); }
	void EnableVelocityControl(bool enable) { velEnabled = enable; }

	/** @brief ���B�ڕW�p�����Z�b�g
	*/
	void SetTargetOrientation(Vec3f ori) { oriEnd = ori; CalcTrajectory(); }
	void EnableOrientationControl(bool enable) { oriEnabled = enable; }

	/** @brief ���B�ڕW�p���x���Z�b�g
	*/
	void SetTargetAngularVelocity(Vec3f angvel) { angvelEnd = angvel; CalcTrajectory(); }
	void EnableAngularVelocityControl(bool enable) { angvelEnabled = enable; }

	/** @brief �L���E�����Z�b�g
	*/
	void Enable(bool enable) { bEnabled = enable; }

	/** @brief �q�v�f�֘A
	*/
	virtual bool AddChildObject(ObjectIf* o) {
		CRIKSolidIf* so = o->Cast();
		if (so) {
			solid = so;
			return true;
		}
		return false;
	}

	virtual ObjectIf* GetChildObject(size_t pos) {
		if (pos == 0) {
			return solid->Cast();
		}
		return NULL;
	}

	virtual size_t NChildObject() {
		if (solid != NULL) {
			return 1;
		}
		return 0;
	}

	// --- --- --- --- --- --- ---
	// Non API Methods

	/** @brief ���݂̏����ł̋O���p�����[�^�̌v�Z
	*/
	void CalcTrajectory() {
		/*
		std::cout << "posStart : " << posStart << ", ";
		std::cout << "posEnd : " << posEnd << ", ";
		std::cout << "velStart : " << velStart << ", ";
		std::cout << "velEnd : " << velEnd << ", ";
		std::cout << "timeLimit : " << timeLimit << ", ";
		std::cout << std::endl;
		*/
		a0 = posStart;
		a1 = velStart;
		a4 =  (timeLimit*(5*velEnd + 5*a1) - 10*posEnd + 10*posStart)/pow(timeLimit,4)/4.0;
		a5 = -(timeLimit*(  velEnd +   a1) -  2*posEnd +  2*posStart)/pow(timeLimit,5)/2.0;
		a2 = -(timeLimit*velEnd + 2*timeLimit*a1 - 2*a5*pow(timeLimit,5) -   a4*pow(timeLimit,4) - 3*posEnd + 3*posStart)/pow(timeLimit,2);
		a3 =  (timeLimit*velEnd +   timeLimit*a1 - 3*a5*pow(timeLimit,5) - 2*a4*pow(timeLimit,4) - 2*posEnd + 2*posStart)/pow(timeLimit,3);
	}
};
}
//@}

#endif//CRTRAJECTORY_H
