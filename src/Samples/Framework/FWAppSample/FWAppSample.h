#ifndef FWAPPSAMPLE_H								// �w�b�_�t�@�C������鎞�̂��܂��Ȃ�
#define FWAPPSAMPLE_H								// ����

#include <Springhead.h>								// Springhead��If�^���g�����߂̃w�b�_�t�@�C��
#include <Framework/SprFWApp.h>						// Framework�N���X���錾����Ă���w�b�_�t�@�C��

using namespace Spr;								// Springhead�̖��O���
class FWAppSample;
class FWAppSample : public FWApp{
public:
	FWAppSample();									// �R���X�g���N�^�i���̃N���X�̃C���X�^���X���������ꂽ���ɌĂ΂��֐��j
	virtual void Init(int argc, char* argv[]);		// GLUT�̏�����
	virtual void Timer();
	virtual void IdleFunc();
	void InitCameraView();							// �J�������W		
	void BuildObject();								// ���̂��쐬
	void Step();									// �V�~�����[�V������1�X�e�b�v�i�߂�
	void Display();									// �`��
	void Reset();									// �V�[���̃��Z�b�g
	void Keyboard(int key, int x, int y);			// �L�[�{�[�h�֐�
	bool bDrawInfo;									// �ڐG�͂Ȃǂ�\�����邩
	static void _cdecl TimerFunc(int id);
	void CallStep();
}; 

#endif