#ifndef PH_HAPTICRENDER_SINGLEBASE_H
#define PH_HAPTICRENDER_SINGLEBASE_H

#include <Physics/PHHapticEngine.h>

namespace Spr{;

//----------------------------------------------------------------------------
// PHHapticRenderSingleBase
class PHHapticRenderSingleBase : public PHHapticRenderImp{
	SPR_OBJECTDEF_NOIF(PHHapticRenderSingleBase);
public:
	PHHapticRenderSingleBase();
	virtual void Step();		// phengine->phhapticengine����Ă΂��
	virtual void UpdateInterface();
	virtual void UpdateHapticPointer();
	virtual void HapticRendering();

	//�f�o�b�N�p�R�[�h
	virtual void StepSimulation();
};

}

#endif