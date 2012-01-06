#ifndef PHHAPTICRENDER_MULTIBASE_H
#define PHHAPTICRENDER_MULTIBASE_H

#include <Physics/PHHapticEngine.h>

namespace Spr{;

//----------------------------------------------------------------------------
// PHHapticLoopImp
class PHHapticLoopImp : public SceneObject{
	SPR_OBJECTDEF_NOIF(PHHapticLoopImp);
public:
	int loopCount;
	PHHapticRenderImp* renderImp;
	PHHapticPointers hapticPointers;
	PHSolidsForHaptic hapticSolids;
	UTCombination< UTRef<PHSolidPairForHaptic> > solidPairs; 

	PHHapticLoopImp(){}
	virtual void Step(){};
	double GetPhysicsTimeStep();
	double GetHapticTimeStep();
	int NHapticPointers();
	int NHapticSolids();
	PHHapticPointer* GetHapticPointer(int i);
	PHSolidForHaptic* GetHapticSolid(int i);
	PHSolidPairForHaptic* GetSolidPairForHaptic(int i, int j);
	PHHapticPointers* GetHapticPointers();
	PHSolidsForHaptic* GetHapticSolids();
	
	//PHHapticPointers* GetHapticPointers();
	//PHSolidForHaptic** GetHapticSolids();
};

//----------------------------------------------------------------------------
// PHHapticRenderMultiBase
class PHHapticRenderMultiBase : public PHHapticRenderImp{
	SPR_OBJECTDEF_NOIF(PHHapticRenderMultiBase);
protected:
	volatile int	hapticCount;
	volatile bool	bSync;
	bool			bCalcPhys;

public:
	PHHapticLoopImp* hapticLoop;
	PHHapticRenderMultiBase();
	virtual void StepHapticLoop();	// hapticloop
	virtual void Step(){}			// phengine->phhapticengine����Ă΂��
	void Sync();
	virtual void UpdateHapticPointer(); // haptic�X���b�h����hapticpointer�̏�Ԃ�physics�֓���
	virtual void UpdateArrays();
	virtual void SyncHaptic2Physic(){}	// haptic�X���b�h����physics�X���b�h�ւ̓���
	virtual void SyncPhysic2Haptic(){}	// physics�X���b�h����haptic�X���b�h�ւ̓���

	//�f�o�b�N�p�R�[�h
	virtual void StepSimulation();
};

}

#endif