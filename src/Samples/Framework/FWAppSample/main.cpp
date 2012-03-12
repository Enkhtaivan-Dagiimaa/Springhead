/**
\page pageFWAppSample �t���[�����[�NSDK���g������ԊȒP�ȃT���v��
 Springhead2/src/Samples/FWAppSample

- �v���O�����̋@�\
 - ���̏�ɔ����ڂ��Ă��邾���̃V�[���B
 - �`��� \ref pagePhysics �̃f�o�b�O�\���̂݁B
- �\�[�X�ɂ���
 - �V�[���̍\�z�́AC++�����API���Ăяo���čs���B
 - \ref pageFramework �� \ref pagePhysics �݂̂��g�p
*/

#include <Springhead.h>
#include <Framework/SprFWApp.h>
//#include "Samples\SampleApp.h"
using namespace Spr;

class MyApp : public FWApp{
public:
	virtual void Init(int argc = 0, char* argv[] = 0){
		FWApp::Init(argc, argv);

		PHSdkIf* phSdk = GetSdk()->GetPHSdk();
		PHSceneIf* phscene = GetSdk()->GetScene()->GetPHScene();
		CDBoxDesc bd;
		
		// �����쐬
		PHSolidIf* floor = phscene->CreateSolid();
		floor->SetDynamical(false);
		bd.boxsize = Vec3f(5.0f, 1.0f, 5.0f);
		floor->AddShape(phSdk->CreateShape(bd));
		floor->SetFramePosition(Vec3d(0, -1.0, -1.0));
	
		// �����쐬
		PHSolidIf* box = phscene->CreateSolid();
		bd.boxsize = Vec3f(0.2f, 0.2f, 0.2f);
		box->AddShape(phSdk->CreateShape(bd));
		box->SetFramePosition(Vec3d(-0.1, 0.0, -1.0));
		
		GetSdk()->SetDebugMode(true);
	}
} app;

int _cdecl main(int argc, char* argv[]){
	app.Init(argc, argv);
	app.StartMainLoop();
	return 0;
}
