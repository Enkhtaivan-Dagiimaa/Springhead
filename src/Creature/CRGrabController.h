﻿/*
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
/** @brief 把持コントローラ
*/
class CRGrabController : public CRController, public CRGrabControllerDesc {
private:
	/// この剛体をつかって掴む
	CRBoneIf* solid;

	/// 連結用バネ
	PHSpringIf* grabSpring;

	/// 把持中の剛体
	PHSolidIf* grabbingSolid;

	/// 把持を行うための作成済Springのマップ
	//// 把持対象の剛体→連結用ばね
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

	/// 初期化
	virtual void Init() {}

	/// 1ステップ
	virtual void Step() {}

	/// 状態をリセットする
	virtual void Reset() {
		grabSpring->Enable(false);
		grabbingSolid = NULL;
		grabSpring    = NULL;
	}

	/// 現状を返す
	virtual int  GetStatus() { return CRControllerDesc::CS_WAITING; }

	/// 指定した物体をつかむ．
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

	/// つかんでいる物体を返す．つかんでいなければNULL
	PHSolidIf* GetGrabbingSolid() {
		return grabbingSolid;
	}

	/// 把持に使用する剛体を返す
	PHSolidIf* GetSolid() {
		return solid->GetPHSolid();
	}
};
}
//@}

#endif//CRGRABCONTROLLER_H
