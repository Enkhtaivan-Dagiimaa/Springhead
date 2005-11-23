#include <Springhead.h>		//	Springheadのインタフェース
#pragma hdrstop
using namespace Spr;


PHSdkIf* sdk;
PHSceneIf* scene;
PHSolidIf* solid1, *solid2;

int main(int argc, char* argv[]){
	sdk = CreatePHSdk();				//	SDKの作成
	scene = sdk->CreateScene();			//	シーンの作成
	PHSolidDesc desc;
	desc.mass = 2.0;
	desc.inertia *= 2.0;
	solid1 = scene->CreateSolid(desc);	//	剛体をdescに基づいて作成
	desc.center = Vec3f(1,0,0);
	solid2 = scene->CreateSolid(desc);	//	剛体をdescに基づいて作成
	
	//	形状の作成
	CDConvexMeshDesc md;
	md.vertices.push_back(Vec3f(-1,-1,-1));
	md.vertices.push_back(Vec3f(-1,-1, 1));
	md.vertices.push_back(Vec3f(-1, 1,-1));
	md.vertices.push_back(Vec3f(-1, 1, 1));
	md.vertices.push_back(Vec3f( 1,-1,-1));
	md.vertices.push_back(Vec3f( 1,-1, 1));
	md.vertices.push_back(Vec3f( 1, 1,-1));
	md.vertices.push_back(Vec3f( 1, 1, 1));
	CDShapeIf* mesh1 = scene->CreateShape(md);
	CDShapeIf* mesh2 = scene->CreateShape(md);
	solid1->AddShape(mesh1);
	solid2->AddShape(mesh2);

	for(int i=0; i<10; ++i){
		solid1->AddForce(Vec3f(10,0,0), Vec3f(0,2,0));
		solid2->AddForce(Vec3f(10,0,0), Vec3f(0,2,0));
		scene->Step();
		std::cout << solid1->GetFramePosition();
		std::cout << solid2->GetFramePosition() << std::endl;
//		std::cout << solid1->GetOrientation() << std::endl;
	}
	//	SDKは開放しなくても良い．しなくてもmainを抜けてから開放される．
	delete sdk;

	//	メモリリークのテスト．
	//	デバッグありで実行するとメモリリークがVCのデバッグ出力に表示される．
	char* memoryLeak = new char[123];
}
