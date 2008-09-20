// FWAppGLtest.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include <windows.h>
#include <Springhead.h>
#include <SprFramework.h>
#include <Framework/SprFWAppGLUT.h>
#include <gl/glut.h>

using namespace Spr;

class MyApp: public FWAppGLUT{
protected:
	bool bRun;
public:
	MyApp():bRun(true){
	}
	void Step(){
		if (bRun){
			FWAppGLUT::Step();
			//PHSolidIf* s = DCAST(PHSolidIf,  GetSdk()->GetScene()->FindObject("soBlock1"));
			glutPostRedisplay();
		}
	}
	void Keyboard(int key, int x, int y){
		if (key==0x1b){
			exit(0);
		}
		if (key=='p'){
			if (!bRun) Step();
			bRun = false;
		}
		if (key=='r'){
			bRun = true;
		}
		if (key=='w'){
			FWWin* win = GetCurrentWin();
			FWSceneIf* s = win->GetScene();
			if (GetCurrentWin()->GetFullScreen()){
				DestroyWin(GetCurrentWin());
				FWWinDesc wd;
				wd.height = 600;
				wd.width = 800;
				win = CreateWin(wd);
			}else{
				DestroyWin(GetCurrentWin());
				FWWinDesc wd;
				wd.fullscreen = true;
				wd.height = 768;
				wd.width = 1024;
				win = CreateWin(wd);				
			}
			win->SetScene(s);
		}
	}
	void Display(){
#if 1	//	debug mode
		GetSdk()->SetDebugMode(true);
		GRDebugRenderIf* r = GetCurrentWin()->render->Cast();
		r->SetRenderMode(true, true);
		r->EnableRenderAxis();
		r->EnableRenderForce();
		r->EnableRenderContact();
#else
#endif
		GetCurrentWin()->render->SetViewMatrix(cameraInfo.view.inv());
		FWAppGLUT::Display();
	}
};
MyApp app;

int SPR_CDECL main(int argc, char* argv[]){
	app.Init(argc, argv);

	app.GetSdk()->Clear();	//	SDK�S�̂�������

	//	�V�[���̍\�z
	app.GetSdk()->CreateScene(PHSceneDesc(), GRSceneDesc());
	PHSceneIf* phscene = app.GetSdk()->GetScene()->GetPHScene();
	PHSolidIf* floor = phscene->CreateSolid(PHSolidDesc());
	floor->SetDynamical(false);
	CDBoxDesc boxdesc;
	boxdesc.boxsize = Vec3d(0.1, 0.1, 0.1);
	floor->AddShape(app.GetSdk()->GetPHSdk()->CreateShape(boxdesc));
	//	�E�B���h�E1���쐬
	FWAppGLUTDesc wd;
	wd.left = 0; wd.top = 0; wd.width = 500; wd.title = "original scene";
	FWWin* w0 = app.CreateWin(wd);	//	�쐬�Ɠ����ɁC�V�[��0 ���E�B���h�E0 �Ɋ��蓖�Ă���D
	app.Start();
	return 0;
}
