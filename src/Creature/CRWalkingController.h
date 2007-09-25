/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRWALKINGCONTROLLER_H
#define CRWALKINGCONTROLLER_H

#include <Springhead.h>

#include <Foundation/Object.h>
#include "IfStubCreature.h"

#include "CRController.h"
#include ".\crwctimeleft.h"
#include ".\crwcchangearoundcenter.h"
#include ".\crwclandingsite.h"
#include ".\crwcfootforce.h"
#include ".\crwcgeneforce.h"

//@{
namespace Spr{;
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief ���ӃR���g���[��
*/
class CRWalkingController : public CRController, public CRWalkingControllerIfInit, public CRWalkingControllerDesc {
private:
	/// �ˑ����鑼�̃I�u�W�F�N�g�̗�D�K�v�Ȃ��̂�I��ŋL�q�D������CRController�Ɋ��ɂ�����͕̂s�v�D
	
	CRTrunkFootHumanBodyIf* body;

	std::vector<PHSolidIf*> UpperBody;
	PHSolidIf* soHead;
	PHSolidIf* soCenterObject;
	PHSolidIf* soHip;
	PHSolidIf* footleft;
	PHSolidIf* footright;

    double paramVelocityX, paramVelocityZ;              //X, Z�����̖ڕW���s���x 
    double paramHalfStrideX, paramHalfStrideZ;          //X, Z�����̖ڕW���� 
	double footHeight;                                  //�V�r�̍ō��_ 
    double amplitude;                                   //�d�S�̏㉺���̐U�� 


	/////////////���ϐ�//////
	double T0;            //��{���s����
	double Tc;            //��{���s���������߂�̂Ɏg�p����萔
	double TimeStep;
	double et;      //elapsed time �P�r�x�����Ԃ̌o�ߎ���
	double timeleft;      //�P�r�x�����Ԃ̎c�莞��
	double timeDSleft;    //���r�x�����Ԃ̎c�莞��
	double timehalfcycle; //���s�̗��r�x�����Ԃ����˂��P�T�C�N���̂Q���̂P
	double footsize;      //���̃T�C�Y
	double SingleSupportEndX;   //�P�r�x�����Ԃ̏I�����̏d�S�ʒuX
	double SingleSupportEndZ;   //�P�r�x�����Ԃ̏I�����̏d�S�ʒuZ
	double previousPX;          //1�X�e�b�v�O�̏d�S�ʒuX
	double previousPZ;          //1�X�e�b�v�O�̏d�S�ʒuZ
	double DSDX;                //���r�x�����Ԃɏd�S�̐i�ދ���
	double CurrentDirection;    //x,z���ʏ�̌��݂̑̂̕���
	double TargetDirection;   //x,z���ʏ�̌��݂̖̑̂ڕW����
	bool LF;       //���r���V�r�̎� true
	bool RF;      //�E�r���V�r�̎� true
	bool DSterm;  //���r�x�����Ԃ̎� true
	bool EarthConnection; //���݂̎x���r�����n�����𖞂����Ă��鎞 true
	bool LandAble;        //���̒��n�_�֒��n���邱�Ƃ��\�ȏꍇ true
	bool completefall; //���s�̌p�����s�\�ȏꍇ true
	Vec3d currentlandingsite;   //���݂̒��n�_
	Vec3d nextlandingsite;      //���̖ڕW���n�_
	Vec3d CenterVelocity;       //�d�S�ʒu�̑��x
	Vec3d PreviousV;            //1�X�e�b�v�O�̏d�S�ʒu�̑��x
	Vec3d PositionOfWholeBody;  //�d�S�ʒu
	Vec3d PositionOfUpperBody;  //�㔼�g�̈ʒu
	Vec3d change;   //�o�����X�Ɋւ��鐧��� PD���� 
	long totalStep;   //�J�n����̃X�e�b�v��

	int katoki;       //�X�e�b�v��gaitbegin�Ɏ���܂ŋr�𓮂����Ȃ�����̂��߂Ɏg�p.������Ȃ���
	int gaitbegin;   //�X�e�b�v��gaitbegin�Ɏ���܂ŋr�𓮂����Ȃ�����̂��߂Ɏg�p.������Ȃ���
	int forcestep1;   //�f���Ɏg�p���Ă���ϐ�
	int flag;         //�f���p�Ɏg�p
	int forcestep2;   //�f���Ɏg�p���Ă���ϐ�
	int flag2;        //�f���p�Ɏg�p
	int flag3;        //�f���p�Ɏg�p

	CRWCTimeLeft* tl;
	CRWCChangeAroundCenter* cac;
	CRWCLandingSite* ls;
	CRWCFootForce* ff;
	CRWCGeneForce* gf;
	/*
	CRInternalSceneIf*  internalScene;
	CRGazeControllerIf* gazeController;
	CROpticalSensorIf*  opticalSensor;
	*/

	/// �ق���Private�ȕϐ��⃁�\�b�h���K�v�Ȃ炱���ɒ�`��ǉ��D
	void CreateUpperBody();
	void AssignFoot();  
	void AssignCenterObject(); 
	void AssignHip();        
	void AssignHead();  
	void InvalidGravity();
	void SetTimeParams();
	void AssignInitialLandingSite();
	void CreateCRWCTimeLeft();
	void CreateCRWCChangeAroundCenter();
	void CreateCRWCLandingSite();
	void CreateCRWCFootForce();
	void CreateCRWCGeneForce();
	Vec3d GetCenterOfBlocks(std::vector<PHSolidIf*> objs);
	Vec3d CalcCenterVelocity(std::vector<PHSolidIf*> objs);
	double TotalMass(std::vector<PHSolidIf*> objs);
	double CalcBasicCycle();
	double CalcZParamV(double hz, double restT);
	double CalcZParamH(double v, double restT);
	double CalcLocalX(double xb, double zb, double xt, double zt, double theta);
	double CalcLocalZ(double xb, double zb, double xt, double zt, double theta);
	double CalcLocalVX(double vx, double vz, double theta);
	double CalcLocalVZ(double vx, double vz, double theta);
	void CalcCurrentDirection();
	void CalcTargetAngle();
	void VelocityXChange(double vx);
	void VelocityZChange(double vz);
	void HalfStrideXChange(double sx);
	void AmplitudeChange(double amp);
	bool CalcSustainable();
	void ConstraintForce();
	void CompleteFall();
	Vec3d CalcChange();
	double CalcTimeLeft();
	Vec3d CalcNextLandingSite();
	double CalcTimeHalfCycle();
	void GenerateCenterForce();
	void GenerateFootForce();
	void gait();
	void test();

public:
	OBJECTDEF(CRWalkingController, CRController);
	ACCESS_DESC(CRWalkingController);

	CRWalkingController(){
	}
	CRWalkingController(const CRWalkingControllerDesc& desc, CRCreatureIf* c=NULL) 
		: CRWalkingControllerDesc(desc) 
		, CRController((const CRControllerDesc&)desc, c)
	{
	}

	/** @brief ���������s��
	*/
	virtual void Init();

	/** @brief ��������s����
	*/
	virtual void Step();

	/** @brief ���s�̑��x��ݒ肷��
	*/
	virtual void SetSpeed(float speed);

	/** @brief �]��p�x��ݒ肷��
	*/
	virtual void SetRotationAngle(float rot);

	virtual void SetVelocityX(double vx) {VelocityXChange(vx);}
	virtual void SetVelocityZ(double vz) {VelocityZChange(vz);}
	virtual void SetHalfStrideX(double hsx) {HalfStrideXChange(hsx);}
	virtual void SetAmplitude(double amp) {AmplitudeChange(amp);}
};
}
//@}

#endif//CRWALKINGCONTROLLER_H
