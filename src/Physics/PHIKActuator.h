﻿/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PH_IKACTUATOR_H
#define PH_IKACTUATOR_H

#include <Physics/SprPHIK.h>
#include <Physics/SprPHJoint.h>
#include "../Foundation/Object.h"
#include "PHIKEndEffector.h"
#include "PhysicsDecl.hpp"
#include <set>
#include <map>

namespace Spr{;

class PHIKActuator;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
// IKActuator：IKに使用できるアクチュエータ（作動部品）
// 

class PHIKActuator : public SceneObject{
public:
	SPR_OBJECTDEF(PHIKActuator);
	SPR_DECLMEMBEROF_PHIKActuatorDesc;

	// 関節ツリー上でつながったアクチュエータ
	// ※ 計算に必要となるのは
	//    「先祖と子孫にあたるアクチュエータすべて」
	//      （“「このアクチュエータで動かせるいずれかのエンドエフェクタ」を動かせる他のアクチュエータ” にあたる）
	//    「子孫にあたるアクチュエータにつながれたエンドエフェクタすべて」
	//      （“このアクチュエータで動かせるエンドエフェクタ” にあたる）

	/// 直系祖先・子孫（自分含む）・直接の子供
	std::vector<PHIKActuator*> ascendant, descendant, children;
	/// 祖先・子孫にまとめてアクセスする関数
	PHIKActuator* Link(size_t i) { return (i<ascendant.size()) ? ascendant[i] : descendant[i-ascendant.size()]; }
	int NLinks() {return (int)(ascendant.size()+descendant.size());}

	/// このアクチュエータで直接つながれたエンドエフェクタ．1アクチュエータに対し1エンドエフェクタが対応
	PHIKEndEffector* eef;

	// --- --- --- --- ---

	/// 番号
	int number;

	/// 自由度
	int ndof;

	/// 自由度変化フラグ
	bool bNDOFChanged;

	/// アクチュエータ追加フラグ
	bool bActuatorAdded;

	// --- --- --- --- ---

	/// 計算用の一時変数
	PTM::VVector<double>						alpha, beta;
	std::map< int, PTM::VMatrixRow<double> >	gamma;

	/// ヤコビアン
	std::map< int,PTM::VMatrixRow<double> > Mj;

	/// IKのIterationの一回前の計算結果（収束判定用）
	PTM::VVector<double> omega_prev;

	/// IKの計算結果（角度）
	PTM::VVector<double> omega;

	/// IDの計算結果（トルク）
	PTM::VVector<double> tau;

	// --- --- --- --- --- --- --- --- --- ---

	/** @brief 初期化
	*/
	virtual void Init() {
		number = -1;
		bNDOFChanged = true;
		bActuatorAdded = false;
		eef = NULL;
		if (descendant.size()==0) { descendant.push_back(this); }
	}

	/** @brief デフォルトコンストラクタ
	*/
	PHIKActuator(){
		Init();
	}

	/** @brief コンストラクタ
	*/
	PHIKActuator(const PHIKActuatorDesc& desc){
		Init();
		SetDesc(&desc);
	}

	// --- --- --- --- ---

	/** @brief IKの計算準備をする
	*/
	virtual void PrepareSolve();

	/** @brief IKの計算繰返しの１ステップを実行する
	*/
	virtual void ProceedSolve();

	/** @brief 擬似逆解を解いたままの"生の"計算結果を取得する
	*/
	virtual PTM::VVector<double> GetRawSolution(){ return omega; }

	// --- --- --- --- ---

	/** @brief 計算結果に従って制御対象を動かす
	*/
	virtual void Move(){}

	// --- --- --- --- ---

	/** @brief 動かしにくさを設定・取得する
	*/
	void  SetBias(float bias){ this->bias = bias; }
	float GetBias()          { return bias; }

	/** @brief 速度制御の比例係数を設定・取得する
	*/
	void   SetVelocityGain(double velocityGain){ this->velocityGain = velocityGain; }
	double GetVelocityGain()                   { return velocityGain; }

	/** @brief 有効・無効を設定・取得する
	*/
	void Enable(bool enable){ bEnabled = enable; }
	bool IsEnabled()        { return bEnabled; }

	// --- --- --- --- ---

	virtual bool		AddChildObject(ObjectIf* o);
	virtual ObjectIf*	GetChildObject(size_t pos);
	virtual	size_t		NChildObject()const;

	// --- --- --- --- --- --- --- --- --- ---
	// Non API Methods

	/** @brief ヤコビアン計算前の処理
	*/
	virtual void BeforeCalcAllJacobian() {}

	/** @brief 関係するすべての制御点とのヤコビアンをそれぞれ求める
	*/
	void CalcAllJacobian();

	/** @brief 計算用変数を準備する前の処理
	*/
	virtual void BeforeSetupMatrix() {}

	/** @brief 計算用変数を準備する
	*/
	void SetupMatrix();

	/** @brief 指定した制御点との間のヤコビアンを計算する
	*/
	virtual void CalcJacobian(PHIKEndEffector* endeffector) {}

	/** @brief 繰返し計算の一ステップの後に行う処理
	*/
	virtual void AfterProceedSolve() {}
};

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

class PHIKBallActuator : public PHIKActuator{
public:
	SPR_OBJECTDEF(PHIKBallActuator);
	SPR_DECLMEMBEROF_PHIKBallActuatorDesc;

	/// 制御対象の関節
	PHBallJointIf* joint;

	/// IKの回転軸
	Vec3d e[3];

	// --- --- --- --- --- --- --- --- --- ---

	/** @brief 初期化
	*/
	virtual void Init() {
		ndof = 2;
		joint = NULL;
		PHIKActuator::Init();
	}

	/** @brief デフォルトコンストラクタ
	*/
	PHIKBallActuator(){
		Init();
	}

	/** @brief コンストラクタ
	*/
	PHIKBallActuator(const PHIKBallActuatorDesc& desc) {
		Init();
		SetDesc(&desc);
	}

	// --- --- --- --- ---

	/** @brief 計算結果に従って制御対象を動かす
	*/
	virtual void Move();

	// --- --- --- --- ---

	/** @brief 動作対象の関節を設定する（１アクチュエータにつき１関節が必ず対応する）
	*/
	virtual void SetJoint(PHBallJointIf* joint) { this->joint = joint; }

	/** @brief 動作対象として設定された関節を取得する
	*/
	virtual PHBallJointIf* GetJoint() { return this->joint; }

	// --- --- --- --- ---

	virtual bool		AddChildObject(ObjectIf* o);
	virtual ObjectIf*	GetChildObject(size_t pos);
	virtual	size_t		NChildObject()const;

	// --- --- --- --- --- --- --- --- --- ---
	// Non API Methods

	/** @brief 回転軸を計算する
	*/
	virtual void CalcAxis();

	/** @brief ヤコビアン計算前の処理
	*/
	virtual void BeforeCalcAllJacobian();

	/** @brief 計算用変数を準備する前の処理
	*/
	virtual void BeforeSetupMatrix();

	/** @brief 指定した制御点との間のヤコビアンを計算する
	*/
	virtual void CalcJacobian(PHIKEndEffector* endeffector);
};

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

class PHIKHingeActuator : public PHIKActuator{
public:
	SPR_OBJECTDEF(PHIKHingeActuator);
	SPR_DECLMEMBEROF_PHIKBallActuatorDesc;

	/// 制御対象の関節
	PHHingeJointIf *joint;

	// --- --- --- --- --- --- --- --- --- ---

	/** @brief 初期化
	*/
	virtual void Init() {
		ndof = 1;
		joint = NULL;
		PHIKActuator::Init();
	}

	/** @brief デフォルトコンストラクタ
	*/
	PHIKHingeActuator(){
		Init();
	}

	/** @brief コンストラクタ
	*/
	PHIKHingeActuator(const PHIKHingeActuatorDesc& desc) {
		Init();
		SetDesc(&desc);
	}

	// --- --- --- --- ---

	/** @brief 計算結果に従って制御対象を動かす
	*/
	virtual void Move();

	// --- --- --- --- ---

	/** @brief 動作対象の関節を設定する（１アクチュエータにつき１関節が必ず対応する）
	*/
	virtual void SetJoint(PHHingeJointIf* joint) { this->joint = joint; }

	/** @brief 動作対象として設定された関節を取得する
	*/
	virtual PHHingeJointIf* GetJoint() { return this->joint; }

	// --- --- --- --- ---

	virtual bool		AddChildObject(ObjectIf* o);
	virtual ObjectIf*	GetChildObject(size_t pos);
	virtual	size_t		NChildObject()const;

	// --- --- --- --- --- --- --- --- --- ---
	// Non API Methods

	/** @brief 指定した制御点との間のヤコビアンを計算する
	*/
	virtual void CalcJacobian(PHIKEndEffector* endeffector);
};

}

#endif
