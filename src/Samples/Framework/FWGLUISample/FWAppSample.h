#ifndef FWAPPSAMPLE_H								// �w�b�_�t�@�C������鎞�̂��܂��Ȃ�
#define FWAPPSAMPLE_H								// ����

#include <Springhead.h>								// Springhead��If�^���g�����߂̃w�b�_�t�@�C��
#include <Framework/SprFWAppGLUI.h>					// Framework�N���X���錾����Ă���w�b�_�t�@�C��
#include <GL/glui.h>
using namespace Spr;								// Springhead�̖��O���

class FWAppSample : public FWAppGLUI, public UTRefCount{
private:
	GLUI*			glui;
	GLUI_Panel*		panel;
	GLUI_Button*	button1;
public:
	FWAppSample();									// �R���X�g���N�^�i���̃N���X�̃C���X�^���X���������ꂽ���ɌĂ΂��֐��j
	void Init(int argc, char* argv[]);			// GLUT�̏�����
	void InitCameraView();							// �J�������W		
	void BuildObject();								// ���̂��쐬
	void Step();										// �V�~�����[�V������1�X�e�b�v�i�߂�
	void Display();									// �`��
	void Reset();										// �V�[���̃��Z�b�g
	void Keyboard(int key, int x, int y);		// �L�[�{�[�h�֐�
	bool bDrawInfo;									// �ڐG�͂Ȃǂ�\�����邩

	void SPR_CDECL DesignGUI();

private:
	static void CallButton1(int control);
	void		Button1(int control);
}; 

#endif