#ifndef MYAPP_H
#define MYAPP_H

#include <Framework/SprFWAppGLUT.h>
#include <Springhead.h>
#include <cstdlib>

using namespace Spr;

class MYApp : public FWAppGLUT, public UTRefCount{
	
private:
	double					dt;					//< �ϕ���
	double					nIter;				//< LCP������

	FWWin*					window[2];			//< ������E�B���h�E[0]:���ʁC[1]:�����
	FWSceneIf*				fwScene[2];			//< Physics�̃V�[���O���t[0]:���ʁC[1]:�����
	const char*				winName[2];			//< Window�̖��O[0]:���ʁC[1]:�����

	float					camAngle[2];		//< �J�����̊p�x[0]:���ʁC[1]:�����
	float					camZoom[2];			//< �J�����̔{��[0]:���ʁC[1]:�����
	Affinef					view[2];			//< �J�����ʒu�̉�]�E�Y�[�����s�����̍s��p�̕ϐ�[0]:���ʁC[1]:�����


public:
	
	// �R���X�g���N�^
	MYApp();

	// ���̃N���X���ŐV�����錾����֐�
	void MyRenderInit(FWWin* window, int winNumber);

	// �C���^�t�F�[�X�֐�
	double	GetTimeStep()					{return dt;}
	void	SetTimeStep(double data)		{dt = data;}
	double	GetNumIteration()				{return nIter;}
	void	SetNumIteration(double data)	{nIter = data;}
	
	// ���̃N���X�Ő錾���ꂽ�֐��̂����I�[�o�[���[�h����֐�
	void Init(int argc, char* argv[]);
	void Keyboard(int key, int x, int y);
	//void Display();
	
};

#endif