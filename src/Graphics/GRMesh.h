/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
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

/**	@class	GRMesh
    @brief	�O���t�B�b�N�X�V�[���O���t�ł̍��W�n��\���D */
class GRMesh: public InheritGRVisual<GRMeshIf, GRVisual>, public GRMeshDesc{
	std::vector<unsigned int> list;				///< �f�B�X�v���C���X�g�̎��ʎq
	GRRenderIf* render;
	
	/// ���_�t�H�[�}�b�g GRVertexElement �ɍ��킹�A�f�B�X�v���C���X�g���쐬����
	void CreateList(GRRenderIf* r);
	/// �f�B�X�v���C���X�g�̊Ǘ����s���i1mesh�ɑ΂��Amaterial�𕡐����ꍇ�́Amaterial����List���쐬����j
	void ListManage(void* vtx);
	void LoadTexture();
public:
	OBJECT_DEF(GRMesh);
	ACCESS_DESC(GRMesh);

	std::vector<GRMaterialDesc> material;		///< �}�e���A��
	std::vector<int> materialList;				///< �}�e���A���̃C���f�b�N�X���X�g
	std::vector<GRTextureDesc>  texture;		///< �e�N�X�`��
	/**
	   faces�ƁA�O�p�`��������O��X�t�@�C���Ŏw�肳�ꂽ�ʂƂ̊֘A�t�� \n
	  �iX�t�@�C�����烍�[�h���ꂽ�ʂ́A���ׂĎO�p�`��������Ă��邽�߁A
	   �ǂ̖ʂ��ǂ̃}�e���A����K�p����̂����f���邽�߂̂��́j			*/
	std::vector<unsigned int> elementIndex;
	std::vector<int> originalFaces;			///< �ʂ��\�����邽�߂̒��_�C���f�b�N�X�i�O�p�`�����O�̖ʂɑ΂���C���f�b�N�X�j
	std::vector<int> faceNormals;			///< �@���C���f�b�N�X

	GRMesh(const GRMeshDesc& desc=GRMeshDesc());
	void Render(GRRenderIf* r);
	void Rendered(GRRenderIf* r);
};
}
#endif
