/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

/**
	�T���v���v���O�������p�A�v���P�[�V�����N���X
 **/

#include <Springhead.h>
#include <Framework/SprFWApp.h>

using namespace std;
using namespace Spr;

#define ESC		27

class SampleApp : public FWApp{
public:
	/// �A�N�V����ID
	enum ActionID{
		ID_EXIT,
		ID_TOGGLE,
		ID_STEP,
		ID_LOAD_STATE,
		ID_SAVE_STATE,
		ID_RELEASE_STATE,
		ID_READ_STATE,
		ID_WRITE_STATE,
		ID_DUMP,
		ID_SWITCH_LCP_PENALTY,
		ID_LAST,
	};
	/// �A�N�V�������
	struct Action{
		int			id;							///< �A�N�V����ID
		vector< pair<int, UTString> > keys;		///< �L�[�Ƒ�փe�L�X�g
		UTString	brief;						///< �ȒP�Ȑ���
		UTString	detail;						///< �ڂ�������
	};
	map<int, Action>		actions;

	FWSceneIf*				fwScene;
	PHSceneIf*				phScene;
	UTRef<ObjectStatesIf>	states;

	UTTimerIf*				timer;

	bool					running;		///< �V�~�����[�V�������s���t���O
	bool					useLCP;			///< LCP��penalty�@��
	bool					showhelp;		///< �w���v�\��
	UTString				appName;		///< �T���v����

public:
	/// �A�N�V�����̓o�^
	void AddAction(int id, UTString brief, UTString detail = ""){
		Action& act = actions[id];
		act.id		= id;
		act.brief   = brief;
		act.detail  = (detail == "" ? brief : detail);
	}

	/// �A�N�V�����ƃL�[�̑Ή�
	void AddHotKey(int id, int key, UTString alt = ""){
		actions[id].keys.push_back(make_pair(key, alt));
	}

	/// �t�����̕\��
	void DrawInfo(GRRenderIf* render){

	}
	
	SampleApp(){
		running		= true;
		useLCP		= true;
		showhelp	= false;
		appName		= "untitled";

		AddAction(ID_EXIT, "exit", "exit the application");
		AddHotKey(ID_EXIT, ESC);
		AddHotKey(ID_EXIT, 'q');
		AddHotKey(ID_EXIT, 'Q');
		
		AddAction(ID_TOGGLE, "start", "start simulation");
		AddHotKey(ID_TOGGLE, ' ', "space");
	
		AddAction(ID_STEP, "step", "step simulation");
		AddHotKey(ID_STEP, ';');

		AddAction(ID_LOAD_STATE, "load state", "load saved state");
		AddHotKey(ID_LOAD_STATE, 'L');

		AddAction(ID_SAVE_STATE, "save state", "save current state");
		AddHotKey(ID_SAVE_STATE, 'S');

		AddAction(ID_RELEASE_STATE, "release state", "release saved state");
		AddHotKey(ID_RELEASE_STATE, 'X');

		AddAction(ID_READ_STATE, "read state", "read state from state.bin");
		AddHotKey(ID_READ_STATE, 'R');

		AddAction(ID_WRITE_STATE, "write state", "write state to state.bin");
		AddHotKey(ID_WRITE_STATE, 'W');

		AddAction(ID_DUMP, "dump", "dump object data to dump.bin");
		AddHotKey(ID_DUMP, 'D');

		AddAction(ID_SWITCH_LCP_PENALTY, "switch to penalty", "switch to penalty method");
		AddHotKey(ID_SWITCH_LCP_PENALTY, 'M');

	}
	~SampleApp(){}

public: /** �h���N���X����������֐� **/

	/// �V�[���\�z
	virtual void BuildScene(){}

	/// 1�X�e�b�v�̃V�~�����[�V����
	virtual void OnStep(){ fwScene->Step(); }

	/// �`��
	virtual void OnDraw(GRRenderIf* render){
		fwScene->DrawPHScene(render);
	}

	/// �L�[����
	///virtual bool OnKey(int key, int x, int y){ return false; }

	/// �A�N�V��������
	virtual void OnAction(int id){
		if(id == ID_EXIT)
			exit(0);
		if(id == ID_TOGGLE){
			running = !running;
			if(running)
				 AddAction(ID_TOGGLE, "pause", "pause simulation");
			else AddAction(ID_TOGGLE, "start", "start simulation");
		}
		if(id == ID_STEP)
			fwScene->Step();

		if(id == ID_LOAD_STATE)
			states->LoadState(phScene);
		if(id == ID_SAVE_STATE)
			states->SaveState(phScene);
		if(id == ID_RELEASE_STATE)		
			states->ReleaseState(phScene);
		if(id == ID_READ_STATE){
			//std::cout << "L: Load state from state.bin." << std::endl;
			phScene->ReadState("state.bin");
			//phScene->Step();
			//std::ofstream f("dump_after_load.bin", std::ios::binary|std::ios::out);
			//phScene->DumpObjectR(f);
		}
		if(id == ID_WRITE_STATE){
			//std::cout << "S: Save state to state.bin." << std::endl;
			phScene->WriteState("state.bin");
		}
		if(id == ID_DUMP){
			std::ofstream f("dump.bin", std::ios::binary|std::ios::out);
			phScene->DumpObjectR(f);
		}
		
		if(id == ID_SWITCH_LCP_PENALTY){
			//std::cout << "C: Set contact mode to LCP mode." << std::endl;
			//std::cout << "P: Set contact mode to PENALTY mode." << std::endl;
			useLCP = !useLCP;
			if(useLCP){
				phScene->SetContactMode(PHSceneDesc::MODE_LCP);
				AddAction(ID_SWITCH_LCP_PENALTY, "switch to penalty", "switch to penalty method");
			}
			else{
				phScene->SetContactMode(PHSceneDesc::MODE_PENALTY);
				AddAction(ID_SWITCH_LCP_PENALTY, "switch to lcp", "switch to lcp method");
			}
		}


	}

public: /** FWApp�̎��� **/

	virtual void Init(int argc, char* argv[]){
		SetGRAdaptee(TypeGLUT);
		GRInit(argc, argv);
		CreateSdk();

		FWWinDesc windowDesc;
		windowDesc.title = appName;
		CreateWin(windowDesc);
		InitWindow();

		/// �����ݒ�
		GRRenderIf* render = GetSdk()->GetRender();
		GRLightDesc ld;
		ld.diffuse  = Vec4f(1.0f, 1.0f, 1.0f, 1.0f);
		ld.specular = Vec4f(1.0f, 1.0f, 1.0f, 1.0f);
		ld.ambient  = Vec4f(0.0f, 0.0f, 0.0f, 1.0f);
		ld.position = Vec4f(25.0f, 50.0f, 20.0f, 1.0f);
		render->PushLight(ld);

		/// �V�[���̍쐬
		fwScene = GetSdk()->CreateScene();
		/// �`��ݒ�
		//fwScene->SetSolidMaterial(GRRenderIf::WHITE);
		fwScene->SetWireMaterial(GRRenderIf::WHITE);
		fwScene->SetRenderMode(true, true);				///< �\���b�h�`��C���C���t���[���`��
		fwScene->EnableRenderAxis(true, true, true);		///< ���W��
		fwScene->SetAxisStyle(FWSceneIf::AXIS_ARROWS);	///< ���W���̃X�^�C��
		fwScene->EnableRenderForce(false, true);			///< ��
		fwScene->EnableRenderContact(true);				///< �ڐG�f��

		phScene = fwScene->GetPHScene();
		phScene->SetGravity(Vec3f(0.0f, -9.8f, 0.0f));	// �d�͂�ݒ�
		phScene->SetTimeStep(0.05);
		phScene->SetNumIteration(15);
		//phScene->SetNumIteration(10, 1);	// correction iteration
		//phScene->SetNumIteration(10, 2);	// contact iteration
		
		phScene->SetStateMode(true);
		states = ObjectStatesIf::Create();
		//scene->GetConstraintEngine()->SetUseContactSurface(true); //�ʐڐG�ł̗͌v�Z��L����

		// �V�[���\�z
		BuildScene();

		// �^�C�}
		timer = CreateTimer(UTTimerIf::FRAMEWORK);
		timer->SetInterval(10);
	}

	// �^�C�}�R�[���o�b�N�֐��D�^�C�}�����ŌĂ΂��
	virtual void TimerFunc(int id) {
		/// �����̃`�F�b�N�Ɖ�ʂ̍X�V���s��
		if (running){
			OnStep();
		}
		// �ĕ`��v��
		PostRedisplay();
	}

	// �`��֐��D�`��v���������Ƃ��ɌĂ΂��
	virtual void Display() {
		GRRenderIf *render = GetSdk()->GetRender();
		render->ClearBuffer();
		render->BeginScene();

		GRCameraDesc camera = render->GetCamera();
		camera.front = 0.3;
		render->SetCamera(camera);
		render->SetViewMatrix(cameraInfo.view.inv());

		OnDraw(render);
		DrawInfo(render);

		render->EndScene();
		glutSwapBuffers();
	}

	virtual void Keyboard(int key, int x, int y) {
		for(int i = 0; i < (int)actions.size(); i++){
			for(int j = 0; j < (int)actions[i].keys.size(); j++){
				if(actions[i].keys[j].first == key){
					OnAction(actions[i].id);
					break;
				}
			}
		}
	}

};
