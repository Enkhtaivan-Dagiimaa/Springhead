#ifndef FWAPPSAMPLE_H								// �w�b�_�t�@�C������鎞�̂��܂��Ȃ�
#define FWAPPSAMPLE_H								// ����

#include <Springhead.h>								// Springhead��If�^���g�����߂̃w�b�_�t�@�C��
#include <Framework/SprFWApp.h>					// Framework�N���X���錾����Ă���w�b�_�t�@�C��
#include <Framework/FWGLUI.h>					// Framework�N���X���錾����Ă���w�b�_�t�@�C��
#include <GL/glui.h>

using namespace Spr;								// Springhead�̖��O���

class FWGLUISample : public FWApp{
private:
	GLUI*			glui;
	GLUI_Panel*		panel;
	GLUI_Button*	button1;
public:
	FWGLUISample();									// �R���X�g���N�^�i���̃N���X�̃C���X�^���X���������ꂽ���ɌĂ΂��֐��j
	
	virtual void Init(int argc, char* argv[]);		// GLUT�̏�����
	virtual void TimerFunc(int id);
	virtual void Display();							// �`��
	virtual void Keyboard(int key, int x, int y);	// �L�[�{�[�h�֐�
	
	void InitCameraView();							// �J�������W		
	void CreateObjects();							// ���̂��쐬
	void Reset();									// �V�[���̃��Z�b�g
	bool bDrawInfo;									// �ڐG�͂Ȃǂ�\�����邩
	void SPR_CDECL DesignGUI();
	
private:
	static void _cdecl CallButton1(int control);
	void		Button1(int control);
}; 



#endif