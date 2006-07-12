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
	std::vector<unsigned int> list;		///< �f�B�X�v���C���X�g�̎��ʎq
	GRRenderIf* render;
	/// ���_�t�H�[�}�b�gGRVertexElement �ɍ��킹�A�f�B�X�v���C���X�g���쐬����
	void CreateList(GRRenderIf* r);
	/// X�t�@�C������ MeshMaterialList�ݒ肳�ꂽ�ꍇ�A�}�e���A������sub mesh�P�ʂŃf�B�X�v���C���X�g���쐬����
	void CreateSubList(void* vtx);
public:
	OBJECT_DEF(GRMesh);
	ACCESS_DESC(GRMesh);
	GRMesh(const GRMeshDesc& desc=GRMeshDesc());
	void Render(GRRenderIf* r);
	void Rendered(GRRenderIf* r);
};
}
#endif
