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
	std::vector<Vec3f> positions;		///<	���_�̍��W
	std::vector<Vec3f> normals;			///<	���_�̖@��
	std::vector<Vec4f> colors;			///<	���_�̐F
	std::vector<Vec2f> texCoords;		///<	�e�N�X�`��UV
	std::vector<size_t> faces;		///<	�ʂ��\�����钸�_�̔ԍ��D
};


/**	@brief	�O���t�B�b�N�X�Ŏg���\���p��Mesh */
struct GRMeshIf: public GRVisualIf{
	IF_DEF(GRMesh);
};


//@}
}
#endif
