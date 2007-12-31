/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
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

/**	
	@brief �\���p��BlendMesh(GRBlendMesh)�̃f�X�N���v�^�D
	
�@�@ �P���_�ɓK�p�ł���u�����h�ϊ��s��́A4�܂ŁD
�@�@ ���̂��߁Ablends�AmatrixIndices �́AVec4f�^�Ƃ��Ă���D
�@�@ �������A�C���f�b�N�X�t���u�����h�ϊ��s��ł��邽�߁A���炩���ߓo�^�ł���u�����h�ϊ��s��ɐ����͂Ȃ��D
	
 */
struct GRBlendMeshDesc{	
	std::vector<Vec3f> 	positions;			///< ���_�̍��W
	std::vector<Vec3f> 	normals;			///< ���_�̖@��
	std::vector<Vec4f> 	colors;				///< ���_�̐F
	std::vector<Vec2f> 	texCoords;			///< �e�N�X�`��UV
	std::vector<size_t> faces;				///< �ʂ��\�����钸�_�C���f�b�N�X�i�l�p�`�͎O�p�`�ɕ����j
	std::vector<Vec4f>  blends;				///< �e���_�̏d�݁i1�FN = ���_�Fblend�j
    std::vector<Vec4f>  matrixIndices;		///< �ϊ��s��C���f�b�N�X�i1:N = ���_�Fmatrix�j
	std::vector<Vec4f>  numMatrix;			///< �e���_�ɑ΂���u�����h�ϊ��s��̐�
};


/**	@brief	�O���t�B�b�N�X�Ŏg���\���p��BlendMesh */
struct GRBlendMeshIf: public GRVisualIf{
	IF_DEF(GRBlendMesh);
};


//@}
}
#endif
