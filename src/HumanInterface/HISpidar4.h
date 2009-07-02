#ifndef HISPIDAR4_H
#define HISPIDAR4_H

//#include "HIHapticDevice.h"
//#include "HISpidarMotor.h"
//
//#include "Device/DVDeviceManager.h"
#include "HIHaptic.h"
#include "HISpidarMotor.h"
#include "HISpidarCalc3Dof.h"
#include "DVPioBase.h"

#include <vector>

namespace Spr {

/**	4�{����SPIDAR.
	���̒���������W�v�Z���s������A�o�͂���͊o����e���[�^��
	�o�͂��v�Z�����肷��B�o�͂̌v�Z��2���v��@��p���Ă��邽�߁C
	HIS/Fortran �� libF77.lib libI77.lib QuadPR.lib ���K�v�D
@verbatim
SPIDAR�̒萔�̐ݒ��
Init(dev, motPos, 0.5776, 2.924062107079e-5f, 0.5f, 20.0f);

//	���[�^�̎��t���ʒu. ���[�^�������̂Ɏ��t�����Ă���ꍇ�́C
#define PX	0.265f	//	x�����̕ӂ̒���/2
#define PY	0.265f	//	y�����̕ӂ̒���/2
#define PZ	0.265f	//	z�����̕ӂ̒���/2
Vec3f motorPos[][4] =	//	���[�^�̎��t���ʒu(���S�����_�Ƃ���){
	{Vec3f(-PX, PY, PZ),	Vec3f( PX, PY,-PZ), Vec3f(-PX,-PY,-PZ), Vec3f( PX,-PY, PZ)},
	{Vec3f( PX, PY, PZ),	Vec3f(-PX, PY,-PZ), Vec3f( PX,-PY,-PZ), Vec3f(-PX,-PY, PZ)}
};

//	�P�j���[�g���o�����߂�DA�̏o�͓d��[V/N]
//	maxon:		0.008[m] / 0.0438[Nm/A] / 0.5[A/V] =  0.365296803653
//	��t���@:	0.008[m] / 0.0277[Nm/A] / 0.5[A/V] =  0.5776
sp->VoltPerNewton(0.5776f);

//	�P�J�E���g�̒��� [m/plus]	�����l: 2.924062107079e-5f
sp->LenPerPlus(2.924062107079e-5f);

//	�ŏ����� [N]
sp->MinForce(0.5f);

//	�ő咣�� [N]
sp->MaxForce(20.0f);
@endverbatim
*/

class SPR_DLL HISpidar4: public HIForceInterface3D, public HISpidarCalc3Dof{
protected:
	bool bGood;
public:
	std::vector<HISpidarMotor> motors;	///<	���[�^
	std::vector<HISpidarMotor> dMotors;	///<	���[�^
	std::vector<DVPioBaseIf*> buttons;	///<	�{�^��

	///	�f�o�C�X�̃^�C�v
	SPR_OBJECTDEF(HISpidar4);
	
	HISpidar4();
	virtual ~HISpidar4();
	virtual bool Init(const void* pDesc);
	///	�L�����u���[�V�����̑O�ɍs������
	virtual bool BeforeCalibration(){ SetMinForce(); return true; }
	///	�L�����u���[�V����
	virtual bool Calibration();
	///	�L�����u���[�V�����̂��Ƃɍs������
	virtual bool AfterCalibration(){ return true; }
	///	�Œᒣ�͂��o��
	void SetMinForce();
	///	�͂̏o��
	virtual void SetForce(const Vec3f& f){ HISpidarCalc3Dof::SetForce(f); }
	virtual void SetForce(const Vec3f& f,const Vec3f& t){ HISpidarCalc3Dof::SetForce(f, t); }
	virtual Vec3f GetForce();
	virtual Vec3f GetPosition(){ return pos; }
	HISpidarMotor* Motor(){
		return motor;
	}
protected:
	///	�ʒu���v�Z���邽�߂̒萔
	Vec3f posSqrConst;
	///	�ʒu���v�Z���邽�߂̍s��
	Matrix3f matPos;
	Vec3f phi[4];
	float tension[4];	// computed tension
	
protected:
	///	���[�^
	HISpidarMotor motor[4];

	/**	�o�͉\�ȗ͂��v�Z���A���̏o�͂̓��_���v�Z����.
		@param disable	�g�p���Ȃ����̔ԍ�
		@param f		�o�͂�������. �o�͉\�ȗ͂ɕύX�����.
		@param v3Str	4�{�̎��̌���
		@return ���_(�͂̐��m���ƃ��[�^�o�͂̑傫���Ō��܂�B
				���̎���I�����邱�Ƃ̐�����)	*/
	float AvailableForce(int disable, Vec3f& f, Vec3f* v3Str);
	///	�ʒu���v�Z���邽�߂̍s��̏�����
	void InitMat();

	///	�f�o�C�X�̏�Ԃ��X�V����.
	virtual void Update(float dt);
	virtual void Update(){ HISpidarCalc3Dof::Update(); }

protected:
	virtual void MakeWireVec();					///<	���C�������x�N�g���̌v�Z
	virtual void UpdatePos();					///<	���C���ݒu�ʒu���W�̍X�V
	virtual void MeasureWire();					///<	���C�����̌v��
};

}	//	namespace Spr
#endif

