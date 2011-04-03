/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PHJOINTLIMIT_H
#define PHJOINTLIMIT_H

#include <SprPhysics.h>
#include <Physics/PhysicsDecl.hpp>

namespace Spr{;

class PHScene;
class PHConstraintEngine;
class PHJoint1D;
class PHBallJoint;
struct PH3ElementCommonData;

/**
	関節の可動範囲拘束．インタフェースは持たない．
 */

class PHJointLimit1D{
public:
	PHJoint1D*	joint;
	double  A, Ainv, dA, b, db;
	double	f, F;
	bool	onLower, onUpper;			///< 可動範囲の下限、上限に達している場合にtrue
	
	virtual void	SetupLCP();
	virtual void	IterateLCP();
	virtual void	SetupCorrectionLCP();
	virtual void	IterateCorrectionLCP();


	PHJointLimit1D();
};

struct OnLimit{
	bool onLower;
	bool onUpper;
	bool& operator [] (int i){
		return i==0 ? onLower : onUpper;
	}
};

struct splinePoints{
	double swingUpper[15][6];
	int limitCount;
	bool UorL;	//true:LimitUpper,,false:LimitLower
};

class PHBallJointLimit{
public:
	PHBallJoint*	joint;
	Vec3d	A, Ainv, dA, b, db;
//	SpatialMatrix	J[2];
	Matrix3d		J[2];
//	SpatialMatrix	T[2];
	Matrix3d		T[2];
	Matrix3d		Jc;
	Matrix3d		Jcinv;
	//Vec3d	f, F;
	bool	onLower, onUpper;

	/*********************************************************
	Socket座標系：親剛体についている関節の座標系
	Plug座標系：子剛体についている関節の座標系
	拘束座標系：毎ステップ更新する，拘束の条件に使う座標系
	(x軸：拘束円の接線方向，
	 y軸：limitDirの延長線上の一点と交わる直線，
	 z軸：Socket座標系から見たPlug座標系のz軸の方向)
	**********************************************************/

	Vec3d			currentEulerAngle;				///< 現在SocketからPlugに伸びているベクトル(Jc.ez())と稼動域制限の中心ベクトルとのなす角度(.x:swing, .y:twist, .z:swingDir)
	bool			anyLimit;				///< どこかのリミットにかかっているかどうかを調べるフラグ == (onLimit.onUpper || onLimit.onLower)
	OnLimit			onLimit[2];				///< 可動域制限にかかっているとtrue ([0]:swing, [1]:twist)	
	int				limitCount[2];			///< 上の配列のいくつまで入っているのか数える
	Vec3d			penetration;			///< 侵入量
	Vec3d			tanDir;
	Matrix3d		limDir;					///< 初期の拘束座標系 (x軸,y軸,z軸( = limitDir))
	Vec3d			lastEulerAngle;					///< 前回の位置
	splinePoints	splinePoints[2];			///< 拘束範囲の指定
	Vec4d			neighborAngle;			///< 拘束時の近傍点の情報(SDir,Swing,区間,媒介変数値)
	int				constNumCondition;		///< 拘束軸の数
	int				constrainedAxes[3];		///< 拘束軸を指定

	void	CheckLimit();
	void	SplineLimit();
	void	SwingTwistLimit();
	bool	SetConstLine(char* fileName , bool i);
	void	SetConstPoint(int num, int way, double a);
	double	GetConstLine(int num, int way){ return splinePoints[0].swingUpper[num][way]; }

	void	SetupLCP();
	void	IterateLCP();
	void	Projection(double& f, int k);
	Vec3d	CompResponseMatrix();


	Vec3d	SolveQubicFunction(Vec4d coefficient);
	Vec4d	CalcParameter(int i,int j,bool flag);
	PHBallJointLimit();
};

}

#endif
