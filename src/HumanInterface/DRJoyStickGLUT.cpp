/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

#include <HumanInterface/DRJoyStickGLUT.h>
#include <HumanInterface/HISdk.h>
#ifdef USE_HDRSTOP
 #pragma hdrstop
#endif

namespace Spr {;

//DRJoyStickGLUT::JoyStickMap		joyStickMap;
DRJoyStickGLUT* DRJoyStickGLUT::instance = 0;
	
DRJoyStickGLUT::DRJoyStickGLUT(){
	instance = this;
}

bool DRJoyStickGLUT::Init(){
	SetName("JoyStickGLUT");
	//RegisterCallback();
	return true;
}

HIVirtualDeviceIf* DRJoyStickGLUT::Rent(const IfInfo* ii, const char* n, int portNo){
	HIVirtualDeviceIf* dv = HIRealDevice::Rent(ii, n, portNo);

	// �����̉��z�f�o�C�X���Ȃ���΍쐬
	if(!dv){
		DVJoyStick* js = DBG_NEW DV(this, portNo);
		AddChildObject(js->Cast());
		SetPollInterval(js);
		dv = js->Cast();
	}

	return dv;
}

/*void DRJoyStickGLUT::RegisterCallback(){
	glutJoystickFunc(OnUpdateStatic, pollInterval);
}*/

void DRJoyStickGLUT::SetPollInterval(DVJoyStick* dv){
	int wid = dv->GetPortNo();
	glutSetWindow(wid);
	glutJoystickFunc(OnUpdateStatic, dv->pollInterval);
}

/*void DRJoyStickGLUT::Register(HISdkIf* intf){
	HISdk* sdk = intf->Cast();
	dvJoyStick = DBG_NEW DRJoyStickGLUT::DV(this);
	sdk->RegisterVirtualDevice(dvJoyStick->Cast());
}*/

void DRJoyStickGLUT::OnUpdate(unsigned int buttonMask, int x, int y, int z){
	int wid = glutGetWindow();
	for(int i = 0; i < (int)NChildObject(); i++){
		DVJoyStick* dv = GetChildObject(i)->Cast();
		if(dv && dv->GetPortNo() == wid)
			dv->OnUpdate(buttonMask, x, y, z);
	}
}

void GLUTCALLBACK DRJoyStickGLUT::OnUpdateStatic(unsigned int buttonMask, int x, int y, int z){
	/*int wid = glutGetWindow();
	for(JoyStickMap::iterator it = joyStickMap.begin(); it != joyStickMap.end(); it++){
		if(it->first == wid)
			it->second->OnUpdate(buttonMask, x, y, z);
	}*/
	instance->OnUpdate(buttonMask, x, y, z);
}

}	//	namespace Spr
