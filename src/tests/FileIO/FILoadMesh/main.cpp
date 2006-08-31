/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 Springhead2/src/tests/FileIO/FILoadMesh/main.cpp

【概要】
  ファイル入出力SDKで、Xファイルの詳細を解析させる。
  （頂点位置、頂点インデックス、法線、マテリアル、テクスチャ）

【終了基準】
  ・プログラムが正常終了したら0を返す。  

【Note】
  本ファイルの26行目" #define TEST_FILEX " にて、入力ファイル名を指定する。

*/
#include <Springhead.h>
#include <GL/glut.h>
#define	ESC				27				// Esc key
#define EXIT_TIMER	20000				// 強制終了させるステップ数
#define TEST_FILEX	"box.x"		// ロードするXファイル

namespace Spr{
	GRSdkIf* grSdk;
	GRSceneIf* scene;
	GRDeviceGLIf* grDevice;
	GRDebugRenderIf* render;
	void PHRegisterTypeDescs();
	void CDRegisterTypeDescs();
	void GRRegisterTypeDescs();
	void FIRegisterTypeDescs();
}
using namespace Spr;


/**
 brief     	glutDisplayFuncで指定したコールバック関数
 param	 	なし
 return 	なし
 */
void display(){
	//	バッファクリア
	render->ClearBuffer();
	scene->Render(render);
	if (!scene){
		std::cout << "scene == NULL. File may not found." << std::endl;
		exit(-1);
	}
	render->EndScene();
}

/**
 brief		光源の設定
 param	 	なし
 return 	なし
 */
void setLight() {
	GRLightDesc light0;
	light0.ambient	= Vec4f(0.2, 0.2, 0.2, 1.0);
	light0.diffuse	= Vec4f(0.8, 0.8, 0.8, 1.0);
	light0.specular	= Vec4f(1.0, 1.0, 1.0, 1.0);
	light0.position = Vec4f(-10.81733, 100.99039, -21.19059, 0.0);
	light0.attenuation0  = 1.0;
	light0.attenuation1  = 0.0;
	light0.attenuation2  = 0.0;
	light0.spotDirection = Vec3f(0.0, -1.0, 0.0);
	light0.spotFalloff   = 10.0;
	light0.spotInner	 = 20;
	light0.spotCutoff	 = 150.0;
	render->PushLight(light0);

	GRLightDesc light1;
	light1.ambient	= Vec4f(0.2, 0.2, 0.2, 1.0);
	light1.diffuse	= Vec4f(0.8, 0.8, 0.8, 1.0);
	light1.specular	= Vec4f(1.0, 1.0, 1.0, 1.0);
	light1.position = Vec4f(-1.85930, 100.10970, -49.58080, 0.0);
	light1.attenuation0  = 1.0;
	light1.attenuation1  = 0.0;
	light1.attenuation2  = 0.0;
	light1.spotDirection = Vec3f(0.0, -1.0, 0.0);
	light1.spotFalloff   = 0.0;
	light1.spotInner	 = 10;
	light1.spotCutoff	 = 150.0;
	render->PushLight(light1);
}

/**
 brief  	glutReshapeFuncで指定したコールバック関数
 param	 	<in/--> w　　幅
 param  	<in/--> h　　高さ
 return 	なし
 */
void reshape(int w, int h){
	render->Reshape(Vec2f(), Vec2f(w,h));
}

/**
 brief 		glutKeyboardFuncで指定したコールバック関数 
 param		<in/--> key　　 ASCIIコード
 param 		<in/--> x　　　 キーが押された時のマウス座標
 param 		<in/--> y　　　 キーが押された時のマウス座標
 return 	なし
 */
void keyboard(unsigned char key, int x, int y){
	if (key == ESC) {
		exit(0);
	}
}

/**
 brief  	glutIdleFuncで指定したコールバック関数
 param	 	なし
 return 	なし
 */
void idle(){
//	if(scene && *scene) (*(scene))->Step();
	glutPostRedisplay();
	static int count;
	count ++;
	if (count > EXIT_TIMER){
		std::cout << EXIT_TIMER << " count passed." << std::endl;
		exit(0);
	}
}

/**
 brief		メイン関数
 param		<in/--> argc　　コマンドライン入力の個数
 param		<in/--> argv　　コマンドライン入力
 return		0 (正常終了)
 */
int main(int argc, char* argv[]){
	PHSdkIf::RegisterSdk();
	GRSdkIf::RegisterSdk();
	FWSdkIf::RegisterSdk();

	FISdkIf* fiSdk = FISdkIf::CreateSdk();
	FIFileXIf* fileX = fiSdk->CreateFileX();
	ObjectIfs objs;

	grSdk = GRSdkIf::CreateSdk();	
	objs.push_back(grSdk);
	scene = grSdk->CreateScene();
	objs.push_back(scene);
	fileX->Load(objs, TEST_FILEX);

	if (!grSdk) return -1;
	objs.clear();
	objs.Push(grSdk);
	fileX->Save(objs, "out.x");
	
	fiSdk->Clear();	//	ファイルローダのメモリを解放．
	objs.clear();
	grSdk->Print(DSTR);
	//scene = grSdk->GetScene(0);		// Sceneの取得
	
	grSdk->Print(DSTR);

	DSTR << "Loaded : " << "NScene=" << (int)grSdk->NScene() << std::endl;
	scene->Print(DSTR);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	int window = glutCreateWindow("FILoadMesh");

	render = grSdk->CreateDebugRender();
	grDevice = grSdk->CreateDeviceGL(window);
	grDevice->Init();
	render->SetDevice(grDevice);

	// 視点設定
	Affinef view;
	view.Pos() = Vec3f(0.0, 3.0,-80.0);									// eye
		view.LookAtGL(Vec3f(0.0, 0.0, 0.0), Vec3f(0.0, 1.0, 0.0));		// center, up 
	view = view.inv();	
	render->SetViewMatrix(view);

	// 光源の設定
	setLight();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}
