#ifndef FWLDHAPTIC_SAMPLE_H
#define FWLDHAPTIC_SAMPLE_H

#include <Springhead.h>
#include <Framework/SprFWAppHaptic.h>

using namespace Spr;

class FWLDHapticSample : public FWAppHaptic{
public:
	FWLDHapticSample();
	virtual void Init(int argc, char* argv[]);			///< �A�v���P�[�V�����̏�����
	virtual void InitCameraView();						///< �J�����s��̏�����
	virtual void InitHumanInterface();					///< HapticInterface�̏�����
	virtual void IdleFunc();							
	virtual void BuildScene(){};							///< ���̂̍쐬	
	virtual void BuildPointer();							///< ���̂̍쐬	
	virtual void Display();								///< �`��֐�	
	void Keyboard(int key, int x, int y);		///< �L�[�{�[�h�֐�

	PH3ElementBallJointIf* Balljoint;
}; 

#endif