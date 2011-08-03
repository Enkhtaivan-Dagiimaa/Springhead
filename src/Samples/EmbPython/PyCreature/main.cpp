/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
\page pageBoxStack �L�[���͂Ŕ��𐶐����Đςݏグ��T���v��
Springhead2/src/Samples/BoxStack

\section secQuitBoxStack �I���
- Esc��'q'�ŋ����I���B
- ' ', 'm', 'd', 'c', 'v', 'z', 'x'�Ŕ��������Ă���
	
\section secFlowBoxStack �����̗���
- �V�~�����[�V�����ɕK�v�ȏ��(���̂̌`��E���ʁE�����e���\���Ȃ�)��ݒ肷��B
  ���̂̌`���OpenGL�Ŏw�肷��̂ł͂Ȃ��ASolid���̂Ŏ�������B
- �ڐG�G���W�����S���G���W���ɐݒ肷��B
- �^����ꂽ�����ɂ�臙t(=0.1)�b��̈ʒu�̕ω���ϕ����AOpenGL�ŃV�~�����[�V��������B  
- ���[�U�̃L�[���͂ɑ΂�Solid�𔭐�������B
*/


#include "../../SampleApp.h"
#include <EmbPython/EmbPython.h>

#include <vector>
#include <Creature/CRBodyGenerator/CRBallHumanBodyGen.h>
#include "windows.h"

#pragma hdrstop
using namespace Spr;
using namespace std;


//�}���`�X���b�h�p
#ifdef _WIN32
//std::max�Ȃǂ��g���̂ɕK�v(windows.h�Ƌ������邩��)
#define NOMINMAX 
#include <windows.h>
#endif

/*int frame=0,timepassed=0,timebase=0;
void fps(){
	frame++; 
	timepassed = glutGet(GLUT_ELAPSED_TIME); 
	if (1000 < timepassed - timebase) {
		std::cout << "   FPS:" << frame*1000.0/(timepassed - timebase);
		timebase = timepassed;		
		frame = 0;
	}
}*/

LRESULT CALLBACK NewWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
WNDPROC OldWndProc;

class MyApp : public SampleApp{
public:
	/// �y�[�WID
	enum {
		MENU_MAIN = MENU_SCENE,
	};
	/// �A�N�V����ID
	enum {
		ID_BOX,
		ID_CAPSULE,
		ID_ROUNDCONE,
		ID_SPHERE,
		ID_ROCK,
		ID_BLOCK,
		ID_TOWER,
		ID_SHAKE,
		ID_TOGGLE_IK,
		ID_DROP_OBJECT,
		ID_REACH_HAND,
	};

	PHSolidIf*				soFloor;
	std::vector<PHSolidIf*> soBox;

	double					floorShakeAmplitude;

	bool	bStep;
	bool	bOneStep;
	bool	bIK;

	CRBallHumanBodyGen*		bodyGen;

	CRSdkIf*				crSdk;
	CRCreatureIf*			crCreature;
	CRBodyIf*				crBody;
	CRBodyControllerIf*		crBodyCtl;

	HISpaceNavigatorIf*		spcNav;
	PHSolidIf*				soPointer;

public:
	MyApp(){
		bStep		= true;
		bOneStep	= false;
		bIK			= false;

		appName = "Creature Sample";
		numScenes   = 1;

		floorShakeAmplitude = 0.0;

		AddAction(MENU_MAIN, ID_BOX, "drop box");
		AddHotKey(MENU_MAIN, ID_BOX, 'b');
		AddAction(MENU_MAIN, ID_CAPSULE, "drop capsule");
		AddHotKey(MENU_MAIN, ID_CAPSULE, 'c');
		AddAction(MENU_MAIN, ID_ROUNDCONE, "drop round cone");
		AddHotKey(MENU_MAIN, ID_ROUNDCONE, 'r');
		AddAction(MENU_MAIN, ID_SPHERE, "drop sphere");
		AddHotKey(MENU_MAIN, ID_SPHERE, 's');
		AddAction(MENU_MAIN, ID_ROCK, "drop rock");
		AddHotKey(MENU_MAIN, ID_ROCK, 'd');
		AddAction(MENU_MAIN, ID_BLOCK, "drop block");
		AddHotKey(MENU_MAIN, ID_BLOCK, 'e');
		AddAction(MENU_MAIN, ID_TOWER, "drop tower");
		AddHotKey(MENU_MAIN, ID_TOWER, 't');
		AddAction(MENU_MAIN, ID_SHAKE, "shake floor");
		AddHotKey(MENU_MAIN, ID_SHAKE, 'f');

		AddAction(MENU_SCENE, ID_TOGGLE_IK, "enable/disable IK");
		AddHotKey(MENU_SCENE, ID_TOGGLE_IK, 'p');
		AddAction(MENU_SCENE, ID_DROP_OBJECT,  "drop object");
		AddHotKey(MENU_SCENE, ID_DROP_OBJECT,  'n');
		AddAction(MENU_SCENE, ID_REACH_HAND,  "reach hand");
		AddHotKey(MENU_SCENE, ID_REACH_HAND,  'm');
	}
	~MyApp(){}

	virtual void Init(int argc, char* argv[]) {
		SampleApp::Init(argc, argv);

		HWND hWnd = FindWindow("GLUT", "Creature Sample");

		HISdkIf* hiSdk = HISdkIf::CreateSdk();

		HISpaceNavigatorDesc descSN;
		descSN.hWnd = &hWnd;
		spcNav = hiSdk->CreateHumanInterface(HISpaceNavigatorIf::GetIfInfoStatic())->Cast();
		spcNav->Init(&descSN);
		// spcNav->SetPose(Posef(Vec3f(0,-1.5,0), Quaternionf()));
		spcNav->SetPose(Posef(Vec3f(0,0,0.5), Quaternionf()));

		// �E�B���h�E�v���V�[�W����u������
		OldWndProc = (WNDPROC)(GetWindowLongPtr(hWnd, GWLP_WNDPROC));
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG)(NewWndProc));
	}

	virtual void BuildScene(){
		// soFloor = CreateFloor();

		PHSdkIf* phSdk = GetSdk()->GetPHSdk();
		crSdk = CRSdkIf::CreateSdk();

		// --- --- --- --- ---
		// �N���[�`���̍쐬
		CRCreatureDesc descCreature;
		crCreature = crSdk->CreateCreature(descCreature);
		crCreature->AddChildObject(phScene);

		// Body�̍쐬
		CRBallHumanBodyGenDesc descBody;
		bodyGen = DBG_NEW CRBallHumanBodyGen(descBody);
		crBody = bodyGen->Generate(crCreature);
		DCAST(CRIKSolidIf, crBody->FindByLabel("waist"))->GetPHSolid()->SetDynamical(false);		

		// Body Controller�̍쐬
		CRBodyControllerDesc descBodyCtl;
		crBodyCtl = crCreature->CreateEngine(descBodyCtl)->Cast();
		crBodyCtl->AddChildObject(crBody);

		// --- --- --- --- ---
		// �|�C���^����
		PHSolidDesc descSolid;
		descSolid.dynamical = false;
		soPointer = phScene->CreateSolid(descSolid);

		CDBoxDesc descBox;
		descBox.boxsize = Vec3f(0.2, 0.2, 0.2);
		soPointer->AddShape(phSdk->CreateShape(descBox));

		// --- --- --- --- ---
		// Scene�̐ݒ�
		/// IK�̐ݒ�
		phScene->GetIKEngine()->SetNumIter(15);
		phScene->GetIKEngine()->Enable(bIK);
		
		// phScene->SetGravity(Vec3d(0.0, -9.8, 0.0));
		phScene->SetGravity(Vec3d(0.0, 0.0, 0.0));
		phScene->SetTimeStep(0.01);
		phScene->SetNumIteration(15);
	}

	// �^�C�}�R�[���o�b�N�֐��D�^�C�}�����ŌĂ΂��
	virtual void OnStep() {
		UTAutoLock critical(EPCriticalSection);

		// SampleApp::OnStep();
		soPointer->SetPose(spcNav->GetPose());
		phScene->ClearForce();
		phScene->GenerateForce();
		crSdk->Step();
		phScene->Integrate();

		// ����h�炷
		if (soFloor){
			double time = phScene->GetCount() * phScene->GetTimeStep();
			double omega = 2.0 * M_PI;
			soFloor->SetFramePosition(Vec3d(floorShakeAmplitude*sin(time*omega),0,0));			
			soFloor->SetVelocity(Vec3d(floorShakeAmplitude*omega*cos(time*omega),0,0));
		}
	}

	// �`��֐��D�`��v���������Ƃ��ɌĂ΂��
	virtual void OnDraw(GRRenderIf* render) {
		UTAutoLock critical(EPCriticalSection);
		GRCameraDesc cd;
		render->SetCamera(cd);
		SampleApp::OnDraw(render);

		std::ostringstream sstr;
		sstr << "NObj = " << phScene->NSolids();
		render->DrawFont(Vec2f(-21, 23), sstr.str());
	}

	virtual void OnAction(int menu, int id){
		if(menu == MENU_MAIN){
			Vec3d v, w(0.0, 0.0, 0.2), p(0.5, 20.0, 0.0);
			Quaterniond q = Quaterniond::Rot(Rad(30.0), 'y');

			if(id == ID_BOX){
				Drop(SHAPE_BOX, GRRenderIf::RED, v, w, p, q);
				message = "box dropped.";
			}
			if(id == ID_CAPSULE){
				Drop(SHAPE_CAPSULE, GRRenderIf::GREEN, v, w, p, q);
				message = "capsule dropped.";
			}
			if(id == ID_ROUNDCONE){
				Drop(SHAPE_ROUNDCONE, GRRenderIf::BLUE, v, w, p, q);
				message = "round cone dropped.";
			}
			if(id == ID_SPHERE){
				Drop(SHAPE_SPHERE, GRRenderIf::YELLOW, v, w, p, q);
				message = "sphere dropped.";
			}
			if(id == ID_ROCK){
				Drop(SHAPE_ROCK, GRRenderIf::ORANGE, v, w, p, q);
				message = "random polyhedron dropped.";
			}
			if(id == ID_BLOCK){
				Drop(SHAPE_BLOCK, GRRenderIf::CYAN, v, w, p, q);
				message = "composite block dropped.";
			}
			if(id == ID_TOWER){
				const double tower_radius = 10;
				const int tower_height = 5;
				const int numbox = 20;
				double theta;
				for(int i = 0; i < tower_height; i++){
					for(int j = 0; j < numbox; j++){
						theta = ((double)j + (i % 2 ? 0.0 : 0.5)) * Rad(360) / (double)numbox;
						Drop(SHAPE_BOX, GRRenderIf::BLUE, Vec3d(), Vec3d(), Vec3d(0.5, 20, 0), Quaterniond::Rot(-theta, 'y'));
					}
				}
				message = "tower built.";
			}
			if(id == ID_SHAKE){
				std::cout << "F: shake floor." << std::endl;
				if(floorShakeAmplitude == 0.0){
					floorShakeAmplitude = 2;
					message = "floor shaken.";
				}
				else{
					floorShakeAmplitude = 0;
					message = "floor stopped.";
				}
			}

			if(id == ID_TOGGLE_IK){
				bIK = !bIK;
				DSTR << (bIK ? "IK ON" : "IK OFF") << endl;
				phScene->GetIKEngine()->Enable(bIK);
			}
			if(id == ID_DROP_OBJECT){
				Drop(SHAPE_SPHERE, 0, Vec3d(), Vec3d(), Vec3d(8,0,0), Quaterniond());
				PHSolidIf* so = phScene->GetSolids()[phScene->NSolids() - 1];
				so->SetMass(5.0);
				so->SetVelocity(Vec3d(-8,0,0));
			}
			if(id == ID_REACH_HAND){
				crBodyCtl->SetTargetPos("right_hand", soPointer->GetPose().Pos());
				crBodyCtl->SetTimeLimit("right_hand", 1.0);
			}
		}
		SampleApp::OnAction(menu, id);
	}

};


/////////////////////////////////////////////////////////////////////////////////
//���[�v�̒��g(������Ă΂��
void EPLoop(void* arg)
{
	//�C���^���N�e�B�u���[�h�ŋN��
	PyRun_InteractiveLoop(stdin,"SpringheadPython Console");
	//while(true)
	//{
	//	PyRun_InteractiveOne(stdin,"stdin py loop");
	//}
}

PyObject* SprPy_PassFWSdkIf(FWSdkIf *ob);


//���[�v���s���O�ɂP�x�����Ă΂��
void EPLoopInit(void* arg)
	{
		MyApp* app = (MyApp*)arg;
		//
		////Python�Ń��W���[���̎g�p�錾
		PyRun_SimpleString("import Utility\n");
		PyRun_SimpleString("from Utility import *\n");

		PyRun_SimpleString("import Foundation\n");
		//PyRun_SimpleString("from Foundation import *\n");

		PyRun_SimpleString("import Collision\n");
		//PyRun_SimpleString("from Collision import *\n");

		PyRun_SimpleString("import Physics\n");
		PyRun_SimpleString("from Physics import *\n");

		PyRun_SimpleString("import Graphics\n");
		//PyRun_SimpleString("from Graphics import *\n");

		PyRun_SimpleString("import Creature\n");
		//PyRun_SimpleString("from Creature import *\n");
		
		PyRun_SimpleString("import Framework\n");
		//PyRun_SimpleString("from Framework import *\n");
		
		PHSdkIf* phSdk = app->GetSdk()->GetPHSdk();
		PHSceneIf* scene = app->GetSdk()->GetScene()->GetPHScene();
		PHSolidIf* so1 = scene->CreateSolid();
		PHSolidIf* so2 = scene->CreateSolid();

		Posed p;
		p.PosY() = 15;
		so1->SetPose( p );
		so2->SetPose( Posed(9,8,7,6,5,4,3) ) ;
		PHSolidDesc desc;
		CDConvexMeshIf* mesh;

		// soBox�p��desc
		desc.mass = 2.0;
		desc.inertia = 2.0 * Matrix3d::Unit();

		//	�`��̍쐬
		{
			CDConvexMeshDesc md;
			md.vertices.push_back(Vec3f(-1,-1,-1));
			md.vertices.push_back(Vec3f(-1,-1, 1));	
			md.vertices.push_back(Vec3f(-1, 1,-1));	
			md.vertices.push_back(Vec3f(-1, 1, 1));
			md.vertices.push_back(Vec3f( 1,-1,-1));	
			md.vertices.push_back(Vec3f( 1,-1, 1));
			md.vertices.push_back(Vec3f( 1, 1,-1));
			md.vertices.push_back(Vec3f( 1, 1, 1));
			mesh = DCAST(CDConvexMeshIf, phSdk->CreateShape(md));
			mesh->SetName("meshConvex");
		}
			
		//////c��pyobject��python�œǂ߂�悤�ɂ���
		PyObject *m = PyImport_AddModule("__main__");
		PyObject *dict = PyModule_GetDict(m);
	
		//Python�̌^�ɕϊ�
//		PyObject* py_fwSdk = (PyObject*)newEPFWSdkIf(app->GetSdk());
//		Py_INCREF(py_fwsdk);
//		PyDict_SetItemString(dict,"fwsdk",py_fwsdk);

		//SWIG_NewPointerObj(SWIG_as_voidptr(result), SWIGTYPE_p_FWSdkIf, 0 |  0 );
		//PyRun_SimpleString("import SprPy\n");

		//PyObject *fwSdk = SprPy_PassFWSdkIf(app->GetSdk());
		//Py_INCREF(fwSdk);
		//PyDict_SetItemString(dict,"sdk",fwSdk);

		PyObject* py_sdk = (PyObject*)newEPPHSdkIfObject(phSdk);

		PyObject* py_scene = (PyObject*)newEPPHSceneIfObject(scene);
		Py_INCREF(py_scene);
		PyDict_SetItemString(dict,"scene",py_scene);

		PyObject* py_desc = (PyObject*)newEPPHSolidDescObject(desc);
		Py_INCREF(py_desc);
		PyDict_SetItemString(dict,"desc",py_desc);

		PyObject* py_meshConvex = (PyObject*)newEPCDConvexMeshIfObject(mesh);
		Py_INCREF(py_meshConvex);
		PyDict_SetItemString(dict,"mesh",py_meshConvex);
		
		PyObject* py_bodyctl = (PyObject*)newEPCRBodyControllerIfObject(app->crBodyCtl);
		Py_INCREF(py_bodyctl);
		PyDict_SetItemString(dict,"crBodyCtl",py_bodyctl);

		////�t�@�C���̓ǂݎ��
		ifstream file("boxstack.py");
		string data("");
		string buff;

		while( file && getline(file,buff) )
			data += buff + string("\n");
		file.close();

		PyRun_SimpleString(data.c_str());

	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////


MyApp app;

LRESULT CALLBACK NewWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	MSG m;
	m.hwnd		= hWnd;
	m.message	= msg;
	m.wParam	= wParam;
	m.lParam	= lParam;
	if (app.spcNav->PreviewMessage(&m)) {
		return 0L;
	}
	return CallWindowProc(OldWndProc, hWnd, msg, wParam, lParam);
}

/**
 brief		���C���֐�
 param		<in/--> argc�@�@�R�}���h���C�����͂̌�
 param		<in/--> argv�@�@�R�}���h���C������
 return		0 (����I��)
 */
int main(int argc, char *argv[]) {
	app.Init(argc, argv);

	EPInterpreter* interpreter = EPInterpreter::Create();
	interpreter->Initialize();
	interpreter->EPLoop = EPLoop;
	interpreter->EPLoopInit = EPLoopInit;
	interpreter->Run(&app);

	app.StartMainLoop();
	return 0;
}

