#include "FWSprFileLoader.h"
#include <iostream>
#include <sstream>


#define ESC 27

FWSprfileLoader::FWSprfileLoader(){
	fileName = "./SprFiles/sceneSample.spr";
	bDebug		= false;
}

void FWSprfileLoader::Init(int argc, char* argv[]){
	/// シミュレーションの初期化
	CreateSdk();
	GetSdk()->Clear();						// SDKの初期化
	UTRef<ImportIf> import = GetSdk()->GetFISdk()->CreateImport();
	GetSdk()->LoadScene(fileName, import);			// ファイルのロード

	///	ファイルのセーブテスト
	GetSdk()->SaveScene("save.spr", import);

	/// 描画モードの設定
	SetGRAdaptee(TypeGLUT);					// GLUTで描画
	GRInit(argc, argv);		// 初期化

	/// ウィンドウの作成
	FWWinDesc windowDesc;					// GLのウィンドウディスクリプタ
	windowDesc.title = "FWSprFileLoader";	// ウィンドウのタイトル
	CreateWin(windowDesc);					// ウィンドウの作成
	GetCurrentWin()->SetScene(GetSdk()->GetScene());

	/// カメラビューの初期化
	InitCameraView();	
	CreateTimer();
}

void FWSprfileLoader::InitCameraView(){
	///　カメラビューの初期化
	std::istringstream issView(
		"((0.999816 -0.0126615 0.0144361 -0.499499)"
		"(6.50256e-010 0.751806 0.659384 13.2441)"
		"(-0.019202 -0.659263 0.751667 10.0918)"
		"(     0      0      0      1))"
		);
//	issView >> cameraInfo.view;
}

void FWSprfileLoader::Reset(){
	GetSdk()->Clear();		
	GetSdk()->LoadScene("./xFiles/sceneSample.x");
	GetCurrentWin()->SetScene(GetSdk()->GetScene());
}

void FWSprfileLoader::TimerFunc(int id){
	Step();
}

void FWSprfileLoader::IdleFunc(){
}
void FWSprfileLoader::Step(){
	GetSdk()->Step();
	PostRedisplay();
}

void FWSprfileLoader::Display(){
	/// 描画モードの設定
	GetSdk()->SetDebugMode(bDebug);
	GRDebugRenderIf* render = GetCurrentWin()->render->Cast();
	render->SetRenderMode(true, false);
//	render->EnableRenderAxis(bDebug);
	render->EnableRenderForce(bDebug);
	render->EnableRenderContact(bDebug);

	/// カメラ座標の指定
	FWWin* win = GetCurrentWin();
	if (win->scene && win->scene->GetGRScene()){
		GRCameraIf* cam = win->scene->GetGRScene()->GetCamera();
		if (cam && cam->GetFrame()){
			cam->GetFrame()->SetTransform(cameraInfo.view);
		}else{
			GetCurrentWin()->render->SetViewMatrix(cameraInfo.view.inv());
		}
	}

	/// 描画の実行
	if(!GetCurrentWin()) return;
	GetSdk()->SwitchScene(GetCurrentWin()->GetScene());
	GetSdk()->SwitchRender(GetCurrentWin()->GetRender());
	GetSdk()->Draw();
	glutSwapBuffers();
}


void FWSprfileLoader::Keyboard(int key, int x, int y){
	switch (key) {
		case ESC:
		case 'q':
			exit(0);
			break;
		case 'r':
			Reset();			// ファイルの再読み込み
			break;
		case 'w':				// カメラ初期化
			InitCameraView();	
			break;
		case 'd':				// デバック表示
			bDebug = !bDebug;
			break;
		default:
			break;
	}
}