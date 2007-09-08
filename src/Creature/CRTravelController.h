/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRTRAVELCONTROLLER_H
#define CRTRAVELCONTROLLER_H

#include <Springhead.h>

#include <Foundation/Object.h>
#include "IfStubCreature.h"

#include "CRController.h"

//@{
namespace Spr{;
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief 移動コントローラ（目標地点への移動，障害物回避）
*/
class CRTravelController : public CRController, public CRTravelControllerIfInit, public CRTravelControllerDesc {
private:
	/// 下位のコントローラ：歩行コントローラ
	CREseWalkingControllerIf* walkCtrl;

	/// 情報源：内部シーン
	CRInternalSceneIf* internalScene;

	/// 自キャラの現在位置を得るための剛体
	PHSolidIf* soWaist;

	/// 目標地点（平面内座標）
	Vec2f goal;

public:
	OBJECTDEF(CRTravelController, CRController);
	ACCESS_DESC(CRTravelController);

	CRTravelController(){
		goal = Vec2f(0,0);
	}
	CRTravelController(const CRTravelControllerDesc& desc, CRCreatureIf* c=NULL) 
		: CRTravelControllerDesc(desc) 
		, CRController((const CRControllerDesc&)desc, c)
	{
		walkCtrl = NULL;
		goal = Vec2f(0,0);
	}

	/** @brief 初期化を行う
	*/
	virtual void Init();

	/** @brief 制御を実行する
	*/
	virtual void Step();

	/** @brief 目標地点を設定する
	*/
	virtual void SetGoal(Vec2f goal);
};
}
//@}

#endif//CRTRAVELCONTROLLER_H
