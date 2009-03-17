/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

#ifndef FWHAPTIC_APP_BASE_H
#define FWHAPTIC_APP_BASE_H

#include <Springhead.h>
#include <Framework/SprFWAppGLUT.h>
#include <Framework/FWExpandedPHSolid.h>
#include <vector>

namespace Spr{;

class FWHapticAppBase : public FWAppGLUT, public UTRefCount{
protected:
	// �ϐ�
	int nIter;																	///< iteration��
	double hapticTimeStep;												///< �V�~�����[�V�������[�v����
	double physicTimeStep;												///< �n�v�e�B�b�N���[�v����
	PHSolid hapticInterface;												///< hapticInterface
	PHSolidIf* hapticPointer;												///< hatpticPointer
	FWExpandedPHSolids expandedPHSolids;						///< ExpandedPHSolid���i�[vector
	double localRange;
	// �t���O
	bool bStep;																///< �����V�~�����[�V�������邩�ǂ���
	bool bOneStep;															///< �V�~�����[�V�����̃X�e�b�v���s
	bool bDebug;																///< �f�o�b�N���[�h�ؑ�
public:
	// 
	FWHapticAppBase();
	virtual void Init(int argc, char* argv[]);							///< �������C�͂��߂ɂ�����Ă�
	virtual void InitCameraView();										///< �����J�����ʒu
	virtual void ResetScene() = 0;										///< �V�[�������Z�b�g
	virtual void BuildScene() = 0;										///< �V�[�����r���h
	virtual void CallBack() = 0;											///< MultiMediaTimer���ĂԃR�[���o�b�N�֐�
	virtual void Start();														///< �A�v���P�[�V�����J�n
	virtual void Idle() = 0;													///< glutIdelFunc���ĂԊ֐�
	virtual void Step() = 0;												///< �V�~�����[�V�����X�e�b�v
	virtual void Display();													///< �`��֐�(glutPostRedisplay()�ŌĂ΂��
	virtual void DebugDisplay(GRDebugRenderIf* render);		///< �f�o�b�N�\���ɂ����Ƃ��ɌĂ΂��֐�
	virtual FWExpandedPHSolid** ExpandPHSolidInfo();			///< �V�[��������PHSolid�ɗ͊o�񎦂ɕK�v�ȏ���t������
	virtual void FindNearestObjectFromHapticPointer(PHSolidIf* hPointer);			///< �͊o�|�C���^�ߖT�̕��̂�������
	virtual void UpdateHapticPointer(PHSolidIf* hPointer, PHSolid hInterface);		///< �͊o�C���^�t�F�[�X�̏�Ԃ�͊o�|�C���^�ɐݒ�
	virtual void SyncHapticProcess() = 0;								///< HapticProcess�Ɠ�������֐�
	virtual void Keyboard(int key, int x, int y) = 0;					///< glutKeyboarcFunc()���ĂԊ֐�

	// protected�ϐ��ւ̃A�N�Z�X
	void SetNIter();
	void SetHapticTimeSetp();
	double GetHapticTimeStep();
	void SetPhysicTimeSetp();
	double GetPhysicTimeStep();
	PHSolid GetHapticInterface();
	void SetHapticPointer(PHSolidIf* hpointer);
	PHSolidIf* GetHapticPointer();										///< HapticPointer��Ԃ�
	FWExpandedPHSolid** GetFWExpandedPHSolids();			///< ExpandedPHSolids��Ԃ�
	int GetNExpandedPHSolids();											///< ExpandedPHSolids�̐���Ԃ�

	// �t���O��؂�ւ��邽�߂̊֐�
	void SetDebugMode(bool bD);										///< �f�o�b�N�\�����[�h�ؑ�
	bool GetDebugMode();													///< �f�o�b�N��Ԃ�Ԃ�
};

}
#endif