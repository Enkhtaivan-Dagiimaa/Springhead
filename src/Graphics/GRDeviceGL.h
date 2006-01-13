#ifndef GRDEVICEGL_H
#define GRDEVICEGL_H

#include <SprGraphics.h>

namespace Spr{;

/**	OpenGL�ɂ��O���t�B�b�N�X�`��̎���	*/
class GRDeviceGL: public GRDevice, public GRDeviceGLIf{
	OBJECTDEF(GRDeviceGL);
	BASEIMP_OBJECT(GRDevice);
protected:
	int window;
public:
	GRDeviceGL(int w=0):window(w){}
	virtual void SetWindow(int w){window=w;}
	virtual void Print(std::ostream& os) const{GRDevice::Print(os);}
	virtual void Init();

	///	�o�b�t�@�N���A
	virtual void ClearBuffer();
	///	�����_�����O�̊J�n�O�ɌĂԊ֐�
	virtual void BeginScene();
	///	�����_�����O�̏I����ɌĂԊ֐�
	virtual void EndScene();
	///	Viewport�Ǝˉe�s���ݒ�
	virtual void Resize(Vec2f screen);
	///	���f���s���������
	virtual void MultModelMatrix(const Affinef& afw);
	///	���f���s��̍s��X�^�b�N��Push
	virtual void PushModelMatrix();
	///	���f���s��̍s��X�^�b�N��Pop
	virtual void PopModelMatrix();
	///	���f���s���ݒ�
	virtual void SetModelMatrix(const Affinef& afw);
	///	���_�s���ݒ�
	virtual void SetViewMatrix(const Affinef& afv);
	///	���e�s���ݒ�
	virtual void SetProjectionMatrix(const Affinef& afp);
	///	���_���W���w�肵�ăv���~�e�B�u��`��
	virtual void DrawDirect(TPrimitiveType ty, Vec3f* begin, Vec3f* end);
	///	���_���W�ƃC���f�b�N�X���w�肵�ăv���~�e�B�u��`��
	virtual void DrawIndexed(TPrimitiveType ty, size_t* begin, size_t* end, Vec3f* vtx);
	///	2�����e�L�X�g�̕`��
	virtual void DrawText(Vec2f pos, std::string str, const GRFont& font);
	///	�`��̍ގ��̐ݒ�
	virtual void SetMaterial(const GRMaterial& mat);
	///	�`�悷��_�E���̑����̐ݒ�
	virtual void SetLineWidth(float w);
	///	�����X�^�b�N��Push
	virtual void PushLight(const GRLight& m);
	///	�����X�^�b�N��Pop
	virtual void PopLight();
	///	�`�掞�ɐ[�x�o�b�t�@�֏������ނ��ǂ���
	virtual void SetDepthWrite(bool b);
	///	�`�掞�ɐ[�x�e�X�g���s�����ǂ���
	virtual void SetDepthTest(bool b);
	///	�[�x�e�X�g�̃e�X�g�֐�
	virtual void SetDepthFunc(TDepthFunc f);
	///	�A���t�@�u�����h�̃��[�h�ݒ�
	virtual void SetAlphaMode(TBlendFunc src, TBlendFunc dest);
};

}
#endif
