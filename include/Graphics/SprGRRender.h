#ifndef SPR_GRRENDER_H
#define SPR_GRRENDER_H

#include <float.h>
#include <Physics/SprPHSolid.h>

namespace Spr{;

/**	@struct	GRFont
    @brief	�e�L�X�g�`��̃t�H���g */
class SPR_DLL GRFont{
public:
	int height;
	int width;
	int weight;
	std::string face;
	unsigned long color;
	bool bItalic;
	GRFont(int h=20, const char* f=NULL){
		height=h; width=0;
		face=f ? f : "";
		weight = 400; color=0xffffffff; bItalic=false;
	}
	void SetColor(unsigned long c){ color = c; }
	bool operator < (GRFont& f){
		if (face < f.face) return true;
		if (height < f.height) return true;
		if (color < f.color) return true;
		return false;
	}
};

/**	@struct	GRLight
    @brief	����		*/
struct GRLight{
    Vec4f diffuse;		///<	�g�U��
    Vec4f specular;		///<	���ʌ�
    Vec4f ambient;		///<	����
    Vec4f position;		///<	�����̈ʒu�Cw=0�Ƃ���Ɩ�����(��������)�ɂȂ�D
    float range;		///<	�����͂��͈�
    ///@name	�������DAtten = 1/( att0 + att1 * d + att2 * d^2)
	//@{
	float attenuation0;	///<	att0�@�@ 0..��
    float attenuation1;	///<	att1�@�@ 0..��
    float attenuation2;	///<	att2�@�@ 0..��
	//@}
	Vec3f spotDirection;///<	�X�|�b�g���C�g�̏ꍇ�̌���
    float spotFalloff;	///<	�����̑���(�傫���قǋ}�s)�@�@�@ 0..��
	float spotInner;	///<	�X�|�b�g���C�g�̒��S�����@�@�@�@ 0..spotCutoff
	float spotCutoff;	///<	�X�|�b�g���C�g�̌���������͈́@ 0..��(pi)
	GRLight(){
		range = FLT_MAX;
		attenuation0 = 0.0f;
		attenuation1 = 0.0f;
		attenuation2 = 0.0f;
		spotFalloff = 0.0f;
		spotInner = 0.0f;
		spotCutoff = 0.0f;
	}
};

/**	@struct	GRMaterial
    @brief	�O���t�B�b�N�X�̍ގ� */
struct GRMaterial{
	Vec4f diffuse;					///<	�g�U���ɑ΂��锽�˗�
	Vec4f ambient;					///<	�����ɑ΂��锽�˗�
	Vec4f specular;					///<	���ʌ��ɑ΂��锽�˗�
	//Vec4f emissive;				///<	���ˋP�x
	Vec4f emission;					///<	���ˋP�x
	float power;					///<	
	//float shininess				///<	���ʔ��˂̋��x�A���ʌW��
	std::string texture;			///<	�e�N�X�`���t�@�C����
	GRMaterial(){ power = 0.0f; }
	GRMaterial(Vec4f d, Vec4f a, Vec4f s, Vec4f e, float p):
		diffuse(d), ambient(a), specular(s), emission(e), power(p){}
	GRMaterial(Vec4f c, float p):
		diffuse(c), ambient(c), specular(c), emission(c), power(p){}
	GRMaterial(Vec4f c):
		diffuse(c), ambient(c), specular(c), emission(c), power(0.0f){}
	/**	W()�v�f�́A�A���t�@�l(0.0�`1.0�œ����x��\��). 1.0���s������\��.
		material��W()�v�f�𔻒肵�āA�s�������̂��A�������̂��𔻒肷��. 
		�����ȃI�u�W�F�N�g��`���Ƃ��A�����̂��̂��珇�Ԃɕ`�悵�Ȃ��ƁA�Ӑ}�ɔ��������ʂƂȂ�. */
	bool IsOpaque(){		
		return diffuse.W() >= 1.0 && ambient.W() >= 1.0 && specular.W() >= 1.0 && emission.W() >= 1.0;
	}
};

/**	@struct	GRCamera
    @brief	�J�����̏�� */
struct GRCamera{
	Vec2f size;				///<	�X�N���[���̃T�C�Y
	Vec2f center;			///<	�J��������̃X�N���[���̂���
	float front, back;		///<	���_����N���b�v�ʂ܂ł̑��΋����i���̒l�Ŏw��j
	GRCamera(Vec2f initSize=Vec2f(480.0, 360.0), 
				Vec2f initCenter=Vec2f(0.0, 0.0),
				float initFront=1.0,
				float initBack=5000.0) 
			: size(initSize), center(initCenter), front(initFront), back(initBack) {}
};

struct GRDeviceIf;

/**	@struct	GRRenderBaseIf
    @brief	�O���t�B�b�N�X�����_���[�̊�{�N���X */
struct GRRenderBaseIf: public ObjectIf{
	IF_DEF(GRRenderBase);
	///	�v���~�e�B�u�̎��
	enum TPrimitiveType {
		POINTS,
		LINES,
		LINE_STRIP,
		TRIANGLES,
		TRIANGLE_STRIP,
		TRIANGLE_FAN
	};
	///	Z�o�b�t�@�e�X�g�֐�
	enum TDepthFunc{
		DF_NEVER, DF_LESS, DF_EQUAL, DF_LEQUAL, DF_GREATER, DF_NOTEQUAL, 
		DF_GEQUAL, DF_ALWAYS
	};
	///	�A���t�@�u�����h�֐�
	enum TBlendFunc{
		BF_ZERO = 1,
		BF_ONE = 2,
		BF_SRCCOLOR = 3,
		BF_INVSRCCOLOR = 4,
		BF_SRCALPHA = 5,
		BF_INVSRCALPHA = 6,
		BF_DESTALPHA = 7,
		BF_INVDESTALPHA = 8,
		BF_DESTCOLOR = 9,
		BF_INVDESTCOLOR = 10,
		BF_SRCALPHASAT = 11,
		BF_BOTHSRCALPHA = 12,
		BF_BOTHINVSRCALPHA = 13,
		BF_BLENDFACTOR = 14,
		BF_INVBLENDFACTOR = 15
	};
	///	�o�b�t�@�N���A
	virtual void ClearBuffer()=0;
	///	�����_�����O�̊J�n�O�ɌĂԊ֐�
	virtual void BeginScene()=0;
	///	�����_�����O�̏I����ɌĂԊ֐�
	virtual void EndScene()=0;
	///	���f���s���������
	virtual void MultModelMatrix(const Affinef& afw)=0;
	///	���f���s��̍s��X�^�b�N��Push
	virtual void PushModelMatrix()=0;
	///	���f���s��̍s��X�^�b�N��Pop
	virtual void PopModelMatrix()=0;
	///	���f���s���ݒ�
	virtual void SetModelMatrix(const Affinef& afw)=0;
	///	���_�s���ݒ�
	virtual void SetViewMatrix(const Affinef& afv)=0;
	///	���e�s���ݒ�
	virtual void SetProjectionMatrix(const Affinef& afp)=0;
	///	���_���W���w�肵�ăv���~�e�B�u��`��
	virtual void DrawDirect(TPrimitiveType ty, Vec3f* begin, Vec3f* end)=0;
	///	���_���W�ƃC���f�b�N�X���w�肵�ăv���~�e�B�u��`��
	virtual void DrawIndexed(TPrimitiveType ty, size_t* begin, size_t* end, Vec3f* vtx)=0;
	///	2�����e�L�X�g�̕`��
	virtual void DrawText(Vec2f pos, std::string str, const GRFont& font)=0;
	///	�`��̍ގ��̐ݒ�
	virtual void SetMaterial(const GRMaterial& mat)=0;
	///	�`�悷��_�E���̑����̐ݒ�
	virtual void SetLineWidth(float w)=0;
	///	�����X�^�b�N��Push
	virtual void PushLight(const GRLight& m)=0;
	///	�����X�^�b�N��Pop
	virtual void PopLight()=0;
	///	�`�掞�ɐ[�x�o�b�t�@�֏������ނ��ǂ���
	virtual void SetDepthWrite(bool b)=0;
	///	�`�掞�ɐ[�x�e�X�g���s�����ǂ���
	virtual void SetDepthTest(bool b)=0;
	///	�[�x�e�X�g�̃e�X�g�֐�
	virtual void SetDepthFunc(TDepthFunc f)=0;
	///	�A���t�@�u�����h�̃��[�h�ݒ�
	virtual void SetAlphaMode(TBlendFunc src, TBlendFunc dest)=0;
};

/**	@struct	GRRenderIf
    @brief	�����_���̊�{�N���X */
struct GRRenderIf: public GRRenderBaseIf{
	IF_DEF(GRRender);
	virtual void SetDevice(GRDeviceIf* dev)=0;
	virtual void SetCamera(GRCamera& cam)=0;
};

/**	@struct	GRDeviceIf
    @brief	�O���t�B�b�N�X�����_���[�̃f�o�C�X�N���X�DOpenGL��DirectX�̃��b�p */
struct GRDeviceIf: public GRRenderBaseIf{
	IF_DEF(GRDevice);
	///	������
	virtual void Init()=0;
	///	�f�o�b�O�p�̏�ԃ��|�[�g
	virtual void Print(std::ostream& os) const=0;
};

/**	@struct	GRDeviceGLIf
    @brief	OpenGL�̃����_���[��{�N���X */
struct GRDeviceGLIf: public GRDeviceIf{
	IF_DEF(GRDeviceGL);
	/// �E�B���h�EID��ݒ肷��
	virtual void SetWindow(int w)=0;
};

/**	@struct	GRDeviceD3DIf
    @brief	DirectX�̃����_���[��{�N���X */
struct GRDeviceD3DIf: public GRDeviceIf{
	IF_DEF(GRDeviceD3D);
};

/**	@struct	GRDebugRenderIf
    @brief	�f�o�b�O��񃌃��_���[�̊�{�N���X */
struct GRDebugRenderIf:public GRRenderIf{
	IF_DEF(GRDebugRender);
	///	Viewport�Ǝˉe�s���ݒ�
	virtual void Reshape(Vec2f screen)=0;
	/// ���̂������_�����O����
	virtual void DrawSolid(PHSolidIf* so)=0;
	/// �ʂ������_�����O����
	virtual void DrawFace(CDFaceIf* face, Vec3f * base)=0;
};


}
#endif
