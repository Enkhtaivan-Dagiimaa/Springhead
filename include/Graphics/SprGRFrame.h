/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 *	@file SprGRFrame.h
 *	@brief �V�[���O���t�̃m�[�h
*/

/**	\addtogroup	gpGraphics	*/
//@{

#ifndef SPR_GRFrame_H
#define SPR_GRFrame_H
#include <SprFoundation.h>

namespace Spr{;

struct GRRenderIf;

/** @brief �r�W���A���̃f�B�X�N���v�^ */
struct GRVisualDesc{
};

/**	@brief	�O���t�B�b�N�X�ŕ\���ɉe����^�������	*/
struct GRVisualIf: public NamedObjectIf{
	IF_DEF(GRVisual);
	///	�����_�����O�D�q�m�[�h�C��m�[�h�̃����_�����O���O�ɌĂ΂��D
	virtual void Render(GRRenderIf* r)=0;
	///	�����_�����O�I�������D�q�m�[�h�C��m�[�h�̃����_�����O���I����Ă���Ă΂��D
	virtual void Rendered(GRRenderIf* r)=0;
};
		
///	DirectX �� Frame �̕ϊ���\�� FrameTransformMatrix �m�[�h��ǂނ��߂�Desc
struct GRFrameTransformMatrix{
	Affinef transform;
};

	
/**	@brief	�O���t�B�b�N�X�V�[���O���t�̃c���[�̃m�[�h�D���W�n�����D*/
struct GRFrameIf: public GRVisualIf{
	IF_DEF(GRFrame);

	/** @brief �e�t���[�����擾���� */
	virtual GRFrameIf* GetParent()=0;

	/** @brief �e�t���[����ݒ肷�� */
	virtual void SetParent(GRFrameIf* fr)=0;

	/** @brief �q�m�[�h�̐����擾 */
	virtual int NChildren() = 0;

	/** @brief �q�m�[�h�̔z����擾 */
	virtual GRVisualIf** GetChildren() = 0;

	/** @brief �e�m�[�h�Ƃ̑��Εϊ����擾 */
	virtual Affinef GetTransform()=0;

	/** @brief ���[���h�t���[���Ƃ̑��Εϊ����擾 */
	virtual Affinef GetWorldTransform()=0;

	/** @brief �e�m�[�h�Ƃ̑��Εϊ���ݒ� */
	virtual void SetTransform(const Affinef& af)=0;

	virtual void Print(std::ostream& os) const =0;
};
///	@brief GRFrame ��Desc�D���W�n���w�肷��
struct GRFrameDesc:public GRVisualDesc{
	DESC_DEF_FOR_OBJECT(GRFrame);
	Affinef transform;
};


/**	@brief	�A�j���[�V����(GRFrame�̃c���[�𓮂���)	*/
struct GRAnimationIf: public SceneObjectIf{
	IF_DEF(GRAnimation);
	///	�A�j���[�V�������Ǝ����Œ�܂�{�[���̕ύX���C�d�݂����Č��݂̃{�[���̕ϊ��s��ɓK�p����D
	virtual void BlendPose(float time, float weight)=0;
	///	�{�[���̕ϊ��s��������l�ɖ߂��D
	virtual void ResetPose()=0;
	///	�t���[���̕ϊ��s��������l�ɖ߂��D
	virtual void LoadInitialPose()=0;
};
///	@brief �A�j���[�V�����̃f�X�N���v�^ X��Animation�ɑΉ�
struct GRKey{
	unsigned time;				//	����
	std::vector<float> values;	//	�ϊ��̒l
};
///	�����ƕϊ��̑Ή��\ X��AnimationKey�ɑΉ�
struct GRAnimationKey{
	int keyType;			//	�ϊ��̎��
	std::vector<GRKey> keys;		//	�����ƕϊ��̃Z�b�g
};
struct GRAnimationDesc{
	DESC_DEF_FOR_OBJECT(GRAnimation);
	enum KeyType { ROTATION, SCALE, POSITION, MATRIX };
	///	������AnimationKey�łЂƂ̕ϊ���\��
	std::vector<GRAnimationKey> keys;	
};

/**	@brief	�A�j���[�V����(GRFrame�̃c���[�𓮂���)	*/
struct GRAnimationSetIf: public SceneObjectIf{
	IF_DEF(GRAnimationSet);
	///	�w��̎����̕ϊ��ɏd�݂������āA�{�[��������킷�^�[�Q�b�g�̃t���[���ɓK�p����B
	virtual void BlendPose(float time, float weight)=0;
	///	�t���[���̕ϊ��s��������l�ɖ߂��D
	virtual void ResetPose()=0;
	///	�t���[���̕ϊ��s��������l�ɖ߂��D
	virtual void LoadInitialPose()=0;
};
///	@brief �A�j���[�V�����Z�b�g�̃f�X�N���v�^
struct GRAnimationSetDesc{
	DESC_DEF_FOR_OBJECT(GRAnimationSet);
};

/**	@brief	�A�j���[�V����(GRFrame�̃c���[�𓮂���)	*/
struct GRAnimationControllerIf: public SceneObjectIf{
	IF_DEF(GRAnimationController);
	///	�w��̎����̕ϊ��ɏd�݂������āA�{�[��������킷�^�[�Q�b�g�̃t���[���ɓK�p����B
	virtual void BlendPose(UTString name, float time, float weight)=0;
	///	�t���[���̕ϊ��s��������l�ɖ߂��D
	virtual void ResetPose()=0;
	///	�t���[���̕ϊ��s��������l�ɖ߂��D
	virtual void LoadInitialPose()=0;
};
///	@brief �A�j���[�V�����R���g���[���̃f�X�N���v�^
struct GRAnimationControllerDesc{
	DESC_DEF_FOR_OBJECT(GRAnimationController);
};
//@}
}
#endif
