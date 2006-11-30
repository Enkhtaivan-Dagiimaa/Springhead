/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef GRBlendMesh_H
#define GRBlendMesh_H

#include <SprGraphics.h>
#include "GRFrame.h"

namespace Spr{;

class GRMaterial;

/** @class GRBlendMesh
	@brief �O���t�B�b�N�X�V�[���O���t�ł̍��W�n��\���D	*/
class GRBlendMesh: public GRVisual,GRBlendMeshIfInit, public GRBlendMeshDesc{
	GRRenderIf* render;							///< �����_���[				
	std::vector<unsigned int> list;				///< �f�B�X�v���C���X�g�̎��ʎq
			
	/// ���_�t�H�[�}�b�g�A�V�F�[�_�t�H�[�}�b�g �ɍ��킹�A�f�B�X�v���C���X�g���쐬����D
	void CreateList(GRRenderIf* r);
public:
	OBJECT_DEF(GRBlendMesh);
	ACCESS_DESC(GRBlendMesh);

	std::vector< UTRef<GRMaterial> > material;				///< �}�e���A��
	std::vector<int>                 materialList;			///< �}�e���A���̃C���f�b�N�X���X�g
	/**
	   faces�ƁA�O�p�`��������O��X�t�@�C���Ŏw�肳�ꂽ�ʂƂ̊֘A�t�� \n
	  �iX�t�@�C�����烍�[�h���ꂽ�ʂ́A���ׂĎO�p�`��������Ă��邽�߁A
	   �ǂ̖ʂ��ǂ̃}�e���A����K�p����̂����f���邽�߂̂��́j			*/
	std::vector<int> originalFaces;				///< �ʂ��\�����邽�߂̒��_�C���f�b�N�X�i�O�p�`�����O�̖ʂɑ΂���C���f�b�N�X�j
	std::vector<unsigned int> elementIndex;		///< �ʂ��\�����邽�߂̒��_�C���f�b�N�X�i�O�p�`������̖ʂɑ΂���C���f�b�N�X�j
	std::vector<int> faceNormals;				///< �@���C���f�b�N�X
	
	std::vector<Affinef>				blendMatrix;			///< �u�����h�ϊ��s��

	
	GRBlendMesh(const GRBlendMeshDesc& desc=GRBlendMeshDesc());
	~GRBlendMesh();
	void Render(GRRenderIf* r);
	void Rendered(GRRenderIf* r);
};
}
#endif
