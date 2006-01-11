#ifndef SPR_GRRENDER_H
#define SPR_GRRENDER_H

#include <Springhead.h>

namespace Spr{;

struct GRDeviceIf;

/**	�O���t�B�b�N�X�����_���[�̊�{�N���X	*/
struct GRRenderIf: public ObjectIf{
	IF_DEF(GRRender);
	///	�v���~�e�B�u�̎��
	enum TPrimitiveType {
		POINTS,
		LINES,
		LINESTRIP,
		TRIANGLES,
		TRIANGLESTRIP,
		TRIANGLEFAN
	};
	///	�v���~�e�B�u�̕`��
	virtual void DrawDirect(TPrimitiveType ty, Vec3f* begin, Vec3f* end)=0;
	///	�C���f�b�N�X�ɂ��v���~�e�B�u�̕`��
	virtual void DrawIndexed(TPrimitiveType ty, size_t* begin, size_t* end, Vec3f* vtx)=0;
	///	�ގ��̐ݒ�
	virtual void SetMaterial(const GRMaterial& mat)=0;
	///	�`��f�o�C�X�̐ݒ�
	virtual void SetDevice(GRDeviceIf* dev)=0;
};
/**	�O���t�B�b�N�X�����_���[�̃f�o�C�X�N���X�DOpenGL��DirectX�̃��b�p	*/
struct GRDeviceIf: public ObjectIf{
	IF_DEF(GRDevice);
	typedef GRRenderIf::TPrimitiveType TPrimitiveType;
	///	�v���~�e�B�u�̕`��
	virtual void DrawDirect(TPrimitiveType ty, Vec3f* begin, Vec3f* end)=0;
	///	�C���f�b�N�X�ɂ��v���~�e�B�u�̕`��
	virtual void DrawIndexed(TPrimitiveType ty, size_t* begin, size_t* end, Vec3f* vtx)=0;
	///	�ގ��̐ݒ�
	virtual void SetMaterial(const GRMaterial& mat)=0;
};

/**	�f�o�b�O��񃌃��_���[	*/
struct GRDebugRenderIf:public GRRenderIf{
	IF_DEF(GRDebugRender);
	virtual void DrawSolid(PHSolidIf* so)=0;
	virtual void DrawFace(CDFaceIf* face, Vec3f * base)=0;
};


}
#endif
