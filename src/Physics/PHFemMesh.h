/*
 *  Copyright (c) 2003-2011, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PHFEMMESH_H
#define PHFEMMESH_H

#include <Physics/SprPHFemMesh.h>
#include <Foundation/Object.h>
#include <Physics/PHScene.h>

namespace Spr{;



/**	有限要素法シミュレーションのためのメッシュ(4面体メッシュ)

	有限要素法では、辺が、頂点・四面体から参照できることが必要になる。
	有限要素法では、要素（＝四面体）の体積や面積の積分から、各ノードの物理量を求める。
	各ノード(頂点)は複数の四面体で共有されているので、頂点や頂点と頂点(＝辺)は、
	複数四面体の係数を合成したものを持つことになる。

	隣り合う頂点と頂点(=辺)の間に係数がある。
	   		    i列 j列
	+------------------+	        
	|		     x     |
	|                  |   係数行列Aは左のようになる。 要素　ii, jj（対角成分） や ij, ji（非対角成分） に値が入る(i,jは頂点、ijは辺)					//(ii,jj)が頂点、ijやji...etcは辺
i行	|         x  d  o  | 
	|                  |
j行	| 		     o  d  |　
	|                  |
	+------------------+
	
			  
	Ax + b = 0,  (D-F)x + b = 0 , x = D^{-1} F x -  D^{-1} b, x = D^{-1} F x - b'
	x_i =   (F_i * x)/D_ii - b_i/D_ii ... (1)
	i->j, j->i で同じ係数(F_ij, F_ji)が必要だが、計算は上から行うので、両方から係数が見えないとならない。
	
	1. 全四面体について係数を求め、辺に係数を記録していく
    2. ガウスザイデルの計算＝上から１行ずつ、式(1)の計算をする。
	　 この時、F_ij=辺の係数 が必要なので、頂点iから辺ixが参照したくなる。
	
	辺の係数F_ijを計算するためには、辺ijを含む四面体の係数の和を求める必要がある。
	係数の計算は四面体単位で行うので、四面体から辺への参照も必要。
	
	∴辺構造体 Edge が必要
**/
class PHFemMesh: public SceneObject{
public:
	//	頂点
	struct FemVertex{
		Vec3d pos;
		std::vector<int> tets;
		std::vector<int> edges;
		FemVertex();
		double temp;		//	温度
		double k;			//	全体剛性行列Kの要素　計算に利用
		double c;			//	同上
	};
	//	四面体
	struct Tet{
		int vertices[4];	//	頂点ID																																	  
		int faces[4];		//	表面４つ
		int edges[6];		//	対応する辺のID。0:辺01, 1:辺12, 2:辺20, 3:辺03, 4:辺13, 5:辺23
		int& edge(int i, int j);
		double volume;		//	積分計算で用いるための体積
	};
	//	四面体の面。
	class Face{
		///	比較するための、ソート済みの頂点id。Update()で更新。
		int sorted[3];
	public:
		///	頂点ID。順番で面の表裏を表す。
		int vertices[3];
		void Update();
		///	頂点IDで比較
		bool operator < (const Face& f2);
		///	頂点IDで比較
		bool operator == (const Face& f2);
		//行列計算に用いるための面積
		double area;			//四面体の各面の面積
	};
	//	辺
	struct Edge{
		Edge(int v1=-1, int v2=-1);
		int vertices[2];
		///	頂点IDで比較
		bool operator < (const Edge& e2);
		///	頂点IDで比較
		bool operator == (const Edge& e2);
		double k;		//全体剛性行列Kの要素
		double c;		//	
	};
	///	基本情報(生成時にデスクリプタから与えられる情報)
	//@{
	///	頂点
	std::vector<FemVertex> vertices;
	///	四面体
	std::vector<Tet> tets;
	//@}
	
	/// @name 追加情報	基本情報からSetDesc()が計算して求める。
	//@{
	///	物体表面の頂点のID
	std::vector<int> surfaceVertices;
	///	面
	std::vector<Face> faces;
	///	面のうち物体表面のものが、faces[0]..faces[nSurfaceFace-1]
	unsigned nSurfaceFace;
	///	辺
	std::vector<Edge> edges;
	///	辺のうち物体表面のものが、edges[0]..edges[nSurfaceEdge-1]
	unsigned nSurfaceEdge;
	//@}

	/// @name 四面体の計算	対象によらずに必要になる形状関数のXXを計算する関数
	//@{
	void UpdateJacobian();
	//@}


	SPR_OBJECTDEF(PHFemMesh);
	PHFemMesh(const PHFemMeshDesc& desc=PHFemMeshDesc(), SceneIf* s=NULL);
	///	デスクリプタのサイズ
	virtual size_t GetDescSize() const { return sizeof(PHFemMeshDesc); };
	///	デスクリプタの読み出し(コピー版)
	virtual bool GetDesc(void* desc) const ;
	///デスクリプタの設定。ここで、頂点座標と四面体の4頂点のIDの情報から、面や辺についての情報を計算しておく。
	virtual void SetDesc(const void* desc);
	///	時刻をdt進める処理。PHFemEngineが呼び出す。
	virtual void Step(double dt);
};




}	//	namespace Spr
#endif
