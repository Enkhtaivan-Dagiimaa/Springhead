#ifndef FWVRMLFILE_LOADER_H							// �w�b�_�t�@�C������鎞�̂��܂��Ȃ�
#define FWVRMLFILE_LOADER_H							// ����

#include <Springhead.h>								// Springhead��If�^���g�����߂̃w�b�_�t�@�C��
#include <Framework/SprFWApp.h>						// Framework�N���X���錾����Ă���w�b�_�t�@�C��

using namespace Spr;								// Springhead�̖��O���

class FWVrmlfileLoader : public FWApp{
public:
	std::string fileName;
	bool bDebug;									///< �f�o�b�N���[�h�̐؂�ւ�
	FWVrmlfileLoader();								///< �R���X�g���N�^�i���̃N���X�̃C���X�^���X���������ꂽ���ɌĂ΂��֐��j
	virtual void Init(int argc, char* argv[]);		///< GLUT�̏�����
	virtual void TimerFunc(int id);
	virtual void IdleFunc();
	virtual void Display();							///< �`��
	virtual void Keyboard(int key, int x, int y);	///< �L�[�{�[�h�֐�

	void InitCameraView();							///< �J�������W				
	void Reset();
	void Step();									///< �V�~�����[�V������1�X�e�b�v�i�߂�
}; 

#endif