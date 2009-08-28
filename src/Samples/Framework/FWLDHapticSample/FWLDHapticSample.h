#ifndef FWLDHAPTIC_SAMPLE_H
#define FWLDHAPTIC_SAMPLE_H

#include <Springhead.h>
#include <Framework/SprFWApp.h>

using namespace Spr;

class FWLDHapticSample : public FWApp{
public:
	FWLDHapticSample();									
	void Init(int argc, char* argv[]);			///< �A�v���P�[�V�����̏�����
	void InitCameraView();						///< �J�����s��̏�����
	void InitHumanInterface();					///< HapticInterface�̏�����
	void Start();								///< �A�v���P�[�V�����J�n
	void IdleFunc();							
	static void CallBackHapticLoop(void* arg);	///< hapticloop�̃R�[���o�b�N
	void Display();								///< �`��֐�	
	void BuildScene();							///< ���̂̍쐬									
	void Reset();								///< ���Z�b�g(������)
	void Keyboard(int key, int x, int y);		///< �L�[�{�[�h�֐�

	/// �I�v�V����
	bool bDrawInfo;								///< �f�o�b�N�\�����邩�ǂ���									
	void DisplayContactPlane();					///< �ڐG�ʂ̕`��
	void DisplayLineToNearestPoint();			///< �|�C���^�̋ߖT�_���獄�̂̋ߖT�_�ւ̐�
}; 

#endif