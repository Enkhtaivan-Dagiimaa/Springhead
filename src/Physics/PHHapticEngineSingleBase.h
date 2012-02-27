#ifndef PH_HAPTICENGINE_SINGLEBASE_H
#define PH_HAPTICENGINE_SINGLEBASE_H

#include <Physics/PHHapticEngine.h>
#include <Physics/PHHapticRender.h>

namespace Spr{;

//----------------------------------------------------------------------------
// PHHapticEngineSingleBase
class PHHapticEngineSingleBase : public PHHapticEngineImp{
	SPR_OBJECTDEF_NOIF(PHHapticEngineSingleBase);
public:
	PHHapticEngineSingleBase();
	virtual void Step1();
	virtual void Step2();
	virtual void UpdateInterface();
	virtual void UpdateHapticPointer();

	PHHapticRender* GetHapticRender(){
		return engine->hapticRender;
	}

	//�f�o�b�N�p�R�[�h
	virtual void StepSimulation();
};

}

#endif