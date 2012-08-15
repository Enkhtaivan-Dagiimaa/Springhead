﻿/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 *	@file SprPHFemMesh.h
 *	@brief FEMシミュレーションのための四面体メッシュ
*/
#ifndef SPR_PHFemMeshIf_H
#define SPR_PHFemMeshIf_H

#include <Foundation/SprObject.h>

/**	\addtogroup gpPhysics	*/
//@{
namespace Spr{;

///	FemMeshのステート
struct PHFemMeshState{
};
///	FemMeshのディスクリプタ
struct PHFemMeshDesc: public PHFemMeshState{
	std::vector<Vec3d> vertices;
	std::vector<int> tets;
	std::vector<int> faces;
	PHFemMeshDesc();
	void Init();
};

///	FEM用の四面体メッシュ
struct PHFemMeshIf : public SceneObjectIf{
	SPR_IFDEF(PHFemMesh);
	//int GetSurfaceVertex(int id);
	//int NSurfaceVertices();
	//void SetVertexTc(int id,double temp);
	//Vec3d GetPose(unsigned id);
	//Vec3d GetSufVtxPose(unsigned id);
	//std::vector<Spr::PHFemMesh::Edge> GetEdge();
	
};

///	FemMeshThermoのディスクリプタ
struct PHFemMeshThermoDesc: public PHFemMeshDesc{
	double rho;						//	密度
	double thConduct;				//熱伝導率
	double heatTrans;				//熱伝達率			//class 節点には、heatTransRatioが存在する
	double specificHeat;			//比熱
	PHFemMeshThermoDesc();
	void Init();
};

///	温度のFEM用のメッシュ
struct PHFemMeshThermoIf : public PHFemMeshIf{
	SPR_IFDEF(PHFemMeshThermo);
	int GetSurfaceVertex(int id);
	int NSurfaceVertices();
	void SetVertexTc(int id,double temp);
	void SetVertexTc(int id,double temp,double heatTrans);
	Vec3d GetPose(int id);
	Vec3d GetSufVtxPose(unsigned id);
	unsigned long GetStepCount();		///	カウント１
	unsigned long GetStepCountCyc();		///	カウント１が何週目か	計算式:TotalCount = GetStepCount() + GetStepCountCyc() * (1000 * 1000 * 1000) 
	void SetVertexTemp(unsigned id,double temp);
	void SetvecFAll(unsigned id,double dqdt);
	void SetRohSpheat(double roh,double Cp);		//素材固有の物性
	unsigned GetNFace();
	std::vector<Vec3d> GetFaceEdgeVtx(unsigned id);
	Vec3d GetFaceEdgeVtx(unsigned id, unsigned	 vtx);
	Vec2d GetIHbandDrawVtx();
	void CalcIHdqdt_atleast(double r,double R,double dqdtAll);
	void UpdateIHheat(unsigned heating);						//IH加熱状態の更新
};

//@}

}	//	namespace Spr
#endif
