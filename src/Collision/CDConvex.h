﻿/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CDCONVEX_H
#define CDCONVEX_H

#include <Foundation/Object.h>
#include <Collision/CDShape.h>
#include <Collision/CDCutRing.h>
#include <Collision/CollisionDecl.hpp>

namespace Spr{;

class CDConvex : public CDShape{
protected:
	bool		valid;
	struct Boxel{
		Vec3f		pos;
		float		volume;
		Matrix3f	moment;
	};
	std::vector<Boxel> boxels;

	float		volume;
	Vec3f		center;
	Matrix3f	moment;

	/// 
	bool	IsBoxInside(const Vec3f& bbmin, const Vec3f& bbmax);
	/// 幾何情報の無効化
	void	Invalidate(){valid = false;}
	/// 幾何解析
	void	Analyze();
	void	AccumulateBoxels(const Vec3f& bbmin, const Vec3f& bbmax, float eps);
public:
	SPR_OBJECTDEF_ABST(CDConvex);

	/// 偏心によって生じるイナーシャ
	static void OffsetInertia(const Vec3f& c, Matrix3f& I);

	/**	サポートポイント(方向ベクトルvとの内積が最大の点)をwに格納する。
		戻り値には、頂点番号があれば返す。無ければ-1。頂点番号はメッシュの頂点の場合
		のほか、RoundConeなどの球の中心の番号のこともある	*/
	virtual int Support(Vec3f&w, const Vec3f& v) const =0;
	///	切り口を求める．接触解析に使う．
	virtual bool FindCutRing(CDCutRing& r, const Posed& toW) =0;
	///	頂点vtx のとなりの頂点を列挙する。
	virtual std::vector<int>& FindNeighbors(int vtx);
	///	頂点バッファを返す。
	virtual Vec3f* GetBase(){return NULL;}
	
	///	バウンディングボックスを求める．
	virtual void CalcBBox(Vec3f& bbmin, Vec3f& bbmax, const Posed& pose);
	/// 表面上の点pにおける曲率半径を求める
	virtual double CurvatureRadius(Vec3d p){ return 1e+10; } /// 平面とした場合の値。現時点ではRoundConeについてのみ実装されている(09/02/08, mitake)
	///< 表面上の点pにおける法線
	virtual Vec3d Normal(Vec3d p){ return Vec3d(); } /// 現時点ではRoundConeについてのみ実装されている(09/02/14, mitake)

	virtual float CalcVolume(){
		if(!valid)Analyze();
		return volume;
	}
	virtual Vec3f	CalcCenterOfMass(){
		if(!valid)Analyze();
		return center;
	}
	virtual Matrix3f CalcMomentOfInertia(){
		if(!valid)Analyze();
		return moment;
	}

	CDConvex():valid(false){}
};

}	//	namespace Spr
#endif
