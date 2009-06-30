#include "HumanInterface.h"
#ifdef _WIN32
 #pragma hdrstop
#endif
#include "HISpidar4D.h"

namespace Spr {
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
//----------------------------------------------------------------------------

//HIOBJECTIMP(HISpidar4D, HIForceDevice3D);
HISpidar4DDesc::HISpidar4DDesc(){


}

void HISpidar4DDesc::Init(int nMotor, Vec3f* motorPos, Vec3f* knotPos, float vpn, float lpp, float minF, float maxF){
	motors.resize(4);
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
void HISpidar4DDesc::Init(char* type){
	if (stricmp(type, "SpidarG6X3L")==0 || stricmp(type, "SpidarG6X3R")==0){
		bool bLeft = stricmp(type, "SpidarG6X3L")==0;
		//	���[�^�̎��t���ʒu. ���[�^�������̂Ɏ��t�����Ă���ꍇ�́C
		const float PX = 0.12f/2;		//	x�����̕ӂ̒���/2
		const float PY = 0.14f/2;		//	y�����̕ӂ̒���/2
		const float PZ = 0.12f/2;		//	z�����̕ӂ̒���/2
		//	���̃O���b�v�ւ̎��t���ʒu�D�O���b�v�̓s���|���ʂ��̗p���Ă��܂��D
		const float GX = 0.02f/2;		//	x�����̕ӂ̒���/2
		const float GY = 0.02f/2;		//	y�����̕ӂ̒���/2
		Matrix3f rotR = Matrix3f::Rot((float)Rad(-45), 'y');
		Matrix3f rotL = Matrix3f::Rot((float)Rad(-45), 'y');
		Matrix3f rotZ = Matrix3f::Rot((float)Rad(-45), 'z');

	//Vec3f motorPos[2][8][2] = {		//	���[�^�̎��t���ʒu(���S�����_�Ƃ���DirectX���W�n�i�E��X,�オY,����Z�j)
	//	{
	//		{rotR*Vec3f(-PX,-PY, PZ), rotR*Vec3f( -GX, 0.0f, 0.0f)},
	//		{rotR*Vec3f( PX,-PY, PZ), rotR*Vec3f(  GX, 0.0f, 0.0f)},
	//		{rotR*Vec3f( PX,-PY,-PZ), rotR*Vec3f(0.0f,  -GY, 0.0f)},
	//		{rotR*Vec3f(-PX,-PY,-PZ), rotR*Vec3f(0.0f,  -GY, 0.0f)},
	//		{rotR*Vec3f(-PX, PY, PZ), rotR*Vec3f( -GX, 0.0f, 0.0f)},
	//		{rotR*Vec3f( PX, PY, PZ), rotR*Vec3f(  GX, 0.0f, 0.0f)},
	//		{rotR*Vec3f( PX, PY,-PZ), rotR*Vec3f(0.0f,   GY, 0.0f)},
	//		{rotR*Vec3f(-PX, PY,-PZ), rotR*Vec3f(0.0f,   GY, 0.0f)}
	//	},
	//	{
	//		{rotL*Vec3f( PX, PY,-PZ), rotL*Vec3f(0, 0.0f,  -GX)},
	//		{rotL*Vec3f( PX, PY, PZ), rotL*Vec3f(0, 0.0f,   GX)},
	//		{rotL*Vec3f(-PX, PY, PZ), rotL*Vec3f(0,   GY, 0.0f)},
	//		{rotL*Vec3f(-PX, PY,-PZ), rotL*Vec3f(0,   GY, 0.0f)},
	//		{rotL*Vec3f( PX,-PY,-PZ), rotL*Vec3f(0, 0.0f,  -GX)},
	//		{rotL*Vec3f( PX,-PY, PZ), rotL*Vec3f(0, 0.0f,   GX)},
	//		{rotL*Vec3f(-PX,-PY, PZ), rotL*Vec3f(0,  -GY, 0.0f)},
	//		{rotL*Vec3f(-PX,-PY,-PZ), rotL*Vec3f(0,  -GY, 0.0f)},
	//	}
	//};

	Vec3f motorPos[2][4][2] = {		//	���[�^�̎��t���ʒu(���S�����_�Ƃ���DirectX���W�n�i�E��X,�オY,����Z�j)
		{
			{rotR*Vec3f(-PX,-PY, PZ), rotR*rotZ*Vec3f(0.0f, -GY, 0.0f)},//1����
			{rotR*Vec3f( PX,-PY,-PZ), rotR*rotZ*Vec3f( GX, 0.0f, 0.0f)},//3���E
			{rotR*Vec3f( PX, PY, PZ), rotR*rotZ*Vec3f(0.0f,  GY, 0.0f)},//6�㉜
			{rotR*Vec3f(-PX, PY,-PZ), rotR*rotZ*Vec3f(-GX, 0.0f, 0.0f)}//8��O
		},
		{
			{rotL*Vec3f( PX, PY, PZ), rotL*rotZ*Vec3f( GX, 0.0f, 0.0f)},//2����
			{rotL*Vec3f(-PX, PY,-PZ), rotL*rotZ*Vec3f(0.0f, -GY, 0.0f)},//4���O
			{rotL*Vec3f( PX,-PY,-PZ), rotL*rotZ*Vec3f(-GX, 0.0f, 0.0f)},//5�㍶
			{rotL*Vec3f(-PX,-PY, PZ), rotL*rotZ*Vec3f(0.0f,  GY, 0.0f)},//7��E

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

//---------------------------------------------------------------------------------------------------
HISpidar4D::HISpidar4D(){
    for (int i=0;i<4;i++){
        tension[i] = 0;
    }
}
HISpidar4D::~HISpidar4D(){SetMinForce();}


bool HISpidar4D::Init(const void* pDesc){
	HISdkIf* sdk = GetSdk();
	HISpidar4DDesc& desc = *(HISpidar4DDesc*)pDesc;
	//	�v�Z�̂��߂̒萔�̐ݒ�
	nRepeat = 2;
	sigma = 0.001f; //sigma=sigma*sigma
	int nMotor = desc.motors.size();
	motors.resize(nMotor);
	VVector<float> minForce, maxForce;
	minForce.resize(desc.motors.size());
	maxForce.resize(desc.motors.size());
	for(unsigned i=0; i<desc.motors.size(); ++i){
		motors[i].SetDesc(&desc.motors[i]);
		minForce[i] = desc.motors[i].minForce;
		maxForce[i] = desc.motors[i].maxForce;
	}
	HISpidarCalc3Dof::Init(3, minForce, maxForce);
	////	�h���C�o�̎擾
	int i;
	for(i=0; i<4; ++i){
		motors[i].da = sdk->RentVirtualDevice(DVDaBase::TypeS())->Cast();
		if (!motors[i].da) break;
		AddDeviceDependency(motors[i].da->RealDevice()->Cast());
	}
	if (i<4) return false;
	for(i=0; i<4; ++i){
		motors[i].counter = sdk->RentVirtualDevice(DVCounterBase::TypeS())->Cast();
		if (!motors[i].counter) break;
		AddDeviceDependency(motors[i].counter->RealDevice()->Cast());
	}
	if (i<4) return false;
	InitMat();
	BeforeCalibration();
	Calibration();
	AfterCalibration();
	bGood = true;
	return true;
}

bool HISpidar4D::Calibration(){
	//	�|�C���^�����_(���S)�ɒu���āA�L�����u���[�V�������s��
	// calibration sets the center of the reference frame at the current
	// position of the spidar (motor[i].pos.norm())
	Update(0.001f);
	for(int i=0; i<4; i++) motor[i].SetLength(motor[i].pos.norm());
	return true;
}
void HISpidar4D::Update(float dt){
	HIForceInterface3D::Update(dt);
	HISpidarCalc3Dof::Update();
	for(unsigned int i=0; i<4; ++i){
		motors[i].SetForce(Tension()[i]);
	}
}

Vec3f HISpidar4D::GetForce(){
    int i;
	Vec3f f;
    for (i=0;i<4;i++) f=f+tension[i]*phi[i];
    return f;
}

void HISpidar4D::SetMinForce(){
	for(int i=0; i<4; i++) motor[i].SetForce(motor[i].minForce);
}
void HISpidar4D::InitMat(){
	matPos = Matrix3f(
		motor[1].pos-motor[0].pos,
		motor[2].pos-motor[1].pos,
		motor[3].pos-motor[2].pos).trans() * 2;
	matPos = matPos.inv();

	posSqrConst = Vec3f(motor[1].pos.square()-motor[0].pos.square(),
		motor[2].pos.square()-motor[1].pos.square(),
		motor[3].pos.square()-motor[2].pos.square());
}

void HISpidar4D::MakeWireVec(){
	for(unsigned int i=0; i<4; ++i){
		wireDirection[i] = motors[i].pos - pos;
		calculatedLength[i] = wireDirection[i].norm();
		wireDirection[i] /= calculatedLength[i];
	}
}
void HISpidar4D::UpdatePos(){
}
void HISpidar4D::MeasureWire(){
	for(unsigned int i=0; i<4; ++i){
		measuredLength[i] = motors[i].GetLength();
	}	
}

}	//	namespace Spr
