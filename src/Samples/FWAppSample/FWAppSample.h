#ifndef FWAPPSAMPLE_H								// �w�b�_�t�@�C������鎞�̂��܂��Ȃ�
#define FWAPPSAMPLE_H								// ����

#include <Springhead.h>								// Springhead��If�^���g�����߂̃w�b�_�t�@�C��
#include <Framework/SprFWAppGLUT.h>					// Framework�N���X���錾����Ă���w�b�_�t�@�C��

using namespace Spr;								// Springhead�̖��O���

class FWAppSample : public FWAppGLUT, public UTRefCount{
public:
	FWAppSample();									// �R���X�g���N�^�i���̃N���X�̃C���X�^���X���������ꂽ���ɌĂ΂��֐��j
	void Init(int argc, char* argv[]);				// GLUT�̏�����
	void InitCameraView();							// �J�������W				
	void BuildObject();								// ���̂��쐬
	void Step();									// �V�~�����[�V������1�X�e�b�v�i�߂�
	void Display();									// �`��
	void Keyboard(int key, int x, int y);			// �L�[�{�[�h�֐�
	bool bDrawInfo;									// �ڐG�͂Ȃǂ�\�����邩
}; 

#endif