/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
\page pageFEMThermo FEM�ɂ�鉷�x�V�~�����[�V�����̃T���v��
Springhead2/src/Samples/FEMThermo

\section secQuitBoxStack �I���
- Esc��'q'�ŋ����I���B
	
*/
//
#include "../../SampleApp.h"
#include "FWMyApptest.h"

#pragma hdrstop
using namespace Spr;
using namespace std;



class MyApp : public SampleApp{
public:
	/// �y�[�WID
	enum {
		MENU_MAIN = MENU_SCENE,
	};
	/// �A�N�V����ID
	enum {
		ID_BOX,
	};

public:
	MyApp(){
		appName = "FEMThermo";

		AddAction(MENU_MAIN, ID_BOX, "drop object");
		AddHotKey(MENU_MAIN, ID_BOX, 'd');
	}
	~MyApp(){}

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


		// �`��̍쐬
		CDBoxDesc bd;
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
		
		//	�t�@�C���̃��[�h
		UTRef<ImportIf> import = GetSdk()->GetFISdk()->CreateImport();
		GetSdk()->LoadScene("scene.spr", import);			// �t�@�C���̃��[�h
		if (GetSdk()->NScene())	SwitchScene(GetSdk()->NScene()-1);

		/// �`��ݒ�
		if (fwScene){
			fwScene->SetWireMaterial(GRRenderIf::WHITE);
			fwScene->SetRenderMode(true, true);				///< �\���b�h�`��C���C���t���[���`��
			fwScene->EnableRenderAxis(true, true, true);		///< ���W��
			fwScene->SetAxisStyle(FWSceneIf::AXIS_ARROWS);	///< ���W���̃X�^�C��
			fwScene->EnableRenderForce(false, true);			///< ��
			fwScene->EnableRenderContact(true);				///< �ڐG�f��
		}
		if (phScene){
			phScene = fwScene->GetPHScene();
			phScene->SetGravity(Vec3f(0.0f, -9.8f, 0.0f));	// �d�͂�ݒ�
			phScene->SetTimeStep(0.05);
			phScene->SetNumIteration(15);
		
			phScene->SetStateMode(true);
		}

		//	�Z�[�u�p�̃X�e�[�g�̗p��
		states = ObjectStatesIf::Create();		
		// �^�C�}
		timer = CreateTimer(UTTimerIf::FRAMEWORK);
		timer->SetInterval(10);
	}

	// �`��֐��D�`��v���������Ƃ��ɌĂ΂��
	virtual void OnDraw(GRRenderIf* render) {
		SampleApp::OnDraw(render);

		std::ostringstream sstr;
		sstr << "NObj = " << phScene->NSolids();
		render->DrawFont(Vec2f(-21, 23), sstr.str());
	}

	//�A�N�V��������
	virtual void OnAction(int menu, int id){
		if(menu == MENU_MAIN){
			Vec3d v, w(0.0, 0.0, 0.2), p(0.5, 20.0, 0.0);
			Quaterniond q = Quaterniond::Rot(Rad(30.0), 'y');

			if(id == ID_BOX){
				Drop(SHAPE_BOX, GRRenderIf::RED, v, w, p, q);
				message = "box dropped.";
			}
		}
		SampleApp::OnAction(menu, id);
	}

};


////////////////////////////////////////////////////////////////////////////////////////////////////////////

//FWMyApptest app;
MyApp app;

/**
 brief		���C���֐�
 param		<in/--> argc�@�@�R�}���h���C�����͂̌�
 param		<in/--> argv�@�@�R�}���h���C������
 return		0 (����I��)
 */
int main(int argc, char *argv[]) {
	app.Init(argc, argv);
	app.StartMainLoop();
	return 0;
}

