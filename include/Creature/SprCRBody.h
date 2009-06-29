/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_CRBODYIF_H
#define SPR_CRBODYIF_H

#include <SprFoundation.h>
#include <SprPhysics.h>

namespace Spr{;

//@{

struct PHSolidIf;
struct PHJointIf;

// ------------------------------------------------------------------------------

/// �N���[�`���̃{�f�B���\������v�f
struct CRBodyPartIf : SceneObjectIf {
	SPR_IFDEF(CRBodyPart);

	/** @brief ���x���i����������������FHand�Ȃǁj�̎擾
	 */
	const char* GetLabel() const;

	/** @brief ���x���i����������������FHand�Ȃǁj�̐ݒ�
	 */
	void SetLabel(const char* str);
};
struct CRBodyPartDesc {
	SPR_DESCDEF(CRBodyPart);
	std::string	label;			///<	���x���i����������������FHand�Ȃǁj
};

/// �N���[�`���̃{�f�B���\�����鍄��
struct CRSolidIf : CRBodyPartIf {
	SPR_IFDEF(CRSolid);

	/** @brief PHSolid���擾
	 */
	PHSolidIf* GetPHSolid();

	/** @brief PHSolid��ݒ�
	 */
	void SetPHSolid(PHSolidIf* so);
};
struct CRSolidDesc : CRBodyPartDesc {
	SPR_DESCDEF(CRSolid);
};

/// �N���[�`���̃{�f�B���\������IK����_�t���̍���
struct CRIKSolidIf : CRSolidIf {
	SPR_IFDEF(CRIKSolid);

	/** @brief IK����_�̐����擾
	 */
	int NIKControlPoints();

	/** @brief i�Ԗڂ�IK����_���擾
	 */
	PHIKControlPointIf* GetIKControlPoint(int i);

	/** @brief IK����_��ǉ�
	 */
	void AddIKControlPoint(PHIKControlPointIf* ikCP);
};
struct CRIKSolidDesc : CRSolidDesc {
	SPR_DESCDEF(CRSolid);
};

/// �N���[�`���̃{�f�B���\������֐�
struct CRJointIf : CRBodyPartIf {
	SPR_IFDEF(CRJoint);

	/** @brief PHJoint���擾
	 */
	PHJointIf* GetPHJoint();

	/** @brief PHJoint��ݒ�
	 */
	void SetPHJoint(PHJointIf* jo);
};
struct CRJointDesc : CRBodyPartDesc {
	SPR_DESCDEF(CRJoint);
};

/// �N���[�`���̃{�f�B���\������IK�m�[�h�t���̊֐�
struct CRIKJointIf : CRJointIf {
	SPR_IFDEF(CRIKJoint);

	/** @brief IK�m�[�h�̐����擾
	 */
	int NIKNodes();

	/** @brief i�Ԗڂ�IK�m�[�h���擾
	 */
	PHIKNodeIf* GetIKNode(int i);

	/** @brief IK�m�[�h��ǉ�
	 */
	void AddIKNode(PHIKNodeIf* ikNd);
};
struct CRIKJointDesc : CRJointDesc {
	SPR_DESCDEF(CRJoint);
};

// ------------------------------------------------------------------------------

/// �N���[�`���̃{�f�B���f���̃C���^�[�t�F�C�X
struct CRBodyIf : SceneObjectIf{
	SPR_IFDEF(CRBody);

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
