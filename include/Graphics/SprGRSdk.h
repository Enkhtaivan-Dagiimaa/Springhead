/**	\addtogroup	gpGraphics	*/
//@{

	/**
 *	@file SprGRSdk.h
 *	@brief �O���t�B�b�N�XSDK�̊�{�N���X
 */
#ifndef SPR_GRSDK_H
#define SPR_GRSDK_H

#include <Springhead.h>

namespace Spr{;

struct GRDeviceGLIf;
struct GRDebugRenderIf;

/**	@brief	�O���t�B�b�N�XSDK�̊�{�N���X�@ */
struct GRSdkIf: public ObjectIf{
	IF_DEF(GRSdk);
	virtual GRDebugRenderIf* CreateDebugRender()=0;
	virtual GRDeviceGLIf* CreateDeviceGL(int window)=0;
};
///	�O���t�B�b�N�XSDK
GRSdkIf* SPR_CDECL CreateGRSdk();
//@}
}
#endif
