/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_CRControllerIf_H
#define SPR_CRControllerIf_H

#include <SprFoundation.h>
#include <SprPhysics.h>

namespace Spr{;

//@{a

struct PHSolidIf;
struct PHHingeJointIf;

// ------------------------------------------------------------------------------

/// �R���g���[���̃C���^�[�t�F�C�X
struct CRControllerIf : SceneObjectIf{
	SPR_IFDEF(CRController);

	/** @brief ���������s��
	*/
	void Init();

	/** @brief ���䏈�������s����
	*/
	void Step();

	/** @breif �L���E������؂�ւ���
	*/
	void SetEnable(bool enable);

	/** @brief �L���E��������Ԃ�
	*/
	bool IsEnabled();
};

/// �R���g���[���̃f�X�N���v�^
struct CRControllerDesc{
	SPR_DESCDEF(CRController);

	CRControllerDesc(){
	}
};

// ------------------------------------------------------------------------------

/// �ዅ�^���R���g���[���̃C���^�[�t�F�C�X
struct CREyeControllerIf : CRControllerIf{
	SPR_IFDEF(CREyeController);

	/** @brief �����_��ݒ肷��
		@param pos �����_�̂R�������W
		@param vel �����_�̈ړ����x�x�N�g��
	*/
	void LookAt(Vec3f pos, Vec3f vel);

	/** @brief �T�b�P�[�h�����ǂ�����Ԃ�
	*/
	bool IsSaccading();
};

/// �ዅ�^���R���g���[���̃X�e�[�g
struct CREyeControllerState{
	/// ����̏��
	enum ControlState{
		CS_SACCADE, CS_PURSUIT,
	} controlState; 
};

/// �ዅ�^���R���g���[���̃f�X�N���v�^
struct CREyeControllerDesc: public CREyeControllerState, public CRControllerDesc{
	SPR_DESCDEF(CREyeController);

	CREyeControllerDesc(){
	}
};

// ------------------------------------------------------------------------------

/// ��^���R���g���[���̃C���^�[�t�F�C�X
struct CRNeckControllerIf : CRControllerIf{
	SPR_IFDEF(CRNeckController);

	/** @brief �����_��ݒ肷��
		@param pos �����_�̂R�������W
		@param vel �����_�̈ړ����x�x�N�g��
	*/
	void LookAt(Vec3f pos, Vec3f vel, float attractiveness);
};

/// ��^���R���g���[���̃X�e�[�g
struct CRNeckControllerState{
};

/// ��^���R���g���[���̃f�X�N���v�^
struct CRNeckControllerDesc: public CRNeckControllerState, public CRControllerDesc{
	SPR_DESCDEF(CRNeckController);

	/// ��𓮂����͂��߂�Attractiveness��臒l
	float lowerAttractiveness;
	/// ������S�ɑΏۂɌ�����Attractiveness��臒l
	float upperAttractiveness;

	CRNeckControllerDesc(){
		lowerAttractiveness = -1.0;
		upperAttractiveness =  0.0;
	}
};

// ------------------------------------------------------------------------------

/// ���B�^���R���g���[���̃C���^�[�t�F�[�X
struct CRReachingControllerIf : CRControllerIf{
	SPR_IFDEF(CRReachingController);

	/** @brief �ڕW�ʒu�̍Đݒ�
		@param pos �ڕW�ʒu
	*/
	void SetPos(Vec3f pos);

	/** @brief �ʒu�𓞒B������
		@param pos �ڕW�ʒu
		@param v �ڕW���x
		@param t �ڕW���B����
	*/
	void Start(Vec3d pose, Vec3d v, float t);

	/** @brief ���B��Ԃ���������
	*/
	void Stop();

	/** @brief IK����_�̐ݒ�
	*/
	void SetIKCP(PHIKPosCtlIf* ikcp);

	/** @brief IK����_�̎擾
	*/
	PHIKPosCtlIf* GetIKCP();
};

/// ���B�^���R���g���[���̃f�X�N���v�^
struct CRReachingControllerDesc : public CRControllerDesc{
	SPR_DESCDEF(CRReachingController);
};

/// ���B�^���R���g���[���̏W���̂̃C���^�[�t�F�[�X
struct CRReachingControllersIf : CRControllerIf{
	SPR_IFDEF(CRReachingControllers);

	/** @brief ���B�^���R���g���[�����擾����
		@param solid ���B�Ώۂ̍���
	*/
	CRReachingControllerIf* GetReachingController(PHSolidIf* solid);
};

/// ���B�^���R���g���[���̃f�X�N���v�^
struct CRReachingControllersDesc : public CRControllerDesc{
	SPR_DESCDEF(CRReachingControllers);
};

// ------------------------------------------------------------------------------

/// �����R���g���[���i�ዅ�^���E��^���̃R���g���[���̏�ʋ@�\�j�̃C���^�[�t�F�C�X
struct CRGazeControllerIf : CRControllerIf{
	SPR_IFDEF(CRGazeController);

	/** @brief �����_��ݒ肷��
		@param pos �����_�̂R�������W
		@param vel �����_�̈ړ����x�x�N�g��
		@param attractiveness ���ӂ������x����
	*/
	void LookAt(Vec3f pos, Vec3f vel, float attractiveness);
};

/// �����R���g���[���̃X�e�[�g
struct CRGazeControllerState{
};

/// �����R���g���[���̃f�X�N���v�^
struct CRGazeControllerDesc: public CRGazeControllerState, public CRControllerDesc{
	SPR_DESCDEF(CRGazeController);

	CRGazeControllerDesc(){
	}
};

// ------------------------------------------------------------------------------

/// ���ӃR���g���[���̃C���^�t�F�[�X
struct CRAttentionControllerIf : CRControllerIf{
	SPR_IFDEF(CRAttentionController);

};

/// ���ӃR���g���[���̃f�X�N���v�^
struct CRAttentionControllerDesc : public CRControllerDesc{
	SPR_DESCDEF(CRAttentionController);

	CRAttentionControllerDesc(){
	}
};

// ------------------------------------------------------------------------------
/// �|�ꂽ�痧�Ƃ��Ɠw�͂���R���g���[���̃C���^�t�F�[�X
struct CRTryStandingUpControllerIf : CRControllerIf{
	SPR_IFDEF(CRTryStandingUpController);

	/** @brief ���������s��
	*/
	void Init();

	/** @brief ����̃V�~�����[�V����������
	*/
	void Step();

	/** @brief ��`�q���̃V���N����������
	*/
	void Sync() ;

};
/// �|�ꂽ�痧�Ƃ��Ɠw�͂���R���g���[���̃f�X�N���v�^
struct CRTryStandingUpControllerDesc : public CRControllerDesc{
	SPR_DESCDEF(CRTryStandingUpController);
	
	CRTryStandingUpControllerDesc(){
	}
};

// ------------------------------------------------------------------------------
/// ���s�R���g���[���̃C���^�t�F�[�X
struct CRWalkingControllerIf : CRControllerIf{
	SPR_IFDEF(CRWalkingController);

	/** @brief ���s�̑��x��ݒ肷��
	*/
	void SetSpeed(float speed);

	/** @brief �]��p�x��ݒ肷��
	*/
	void SetRotationAngle(float rot);

	/** @brief ���[���h���W�n�̐i�s�p�x��ݒ肷��
	*/
	void SetRotationWorldCoordinate(double r);

	/** @brief �Î~����
	*/
	void Stop();

	/** @brief �������ɕ���
	*/
	void Reverse();

	/** @brief �ʒu��ݒ肷��i������Ԍ���̂��߁j
	*/
	void SetPos(Vec3f pos);

	/** @brief ��{���s�������擾
	*/
	double GetBasicCycle();

	/** @brief ���s�̌p�����s�\���H
	*/
	bool IsCompleteFall();
};

/// ���s�R���g���[���̃f�X�N���v�^
struct CRWalkingControllerDesc : public CRControllerDesc{
	SPR_DESCDEF(CRWalkingController);

	/// ���s�R���g���[���̓��������߂�p�����[�^�������ɋL�q�D
	/// ���s���ɕω���������̂�C�ق��̗v�f�iBody�Ȃǁj����v�Z�ŋ��߂���̂͏����D
	//double paramVelocityX, paramVelocityZ;              //X, Z�����̖ڕW���s���x 
    //double paramHalfStrideX, paramHalfStrideZ;          //X, Z�����̖ڕW����          
    double paramLdx, paramLdz;                          //�����x�����Ԃɐi��X, Z�����̋��� 
    double maxHalfStride;                               //�����̌��E�l 
    double height;                                      //�ڕW�d�S�� 
    double minCycleRate;                                //�ŒZ���n���Ԃ̊�{�����ɑ΂��銄�� 
    double maxDSRate;                                   //�����x�����Ԃ̍ő剄������ 
	double FootLength;                                  //�W����Ԃ̋r�̒���
    double MaxFootLength;                               //���s���̋r�̍Œ��l
    double MinFootLength;                               //���s���̋r�̍ŒZ�l
    double MaxRoGround;                                 //���s���p�������̊p�x��臒l
	double MaxRoLandingSiteFront;                       //���n���ێ��\�ȏ�̂Ƌr�̊p�x��臒l
	double MaxRoLandingSiteSide;                        //���n���ێ��\�ȏ�̂Ƌr�̊p�x��臒l
	double MaxRoLandingSiteBack;                        //���n���ێ��\�ȏ�̂Ƌr�̊p�x��臒l
    double MaxRoConstraint;                             //��̂Ƌr�̊p�x�̌��E�l
	double MaxFootSpeedFront;                           //���̈ړ����x�̌��E�l
	double MaxFootSpeedSide;                            //���̈ړ����x�̌��E�l
	double MaxFootSpeedBack;                            //���̈ړ����x�̌��E�l
	double MaxFootAccelerationFront;                    //���̉����x�̍ő�l
	double MaxFootAccelerationSide;                     //���̉����x�̍ő�l
	double MaxFootAccelerationBack;                     //���̉����x�̍ő�l
    double DoubleSupportLimitRate;                      //�o�����X�����ꂽ���ɗ��r�x�����Ԃ̒������肷��p�����[�^
    double LimitChange;                                 //�ǂ�قǃo�����X��������Ƃ��ɗ��r�x�����Ԃ��k�߂邩���肷��臒l
	double pi;                                          //��
	double footsize;
	double miu;

	CRWalkingControllerDesc(){
		/// �e�ϐ��Ƀf�t�H���g�̒l������D�[���Ƃ�NULL�ł������D
		//paramVelocityX = 0.6;           
		//paramHalfStrideX = 0.115;            
		//paramHalfStrideZ = 0.015;          
		paramLdx = 0.00001;             
		paramLdz = 0.0000001;                 
		maxHalfStride = 0.83;                   
		height = 1.1;                   
		minCycleRate = 0.55;                
		maxDSRate = 1.5;     
		FootLength = 0.85;
		MaxFootLength = 1.05;           
		MinFootLength = 0.35;         
		MaxRoGround = 3.14 / 2.0;    
		MaxRoLandingSiteFront = 3.14 / 2.2; 
	    MaxRoLandingSiteSide = 3.14 / 4.0; 
	    MaxRoLandingSiteBack = 3.14 / 3.5; 
		MaxRoConstraint = 3.14 / 2.2; 
		MaxFootSpeedFront = 50.0;    
	    MaxFootSpeedSide = 35.0;
	    MaxFootSpeedBack = 40.0;  
	    MaxFootAccelerationFront = 50.0;  
	    MaxFootAccelerationSide = 35.0;
	    MaxFootAccelerationBack = 40.0; 
		DoubleSupportLimitRate = 0.05;    
		LimitChange = 11.0;  
		pi = 3.1415926535;
		footsize = 0.0619;
		miu = 0.8;
	}
};

// ------------------------------------------------------------------------------
/// �U���s�R���g���[���F�{���̕��s�R���g���[�����ł���܂ł̒��p��
struct CREseWalkingControllerIf : CRControllerIf{
	SPR_IFDEF(CREseWalkingController);

	/** @brief ���s�̑��x��ݒ肷��
	*/
	void SetSpeed(float speed);

	/** @brief �]��p�x��ݒ肷��
	*/
	void SetRotationAngle(float rot);

	/** @brief �ʒu��ݒ肷��i������Ԍ���̂��߁j
	*/
	void SetPos(Vec3f pos);
};

/// �U���s�R���g���[���̃f�X�N���v�^
struct CREseWalkingControllerDesc : public CRControllerDesc{
	SPR_DESCDEF(CREseWalkingController);

	CREseWalkingControllerDesc(){
	}
};

// ------------------------------------------------------------------------------
/// �ړ��R���g���[���F�ڕW�n�_�ւ̈ړ��Ə�Q�����
struct CRTravelControllerIf : CRControllerIf{
	SPR_IFDEF(CRTravelController);

	/** @brief �ڕW�n�_��ݒ肷��
	*/
	void SetGoal(Vec3f goal);
};

/// �ړ��R���g���[���̃f�X�N���v�^
struct CRTravelControllerDesc : public CRControllerDesc{
	SPR_DESCDEF(CRTravelController);

	CRTravelControllerDesc(){
	}
};

// ------------------------------------------------------------------------------
/// �c���R���g���[��
struct CRGrabControllerIf : CRControllerIf{
	SPR_IFDEF(CRGrabController);

	/** @brief ���̂̏��֎��L�΂��Ă��ށD
	*** @return true: Reach�J�n�����D false: ���̕��̂ւ͎肪�͂��Ȃ��D
	*/
	bool Reach(PHSolidIf* solid, float radius);

	/** @brief �Ώ�Solid��Reach���\���ǂ�����Ԃ��i�����Ƃ��j
	*/
	bool IsReachable(PHSolidIf* solid);

	/** @brief �Ώ�Solid��Reach���\���ǂ�����Ԃ��i�����Ƃ��j
	*** @param safety ���S�W���F1�ȉ��̌W���C������safety�{���Čv�Z�D1.0�̂Ƃ����肬�蓞�B�\
	*/
	bool IsReachable(PHSolidIf* solid, float safety);

	/** @brief Reach�������������ǂ�����Ԃ�
	*/
	bool IsReachComplete();

	/** @brief ���ݕ��̂�����ł���΁C���̕��̂��茳�Ɉ����񂹕ێ�����D
	*** @return true: Uphold�J�n�����D false: ���̂�����ł��Ȃ�(Reach�������܂ށD)
	*/
	bool Uphold();

	/** @brief Uphold���\���ǂ�����Ԃ�
	*/
	bool IsUpholdable();

	/** @brief Uphold�������������ǂ�����Ԃ�
	*/
	bool IsUpholdComplete();

	/** @brief ���ݕ��̂�����ł���΁C���̕��̂����̏ꏊ�ɒu���D
	*** @return true: Place�J�n�����D false: ���̏ꏊ�ւ͎肪�͂��Ȃ��C�܂��͕��̂������ĂȂ��D
	*/
	bool Place(Vec3d pos);

	/** @brief Place���\���ǂ�����Ԃ�
	*/
	bool IsPlaceable(Vec3d pos);

	/** @brief Place���\���ǂ�����Ԃ�
	*** @param safety ���S�W���F1�ȉ��̌W���C������safety�{���Čv�Z�D1.0�̂Ƃ����肬�蓞�B�\
	*/
	bool IsPlaceable(Vec3d pos, float safety);

	/** @brief Place�������������ǂ�����Ԃ�
	*/
	bool IsPlaceComplete();

	/** @brief ���݂̓���𒆒f����
	*/
	void Abort();

	/** @brief ���ׂĂ̔c������𒆒f����
	*/
	void AbortAll();

	/** @brief ���݂̓����Ԃ�Ԃ�
	*/
	enum CRGCControlState {
		CRGC_STANDBY=0,
		CRGC_REACH,  CRGC_REACH_COMPLETE,
		CRGC_UPHOLD, CRGC_UPHOLD_COMPLETE,
		CRGC_PLACE,  CRGC_PLACE_COMPLETE,
	};
	CRGrabControllerIf::CRGCControlState GetControlState();
};

/// �c���R���g���[���̃f�X�N���v�^
struct CRGrabControllerDesc : public CRControllerDesc {
	SPR_DESCDEF(CRGrabController);

	/// �̂��_�炩�����邽�߂̃o�l�_���p�ւ̌W��
	double rateSpringSoften, rateDamperSoften;

	/// �̂����������邽�߂̃o�l�_���p�ւ̌W��
	double rateSpringHarden, rateDamperHarden;

	CRGrabControllerDesc(){
		rateSpringSoften = 0.0;  rateDamperSoften = 0.5;
		rateSpringHarden = 1.5;  rateDamperHarden = 1.5;
	}
};

//@}

}

#endif//SPR_CRControllerIf_H
