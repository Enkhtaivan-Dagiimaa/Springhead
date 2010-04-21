/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

#ifndef FWLDHaptic3D_H 
#define FWLDHaptic3D_H

#include <Framework/FWMultiRateHaptic.h>
#include <Framework/FWInteractScene.h>

class FWLDHaptic3DLoop : public FWHapticLoopBase{
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
	std::vector<Vec3d> oVibForce;
	int picNum;								//	�c�����Ă��镨�̂̔ԍ�
	Vec3d DisplayProxy[2];					//	�f�o�b�O�p
	Vec3d DisplayForce[2];					//	�v���b�g�p
	bool bPic;								//	�v���b�g�p
	std::vector<bool> contactFlag[2];		//	�v���b�g�p
public:
	FWLDHaptic3DLoop();
	virtual void Step();
	virtual void HapticRendering3D();
	virtual void HapticRendering();
	virtual void Proxy();
	virtual void ProxySimulation();
	int* ContactStat(int nPic);
	virtual Vec3d Vibration(FWInteractSolid*, FWInteractPointer*, int);		//	�|�C���^-����
	virtual void PicVibration(int nSolids);									//	����-����

	Vec3d* GetProxyPoint();					//	�f�o�b�O�p
	Vec3d* GetForce();
	bool GetContactFlag();					//	�c�����Ă��邩�̔���
	Vec3d GetPicCenterPosition();			//	�c�����Ă��镨�̂̏d�S�ʒu
//	int GetPicTime();
	virtual void LocalDynamics();
};


class FWLDHaptic3D : public FWMultiRateHaptic{
protected:
	UTRef<ObjectStatesIf> states, states2;
	FWLDHaptic3DLoop ldLoop3D;
public:
	FWLDHaptic3D();
	virtual void Init();
	virtual void Clear();
	void CallBackHapticLoop();
	virtual void SyncHaptic2Physic();
	virtual void SyncPhysic2Haptic();
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
	Vec3d GetPicCenterPosition();
};

#endif