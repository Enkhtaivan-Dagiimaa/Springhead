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

class GRSkinWeight: public GRVisual, public GRSkinWeightDesc{
public:
	SPR_OBJECTDEF(GRSkinWeight);
	ACCESS_DESC(GRSkinWeight);

	/// �q�I�u�W�F�N�g
	GRFrame* frame;

	virtual bool AddChildObject(ObjectIf* o);
	virtual size_t NChildObject() const ;
	virtual ObjectIf* GetChildObject(size_t pos) ;

	GRSkinWeight(const GRSkinWeightDesc& desc = GRSkinWeightDesc());
};

/**	@class	GRMesh
    @brief	�O���t�B�b�N�X�V�[���O���t�ł̍��W�n��\���D */
class GRMesh: public GRVisual, public GRMeshDesc{
protected:
	UTRef<GRRenderIf> render;			///< �����_���[
	unsigned int list;					///< �f�B�X�v���C���X�g�̎��ʎq

	///	���_�t�H�[�}�b�g�ɍ��������_�o�b�t�@
	std::vector<float>	vtxs;
	std::vector<float>	blendedVtxs;
	const GRVertexElement* vtxFormat;
	int stride;			///< 1���_�̃o�C�g��
	int vertexOffset;	///< ���_���W�܂ł̃I�t�Z�b�g
	int normalOffset;	///< �@���܂ł̃I�t�Z�b�g
	int texOffset;		///< �e�N�X�`�����W�܂ł̃I�t�Z�b�g

	Vec3f& Pos(std::vector<float>& v, int i){ return (Vec3f&)v[stride*i + vertexOffset]; }
	Vec3f& Normal(std::vector<float>& v, int i){ return (Vec3f&)v[stride*i + normalOffset]; }

	/// ���_�o�b�t�@�̍쐬
	void MakeBuffer();
	/// ���_�o�b�t�@�̕`��
	void DrawBuffer(void* vtx);

	/// 3�p�`����
	void DecomposeFaces();
	/// �@����������
	void GenerateNormals();

	virtual void AfterSetDesc();
public:
	SPR_OBJECTDEF(GRMesh);
	ACCESS_DESC(GRMesh);

	bool tex3d;												///< 3�����e�N�X�`���̏ꍇ true

	/// �q�I�u�W�F�N�g
	std::vector< UTRef<GRMaterial> >	material;			///< �}�e���A��
	std::vector< UTRef<GRSkinWeight> >	skinWeights;		///< �X�L��Mesh�̏ꍇ��skinWeights

	/**
	   faces�ƁA�O�p�`��������O��X�t�@�C���Ŏw�肳�ꂽ�ʂƂ̊֘A�t�� \n
	  �iX�t�@�C�����烍�[�h���ꂽ�ʂ́A���ׂĎO�p�`��������Ă��邽�߁A
	   �ǂ̖ʂ��ǂ̃}�e���A����K�p����̂����f���邽�߂̂��́j
	 */
	std::vector<size_t>	triFaces;		///< �O�p�`������̖ʂ̒��_�C���f�b�N�X
	std::vector<size_t>	triML;			///< �O�p�`������̃}�e���A���C���f�b�N�X
	//std::vector<int>	orgFaces;		///< �O�p�`�����O�̖ʂɒ��_�C���f�b�N�X
	//std::vector<int>	orgFaceIds;		///< �O�p�`�ɑΉ����镪���O�̖ʂ̔ԍ�

public:
	/// �f�B�X�v���C���X�g���쐬����D
	void CreateList(GRRenderIf* r);

	//float* GetVertex(int i=0){							///< i�Ԗڂ̒��_�o�b�t�@��Ԃ�
	//	return vtxs + stride * i;
	//}
	//int GetNVertex(){ return nVtxs; }						///< ���_��
	const GRVertexElement* GetVertexFormat(){				///< ���_�t�H�[�}�b�g��Ԃ��B
		return vtxFormat;
	}
	void	EnableTex3D(bool on = true){ tex3d = on; }
	bool	IsTex3D(){ return tex3d; }						///< 3�����e�N�X�`���Ȃ� true

	//int GetStride(){ return stride; }						///< 1���_�̃f�[�^��float������
	//int GetNormalOffset(){ return normalOffset; }			///< �@���̃I�t�Z�b�g(float����)
	//int GetPositionOffset(){ return positionOffset; }		///< �ʒu�̃I�t�Z�b�g(float����)
	//int GetTexOffset(){ return texOffset; }					///< �e�N�X�`�����W�̃I�t�Z�b�g(float����)

	GRMesh(const GRMeshDesc& desc=GRMeshDesc());
	~GRMesh();
	void Render(GRRenderIf* r);
	void Rendered(GRRenderIf* r);

	int		NVertex(){ return (int)vertices.size(); }
	int		NFace(){ return (int)faces.size(); }
	int		NTriangle(){ return (int)triFaces.size()/3; }
	
	Vec3f*		GetVertices(){ return &vertices[0]; }
	Vec3f*		GetNormals(){ return &normals[0]; }
	Vec4f*		GetColors(){ return &colors[0]; }
	Vec2f*		GetTexCoords(){ return &texCoords[0]; }
	GRMeshFace*	GetFaces(){ return &faces[0]; }
	GRMeshFace*	GetFaceNormals(){ return &faceNormals[0]; }
	unsigned*	GetTriangles(){ return &triFaces[0]; }
	int*		GetMaterialIndices(){ return &materialList[0]; }
	void		SwitchCoordinate();

	GRSkinWeightIf* CreateSkinWeight(const GRSkinWeightDesc& desc);
	
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
