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
	std::vector<PHSolidIf*> hapticPointers;
//	std::vector<ExpandedPHSolid> eSolids;
	std::vector<ExpandedPHSolid> expandedPHSolids;
	// �t���O
	bool bDebug;
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
	virtual void SyncHapticProcess() = 0;								///< HapticProcess�Ɠ�������֐�
	virtual void ExpandPHSolidInfo() = 0;							///< �V�[��������PHSolid�ɗ͊o�񎦂ɕK�v�ȏ���t������
	virtual void FindNearestObjectFromHapticPointer() = 0;	///< �͊o�|�C���^�ߖT�̕��̂�������
//	virtual void TestSimulation();											///< �e�X�g�V�~�����[�V����
	virtual void Keyboard(int key, int x, int y) = 0;					///< glutKeyboarcFunc()���ĂԊ֐�

	// protected�ϐ��ւ̃A�N�Z�X
	void AddHapticPointer(PHSolidIf* ps);
	std::vector<PHSolidIf*> GetHapticPointers();
	void AddExpandedPHSolids(ExpandedPHSolid es);
	std::vector<ExpandedPHSolid>* GetExpandedPHSolids();

	// �t���O��؂�ւ��邽�߂̊֐�
	void SetDebugMode(bool bD);
	bool GetDebugMode();
};

}
#endif