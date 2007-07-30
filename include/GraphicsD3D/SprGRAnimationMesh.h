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
	
	/** @brief �{�[���̊p�x���i�������f���̊֐߂̊p�x�ȂǂŁj�㏑������
		@param name        �{�[���̖��O
		@param orientation �㏑������p�x
		@param weight      �㏑�����i0�`1, 0�F�L�[�t���[����100%�]��, 1�Forientation��100%�]���j
	*/
	virtual void OverrideBoneOrientation(const std::string& name, const Quaterniond& orientation, double weight)=0;
	
	/** @brief �{�[���̊p�x�Ɛe�{�[������̑��Έʒu�i���[�g�{�[���̏ꍇ�͎��g�̐�Έʒu�j���㏑������
		@param name   �{�[���̖��O
		@param pose   �㏑������ʒu�p�x
		@param weight �㏑�����i0�`1, 0�F�L�[�t���[����100%�]��, 1�Fpose��100%�]���j
	*/
	virtual void OverrideBonePose(const std::string& name, const Posed& pose, double weight)=0;
};


}
#endif
