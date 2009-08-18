#include "HumanInterface.h"
#ifdef _WIN32
 #pragma hdrstop
#endif
#include "HISpidarG.h"

namespace Spr {;


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
		const float PY = 0.14f/2;		//	y�����̕ӂ̒���/2
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
HISpidarG::HISpidarG():bGood(false){
	SetWeight();
}
HISpidarG::~HISpidarG(){
	SetMinForce();
}
bool HISpidarG::Init(const void* pDesc){
	HISdkIf* sdk = GetSdk();
	HISpidarGDesc& desc = *(HISpidarGDesc*)pDesc;
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
	HISpidarCalc6Dof::Init(6, minForce, maxForce);

	//	�h���C�o�̎擾
	unsigned int i;
	for(i=0; i<motors.size(); ++i){
		motors[i].da = sdk->RentVirtualDevice(DVDaBase::TypeS())->Cast();
		if (!motors[i].da) break;
		AddDeviceDependency(motors[i].da->RealDevice()->Cast());
	}
	if (i<motors.size()) return false;
	for(i=0; i<motors.size(); ++i){
		motors[i].counter = sdk->RentVirtualDevice(DVCounterBase::TypeS())->Cast();
		if (!motors[i].counter) break;
		AddDeviceDependency(motors[i].counter->RealDevice()->Cast());
	}
	buttons.resize(8);
	for(i=0; i<buttons.size(); ++i){
		buttons[i] = sdk->RentVirtualDevice(DVPioBase::TypeS())->Cast();
		if (!buttons[i]) break;
		AddDeviceDependency(buttons[i]->RealDevice()->Cast());
	}

	SetMinForce();
	BeforeCalibration();
	Calibration();
	AfterCalibration();
	bGood = true;
	return true;
}
bool HISpidarG::Calibration(){
	//	�|�C���^�����_(���S)�ɒu���āA�L�����u���[�V�������s��
	for(unsigned i=0; i<motors.size(); i++) motors[i].SetLength( (motors[i].pos - motors[i].jointPos).norm() );
	lengthDiffAve.clear();
	pos = Vec3f();
	ori = Quaterniond();
	for(int i=0; i<4; ++i) HISpidarCalc6Dof::Update();	//	�p�����X�V
	return true;
}
void HISpidarG::Update(float dt){
	if (!bGood) return;
	HIForceInterface6D::Update(dt);
	HISpidarCalc6Dof::Update();
	for(unsigned int i=0; i<motors.size(); ++i){
		motors[i].SetForce(Tension()[i]);
	}
}
void HISpidarG::SetMinForce(){
	for(unsigned int i=0; i<motors.size(); ++i){
		motors[i].SetForce(motors[i].minForce);
	}
	Update(0.001f);
}
void HISpidarG::MakeWireVec(){
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
	for(unsigned int i=0; i<motors.size(); ++i){
		measuredLength[i] = motors[i].GetLength();
	}	
}
HISpidarMotorIf* HISpidarG::GetMotor(size_t i){
	return motors[i].Cast();
}
size_t HISpidarG::NMotor() const{
	return motors.size();
}


}//	namespace Spr
