#ifndef PH_HAPTICENGINE_MULTIBASE_H
#define PH_HAPTICENGINE_MULTIBASE_H

#include <Physics/PHHapticEngine.h>
#include <Physics/PHHapticRenderBase.h>

namespace Spr{;

//----------------------------------------------------------------------------
// PHHapticLoopImp
class PHHapticLoopImp : public PHHapticRenderBase, public SceneObject{
	SPR_OBJECTDEF_NOIF(PHHapticLoopImp);
protected:
	PHHapticPointers hapticPointers;
	PHSolidsForHaptic hapticSolids;
	PHSolidPairsForHaptic solidPairs;
public:
	PHHapticEngineImp* engineImp;
	int loopCount;

	PHHapticLoopImp(){}
	virtual void Step(){};
	virtual void UpdateInterface();
	virtual void HapticRendering(){};

	double GetPhysicsTimeStep();
	double GetHapticTimeStep();
	int NHapticPointers();
	int NHapticSolids();
	PHHapticPointer* GetHapticPointer(int i);
	PHSolidForHaptic* GetHapticSolid(int i);
	PHSolidPairForHaptic* GetSolidPairForHaptic(int i, int j);
	PHHapticPointers* GetHapticPointers();
	PHSolidsForHaptic* GetHapticSolids();
	PHSolidPairsForHaptic* GetSolidPairsForHaptic();
};

//----------------------------------------------------------------------------
// PHHapticEngineMultiBase
class PHHapticEngineMultiBase : public PHHapticEngineImp{
	SPR_OBJECTDEF_NOIF(PHHapticEngineMultiBase);
protected:
	volatile int	hapticCount;
	volatile bool	bSync;
	bool			bCalcPhys;

public:
	PHHapticLoopImp* hapticLoop;
	PHHapticEngineMultiBase();
	virtual void StepHapticLoop();	// hapticloop
	virtual void Step1(){}
	virtual void Step2(){}
	virtual void SyncThreads();
	virtual void SyncHapticPointers(); // haptic�X���b�h����hapticpointer�̏�Ԃ�physics�֓���
	virtual void SyncArrays();
	virtual void SyncHaptic2Physic(){}	// haptic�X���b�h����physics�X���b�h�ւ̓���
	virtual void SyncPhysic2Haptic(){}	// physics�X���b�h����haptic�X���b�h�ւ̓���

	//�f�o�b�N�p�R�[�h
	virtual void StepSimulation();
};

}

#endif