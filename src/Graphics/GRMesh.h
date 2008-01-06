/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef GRMesh_H
#define GRMesh_H

#include <SprGraphics.h>
#include "GRFrame.h"

namespace Spr{;

class GRMaterial;

/**	@class	GRMesh
    @brief	�O���t�B�b�N�X�V�[���O���t�ł̍��W�n��\���D */
class GRMesh: public GRVisual, public GRMeshIfInit, public GRMeshDesc{
	UTRef<GRRenderIf> render;			///< �����_���[
	unsigned int list;					///< �f�B�X�v���C���X�g�̎��ʎq

	/// ���_�o�b�t�@�̍쐬
	void MakeBuffer();
	/// ���_�o�b�t�@�̕`��
	void DrawBuffer(void* vtx);
public:
	/// �f�B�X�v���C���X�g���쐬����D
	void CreateList(GRRenderIf* r);
private:
	///	���_�t�H�[�}�b�g�ɍ��������_�o�b�t�@
	float* vtxs;
	int nVtxs;
	float* blendedVtxs;
	const GRVertexElement* vtxFormat;
	int stride, normalOffset, positionOffset, texOffset;
public:
	OBJECTDEF(GRMesh, GRVisual);
	ACCESS_DESC(GRMesh);

	bool tex3d;												///< 3�����e�N�X�`���̏ꍇ true
	std::vector< UTRef<GRMaterial> > material;				///< �}�e���A��
	std::vector<int>                 materialList;			///< �}�e���A���̃C���f�b�N�X���X�g

	struct SkinWeight{
		GRFrame* frame;
		Affinef offset;
		std::vector<unsigned> indices;
		std::vector<float> weights;
		SkinWeight(): frame(NULL){}
	};
	std::vector<SkinWeight>	skinWeights;					///< �X�L��Mesh�̏ꍇ��skinWeights

	float* GetVertex(int i=0){								///< i�Ԗڂ̒��_�o�b�t�@��Ԃ�
		return vtxs + stride * i;
	}
	int GetNVertex(){ return nVtxs; }						///< ���_��
	const GRVertexElement* GetVertexFormat(){				///< ���_�t�H�[�}�b�g��Ԃ��B
		return vtxFormat;
	}
	bool IsTex3d(){ return tex3d; }							///< 3�����e�N�X�`���Ȃ� true
	int GetStride(){ return stride; }						///< 1���_�̃f�[�^��float������
	int GetNormalOffset(){ return normalOffset; }			///< �@���̃I�t�Z�b�g(float����)
	int GetPositionOffset(){ return positionOffset; }		///< �ʒu�̃I�t�Z�b�g(float����)
	int GetTexOffset(){ return texOffset; }					///< �e�N�X�`�����W�̃I�t�Z�b�g(float����)

	/**
	   faces�ƁA�O�p�`��������O��X�t�@�C���Ŏw�肳�ꂽ�ʂƂ̊֘A�t�� \n
	  �iX�t�@�C�����烍�[�h���ꂽ�ʂ́A���ׂĎO�p�`��������Ă��邽�߁A
	   �ǂ̖ʂ��ǂ̃}�e���A����K�p����̂����f���邽�߂̂��́j			*/
	std::vector<size_t> originalFaces;			///< �O�p�`�����O�̖ʂɑΉ����钸�_
	std::vector<unsigned int> originalFaceIds;	///< �O�p�`�ɑΉ����镪���O�̖ʂ̔ԍ�
	std::vector<int> faceNormals;				///< �@���C���f�b�N�X

	GRMesh(const GRMeshDesc& desc=GRMeshDesc());
	~GRMesh();
	void Render(GRRenderIf* r);
	void Rendered(GRRenderIf* r);
	
	virtual bool AddChildObject(ObjectIf* o);
	///	�q�I�u�W�F�N�g�̐�
	virtual size_t NChildObject() const ;
	///	�q�I�u�W�F�N�g�̎擾
	virtual ObjectIf* GetChildObject(size_t pos) ;
	///
	void Print(std::ostream& os) const ;

};

}
#endif
