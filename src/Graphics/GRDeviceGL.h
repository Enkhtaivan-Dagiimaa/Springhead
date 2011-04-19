/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 *	@file GRDeviceGL.h
 *	@brief OpenGL�ɂ��O���t�B�b�N�X�`��̎����@�@
 */
#ifndef GRDEVICEGL_H
#define GRDEVICEGL_H

#include <map>
#include <stack>
#include <SprGraphics.h>

namespace Spr{;

/**	@class	GRDeviceGL
    @brief	OpenGL�ɂ��O���t�B�b�N�X�`��̎����@ */
class GRDeviceGL: public GRDevice{
	SPR_OBJECTDEF(GRDeviceGL);
protected:
	int		nLights;					///< �����̐�
	int		vertexFormatGl;				///< glInterleavedArrays�Ŏg���CGL�̒��_�t�H�[�}�b�gID
	size_t	vertexSize;					///< ���_�̃T�C�Y
	bool	vertexColor;				///< ���_���F�������ǂ���
	GRMaterialDesc currentMaterial;		///< ���݂̃}�e���A��
	
	/**
	 *	@name	�}�g���b�N�X�ϐ�
	 *�@�@ GL�ł�Model��ς�����View������ς���Ƃ������Ƃ��ł��Ȃ��B \n
	 *�@�@ ���_�𓮂����ɂ́A���[�U��Model���o���Ă����K�v������B	\n
	 *�@�@ �iDirect3D�̏ꍇ�́AModel��������������View�������������A���_�𓮂������Ƃ��ł���j
	 *	@{ 
	 */
	Affinef								viewMatrix;				///< �J�����g���_�s��
	Affinef								modelMatrix;			///< �J�����g���f���s�� 
	std::stack<Affinef>                	modelMatrixStack;		///< ���f���s��X�^�b�N
	std::vector<Affinef>				blendMatrix;			///< �u�����h�ϊ��s��
	/** @} */

	/**
	 *	@name	�e�N�X�`���}�l�[�W��
	 *�@�@ ���[�h���ꂽ�e�N�X�`����ID���o���Ă���map
	 *
	 */
	typedef std::map<std::string, unsigned int, UTStringLess> GRTexnameMap;
	GRTexnameMap texnameMap;

	/**
	 *	@name	�t�H���g�ϐ�
	 *�@�@ �V�K�Ɏw�肳�ꂽ�t�H���g��fontList�Ɋi�[�����B
	 *	@{
	 */
	std::map<unsigned int, GRFont> fontList;		///< �t�H���g���X�g<DisplayList��index, font>    
	unsigned int	fontBase;						///< �f�B�X�v���C���X�g��index number�̊�ꐔ 
	GRFont			font;							///< �t�H���g���
	/** @} */	

	/**
	 *	@name	�V�F�[�_�ϐ�
	 *	@{
	 */
	std::string vertexShaderFile;						///< VertexShader �t�@�C����
	std::string fragmentShaderFile;						///< FragmentShader �t�@�C����
	GRShaderFormat::ShaderType shaderType;				///< �V�F�[�_�̃��P�[�V�����^�C�v
	/** @} */	

public:
	///	�R���X�g���N�^
	GRDeviceGL(){}
	
	virtual void Init();
	virtual void SetViewport(Vec2f pos, Vec2f sz);
	virtual void ClearBuffer();
	virtual void BeginScene();
	virtual void EndScene();
	///	�o�b�t�@��\�����邽�߂̌ĂԊ֐�
	virtual void SwapBuffers();
	///	�J�����g�̎��_�s���afv�Œu��������
	virtual void SetViewMatrix(const Affinef& afv);
	virtual void GetViewMatrix(Affinef& afv);
	///	�J�����g�̓��e�s����擾����
	virtual void SetProjectionMatrix(const Affinef& afp);
	virtual void GetProjectionMatrix(Affinef& afp);
	virtual void SetModelMatrix(const Affinef& afw);
	virtual void GetModelMatrix(Affinef& afw);
	///	�J�����g�̃��f���s��ɑ΂���afw���|����
	virtual void MultModelMatrix(const Affinef& afw);
	virtual void PushModelMatrix();
	virtual void PopModelMatrix();
	virtual void ClearBlendMatrix();
	virtual bool SetBlendMatrix(const Affinef& afb);
	virtual bool SetBlendMatrix(const Affinef& afb, unsigned int id);
	virtual void SetVertexFormat(const GRVertexElement* e);
	virtual void SetVertexShader(void* s);
	virtual void DrawDirect(GRRenderBaseIf::TPrimitiveType ty, void* begin, size_t count, size_t stride=0);
	virtual void DrawIndexed(GRRenderBaseIf::TPrimitiveType ty, size_t* idx, void* vtx, size_t count, size_t stride=0);
	virtual void DrawLine		(Vec3f p0, Vec3f p1);
	virtual void DrawArrow		(Vec3f p0, Vec3f p1, float rbar, float rhead, float lhead, int slice, bool solid);
	virtual void DrawBox		(float sx, float sy, float sz, bool solid=true);
	virtual void DrawSphere		(float radius, int slices, int stacks, bool solid=true);
	virtual void DrawCone		(float radius, float height, int slice, bool solid=true);
	virtual void DrawCylinder	(float radius, float height, int slice, bool solid=true);
	virtual void DrawCapsule	(float radius, float height, int slice=20, bool solid=true);
	virtual void DrawRoundCone	(float rbottom, float rtop, float height, int slice=20, bool solid=true);
	virtual void DrawGrid		(float size, int slice, float lineWidth);

	virtual int  StartList();
	virtual void EndList();
	virtual void DrawList(int i);
	virtual void ReleaseList(int i);
	virtual void SetFont(const GRFont& font);
	virtual void DrawFont(Vec2f pos, const std::string str);
	virtual void DrawFont(Vec3f pos, const std::string str);
	virtual void SetMaterial(const GRMaterialDesc& mat);
	virtual void SetMaterial(const GRMaterialIf* mat);
	///	�`�悷��_�E���̑����̐ݒ�
	virtual void SetLineWidth(float w);
	virtual void PushLight(const GRLightDesc& light);
	virtual void PushLight(const GRLightIf* light){ GRDevice::PushLight(light); }
	virtual void PopLight();
	virtual int  NLights();
	virtual void SetDepthWrite(bool b);
	virtual void SetDepthTest(bool b);
	virtual void SetDepthFunc(GRRenderBaseIf::TDepthFunc f);
	virtual void SetAlphaTest(bool b);
	virtual void SetAlphaMode(GRRenderBaseIf::TBlendFunc src, GRRenderBaseIf::TBlendFunc dest);
	virtual void SetLighting(bool on);
	virtual unsigned int LoadTexture(const std::string filename);
	virtual void SetTextureImage(const std::string id, int components, int xsize, int ysize, int format, char* tb);
	virtual void InitShader();
	virtual void SetShaderFormat(GRShaderFormat::ShaderType type);	
	virtual bool CreateShader(std::string vShaderFile, std::string fShaderFile, GRHandler& shader);
	virtual GRHandler CreateShader();
	virtual bool ReadShaderSource(GRHandler shader, std::string file);	
	virtual void GetShaderLocation(GRHandler shader, void* location);		
};

}
#endif
