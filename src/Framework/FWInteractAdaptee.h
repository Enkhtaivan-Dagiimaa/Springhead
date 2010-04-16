/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

#ifndef FWINTERACT_ADAPTEE_H
#define FWINTERACT_ADAPTEE_H

#include <Springhead.h>
#include <Framework/FWInteractPointer.h>
#include <Framework/FWInteractSolid.h>

namespace Spr{;

class FWInteractScene;
class FWInteractPointer;
enum FWHapticMode;
class FWInteractAdaptee;

/** Haptic�C���^���N�V�����̂��߂̃N���X*/
class FWHapticLoopBase : public UTRefCount{
protected:
	std::vector<FWInteractPointer>	interactPointers;
	std::vector<FWInteractSolid>	interactSolids;
	FWInteractAdaptee* interactAdaptee;

public:
	double pdt, hdt;
	int loopCount;
	FWHapticMode hmode;
	FWHapticLoopBase(){}
	FWInteractPointer* GetIAPointer(int i){ return &interactPointers[i]; }
	std::vector<FWInteractPointer>* GetIAPointers(){ return &interactPointers;}
	int NIAPointers(){ return (int)interactPointers.size();}
	FWInteractSolid* GetIASolid(int i){ return &interactSolids[i]; } 
	FWInteractSolids* GetIASolids(){ return &interactSolids; }
	int NIASolids(){ return (int)interactSolids.size(); }
	int GetLoopCount(){ return loopCount; }
	FWInteractAdaptee* GetIAAdaptee(){ return interactAdaptee; }
	void SetIAAdaptee(FWInteractAdaptee* ia){ interactAdaptee = ia; }

	virtual void Clear();
	void Init(double physicdt, double hapticdt){
		pdt = physicdt;
		hdt = hapticdt;
		loopCount = 1;
	}
	virtual void Step(){};
	virtual void HapticRendering(){}
	virtual void UpdateInterface();
	virtual void SetRenderedForce(HIBaseIf* hi, bool bForce, SpatialVector f);
};


/** HumanInterface���g���ăC���^���N�V�������\�ɂ���N���X 
	���̃N���X���p�������Ď������ĂԁD
*/
class FWInteractAdaptee : public UTRefCount{
protected:
	FWInteractScene*	interactScene;
public:
	FWInteractAdaptee();

	/// IAScene�֌W
	void SetIAScene(FWInteractScene* inScene);
	FWInteractScene* GetIAScene();
	PHSceneIf* GetPHScene();

	/// HapticLoop�֌W
	virtual FWHapticLoopBase* GetHapticLoop();
	void InitIAAdaptee();
	void SetHMode(FWHapticMode hMode);

	/// IAPointer�֌W(IAScene����Ď擾)
	FWInteractPointer* GetIAPointer(int i = -1);
	FWInteractPointers* GetIAPointers();
	int NIAPointers();

	/// IASolid�֌W(IAScene����Ď擾)
	FWInteractSolid* GetIASolid(int i = -1);
	FWInteractSolids* GetIASolids();
	int NIASolids();

	void UpdateSolidList();

	virtual void UpdatePointer(){};													///< �|�C���^�̈ʒu���X�V����
	void NeighborObjectFromPointer();												///< �|�C���^�ߖT�̕��̂�T��
	/// �ȉ�2�̊֐���NeighborObjectFromPointer()�ŌĂ΂��
	virtual void UpdateInteractSolid(int index, FWInteractPointer* iPointer);		///< InteractSolid�̏����X�V����(�I�[�o�[���C�h���Ďg�p)
	int FindNearestPoint(const CDConvexIf* a, const CDConvexIf* b,
											const Posed& a2w, const Posed& b2w, const Vec3d pc, Vec3d& dir, 
											Vec3d& normal, Vec3d& pa, Vec3d& pb);	///< �|�C���^�ߖT�̕��̂Ƃ̋ߖT�_��T��
	void AnalyzeContactResion(PHSolid* solida, PHSolid* solidb, Vec3d pa, Vec3d pb, ToHaptic* th);

	virtual void Clear(){};
	virtual void Init(){};
	virtual void CallBackHapticLoop(){};
	virtual void Step(){};
	virtual void BeginKeyboard(){};
	virtual void EndKeyboard(){};
	//virtual Vec3d* GetProxyPoint();
	//virtual Vec3d* GetForce();
	virtual bool GetContactFlag();
	virtual Vec3d GetPicCenterPosition();
	virtual void CreatePointerSolid(){};
};

}
#endif