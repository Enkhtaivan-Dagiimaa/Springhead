/** \page pageFWAppGLUT Framework���g����GLUT�A�v���P�[�V����

\contents

<div>
	<p>
		FWAppGLUT�͊ȒP��Springhead2��p�����A�v���P�[�V�������쐬���鎖���\�ɂ����N���X�ł��D
		�ʓ|�ȏ�����Springhead��Framework���S�ď������Ă���邽�߁C
		�����ōs�������ݒ�����Ȃ����邱�Ƃ��ł��܂��D
		�Ȃ��C���̃N���X�̓I�u�W�F�N�g���������鎖��������Ă���܂���D
		���̂��߁C�����̐݌v�̓V���O���g���p�^�[���ƂȂ��Ă��܂��D
		FWAppGLUT��p���ăA�v���P�[�V�������쐬����ɂ́C
		�����̍�肽���A�v���P�[�V�����N���X��FWAppGLUT���p��������K�v������܂��D
		�ȉ��̗�ł͎����̃A�v���P�[�V�����N���X��class MYApp�ŕ\�����ƂƂ��܂��D
	</p>
	<h2>�g����</h2>
	<p>
in MYApp.h
\verbatim

#include <Framework/SprFWAppGLUT.h>

class MYApp : public FWAppGLUT{

private:
	MYApp();
	~MYApp();
	
	// �K�v�ɉ����Ĉȉ��̉��z�֐����I�[�o�[���C�h����D
	// �����́CSprFWApp.h�ŉ��z�֐��Ƃ��Ē�`����Ă���D
	void Display();
	void Reshape(int w, int h);
	void Keyboard(int key, int x, int y);
	void MouseButton(int button, int state, int x, int y);
	void MouseMove(int x, int y);
	void Step();
	void Idle();
	void Joystick(unsinged int buttonMask, int x, int y, int z);

public:
	// �C���X�^���X�����֐�
	static MYApp* GetInstance(){
		// instance is a static variable declared in class FWAppGLUT.
		if(!instance) instance = new MYApp();
		return (MYApp*)instance;
	}

	void Init(int argc, char* argv[]);
	void Start();
public:
		// �����ō���������o�֐���ǉ�
private:
		// �����ō���������o�֐��⃁���o�ϐ���ǉ�
};
\endverbatim
	</p>
	<p>
in MYApp.cpp
\verbatim

#include "MYApp.h"

void MYApp::Display()
{
	//�`�揈���C�I�[�o�[���C�h���Ȃ���΃V�[���O���t���`�悳���
}
void MYApp::Reshape(int w, int h){
	//��ʃT�C�Y���ύX���ꂽ���̏����C�I�[�o�[���C�h���Ȃ���΋K���Reshape�֐����Ă΂��
}
void MYApp::Keyboard(int key, int x, int y){
	//�L�[�{�[�h�̃L�[�������ꂽ���̏����C�I�[�o�[���C�h���Ȃ���Ή��������͂���Ȃ��D
}
void MYApp::MouseButton(int button, int state, int x, int y){
	//�}�E�X�������ꂽ�Ƃ��̏����C�I�[�o�[���C�h���Ȃ���΋K���MouseButton�֐����Ă΂��D
}
void MYApp::MouseMove(int x, int y){
	//�}�E�X�𓮂������Ƃ��̏����C�I�[�o�[���C�h���Ȃ���΋K���MouseMove�֐����Ă΂��
}
void MYApp::Step(){
	//�V�~�����[�V�����̃X�e�b�v�����C�I�[�o�[���C�h���Ȃ���Έ��������ƂɋK���Step�֐����Ă΂�C�V�~�����[�V�������Ԃ��i��
	//GLUT�ł����Ƃ����glutTimerFunc�֓o�^����֐��Ɠ������
}
void MYApp::Idle(){
	//�V�~�����[�V�����̃A�C�h�������C�I�[�o�[���C�h���Ȃ���Ή����Ă΂�Ȃ�
}
void MYApp::Joystick(unsinged int buttonMask, int x, int y, int z){
	//�Q�[���R���g���[���̓��͂������C�I�[�o�[���C�h���Ȃ���Ή����Ă΂�Ȃ�
}
\endverbatim
	</p>

	<p>
in main.cpp
\verbatim

#include <Springhead.h>
#include "MYApp.h"

 
// Global��Ԃŕϐ����`���Ȃ��ƁC�f�X�g���N�^���Ă΂�Ȃ��D
// �Ȃ��Ȃ�Cglut���甲���鎞��exit�֐����g�p����ق��������߁D
// �imain�֐�������ɍŌ�܂ő���Ȃ��D�j
// �܂��R���X�g���N�^��public�ɂ��Ď��̂𐶐�����ƁC
// ������MYApp�^�̕ϐ�����`�o���Ă��܂��\��������댯�D
MYApp* app = MYApp::GetInstance();

int main(int argc, char* argv[]){
	
	app->Init(argc, argv);
	app->Start();

	return 0; // �����܂ŗ��Ȃ�����C���[�J���錾���ƃf�X�g���N�^���Ă΂�Ȃ��D
}
\endverbatim
</div>

*/