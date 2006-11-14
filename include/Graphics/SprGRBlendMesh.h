/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 *	@file SprGRBlendMesh.h
 *	@brief �X�L�����b�V���ȂǂŃx�[�X�ƂȂ钸�_�u�����f�B���O�p���b�V��
*/

/**	\addtogroup	gpGraphics	*/
//@{

#ifndef SPR_GRBlendMesh_H
#define SPR_GRBlendMesh_H

#include <Graphics/SprGRFrame.h>

namespace Spr{;

///	@brief �\���p��BlendMesh(GRBlendMesh)�̃f�X�N���v�^�D
struct GRBlendMeshDesc{
	std::vector<Vec3f> 	positions;				///< ���_�̍��W
	std::vector<Vec3f> 	normals;				///< ���_�̖@��
	std::vector<Vec4f> 	colors;					///< ���_�̐F
	std::vector<Vec2f> 	texCoords;				///< �e�N�X�`��UV
	std::vector<size_t> faces;					///< �ʂ��\�����钸�_�C���f�b�N�X�i�l�p�`�͎O�p�`�ɕ����j
	std::vector<float> 	blend;					///< �e���_�̏d��
};


/**	@brief	�O���t�B�b�N�X�Ŏg���\���p��BlendMesh */
struct GRBlendMeshIf: public GRVisualIf{
	IF_DEF(GRBlendMesh);
};


//@}
}
#endif
