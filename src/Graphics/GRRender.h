/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 *	@file GRRender.h
 *	@brief �O���t�B�b�N�X�����_���[�̊�{�N���X�@�@
 */
#ifndef GRRENDER_H
#define GRRENDER_H

#include <SprGraphics.h>
#include <Foundation/Scene.h>
#include "GRFrame.h"
#include "GRSphere.h"

namespace Spr{;

class GRCamera:public GRVisual, public GRCameraDesc{
public:
	SPR_OBJECTDEF(GRCamera);
	ACCESS_DESC(GRCamera);
	UTRef<GRFrameIf> frame;
	GRCamera(const GRCameraDesc& desc=GRCameraDesc()):GRCameraDesc(desc), frame(NULL){}
	virtual size_t NChildObject() const ;
	virtual ObjectIf* GetChildObject(size_t pos);
	virtual GRFrameIf* GetFrame(){ return frame; }
	virtual void SetFrame(GRFrameIf* fr){ frame = fr; }
	virtual bool AddChildObject(ObjectIf* o);
	virtual void Render(GRRenderIf* render);	
};

class GRLight :public GRVisual, public GRLightDesc{
public:
	SPR_OBJECTDEF(GRLight);
	ACCESS_DESC(GRLight);
	GRLight(const GRLightDesc& desc = GRLightDesc()):GRLightDesc(desc){}
	virtual void Render(GRRenderIf* render);
	virtual void Rendered(GRRenderIf* render);
};

/**	@brief	�O���t�B�b�N�X�̍ގ� */
class GRMaterial :public GRVisual, public GRMaterialDesc{
public:
	UTString texnameAbs;			//	�e�N�X�`���t�@�C���̐�΃p�X�������Ă��邱�Ƃ�����B�����ĂȂ��ꍇ�́Atexname���g����
	SPR_OBJECTDEF(GRMaterial);
	ACCESS_DESC(GRMaterial);
	GRMaterial(const GRMaterialDesc& desc=GRMaterialDesc()):GRMaterialDesc(desc){}
	GRMaterial(Vec4f a, Vec4f d, Vec4f s, Vec4f e, float p){
		ambient = a;
		diffuse = d;
		specular = s;
		emissive = e;
		power = p;
	}
	GRMaterial(Vec4f c, float p){
		ambient = diffuse = specular = emissive = c;
		power = p;
	}
	GRMaterial(Vec4f c){
		ambient = diffuse = specular = emissive = c;		
	}
	/**	W()�v�f�́A�A���t�@�l(0.0�`1.0�œ����x��\��). 1.0���s������\��.
		material��W()�v�f�𔻒肵�āA�s�������̂��A�������̂��𔻒肷��. 
		�����ȃI�u�W�F�N�g��`���Ƃ��A�����̂��̂��珇�Ԃɕ`�悵�Ȃ��ƁA�Ӑ}�ɔ��������ʂƂȂ�. */
	bool IsOpaque() const {		
		return ambient.W() >= 1.0 && diffuse.W() >= 1.0 && specular.W() >= 1.0 && emissive.W() >= 1.0;
	}
	void Render(GRRenderIf* render);
};

/**	@class	GRRenderBase
    @brief	�O���t�B�b�N�X�����_���[/�f�o�C�X�̊�{�N���X�@ */
class GRRenderBase: public Object{
public:
	SPR_OBJECTDEF_ABST(GRRenderBase);
	///	�r���[�|�[�g�̐ݒ�
	virtual void SetViewport(Vec2f pos, Vec2f sz){}
	///	�o�b�t�@�N���A
	virtual void ClearBuffer(){}
	/// �w�i�F�̎擾
	virtual void GetClearColor(Vec4f& color){}
	/// �w�i�F�̐ݒ�
	virtual void SetClearColor(const Vec4f& color){}
	///	�����_�����O�̊J�n�O�ɌĂԊ֐�
	virtual void BeginScene(){}
	///	�����_�����O�̏I����ɌĂԊ֐�
	virtual void EndScene(){}
	///	�J�����g�̎��_�s���afv�Œu��������
	virtual void SetViewMatrix(const Affinef& afv){}
	virtual void GetViewMatrix(Affinef& afv){}
	///	�J�����g�̓��e�s���afp�Œu��������
	virtual void SetProjectionMatrix(const Affinef& afp){}
	///	�J�����g�̓��e�s����擾����
	virtual void GetProjectionMatrix(Affinef& afp){}
	///	�J�����g�̃��f���s���afw�Œu��������
	virtual void SetModelMatrix(const Affinef& afw){}
	virtual void GetModelMatrix(Affinef& afw){}
	///	�J�����g�̃��f���s��ɑ΂���afw���|����
	virtual void MultModelMatrix(const Affinef& afw){}
	///	�J�����g�̃��f���s������f���s��X�^�b�N�֕ۑ�����
	virtual void PushModelMatrix(){}
	///	���f���s��X�^�b�N������o���A�J�����g�̃��f���s��Ƃ���
	virtual void PopModelMatrix(){}
	/// �u�����h�ϊ��s��̑S�v�f���폜����
	virtual void ClearBlendMatrix(){}
	/// �u�����h�ϊ��s���ݒ肷��
	virtual bool SetBlendMatrix(const Affinef& afb, unsigned int id=0){return 0;}	
	///	���_�t�H�[�}�b�g�̎w��
	virtual void SetVertexFormat(const GRVertexElement* e){}
	///	���_�V�F�[�_�[�̎w��	API�����D�����ȂǗv���� 2006.6.7 hase
	virtual void SetVertexShader(void* shader){}
	///	���_���w�肵�ăv���~�e�B�u��`��
	virtual void DrawDirect(GRRenderBaseIf::TPrimitiveType ty, void* vtx, size_t count, size_t stride=0){}
	///	���_�ƃC���f�b�N�X���w�肵�ăv���~�e�B�u��`��
	virtual void DrawIndexed(GRRenderBaseIf::TPrimitiveType ty, size_t* idx, void* vtx, size_t count, size_t stride=0){}
	///	���̂�`��
	virtual void DrawSphere(float radius, int slices, int stacks, bool solid=true){}
 	/// �~���̕`��
	virtual void DrawCone(float radius, float height, int slice, bool solid=true){}
	/// �~���̕`��
	virtual void DrawCylinder(float radius, float height, int slice, bool solid=true){}
	///	���_�̐������Ƃ̔z����w�肵�āC�v���~�e�B�u��`��
	virtual void DrawArrays(GRRenderBaseIf::TPrimitiveType ty, GRVertexArray* arrays, size_t count){}
 	///	�C���f�b�N�X�ƒ��_�̐������Ƃ̔z����w�肵�āC�v���~�e�B�u��`��
	virtual void DrawArrays(GRRenderBaseIf::TPrimitiveType ty, size_t* idx, GRVertexArray* arrays, size_t count){}
	
	///	DiplayList �̍쐬(���X�g�쐬�J�n)
	virtual int StartList(){return 0;}
	///	���X�g�쐬�I��
	virtual void EndList(){}
	///	DisplayList�̕\��
	virtual void DrawList(int i){}
	///	DisplayList�̉��
	virtual void ReleaseList(int i){}
	virtual void SetFont(const GRFont& font)=0;
	///	2�����e�L�X�g�̕`��
	virtual void DrawFont(Vec2f pos, const std::string str){}
	///	3�����e�L�X�g�̕`��
	virtual void DrawFont(Vec3f pos, const std::string str){}
	///	�`��̍ގ��̐ݒ�
	virtual void SetMaterial(const GRMaterialDesc& mat){}
	virtual void SetMaterial(const GRMaterialIf* mat){}
	///	�`�悷��_�E���̑����̐ݒ�
	virtual void SetLineWidth(float w){}
	///	�����X�^�b�N��Push
	virtual void PushLight(const GRLightDesc& light){}
	virtual void PushLight(const GRLightIf* light){}
	///	�����X�^�b�N��Pop
	virtual void PopLight(){}
	///	�f�v�X�o�b�t�@�ւ̏������݂�����/�֎~����
	virtual void SetDepthWrite(bool b){}
	///	�f�v�X�e�X�g��L��/�����ɂ���
	virtual void SetDepthTest(bool b){}
	///	�f�v�X�o�b�t�@�@�ɗp���锻��������w�肷��
	virtual void SetDepthFunc(GRRenderBaseIf::TDepthFunc f){}
	/// �A���t�@�u�����f�B���O��L��/�����ɂ���
	virtual void SetAlphaTest(bool b){}
	///	�A���t�@�u�����f�B���O�̃��[�h�ݒ�(SRC�̍����W��, DEST�����W��)
	virtual void SetAlphaMode(GRRenderBaseIf::TBlendFunc src, GRRenderBaseIf::TBlendFunc dest){}
	///	�V�F�[�f�B���OON(glMaterial) or OFF�iglColor)
	virtual void SetLighting(bool l){}

	/// �e�N�X�`���̃��[�h�i�߂�l�F�e�N�X�`��ID�j
	virtual unsigned int LoadTexture(const std::string filename){return 0;}
	/// �e�N�X�`���摜�̐ݒ�
	virtual void SetTextureImage(const std::string id, int components, int xsize, int ysize, int format, char* tb){}
	/// �V�F�[�_�̏�����
	virtual void InitShader(){}
	/// �V�F�[�_�t�H�[�}�b�g�̐ݒ�
	virtual void SetShaderFormat(GRShaderFormat::ShaderType type){}	
	/// �V�F�[�_�I�u�W�F�N�g�̍쐬
	virtual bool CreateShader(std::string vShaderFile, std::string fShaderFile, GRHandler& shader){return 0;}
	/// �V�F�[�_�I�u�W�F�N�g�̍쐬�AGRDeviceGL::shader�ւ̓o�^�i���炩����ShaderFile����o�^���Ă����K�v������j	
	virtual GRHandler CreateShader(){return 0;}
	/// �V�F�[�_�̃\�[�X�v���O�������������ɓǂݍ��݁A�V�F�[�_�I�u�W�F�N�g�Ɗ֘A�t����
	virtual bool ReadShaderSource(GRHandler shader, std::string file){return 0;}	
	/// ���P�[�V�������̎擾�iSetShaderFormat()�ŃV�F�[�_�t�H�[�}�b�g��ݒ肵�Ă����K�v����j
	virtual void GetShaderLocation(GRHandler shader, void* location){}
};

/**	@class	GRRender
    @brief	�O���t�B�b�N�X�����_���[�̊�{�N���X�i�f�o�C�X�̐؂蕪���j�@ */
class GRRender: public GRRenderBase{
	SPR_OBJECTDEF(GRRender);
protected:
	UTRef<GRDeviceIf> device;		///<	�f�o�C�X
	GRCameraDesc camera;			///<	�J����
	Vec2f viewportPos;				///<	�r���[�|�[�g�̍���
	Vec2f viewportSize;				///<	�r���[�|�[�g�̃T�C�Y
public:
#define REDIRECTIMP_GRRENDERBASE(ptr)																		\
	virtual void SetViewport(Vec2f p, Vec2f s){ ptr SetViewport(p, s); }									\
	virtual void ClearBuffer(){ ptr ClearBuffer(); }														\
	virtual void GetClearColor(Vec4f& color){ ptr GetClearColor(color); }									\
	virtual void SetClearColor(const Vec4f& color){ ptr SetClearColor(color); }								\
	virtual void BeginScene(){ ptr BeginScene(); }															\
	virtual void EndScene(){ ptr EndScene(); }																\
	virtual void SetViewMatrix(const Affinef& afv){ ptr SetViewMatrix(afv); }								\
	virtual void GetViewMatrix(Affinef& afv){ ptr SetViewMatrix(afv); }										\
	virtual void SetProjectionMatrix(const Affinef& afp){ ptr SetProjectionMatrix(afp); }					\
	virtual void GetProjectionMatrix(Affinef& afp){ ptr GetProjectionMatrix(afp); }							\
	virtual void SetModelMatrix(const Affinef& afw){ ptr SetModelMatrix(afw); }								\
	virtual void GetModelMatrix(Affinef& afw){ ptr SetModelMatrix(afw); }									\
	virtual void MultModelMatrix(const Affinef& afw){ ptr MultModelMatrix(afw); }							\
	virtual void PushModelMatrix(){ ptr PushModelMatrix(); }												\
	virtual void PopModelMatrix(){ ptr PopModelMatrix(); }													\
	virtual bool SetBlendMatrix(const Affinef& afb){ return ptr SetBlendMatrix(afb); }						\
	virtual bool SetBlendMatrix(const Affinef& afb, unsigned int id){ return ptr SetBlendMatrix(afb, id); }	\
	virtual void ClearBlendMatrix(){ ptr ClearBlendMatrix(); }												\
	virtual void SetVertexFormat(const GRVertexElement* f){ ptr SetVertexFormat(f); }						\
	virtual void DrawDirect(GRRenderBaseIf::TPrimitiveType ty, void* vtx, size_t ct, size_t st=0)			\
		{ ptr DrawDirect(ty, vtx, ct, st); }																\
	virtual void DrawIndexed(GRRenderBaseIf::TPrimitiveType ty,												\
		size_t* idx, void* vtx, size_t ct, size_t st=0)														\
		{ ptr DrawIndexed(ty, idx, vtx, ct, st); }															\
	virtual void DrawSphere(float radius, int stacks, int slice, bool solid = true)							\
		{ ptr DrawSphere(radius, stacks, slice, solid); }													\
	virtual void DrawCone(float radius, float height, int slice, bool solid=true)							\
		{ ptr DrawCone(radius, height, slice, solid); }														\
	virtual void DrawCylinder(float radius, float height, int slice, bool solid=true)						\
		{ ptr DrawCylinder(radius, height, slice, solid); }													\
	virtual int StartList()																					\
		{ return ptr StartList(); }																			\
	virtual void EndList()																					\
		{ ptr EndList(); }																					\
	virtual void DrawList(int i){ ptr DrawList(i); }														\
	virtual void ReleaseList(int i){ ptr ReleaseList(i); }													\
	virtual void SetFont(const GRFont& font){ ptr SetFont(font); }											\
	virtual void DrawFont(Vec2f pos, const std::string str){ ptr DrawFont(pos, str); }						\
    virtual void DrawFont(Vec3f pos, const std::string str){ ptr DrawFont(pos, str); }						\
	virtual void SetMaterial(const GRMaterialDesc& mat){ ptr SetMaterial(mat); }							\
	virtual void SetMaterial(const GRMaterialIf* mat){ ptr SetMaterial(mat); }								\
	virtual void SetLineWidth(float w){ ptr SetLineWidth(w); }												\
	virtual void PushLight(const GRLightDesc& light){ptr PushLight(light);}									\
	virtual void PushLight(const GRLightIf* light){ptr PushLight(light);}									\
	virtual void PopLight(){ptr PopLight(); }																\
	virtual void SetDepthWrite(bool b){ ptr SetDepthWrite(b); }												\
	virtual void SetDepthTest(bool b){ptr SetDepthTest(b); }												\
	virtual void SetDepthFunc(GRRenderBaseIf::TDepthFunc f){ ptr SetDepthFunc(f); }							\
	virtual void SetAlphaTest(bool b){ptr SetAlphaTest(b); }												\
	virtual void SetAlphaMode(GRRenderBaseIf::TBlendFunc src, GRRenderBaseIf::TBlendFunc dest)				\
		{ ptr SetAlphaMode(src, dest); }																	\
	virtual void SetLighting(bool l) { ptr SetLighting(l); }												\
	virtual unsigned int LoadTexture(const std::string filename){ return ptr LoadTexture(filename); }		\
	virtual void InitShader(){ ptr InitShader(); }															\
	virtual void SetShaderFormat(GRShaderFormat::ShaderType type){ ptr SetShaderFormat(type); }				\
	virtual bool CreateShader(std::string vShaderFile, std::string fShaderFile, GRHandler& shader)			\
		{ return ptr CreateShader(vShaderFile, fShaderFile, shader); }										\
	virtual GRHandler CreateShader(){ return ptr CreateShader(); }											\
	virtual bool ReadShaderSource(GRHandler shader, std::string file)										\
		{ return ptr ReadShaderSource(shader, file); }														\
	virtual void GetShaderLocation(GRHandler shader, void* location)										\
		{ ptr GetShaderLocation(shader, location); }														\
	
	REDIRECTIMP_GRRENDERBASE(device->)

	///	�f�o�C�X�̐ݒ�
	virtual void SetDevice(GRDeviceIf* dev){ device = dev; }
	///	�f�o�C�X�̎擾
	virtual GRDeviceIf* GetDevice(){ return device; }
	///	�f�o�b�O�\��
	virtual void Print(std::ostream& os) const;
	///	�J�����̐ݒ�
	void SetCamera(const GRCameraDesc& c);
	const GRCameraDesc& GetCamera(){ return camera; }
	///	�X�N���[���T�C�Y�ƃv���W�F�N�V�����s��̐ݒ�
	virtual void Reshape(Vec2f pos, Vec2f sz);
	/// Viewport�̊�_���W�̎擾
	Vec2f GetViewportPos(){ return viewportPos; }
	/// Viewport�̃T�C�Y�̎擾
	Vec2f GetViewportSize(){ return viewportSize; }
	///
	Vec2f GetPixelSize();
	/// �X�N���[���E�J�������W�ϊ�
	Vec3f	ScreenToCamera(int x, int y, float depth, bool LorR = false);
};

/**	@class	GRDevice
    @brief	�O���t�B�b�N�X�`��̎����@ */
class GRDevice: public GRRenderBase{
protected:
	Vec4f	clearColor;		///< �w�i�F
public:
	SPR_OBJECTDEF_ABST(GRDevice);
	virtual void Init(){}
	virtual void GetClearColor(Vec4f& color){ color = clearColor; }
	virtual void SetClearColor(const Vec4f& color){ clearColor = color; }
	virtual void SetMaterial(const GRMaterialDesc& mat){}
	virtual void SetMaterial(const GRMaterialIf* mat){}
	virtual void PushLight(const GRLightDesc& light){}
    virtual void PushLight(const GRLightIf* light){
        if(light) PushLight(*DCAST(GRLight, light)); }
};
}
#endif
