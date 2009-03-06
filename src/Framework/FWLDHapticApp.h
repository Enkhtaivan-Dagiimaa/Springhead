/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

#ifndef FWLD_Haptic_App_H 
#define FWLD_Haptic_App_H

#include <Framework/FWHapticBase.h>
#include <Framework/FWMultiRateHapticApp.h>

class FWLDHapticProcess : public FWHapticBase{
public:
	FWExpandedPHSolids expandedPHSolids;
	void Step();
	void LocalDynamics();
	FWExpandedPHSolid** GetFWExpandedPHSoilds();
	int GetNExpandedPHSolids();
};

class FWLDHapticApp : public FWMultiRateHapticApp{
public:
	UTRef<ObjectStatesIf> states, states2;
	FWLDHapticProcess hprocess;
	virtual void ResetScene();
	virtual void CallBack();
	virtual void Step();
	virtual void TestSimulation();
};


#endif