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
	IF_DEF(CRController);

	/** @brief ���������s��
	*/
	virtual void Init()= 0;

	/** @brief ���䏈�������s����
	*/
	virtual void Step()= 0;

	/** @breif �L���E������؂�ւ���
	*/
	virtual void SetEnable(bool enable)= 0;

	/** @brief �L���E��������Ԃ�
	*/
	virtual bool IsEnabled()= 0;
};

/// �R���g���[���̃f�X�N���v�^
struct CRControllerDesc{
	DESC_DEF_FOR_OBJECT(CRController);

	CRControllerDesc(){
	}
};

// ------------------------------------------------------------------------------

/// �ዅ�^���R���g���[���̃C���^�[�t�F�C�X
struct CREyeControllerIf : CRControllerIf{
	IF_DEF(CREyeController);

	/** @brief �����_��ݒ肷��
		@param pos �����_�̂R�������W
		@param vel �����_�̈ړ����x�x�N�g��
	*/
	virtual void LookAt(Vec3f pos, Vec3f vel)= 0;

	/** @brief �T�b�P�[�h�����ǂ�����Ԃ�
	*/
	virtual bool IsSaccading()= 0;
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
	DESC_DEF_FOR_OBJECT(CREyeController);

	CREyeControllerDesc(){
	}
};

// ------------------------------------------------------------------------------

/// ��^���R���g���[���̃C���^�[�t�F�C�X
struct CRNeckControllerIf : CRControllerIf{
	IF_DEF(CRNeckController);

	/** @brief �����_��ݒ肷��
		@param pos �����_�̂R�������W
		@param vel �����_�̈ړ����x�x�N�g��
	*/
	virtual void LookAt(Vec3f pos, Vec3f vel, float attractiveness)= 0;
};

/// ��^���R���g���[���̃X�e�[�g
struct CRNeckControllerState{
};

/// ��^���R���g���[���̃f�X�N���v�^
struct CRNeckControllerDesc: public CRNeckControllerState, public CRControllerDesc{
	DESC_DEF_FOR_OBJECT(CRNeckController);

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
	IF_DEF(CRReachingController);

	/** @brief ����Ώۂ̍��̂�Ԃ�
	*/
	virtual PHSolidIf* GetSolid()= 0;

	/** @brief �ڕW�ʒu��ݒ肷��
		�i���݂̎����ł́A���B�^���̓r���ł��ڕW�ʒu�̕ύX�͔��f�����B�j
		@param p �ڕW�ʒu
		@param v �ڕW�̑��x
	*/
	virtual void SetTargetPos(Vec3f p, Vec3f v)= 0;

	/** @brief �ڕW�p����ݒ肷��
		�i���݂̎����ł́A���B�^���̓r���ł��ڕW�p���̕ύX�͔��f�����B�j
		@param q �ڕW�p��
		@param av �ڕW�p���x
	*/
	virtual void SetTargetOri(Quaterniond q, Vec3f av)= 0;

	/** @brief ���B�ڕW���Ԃ�ݒ肷��
		@param t �ڕW���B����
	*/
	virtual void SetTargetTime(float t)= 0;

	/** @brief ���B�^�����J�n����
		�i���݂̎����ł́A���B�^���̓r���ł��������ɐV���������ŉ^�����J�n����B�j
		@param mode ���B�̍S�����[�h
		@param keeptime ���B�^���I����ɕێ��𑱂��鎞�ԁi���Ȃ�ێ��𑱂���j
	*/
	enum ConstraintMode {
		CM_P3R0=0,  // �R���R�x�S���i�ʒu�݂̂����킹��j
		CM_P3R2,    // �R�{�Q���R�x�S���i�ʒu�ƌ��������킹�邪�A�����ɂP���̎��R�x������j
		CM_P3R3,    // �R�{�R���R�x�S���i�ʒu�ƌ��������킹��j
	};
	virtual void Start(CRReachingControllerIf::ConstraintMode mode, float keeptime)=0;

	/** @brief ����̎c�莞�Ԃ�Ԃ�
	*/
	virtual float GetRemainingTime()= 0;

	/** @brief ���B�󋵂�Ԃ�
	*/
	enum ReachState {
		RS_NOTHING_REACHED=0,  // �ڕW���̂��Ώۍ��̂����B���Ă��Ȃ�
		RS_TARGET_REACHED,     // �ڕW���͓̂��B�����i�Ώۍ��̂͂܂��ǂ����Ă��Ȃ��j
		RS_SOLID_REACHED,      // �Ώۍ��̂����B����
		RS_STOP,               // ���삵�Ă��Ȃ�
	};
	virtual CRReachingControllerIf::ReachState GetReachState()= 0;

	/** @brief ���B��Ԃ���߂�
	*/
	virtual void Reset()= 0;
};

/// ���B�^���R���g���[���̃f�X�N���v�^
struct CRReachingControllerDesc : public CRControllerDesc{
	DESC_DEF_FOR_OBJECT(CRReachingController);

	PHSolidIf*  solid;    ///< ���B������������
	Vec3f       reachPos; ///< ���̓��̓��B���������|�C���g
	Quaterniond fixOri;   ///< HingeJoint���������

	float limitForce; ///< �͂̍ő�l
	float springPos;  ///< �ʒu����̃o�l�W��
	float damperPos;  ///< �ʒu����̃_���p�W��
	float springOri;  ///< �p������̃o�l�W��
	float damperOri;  ///< �p������̃_���p�W��

	CRReachingControllerDesc(){
		solid = NULL;
		limitForce = 1000.0f;
		springPos = 500.0f;  damperPos =  20.0f;
		springOri = 500.0f;  damperOri =  20.0f;
		reachPos = Vec3f(0,0,0);
		fixOri = Quaterniond();
	}
};

// ------------------------------------------------------------------------------

/// �����R���g���[���i�ዅ�^���E��^���̃R���g���[���̏�ʋ@�\�j�̃C���^�[�t�F�C�X
struct CRGazeControllerIf : CRControllerIf{
	IF_DEF(CRGazeController);

	/** @brief �����_��ݒ肷��
		@param pos �����_�̂R�������W
		@param vel �����_�̈ړ����x�x�N�g��
		@param attractiveness ���ӂ������x����
	*/
	virtual void LookAt(Vec3f pos, Vec3f vel, float attractiveness)= 0;
};

/// �����R���g���[���̃X�e�[�g
struct CRGazeControllerState{
};

/// �����R���g���[���̃f�X�N���v�^
struct CRGazeControllerDesc: public CRGazeControllerState, public CRControllerDesc{
	DESC_DEF_FOR_OBJECT(CRGazeController);

	CRGazeControllerDesc(){
	}
};

// ------------------------------------------------------------------------------

/// ���ӃR���g���[���̃C���^�t�F�[�X
struct CRAttentionControllerIf : CRControllerIf{
	IF_DEF(CRAttentionController);

};

/// ���ӃR���g���[���̃f�X�N���v�^
struct CRAttentionControllerDesc : public CRControllerDesc{
	DESC_DEF_FOR_OBJECT(CRAttentionController);

	CRAttentionControllerDesc(){
	}
};

// ------------------------------------------------------------------------------
/// �|�ꂽ�痧�Ƃ��Ɠw�͂���R���g���[���̃C���^�t�F�[�X
struct CRTryStandingUpControllerIf : CRControllerIf{
	IF_DEF(CRTryStandingUpController);

	/** @brief ���������s��
	*/
	virtual void Init() = 0;

	/** @brief ����̃V�~�����[�V����������
	*/
	virtual void Step() = 0;

	/** @brief ��`�q���̃V���N����������
	*/
	virtual void Sync() =0;

};
/// �|�ꂽ�痧�Ƃ��Ɠw�͂���R���g���[���̃f�X�N���v�^
struct CRTryStandingUpControllerDesc : public CRControllerDesc{
	DESC_DEF_FOR_OBJECT(CRTryStandingUpController);
	
	CRTryStandingUpControllerDesc(){
	}
};

// ------------------------------------------------------------------------------
/// ���s�R���g���[���̃C���^�t�F�[�X
struct CRWalkingControllerIf : CRControllerIf{
	IF_DEF(CRWalkingController);

	/** @brief ���s�̑��x��ݒ肷��
	*/
	virtual void SetSpeed(float speed)= 0;

	/** @brief �]��p�x��ݒ肷��
	*/
	virtual void SetRotationAngle(float rot)= 0;

	/** @brief ���[���h���W�n�̐i�s�p�x��ݒ肷��
	*/
	virtual void SetRotationWorldCoordinate(double r)= 0;

	/** @brief �Î~����
	*/
	virtual void Stop()= 0;

	/** @brief �������ɕ���
	*/
	virtual void Reverse()= 0;

	/** @brief �ʒu��ݒ肷��i������Ԍ���̂��߁j
	*/
	virtual void SetPos(Vec3f pos)= 0;

	/** @brief ��{���s�������擾
	*/
	virtual double GetBasicCycle()= 0;

	/** @brief ���s�̌p�����s�\���H
	*/
	virtual bool IsCompleteFall()= 0;
};

/// ���s�R���g���[���̃f�X�N���v�^
struct CRWalkingControllerDesc : public CRControllerDesc{
	DESC_DEF_FOR_OBJECT(CRWalkingController);

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
	IF_DEF(CREseWalkingController);

	/** @brief ���s�̑��x��ݒ肷��
	*/
	virtual void SetSpeed(float speed)= 0;

	/** @brief �]��p�x��ݒ肷��
	*/
	virtual void SetRotationAngle(float rot)= 0;

	/** @brief �ʒu��ݒ肷��i������Ԍ���̂��߁j
	*/
	virtual void SetPos(Vec3f pos)= 0;
};

/// �U���s�R���g���[���̃f�X�N���v�^
struct CREseWalkingControllerDesc : public CRControllerDesc{
	DESC_DEF_FOR_OBJECT(CREseWalkingController);

	CREseWalkingControllerDesc(){
	}
};

// ------------------------------------------------------------------------------
/// �ړ��R���g���[���F�ڕW�n�_�ւ̈ړ��Ə�Q�����
struct CRTravelControllerIf : CRControllerIf{
	IF_DEF(CRTravelController);

	/** @brief �ڕW�n�_��ݒ肷��
	*/
	virtual void SetGoal(Vec3f goal)= 0;
};

/// �ړ��R���g���[���̃f�X�N���v�^
struct CRTravelControllerDesc : public CRControllerDesc{
	DESC_DEF_FOR_OBJECT(CRTravelController);

	CRTravelControllerDesc(){
	}
};

// ------------------------------------------------------------------------------
/// �c���R���g���[��
struct CRGrabControllerIf : CRControllerIf{
	IF_DEF(CRGrabController);

	/** @brief ���̂̏��֎��L�΂��Ă��ށD
	*** @return true: Reach�J�n�����D false: ���̕��̂ւ͎肪�͂��Ȃ��D
	*/
	virtual bool Reach(PHSolidIf* solid, float radius)= 0;

	/** @brief �Ώ�Solid��Reach���\���ǂ�����Ԃ��i�����Ƃ��j
	*/
	virtual bool IsReachable(PHSolidIf* solid)= 0;

	/** @brief �Ώ�Solid��Reach���\���ǂ�����Ԃ��i�����Ƃ��j
	*** @param safety ���S�W���F1�ȉ��̌W���C������safety�{���Čv�Z�D1.0�̂Ƃ����肬�蓞�B�\
	*/
	virtual bool IsReachable(PHSolidIf* solid, float safety)= 0;

	/** @brief Reach�������������ǂ�����Ԃ�
	*/
	virtual bool IsReachComplete()= 0;

	/** @brief ���ݕ��̂�����ł���΁C���̕��̂��茳�Ɉ����񂹕ێ�����D
	*** @return true: Uphold�J�n�����D false: ���̂�����ł��Ȃ�(Reach�������܂ށD)
	*/
	virtual bool Uphold()= 0;

	/** @brief Uphold���\���ǂ�����Ԃ�
	*/
	virtual bool IsUpholdable()= 0;

	/** @brief Uphold�������������ǂ�����Ԃ�
	*/
	virtual bool IsUpholdComplete()= 0;

	/** @brief ���ݕ��̂�����ł���΁C���̕��̂����̏ꏊ�ɒu���D
	*** @return true: Place�J�n�����D false: ���̏ꏊ�ւ͎肪�͂��Ȃ��C�܂��͕��̂������ĂȂ��D
	*/
	virtual bool Place(Vec3d pos)= 0;

	/** @brief Place���\���ǂ�����Ԃ�
	*/
	virtual bool IsPlaceable(Vec3d pos)= 0;

	/** @brief Place���\���ǂ�����Ԃ�
	*** @param safety ���S�W���F1�ȉ��̌W���C������safety�{���Čv�Z�D1.0�̂Ƃ����肬�蓞�B�\
	*/
	virtual bool IsPlaceable(Vec3d pos, float safety)= 0;

	/** @brief Place�������������ǂ�����Ԃ�
	*/
	virtual bool IsPlaceComplete()= 0;

	/** @brief ���݂̓���𒆒f����
	*/
	virtual void Abort()= 0;

	/** @brief ���ׂĂ̔c������𒆒f����
	*/
	virtual void AbortAll()= 0;

	/** @brief ���݂̓����Ԃ�Ԃ�
	*/
	enum CRGCControlState {
		CRGC_STANDBY=0,
		CRGC_REACH,  CRGC_REACH_COMPLETE,
		CRGC_UPHOLD, CRGC_UPHOLD_COMPLETE,
		CRGC_PLACE,  CRGC_PLACE_COMPLETE,
	};
	virtual CRGrabControllerIf::CRGCControlState GetControlState()= 0;
};

/// �c���R���g���[���̃f�X�N���v�^
struct CRGrabControllerDesc : public CRControllerDesc {
	DESC_DEF_FOR_OBJECT(CRGrabController);

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
