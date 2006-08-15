/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_GRRENDER_H
#define SPR_GRRENDER_H

#include <float.h>
#include <Physics/SprPHSolid.h>
#include <Physics/SprPHScene.h>
#include <Collision/SprCDShape.h>
#include <Graphics/SprGRVertex.h>
#include <Graphics/SprGRFrame.h>

namespace Spr{;

/**	\addtogroup	gpGraphics	*/
//@{

/**
 *	@file SprGRRender.h
 *	@brief �O���t�B�b�N�X�����_���[�̊�{�N���X�A��{�C���^�t�F�[�X
 */

/**	@brief	�e�L�X�g�`��̃t�H���g */
class SPR_DLL GRFont{
public:
	int height;				///<	�t�H���g�̍���
	int width;				///<	���ϕ�����
	int weight;				///<	�t�H���g�̑����i0 - 900�j
	std::string face;		///<	�^�C�v�t�F�C�X
	unsigned long color;	///<	�t�H���g�̐F
	bool bItalic;			///<	�C�^���b�N��
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
	GRFont& operator=(const GRFont& rhs) {
		if (this==&rhs)	return *this;
		height	= rhs.height;
		width	= rhs.width;
		weight	= rhs.weight;
		face	= rhs.face;
		color	= rhs.color;
		bItalic	= rhs.bItalic;
		return *this;
	}
};
	
/**	@brief	����		*/
struct GRLightDesc{
    Vec4f ambient;		///<	����
    Vec4f diffuse;		///<	�g�U��
    Vec4f specular;		///<	���ʌ�
    Vec4f position;		///<	�����ʒu�iw=1.0�œ_�����Aw=0.0�ŕ��s�����j
    float range;		///<	�����͂��͈́idevice��DirectX�̏ꍇ�̂ݗ��p�\�j
	/**
	 *	@name	�����p�����[�^
	 *�@�@ ������ݒ肷�邱�ƂŌ��̌�����\���ł���B \n
	 *�@�@ �����̃p�����[�^��ݒ肷��ƁA�������痣���قǍ����ɂ����ʂ͏������Ȃ�B \n
	 *�@�@ �f�t�H���g�ł́A�����Ȃ��ɐݒ肳��Ă���B \n
	 *�@�@ ���s�����̏ꍇ�A�����͂����Ȃ��B \n
	 *�@�@�@�@ �����W�� = 1/( att0 + att1 * d + att2 * d^2)�@�@�@d:����
	 *	@{
	 */
	float attenuation0;	///<	��茸����
    float attenuation1;	///<	���`������
    float attenuation2;	///<	2��������
	/** @} */	// end of �����p�����[�^

	Vec3f spotDirection;///<	�X�|�b�g���C�g�̌���
    float spotFalloff;	///<	�X�|�b�g���C�g�̉~�����ł̌�����(�傫���قǋ}�s) 0..��
	float spotInner;	///<	�X�|�b�g���C�g�̒��S����(�����R�[��)�idevice��DirectX�̏ꍇ�̂ݗ��p�\�j 0..spotCutoff
	float spotCutoff;	///<	�X�|�b�g���C�g�̍L����p�x(�x)(�O���R�[��) 0..��(pi)
	GRLightDesc(){
		ambient = Vec4f(0.0, 0.0, 0.0, 1.0);
		diffuse = Vec4f(1.0, 1.0, 1.0, 1.0);
		specular = Vec4f(1.0, 1.0, 1.0, 1.0);
		position = Vec4f(0.0, 0.0, 1.0, 1.0);
		range = FLT_MAX;
		attenuation0 = 1.0f;
		attenuation1 = 0.0f;
		attenuation2 = 0.0f;
		spotDirection = Vec3f(0.0, -1.0, 0.0);
		spotFalloff  = 0.0f;
		spotInner    = 0.0f;
		spotCutoff   = 180.0f;
	}
};

/** @brief �����̊�{�N���X		*/
struct GRLightIf: public GRVisualIf{
	IF_DEF(GRLight);
};

/**	@brief	�ގ�	*/
struct GRMaterialDesc{
	Vec4f ambient;					///<	�����ɑ΂��锽�˗�
	Vec4f diffuse;					///<	�g�U���ɑ΂��锽�˗�
	Vec4f specular;					///<	���ʌ��ɑ΂��锽�˗�
	Vec4f emissive;					///<	���ˋP�x
	float power;					///<	���ʔ��˂̋��x�A���ʌW��
	std::string	texname;			///<	�e�N�X�`���t�@�C����

	GRMaterialDesc(){
		ambient = Vec4f(0.2, 0.2, 0.2, 1.0);
		diffuse = Vec4f(0.8, 0.8, 0.8, 1.0);
		specular = Vec4f(1.0, 1.0, 1.0, 1.0);
		emissive = Vec4f(0.0, 0.0, 0.0, 1.0);
		power = 20.0;
	}
	GRMaterialDesc(Vec4f a, Vec4f d, Vec4f s, Vec4f e, float p):
		ambient(a), diffuse(d), specular(s), emissive(e), power(p){}
	GRMaterialDesc(Vec4f c, float p):
		ambient(c), diffuse(c), specular(c), emissive(c), power(p){}
	GRMaterialDesc(Vec4f c):
		ambient(c), diffuse(c), specular(c), emissive(c), power(0.0f){}
	/**	W()�v�f�́A�A���t�@�l(0.0�`1.0�œ����x��\��). 1.0���s������\��.
		material��W()�v�f�𔻒肵�āA�s�������̂��A�������̂��𔻒肷��. 
		�����ȃI�u�W�F�N�g��`���Ƃ��A�����̂��̂��珇�Ԃɕ`�悵�Ȃ��ƁA�Ӑ}�ɔ��������ʂƂȂ�. */
	bool IsOpaque() const {		
		return ambient.W() >= 1.0 && diffuse.W() >= 1.0 && specular.W() >= 1.0 && emissive.W() >= 1.0;
	}	
};

/** @brief�@�ގ��̊�{�N���X�@�@	*/
struct GRMaterialIf: public GRVisualIf{
	IF_DEF(GRMaterial);
	virtual bool IsOpaque() const = 0;
};

/** @brief �e�N�X�`��				*/
struct GRTextureDesc{
	std::string		filename;		///<	�e�N�X�`���t�@�C����
	int				width;			///<	�e�N�X�`���T�C�Y
	int				height;			///<	�e�N�X�`���T�C�Y
	unsigned int	id;				///<	�e�N�X�`��ID
	unsigned char*	data;			///<	�e�N�X�`���f�[�^(RGB)
	GRTextureDesc(): filename(), width(0), height(0), id(0), data(NULL){}
	GRTextureDesc(std::string fn): filename(fn), width(0), height(0), id(0), data(NULL){}
};
/** @brief �e�N�X�`���̊�{�N���X	*/
struct GRTextureIf: public GRVisualIf{
	IF_DEF(GRTexture);
};

/**	@brief	�J�����̏��			*/
struct GRCameraDesc{
	Vec2f size;				///<	�X�N���[���̃T�C�Y
	Vec2f center;			///<	�J��������̃X�N���[���̂���
	float front, back;		///<	���_����N���b�v�ʂ܂ł̑��΋����i���̒l�Ŏw��j
	GRCameraDesc();
	GRCameraDesc(Vec2f sz, Vec2f c, float f, float b): size(sz), center(c), front(f), back(b) {}
};
struct GRCameraIf: public GRVisualIf{
	IF_DEF(GRCamera);
};

struct GRDeviceIf;

/**	@brief	�O���t�B�b�N�X�����_���[�̊�{�N���X�i���[�U�C���^�t�F�[�X�j */
struct GRRenderBaseIf: public ObjectIf{
	IF_DEF(GRRenderBase);

	///	�v���~�e�B�u�̎��
	enum TPrimitiveType {
		POINTS,
		LINES,
		LINE_STRIP,
		TRIANGLES,
		TRIANGLE_STRIP,
		TRIANGLE_FAN,
		QUADS
	};
	/**
	 *	@name	�f�v�X�o�b�t�@�@�ɗp���锻�����
	 *	@{
	 */
	enum TDepthFunc{
		DF_NEVER,		///<	�V����Z�l�Ɋ֌W�Ȃ��X�V���Ȃ�
		DF_LESS,		///<	�V����Z�l����������΍X�V����idefault)
		DF_EQUAL,		///<	�V����Z�l����������Ε`�����
		DF_LEQUAL,		///<	�V����Z�l���傫���Ȃ���Ε`�����
		DF_GREATER,		///<	�V����Z�l���傫����Ε`�����
		DF_NOTEQUAL,	///<	�V����Z�l�Ɠ������Ȃ���Ε`�����
		DF_GEQUAL,		///<	�V����Z�l���������Ȃ���Ε`�����
		DF_ALWAYS		///<	�V����Z�l�Ɋ֌W�Ȃ��X�V����
	};
	/** @} */	
	/**
	 *	@name	�A���t�@�u�����f�B���O�̍����W��
	 *�@�@ SRC�����ꂩ��`�悳��鍇�����������F�ADEST�����łɕ`���ꂽ�J���[�o�b�t�@�̐F    \n
	 *�@�@�@�@ �������� =SRC * SRC�̃u�����f�B���O�W�� + DEST * DEST�̃u�����f�B���O�W�� 			\n
	 *�@�@ �������ABF_BOTHINVSRCALPHA�ABF_BLENDFACTOR�ABF_INVBLENDFACTOR �Ɋւ��ẮA			\n
	 *�@�@ device��DirectX�̂Ƃ��̂ݗ��p�\�B�w�肵�Ă�BF_ZERO�Ƃ��Ĉ����B
	 *	@{
	 */
	enum TBlendFunc{
		BF_ZERO,				///<	(0,0,0,0)	
		BF_ONE,					///<	(1,1,1,1)
		BF_SRCCOLOR,			///<	(Rs, Gs, Bs, As)
		BF_INVSRCCOLOR,			///<	(1-Rs, 1-Gs, 1-Bs, 1-As) 
		BF_SRCALPHA,			///<	(As, As, As, As) 
		BF_INVSRCALPHA,			///<	(1-As, 1-As, 1-As, 1-As)
		BF_DESTALPHA,			///<	(Ad, Ad, Ad, Ad)  
		BF_INVDESTALPHA,		///<	(1-Ad, 1-Ad, 1-Ad, 1-Ad) 
		BF_DESTCOLOR,			///<	(Rd, Gd, Bd, Ad)
		BF_INVDESTCOLOR,		///<	(1-Rd, 1-Gd, 1-Bd, 1-Ad) 
		BF_SRCALPHASAT,			///<	(f, f, f, 1)�Af = min(As, 1-Ad) 
		BF_BOTHINVSRCALPHA,		///<	SRC�u�����f�B���O�W����(1-As, 1-As, 1-As, 1-As)�BDST�u�����f�B���O�W����(As, As, As, As)�B
		BF_BLENDFACTOR,			///<	�t���[���o�b�t�@�������Ɏg����萔�F�ō���
		BF_INVBLENDFACTOR		///<	�t���[���o�b�t�@�������Ɏg����萔�F�𔽓]�����F�ō���
	};
	/** @} */

	///	�r���[�|�[�g�̐ݒ�
	virtual void SetViewport(Vec2f pos, Vec2f sz)=0;
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

	///	���_�t�H�[�}�b�g�̎w��
	virtual void SetVertexFormat(const GRVertexElement* e)=0;
	///	���_�V�F�[�_�[�̎w��	API�����D�����ȂǗv���� 2006.6.7 hase
	virtual void SetVertexShader(void* shader){}
	///	���_���w�肵�ăv���~�e�B�u��`��
	virtual void DrawDirect(TPrimitiveType ty, void* vtx, size_t count, size_t stride=0)=0;
	///	���_�ƃC���f�b�N�X���w�肵�ăv���~�e�B�u��`��
	virtual void DrawIndexed(TPrimitiveType ty, size_t* idx, void* vtx, size_t count, size_t stride=0)=0;
 	///	���_�̐������Ƃ̔z����w�肵�āC�v���~�e�B�u��`��
	virtual void DrawArrays(TPrimitiveType ty, GRVertexArray* arrays, size_t count){}
 	///	�C���f�b�N�X�ƒ��_�̐������Ƃ̔z����w�肵�āC�v���~�e�B�u��`��
	virtual void DrawArrays(TPrimitiveType ty, size_t* idx, GRVertexArray* arrays, size_t count){}
	
	///	DiplayList�̍쐬
	virtual int CreateList(TPrimitiveType ty, void* vtx, size_t count, size_t stride=0)=0;
	virtual int CreateList(GRMaterialDesc& mat, unsigned int texid, 
						   TPrimitiveType ty, void* vtx, size_t count, size_t stride=0)=0;
	///	DiplayList�̍쐬
	virtual int CreateIndexedList(TPrimitiveType ty, size_t* idx, void* vtx, size_t count, size_t stride=0)=0;
	virtual int CreateIndexedList(GRMaterialDesc& mat, unsigned int texid, 
								  TPrimitiveType ty, size_t* idx, void* vtx, size_t count, size_t stride=0)=0;
	///	DisplayList�̕\��
	virtual void DrawList(int i)=0;
	///	DisplayList�̉��
	virtual void ReleaseList(int i)=0;
	///	2�����e�L�X�g�̕`��@�@ Windows��(VC)�ł̂�font���T�|�[�g���A���̊��ł�font���w�肵�Ă����p����Ȃ��B
	virtual void DrawFont(Vec2f pos, const std::string str, const GRFont& font=0)=0;
	///	3�����e�L�X�g�̕`��@�@ Windows��(VC)�ł̂�font���T�|�[�g���A���̊��ł�font���w�肵�Ă����p����Ȃ��B	
	virtual void DrawFont(Vec3f pos, const std::string str, const GRFont& font=0)=0;
	///	�`��̍ގ��̐ݒ�
	virtual void SetMaterial(const GRMaterialDesc& mat)=0;
	virtual void SetMaterial(const GRMaterialIf* mat)=0;
	///	�`�悷��_�E���̑����̐ݒ�
	virtual void SetLineWidth(float w)=0;
	///	�����X�^�b�N��Push
	virtual void PushLight(const GRLightDesc& light)=0;
	virtual void PushLight(const GRLightIf* light)=0;
	///	�����X�^�b�N��Pop
	virtual void PopLight()=0;
	///	�f�v�X�o�b�t�@�ւ̏������݂�����/�֎~����
	virtual void SetDepthWrite(bool b)=0;
	///	�f�v�X�e�X�g��L��/�����ɂ���
	virtual void SetDepthTest(bool b)=0;
	///	�f�v�X�o�b�t�@�@�ɗp���锻��������w�肷��
	virtual void SetDepthFunc(TDepthFunc f)=0;
	/// �A���t�@�u�����f�B���O��L��/�����ɂ���
	virtual void SetAlphaTest(bool b)=0;
	///	�A���t�@�u�����f�B���O�̃��[�h�ݒ�(SRC�̍����W��, DEST�����W��)
	virtual void SetAlphaMode(TBlendFunc src, TBlendFunc dest)=0;
	/// �e�N�X�`�������[�h���A�e�N�X�`���I�u�W�F�N�g���쐬����
	virtual void LoadTexture(GRTextureDesc& tex)=0;
	virtual void LoadTexture(GRTextureIf* tex)=0;
};

/**	@brief	�O���t�B�b�N�X�����_���[�̊�{�N���X�i�f�o�C�X�̐ݒ�A�J�����̐ݒ�j */
struct GRRenderIf: public GRRenderBaseIf{
	IF_DEF(GRRender);
	///	�f�o�C�X�̐ݒ�
	virtual void SetDevice(GRDeviceIf* dev)=0;
	///	�J�����̐ݒ�
	virtual void SetCamera(const GRCameraDesc& cam)=0;
	///	�X�N���[��(�E�B���h�E)�T�C�Y�ύX����Viewport�Ǝˉe�s���ݒ�
	virtual void Reshape(Vec2f pos, Vec2f screenSize)=0;
};

/**	@brief	�O���t�B�b�N�X�����_���[�̃f�o�C�X�N���X�DOpenGL��DirectX�̃��b�p */
struct GRDeviceIf: public GRRenderBaseIf{
	IF_DEF(GRDevice);
	///	������
	virtual void Init()=0;
	///	�f�o�b�O�p�̏�ԃ��|�[�g
	virtual void Print(std::ostream& os) const=0;
};

/**	@brief	OpenGL�̃����_���[��{�N���X */
struct GRDeviceGLIf: public GRDeviceIf{
	IF_DEF(GRDeviceGL);
	/// �E�B���h�EID��ݒ肷��
	virtual void SetWindow(int w)=0;
};

/**	@brief	DirectX�̃����_���[��{�N���X */
struct GRDeviceD3DIf: public GRDeviceIf{
	IF_DEF(GRDeviceD3D);
};

/**	@brief	�f�o�b�O��񃌃��_���[�̊�{�N���X */
struct GRDebugRenderIf:public GRRenderIf{
	IF_DEF(GRDebugRender);
	///	�����_���[�ŗp�ӂ��Ă���ގ�(24���)
	enum MaterialSample {
		RED,
		GREEN,
		BLUE,
		YELLOW,
		CYAN,
		MAGENTA,
		WHITE,
		GRAY,
		ORANGE,
		BLOWN,
		LIGHT_BLUE,
		MIDIUM_PURPLE,
		DARK_GREEN,
		DARK_VIOLET,
		DARK_CYAN,
		GREEN_YELLOW,
		LIME_GREEN,
		INDIAN_RED,
		INDIGO,
		GREEN_INDIGO,
		OLIVE_GREEN,
		NAVY_BLUE,
		TURQUOISE_BLUE,
		EMERALD_GREEN
	};

	/// �V�[�����̑S�ẴI�u�W�F�N�g�������_�����O����
	virtual void DrawScene(PHSceneIf* scene)=0;
	/// ���̂������_�����O����
	virtual void DrawSolid(PHSolidIf* so)=0;
	/// �ʂ������_�����O����
	virtual void DrawFace(CDFaceIf* face, Vec3f * base)=0;
	/// �w�肵���ގ�(�}�e���A���T���v��)�����蓖�Ă�
	virtual void SetMaterialSample(MaterialSample matname)=0;
};

//@}


}
#endif
