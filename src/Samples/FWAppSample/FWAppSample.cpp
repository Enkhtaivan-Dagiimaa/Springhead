#include "FWAppSample.h"
#include <iostream>
#include <sstream>
#include <GL/glut.h>

#define ESC 27

FWAppSample::FWAppSample(){}

void FWAppSample::Init(int argc, char* argv[]){
	FWAppGLUT::Init(argc, argv);

	GetSdk()->Clear();															// SDKの作成
	GetSdk()->CreateScene(PHSceneDesc(), GRSceneDesc());		// Sceneの作成
	PHSceneIf* phscene = GetSdk()->GetScene()->GetPHScene();

	BuildObject();																// 剛体を作成

	FWWinDesc windowDesc;
	windowDesc.title = "FWAppFWAppSample";
	CreateWin(windowDesc);
	GetCurrentWin()->scene = GetSdk()->GetScene();

	InitCameraView();
}

void FWAppSample::InitCameraView(){
	std::istringstream issView(
		"((0.9996 0.0107463 -0.0261432 -0.389004)"
		"(-6.55577e-010 0.924909 0.380188 5.65711)"
		"(0.0282657 -0.380037 0.92454 13.7569)"
		"(     0      0      0      1))"
	);
	issView >> cameraInfo.view;
}

void FWAppSample::BuildObject(){
	PHSceneIf* phscene = GetSdk()->GetScene()->GetPHScene();
	PHSolidDesc desc;
	CDBoxDesc bd;

	// 床(物理法則に従わない，運動が変化しない)
	{
		// 剛体(soFloor)の作成
		desc.mass = 1e20f;
		desc.inertia *= 1e30f;
		PHSolidIf* soFloor = phscene->CreateSolid(desc);		// 剛体をdescに基づいて作成
		soFloor->SetDynamical(false);
		soFloor->SetGravity(false);
		// 形状(shapeFloor)の作成
		bd.boxsize = Vec3f(50, 10, 50);
		CDShapeIf* shapeFloor = GetSdk()->GetPHSdk()->CreateShape(bd);
		// 剛体に形状を付加する
		soFloor->AddShape(shapeFloor);
		soFloor->SetFramePosition(Vec3d(0, -5, 0));
	}

	// 箱(物理法則に従う，運動が変化)
	{
		// 剛体(soBox)の作成
		desc.mass = 0.05;
		desc.inertia *= 0.033;
		PHSolidIf* soBox = phscene->CreateSolid(desc);
		// 形状(shapeBox)の作成
		bd.boxsize = Vec3f(2,2,2);
		CDShapeIf* shapeBox = GetSdk()->GetPHSdk()->CreateShape(bd);
		// 剛体に形状を付加
		soBox->AddShape(shapeBox);
		soBox->SetFramePosition(Vec3d(0, 10, 0));
	}
}

void FWAppSample::Step(){
	PHSceneIf* phscene = GetSdk()->GetScene()->GetPHScene();
	phscene->Step();
	glutPostRedisplay();
}

void FWAppSample::Display(){
	// 描画モードの設定
	GetSdk()->SetDebugMode(true);
	GRDebugRenderIf* render = GetCurrentWin()->render->Cast();
	render->SetRenderMode(true, false);
	//render->EnableRenderAxis(bDebug);
	//render->EnableRenderForce(bDebug);
	render->EnableRenderContact(false);

	// カメラ座標の指定
	GRCameraIf* cam = GetCurrentWin()->scene->GetGRScene()->GetCamera();
	if (cam && cam->GetFrame()){
		cam->GetFrame()->SetTransform(cameraInfo.view);
	}else{
		GetCurrentWin()->render->SetViewMatrix(cameraInfo.view.inv());
	}

	// 描画の実行
	if(!GetCurrentWin()) return;
	GetSdk()->SwitchScene(GetCurrentWin()->GetScene());
	GetSdk()->SwitchRender(GetCurrentWin()->GetRender());
	GetSdk()->Draw();
	glutSwapBuffers();
}

void FWAppSample::Keyboard(int key, int x, int y){
	switch (key) {
		case ESC:
		case  'q':
			exit(0);
			break;
		default:
			break;
	}
}