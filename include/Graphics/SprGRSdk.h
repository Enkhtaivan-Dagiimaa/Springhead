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
struct GRSceneIf;

///	@brief GRSdkDesc GRSDK�̐ݒ�DDirectX���g����OpenGL���g�����̑I���Ȃǂ������ɓ���\��D
struct GRSdkDesc{
};

/**	@brief	�O���t�B�b�N�XSDK�̊�{�N���X�@ */
struct GRSdkIf: public ObjectIf{
	IF_DEF(GRSdk);
	virtual GRDebugRenderIf* CreateDebugRender()=0;
	virtual GRDeviceGLIf* CreateDeviceGL(int window)=0;
	virtual GRSceneIf* CreateScene()=0;
	virtual size_t NChildObject() const = 0;
	virtual ObjectIf* GetChildObject(size_t i) = 0;
	virtual GRSceneIf* GetScene(size_t i)=0;
	virtual size_t NScene()=0;
};
///	�O���t�B�b�N�XSDK
GRSdkIf* SPR_CDECL CreateGRSdk();
//@}
}
#endif
