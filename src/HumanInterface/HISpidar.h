#ifndef HI_SPIDAR_H
#define HI_SPIDAR_H

#include <HumanInterface/HISpidarMotor.h>
#include <HumanInterface/HISpidarCalc.h>
#include <vector>

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

namespace Spr {;

/**
	Spidar�̊�{�N���X
 **/
class HISpidar: public HIHaptic{
protected:
	bool bGood;
	std::vector<HISpidarMotor>	motors;		///<	���[�^

public:
	SPR_OBJECTDEF_ABST(HISpidar);

	///	���[�^���擾
	HISpidarMotorIf* GetMotor(size_t i){ return motors[i].Cast(); }
	///	���[�^�̐�
	size_t NMotor() const { return motors.size(); }

	///	�Œᒣ�͂��o��
	void SetMinForce();
	/// �S�Ẵ��[�^�ɑ΂��čŏ��o�͂�ݒ�
	void SetLimitMinForce(float f);
	/// �S�Ẵ��[�^�ɑ΂��čő�o�͂�ݒ�
	void SetLimitMaxForce(float f);
	
	virtual bool Init(const void* desc);
	virtual bool BeforeCalibration(){
		SetMinForce();
		return true;
	}
	
	HISpidar(){ bGood = false; }
	virtual ~HISpidar(){ SetMinForce(); }
};

/**
	4-wire Spidar
	�����v�Z��HISpidarCalc3Dof���g�p
 */
class SPR_DLL HISpidar4: public HISpidar, public HISpidarCalc3Dof{
public:
	SPR_OBJECTDEF(HISpidar4);

	HISpidar4(const HISpidar4Desc& desc = HISpidar4Desc());
	virtual ~HISpidar4();
	Vec3f	GetPosition(){ return HISpidarCalc3Dof::GetPosition(); }

protected:
	/**	�o�͉\�ȗ͂��v�Z���A���̏o�͂̓��_���v�Z����.
		@param disable	�g�p���Ȃ����̔ԍ�
		@param f		�o�͂�������. �o�͉\�ȗ͂ɕύX�����.
		@param v3Str	4�{�̎��̌���
		@return ���_(�͂̐��m���ƃ��[�^�o�͂̑傫���Ō��܂�B
				���̎���I�����邱�Ƃ̐�����)	*/
	float AvailableForce(int disable, Vec3f& f, Vec3f* v3Str);

	virtual bool Init(const void* pDesc);
	virtual bool Calibration();
	virtual void Update(float dt);

	/// HISpidarCalc3Dof�̉��z�֐�
	virtual void MakeWireVec();					///<	���C�������x�N�g���̌v�Z
	virtual void UpdatePos();					///<	���C���ݒu�ʒu���W�̍X�V
	virtual void MeasureWire();					///<	���C�����̌v��
};

/**
	4-wire Spidar double grip
	�����v�Z�͓Ǝ������i���ڈʒu�v�Z���s�������j
 */
class HISpidar4D: public HISpidar4{
public:
	SPR_OBJECTDEF(HISpidar4D);

protected:
	Vec3f pos;
	///	�ʒu���v�Z���邽�߂̒萔
	Vec3f posSqrConst;
	///	�ʒu���v�Z���邽�߂̍s��
	Matrix3f matPos;
	Vec3f	phi[4];
	float	eff;
	float	cont;
	float	tension[4];
	///	�ʒu���v�Z���邽�߂̍s��̏�����
	void InitMat();

public:
	virtual bool Calibration();
	virtual void Update(float dt);

	void	SetForce(const Vec3f& f);
	Vec3f	GetForce();
	Vec3f	GetPosition(){ return pos; }

	HISpidar4D(const HISpidar4DDesc& desc = HISpidar4DDesc());
};

/**	@JA	�G���h�G�t�F�N�^(�O���b�v)�����̂�6���R�x��SPIDAR�D���̖{���͉ρD
	@EN	A SPIDAR with a 6DOF solid grip. The number of the strings is variable.@@*/
class SPR_DLL HISpidarG: public HISpidar, protected HISpidarCalc6Dof{
public:
	SPR_OBJECTDEF(HISpidarG);

	std::vector< DVPioIf* >		buttons;	///<	�{�^��

public:
	HISpidarG(const HISpidarGDesc& desc = HISpidarGDesc());
	virtual ~HISpidarG();

	/// �{�^���̏�Ԃ��擾
	int GetButton(size_t i){
		if(buttons[i]) return buttons[i]->Get();
		else return 0;
	}
	/// �{�^���̐�
	size_t NButton() const { return buttons.size(); }

	///	�f�o�C�X�̏�����(�g�p�O�ɌĂ�)
	virtual bool Init(const void* desc);
	virtual bool Calibration();
	Vec3f	GetPosition(){ return HISpidarCalc6Dof::GetPosition(); }
	Quaternionf GetOrientation(){ return HISpidarCalc6Dof::GetOrientation(); }
	virtual Vec3f GetTorque(){ return HISpidarCalc6Dof::GetTorque(); }
	virtual Vec3f GetForce(){ return HISpidarCalc6Dof::GetForce(); }
	virtual void SetForce(const Vec3f& f, const Vec3f& t = Vec3f()){ HISpidarCalc6Dof::SetForce(f, t); }		

	///	�d�ݕt���̐ݒ�
	void SetWeight(float s=0.3f, float t=1.0f, float r=6.0f){ smooth=s; lambda_t=t; lambda_r=r; }

	///	�f�o�C�X�̏�Ԃ��X�V����.
	virtual void Update(float dt);

protected:
	virtual void MakeWireVec();					///<	���C�������x�N�g���̌v�Z
	virtual void UpdatePos();					///<	���C���ݒu�ʒu���W�̍X�V
	virtual void MeasureWire();					///<	���C�����̌v��
};

}	//	namespace Spr
#endif

