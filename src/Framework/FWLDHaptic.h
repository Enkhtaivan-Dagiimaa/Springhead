/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

#ifndef FWLDHaptic_H 
#define FWLDHaptic_H

#include <Framework/FWMultiRateHaptic.h>
#include <Framework/FWInteractScene.h>

class FWLDHapticLoop : public FWHapticLoopBase{
private:
	Vec3d vibV;
	double vibT;
	double proK;
	double proD;
	double proM;
	std::vector<Vec3d> pLastPoint[2];
	std::vector<Vec3d> proxy[2];
	std::vector<Vec3d> proVel[2];
	Vec3d dproxy;
	Vec3d DisplayProxy[2];				//	デバッグ用
	Vec3d DisplayForce[2];				//	プロット用
	bool contactFlag[2];				//	プロット用
public:
	FWLDHapticLoop();
	virtual void Step();
	virtual void UpdateInterface();
	virtual void HapticRendering();
	virtual void Proxy();
	virtual void ProxySimulation();
	virtual Vec3d Vibration(FWInteractSolid*, FWInteractPointer*, int);
	Vec3d* GetProxyPoint();				//	デバッグ用
	Vec3d* GetForce();
	bool GetContactFlag();				//	把持しているかの判定
//	int GetPicTime();
//	int GetPenetrateLength();
	virtual void LocalDynamics();
};


class FWLDHaptic : public FWMultiRateHaptic{
protected:
	UTRef<ObjectStatesIf> states, states2;
	FWLDHapticLoop LDHapticLoop;
public:
	FWLDHaptic();
	virtual void Init();
	virtual void Clear();
	void CallBackHapticLoop();
	virtual void Step();
	virtual void PhysicsStep();
	virtual void UpdatePointer();
	virtual void TestSimulation();
	virtual void BeginKeyboard();
	virtual void EndKeyboard();
	void ReleaseState(PHSceneIf*);
	Vec3d* GetProxyPoint();
	Vec3d* GetForce();
	bool GetContactFlag();

};

#endif