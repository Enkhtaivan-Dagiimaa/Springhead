/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
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

namespace Spr{;

struct GRMeshFace{
	int	nVertices;	///< 3 or 4
	int	indices[4];
};

struct GRSkinWeightIf: public GRVisualIf{
	SPR_IFDEF(GRSkinWeight);
};

struct GRSkinWeightDesc{
	SPR_DESCDEF(GRSkinWeight);
	Affinef offset;
	std::vector<unsigned> indices;
	std::vector<float> weights;
};

/**	@brief	�O���t�B�b�N�X�Ŏg���\���p��Mesh */
struct GRMeshIf: public GRVisualIf{
	SPR_IFDEF(GRMesh);

	/// ���_�̌�
	int		NVertex();
	/// �ʂ̌�(3�p�`������)
	int		NTriangle();
	/// �ʂ̌�(3�p�`�����O)
	int		NFace();
	
	/// ���_�z��
	Vec3f*	GetVertices();
	/// �@���z��
	Vec3f*	GetNormals();
	/// ���_�F�z��
	Vec4f*	GetColors();
	/// �e�N�X�`�����W�z��
	Vec2f*	GetTexCoords();
	/// ���_�C���f�b�N�X�z��
	GRMeshFace*	GetFaces();
	/// �ʖ@��
	GRMeshFace* GetFaceNormals();
	/// �}�e���A���C���f�b�N�X�z��
	int*	GetMaterialIndices();

	/** @brief �E��/������W�n�̐؂芷��
		���_���W��Z�����̕����𔽓]����D
	 */
	void SwitchCoordinate();

	/// 3D�e�N�X�`����On/Off
	void EnableTex3D(bool on = true);
	bool IsTex3D();

	/// �X�L���E�F�C�g�̍쐬
	GRSkinWeightIf* CreateSkinWeight(const GRSkinWeightDesc& desc);

};

///	@brief �\���p��Mesh(GRMesh)�̃f�X�N���v�^�D
struct GRMeshDesc : GRVisualDesc{
	SPR_DESCDEF(GRMesh);
	std::vector<Vec3f>		vertices;			///< ���_�̍��W
	std::vector<GRMeshFace>	faces;				///< ��. �ʂ��\�����钸�_���ƒ��_�C���f�b�N�X�z��
	std::vector<Vec3f>		normals;			///< �@��
	std::vector<GRMeshFace>	faceNormals;		///< �ʂ̖@��
	std::vector<Vec4f>		colors;				///< ���_�̐F
	std::vector<Vec2f>		texCoords;			///< �e�N�X�`��UV	
	std::vector<int>		materialList;		///< �}�e���A�����X�g
};

//@}
}
#endif
