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
	/// �����ݒ�
	virtual void Init();

	///	Viewport�ݒ�
	virtual void SetViewport(Vec2f pos, Vec2f sz);
	///	�o�b�t�@�N���A
	virtual void ClearBuffer();
	///	�����_�����O�̊J�n�O�ɌĂԊ֐�
	virtual void BeginScene();
	///	�����_�����O�̏I����ɌĂԊ֐�
	virtual void EndScene();
	///	�J�����g�̎��_�s���afv�Œu��������
	virtual void SetViewMatrix(const Affinef& afv);
	virtual void GetViewMatrix(Affinef& afv);
	///	�J�����g�̓��e�s����擾����
	virtual void SetProjectionMatrix(const Affinef& afp);
	///	�J�����g�̓��e�s���afp�Œu��������
	virtual void GetProjectionMatrix(Affinef& afp);
	///	�J�����g�̃��f���s���afw�Œu��������
	virtual void SetModelMatrix(const Affinef& afw);
	virtual void GetModelMatrix(Affinef& afw);
	///	�J�����g�̃��f���s��ɑ΂���afw���|����
	virtual void MultModelMatrix(const Affinef& afw);
	///	�J�����g�̃��f���s������f���s��X�^�b�N�֕ۑ�����
	virtual void PushModelMatrix();
	///	���f���s��X�^�b�N������o���A�J�����g�̃��f���s��Ƃ���
	virtual void PopModelMatrix();
	/// �u�����h�ϊ��s��̑S�v�f���폜����
	virtual void ClearBlendMatrix();
	/// �u�����h�ϊ��s���ݒ肷��
	virtual bool SetBlendMatrix(const Affinef& afb);
	/// �u�����h�ϊ��s���ݒ肷��
	virtual bool SetBlendMatrix(const Affinef& afb, unsigned int id);
	///	���_�t�H�[�}�b�g�̎w��
	virtual void SetVertexFormat(const GRVertexElement* e);
	///	���_�V�F�[�_�[�̎w��
	virtual void SetVertexShader(void* s);
	///	���_���W���w�肵�ăv���~�e�B�u��`��
	virtual void DrawDirect(GRRenderBaseIf::TPrimitiveType ty, void* begin, size_t count, size_t stride=0);
	///	���_���W�ƃC���f�b�N�X���w�肵�ăv���~�e�B�u��`��
	virtual void DrawIndexed(GRRenderBaseIf::TPrimitiveType ty, size_t* idx, void* vtx, size_t count, size_t stride=0);
	///	���̂̕`��
	void DrawSphere(float radius, int slices, int stacks, bool solid=true);
	/// �~���̕`��
	void DrawCone(float radius, float height, int slice, bool solid=true);
	/// �~���̕`��
	void DrawCylinder(float radius, float height, int slice, bool solid=true);
	///	DiplayList �̍쐬(�L�^�J�n)
	virtual int StartList();
	///	DiplayList �̍쐬�̏I��(�L�^�I��)
	virtual void EndList();
	///	DisplayList�̕\��
	virtual void DrawList(int i);
	///	DisplayList�̉��
	virtual void ReleaseList(int i);
	virtual void SetFont(const GRFont& font);
	/// 3�����e�L�X�g�̕`��iGL�I�����[�ł�font�͎w��Ȃ��j
	virtual void DrawFont(Vec2f pos, const std::string str);
	/// 3�����e�L�X�g�̕`��iGL�I�����[�ł�font�͎w��Ȃ��j
	virtual void DrawFont(Vec3f pos, const std::string str);
	///	�`��̍ގ��̐ݒ�
	virtual void SetMaterial(const GRMaterialDesc& mat);
	virtual void SetMaterial(const GRMaterialIf* mat);
	///	�`�悷��_�E���̑����̐ݒ�
	virtual void SetLineWidth(float w);
	///	�����X�^�b�N��Push
	virtual void PushLight(const GRLightDesc& light);
	virtual void PushLight(const GRLightIf* light){ GRDevice::PushLight(light); }
	///	�����X�^�b�N��Pop
	virtual void PopLight();
	///	�f�v�X�o�b�t�@�ւ̏������݂�����/�֎~����
	virtual void SetDepthWrite(bool b);
	///	�f�v�X�e�X�g��L��/�����ɂ���
	virtual void SetDepthTest(bool b);
	///	�f�v�X�o�b�t�@�@�ɗp���锻��������w�肷��
	virtual void SetDepthFunc(GRRenderBaseIf::TDepthFunc f);
	/// �A���t�@�u�����f�B���O��L��/�����ɂ���
	virtual void SetAlphaTest(bool b);
	///	�A���t�@�u�����f�B���O�̃��[�h�ݒ�(SRC�̍����W��, DEST�����W��)
	virtual void SetAlphaMode(GRRenderBaseIf::TBlendFunc src, GRRenderBaseIf::TBlendFunc dest);
	void SetLighting(bool on);
	/// �e�N�X�`���̃��[�h�i�߂�l�F�e�N�X�`��ID�j
	virtual unsigned int LoadTexture(const std::string filename);
	/// �e�N�X�`���摜�̐ݒ�
	virtual void SetTextureImage(const std::string id, int components, int xsize, int ysize, int format, char* tb);
	/// �V�F�[�_�̏�����
	virtual void InitShader();
	/// �V�F�[�_�t�H�[�}�b�g�̐ݒ�
	virtual void SetShaderFormat(GRShaderFormat::ShaderType type);	
	/// �V�F�[�_�I�u�W�F�N�g�̍쐬
	virtual bool CreateShader(std::string vShaderFile, std::string fShaderFile, GRHandler& shader);
	/// �V�F�[�_�I�u�W�F�N�g�̍쐬�AGRDeviceGL::shader�ւ̓o�^�i���炩����ShaderFile����o�^���Ă����K�v������j
	virtual GRHandler CreateShader();
	/// �V�F�[�_�̃\�[�X�v���O�������������ɓǂݍ��݁A�V�F�[�_�I�u�W�F�N�g�Ɗ֘A�t����
	virtual bool ReadShaderSource(GRHandler shader, std::string file);	
	/// ���P�[�V�������̎擾�iSetShaderFormat()�ŃV�F�[�_�t�H�[�}�b�g��ݒ肵�Ă����K�v����j
	virtual void GetShaderLocation(GRHandler shader, void* location);		
};

}
#endif
