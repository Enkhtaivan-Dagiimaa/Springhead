/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRGRABCONTROLLER_H
#define CRGRABCONTROLLER_H

#include <Creature/CREngine.h>
#include <Creature/SprCRController.h>
#include <Creature/SprCRBone.h>
#include <Creature/SprCRCreature.h>
#include <Physics/SprPHJoint.h>
#include <Physics/SprPHSolid.h>
#include <Physics/SprPHScene.h>
#include <map>

//@{
namespace Spr{;

struct PHSpringIf;
struct CRBodyIf;

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief �c���R���g���[��
*/
class CRGrabController : public CRController, public CRGrabControllerDesc {
private:
	/// ���̍��̂������Ē͂�
	CRBoneIf* solid;

	/// �A���p�o�l
	PHSpringIf* grabSpring;

	/// �c�����̍���
	PHSolidIf* grabbingSolid;

	/// �c�����s�����߂̍쐬��Spring�̃}�b�v
	//// �c���Ώۂ̍��́��A���p�΂�
	typedef std::map< PHSolidIf*, PHSpringIf* > GrabSpringMap;
	GrabSpringMap grabSpringMap;

public:
	SPR_OBJECTDEF(CRGrabController);
	ACCESS_DESC(CRGrabController);

	CRGrabController(){}
	CRGrabController(const CRGrabControllerDesc& desc) 
		: CRGrabControllerDesc(desc) 
	{
		grabSpring    = NULL;
		grabbingSolid = NULL;
		solid         = NULL;
	}

	/// ������
	virtual void Init() {}

	/// 1�X�e�b�v
	virtual void Step() {}

	/// ��Ԃ����Z�b�g����
	virtual void Reset() {
		grabSpring->Enable(false);
		grabbingSolid = NULL;
		grabSpring    = NULL;
	}

	/// �����Ԃ�
	virtual int  GetStatus() { return CRControllerDesc::CS_WAITING; }

	/// �w�肵�����̂����ށD
	void SetTargetSolid(PHSolidIf* targetSolid) {
		PHSceneIf *phScene = DCAST(CRCreatureIf,GetScene())->GetPHScene();

		Posed relpose = solid->GetPHSolid()->GetPose().Inv() * targetSolid->GetPose();

		GrabSpringMap::iterator it = grabSpringMap.find(targetSolid);
		if (it==grabSpringMap.end()) {
			PHSpringDesc descSpring;
			descSpring.bEnabled = false;
			descSpring.spring   = Vec3d(1,1,1) * 500;
			descSpring.damper   = Vec3d(1,1,1) *   5;
			PHSpringIf* spring = phScene->CreateJoint(solid->GetPHSolid(), targetSolid, descSpring)->Cast();

			grabSpringMap[targetSolid] = spring;
			grabSpring = spring;
		} else {
			grabSpring = it->second;
		}

		Posed pose;
		grabSpring->Enable(true);
		grabSpring->SetPlugPose(Posed());
		grabSpring->SetSocketPose(relpose);

		grabbingSolid = targetSolid;
	}

	/// ����ł��镨�̂�Ԃ��D����ł��Ȃ����NULL
	PHSolidIf* GetGrabbingSolid() {
		return grabbingSolid;
	}

	/// �c���Ɏg�p���鍄�̂�Ԃ�
	PHSolidIf* GetSolid() {
		return solid->GetPHSolid();
	}
};
}
//@}

#endif//CRGRABCONTROLLER_H
