/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 *	@file SprGRAnimationMesh.h
 *	@brief �L�����N�^�A�j���[�V�����p���b�V��
*/

#ifndef SPR_GRAnimationMesh_H
#define SPR_GRAnimationMesh_H

#include <Graphics/SprGRFrame.h>

namespace Spr{;

///	@brief GRAnimationMesh�̃f�X�N���v�^�D
struct GRAnimationMeshDesc /*: GRVisualDesc*/{
	std::string filename;		///< �ǂݍ��ރt�@�C����(.x)
};


/**	@brief	�O���t�B�b�N�X�Ŏg���L�����N�^�A�j���[�V�����\���p��Mesh */
struct GRAnimationMeshIf: public GRVisualIf{
	IF_DEF(GRAnimationMesh);
	virtual void SetMotion(const std::string& name)=0;		///< ���[�V������I��
	virtual void SetTime(double time)=0;					///< ���[�V�����̎�����ݒ�(�P�ʂ͕b)
};


}
#endif
