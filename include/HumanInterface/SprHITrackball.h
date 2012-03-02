/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_HI_TRACKBALL_H
#define SPR_HI_TRACKBALL_H

#include <HumanInterface/SprHIDevice.h>
#include <HumanInterface/SprHIKeyMouse.h>

namespace Spr {;

/** @brief �g���b�N�{�[���C���^�t�F�[�X
	�}�E�X����ɂ�蒍���_����̌����Ƌ����𑀍삷�邱�Ƃ��ł���D
	��ɃJ�����̑���ɗ��p���邪�C�I�u�W�F�N�g�̑���ɂ����p�ł��邩���m��Ȃ��D
	�g���b�N�{�[���ƃ^�[���e�[�u����2���[�h������C�}�E�X�ړ��ʂ���J������]�ւ̃}�b�s���O���@���قȂ�D
	�g���b�N�{�[���ł̓}�E�X��x,y�ړ��ʂ����ꂼ�ꉡ��]�C�c��]�i�X�N���[���Ɋւ��āj�Ƀ}�b�s���O����D
	�^�[���e�[�u���ł̓��[���h���W�n�̌o�x�C�ܓx�̕ω��ʂɃ}�b�s���O����D
	�Y�[������ђ����_�ړ��Ɋւ��Ă͋��ʁD

	motion			default button state	function
	--------------------------------------------------------
	delta x			LBUTTON					(trackball) rotate w.r.t. screen y-axis
											(turntable) change longitude	
	delta y			LBUTTON					(trackball) rotate w.r.t. screen x-axis
											(turntable) change latitude
	delta x			LBUTTON + CTRL			(trackball) rotate w.r.t. screen z-axis
	delta y			RBUTTON					zoom in/out
	delta x			LBUTTON + ALT			move look-at point along screen x-axis
	delta y			LBUTTON + ALT			move look-at point along screen y-axis
 */

struct HITrackballIf;

struct HITrackballDesc{
	bool	trackball;	///< true�Ȃ�g���b�N�{�[���Cfalse�Ȃ�^�[���e�[�u��
	Vec3f	target;		///< �����_

	float	longitude;	///< �o�x[rad]
	float	latitude;	///< �ܓx[rad]
	Vec2f	lonRange;	///< �o�x�͈�
	Vec2f	latRange;	///< �ܓx�͈�
	
	float	distance;	///< ����
	Vec2f	distRange;	///< �����͈�
	
	float	rotGain;	///< ��]�Q�C�� [rad/px]
	float	zoomGain;	///< �Y�[���Q�C��	 [ln/px]
	float	trnGain;	///< ���s�ړ��Q�C��

	int		rotMask;
	int		zoomMask;
	int		trnMask;

	HITrackballDesc(){
		trackball	= false;		///< �f�t�H���g�Ń^�[���e�[�u��
		target		= Vec3f();

		longitude	= Radf(0.0f);
		latitude	= Radf(0.0f);
		lonRange	= Vec2f(Radf(-180.0f), Radf(180.0f));
		latRange	= Vec2f(Radf(- 80.0f), Radf( 80.0f));
		distance	= 1.0f;
		distRange	= Vec2f(0.1f, 100.0f);
		rotGain		= 0.01f;
		trnGain		= 0.001f;
		zoomGain	= 0.1f;

		rotMask		= DVButtonMask::LBUTTON;
		zoomMask	= DVButtonMask::RBUTTON;
		trnMask		= DVButtonMask::LBUTTON + DVButtonMask::ALT;
	}
};

/**	�g���b�N�{�[���̃R�[���o�b�N�N���X
 **/
struct HITrackballCallback{
	/** @brief �����̍X�V
		@param trackball	�Ăяo�����̃g���b�N�{�[��
		@param dir			�X�V�̌����D
							true�Ȃ�΃g���b�N�{�[����pose���R�[���o�b�N���ɔ��f���C
							false�Ȃ�΃R�[���o�b�N����pose���g���b�N�{�[���ɐݒ肷��D
	 */
	virtual void OnUpdatePose(HITrackballIf* trackball, bool dir){}
};

struct GRCameraDesc;

struct HITrackballIf : HIPoseIf{
	SPR_VIFDEF(HITrackball);

	/// ���[�h
	void	SetMode(bool mode);
	bool	GetMode();
	/// �����_
	void	SetTarget(Vec3f t);
	Vec3f	GetTarget();
	/// �o�x�E�ܓx�p
	void	SetAngle(float lon, float lat);
	void	GetAngle(float& lon, float& lat);
	/// �����_����̋���
	void	SetDistance(float dist);
	float	GetDistance();
	/// �o�x�͈�
	void	SetLongitudeRange(float rmin, float rmax);
	void	GetLongitudeRange(float& rmin, float& rmax);
	/// �ܓx�͈�
	void	SetLatitudeRange(float rmin, float rmax);
	void	GetLatitudeRange(float& rmin, float& rmax);
	/// �����͈�
	void	SetDistanceRange(float rmin, float rmax);
	void	GetDistanceRange(float& rmin, float& rmax);
	/// ��]����Q�C��
	void	SetRotGain(float g);
	float	GetRotGain();
	/// �Y�[������Q�C��
	void	SetZoomGain(float g);
	float	GetZoomGain();
	/// �����_�ړ��Q�C��
	void	SetTrnGain(float g);
	float	GetTrnGain();

	/// �J�����p�����[�^�ƃI�u�W�F�N�g�̃T�C�Y���狗����ݒ�
	void	Fit(const GRCameraDesc& cam, float radius);

	/// �w�肳�ꂽ���_�ʒu����o�x�C�ܓx�C�������t�Z
	void	SetPosition(Vec3f pos);

	/// �w�肳�ꂽ�J�����p������o�x�C�ܓx�C�����_���t�Z
	void	SetOrientation(Quaternionf ori);

	/// �}�E�X�ɂ�鑀��̃I��/�I�t
	void	Enable(bool on = true);

	/** @brief	��]����̃{�^���A�T�C���̕ύX
		@param	mask	DVButtonMask�l�̑g�ݍ��킹
		mask��0���w�肷��Ƒ��삪�����������
	 */
	void SetRotMask(int mask);
	/** @brief	�Y�[������̃{�^���A�T�C���̕ύX
	 */
	void SetZoomMask(int mask);
	/** @brief	�p������̃{�^���A�T�C���̕ύX
	 */
	void SetTrnMask(int mask);
	/** @brief	���Z�b�g����̃{�^���A�T�C���̕ύX
	 */
	//void SetResetMask(int mask, bool single_or_double);

	/// �g���b�N�{�[���Ɋ֘A�t�����Ă���}�E�X���擾����
	DVKeyMouseIf*	GetKeyMouse();
	/// �g���b�N�{�[���Ƀ}�E�X���֘A�t����
	void			SetKeyMouse(DVKeyMouseIf* dv);
	/// �R�[���o�b�N��o�^����
	void			SetCallback(HITrackballCallback* callback);
};

}	//	namespace Spr
#endif
