// FWAppGLtest.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include <Springhead.h>
#include <SprFramework.h>
#include <Framework/SprFWAppGLUT.h>
#include <HumanInterface/HIBase.h>
#include <gl/glut.h>

using namespace Spr;

class MyApp;
MyApp* app;

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
			SetCurrentWin(GetWin(0));
			glutPostRedisplay();
			if (GetWin(1)){
				SetCurrentWin(GetWin(1));
				glutPostRedisplay();
			}
		}
	}
	void Keyboard(unsigned char key, int x, int y){
		if (key==0x1b){
			delete app;
			exit(0);
		}
		if (key=='s'){
			if (!bRun) FWAppGLUT::Step();
			bRun = false;
		}
		if (key=='r'){
			bRun = true;
		}
	}
	void Display(){
#ifdef _DEBUG
		GetSdk()->SetDebugMode(true);
		FWWin* wr = GetCurrentWin();
		GRDebugRenderIf* r = wr->render->Cast();
		r->SetRenderMode(false, true);
//		r->EnableRenderAxis();
		r->EnableRenderForce();
#endif
		FWAppGLUT::Display();
	}
};


int SPR_CDECL main(int argc, char* argv[]){
	app = new MyApp;
	app->Init(argc, argv);
	app->GetSdk()->Clear();
#if 0	//	�V�[���̍\�z��C++����ł��ꍇ
	app->GetSdk()->CreateScene(PHSceneDesc(), GRSceneDesc());
	PHSceneIf* phscene = app->GetSdk()->GetScene()->GetPHScene();
	PHSolidIf* floor = phscene->CreateSolid(PHSolidDesc());
	floor->SetDynamical(false);
	CDBoxDesc boxdesc;
	boxdesc.boxsize = Vec3d(0.1, 0.1, 0.1);
	floor->AddShape(app->GetSdk()->GetPHSdk()->CreateShape(boxdesc));
#else	//	�V�[�����t�@�C�����烍�[�h����΂����B
	app->GetSdk()->LoadScene("scene.x");
#endif
	//	�E�B���h�E1�ɃV�[����\������悤�ݒ�
	FWAppGLUTDesc wd;
	wd.left = 0; wd.top = 0; wd.width = 500; wd.title = "original scene";
	FWWin* w1 = app->CreateWin(wd);
	w1->scene = app->GetSdk()->GetScene(0);

	//	�V�[���̃Z�[�u
	//app->GetSdk()->SaveScene("save.x");
	//	�Z�[�u�����V�[���̃��[�h
/*	app->GetSdk()->LoadScene("save.x");
	
	//	���[�h�����V�[�����E�B���h�E�Q�ɕ\������悤�ɐݒ�
	wd.left = 512; wd.top = 0; wd.width = 500; wd.title = "saved scene";
	FWWin* w2 = app->CreateWin(wd);
	w2->scene = app->GetSdk()->GetScene(1);
*/
	app->Start();
	return 0;
}
