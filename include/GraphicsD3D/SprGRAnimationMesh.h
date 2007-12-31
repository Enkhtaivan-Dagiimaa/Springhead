/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
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
#include <windows.h>
#include <d3dx9.h>
#include <vector>

namespace Spr{;

///	@brief GRAnimationMesh�̃f�X�N���v�^�D
struct GRAnimationMeshDesc /*: GRVisualDesc*/{
	std::string filename;		///< �ǂݍ��ރt�@�C����(.x)
};

///	@brief SetBlend()�Ŏg���B
struct GRAnimationMeshBlendData{
	std::string	name;
	double		time;
	float		weight;
};

typedef void (SPR_CDECL *GRAnimationMeshDrawSubsetListenerFunc)(DWORD attribId, void* ptr);

/**	@brief	�O���t�B�b�N�X�Ŏg���L�����N�^�A�j���[�V�����\���p��Mesh */
struct GRAnimationMeshIf: public GRVisualIf{
	IF_DEF(GRAnimationMesh);
	virtual void SetMotion(const std::string& name)=0;		///< ���[�V������I��
	virtual void SetTime(double time)=0;					///< ���[�V�����̎�����ݒ�(�P�ʂ͕b)
	virtual void SetBlend(const std::vector<GRAnimationMeshBlendData>& data)=0;

	/** @brief �L�[�t���[���ɂ��{�[���̊p�x���擾
		@param name   �{�[���̖��O
	*/
	virtual Posed GetBoneKeyframePose(const std::string& name)=0;
	
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
	virtual void OverrideBonePose(const std::string& name, const Posed& pose, double weight, bool posRelative=false)=0;
	
	/** @brief �S�Ẵ{�[���̏㏑���������l�ɂ���
		@param weight �㏑�����i0�`1, 0�F�L�[�t���[����100%�]��, 1�Fpose��100%�]���j
	*/
	virtual void SetAllBonesOverrideWeight(double weight)=0;
	
	/** @brief DrawSubset�̎��s���ɃR�[���o�b�N�����֐���o�^
		@param beforeFunc DrawSubset�����s����钼�O�ɃR�[���o�b�N�����֐�
		@param afterFunc  DrawSubset�����s���ꂽ����ɃR�[���o�b�N�����֐�
		@param ptr        �R�[���o�b�N�֐��Ɉ����Ƃ��ēn�����
	*/
	virtual void AddDrawSubsetListener(GRAnimationMeshDrawSubsetListenerFunc beforeFunc, GRAnimationMeshDrawSubsetListenerFunc afterFunc, void* ptr=NULL)=0;

	/** @brief �`��Ɏg�p����G�t�F�N�g��ݒ�
		@param effect            �`��Ɏg�p����G�t�F�N�g
		@param matrixPaletteSize �{�[���̃��[���h�ϊ����i�[����z��̃T�C�Y
		�G�t�F�N�g���g���ꍇ��Render()�Ăяo����ID3DXEffect::BeginPass()��EndPass()�ŋ���
		�G�t�F�N�g���ɗp�ӂ���p�����[�^�͈ȉ��̂Ƃ���i�^�A�Z�}���e�B�N�X�j
			float4x4						WORLD						// ���[���h�s��
			int								MAXVERTEXINFL				// ��̒��_�ɉe������{�[���̍ő吔�i�X�L�j���O���Ȃ����b�V���ł͂O�j
			float4x3[matrixPaletteSize]		BONEMATRIXARRAY				// �{�[���̕ϊ��s��̔z��i�f���A���N�H�[�^�j�I�����g���Ȃ�s�v�j
			float4[matrixPaletteSize]		BONEQUATERNIONARRAYREAL		// �{�[���̃f���A���N�H�[�^�j�I����real-part�̔z��i�s����g���Ȃ�s�v�j
			float4[matrixPaletteSize]		BONEQUATERNIONARRAYDUAL		// �{�[���̃f���A���N�H�[�^�j�I����dual-part�̔z��i�s����g���Ȃ�s�v�j
			float4							DIFFUSECOLOR				// �}�e���A���̃f�B�t���[�Y�F
			float4							EMISSIVECOLOR				// �}�e���A���̃G�~�b�V�u�F
			texture							TEXTURE						// �e�N�X�`��
			bool							ISTEXTURED					// �e�N�X�`�����g���Ă��邩�ǂ���
		�X�L�j���O���Ȃ����b�V���ł�BONE***�͎g���Ȃ�
	*/
	virtual void SetEffect(LPD3DXEFFECT effect, int matrixPaletteSize)=0;
};


}
#endif
