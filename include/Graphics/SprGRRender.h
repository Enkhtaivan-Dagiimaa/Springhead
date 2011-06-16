/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_GRRENDER_H
#define SPR_GRRENDER_H

#include <Graphics/SprGRFrame.h>
#include <Graphics/SprGRVertex.h>
#include <Graphics/SprGRShader.h>

namespace Spr{;

/**	\addtogroup	gpGraphics	*/
//@{

/**
 *	@file SprGRRender.h
 *	@brief �O���t�B�b�N�X�����_���[�̃C���^�t�F�[�X�A��{�C���^�t�F�[�X
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
		height=h;
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
	
/** @brief �����̃C���^�t�F�[�X		*/
struct GRLightIf: public GRVisualIf{
	SPR_IFDEF(GRLight);
};
/**	@brief	����		*/
struct GRLightDesc : GRVisualDesc{
	SPR_DESCDEF(GRLight);
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



/** @brief�@�ގ��̃C���^�t�F�[�X�@�@	*/
struct GRMaterialIf: public GRVisualIf{
	SPR_IFDEF(GRMaterial);
	bool IsOpaque() const;
};
/**	@brief	�ގ�	*/
struct GRMaterialDesc : GRVisualDesc{
	SPR_DESCDEF(GRMaterial);
	Vec4f ambient;					///<	�����ɑ΂��锽�˗�
	Vec4f diffuse;					///<	�g�U���ɑ΂��锽�˗�
	Vec4f specular;					///<	���ʌ��ɑ΂��锽�˗�
	Vec4f emissive;					///<	���ˋP�x
	float power;					///<	���ʔ��˂̋��x�A���ʌW��
	std::string	texname;			///<	�e�N�X�`���t�@�C�����i�t�@�C���̕ۑ����鑊�΃p�X���j

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
	bool Is3D() const ;
};

struct GRFrameIf;
struct GRCameraDesc;
///	�J�����̃C���^�t�F�[�X
struct GRCameraIf: public GRVisualIf{
	SPR_IFDEF(GRCamera);
	GRFrameIf* GetFrame();
	void SetFrame(GRFrameIf* fr);
};
/**	@brief	�J�����̏��			*/
struct GRCameraDesc : GRVisualDesc{
	SPR_DESCDEF(GRCamera);
	Vec2f size;				///<	�X�N���[���̃T�C�Y
	Vec2f center;			///<	�J��������̃X�N���[���̂���
	float front, back;		///<	���_����N���b�v�ʂ܂ł̑��΋����i���̒l�Ŏw��j
	//GRCameraDesc():center(Vec2f()), size(Vec2f(0.2f, 0)), front(0.1f), back(500.0f){}
	GRCameraDesc(Vec2f sz = Vec2f(0.2f, 0.0f), Vec2f c = Vec2f(), float f = 0.1f, float b = 500.0f):
		size(sz), center(c), front(f), back(b) {}
};

struct GRDeviceIf;
typedef unsigned GRHandler;

/**	@brief	�O���t�B�b�N�X�����_���[�̃C���^�t�F�[�X�i���[�U�C���^�t�F�[�X�j */
struct GRRenderBaseIf: public ObjectIf{
	SPR_IFDEF(GRRenderBase);

	///	�v���~�e�B�u�̎��
	enum TPrimitiveType {
		POINTS,
		LINES,
		LINE_STRIP,
		LINE_LOOP,
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

	///	�����_���[�ŗp�ӂ��Ă���ގ�(24���)
	enum TMaterialSample {
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
		MEDIUM_PURPLE,
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
		EMERALD_GREEN,
		MATERIAL_SAMPLE_END
	};

	///	�r���[�|�[�g�̐ݒ�
	void SetViewport(Vec2f pos, Vec2f sz);
	///	�o�b�t�@�N���A
	void ClearBuffer();
	///	�o�b�t�@�̓���ւ��i�\���j
	void SwapBuffers();
	/// �w�i�F�̎擾
	void GetClearColor(Vec4f& color);
	/// �w�i�F�̐ݒ�
	void SetClearColor(const Vec4f& color);
	///	�����_�����O�̊J�n�O�ɌĂԊ֐�
	void BeginScene();
	///	�����_�����O�̏I����ɌĂԊ֐�
	void EndScene();
	///	�J�����g�̎��_�s���afv�Œu��������
	void SetViewMatrix(const Affinef& afv);
	///	�J�����g�̎��_�s����擾����
	void GetViewMatrix(Affinef& afv);
	///	�J�����g�̓��e�s���afp�Œu��������
	void SetProjectionMatrix(const Affinef& afp);
	///	�J�����g�̓��e�s����擾����
	void GetProjectionMatrix(Affinef& afp);
	///	�J�����g�̃��f���s���afw�Œu��������
	void SetModelMatrix(const Affinef& afw);
	///	�J�����g�̃��f���s����擾����
	void GetModelMatrix(Affinef& afw);
	///	�J�����g�̃��f���s��ɑ΂���afw���|����
	void MultModelMatrix(const Affinef& afw);
	///	�J�����g�̃��f���s������f���s��X�^�b�N�֕ۑ�����
	void PushModelMatrix();
	///	���f���s��X�^�b�N������o���A�J�����g�̃��f���s��Ƃ���
	void PopModelMatrix();
	/// �u�����h�ϊ��s��̑S�v�f���폜����
	void ClearBlendMatrix();
	/// �u�����h�ϊ��s���ݒ肷��
	bool SetBlendMatrix(const Affinef& afb, unsigned int id=0);
	///	���_�t�H�[�}�b�g�̎w��
	void SetVertexFormat(const GRVertexElement* e);
	///	���_�V�F�[�_�[�̎w��	API�����D�����ȂǗv���� 2006.6.7 hase
	void SetVertexShader(void* shader);

	///	���_���w�肵�ăv���~�e�B�u��`��
	void DrawDirect(GRRenderBaseIf::TPrimitiveType ty, void* vtx, size_t count, size_t stride=0);
	///	���_�ƃC���f�b�N�X���w�肵�ăv���~�e�B�u��`��
	void DrawIndexed(GRRenderBaseIf::TPrimitiveType ty, size_t* idx, void* vtx, size_t count, size_t stride=0);
 	///	���_�̐������Ƃ̔z����w�肵�āC�v���~�e�B�u��`��
	void DrawArrays(GRRenderBaseIf::TPrimitiveType ty, GRVertexArray* arrays, size_t count);
 	///	�C���f�b�N�X�ƒ��_�̐������Ƃ̔z����w�肵�āC�v���~�e�B�u��`��
	void DrawArrays(GRRenderBaseIf::TPrimitiveType ty, size_t* idx, GRVertexArray* arrays, size_t count);

	/// ������`��
	void	DrawLine(Vec3f p0, Vec3f p1);
	/// ����`��
	void	DrawArrow(Vec3f p0, Vec3f p1, float rbar, float rhead, float lhead, int slice, bool solid);
	/// �����̂�`��
	void	DrawBox(float sx, float sy, float sz, bool solid=true);
	/// ���̂�`��
	void	DrawSphere(float radius, int slices, int stacks, bool solid=true);
	/// �~���̕`��
	void	DrawCone(float radius, float height, int slice, bool solid=true);
	/// �~���̕`��
	void	DrawCylinder(float radius, float height, int slice, bool solid=true);
	/// �J�v�Z���̕`��
	void	DrawCapsule(float radius, float height, int slice=20, bool solid=true);
	/// ���~���̕`��
	void	DrawRoundCone(float rbottom, float rtop, float height, int slice=20, bool solid=true);
	/**	�O���b�h��`��
		@param	size		��
		@param	slice		������
		@param	lineWidth	���̑���
		xy�ʏ�ɃO���b�h��`�悷��
	 */
	void	DrawGrid(float size, int slice, float lineWidth = 1.0f);
	
	///	�f�B�X�v���C���X�g�̍쐬(�L�^�J�n)
	int StartList();
	///	�f�B�X�v���C���X�g�̏I��(�L�^�I��)
	void EndList();
	///	�f�B�X�v���C���X�g�̕`��
	void DrawList(int i);
	///	�f�B�X�v���C���X�g�̉��
	void ReleaseList(int i);

	/// �t�H���g�̐ݒ�(Windows����)
	void SetFont(const GRFont& font);
	
	/** @brief	2�����e�L�X�g�̕`��
	 */
	void DrawFont(Vec2f pos, const std::string str);

	/** @brief	3�����e�L�X�g�̕`��
		@param  pos		World���W�n�ł̏����n�߂̈ʒu�C�r���[�|�[�g���͂ݏo���Ɖ����\������Ȃ��Ȃ�̂Œ��ӁD
		@param	str		����������(bitmapfont�ŏ����镨�̂�)
		@param	font	�t�H���g�Ɋւ���ݒ�
		Windows��(VC)�ł̂�font���T�|�[�g���A���̊��ł�font���w�肵�Ă����p����Ȃ��B	
	 */
	void DrawFont(Vec3f pos, const std::string str);

	///	�`��}�e���A���̐ݒ�i�f�X�N���v�^�Łj
	void SetMaterial(const GRMaterialDesc& mat);
	///	�`��}�e���A���̐ݒ�i�I�u�W�F�N�g�Łj
	void SetMaterial(const GRMaterialIf* mat);
	/// �`��}�e���A���̐ݒ�i�\��}�e���A���̖��O�Ŏw��j
	void SetMaterial(int matname);

	///	�`�悷��_�E���̑����̐ݒ�
	void SetLineWidth(float w);
	///	�����X�^�b�N��Push
	void PushLight(const GRLightDesc& light);
	void PushLight(const GRLightIf* light);
	///	�����X�^�b�N��Pop
	void PopLight();
	/// �ݒ肳��Ă�������̐�
	int	 NLights();
	///	�f�v�X�o�b�t�@�ւ̏������݂�����/�֎~����
	void SetDepthWrite(bool b);
	///	�f�v�X�e�X�g��L��/�����ɂ���
	void SetDepthTest(bool b);
	///	�f�v�X�o�b�t�@�@�ɗp���锻��������w�肷��
	void SetDepthFunc(GRRenderBaseIf::TDepthFunc f);
	/// �A���t�@�u�����f�B���O��L��/�����ɂ���
	void SetAlphaTest(bool b);
	///	�A���t�@�u�����f�B���O�̃��[�h�ݒ�(SRC�̍����W��, DEST�����W��)
	void SetAlphaMode(GRRenderBaseIf::TBlendFunc src, GRRenderBaseIf::TBlendFunc dest);
	///	�V�F�[�f�B���OON(glMaterial) or OFF�iglColor)
	void SetLighting(bool l);
	/// �e�N�X�`���̃��[�h�i�߂�l�F�e�N�X�`��ID�j
	unsigned int LoadTexture(const std::string filename);
	/// �e�N�X�`���摜�̐ݒ�
	void SetTextureImage(const std::string id, int components, int xsize, int ysize, int format, char* tb);
	/// �V�F�[�_�̏�����
	void InitShader();
	/// �V�F�[�_�t�H�[�}�b�g�̐ݒ�
	void SetShaderFormat(GRShaderFormat::ShaderType type);
	/// �V�F�[�_�I�u�W�F�N�g�̍쐬
	bool CreateShader(std::string vShaderFile, std::string fShaderFile, GRHandler& shader);
	/// �V�F�[�_�I�u�W�F�N�g�̍쐬�AGRDeviceGL::shader�ւ̓o�^�i���炩����ShaderFile����o�^���Ă����K�v������j	
	GRHandler CreateShader();
	/// �V�F�[�_�̃\�[�X�v���O�������������ɓǂݍ��݁A�V�F�[�_�I�u�W�F�N�g�Ɗ֘A�t����
	bool ReadShaderSource(GRHandler shader, std::string file);	
	/// ���P�[�V�������̎擾�iSetShaderFormat()�ŃV�F�[�_�t�H�[�}�b�g��ݒ肵�Ă����K�v����j
	void GetShaderLocation(GRHandler shader, void* location);	
};

/**	@brief	�O���t�B�b�N�X�����_���[�̃C���^�t�F�[�X�i�f�o�C�X�̐ݒ�A�J�����̐ݒ�j */
struct GRRenderIf: public GRRenderBaseIf{
	SPR_IFDEF(GRRender);

	///	�f�o�C�X�̐ݒ�
	void SetDevice(GRDeviceIf* dev);
	///	�f�o�C�X�̎擾
	GRDeviceIf* GetDevice();
	///	�J�����̐ݒ�
	void SetCamera(const GRCameraDesc& cam);
	///	�J�����̎擾
	const GRCameraDesc& GetCamera();
	///	�X�N���[��(�E�B���h�E)�T�C�Y�ύX����Viewport�Ǝˉe�s���ݒ�
	void Reshape(Vec2f pos, Vec2f screenSize);
	/// �r���[�|�[�g�̊�_���W�̎擾
	Vec2f GetViewportPos();
	/// �r���[�|�[�g�̃T�C�Y�̎擾
	Vec2f GetViewportSize();
	/// 1�s�N�Z���̕����T�C�Y���擾
	Vec2f GetPixelSize();

	/** @brief �X�N���[�����W����J�������W�ւ̕ϊ�
		@param	x		�X�N���[��x���W
		@param	y		�X�N���[��y���W
		@param depth	z���W
		@param LorR		true�Ȃ獶����W(D3D)�Cfalse�Ȃ�ΉE����W(GL)
		@return			�J�������W
		�J�[�\���ʒu�Ȃǂ̃X�N���[�����W�i�s�N�Z���j���J�������W�n�ɕϊ�����D
		z�����Ɏ��R�x���c��̂�depth�Ŏw�肷��D
		depth�̓J�������_�����z���������ŕK��depth > 0�D
	 */
	Vec3f	ScreenToCamera(int x, int y, float depth, bool LorR = false);

	/** @brief �X�N���[�����W�n�֐؂�ւ���
		�X�N���[�����W�ŕ����Ȃǂ�`�悷�邽�߂̋@�\�D
		��ʍ��オ���_�C��ʉE������x���������C��ʉ�������y���������D
		���W�̓s�N�Z���P�ʁD
	 */
	void EnterScreenCoordinate();
	/// �X�N���[�����W�n����߂�
	void LeaveScreenCoordinate();
	
};

/**	@brief	�O���t�B�b�N�X�����_���[�̃f�o�C�X�N���X�DOpenGL��DirectX�̃��b�p */
struct GRDeviceIf: public GRRenderBaseIf{
	SPR_IFDEF(GRDevice);
	///	������
	void Init();
	///	�f�o�b�O�p�̏�ԃ��|�[�g
	void Print(std::ostream& os) const;
};

/**	@brief	OpenGL�̃����_���[��{�N���X */
struct GRDeviceGLIf: public GRDeviceIf{
	SPR_IFDEF(GRDeviceGL);
};

/**	@brief	DirectX�̃����_���[��{�N���X */
/*
struct GRDeviceD3DIf: public GRDeviceIf{
	SPR_IFDEF(GRDeviceD3D);
};
*/

//@}


}
#endif
