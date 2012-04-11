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
#include <map>

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
		MENU_DRAW,						///< �`��ݒ�n
		MENU_STATE,						///< �X�e�[�g�ۑ��n
		MENU_EDITOR,					///< FWEditor��\��
		MENU_COMMON_LAST,
	};
	/// �A�N�V����ID
	/// ��ɂ�����A�N�V����
	enum ActionAlways{
		ID_EXIT,					///< �I��
		ID_RUN,						///< �V�~�����[�V�����̊J�n�ƒ�~
		ID_STEP,					///< �X�e�b�v���s
	};
	/// �X�e�[�g�̕ۑ��ƕ��A�i�����؁j
	enum ActionState{
		ID_LOAD_STATE,
		ID_SAVE_STATE,
		ID_RELEASE_STATE,
		ID_READ_STATE,
		ID_WRITE_STATE,
		ID_DUMP,
	};
	/// �����V�~�����[�V�����̐ݒ�
	enum ActionConfig{
		ID_SWITCH_LCP_PENALTY,		///< LCP�ƃy�i���e�B�@��؂�ւ���
		ID_TOGGLE_GRAVITY,			///< �d�̗͂L�����Ɩ�����
		ID_TOGGLE_JOINT,			///< �֐߂̗L�����Ɩ�����
		ID_TOGGLE_ABA,				///< ABA�̗L�����Ɩ�����
		ID_INC_TIMESTEP,			///< �^�C���X�e�b�v�𑝂₷
		ID_DEC_TIMESTEP,			///< �^�C���X�e�b�v�����炷
		ID_INC_TIMER,				///< �^�C�}�����𑝂₷
		ID_DEC_TIMER,				///< �^�C�}���������炷
	};
	/// �`��̐ݒ�
	enum ActionDraw{
		ID_DRAW_SOLID,				///< �\���b�h
		ID_DRAW_WIREFRAME,			///< ���C���t���[��
		ID_DRAW_AXIS,				///< ���W��
		ID_DRAW_FORCE,				///< ��
		ID_DRAW_CONTACT,			///< �ڐG�f��
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
		bool		toggle;						///< On/Off�^�C�v�̃A�N�V����
		bool		enabled;					///< On��Off
		vector< pair<int, UTString> > keys;		///< �L�[�Ƒ�փe�L�X�g
		UTString	desc[2];					///< ����(0: On�ɂ���Ƃ�, 1: Off�ɂ���Ƃ�)
		UTString	message[2];					///< �A�N�V�����チ�b�Z�[�W(0: On�ɂ����Ƃ��C1: Off�ɂ����Ƃ��j
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
	void AddAction(int menu, int id, UTString desc, UTString msg = "", UTString descd = "", UTString msgd = ""){
		Action& act = menus[menu][id];
		act.id		= id;
		act.enabled = true;
		act.desc[0] = desc;
		act.desc[1] = (descd == "" ? desc : descd);
		act.message[0] = msg;
		act.message[1] = (msgd == "" ? msg : msgd);
	}
	/// �A�N�V�����ƃL�[�̑Ή�
	void AddHotKey(int menu, int id, int key, UTString alt = ""){
		menus[menu][id].keys.push_back(make_pair(key, alt));
	}
	/// On/Off�̔��]
	bool ToggleAction(int menu, int id){
		Action& act = menus[menu][id];
		return act.enabled = !act.enabled;
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
		// id�Ԗڂ̃V�[����I��
		fwScene = GetSdk()->GetScene(id);
		phScene = fwScene->GetPHScene();
		GetCurrentWin()->SetScene(fwScene);
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
			render->DrawFont(pos + offset, (a.enabled ? a.desc[1] : a.desc[0]));

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
		AddAction(MENU_ALWAYS, ID_EXIT, "exit");
		AddHotKey(MENU_ALWAYS, ID_EXIT, ESC, "ESC");
		AddHotKey(MENU_ALWAYS, ID_EXIT, 'q');
		AddHotKey(MENU_ALWAYS, ID_EXIT, 'Q');
		AddAction(MENU_ALWAYS, ID_RUN,
			"start", "simulation started.",
			"pause", "simulation paused.");
		AddHotKey(MENU_ALWAYS, ID_RUN, ' ', "space");
		AddAction(MENU_ALWAYS, ID_STEP, "step", "one step proceeded.");
		AddHotKey(MENU_ALWAYS, ID_STEP, ';');

		/// ���L�R�}���h�̓V�[���R�}���h�Ƃ̏Փˉ���̂��߂ɑ啶�������蓖�Ă�
		/// �X�e�[�g�̕ۑ��╜�A
		AddMenu(MENU_STATE, "< save and load states >");
		AddAction(MENU_STATE, ID_LOAD_STATE, "load state", "state loaded.");
		AddHotKey(MENU_STATE, ID_LOAD_STATE, 'L');
		AddAction(MENU_STATE, ID_SAVE_STATE, "save state", "state saved.");
		AddHotKey(MENU_STATE, ID_SAVE_STATE, 'S');
		AddAction(MENU_STATE, ID_RELEASE_STATE, "release state", "saved state is released.");
		AddHotKey(MENU_STATE, ID_RELEASE_STATE, 'X');
		AddAction(MENU_STATE, ID_READ_STATE, "read state", "state read from state.bin.");
		AddHotKey(MENU_STATE, ID_READ_STATE, 'R');
		AddAction(MENU_STATE, ID_WRITE_STATE, "write state", "state written to state.bin.");
		AddHotKey(MENU_STATE, ID_WRITE_STATE, 'W');
		AddAction(MENU_STATE, ID_DUMP, "dump", "object data dumped to dump.bin.");
		AddHotKey(MENU_STATE, ID_DUMP, 'D');
		/// �V�~�����[�V�����ݒ�
		AddMenu(MENU_CONFIG, "< simulation settings >");
		AddAction(MENU_CONFIG, ID_SWITCH_LCP_PENALTY,
			"switch to penalty", "switched to penalty method",
			"switch to lcp", "switched to lcp method");
		AddHotKey(MENU_CONFIG, ID_SWITCH_LCP_PENALTY, 'M');
		AddAction(MENU_CONFIG, ID_TOGGLE_GRAVITY,
			"enable gravity", "gravity enabled.",
			"disable gravity", "gravity disabled.");
		AddHotKey(MENU_CONFIG, ID_TOGGLE_GRAVITY, 'G');
		AddAction(MENU_CONFIG, ID_TOGGLE_JOINT,
			"enable joints", "joints enabled.",
			"disable joints", "joints disabled.");
		AddHotKey(MENU_CONFIG, ID_TOGGLE_JOINT, 'J');
		AddAction(MENU_CONFIG, ID_TOGGLE_ABA,
			"enable ABA", "ABA enabled.",
			"disable ABA", "ABA disabled.");
		AddHotKey(MENU_CONFIG, ID_TOGGLE_ABA, 'A');
		AddAction(MENU_CONFIG, ID_INC_TIMESTEP, "increase time step");
		AddHotKey(MENU_CONFIG, ID_INC_TIMESTEP, 'I');
		AddAction(MENU_CONFIG, ID_DEC_TIMESTEP, "decrease time step");
		AddHotKey(MENU_CONFIG, ID_DEC_TIMESTEP, 'D');
		AddAction(MENU_CONFIG, ID_INC_TIMER, "increase timer interval");
		AddHotKey(MENU_CONFIG, ID_INC_TIMER, 'F');
		AddAction(MENU_CONFIG, ID_DEC_TIMER, "decrease timer interval");
		AddHotKey(MENU_CONFIG, ID_DEC_TIMER, 'S');
		/// �`��ݒ�n
		AddMenu(MENU_DRAW, "< drawing setting >");
		AddAction(MENU_DRAW, ID_DRAW_WIREFRAME,
			"enable wireframe", "wireframe enabled.",
			"disable wireframe", "wireframe disabled");
		AddHotKey(MENU_DRAW, ID_DRAW_WIREFRAME, 'W');
		AddAction(MENU_DRAW, ID_DRAW_SOLID,
			"enable solid", "solid enabled.",
			"disable solid", "solid disabled");
		AddHotKey(MENU_DRAW, ID_DRAW_SOLID, 'S');
		AddAction(MENU_DRAW, ID_DRAW_AXIS,
			"enable axis drawing", "axis drawing enabled.",
			"disable axis drawing", "axis drawing disabled.");
		AddHotKey(MENU_DRAW, ID_DRAW_AXIS, 'A');
		AddAction(MENU_DRAW, ID_DRAW_FORCE,
			"enable force drawing", "force drawing enabled.",
			"disable force drawing", "force drawing disabled.");
		AddHotKey(MENU_DRAW, ID_DRAW_FORCE, 'F');
		AddAction(MENU_DRAW, ID_DRAW_CONTACT,
			"enable contact drawing", "contact drawing enabled.",
			"disable contact drawing", "contact drawing disabled.");
		AddHotKey(MENU_DRAW, ID_DRAW_CONTACT, 'C');
	}
	~SampleApp(){}

public: /** �h���N���X����������֐� **/

	/// �V�[���\�z���s���D
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
			if(id == ID_RUN)
				ToggleAction(menu, id);
			if(id == ID_STEP)
				fwScene->Step();
		}
		if(menu == MENU_STATE){
			if(id == ID_LOAD_STATE)
				states->LoadState(phScene);
			if(id == ID_SAVE_STATE)
				states->SaveState(phScene);
			if(id == ID_RELEASE_STATE)
				states->ReleaseState(phScene);
			if(id == ID_READ_STATE)
				phScene->ReadState("state.bin");
			if(id == ID_WRITE_STATE)
				phScene->WriteState("state.bin");
			if(id == ID_DUMP){
				std::ofstream f("dump.bin", std::ios::binary|std::ios::out);
				phScene->DumpObjectR(f);
			}
		}
		if(menu == MENU_CONFIG){
			if(id == ID_SWITCH_LCP_PENALTY){
				if(ToggleAction(menu, id))
					 phScene->SetContactMode(PHSceneDesc::MODE_LCP);
				else phScene->SetContactMode(PHSceneDesc::MODE_PENALTY);
			}
			if(id == ID_TOGGLE_GRAVITY){
				if(ToggleAction(menu, id)){
					phScene->SetGravity(tmpGravity);
					//for(int i = 0; i < phScene->NSolids(); i++)
					//	phScene->GetSolids()[i]->SetGravity(true);
				}
				else{
					tmpGravity = phScene->GetGravity();
					phScene->SetGravity(Vec3d());
					//for(int i = 0; i < phScene->NSolids(); i++)
					//	phScene->GetSolids()[i]->SetGravity(false);
				}
			}
			if(id == ID_TOGGLE_JOINT){
				bool on = ToggleAction(menu, id);
				for(int i = 0; i < (int)phScene->NJoints(); i++)
					phScene->GetJoint(i)->Enable(on);
			}
			if(id == ID_TOGGLE_ABA){
				bool on = ToggleAction(menu, id);
				for(int i=0; i<phScene->NRootNodes(); ++i){
					PHRootNodeIf* rn = phScene->GetRootNode(i);
					rn->Enable(on);
				}
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
			if(id == ID_INC_TIMER){
				timer->SetInterval(std::min(1000, 2 * (int)timer->GetInterval()));
				ss.str("");
				ss << "timer interval is now " << timer->GetInterval();
				message = ss.str();
			}
			if(id == ID_DEC_TIMER){
				timer->SetInterval(std::max(10, (int)timer->GetInterval() / 2));
				ss.str("");
				ss << "timer interval is now " << timer->GetInterval();
				message = ss.str();
			}
		}
		if(menu == MENU_DRAW){
			if(id == ID_DRAW_SOLID)
				fwScene->SetRenderMode(ToggleAction(menu, id), menus[menu][ID_DRAW_WIREFRAME].enabled);
			if(id == ID_DRAW_WIREFRAME)
				fwScene->SetRenderMode(menus[menu][ID_DRAW_SOLID].enabled, ToggleAction(menu, id));
			if(id == ID_DRAW_AXIS){
				bool on = ToggleAction(menu, id);
				fwScene->EnableRenderAxis(on, on, on);
			}
			if(id == ID_DRAW_FORCE){
				bool on = ToggleAction(menu, id);
				fwScene->EnableRenderForce(on, on);
			}
			if(id == ID_DRAW_CONTACT){
				fwScene->EnableRenderContact(ToggleAction(menu, id));
			}
		}
		// ���̎��_�Ń��b�Z�[�W���ݒ肳��Ă��Ȃ���΃f�t�H���g���b�Z�[�W
		if(message == ""){
			Action& act = menus[menu][id];
			message = act.message[!act.enabled];
		}
	}

public: /** FWApp�̎��� **/

	virtual void Init(int argc, char* argv[]){
		CreateSdk();
		GRInit(argc, argv);
		
		FWWinDesc windowDesc;
		// windowDesc.width = 1024;
		// windowDesc.height = 768;
		windowDesc.width = 1280;
		windowDesc.height = 720;
		windowDesc.title = appName;
		CreateWin(windowDesc);
		
		/// �����ݒ�
		GRRenderIf* render = GetCurrentWin()->GetRender();
		GRLightDesc ld;
		ld.diffuse  = Vec4f(1.0f, 1.0f, 1.0f, 1.0f);
		ld.specular = Vec4f(1.0f, 1.0f, 1.0f, 1.0f);
		ld.ambient  = Vec4f(0.0f, 0.0f, 0.0f, 1.0f);
		ld.position = Vec4f(25.0f, 50.0f, 20.0f, 1.0f);
		render->PushLight(ld);

		/// ���p�̌`��
		CDBoxDesc bd;
		bd.boxsize = Vec3d(60.0, 2.0, 40.0);
		shapeFloor = GetSdk()->GetPHSdk()->CreateShape(bd)->Cast();
		bd.boxsize.y *= 6.0;
		shapeWall = GetSdk()->GetPHSdk()->CreateShape(bd)->Cast();

		// �`��̍쐬
		bd.boxsize = Vec3f(2,2,2);
		shapeBox = GetSdk()->GetPHSdk()->CreateShape(bd)->Cast();
		
		CDSphereDesc sd;
		sd.radius = 1;
		shapeSphere = GetSdk()->GetPHSdk()->CreateShape(sd)->Cast();
		
		CDCapsuleDesc cd;
		cd.radius = 1;
		cd.length = 1;
		shapeCapsule = GetSdk()->GetPHSdk()->CreateShape(cd)->Cast();
		
		CDRoundConeDesc rcd;
		rcd.length = 3;
		shapeRoundCone= GetSdk()->GetPHSdk()->CreateShape(rcd)->Cast();
		
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
		if (menus[MENU_ALWAYS][ID_RUN].enabled){
			OnStep();
		}
		// �ĕ`��v��
		PostRedisplay();
	}

	// �`��֐��D�`��v���������Ƃ��ɌĂ΂��
	virtual void Display() {
		FWWinIf* win = GetCurrentWin();
		GRRenderIf *render = win->GetRender();
		render->ClearBuffer();
		render->BeginScene();

		GRCameraDesc camera = render->GetCamera();
		camera.front = 0.3f;
		render->SetCamera(camera);
		render->SetViewMatrix(win->GetTrackball()->GetAffine().inv());

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
		message = "";
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
