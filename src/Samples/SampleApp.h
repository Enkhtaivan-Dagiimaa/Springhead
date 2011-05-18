/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SAMPLE_APP_H
#define SAMPLE_APP_H

/**
	�T���v���v���O�������p�A�v���P�[�V�����N���X
 **/

#include <Springhead.h>
#include <Framework/SprFWApp.h>
#include <Framework/SprFWEditor.h>


using namespace std;
using namespace Spr;

#define ESC		27

class SampleApp : public FWApp{
public:
	/** ���j���[ID
		MENU_ALWAYS�͂��ł��\�������
		�V�[���ɑΉ����郁�j���[��1�`99��ID���g�p(�V�[����99�܂�)
		100�ȍ~�����L���j���[���g�p
	 */
	enum MenuID{
		MENU_ALWAYS		= 0,			///< ���ł��L���ȃ��j���[
		MENU_SCENE		= 1,			///< �V�[�����j���[
		MENU_COMMON		= 100,			///< ���L���j���[
		MENU_CONFIG		= MENU_COMMON,	///< �p�����[�^�ݒ�n
		MENU_STATE,						///< �X�e�[�g�ۑ��n
		MENU_EDITOR,					///< FWEditor��\��
		MENU_COMMON_LAST,
	};
	/// �A�N�V����ID
	enum ActionAlways{
		ID_EXIT,					///< �I��
		ID_TOGGLE,					///< �V�~�����[�V�����̊J�n�ƒ�~
		ID_STEP,					///< �X�e�b�v���s
	};
	enum ActionState{
		ID_LOAD_STATE,
		ID_SAVE_STATE,
		ID_RELEASE_STATE,
		ID_READ_STATE,
		ID_WRITE_STATE,
		ID_DUMP,
	};
	enum ActionConfig{
		ID_SWITCH_LCP_PENALTY,		///< LCP�ƃy�i���e�B�@��؂�ւ���
		ID_TOGGLE_GRAVITY,			///< �d�̗͂L�����Ɩ�����
		ID_TOGGLE_JOINT,			///< �֐߂̗L�����Ɩ�����
		ID_TOGGLE_ABA,				///< ABA�̗L�����Ɩ�����
		ID_INC_TIMESTEP,			///< �^�C���X�e�b�v�𑝂₷
		ID_DEC_TIMESTEP,			///< �^�C���X�e�b�v�����炷
	};
	/// �`��ID
	enum ShapeID{
		SHAPE_BOX,
		SHAPE_CAPSULE,
		SHAPE_ROUNDCONE,
		SHAPE_SPHERE,
		SHAPE_ROCK,
		SHAPE_BLOCK,
	};
	
	/// �A�N�V�������
	struct Action{
		int			id;							///< �A�N�V����ID
		vector< pair<int, UTString> > keys;		///< �L�[�Ƒ�փe�L�X�g
		UTString	brief;						///< �ȒP�Ȑ���
		UTString	detail;						///< �ڂ�������
	};
	struct Menu : map<int, Action>{
		UTString	brief;						///< ���j���[�̐���
		/// �L�[�ɑΉ�����A�N�V����ID��Ԃ�
		int Query(int key){
			for(iterator it = begin(); it != end(); it++){
				Action& a = it->second;
				for(int i = 0; i < (int)a.keys.size(); i++){
					if(a.keys[i].first == key)
						return a.id;
				}
			}
			return -1;
		}
	};
	typedef map<int, Menu>	Menus;
	Menus					menus;
	int						curMenu;		///< �\�����̋��L���j���[
	stringstream			ss;
	UTString				message;		///< ��s���b�Z�[�W
	
	FWEditorOverlay			editor;			///< �������̕\��/�ҏW�@�\
	FWSceneIf*				fwScene;		///< �A�N�e�B�u�ȃV�[��
	PHSceneIf*				phScene;
	UTRef<ObjectStatesIf>	states;			///< ��ԕۑ��p
	UTTimerIf*				timer;			///< �^�C�}

	/// ���p�̌`��
	CDBoxIf*				shapeFloor;
	CDBoxIf*				shapeWall;
	/// ��肨���`��
	CDBoxIf*				shapeBox;
	CDSphereIf*				shapeSphere;
	CDCapsuleIf*			shapeCapsule;
	CDRoundConeIf*			shapeRoundCone;
	
	/// ���
	bool					running;		///< �V�~�����[�V�������s���t���O
	bool					useLCP;			///< LCP��penalty�@��
	bool					showHelp;		///< �w���v�\��
	int						curScene;		///< �A�N�e�B�u�ȃV�[���̔ԍ�
	Vec3d					tmpGravity;		///< �d�͖��������̑ޔ�ϐ�
	
	/// ����: �h���N���X���R���X�g���N�^�Őݒ肷��
	UTString				appName;		///< �T���v����. �h���N���X���R���X�g���N�^�Őݒ肷��
	int						numScenes;		///< �V�[���̐�
	
	/// �w���v�̕`�摮��
	float ymargin;
	float xmargin;
	float yline;
	float xkeys;
	float xbrief;

public:
	/// ���j���[�̓o�^
	void AddMenu(int menu, UTString brief){
		menus[menu].brief = brief;
	}
	/// �A�N�V�����̓o�^
	void AddAction(int menu, int id, UTString brief, UTString detail = ""){
		Action& act = menus[menu][id];
		act.id		= id;
		act.brief   = brief;
		act.detail  = (detail == "" ? brief : detail);
	}

	/// �A�N�V�����ƃL�[�̑Ή�
	void AddHotKey(int menu, int id, int key, UTString alt = ""){
		menus[menu][id].keys.push_back(make_pair(key, alt));
	}

	/// ���̍쐬
	PHSolidIf* CreateFloor(){
		PHSolidIf* soFloor = phScene->CreateSolid();
		soFloor->SetDynamical(false);
		soFloor->SetFramePosition(Vec3f(0,-1,0));
	
		soFloor->AddShape(shapeFloor);
		soFloor->AddShape(shapeWall);
		soFloor->AddShape(shapeWall);
		soFloor->AddShape(shapeWall);
		soFloor->AddShape(shapeWall);
		soFloor->SetShapePose(1, Posed::Trn(-60, 0,   0));
		soFloor->SetShapePose(2, Posed::Trn(  0, 0, -40));
		soFloor->SetShapePose(3, Posed::Trn( 60, 0,   0));
		soFloor->SetShapePose(4, Posed::Trn(  0, 0,  40));

		fwScene->SetSolidMaterial(GRRenderIf::GRAY, soFloor);

		return soFloor;
	}

	void SwitchScene(int id){
		if(id < 0 || numScenes <= id)
			return;
		curScene = id;
		GetSdk()->SwitchScene(GetSdk()->GetScene(id));
		fwScene = GetSdk()->GetScene();
		phScene = fwScene->GetPHScene();
		editor.SetObject(phScene);
		//cameraInfo.Fit(GetSdk()->GetRender()->GetCamera(), activeHandler->GetSceneRadius());
	}
	
	/** @brief	�F�X�Ȍ`�̕��̂��쐬
		@param	shape	�`
	 */
	void Drop(int shape, int mat, Vec3d v, Vec3d w, Vec3d p, Quaterniond q){
		// �X�e�[�g�����
		states->ReleaseState(phScene);
	
		// ���̂��쐬
		PHSolidIf* solid = phScene->CreateSolid();
		// �}�e���A����ݒ�
		fwScene->SetSolidMaterial(mat, solid);
			
		// �`��̊�����
		if(shape == SHAPE_BOX)
			solid->AddShape(shapeBox);
		if(shape == SHAPE_CAPSULE)
			solid->AddShape(shapeCapsule);
		if(shape == SHAPE_ROUNDCONE)
			solid->AddShape(shapeRoundCone);
		if(shape == SHAPE_SPHERE)
			solid->AddShape(shapeSphere);
		if(shape == SHAPE_ROCK){
			CDConvexMeshDesc md;
			int nv = rand()%100 + 50;
			for(int i=0; i < nv; ++i){
				Vec3d v;
				for(int c=0; c<3; ++c){
					v[c] = (rand() % 100 / 100.0 - 0.5) * 5 * 1.3;
				}
				md.vertices.push_back(v);
			}
			solid->AddShape(GetSdk()->GetPHSdk()->CreateShape(md));
		}
		if(shape == SHAPE_BLOCK){
			for(int i = 0; i < 7; i++)
				solid->AddShape(shapeBox);
			Posed pose;
			pose.Pos() = Vec3d( 3,  0,  0); solid->SetShapePose(1, pose);
			pose.Pos() = Vec3d(-3,  0,  0); solid->SetShapePose(2, pose);
			pose.Pos() = Vec3d( 0,  3,  0); solid->SetShapePose(3, pose);
			pose.Pos() = Vec3d( 0, -3,  0); solid->SetShapePose(4, pose);
			pose.Pos() = Vec3d( 0,  0,  3); solid->SetShapePose(5, pose);
			pose.Pos() = Vec3d( 0,  0, -3); solid->SetShapePose(6, pose);
		}

		solid->SetVelocity(v);
		solid->SetAngularVelocity(w);
		solid->SetFramePosition(p);
		solid->SetOrientation(q);
	}

	///
	void Shoot(int shape, int mat){

	}

	/// ���j���[�̕\��
	void DrawMenu(GRRenderIf* render, int id, Vec2f& offset){
		Vec2f pos;

		Menu& menu = menus[id];

		render->DrawFont(pos + offset, menu.brief);
		pos.y += yline;

		for(Menu::iterator it = menu.begin(); it != menu.end(); it++){
			Action& a = it->second;
			// �z�b�g�L�[
			ss.str("");
			for(int i = 0; i < (int)a.keys.size(); i++){
				if(a.keys[i].second.empty())
					 ss << (char)a.keys[i].first;
				else ss << a.keys[i].second;
				ss << ' ';
			}
			render->DrawFont(pos + offset, ss.str());

			// ����
			pos.x = xbrief;
			render->DrawFont(pos + offset, a.brief);

			pos.y += yline;
			pos.x = 0;
		}
		offset += pos;
	}
	/// �t�����̕\��
	void DrawHelp(GRRenderIf* render){
		render->SetLighting(false);
		render->SetDepthTest(false);
		render->EnterScreenCoordinate();
		Vec2f pos(xmargin, ymargin);

		// �w���v�ɂ���
		if(showHelp)
			 render->DrawFont(pos, "hit \'h\' to hide help");
		else render->DrawFont(pos, "hit \'h\' to show help");
		pos.y += yline;
		// �V�[�����
		ss.str("");
		ss << "# of scenes : " << GetSdk()->NScene() << " hit [0-9] to switch scene";
		render->DrawFont(pos, ss.str());
		pos.y += yline;
		
		if(showHelp){
			// ���ł��\���n���j���[
			DrawMenu(render, MENU_ALWAYS, pos);
			pos.y += yline;

			// �V�[�����j���[
			DrawMenu(render, MENU_SCENE + curScene, pos);
			pos.y += yline;

			// ���L���j���[
			DrawMenu(render, curMenu, pos);
			pos.y += yline;

			render->DrawFont(pos, "hit TAB to switch menu");
			pos.y += yline;
		}

		// ���b�Z�[�W
		render->DrawFont(pos, message);

		render->LeaveScreenCoordinate();
		render->SetLighting(true);
		render->SetDepthTest(true);
	}
	
	SampleApp(){
		running		= true;
		useLCP		= true;
		showHelp	= false;
		appName		= "untitled";
		numScenes	= 1;
		curScene	= 0;
		curMenu		= MENU_COMMON;

		ymargin		= 10;
		xmargin		= 20;
		yline		= 20;
		xkeys		= 0;
		xbrief		= 100;

		/// ���ł��L���n
		AddMenu(MENU_ALWAYS, "");
		AddAction(MENU_ALWAYS, ID_EXIT, "exit", "exit the application");
		AddHotKey(MENU_ALWAYS, ID_EXIT, ESC, "ESC");
		AddHotKey(MENU_ALWAYS, ID_EXIT, 'q');
		AddHotKey(MENU_ALWAYS, ID_EXIT, 'Q');
		AddAction(MENU_ALWAYS, ID_TOGGLE, "start", "start simulation");
		AddHotKey(MENU_ALWAYS, ID_TOGGLE, ' ', "space");
		AddAction(MENU_ALWAYS, ID_STEP, "step", "step simulation");
		AddHotKey(MENU_ALWAYS, ID_STEP, ';');

		/// ���L�R�}���h�̓V�[���R�}���h�Ƃ̏Փˉ���̂��߂ɑ啶�������蓖�Ă�
		/// �X�e�[�g�̕ۑ��╜�A
		AddMenu(MENU_STATE, "< save and load states >");
		AddAction(MENU_STATE, ID_LOAD_STATE, "load state", "load saved state");
		AddHotKey(MENU_STATE, ID_LOAD_STATE, 'L');
		AddAction(MENU_STATE, ID_SAVE_STATE, "save state", "save current state");
		AddHotKey(MENU_STATE, ID_SAVE_STATE, 'S');
		AddAction(MENU_STATE, ID_RELEASE_STATE, "release state", "release saved state");
		AddHotKey(MENU_STATE, ID_RELEASE_STATE, 'X');
		AddAction(MENU_STATE, ID_READ_STATE, "read state", "read state from state.bin");
		AddHotKey(MENU_STATE, ID_READ_STATE, 'R');
		AddAction(MENU_STATE, ID_WRITE_STATE, "write state", "write state to state.bin");
		AddHotKey(MENU_STATE, ID_WRITE_STATE, 'W');
		AddAction(MENU_STATE, ID_DUMP, "dump", "dump object data to dump.bin");
		AddHotKey(MENU_STATE, ID_DUMP, 'D');
		/// �V�~�����[�V�����ݒ�
		AddMenu(MENU_CONFIG, "< simulation settings >");
		AddAction(MENU_CONFIG, ID_SWITCH_LCP_PENALTY, "switch to penalty", "switch to penalty method");
		AddHotKey(MENU_CONFIG, ID_SWITCH_LCP_PENALTY, 'M');
		AddAction(MENU_CONFIG, ID_TOGGLE_GRAVITY, "disable gravity");
		AddHotKey(MENU_CONFIG, ID_TOGGLE_GRAVITY, 'G');
		AddAction(MENU_CONFIG, ID_TOGGLE_JOINT, "disable joints");
		AddHotKey(MENU_CONFIG, ID_TOGGLE_JOINT, 'J');
		AddAction(MENU_CONFIG, ID_TOGGLE_ABA, "disable ABA (non implemented)");
		AddHotKey(MENU_CONFIG, ID_TOGGLE_ABA, 'A');
		AddAction(MENU_CONFIG, ID_INC_TIMESTEP, "increase time step");
		AddHotKey(MENU_CONFIG, ID_INC_TIMESTEP, 'I');
		AddAction(MENU_CONFIG, ID_DEC_TIMESTEP, "decrease time step");
		AddHotKey(MENU_CONFIG, ID_DEC_TIMESTEP, 'D');
		
	}
	~SampleApp(){}

public: /** �h���N���X����������֐� **/

	///
	virtual void BuildScene(){}

	/// 1�X�e�b�v�̃V�~�����[�V����
	virtual void OnStep(){
		fwScene->Step();
	}

	/// �`��
	virtual void OnDraw(GRRenderIf* render){
		fwScene->DrawPHScene(render);
	}

	/// �A�N�V��������
	virtual void OnAction(int menu, int id){
		/// ���ł��L���A�N�V����
		if(menu == MENU_ALWAYS){
			if(id == ID_EXIT)
				exit(0);
			if(id == ID_TOGGLE){
				running = !running;
				if(running){
					AddAction(MENU_ALWAYS, ID_TOGGLE, "pause", "pause simulation");
					message = "simulation started.";
				}
				else{
					AddAction(MENU_ALWAYS, ID_TOGGLE, "start", "start simulation");
					message = "simulation paused.";
				}
			}
			if(id == ID_STEP){
				fwScene->Step();
				message = "one step simulated.";
			}
		}
		if(menu == MENU_STATE){
			if(id == ID_LOAD_STATE){
				states->LoadState(phScene);
				message = "state loaded.";
			}
			if(id == ID_SAVE_STATE){
				states->SaveState(phScene);
				message = "state saved.";
			}
			if(id == ID_RELEASE_STATE){	
				states->ReleaseState(phScene);
				message = "saved state is released.";
			}
			if(id == ID_READ_STATE){
				phScene->ReadState("state.bin");
				message = "state read from state.bin.";
			}
			if(id == ID_WRITE_STATE){
				phScene->WriteState("state.bin");
				message = "state written to state.bin.";
			}
			if(id == ID_DUMP){
				std::ofstream f("dump.bin", std::ios::binary|std::ios::out);
				phScene->DumpObjectR(f);
				message = "dumped to dump.bin.";
			}
		}
		if(menu == MENU_CONFIG){
			if(id == ID_SWITCH_LCP_PENALTY){
				useLCP = !useLCP;
				if(useLCP){
					phScene->SetContactMode(PHSceneDesc::MODE_LCP);
					AddAction(MENU_CONFIG, ID_SWITCH_LCP_PENALTY, "switch to penalty", "switch to penalty method");
					message = "switched to LCP method.";
				}
				else{
					phScene->SetContactMode(PHSceneDesc::MODE_PENALTY);
					AddAction(MENU_CONFIG, ID_SWITCH_LCP_PENALTY, "switch to lcp", "switch to lcp method");
					message = "switched to penalty method.";
				}
			}
			if(id == ID_TOGGLE_GRAVITY){
				static bool enable = true;
				enable = !enable;
				if(enable){
					phScene->SetGravity(tmpGravity);
					AddAction(MENU_CONFIG, ID_TOGGLE_GRAVITY, "disable gravity");
					message = "gravity is enabled.";
				}
				else{
					tmpGravity = phScene->GetGravity();
					phScene->SetGravity(Vec3d());
					AddAction(MENU_CONFIG, ID_TOGGLE_GRAVITY, "enable gravity");
					message = "gravity is disabled";
				}
			}
			if(id == ID_TOGGLE_JOINT){
				static bool enable = true;
				enable = !enable;
				for(int i = 0; i < (int)phScene->NJoints(); i++)
					phScene->GetJoint(i)->Enable(enable);
				if(enable){
					AddAction(MENU_CONFIG, ID_TOGGLE_JOINT, "disable joints");
					message = "joints are enabled.";
				}
				else{
					AddAction(MENU_CONFIG, ID_TOGGLE_JOINT, "enable joints");
					message = "joints are disabled.";
				}
			}
			if(id == ID_TOGGLE_ABA){

			}
			if(id == ID_INC_TIMESTEP){
				phScene->SetTimeStep(std::min(0.1, 2.0 * phScene->GetTimeStep()));
				ss.str("");
				ss << "time step is now " << phScene->GetTimeStep();
				message = ss.str();
			}
			if(id == ID_DEC_TIMESTEP){
				phScene->SetTimeStep(std::max(0.001, 0.5 * phScene->GetTimeStep()));
				ss.str("");
				ss << "time step is now " << phScene->GetTimeStep();
				message = ss.str();
			}
		}
	}

public: /** FWApp�̎��� **/

	virtual void Init(int argc, char* argv[]){
		SetGRAdaptee(TypeGLUT);
		GRInit(argc, argv);
		CreateSdk();

		FWWinDesc windowDesc;
		windowDesc.width = 1024;
		windowDesc.height = 768;
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

		/// ���p�̌`��
		CDBoxDesc bd;
		bd.boxsize = Vec3d(60.0, 2.0, 40.0);
		shapeFloor = XCAST(GetSdk()->GetPHSdk()->CreateShape(bd));		
		bd.boxsize.y *= 6.0;
		shapeWall = XCAST(GetSdk()->GetPHSdk()->CreateShape(bd));

		// �`��̍쐬
		bd.boxsize = Vec3f(2,2,2);
		shapeBox = XCAST(GetSdk()->GetPHSdk()->CreateShape(bd));
		
		CDSphereDesc sd;
		sd.radius = 1;
		shapeSphere = XCAST(GetSdk()->GetPHSdk()->CreateShape(sd));
		
		CDCapsuleDesc cd;
		cd.radius = 1;
		cd.length = 1;
		shapeCapsule = XCAST(GetSdk()->GetPHSdk()->CreateShape(cd));
		
		CDRoundConeDesc rcd;
		rcd.length = 3;
		shapeRoundCone= XCAST(GetSdk()->GetPHSdk()->CreateShape(rcd));
		
		/// �V�[���̍쐬
		for(int i = 0; i < numScenes; i++){
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
			//scene->GetConstraintEngine()->SetUseContactSurface(true); //�ʐڐG�ł̗͌v�Z��L����

			// �V�[���\�z
			curScene = i;
			BuildScene();
	
		}
		states = ObjectStatesIf::Create();
		SwitchScene(0);
		
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
		DrawHelp(render);

		render->EndScene();
		render->SwapBuffers();
	}

	virtual void Keyboard(int key, int x, int y) {
		// 'h' : �w���v�̕\���؂�ւ�
		if(key == 'h' || key == 'H'){
			showHelp = !showHelp;
			return;
		}
		// TAB : ���j���[�؂�ւ�
		if(showHelp && key == '\t'){
			if(++curMenu == MENU_COMMON_LAST)
				curMenu = MENU_COMMON;
		}

		// �����L�[�@->�@�V�[���؂�ւ�
		if(key <= 0xff && isdigit(key)){
			SwitchScene(key - '0');
			return;
		}

		// �L�[�ɑΉ�����A�N�V���������s
		int id;
		// �펞�\�����j���[
		id = menus[MENU_ALWAYS].Query(key);
		if(id != -1)
			OnAction(MENU_ALWAYS, id);
		// �V�[�����j���[
		id = menus[MENU_SCENE + curScene].Query(key);
		if(id != -1)
			OnAction(MENU_SCENE + curScene, id);
		// ���L���j���[
		if(curMenu == MENU_EDITOR){
			editor.Key(key);
			editor.SetObject(phScene);
		}
		else{
			id = menus[curMenu].Query(key);
			if(id != -1)
				OnAction(curMenu, id);
		}
	}

};

#endif
