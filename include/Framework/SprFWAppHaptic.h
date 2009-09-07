#ifndef FWAPPHAPTIC_H
#define FWAPPHAPTIC_H

#include <Springhead.h>
#include <Framework/SprFWApp.h>

using namespace Spr;

class FWAppHaptic : public FWApp{
public:
	FWAppHaptic();									
	virtual void Init(int argc, char* argv[]);			///< �A�v���P�[�V�����̏�����
	virtual void InitCameraView();						///< �J�����s��̏�����
	virtual void InitHumanInterface();					///< HapticInterface�̏�����
	virtual void Start();								///< �A�v���P�[�V�����J�n
	virtual void IdleFunc();							
	virtual void Display();								///< �`��֐�	
	virtual void BuildScene();							///< ���̂̍쐬
	virtual void BuildPointer();							///< ���̂̍쐬
	virtual void Reset(){};								///< ���Z�b�g(������)
	virtual void Keyboard(int key, int x, int y);		///< �L�[�{�[�h�֐�

	static void CallBackHapticLoop(void* arg);			///< hapticloop�̃R�[���o�b�N

	/// �I�v�V����
	bool bStep;									///< �V�~�����[�V������i�߂邩�ǂ���
	bool bOneStep;								///< �X�e�b�v���s
	bool bDrawInfo;								///< �f�o�b�N�\�����邩�ǂ���									
	void DisplayContactPlane();					///< �ڐG�ʂ̕`��
	void DisplayLineToNearestPoint();			///< �|�C���^�̋ߖT�_���獄�̂̋ߖT�_�ւ̐�
}; 

#endif