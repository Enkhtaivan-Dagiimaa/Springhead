/**
 *	@file GRSdk.h
 *	@brief �O���t�B�b�N�XSDK
 */
#ifndef GRSDK_H
#define GRSDK_H
#include <SprGraphics.h>


namespace Spr {;

/**	@class	GRSdk
    @brief	�O���t�B�b�N�XSDK�@ */
class SPR_DLL GRSdk:public InheritObject<GRSdkIf, Object>{
protected:
	std::vector< UTRef<Object> > objects;
public:
	OBJECTDEF(GRSdk);
	~GRSdk();
	void Print(std::ostream& os) const {}
	GRDebugRenderIf* CreateDebugRender();
	GRDeviceGLIf* CreateDeviceGL(int window);
};

}
#endif
