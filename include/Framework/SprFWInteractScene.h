/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_FWINTERACT_SCENEIF_H
#define SPR_FWINTERACT_SCENEIF_H

#include <Foundation/SprScene.h>
#include <Framework/SprFWScene.h>
#include <Framework/SprFWInteractPointer.h>
#include <Framework/FWInteractAdaptee.h>

namespace Spr{;

class FWInteractAdaptee;

typedef enum FWInteractMode{
	NONE,
	LOCAL_DYNAMICS
}INMode;

struct FWInteractSceneDesc{
public:
	FWSceneIf* fwScene;
	FWInteractMode mode;
	double hdt;
	FWInteractSceneDesc(){Init();}
	void Init(){
		fwScene = NULL;
		mode = NONE;
		hdt = 0.001;
	}
};

struct FWInteractSceneIf : public SceneIf{
public:
	SPR_IFDEF(FWInteractScene);


	FWSceneIf* GetScene();
	void CreateINAdaptee(FWInteractMode inMode);
	FWInteractAdaptee*		GetINAdaptee();
	FWInteractPointerIf*	CreateINPointer(const FWInteractPointerDesc& desc);
	FWInteractPointerIf*	GetINPointer(int i = -1);
	int						NINPointers();
	FWInteractMode			GetINMode();

	/// �ȉ��̊֐���adaptee���Ƃ����Ď������Ă΂��
	void Init();
	void Step();
	void Clear();
	void Reset();
	void CallBackHapticLoop();
};

typedef UTRef<FWInteractSceneIf> UTRef_FWInteractSceneIf;
typedef std::vector<UTRef_FWInteractSceneIf> FWInteractScenes;

}
#endif
