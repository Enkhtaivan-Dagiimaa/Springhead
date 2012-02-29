/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include <HumanInterface/HISpidar.h>
#ifdef USE_HDRSTOP
 #pragma hdrstop
#endif

/*
//	���[�^�̎��t���ʒu
//	���[�^�������̂Ɏ��t�����Ă���ꍇ
#define PX	0.265f	//	x�����̕ӂ̒���/2
#define PY	0.265f	//	y�����̕ӂ̒���/2
#define PZ	0.265f	//	z�����̕ӂ̒���/2
Vec3f HISpidar4::motorPosDef[][4] =	//	���[�^�̎��t���ʒu(���S�����_�Ƃ���)
	{
		{Vec3f(-PX, PY, PZ),	Vec3f( PX, PY,-PZ), Vec3f(-PX,-PY,-PZ), Vec3f( PX,-PY, PZ)},
		{Vec3f( PX, PY, PZ),	Vec3f(-PX, PY,-PZ), Vec3f( PX,-PY,-PZ), Vec3f(-PX,-PY, PZ)}
	};
*/

namespace Spr {;

bool HISpidar::Init(const void* pDesc){
	HISdkIf* sdk = GetSdk();
	const HISpidarDesc& desc = *(const HISpidarDesc*)pDesc;
	int nMotor = desc.motors.size();
	motors.resize(nMotor);
	
	sdk->Print(DSTR);
	size_t i;
	for(i = 0; i < motors.size(); ++i){
		motors[i].SetDesc(&desc.motors[i]);
		motors[i].da	  = DCAST( DVDaIf,		sdk->RentVirtualDevice(DVDaIf::GetIfInfoStatic(),	   "", motors[i].ch) );
		motors[i].counter = DCAST( DVCounterIf, sdk->RentVirtualDevice(DVCounterIf::GetIfInfoStatic(), "", motors[i].ch) );
		if (!motors[i].da || !motors[i].counter)
			break;
		AddDeviceDependency(motors[i].da->GetRealDevice()->Cast());
		AddDeviceDependency(motors[i].counter->GetRealDevice()->Cast());
	}
	if(i < motors.size())
		return false;
	return true;
}
	
void HISpidar::SetMinForce(){
	for(int i = 0; i < (int)motors.size(); i++)
		motors[i].SetForce(motors[i].minForce);
	Update(0.001f);
}

void HISpidar::SetLimitMinForce(float f){
	for(int i = 0; i < (int)motors.size(); i++)
		motors[i].SetLimitMinForce(f);
}

void HISpidar::SetLimitMaxForce(float f){
	for(int i = 0; i < (int)motors.size(); i++)
		motors[i].SetLimitMaxForce(f);
}
//----------------------------------------------------------------------------
// HISpidar4Desc

HISpidar4Desc::HISpidar4Desc(){

}

void HISpidar4Desc::Init(int nMotor, Vec3f* motorPos, Vec3f* knotPos, float vpn, float lpp, float minF, float maxF){
	motors.resize(nMotor);
	for(int i=0; i<nMotor; ++i){
		motors[i].pos = motorPos[i];
		motors[i].jointPos = knotPos[i];
		motors[i].lengthPerPulse = lpp;
		motors[i].voltPerNewton = vpn;		//	volt per newton
		motors[i].currentPerVolt = 1;		//	��R�l�̋t��
		motors[i].minForce = minF;
		motors[i].maxForce = maxF;
	}
	VVector<float> minForce, maxForce;
	minForce.resize(nMotor);
	maxForce.resize(nMotor);
	for(int i=0; i<nMotor; ++i){
		minForce[i] = minF;
		maxForce[i] = maxF;
	}
}

void HISpidar4Desc::InitSpidarG(char* type){
	if (stricmp(type, "SpidarG6X3L")==0 || stricmp(type, "SpidarG6X3R")==0){
		bool bLeft = stricmp(type, "SpidarG6X3L")==0;
		//	���[�^�̎��t���ʒu. ���[�^�������̂Ɏ��t�����Ă���ꍇ�́C
		const float PX = 0.12f/2;		//	x�����̕ӂ̒���/2
		const float PY = 0.14f/2;		//	y�����̕ӂ̒���/2
		const float PZ = 0.12f/2;		//	z�����̕ӂ̒���/2
		//	���̃O���b�v�ւ̎��t���ʒu�D�O���b�v�̓s���|���ʂ��̗p���Ă��܂��D
		const float GX = 0.02f/2;		//	x�����̕ӂ̒���/2
		const float GY = 0.02f/2;		//	y�����̕ӂ̒���/2
		Matrix3f rotR = Matrix3f::Rot((float)Rad(-90), 'y');
		Matrix3f rotL = Matrix3f::Rot((float)Rad(0), 'y');
		Matrix3f rotZ = Matrix3f::Rot((float)Rad(-45), 'z');
		Matrix3f tMat = Matrix3f(); tMat.xx*=-1; tMat.yy*=-1; //����n���E��n�ɕϊ�

		Vec3f motorPos[2][4][2] = {		//	���[�^�̎��t���ʒu(���S�����_�Ƃ���DirectX���W�n�i�E��X,�オY,����Z�j)
			{
				{rotR*Vec3f(-PX,-PY, PZ), rotR*rotZ*Vec3f(0.0f, -GY, 0.0f)},//1����
				{rotR*Vec3f( PX,-PY,-PZ), rotR*rotZ*Vec3f( GX, 0.0f, 0.0f)},//3���E
				{rotR*Vec3f( PX, PY, PZ), rotR*rotZ*Vec3f(0.0f,  GY, 0.0f)},//6�㉜
				{rotR*Vec3f(-PX, PY,-PZ), rotR*rotZ*Vec3f(-GX, 0.0f, 0.0f)}//8��O
			},
			{
				{tMat*rotL*Vec3f( PX, PY, PZ), tMat*rotL*rotZ*Vec3f( GX, 0.0f, 0.0f)},//2����
				{tMat*rotL*Vec3f(-PX, PY,-PZ), tMat*rotL*rotZ*Vec3f(0.0f, -GY, 0.0f)},//4���O
				{tMat*rotL*Vec3f( PX,-PY,-PZ), tMat*rotL*rotZ*Vec3f(-GX, 0.0f, 0.0f)},//5�㍶
				{tMat*rotL*Vec3f(-PX,-PY, PZ), tMat*rotL*rotZ*Vec3f(0.0f,  GY, 0.0f)},//7��E

			}
		};

		Vec3f mp[4];
		Vec3f kp[4];
		if (bLeft){
			for(int i=0; i<4; ++i){
				mp[i] = motorPos[1][i][0];
				kp[i] = motorPos[1][i][1];
			}
		}else{
			for(int i=0; i<4; ++i){
				mp[i] = motorPos[0][i][0];
				kp[i] = motorPos[0][i][1];
			}
		}
		Init(4, mp, kp, 0.365296803653f, 1.66555e-5f, 0.3f, 20.0f);
		if (bLeft){
			motors[1].lengthPerPulse *= -1;
			motors[2].lengthPerPulse *= -1;

		}else{
			motors[1].lengthPerPulse *= -1;
			motors[2].lengthPerPulse *= -1;
		}
	}
}

void HISpidar4Desc::InitSpidarBig(char* type){
	if (stricmp(type, "SpidarL")==0 || stricmp(type, "SpidarR")==0){
		bool bLeft = stricmp(type, "SpidarL")==0;
		//	���[�^�̎��t���ʒu. ���[�^�������̂Ɏ��t�����Ă���ꍇ�́C
		const float PX = 0.625f/2;		//	x�����̕ӂ̒���/2
		const float PY = 0.54f/2;		//	y�����̕ӂ̒���/2
		const float PZ = 0.55f/2;		//	z�����̕ӂ̒���/2
		//	���̃O���b�v�ւ̎��t���ʒu�D�O���b�v�̓s���|���ʂ��̗p���Ă��܂��D
		const float GX = 0.02f/2;		//	x�����̕ӂ̒���/2
		const float GY = 0.02f/2;		//	y�����̕ӂ̒���/2

		Matrix3f rotZ = Matrix3f::Rot((float)Rad(45), 'z');
		Matrix3f tMatR = Matrix3f(); tMatR.zz*=-1; //���W����
		Matrix3f tMatL = Matrix3f(); tMatL.xx*=-1; //���W����

		Vec3f motorPos[2][4][2] = {		//	���[�^�̎��t���ʒu(���S�����_�Ƃ���DirectX���W�n�i�E��X,�オY,����Z�j)
			{
				{tMatR*Vec3f( PX,  PY,-PZ), rotZ*tMatR*Vec3f(0.0f,  GY, 0.0f)},//0��E�O
				{tMatR*Vec3f(-PX,  PY, PZ), rotZ*tMatR*Vec3f(-GX,  0.0f, 0.0f)},//1�㍶��
				{tMatR*Vec3f( PX, -PY, PZ), rotZ*tMatR*Vec3f( GX, 0.0f, 0.0f)},//2���E��
				{tMatR*Vec3f(-PX, -PY,-PZ), rotZ*tMatR*Vec3f(0.0f, -GY, 0.0f)},//3�����O
			},
			{
				{tMatL*Vec3f( PX, PY, PZ),  rotZ*tMatL*Vec3f( -GX, 0.0f, 0.0f)},//4�㍶�O
				{tMatL*Vec3f(-PX, PY,-PZ),  rotZ*tMatL*Vec3f( 0.0f, GY, 0.0f)},//5��E��
				{tMatL*Vec3f( PX,-PY,-PZ),  rotZ*tMatL*Vec3f( 0.0f, -GY, 0.0f)},//6���E�O
				{tMatL*Vec3f(-PX,-PY, PZ),  rotZ*tMatL*Vec3f( GX, 0.0f, 0.0f)},//7������
			}
		};


		Vec3f mp[4];
		Vec3f kp[4];
		if (bLeft){
			for(int i=0; i<4; ++i){
				mp[i] = motorPos[1][i][0];
				kp[i] = motorPos[1][i][1];
			}
		}else{
			for(int i=0; i<4; ++i){
				mp[i] = motorPos[0][i][0];
				kp[i] = motorPos[0][i][1];
			}
		}
		Init(4, mp, kp, 0.365296803653f, 1.66555e-5f*1.667f, 0.25f, 10.0f);
	}
	
}

void HISpidar4Desc::Init(char* type){
	InitSpidarG(type);
	InitSpidarBig(type);
}

//----------------------------------------------------------------------------
// HISpidar4D

HISpidar4::HISpidar4(const HISpidar4Desc& desc){}
HISpidar4::~HISpidar4(){}

bool HISpidar4::Init(const void* pDesc){
	if(!HISpidar::Init(pDesc))
		return false;

	const HISpidar4Desc& desc = *(const HISpidar4Desc*)pDesc;
	
	// HISpidarCalc3Dof�̏�����
	nRepeat = 2;		
	sigma	= 0.001f;		//sigma=sigma*sigma
	VVector<float> minForce, maxForce;
	minForce.resize(desc.motors.size());
	maxForce.resize(desc.motors.size());
	for(unsigned i = 0; i < desc.motors.size(); ++i){
		minForce[i] = desc.motors[i].minForce;
		maxForce[i] = desc.motors[i].maxForce;
	}
	HISpidarCalc3Dof::Init(3, minForce, maxForce);
	
	SetMinForce();
	BeforeCalibration();
	Calibration();
	AfterCalibration();
	bGood = true;
	return true;
}

bool HISpidar4::Calibration(){
	//	�|�C���^�����_(���S)�ɒu���āA�L�����u���[�V�������s��
	for(unsigned i=0; i<motors.size(); i++)
		motors[i].SetLength( (motors[i].pos - motors[i].jointPos).norm() );

	lengthDiffAve.clear();

	// �p�����X�V
	for(size_t i=0; i<motors.size(); ++i)
		HISpidarCalc3Dof::Update();	

	return true;
}

void HISpidar4::Update(float dt){
	HIHaptic::Update(dt);
	HISpidarCalc3Dof::Update();
	for(unsigned int i = 0; i < motors.size(); ++i){
		motors[i].SetForce(Tension()[i]);
	}
}

void HISpidar4::MakeWireVec(){
	for(unsigned int i=0; i<motors.size(); ++i){
		wireDirection[i] = motors[i].pos - pos;
		calculatedLength[i] = wireDirection[i].norm();
		wireDirection[i] /= calculatedLength[i];
	}
}

void HISpidar4::UpdatePos(){
}

void HISpidar4::MeasureWire(){
	for(unsigned int i=0; i<motors.size(); ++i){
		measuredLength[i] = motors[i].GetLength();
	}	
}

//----------------------------------------------------------------------------
// HISpidar4D

HISpidar4D::HISpidar4D(const HISpidar4DDesc& desc){
    for (int i=0;i<4;i++){
        tension[i] = 0;
    }
}

bool HISpidar4D::Calibration(){
	//	�|�C���^�����_(���S)�ɒu���āA�L�����u���[�V�������s��
	// calibration sets the center of the reference frame at the current
	// position of the spidar (motor[i].pos.norm())
	Update(0.001f);
	for(int i=0; i<4; i++)
		motors[i].SetLength(motors[i].pos.norm());
	return true;
}

void HISpidar4D::InitMat(){
	matPos = Matrix3f(
		motors[1].pos - motors[0].pos,
		motors[2].pos - motors[1].pos,
		motors[3].pos - motors[2].pos).trans() * 2;
	matPos = matPos.inv();

	posSqrConst = Vec3f(
		motors[1].pos.square() - motors[0].pos.square(),
		motors[2].pos.square() - motors[1].pos.square(),
		motors[3].pos.square() - motors[2].pos.square());
}

Vec3f HISpidar4D::GetForce(){
	Vec3f f;
	for (size_t i = 0; i < motors.size(); i++)
		f += tension[i] * phi[i];
	return f;
}

void HISpidar4D::SetForce(const Vec3f& v3force){
	/*	2���v��@�ɂ�钣�͌v�Z	*/
	//	���̕����x�N�g�������߂�B
	for (int i = 0; i < 4; i++){
		phi[i] = (motors[i].pos - pos).unit();		//	���͂̒P�ʃx�N�g��		tension direction
	}

	/*	�ړI�֐�
			|f-��t|^2 + cont*|t-t'|^2 + eff*t^2
		= (��^2+eff+cont)*t^2 + (-2*��*f -2*cont*t')*t + f^2+cont*t'^2
		���ŏ��ɂ��钣��t�����߂�D	*/
	TQuadProgram<float, 4> qp;
	//	�ړI�֐��̂Q���W���s��
	for(int i = 0; i < 4; i++)
		qp.matQ[i][i] = phi[i]*phi[i] + eff + cont;
	qp.matQ[0][1] = qp.matQ[1][0] = phi[0]*phi[1];
	qp.matQ[0][2] = qp.matQ[2][0] = phi[0]*phi[2];
	qp.matQ[0][3] = qp.matQ[3][0] = phi[0]*phi[3];
	qp.matQ[1][2] = qp.matQ[2][1] = phi[1]*phi[2];
	qp.matQ[1][3] = qp.matQ[3][1] = phi[1]*phi[3];
	qp.matQ[2][3] = qp.matQ[3][2] = phi[2]*phi[3];

	//	�ړI�֐��̂P���W���x�N�g��
	for(int i = 0; i < 4; i++)
		qp.vecC[i] = phi[i]*v3force + cont*tension[i];
	//	�ŏ����́E�ő咣�͂̐ݒ�
	Vec4f minF, maxF;
	minF.clear(1); maxF.clear(20);
	qp.Init(minF, maxF);
	qp.Solve();
	for(int i=0;i<4;i++) {
		tension[i]=qp.vecX[i];							//���́�x[]
	}
}

void HISpidar4D::Update(float dt){
	HIHaptic::Update(dt);

	float len[4];
	for(int i = 0; i < 4; i++)
		len[i] = motors[i].GetLength();

	pos = matPos * (
		Vec3f(	Square(len[0]) - Square(len[1]),
				Square(len[1]) - Square(len[2]),
				Square(len[2]) - Square(len[3])	) + posSqrConst);

	for (int i=0;i<4;i++){
		motors[i].SetForce(tension[i]);
	}
}

//----------------------------------------------------------------------------
// HISpidarG

HISpidarGDesc::HISpidarGDesc(){
}

void HISpidarGDesc::Init(int nMotor, Vec3f* motorPos, Vec3f* knotPos, float vpn, float lpp, float minF, float maxF){
	motors.resize(nMotor);
	for(int i=0; i<nMotor; ++i){
		motors[i].pos = motorPos[i];
		motors[i].jointPos = knotPos[i];
		motors[i].lengthPerPulse = lpp;
		motors[i].voltPerNewton = vpn;		//	volt per newton
		motors[i].currentPerVolt = 1;		//	��R�l�̋t��
		motors[i].minForce = minF;
		motors[i].maxForce = maxF;
	}
	VVector<float> minForce, maxForce;
	minForce.resize(nMotor);
	maxForce.resize(nMotor);
	for(int i=0; i<nMotor; ++i){
		minForce[i] = minF;
		maxForce[i] = maxF;
	}
}

void HISpidarGDesc::Init(char* type){
	if (stricmp(type, "SpidarG6X3L")==0 || stricmp(type, "SpidarG6X3R")==0){
		bool bLeft = stricmp(type, "SpidarG6X3L")==0;
		//	���[�^�̎��t���ʒu. ���[�^�������̂Ɏ��t�����Ă���ꍇ�́C
		const float PX = 0.12f/2;		//	x�����̕ӂ̒���/2
		const float PY = 0.152f/2;//0.14f/2;		//	y�����̕ӂ̒���/2
		const float PZ = 0.12f/2;		//	z�����̕ӂ̒���/2
		//	���̃O���b�v�ւ̎��t���ʒu�D�O���b�v�̓s���|���ʂ��̗p���Ă��܂��D
		const float GX = 0.045f/2;		//	x�����̕ӂ̒���/2
		const float GY = 0.045f/2;		//	y�����̕ӂ̒���/2
		Matrix3f rotR = Matrix3f::Rot((float)Rad(-45), 'y');
		Matrix3f rotL = Matrix3f::Rot((float)Rad(-45), 'y');

		Vec3f motorPos[2][8][2] = {		//	���[�^�̎��t���ʒu(���S�����_�Ƃ���DirectX���W�n�i�E��X,�オY,����Z�j)
			{
				{rotR*Vec3f(-PX,-PY, PZ), rotR*Vec3f( -GX, 0.0f, 0.0f)},
				{rotR*Vec3f( PX,-PY, PZ), rotR*Vec3f(  GX, 0.0f, 0.0f)},
				{rotR*Vec3f( PX,-PY,-PZ), rotR*Vec3f(0.0f,  -GY, 0.0f)},
				{rotR*Vec3f(-PX,-PY,-PZ), rotR*Vec3f(0.0f,  -GY, 0.0f)},
				{rotR*Vec3f(-PX, PY, PZ), rotR*Vec3f( -GX, 0.0f, 0.0f)},
				{rotR*Vec3f( PX, PY, PZ), rotR*Vec3f(  GX, 0.0f, 0.0f)},
				{rotR*Vec3f( PX, PY,-PZ), rotR*Vec3f(0.0f,   GY, 0.0f)},
				{rotR*Vec3f(-PX, PY,-PZ), rotR*Vec3f(0.0f,   GY, 0.0f)}
			},
			{
				{rotL*Vec3f( PX, PY,-PZ), rotL*Vec3f(0, 0.0f,  -GX)},
				{rotL*Vec3f( PX, PY, PZ), rotL*Vec3f(0, 0.0f,   GX)},
				{rotL*Vec3f(-PX, PY, PZ), rotL*Vec3f(0,   GY, 0.0f)},
				{rotL*Vec3f(-PX, PY,-PZ), rotL*Vec3f(0,   GY, 0.0f)},
				{rotL*Vec3f( PX,-PY,-PZ), rotL*Vec3f(0, 0.0f,  -GX)},
				{rotL*Vec3f( PX,-PY, PZ), rotL*Vec3f(0, 0.0f,   GX)},
				{rotL*Vec3f(-PX,-PY, PZ), rotL*Vec3f(0,  -GY, 0.0f)},
				{rotL*Vec3f(-PX,-PY,-PZ), rotL*Vec3f(0,  -GY, 0.0f)},
			}
		};
		Vec3f mp[8];
		Vec3f kp[8];
		if (bLeft){
			for(int i=0; i<8; ++i){
				mp[i] = motorPos[1][i][0];
				kp[i] = motorPos[1][i][1];
			}
		}else{
			for(int i=0; i<8; ++i){
				mp[i] = motorPos[0][i][0];
				kp[i] = motorPos[0][i][1];
			}
		}
		Init(8, mp, kp, 0.365296803653f, 1.66555e-5f, 0.3f, 20.0f);
		if (bLeft){
			motors[2].lengthPerPulse *= -1;
			motors[3].lengthPerPulse *= -1;
			motors[4].lengthPerPulse *= -1;
			motors[5].lengthPerPulse *= -1;

		}else{
			motors[2].lengthPerPulse *= -1;
			motors[3].lengthPerPulse *= -1;
			motors[4].lengthPerPulse *= -1;
			motors[5].lengthPerPulse *= -1;
		}
	}
}

//----------------------------------------------------------------------------
HISpidarG::HISpidarG(const HISpidarGDesc& desc){
	SetWeight();
}

HISpidarG::~HISpidarG(){
}

bool HISpidarG::Init(const void* pDesc){
	if(!HISpidar::Init(pDesc))
		return false;

	HISdkIf* sdk = GetSdk();
	const HISpidarGDesc& desc = *(const HISpidarGDesc*)pDesc;

	// HISpidarCalc6Dof�̏�����
	nRepeat = 2;
	sigma = 0.001f; //sigma=sigma*sigma
	VVector<float> minForce, maxForce;
	minForce.resize(desc.motors.size());
	maxForce.resize(desc.motors.size());
	for(unsigned i=0; i<desc.motors.size(); ++i){
		minForce[i] = desc.motors[i].minForce;
		maxForce[i] = desc.motors[i].maxForce;
	}
	HISpidarCalc6Dof::Init(6, minForce, maxForce);

	// �{�^���f�o�C�X�̏�����
	buttons.resize(8);
	size_t i;
	for(i=0; i<buttons.size(); ++i){
		buttons[i] = sdk->RentVirtualDevice(DVPioIf::GetIfInfoStatic())->Cast();
		if (!buttons[i]) break;
		AddDeviceDependency(buttons[i]->GetRealDevice()->Cast());
	}
	if(i < buttons.size())
		return false;

	SetMinForce();
	BeforeCalibration();
	Calibration();
	AfterCalibration();
	bGood = true;
	return true;
}

bool HISpidarG::Calibration(){
	if (!bGood) return false;
	//	�|�C���^�����_(���S)�ɒu���āA�L�����u���[�V�������s��
	for(unsigned i = 0; i < motors.size(); i++)
		motors[i].SetLength( (motors[i].pos - motors[i].jointPos).norm() );
	lengthDiffAve.clear();
	pos = Vec3f();
	ori = Quaterniond();
	HISpidarCalc6Dof::Update();	//	�p�����X�V
	return true;
}

void HISpidarG::Update(float dt){
	if (!bGood) return;
	HIHaptic::Update(dt);
	HISpidarCalc6Dof::Update();
	for(unsigned int i=0; i<motors.size(); ++i){
		motors[i].SetForce(Tension()[i]);
	}
}

void HISpidarG::MakeWireVec(){
	if (!bGood) return;
	for(unsigned int i=0; i<motors.size(); ++i){
		wireDirection[i] = motors[i].pos - (ori*motors[i].jointPos + pos);
		calculatedLength[i] = wireDirection[i].norm();
		wireDirection[i] /= calculatedLength[i];
		wireMoment[i] = (ori * motors[i].jointPos) ^ wireDirection[i];
	}
}

void HISpidarG::UpdatePos(){
}

void HISpidarG::MeasureWire(){
	if (!bGood) return;
	for(unsigned int i=0; i<motors.size(); ++i){
		measuredLength[i] = motors[i].GetLength();
	}	
}

}	//	namespace Spr
