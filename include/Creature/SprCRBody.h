/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_CRBODYIF_H
#define SPR_CRBODYIF_H

#include <Foundation/SprObject.h>

namespace Spr{;

//@{

struct CRBodyPartIf;
struct CRSolidIf;
struct CRJointIf;

/// �N���[�`���̃{�f�B���f���̃C���^�[�t�F�C�X
struct CRBodyIf : SceneObjectIf{
	SPR_IFDEF(CRBody);

	/** @brief ���x������\���v�f��T��
	*/
	CRBodyPartIf* FindByLabel(UTString label);

	/** @brief �{�f�B�Ɋւ���X�e�b�v�������s��
	*/
	void Step();

	/** @brief �\�����̂̐��𓾂�
	*/
	int NSolids();

	/** @brief i�Ԗڂ̍\�����̂𓾂�
	*/
	CRSolidIf* GetSolid(int i);

	/** @brief �\���֐߂̐��𓾂�
	*/
	int NJoints();

	/** @brief i�Ԗڂ̍\���֐߂𓾂�
	*/
	CRJointIf* GetJoint(int i);

	/** @brief �{�f�B�̏d�S���W�𓾂�
	*/
	Vec3d GetCenterOfMass();

	/** @brief �{�f�B�̑����ʂ𓾂�
	*/
	double GetSumOfMass();
};

/// �N���[�`���̃{�f�B���f���̃f�X�N���v�^
struct CRBodyDesc{
	SPR_DESCDEF(CRBody);

	CRBodyDesc(){
	}
};

}

#endif//SPR_CRBODY_H
