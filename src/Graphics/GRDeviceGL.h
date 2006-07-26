/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
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
#include <SprGraphics.h>

namespace Spr{;

/**	@class	GRDeviceGL
    @brief	OpenGL�ɂ��O���t�B�b�N�X�`��̎����@ */
class GRDeviceGL: public InheritGRDevice<GRDeviceGLIf, GRDevice>{
	OBJECT_DEF(GRDeviceGL);
protected:
	int		window;					///<	�E�B���h�EID
	int		vertexFormatGl;			///<	glInterleavedArrays�Ŏg���C���_�t�H�[�}�b�gID
	size_t	vertexSize;				///<	���_�̃T�C�Y
	bool	vertexColor;			///<	���_���F�������ǂ���
	GRMaterialDesc currentMaterial;	///<	���݂̃}�e���A��
	/**
	 *	@name	�}�g���b�N�X�ϐ�
	 *�@�@ GL�ł�Model��ς�����View������ς���Ƃ������Ƃ��ł��Ȃ��B \n
	 *�@�@ ���_�𓮂����ɂ́A���[�U��Model���o���Ă����K�v������B	\n
	 *�@�@ �iDirect3D�̏ꍇ�́AModel��������������View�������������A���_�𓮂������Ƃ��ł���j
	 *	@{ 
	 */
	Affinef	viewMatrix;				///<	���_�s��
	Affinef	modelMatrix;			///<	���f���s�� 
	Affinef	projectionMatrix;		///<	�ˉe�s�� 
	/** @} */
	/**
	 *	@name	�t�H���g�ϐ�
	 *�@�@ �V�K�Ɏw�肳�ꂽ�t�H���g��fontList�Ɋi�[�����B
	 *	@{
	 */
	std::map<unsigned int, GRFont> fontList;		///<	�t�H���g���X�g<DisplayList��index, font>    
	unsigned int	fontBase;						///<	�f�B�X�v���C���X�g��index number�̊�ꐔ 
	/** @} */	
	int				nLights;		///<	�����̐�
	
public:
	///	�R���X�g���N�^
	GRDeviceGL(int w=0):window(w){}
	///	�E�B���h�EID�̐ݒ�
	virtual void SetWindow(int w){window=w;}
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
	
	///	���_�t�H�[�}�b�g�̎w��
	virtual void SetVertexFormat(const GRVertexElement* e);
	///	���_�V�F�[�_�[�̎w��
	virtual void SetVertexShader(void* s);
	///	���_���W���w�肵�ăv���~�e�B�u��`��
	virtual void DrawDirect(TPrimitiveType ty, void* begin, size_t count, size_t stride=0);
	///	���_���W�ƃC���f�b�N�X���w�肵�ăv���~�e�B�u��`��
	virtual void DrawIndexed(TPrimitiveType ty, size_t* idx, void* vtx, size_t count, size_t stride=0);
	///	�_�C���N�g�`���ɂ��DiplayList�̍쐬
	virtual int CreateList(TPrimitiveType ty, void* vtx, size_t count, size_t stride=0);
	///	�_�C���N�g�`���ɂ��DiplayList�̍쐬�i�}�e���A���ƃe�N�X�`�����w��\�A�e�N�X�`����K�p���ɂ�0���w��j
	virtual int CreateList(GRMaterialDesc& mat, unsigned int texid, 
						   TPrimitiveType ty, void* vtx, size_t count, size_t stride=0);
	///	�C���f�b�N�X�`���ɂ��DiplayList�̍쐬
	virtual int CreateIndexedList(TPrimitiveType ty, size_t* idx, void* vtx, size_t count, size_t stride=0);
	///	�C���f�b�N�X�`���ɂ��DiplayList�̍쐬�i�}�e���A���ƃe�N�X�`�����w��\�A�e�N�X�`����K�p���ɂ�0���w��j
	virtual int CreateIndexedList(GRMaterialDesc& mat, unsigned int texid, 
								  TPrimitiveType ty, size_t* idx, void* vtx, size_t count, size_t stride=0);
	///	DisplayList�̕\��
	virtual void DrawList(int i);
	///	DisplayList�̉��
	virtual void ReleaseList(int i);
	/// 3�����e�L�X�g�̕`��iGL�I�����[�ł�font�͎w��Ȃ��j
	virtual void DrawFont(Vec2f pos, const std::string str);
	/// 3�����e�L�X�g�̕`��iGL�I�����[�ł�font�͎w��Ȃ��j
	virtual void DrawFont(Vec3f pos, const std::string str);
	///	3�����e�L�X�g�̕`��
	virtual void DrawFont(Vec2f pos, const std::string str, const GRFont& font);
	///	3�����e�L�X�g�̕`��
	virtual void DrawFont(Vec3f pos, const std::string str, const GRFont& font);
	///	�`��̍ގ��̐ݒ�
	virtual void SetMaterial(const GRMaterialDesc& mat);
	///	�`�悷��_�E���̑����̐ݒ�
	virtual void SetLineWidth(float w);
	///	�����X�^�b�N��Push
	virtual void PushLight(const GRLightDesc& light);
	///	�����X�^�b�N��Pop
	virtual void PopLight();
	///	�f�v�X�o�b�t�@�ւ̏������݂�����/�֎~����
	virtual void SetDepthWrite(bool b);
	///	�f�v�X�e�X�g��L��/�����ɂ���
	virtual void SetDepthTest(bool b);
	///	�f�v�X�o�b�t�@�@�ɗp���锻��������w�肷��
	virtual void SetDepthFunc(TDepthFunc f);
	/// �A���t�@�u�����f�B���O��L��/�����ɂ���
	virtual void SetAlphaTest(bool b);
	///	�A���t�@�u�����f�B���O�̃��[�h�ݒ�(SRC�̍����W��, DEST�����W��)
	virtual void SetAlphaMode(TBlendFunc src, TBlendFunc dest);
	/// �e�N�X�`�������[�h���A�e�N�X�`���I�u�W�F�N�g���쐬����B�߂�l:�e�N�X�`�����
	virtual void LoadTexture(GRTextureDesc& tex);
};

}
#endif
