#ifndef FWINTERACT_ADAPTEE_H
#define FWINTERACT_ADAPTEE_H

#include <Springhead.h>
#include <Framework/FWInteractPointer.h>
#include <Framework/FWInteractSolid.h>

namespace Spr{;

class FWInteractScene;

/** Haptic�C���^���N�V�����̂��߂̃N���X*/
class FWHapticLoopBase : public UTRefCount{
protected:
	std::vector<FWInteractPointer>	interactPointers;
	std::vector<FWInteractSolid>	interactSolids;
public:
	double pdt, hdt;
	int loopCount;
	FWHapticLoopBase(){}
	FWInteractPointer* GetInteractPointer(int i){ return &interactPointers[i]; }
	std::vector<FWInteractPointer>* GetInteractPointers(){ return &interactPointers;}
	int NInteractPointers(){ return (int)interactPointers.size();}
	FWInteractSolid* GetInteractSolid(int i){ return &interactSolids[i]; } 
	FWInteractSolids* GetInteractSolids(){ return &interactSolids; }
	int NInteractSolids(){ return (int)interactSolids.size(); }
	int GetLoopCount(){ return loopCount; }

	void Init(double physicdt, double hapticdt){
		pdt = physicdt;
		hdt = hapticdt;
		loopCount = 1;
	}
	virtual void Clear();
	virtual void Step(){};
	virtual void HapticRendering(){}
};


/** HumanInterface���g���ăC���^���N�V�������\�ɂ���N���X 
	���̃N���X���p�������Ď������ĂԁD
*/
class FWInteractAdaptee : public UTRefCount{
protected:
	FWInteractScene*	interactScene;
public:
	FWInteractAdaptee();

	/// INScene�֌W
	void SetINScene(FWInteractScene* inScene);
	FWInteractScene* GetINScene();
	PHSceneIf* GetPHScene();

	/// HapticLoop�֌W
	virtual FWHapticLoopBase* GetHapticLoop();

	/// INPointer�֌W(INScene����Ď擾)
	FWInteractPointer* GetINPointer(int i = -1);
	FWInteractPointers* GetINPointers();
	int NINPointers();

	/// INSolid�֌W(INScene����Ď擾)
	FWInteractSolid* GetINSolid(int i = -1);
	FWInteractSolids* GetINSolids();
	int NINSolids();

	void UpdateSolidList();

	virtual void UpdatePointer(){};													///< �|�C���^�̈ʒu���X�V����
	void NeighborObjectFromPointer();												///< �|�C���^�ߖT�̕��̂�T��
	/// �ȉ�2�̊֐���NeighborObjectFromPointer()�ŌĂ΂��
	virtual void UpdateInteractSolid(int index, FWInteractPointer* iPointer);		///< InteractSolid�̏����X�V����(�I�[�o�[���C�h���Ďg�p)
	double FindNearestPoint(const CDConvexIf* a, const CDConvexIf* b,
											const Posed& a2w, const Posed& b2w, const Vec3d pc, Vec3d& dir, 
											Vec3d& normal, Vec3d& pa, Vec3d& pb);	///< �|�C���^�ߖT�̕��̂Ƃ̋ߖT�_��T��

	virtual void Init(){};
	virtual void CallBackHapticLoop(){};
	virtual void Clear(){};
	virtual void Reset(){};
	virtual void Step(){};
};

}
#endif