/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRNECKCONTROLLER_H
#define CRNECKCONTROLLER_H

#include <Creature/CREngine.h>
#include <Creature/SprCRGazeController.h>

//@{
namespace Spr{;

struct CRIKSolidIf;

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief 首（頭部）運動コントローラ
*/
class CRNeckController : public CREngine, public CRNeckControllerDesc {
private:
	/// 注視点
	Vec3d pos, vel;

	/// 注意の度合
	float attractiveness;

	/// 制御に使うIK付きクリーチャ剛体
	FWObjectIf *fwHead;

	/// 基準方向（タイマー開始時の頭の方向）
	Quaterniond orig;

	/// タイマー
	double t;

	bool bEnabled;

public:
	SPR_OBJECTDEF(CRNeckController);
	ACCESS_DESC_STATE(CRNeckController);

	CRNeckController(){}
	CRNeckController(const CRNeckControllerDesc& desc)
		: CRNeckControllerDesc(desc)
	{
		Init();
	}

	/** @brief 初期化を実行する
	*/
	virtual void Init();

	/** @brief 制御処理を実行する
	*/
	virtual void Step();

	virtual void Stop();

	/** @brief 注視点を設定する
		@param pos 注視点の３次元座標
		@param vel 注視点の移動速度ベクトル
	*/
	virtual void LookAt(Vec3f pos, Vec3f vel, float attractiveness);

	/// CREngineから継承：グループから必要なオブジェクトを取り出してくる
	virtual void AssignObject() {
		for (int i=0; i<groups.size(); ++i) {
			 FWObjectGroupIf* g = groups[i]->FindByLabel(labelHeadSolid);
			 if (g && g->NObjects() > 0) { fwHead = g->GetObject(0); }
		}
	}
};
}
//@}

#endif//CRNECKCONTROLLER_H
