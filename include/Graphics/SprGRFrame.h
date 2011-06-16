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

#include <Foundation/SprObject.h>

namespace Spr{;

struct GRRenderIf;

/** @brief �r�W���A���̃f�B�X�N���v�^ */
struct GRVisualDesc{
};

/**	@brief	�O���t�B�b�N�X�ŕ\���ɉe����^�������	*/
struct GRVisualIf: public SceneObjectIf{
	SPR_IFDEF(GRVisual);
	///	�����_�����O�D�q�m�[�h�C��m�[�h�̃����_�����O���O�ɌĂ΂��D
	void Render(GRRenderIf* r);
	///	�����_�����O�I�������D�q�m�[�h�C��m�[�h�̃����_�����O���I����Ă���Ă΂��D
	void Rendered(GRRenderIf* r);

	/// �����_�����O�̗L����/������
	void Enable(bool on = true);
	/// �L��/������Ԃ̎擾
	bool IsEnabled();

};
		
///	DirectX �� Frame �̕ϊ���\�� FrameTransformMatrix �m�[�h��ǂނ��߂�Desc
struct GRFrameTransformMatrix{
	Affinef transform;
};

	
/**	@brief	�O���t�B�b�N�X�V�[���O���t�̃c���[�̃m�[�h�D���W�n�����D*/
struct GRFrameIf: public GRVisualIf{
	SPR_IFDEF(GRFrame);

	/** @brief �e�t���[�����擾���� */
	GRFrameIf* GetParent();

	/** @brief �e�t���[����ݒ肷�� */
	void SetParent(GRFrameIf* fr);

	/** @brief �q�m�[�h�̐����擾 */
	int NChildren();

	/** @brief �q�m�[�h�̔z����擾 */
	GRVisualIf** GetChildren();

	/** @brief �e�m�[�h�Ƃ̑��Εϊ����擾 */
	Affinef GetTransform();

	/** @brief ���[���h�t���[���Ƃ̑��Εϊ����擾 */
	Affinef GetWorldTransform();

	/** @brief �e�m�[�h�Ƃ̑��Εϊ���ݒ� */
	void SetTransform(const Affinef& af);

	/** @brief ���b�V�����폜���� */
	void Clear();

	void Print(std::ostream& os) const ;
};
///	@brief GRFrame ��Desc�D���W�n���w�肷��
struct GRFrameDesc:public GRVisualDesc{
	SPR_DESCDEF(GRFrame);
	Affinef transform;
};


/**	@brief	�O���t�B�N�X�̃V�[���O���t�̃_�~�[�m�[�h�D
	��Ńv���O��������g�����߂ɁCVisual�����Ă������߂̃R���e�i�D
	�`��Ȃǂ����Ȃ��̂ň��S���Ă��܂��Ă�����D	*/
struct GRDummyFrameIf: public GRVisualIf{
	SPR_IFDEF(GRDummyFrame);
};
/**	@brief GRDummyFrame ��Desc�D�_�~�[�t���[���D
	Mesh�Ȃǂ�\���������͂Ȃ����C�Ƃ肠�������[�h�������Ă����C
	��Ńv���O�����ŎQ�Ƃ������ꍇ�C�_�~�[�t���[���ɓ���Ă�����
	���ʂȕ`�悪����Ȃ��D	*/
struct GRDummyFrameDesc:public GRVisualDesc{
	SPR_DESCDEF(GRDummyFrame);
};
///	@brief �A�j���[�V�����̃f�X�N���v�^ X��Animation�ɑΉ�
struct GRKey{
	unsigned time;				//	����
	std::vector<float> values;	//	�ϊ��̒l
};
///	�����ƕϊ��̑Ή��\ X��AnimationKey�ɑΉ�
struct GRAnimationKey{
	int keyType;				//	�ϊ��̎��
	std::vector<GRKey> keys;	//	�����ƕϊ��̃Z�b�g
};
/**	@brief	�A�j���[�V����(GRFrame�̃c���[�𓮂���)	*/
struct GRAnimationIf: public SceneObjectIf{
	SPR_IFDEF(GRAnimation);
	///	�A�j���[�V�������Ǝ����Œ�܂�{�[���̕ύX���C�d�݂����Č��݂̃{�[���̕ϊ��s��ɓK�p����D
	void BlendPose(float time, float weight);
	///	�{�[���̕ϊ��s��������l�ɖ߂��D
	void ResetPose();
	///	�t���[���̕ϊ��s��������l�ɖ߂��D
	void LoadInitialPose();
	///	AnimationKey���擾����D
	GRAnimationKey GetAnimationKey(int n);
	///	AnimationKey�̐����擾����D
	int NAnimationKey();
	/// ����t��GRKey���폜����
	void DeletePose(float t);
};

struct GRAnimationDesc{
	SPR_DESCDEF(GRAnimation);
	enum KeyType { ROTATION, SCALE, POSITION, MATRIX };
	///	������AnimationKey�łЂƂ̕ϊ���\��
	std::vector<GRAnimationKey> keys;	
};

/**	@brief	�A�j���[�V����(GRFrame�̃c���[�𓮂���)	*/
struct GRAnimationSetIf: public SceneObjectIf{
	SPR_IFDEF(GRAnimationSet);
	///	�w��̎����̕ϊ��ɏd�݂������āA�{�[��������킷�^�[�Q�b�g�̃t���[���ɓK�p����B
	void BlendPose(float time, float weight);
	///	�t���[���̕ϊ��s��������l�ɖ߂��D
	void ResetPose();
	///	�t���[���̕ϊ��s��������l�ɖ߂��D
	void LoadInitialPose();

	///	�q�I�u�W�F�N�g(animations)��Ԃ�
	ObjectIf* GetChildObject(size_t p);
	/// ���݂�GRFrame����|�[�Y���쐬���� t�͎������w��
	void SetCurrentAnimationPose(float t);
	/// ����t��GRKey���폜����
	void DeleteAnimationPose(float t);
	/// �A�j���[�V�����̍ŏI���Ԃ��擾����
	float GetLastKeyTime();
};
///	@brief �A�j���[�V�����Z�b�g�̃f�X�N���v�^
struct GRAnimationSetDesc{
	SPR_DESCDEF(GRAnimationSet);
};

/**	@brief	�A�j���[�V����(GRFrame�̃c���[�𓮂���)	*/
struct GRAnimationControllerIf: public SceneObjectIf{
	SPR_IFDEF(GRAnimationController);
	///	�w��̎����̕ϊ��ɏd�݂������āA�{�[��������킷�^�[�Q�b�g�̃t���[���ɓK�p����B
	void BlendPose(UTString name, float time, float weight);
	///	�t���[���̕ϊ��s��������l�ɖ߂��D
	void ResetPose();
	///	�t���[���̕ϊ��s��������l�ɖ߂��D
	void LoadInitialPose();

	///	GRAnimation�̒ǉ�
	bool AddChildObject(ObjectIf* o);
	///	GRAnimation�̍폜
	bool DelChildObject(ObjectIf* o);
	///	GRAnimation�̐�
	int NChildObject();
	///	GRAnimation�̎擾
	ObjectIf* GetChildObject(size_t p);
	///	GRAnimationSet�̎擾
	GRAnimationSetIf* GetAnimationSet(size_t p);
};
///	@brief �A�j���[�V�����R���g���[���̃f�X�N���v�^
struct GRAnimationControllerDesc{
	SPR_DESCDEF(GRAnimationController);
};
//@}
}
#endif
