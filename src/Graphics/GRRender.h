/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
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
#include "IfStubGraphics.h"

namespace Spr{;

class GRCamera:public GRVisual, GRCameraIfInit, public GRCameraDesc{
public:
	OBJECT_DEF(GRCamera);
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

class GRLight :public GRVisual, GRLightIfInit, public GRLightDesc{
public:
	OBJECT_DEF(GRLight);
	ACCESS_DESC(GRLight);
	GRLight(const GRLightDesc& desc = GRLightDesc()):GRLightDesc(desc){}
	virtual void Render(GRRenderIf* render);
	virtual void Rendered(GRRenderIf* render);
};

/**	@brief	�O���t�B�b�N�X�̍ގ� */
class GRMaterial :public GRVisual, GRMaterialIfInit, public GRMaterialDesc{
public:
	OBJECT_DEF(GRMaterial);
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
	//virtual bool AddChildObject(ObjectIf* o);
};

/**	@class	GRRenderBase
    @brief	�O���t�B�b�N�X�����_���[/�f�o�C�X�̊�{�N���X�@ */
class GRRenderBase: public Object, GRRenderBaseIfInit{
public:
	OBJECT_DEF_ABST(GRRenderBase);
	///	�r���[�|�[�g�̐ݒ�
	virtual void SetViewport(Vec2f pos, Vec2f sz){}
	///	�o�b�t�@�N���A
	virtual void ClearBuffer(){}
	///	�����_�����O�̊J�n�O�ɌĂԊ֐�
	virtual void BeginScene(){}
	///	�����_�����O�̏I����ɌĂԊ֐�
	virtual void EndScene(){}
	///	�J�����g�̎��_�s���afv�Œu��������
	virtual void SetViewMatrix(const Affinef& afv){}
	///	�J�����g�̓��e�s���afp�Œu��������
	virtual void SetProjectionMatrix(const Affinef& afp){}
	///	�J�����g�̓��e�s����擾����
	virtual void GetProjectionMatrix(const Affinef& afp){}
	///	�J�����g�̃��f���s���afw�Œu��������
	virtual void SetModelMatrix(const Affinef& afw){}
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
 	///	���_�̐������Ƃ̔z����w�肵�āC�v���~�e�B�u��`��
	virtual void DrawArrays(GRRenderBaseIf::TPrimitiveType ty, GRVertexArray* arrays, size_t count){}
 	///	�C���f�b�N�X�ƒ��_�̐������Ƃ̔z����w�肵�āC�v���~�e�B�u��`��
	virtual void DrawArrays(GRRenderBaseIf::TPrimitiveType ty, size_t* idx, GRVertexArray* arrays, size_t count){}
	
	///	�_�C���N�g�`���ɂ�� DiplayList �̍쐬
	virtual int CreateList(GRRenderBaseIf::TPrimitiveType ty, void* vtx, size_t count, size_t stride=0){return 0;}
	virtual int CreateList(GRMaterialIf* mat, unsigned int texid, 
						   GRRenderBaseIf::TPrimitiveType ty, void* vtx, size_t count, size_t stride=0){return 0;}
	/// ���I�u�W�F�N�g�� DisplayList �̍쐬
	virtual int CreateList(float radius, int slices, int stacks){return 0;}
	virtual int CreateList(GRMaterialIf* mat, float radius, int slices, int stacks){return 0;}
	///	�C���f�b�N�X�`���ɂ�� DiplayList �̍쐬
	virtual int CreateIndexedList(GRRenderBaseIf::TPrimitiveType ty, size_t* idx, void* vtx, size_t count, size_t stride=0){return 0;}
	virtual int CreateIndexedList(GRMaterialIf* mat, 
								  GRRenderBaseIf::TPrimitiveType ty, size_t* idx, void* vtx, size_t count, size_t stride=0){return 0;}
	/// �C���f�b�N�X�`���ɂ��V�F�[�_��K�p���� DisplayList �̍쐬�iSetVertexFormat() ����� SetShaderFormat() �̌�ɌĂԁj
	virtual int CreateShaderIndexedList(GRHandler shader, void* location, 
									GRRenderBaseIf::TPrimitiveType ty, size_t* idx, void* vtx, size_t count, size_t stride=0){return 0;}	
	///	DisplayList�̕\��
	virtual void DrawList(int i){}
	///	DisplayList�̉��
	virtual void ReleaseList(int i){}
	///	2�����e�L�X�g�̕`��@�@ Windows��(VC)�ł̂�font���T�|�[�g���A���̊��ł�font���w�肵�Ă����p����Ȃ��B
	virtual void DrawFont(Vec2f pos, const std::string str, const GRFont& font=0){}
	///	3�����e�L�X�g�̕`��@�@ Windows��(VC)�ł̂�font���T�|�[�g���A���̊��ł�font���w�肵�Ă����p����Ȃ��B	
	virtual void DrawFont(Vec3f pos, const std::string str, const GRFont& font=0){}
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
	/// �e�N�X�`���̃��[�h�i�߂�l�F�e�N�X�`��ID�j
	virtual unsigned int LoadTexture(const std::string filename){return 0;}
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
class GRRender: public GRRenderBase, GRRenderIfInit{
	OBJECT_DEF(GRRender);
protected:
	UTRef<GRDeviceIf> device;		///<	�f�o�C�X
	GRCameraDesc camera;			///<	�J����
	Vec2f viewportPos;				///<	�r���[�|�[�g�̍���
	Vec2f viewportSize;				///<	�r���[�|�[�g�̃T�C�Y
public:
#define REDIRECTIMP_GRRENDERBASE(ptr)																		\
	virtual void SetViewport(Vec2f p, Vec2f s){ ptr SetViewport(p, s); }									\
	virtual void ClearBuffer(){ ptr ClearBuffer(); }														\
	virtual void BeginScene(){ ptr BeginScene(); }															\
	virtual void EndScene(){ ptr EndScene(); }																\
	virtual void SetViewMatrix(const Affinef& afv){ ptr SetViewMatrix(afv); }								\
	virtual void SetProjectionMatrix(const Affinef& afp){ ptr SetProjectionMatrix(afp); }					\
	virtual void GetProjectionMatrix(const Affinef& afp){ ptr GetProjectionMatrix(afp); }					\
	virtual void SetModelMatrix(const Affinef& afw){ ptr SetModelMatrix(afw); }								\
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
	virtual int CreateList(GRRenderBaseIf::TPrimitiveType ty, void* vtx, size_t count, size_t stride=0)		\
		{ return ptr CreateList(ty, vtx, count, stride); }													\
	virtual int CreateList(GRMaterialIf* mat, unsigned int texid,											\
		GRRenderBaseIf::TPrimitiveType ty, void* vtx, size_t count, size_t stride=0)						\
		{ return ptr CreateList(mat, texid, ty, vtx, count, stride); }										\
	virtual int CreateList(float radius, int slices, int stacks)											\
		{ return ptr CreateList(radius, slices, stacks); }													\
	virtual int CreateList(GRMaterialIf* mat, float radius, int slices, int stacks)							\
		{ return ptr CreateList(mat, radius, slices, stacks); }												\
	virtual int CreateIndexedList(GRRenderBaseIf::TPrimitiveType ty, size_t* idx, void* vtx, 				\
		size_t count, size_t stride=0)																		\
		{ return ptr CreateIndexedList(ty, idx, vtx, count, stride); }										\
	virtual int CreateIndexedList(GRMaterialIf* mat,														\
		GRRenderBaseIf::TPrimitiveType ty, size_t* idx, void* vtx, size_t count, size_t stride=0)			\
		{ return ptr CreateIndexedList(mat, ty, idx, vtx, count, stride); }									\
	virtual int CreateShaderIndexedList(GRHandler shader, void* location, 									\
		GRRenderBaseIf::TPrimitiveType ty, size_t* idx, void* vtx, size_t count, size_t stride=0)			\
		{ return ptr CreateShaderIndexedList(shader, location, ty, idx, vtx, count, stride); }				\
	virtual void DrawList(int i){ ptr DrawList(i); }														\
	virtual void ReleaseList(int i){ ptr ReleaseList(i); }													\
	virtual void DrawFont(Vec2f pos, const std::string str){ ptr DrawFont(pos, str); }						\
    virtual void DrawFont(Vec3f pos, const std::string str){ ptr DrawFont(pos, str); }						\
	virtual void DrawFont(Vec2f pos, const std::string str, const GRFont& font)								\
		{ ptr DrawFont(pos, str, font); }																	\
	virtual void DrawFont(Vec3f pos, const std::string str, const GRFont& font)								\
		{ ptr DrawFont(pos, str, font); }																	\
	virtual void SetMaterial(const GRMaterialDesc& mat){ ptr SetMaterial(mat); }							\
	virtual void SetMaterial(const GRMaterialIf* mat)														\
		{ if(mat) ptr SetMaterial((const GRMaterialDesc&)*DCAST(GRMaterial, mat)); }						\
	virtual void SetLineWidth(float w){ ptr SetLineWidth(w); }												\
	virtual void PushLight(const GRLightDesc& light){ptr PushLight(light);}									\
	virtual void PushLight(const GRLightIf* light)															\
		{ if(light) ptr PushLight((const GRLightDesc&)*DCAST(GRLight, light)); }							\
	virtual void PopLight(){ptr PopLight(); }																\
	virtual void SetDepthWrite(bool b){ ptr SetDepthWrite(b); }												\
	virtual void SetDepthTest(bool b){ptr SetDepthTest(b); }												\
	virtual void SetDepthFunc(GRRenderBaseIf::TDepthFunc f){ ptr SetDepthFunc(f); }							\
	virtual void SetAlphaTest(bool b){ptr SetAlphaTest(b); }												\
	virtual void SetAlphaMode(GRRenderBaseIf::TBlendFunc src, GRRenderBaseIf::TBlendFunc dest)				\
		{ ptr SetAlphaMode(src, dest); }																	\
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
	///	�f�o�b�O�\��
	virtual void Print(std::ostream& os) const;
	///	�J�����̐ݒ�
	void SetCamera(const GRCameraDesc& c);
	///	�X�N���[���T�C�Y�ƃv���W�F�N�V�����s��̐ݒ�
	virtual void Reshape(Vec2f pos, Vec2f sz);
};

/**	@class	GRDevice
    @brief	�O���t�B�b�N�X�`��̎����@ */
class GRDevice: public GRRenderBase, GRDeviceIfInit{
public:
	OBJECT_DEF_ABST(GRDevice);
	virtual void Init(){}
	virtual void SetMaterial(const GRMaterialDesc& mat){}
	virtual void SetMaterial(const GRMaterialIf* mat){
		if(mat) SetMaterial(*DCAST(GRMaterial, mat)); }
	virtual void PushLight(const GRLightDesc& light){}
    virtual void PushLight(const GRLightIf* light){
        if(light) PushLight(*DCAST(GRLight, light)); }
};
}
#endif
