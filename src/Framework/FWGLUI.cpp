#include "Framework/SprFWApp.h"

#include <Framework/SprFWGLUI.h>
#include "FWScene.h"
#include "FWSdk.h"
#include "FWOldSpringheadNode.h"
#include <Physics/PHSdk.h>
#include <Graphics/GRSdk.h>
#include <GL/glui.h>

namespace Spr{;

/*FWGLUIDesc�̎���*///---------------------------------------------------------
FWGLUIDesc::FWGLUIDesc(){
	fromTop				= 50;
	fromLeft			= 30;
	subPosition			= GLUI_SUBWINDOW_RIGHT;
	gluiName			= "Menu";
	createOtherWindow	= true;
}

FWGLUI::~FWGLUI(){
}

/*FWGLUI�̎���*///--------------------------------------------------------------
void FWGLUI::StartMainLoop(){
	instance = this;
	if (!fwApp->NWin()){
		CreateWin();
		FWWin* win = fwApp->wins.back();
		win->SetScene(fwApp->GetSdk()->GetScene());
	}
	// glui���g���Ȃ�FWAppGLUT::GlutTimerFunc��GLUI_Master�̊Ǘ����ɒu���K�v������
	GLUI_Master.set_glutTimerFunc(1, FWGLUT::GlutTimerFunc, 0);
	GLUI_Master.set_glutIdleFunc(FWGLUT::GlutIdleFunc);

	glutIdleFunc(FWGLUT::GlutIdleFunc);
	glutMainLoop();
}

GLUI* FWGLUI::CreateGUI(int wid, FWGLUIDesc desc){
	GLUI* glui;
	if(desc.createOtherWindow == false){
		glui = GLUI_Master.create_glui_subwindow(wid, desc.subPosition);		
	}else{
		glui = GLUI_Master.create_glui(gluiName, 0, desc.fromLeft, desc.fromTop);
		glui->set_main_gfx_window(wid);
	}
	guis.push_back(glui);
	return glui;
}
}