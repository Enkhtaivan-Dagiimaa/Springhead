/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRWALKINGCONTROLLER_H
#define CRWALKINGCONTROLLER_H

#include <Springhead.h>

#include <Foundation/Object.h>
#include "IfStubCreature.h"

#include "CRController.h"

//@{
namespace Spr{;
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief 注意コントローラ
*/
class CRWalkingController : public CRController, public CRWalkingControllerIfInit, public CRWalkingControllerDesc {
private:
	/// 依存する他のオブジェクトの例．必要なものを選んで記述．ただしCRControllerに既にあるものは不要．
	/*
	CRHingeHumanBodyIf* body;
	CRInternalSceneIf*  internalScene;
	CRGazeControllerIf* gazeController;
	CROpticalSensorIf*  opticalSensor;
	*/

	/// ほかにPrivateな変数やメソッドが必要ならここに定義を追加．

public:
	OBJECTDEF(CRWalkingController, CRController);
	ACCESS_DESC(CRWalkingController);

	CRWalkingController(){
	}
	CRWalkingController(const CRWalkingControllerDesc& desc, CRCreatureIf* c=NULL) 
		: CRWalkingControllerDesc(desc) 
		, CRController((const CRControllerDesc&)desc, c)
	{
	}

	/** @brief 初期化を行う
	*/
	virtual void Init();

	/** @brief 制御を実行する
	*/
	virtual void Step();

	/** @brief 歩行の速度を設定する
	*/
	virtual void SetSpeed(float speed);

	/** @brief 転回角度を設定する
	*/
	virtual void SetRotationAngle(float rot);
};
}
//@}

#endif//CRWALKINGCONTROLLER_H
