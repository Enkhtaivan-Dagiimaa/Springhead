#ifndef HAPTIC_PROCESS_H
#define HAPTIC_PROCESS_H

#include "Synchronization.h"
#include <Springhead.h>
#include <Physics/PHSolid.h>
#include <Physics/PHContactPoint.h>
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
	HISpidarG6X3 spidarG6;
	DVDeviceManager devMan;
	// LocalDynamic�Ɏg���ϐ�
	double dt;
	PHSolid hpointer; 
	vector<PHNeighborObject> neighborObjects;
	//�@�͒񎦂Ɏg���ϐ�
	bool bDisplayforce;	///< �͊o��񎦂��邩�ǂ���
	Vec3d addforce;		///< ���[�U�ւ̒񎦗�
	double K;				///< �o�l�W��
	double D;				///< �_���p�W��
	
	HapticProcess(){
		dt = 0.001f;
		K = 30;
		D = 0;
		bDisplayforce = false;
		hpointer.SetDynamical(false);
		hpointer.SetFrozen(true);
	};

	void Init();
	void InitDevice();
	void Step();
	void UpdateSpidar();
	void HapticRendering();
	void CalcForce(Vec3d dis);
	void LocalDynamics();
	void ClearForce();
	void GenerateForce();
	void Integrate();	
	double CalcDistance(Vec3d a2, Vec3d b);
	void Keyboard(unsigned char key);

};



#endif