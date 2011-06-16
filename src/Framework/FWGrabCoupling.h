/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef FWGRABCOUPLING_H
#define FWGRABCOUPLING_H

#include <Framework/FWInteractAdaptee.h>

namespace Spr{;

class FWGrabCoupling : public FWInteractAdaptee{
public:
	std::vector<PHSolidIf*> vcSolid;	
	std::vector<PHJointIf*> vcJoint;
	std::vector<PHJointIf*> grabJoint;
protected:
	volatile int hapticcount;
	volatile bool bSync;
	bool bCalcPhys;
	virtual void CreateHapticLoop(){}
public:
	FWGrabCoupling();

	virtual void UpdatePointer();
	void		 UpdatePointerDirect(); //���ڃ|�C���^���X�V����i�͏o�͂Ȃ��j
	virtual void Clear();
	virtual void Init(){
			FWInteractAdaptee::Init();
	};
	virtual void Step();
	void UpdateInterface();
	virtual void CreatePointerSolid();
	void CallBackHapticLoop(){};

	///���̂�͂��VC����ꍇ
	void GrabSolid();
	void GrabSolid2();
	void AdjustSpring(double n);
	void UpdateGrabPointer();
	PHSolidDesc grabSolidDesc;
	CDShapeIf* pointerShape;

	Posed grabPointerPose;


	///�N���b�`�@�\
	Posed cluchPose[2];			//cluchPose[0]:�N���b�`�O cluchPose[1]:�N���b�`��
	Posed cluchTrans;
	bool cluchFlag;			//cluchFlag[0] ���݂�flag cluchFlag[1] 1�X�e�b�v�O��flag
	void OnCluch(){ cluchFlag = true;}
	void OffCluch(){ cluchFlag = false;}

	SpatialVector lastOutForce;


};
}
#endif