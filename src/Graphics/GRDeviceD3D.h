/**
 *	@file GRDeviceD3D.h
 *	@brief Direct3D�ɂ��O���t�B�b�N�X�`��̎����@�@
 */
#if 0
#ifndef GRDEVICED3D_H
#define GRDEVICED3D_H

#include <SprGraphics.h>

namespace Spr{;

/**	Direct3D�ɂ��O���t�B�b�N�X�`��̎���	*/
class GRDeviceD3D: public GRDevice, public GRDeviceD3DIf{
	OBJECTDEF(GRDeviceD3D);
	BASEIMP_OBJECT(GRDevice);
public:
	virtual void DrawDirect(TPrimitiveType ty, Vec3f* begin, Vec3f* end);
	virtual void DrawIndexed(TPrimitiveType ty, size_t* begin, size_t* end, Vec3f* vtx);
	virtual void SetMaterial(const GRMaterial& mat);
	virtual void Init();
};

}
#endif
#endif