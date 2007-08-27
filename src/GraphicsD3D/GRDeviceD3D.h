/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 *	@file GRDeviceD3D.h
 *	@brief Direct3D�ɂ��O���t�B�b�N�X�`��̎����@�@
 */
#ifndef GRDEVICED3D_H
#define GRDEVICED3D_H

#include "GraphicsD3D.h"
#include <d3d9.h>
#include <atlcomcli.h>

namespace Spr{;

/**	Direct3D�ɂ��O���t�B�b�N�X�`��̎���	*/
class GRDeviceD3D: public GRDevice, public GRDeviceD3DIfInit{
	OBJECTDEF(GRDeviceD3D, GRDevice);
protected:
	CComPtr<IDirect3D9>			direct3D;				///< Direct3D
	CComPtr<IDirect3DDevice9>	d3ddevice;				///< Direct3D�f�o�C�X
	D3DPRESENT_PARAMETERS		pp;						///< �v���[���e�[�V�����p�����[�^
	std::stack<Affinef>			modelMatrixStack;		///< ���f���s��X�^�b�N
	int							nLights;				///< �����̐�
	const GRVertexElement*		currentVertexFormat;	///< ���_�t�H�[�}�b�g
	std::vector<CComPtr<IDirect3DTexture9> >	textures;			///< �e�N�X�`��

	struct DisplayList{
		DWORD									fvf;		///< ���_�t�H�[�}�b�g
		CComPtr<IDirect3DVertexDeclaration9>	decl;		///< ���_�t�H�[�}�b�g
		size_t									stride;		///< ���_�T�C�Y
		CComPtr<IDirect3DVertexBuffer9>			vtxBuf;		///< ���_�o�b�t�@
		int										vtxCount;	///< ���_��
		CComPtr<IDirect3DIndexBuffer9>			idxBuf;		///< �C���f�b�N�X�o�b�t�@
		D3DPRIMITIVETYPE						primType;	///< �v���~�e�B�u�^�C�v
		int										primCount;	///< �v���~�e�B�u��
		GRMaterialIf*							mat;		///< �}�e���A��
		int										texid;		///< �e�N�X�`��ID
	};
	std::vector<DisplayList> displayLists;				///< �f�B�X�v���C���X�g

	GRDeviceD3DConfirmDeviceFunc confirmDeviceFunc;

	bool deviceLost;		///< �f�o�C�X�̓��X�g���Ă��邩�H
	std::vector<GRDeviceD3DListenerFunc> lostDeviceListeners;
	std::vector<GRDeviceD3DListenerFunc> resetDeviceListeners;

	static IDirect3DDevice9* s_d3ddevice;

public:
	///	�R���X�g���N�^
	GRDeviceD3D() : confirmDeviceFunc(NULL) {}
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
	///	�J�����g�̓��e�s����擾����
	virtual void SetProjectionMatrix(const Affinef& afp);
	///	�J�����g�̓��e�s���afp�Œu��������
	virtual void GetProjectionMatrix(const Affinef& afp);
	///	�J�����g�̃��f���s���afw�Œu��������
	virtual void SetModelMatrix(const Affinef& afw);
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
	virtual void SetMaterial(const GRMaterialIf*& mat){ GRDevice::SetMaterial(mat); }
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
	/// �e�N�X�`���̃��[�h�i�߂�l�F�e�N�X�`��ID�j
	virtual unsigned int LoadTexture(const std::string filename);
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
	/// �f�o�C�X�I��p�̃R�[���o�b�N�֐���o�^
	virtual void SetConfirmDeviceFunc(GRDeviceD3DConfirmDeviceFunc func) { confirmDeviceFunc = func; }
	/// �f�o�C�X�����X�g�����Ƃ��ɌĂ΂��R�[���o�b�N�֐���o�^
	virtual void AddLostDeviceListener(GRDeviceD3DListenerFunc func)  { lostDeviceListeners.push_back(func); }
	/// �f�o�C�X�����Z�b�g���ꂽ�Ƃ��ɌĂ΂��R�[���o�b�N�֐���o�^
	virtual void AddResetDeviceListener(GRDeviceD3DListenerFunc func) { resetDeviceListeners.push_back(func); }

	/// D3D�f�o�C�X���擾����
	static IDirect3DDevice9* GetD3DDevice() { return s_d3ddevice; }

protected:
	/// �f�o�C�X�����X�g������Ă�
	void LostDevice();
	/// �f�o�C�X�����Z�b�g����
	void ResetDevice();
};

}
#endif
