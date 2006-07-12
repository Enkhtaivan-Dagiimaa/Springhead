/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 *	@file SprGRMesh.h
 *	@brief ���b�V��
*/

/**	\addtogroup	gpGraphics	*/
//@{

#ifndef SPR_GRMesh_H
#define SPR_GRMesh_H

#include <Graphics/SprGRFrame.h>
#include <Springhead.h>

namespace Spr{;

///	@brief �\���p��Mesh(GRMesh)�̃f�X�N���v�^�D
struct GRMeshDesc{
	std::vector<Vec3f> positions;				///< ���_�̍��W
	std::vector<Vec3f> normals;					///< ���_�̖@��
	std::vector<Vec4f> colors;					///< ���_�̐F
	std::vector<Vec2f> texCoords;				///< �e�N�X�`��UV
	std::vector<size_t> faces;					///< �ʂ��\�����钸�_�̔ԍ��iX�t�@�C������̃��[�h�̏ꍇ�A�l�p�`�͎O�p�`�ɕ����j

	std::vector<GRMaterialDesc> material;		///< �}�e���A��
	std::vector<int> materialList;				///< �}�e���A���̃C���f�b�N�X���X�g�iX�t�@�C����MeshMaterialList�̔ԍ������̂܂܃��[�h�j
	/**
	   faces�ƁA�O�p�`��������O��X�t�@�C���Ŏw�肳�ꂽ�ʂƂ̊֘A�t��		\n
	  �i1 mesh ���}�e���A�����ɂ���� mesh �������A�e sub mesh �ɑ΂��A  �@	\n
�@�@�@�@�@�@�@ �}�e���A���ݒ�G												\n
�@�@�@�@�@�@�@ DisplayList�Ń����_�����O�G									\n
		���J��Ԃ��BX�t�@�C�����烍�[�h���ꂽ�ʂ́A���ׂĎO�p�`��������邽�߁A
		�ǂ̖ʂ��ǂ̃}�e���A����K�p����̂����f���邽�߂ɁAelementIndex ���K�v�B�j */
	std::vector<size_t> elementIndex;	
};


/**	@brief	�O���t�B�b�N�X�Ŏg���\���p��Mesh */
struct GRMeshIf: public GRVisualIf{
	IF_DEF(GRMesh);
};


//@}
}
#endif
