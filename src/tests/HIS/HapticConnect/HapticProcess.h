#ifndef HAPTIC_PROCESS_H
#define HAPTIC_PROCESS_H

#include "Synchronization.h"
#include <Springhead.h>
#include <Physics/PHSolid.h>
#include <Physics/PHContactPoint.h>
#include<Collision/CDDetectorImp.h>
#include <HIS/HISpidarG6X.h>
#include <HIS/HISpidarG6X3.h>
#ifdef _WIN32		//	Win32��(���ʂ͂�����)
 #include <Device/DRUsb20Simple.h>
 #include <Device/DRUsb20Sh4.h>
 #include <conio.h>
#endif

using namespace Spr;
using namespace std;

class HapticProcess{
public:
	// SPIDAR,Device�Ɏg���ϐ�
	DVDeviceManager devMan;
	HISpidarG6X3 spidarG6;
	// LocalDynamic�Ɏg���ϐ�
	double dt;
	PHSolid hpointer; 
	vector<PHNeighborObject> neighborObjects;
	//�@�͒񎦂Ɏg���ϐ�
	bool bDisplayforce;	///< �͊o��񎦂��邩�ǂ���
	bool bInter;		///< �񎦖ʂ̖@�����Ԃ��邩�ǂ���
	Vec3d displayforce;		///< ���[�U�ւ̒񎦗�
	double K;				///< �o�l�W��
	double D;				///< �_���p�W��
	float posScale;
	// �����Ɏg���ϐ�
	int stepcount;
	int countmax;

	HapticProcess();

	void Init();
	void InitDevice();
	void Step();
	void UpdateSpidar();
//	void FindNearestPoint();			// �����̍X�V���g����50Hz�ɂ��Ă݂āC���߂�������g��
	void HapticRendering();
	void LocalDynamics();
	void Keyboard(unsigned char key);
};
extern HapticProcess hprocess;


#endif