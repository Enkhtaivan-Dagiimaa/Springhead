/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include "FWGLUISample.h"
#include <iostream>
#include <sstream>

FWGLUISample::FWGLUISample(){
}

void FWGLUISample::Init(int argc, char* argv[]){
	/// SDK�쐬
	CreateSdk();

	/// �O���t�B�N�X��GLUI�ŏ�����
	GRInit(argc, argv, TypeGLUI);

	/// Scene�̍쐬
	GetSdk()->CreateScene();
	
	FWWinDesc windowDesc;
	windowDesc.title = "FWGLUISample";
	CreateWin(windowDesc);
	
	/// �^�C�}���쐬
	CreateTimer();
}

void FWGLUISample::CreateObjects(){
	PHSceneIf* phscene = GetSdk()->GetScene(0)->GetPHScene();
	PHSolidDesc desc;
	CDBoxDesc bd;

	// ��(�����@���ɏ]��Ȃ��C�^�����ω����Ȃ�)
	{
		// ����(soFloor)�̍쐬
		desc.mass = 1e20f;
		desc.inertia *= 1e30f;
		PHSolidIf* soFloor = phscene->CreateSolid(desc);		// ���̂�desc�Ɋ�Â��č쐬
		soFloor->SetDynamical(false);
		soFloor->SetGravity(false);
		// �`��(shapeFloor)�̍쐬
		bd.boxsize = Vec3f(50, 10, 50);
		CDShapeIf* shapeFloor = GetSdk()->GetPHSdk()->CreateShape(bd);
		// ���̂Ɍ`���t������
		soFloor->AddShape(shapeFloor);
		soFloor->SetFramePosition(Vec3d(0, -5, 0));
	}

	// ��(�����@���ɏ]���C�^�����ω�)
	{
		// ����(soBox)�̍쐬
		desc.mass = 0.5;
		desc.inertia *= 0.033;
		PHSolidIf* soBox = phscene->CreateSolid(desc);
		// �`��(shapeBox)�̍쐬
		bd.boxsize = Vec3f(2,2,2);
		CDShapeIf* shapeBox = GetSdk()->GetPHSdk()->CreateShape(bd);
		// ���̂Ɍ`���t��
		soBox->AddShape(shapeBox);
		soBox->SetFramePosition(Vec3d(0, 10, 0));
	}
}

void FWGLUISample::Keyboard(int key, int x, int y){
	switch (key) {
	case DVKeyCode::ESC:
	case 'q':
		exit(0);
		break;
	default:
		break;
}

}

/*
void FWGLUISample::DesignGUI(){
	panel = glui->add_panel("Sample", true);
	button1 = glui->add_button_to_panel(panel, "Create Box", 1, GLUI_CB(CallButton1));
}

void FWGLUISample::CallButton1(int control){
	((FWGLUISample*)FWApp::instance)->Button1(control);
}

void FWGLUISample::Button1(int control){
	Keyboard('1', 0, 0);
}
*/
