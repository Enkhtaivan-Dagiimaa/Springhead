/*
 *  Copyright (c) 2003-2011, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PH_HAPTIC_STEP_BASE_H
#define PH_HAPTIC_STEP_BASE_H
#include <Physics/PHHapticEngine.h>
#include <Physics/PHHapticRender.h>

namespace Spr{;

class PHHapticEngine;
//----------------------------------------------------------------------------
// PHHapticStepBase
class PHHapticStepBase : public SceneObject{
public:
	SPR_OBJECTDEF_ABST_NOIF(PHHapticStepBase);
	PHHapticEngine* engine;
	PHHapticStepBase(){}
	///	�����V�~�����[�V������dt
	double GetPhysicsTimeStep();
	///	�͊o�����_�����O��dt
	double GetHapticTimeStep();
	///	
	virtual void Step1(){};
	///	
	virtual void Step2(){};
	virtual void StepHapticLoop() = 0;
	virtual void StepHapticSync() = 0;
	virtual void UpdateHapticPointer() = 0;

	int NHapticPointers();
	int NHapticSolids();
	PHHapticPointer*       GetHapticPointer(int i);
	PHSolidForHaptic*      GetHapticSolid(int i);

	virtual int NPointersInHaptic()=0;
	virtual int NSolidsInHaptic()=0;
	virtual PHHapticPointer* GetPointerInHaptic(int i)=0;
	virtual PHSolidForHaptic* GetSolidInHaptic(int i)=0;

	virtual PHSolidPairForHaptic* GetSolidPairInHaptic(int i, int j)=0;
	virtual void ReleaseState(PHSceneIf* scene) {}

	///< ���̂Ɨ͊o�|�C���^�̃y�A���擾����ii:���́Aj:�͊o�|�C���^�j
	// i�ɂ͗͊o�|�C���^���܂܂��B
	PHHapticRender*        GetHapticRender();

	///< �f�o�b�N�p�V�~�����[�V�������s
	virtual void StepPhysicsSimulation();
	/// �V�~�����[�V���������s���钼�O�Ɏ��s�����R�[���o�b�N��o�^����
	virtual bool SetCallbackBeforeStep(PHHapticEngineIf::Callback f, void* arg);
	/// �V�~�����[�V���������s��������Ɏ��s�����R�[���o�b�N��o�^����
	virtual bool SetCallbackAfterStep(PHHapticEngineIf::Callback f, void* arg);
	///	�����X�e�b�v�̒��̉��x�ڂ�HapticStep����Ԃ�
	virtual int GetLoopCount() = 0;
	///	���ԕ\�����Ԃ���ꍇ true
	virtual bool IsInterporate() = 0;
};

}
#endif
