﻿/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_PHOPOBJ_H
#define SPR_PHOPOBJ_H

#include <Foundation/SprObject.h>

namespace Spr{;

///	剛体のステート
struct PHOpObjState{
	Vec3d		velocity;		///<	質量中心の速度		(World系)
	Vec3d		angVelocity;	///<	角速度				(World系)
//	Vec3d		lastVelocity;	///<	前回の速度			(World系)
//	Vec3d		lastAngVelocity;///<	前回の角速度			(World系)
	Posed		pose;			///<	座標原点の位置と向き	(World系)
//	Posed		lastPose;		///<	前回の位置と向き		(World系)
};

struct PHOpHapticRendererDesc{
	float extForceSpring;
	float outForceSpring;
	float constraintSpring;
	float timeStep;
	float forceOnRadius;

	bool useConstrainForce;
	bool useProxyfixSubitr;
	bool useIncreaseMeshMove;
	bool sqrtAlphaForce;
	bool hitWall;
	bool useDualTranFix;
	bool rigid;

	int proxyItrtNum;
	int proxyItrtMaxNum;
	int currDtType;
	int objNum;
	int noCtcItrNum;

	Vec3f proxyFixPos;
};

struct PHOpObjDesc : public PHOpObjState{
	double		mass;			///<	質量
	Matrix3d	inertia;		///<	慣性テンソル	(Local系)
	Vec3d		center;			///<	質量中心の位置	(Local系)
	bool		dynamical;		///<	物理法則に従うか(速度は積分される)

	////model頂点群
	Vec3f *objTargetVts;
	
	//model頂点データ
	float objTargetVtsArr[10000];

	//model頂点群数
	int objTargetVtsNum;
	//粒子の数
	int assPsNum;
	//粒子groupの数
	int assGrpNum;
	
	//粒子groupのlink数定数
	int objGrouplinkCount;
	//ObjectID
	int objId;
	//距離拘束のswitch
	bool objUseDistCstr;
	//Meshを持っていないObject
	bool objNoMeshObj;
	//剛体か
	bool isRigid;
	//力覚Objectと区別のため使う
	int objType;
	//半径の平均
	float objAverRadius;
	//半径の平均
	float objMaxRadius;

	//繰り返す数え計数
	int objitrTime;

	//FaceNormalをBlendのときに更新するか
	bool updateNormals;


	PHOpObjDesc(){ Init(); }
	void Init(){
		mass = 1.0f;
		inertia = Matrix3d::Unit();
		dynamical = true;
	}
};

struct CDShapeIf; 
struct PHTreeNodeIf;

///	剛体のインタフェース
struct PHOpObjIf : public SceneObjectIf {
	SPR_IFDEF(PHOpObj);
	
	
	bool initialPHOpObj(Vec3f *vts, int vtsNum, float pSize);
	void		SetGravity(bool bOn);
	void SimpleSimulationStep();

	void AddVertextoLocalBuffer(Vec3f v);
	bool InitialObjUsingLocalBuffer(float pSize);

	void positionPredict();
	void groupStep();
	void integrationStep();
	void ReducedPositionProject();
	void positionProject();
	void SetDefaultLinkNum(int linkNum);

	void BuildBlendWeight();
	void buildGroupCenter();
	int GetVertexNum();
	Vec3f GetVertex(int vi);
	ObjectIf* GetOpParticle(int pi);
	ObjectIf* GetOpGroup(int gi);
	int GetobjVtoPmap(int vi);
	float GetVtxBlendWeight(int Vtxi, int Grpi);
	
	void SetVelocityDamping(float vd);
	float GetVelocityDamping();
	float GetBoundLength();
	void SetBound(float b);
	void SetTimeStep(float t);
	float GetTimeStep();
	float GetObjBeta();
	void SetObjBeta(float beta);
	void SetObjAlpha(float alpha);
	float GetObjAlpha();
	bool GetObjDstConstraint();
	void SetObjDstConstraint(bool d);
	void SetObjItrTime(int itrT);
	int GetObjItrTime();
	void StoreOrigPose();


};
struct PHOpParticleDesc {

	//頂点数
	int pNvertex;
	//自分のグループid
	int pMyGroupInd;
	//粒子のid
	int pPId;
	//所属objのid
	int pObjId;
	//粒子硬さ係数alpha(開発中)
	float pParaAlpha;
	//マウスに触られたかフラグ
	bool hitedByMouse;
	//頂点持つ質量（仮）
	float pSingleVMass;
	//粒子の総合質量（開発中）
	float pTotalMass;
	
	//粒子球半径（球として扱う時に使う）
	float pRadii;
	//動的粒子半径
	float pDynamicRadius;
	//主半径vector
	Vec3f pMainRadiusVec;
	//副半径vector
	Vec3f pSecRadiusVec;
	//従半径vector
	Vec3f pThrRadiusVec;
	//主半径
	float pMainRadius;
	//副半径
	float pSecRadius;
	//従半径
	float pThrRadius;
	//粒子の現在位置
	Vec3f pCurrCtr;
	//粒子の初期位置
	Vec3f pOrigCtr;
	//粒子の予測位置
	Vec3f pNewCtr;
	//粒子の速度
	Vec3f pVelocity;
	//SPhash に検出されたか
	bool isColliedbySphash;
	//ColliCubeに検出されたか
	bool isColliedbyColliCube;
	//マウスにつかまれたか
	bool isFixedbyMouse;
	//固定してるか
	bool isFixed;
	//SPhash に解決すべしか
	bool isColliedSphashSolved;
	//SPhash にまもなく解決すべしか
	bool isColliedSphashSolvedReady;
	
	//外力
	Vec3f pExternalForce;
	//not yet been used
	Vec3f pExternalTorque;//not yet been used
	//not yet been used
	Vec3f pExternalDisplacement;
	//含むメッシュのindex
	std::vector<int> pFaceInd;
	
	//粒子モーメント行列
	Matrix3f pMomentR;
	//粒子分解後回転行列
	Matrix3f pSmR;
	
	//頂点慣性モーメント 
	Matrix3f pMomentInertia;
	//頂点慣性モーメントの逆 
	Matrix3f pInverseOfMomentInertia;
	//頂点の主成分分析PCAによる楕円の向き行列
	Matrix3f ellipRotMatrix;
	//粒子固有向き、ellipRotMatrixから
	Quaternionf ellipOrigOrint;
	//粒子角速度
	Vec3f pWvel;//角速度、長さは速度の大きさ、方向は速度の方向
	//used to precalculation in Collision Detection;
	Matrix3f pPreEllipA;

	//現在向き
	Quaternionf pCurrOrint;
	//初期向き
	Quaternionf pOrigOrint;
	//予測向き
	Quaternionf pNewOrint;
	

};
struct PHOpGroupDesc {
	//粒子グループのid
	int gMyIndex;
	//グループのモーメント行列
	Matrix3f gAgroup;
	//グループの固有ベクトル群
	Matrix3f gEigen;
	//グループの質量
	float gtotalMass;
	//初期中心
	Vec3f gOrigCenter;
	//現在中心
	Vec3f gCurrCenter;
	//このグループに含む粒子の数
	int gNptcl;
};
struct PHOpParticleIf : public ObjectIf{
	SPR_IFDEF(PHOpParticle);
	int GetOpPtclVtxId(int vi);
	int GetinGrpListNum();
	int GetinGrpList(int gi);
	float GetVtxDisWeight(int vi);
	PHOpParticleDesc* GetParticleDesc();
};
struct PHOpGroupIf : public ObjectIf{
	SPR_IFDEF(PHOpGroup);

	int GetGrpInPtclNum();
	int GetGrpInPtcl(int gi);
	PHOpGroupDesc* GetGroupDesc();
};
struct PHOpHapticControllerDesc
{
	float posScale;
	float rotScale;
	float forceScale;
	Vec3f userPos;
	Posef userPose;
	Vec3f hcCurrPPos;
	Vec3f hcCurrUPos;
	Vec3f hcLastUPos;
	Vec3f hcFixsubGoal;
	float c_obstRadius;

	int hpObjIndex;
	enum HapticDOFType
	{
		_3DOF,
		_6DOF,
	};

	HapticDOFType hcType;
	bool logForce;
	bool hcReady;
	bool IsSetForceReady;

	int fileindex;
	Vec3f lastHpRateF;
	bool hcCollied;
	//proxy
	int constrainCount;//used in dynamic constrain feel, indicate the constrainNum now
	int constrainCountHc;//used in Haptic rate
	Vec3f couterPlaneN;
	float hcElmDtcRadius;
	int collectCount;
	int collectItrtor;
	bool hcProxyOn;
	int suspObjid;

	float proxyRadius;
	int surrCnstrs;// detected constrains Num before fix

	enum cstrainType{
		cstPoint = 1,
		cstEdge,
		cstFace

	};

	struct PvsHcCtcPInfo
	{
		int ptclIndex;
		int objIndex;
		Vec3f ctcPos;
	};

};
struct PHOpHapticControllerIf : public SceneObjectIf{
	SPR_IFDEF(PHOpHapticController);

	void SetHCForceReady(bool flag);
	bool GetHCForceReady();
	bool CheckProxyState();
	void AddColliedPtcl(int pIndex, int objindex, Vec3f ctcPos);
	bool BeginLogForce();
	void EndLogForce();
	void setC_ObstacleRadius(float r);
	Vec3f GetUserPos();
	Vec3f GetHCPosition();
	void SetHCPosition(Vec3f pos);
	void SetHCPose(Posef pose);
	Posef GetHCPose();
	float GetC_ObstacleRadius();
	Vec3f GetCurrentOutputForce();
};
struct PHOpHapticRendererIf : public SceneObjectIf{
	SPR_IFDEF(PHOpHapticRenderer);
#ifdef USEGRMESH
	void ProxySlvPlane();
	void ProxyMove();
	void ProxyTrace();
	bool ProxyCorrection();
	void ForceCalculation();
	void setForceOnRadius(float r);
	void SetForceSpring(float k);
	float GetForceSpring();
	void SetConstraintSpring(float k);
	float GetConstraintSrping();

	void HpNoCtcProxyCorrection();
	void HpConstrainSolve(Vec3f &currSubStart);
	void BuildVToFaceRelation();
	void BuildEdgeInfo();
#endif
	void SetRigid(bool set);
	bool IsRigid();
};

struct PHOpAnimationDesc  {

	struct OpAnimationStruct
	{
		int animatedPindex;
		int animatedObjindex;
		int animatedType;
		int animatedHoldtimes;

		Vec3f animatedForces;
		Vec3f animatedPStarts;
		Vec3f animatedPEnds;

	};
	float timestep;
};
struct PHOpAnimationIf :public  SceneObjectIf, public PHOpAnimationDesc{
	SPR_IFDEF(PHOpAnimation);

	void AddAnimationP(int objIndex, int pIndex, Vec3f start, Vec3f end, float totalTime);
	void AddAnimationP(int objIndex, int pIndex, Vec3f force, float totalTime);
	void AnimationStep(void* opEngine);
	void AnimationIntergration(void* opEngine);
	
};



}	//	namespace Spr
#endif
