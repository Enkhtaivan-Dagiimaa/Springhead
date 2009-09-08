#ifndef FWLDHAPTIC_SAMPLE_H
#define FWLDHAPTIC_SAMPLE_H

#include <Springhead.h>
#include <Framework/SprFWAppHaptic.h>
#include <vector>

using namespace Spr;

class FWLDHapticSample : public FWAppHaptic{
public:
	FWLDHapticSample();
	virtual void Init(int argc, char* argv[]);			///< �A�v���P�[�V�����̏�����
	virtual void InitCameraView();						///< �J�����s��̏�����
	virtual void InitHumanInterface();					///< HapticInterface�̏�����
	virtual void IdleFunc();							
	virtual void BuildScene(){};						///< ���̂̍쐬	
	virtual void BuildPointer();						///< ���̂̍쐬	
	virtual void Display();								///< �`��֐�	
	void Keyboard(int key, int x, int y);				///< �L�[�{�[�h�֐�

	void TwoPointerCalib();								///< 2�_�̃|�C���^�̍��W�n����v������
	void TwoPointerSet();								///< 2�_�̃|�C���^�̍��W�n����v������
	std::vector<Vec3d> mainPosition;
	std::vector<Vec3d> subPosition;


	PH3ElementBallJointIf* Balljoint;
}; 

#endif