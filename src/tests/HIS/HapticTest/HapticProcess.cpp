#include "HapticProcess.h"
#include "PhysicsProcess.h"
#include <iostream>
#include <sstream>
#include <cmath>
#include <ctime>
#include <string>
#include <GL/glut.h>
#include <vector>
#include <windows.h>
#include <fstream>

HapticProcess hprocess;	
#define TORQUE

HapticProcess::HapticProcess(){
	dt = 0.001f;
	K = 10;
	D = 0.0;//0.005;		// �_���p���C0�ɂ����珰�̂����������Ȃ��Ȃ����D
	posScale = 300;
	bDisplayforce = false;
	bInter = true;
	hpointer.SetDynamical(false);
	hpointer.SetFrozen(true);
	stepcount = 1;
	countmax = 100;
};

void HapticProcess::Init(){
	InitDevice();
}

void HapticProcess::InitDevice(){
	// �f�o�C�X�}�l�[�W���̏�����
	devMan.RPool().Register(new DRUsb20Sh4(0));	    // USB2.0�ŃR���g���[�� 8���[�^
	devMan.RPool().Register(new DRUsb20Sh4(1));	    // USB2.0�ŃR���g���[�� 8���[�^
	devMan.Init();														// �f�o�C�X�̏�����
	DSTR << devMan;												// �������̌��ʂ�\��

	// SPIDAR�̏�����
	spidarG6.Init(devMan, false);
	for(int i = 0; i < 8; ++i){
		spidarG6.motor[i].maxForce = 5.0f;
	}
	spidarG6.Calib();
}

void HapticProcess::Step(){
	UpdateSpidar();
	HapticRendering();
	LocalDynamics();
}

void HapticProcess::UpdateSpidar(){
	spidarG6.Update(dt);
	hpointer.SetFramePosition(spidarG6.GetPos() * posScale);
	hpointer.SetOrientation(spidarG6.GetOri());
	hpointer.SetVelocity(spidarG6.GetVel() * posScale);
	hpointer.SetAngularVelocity(spidarG6.GetAngVel());
}

void HapticProcess::HapticRendering(){
	displayforce = Vec3d(0.0, 0.0, 0.0);		
	displaytorque = Vec3d(0.0, 0.0, 0.0);

	for(unsigned i = 0; i < expandedObjects.size(); i++){
		if(!expandedObjects[i].flag.blocal) continue;
		Vec3d cPoint = expandedObjects[i].phSolid.GetPose() * expandedObjects[i].syncInfo.neighborPoint.closestPoint;			// ���̂̋ߖT�_�̃��[���h���W�n
		Vec3d pPoint = hpointer.GetPose() * expandedObjects[i].syncInfo.neighborPoint.pointerPoint;									// �͊o�|�C���^�̋ߖT�_�̃��[���h���W�n
		Vec3d force_dir = pPoint - cPoint;
		Vec3d interpolation_normal;																								// �񎦗͌v�Z�ɂ��悤����@���i�O��̖@���Ƃ̊Ԃ��Ԃ���j

		// ���̖̂ʂ̖@�����
		// �O��̖@���ƌ��݂̖@���̊Ԃ��Ԃ��Ȃ���X�V
		double synccount = pprocess.dt / hprocess.dt;		// �v���Z�X�̍��ݎ��Ԃ̔�
		interpolation_normal = (stepcount * expandedObjects[i].syncInfo.neighborPoint.face_normal + 
			((double)synccount - stepcount) * expandedObjects[i].syncInfo.neighborPoint.last_face_normal) / (double)synccount;															
		if(stepcount > synccount)		interpolation_normal = expandedObjects[i].syncInfo.neighborPoint.face_normal;

		float	f = force_dir * interpolation_normal;								// ���̖̂ʂ̖@���Ɠ��ς��Ƃ�
		if(f < 0.0){																			// ���ς����Ȃ�͂��v�Z
			Vec3d ortho = f * interpolation_normal;								// �ߖT�_����͊o�|�C���^�ւ̃x�N�g���̖ʂ̖@���ւ̐��ˉe
			Vec3d dv = expandedObjects[i].phSolid.GetPointVelocity(cPoint) - hpointer.GetPointVelocity(pPoint);
			Vec3d dvortho = dv.norm() * interpolation_normal;
			static Vec3d addforce = Vec3d(0,0,0);
			
			addforce = -K * ortho + D * dvortho;// * ortho.norm();						// �񎦗͌v�Z (*�_���p�̍���ortho�̃m�����������Ă݂�)
			//Vec3d addtorque = (pPoint - hpointer.GetCenterPosition()) % addforce ;

			displayforce += addforce;			// ���[�U�ւ̒񎦗�		
//			displaytorque += addtorque;										 
			expandedObjects[i].phSolid.AddForce(-addforce, cPoint);			// �v�Z�����͂����̂ɉ�����
			expandedObjects[i].syncInfo.neighborPoint.test_force_norm = addforce.norm();
		}
	}
#ifdef TORQUE
	if(bDisplayforce) spidarG6.SetForce(displayforce);//, displaytorque);		// �͊o��
#else
	if(bDisplayforce) spidarG6.SetForce(displayforce);								// �͊o��
#endif
}

void HapticProcess::LocalDynamics(){
	for(unsigned i = 0; i < expandedObjects.size(); i++){
		if(!expandedObjects[i].flag.blocal) continue;
		SpatialVector vel;																				// ���̂̑��x�i���[���h���W�n�j
		vel.v() = expandedObjects[i].phSolid.GetVelocity();
		vel.w() = expandedObjects[i].phSolid.GetAngularVelocity();
		if(stepcount == 1) vel += (expandedObjects[i].syncInfo.motionCoeff.curb - expandedObjects[i].syncInfo.motionCoeff.lastb) *  pprocess.dt;		// �Փ˂̉e���𔽉f
		vel += (expandedObjects[i].syncInfo.motionCoeff.A * expandedObjects[i].phSolid.nextForce + expandedObjects[i].syncInfo.motionCoeff.b) * dt;	// �͊o�|�C���^����̗͂ɂ�鑬�x�ω�
		expandedObjects[i].phSolid.SetVelocity(vel.v());																		
		expandedObjects[i].phSolid.SetAngularVelocity(vel.w());
		expandedObjects[i].phSolid.SetCenterPosition(expandedObjects[i].phSolid.GetCenterPosition() + vel.v() * dt);
		expandedObjects[i].phSolid.SetOrientation(( Quaterniond::Rot(vel.w() * dt) * expandedObjects[i].phSolid.GetOrientation()).unit());
 		expandedObjects[i].phSolid.SetUpdated(true);
		expandedObjects[i].phSolid.Step();
	}
}

void HapticProcess::Keyboard(unsigned char key){
	switch(key){
		case 'f':
			if(bDisplayforce){
				bDisplayforce = false;
				spidarG6.SetForce(Vec3d(0, 0, 0));
				DSTR << "Force OFF" << endl;
			}else{
				bDisplayforce = true;
				spidarG6.SetForce(Vec3d(0, 0, 0));
				DSTR << "Force ON" << endl;
			}
			break;
		case 'c':
			spidarG6.SetForce(Vec3d(0, 0, 0));
			spidarG6.Calib();
			DSTR << "Calibration" << endl;
			break;
		default:
			break;
	}
}