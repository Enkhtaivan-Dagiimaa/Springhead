/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */


#ifndef SPR_PHHAPTIC_H
#define SPR_PHHAPTIC_H

#include <Foundation/SprObject.h>

namespace Spr{;

struct PHHapticRenderDesc{
	enum HapticRenderMode{
		PENALTY,
		CONSTRAINT,
	};
};
struct PHHapticRenderIf : public PHHapticRenderDesc, public SceneObjectIf{
	SPR_IFDEF(PHHapticRender);
	void SetHapticRenderMode(HapticRenderMode m);
	void EnableMultiPoints(bool b);
};

struct HIBaseIf;
struct PHHapticPointerIf : public PHSolidIf{
	SPR_IFDEF(PHHapticPointer);
	void	SetHumanInterface(HIBaseIf* hi);
	void	EnableForce(bool b);	
	void	EnableFriction(bool b);
	void	EnableVibration(bool b);
	void	EnableDebugControl(bool b);

	void	SetReflexSpring(float s);
	float	GetReflexSpring();
	void	SetReflexDamper(float d);
	float	GetReflexDamper();
	void	SetReflexSpringOri(float s);
	float	GetReflexSpringOri();
	void	SetReflexDamperOri(float d);
	float	GetReflexDamperOri();
	void	SetLocalRange(float r);
	float	GetLocalRange();
	void	SetPosScale(double scale);
	double	GetPosScale();
	void	SetRotationalWeight(double w);
	double	GetRotationalWeight();
	void	SetDefaultPose(Posed p);
	Posed	GetDefaultPose();

};

}

#endif