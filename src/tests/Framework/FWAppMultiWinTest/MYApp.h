#ifndef MYAPP_H
#define MYAPP_H

#include <Framework/SprFWAppGLUT.h>
#include <Springhead.h>
#include <cstdlib>
#include <vector>

using namespace Spr;
using namespace std;

class MYApp : public FWAppGLUT, public UTRefCount{
	
private:
	double					dt;				//< �ϕ���
	double					nIter;			//< LCP������

	int						numWindow;		//< �E�B���h�E�̐�
	vector< FWWin* >		windows;		//< ������E�B���h�E[0]:���ʁC[1]:�����
	vector< FWSceneIf* >	fwScenes;		//< Physics�̃V�[���O���t[0]:���ʁC[1]:�����
	vector< string >		winNames;		//< Window�̖��O[0]:���ʁC[1]:�����

	vector< float >			camAngles;		//< �J�����̊p�x[0]:���ʁC[1]:�����
	vector< float >			camZooms;		//< �J�����̔{��[0]:���ʁC[1]:�����
	vector< Affinef	>		views;			//< �J�����ʒu�̉�]�E�Y�[�����s�����̍s��p�̕ϐ�[0]:���ʁC[1]:�����


public:
	
	// �R���X�g���N�^
	MYApp();

	// ���̃N���X���ŐV�����錾����֐�
	void MyRenderInit(FWWin* window, int num);

	// �C���^�t�F�[�X�֐�
	double	GetTimeStep()					{return dt;}
	void	SetTimeStep(double data)		{dt = data;}
	double	GetNumIteration()				{return nIter;}
	void	SetNumIteration(double data)	{nIter = data;}
	
	// ���̃N���X�Ő錾���ꂽ�֐��̂����I�[�o�[���[�h����֐�
	void Init(int argc, char* argv[]);
	void Keyboard(int key, int x, int y);
	
};

#endif