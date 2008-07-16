#ifndef HDSPIDARG6_H
#define HDSPIDARG6_H

#include "HIHaptic.h"
#include "HISpidarMotor.h"
#include "HISpidarCalc6Dof.h"
#include "DVPioBase.h"

namespace Spr {;

/**	@JA	�G���h�G�t�F�N�^(�O���b�v)�����̂�6���R�x��SPIDAR�D���̖{���͉ρD
	@EN	A SPIDAR with a 6DOF solid grip. The number of the strings is variable.@@*/
class SPR_DLL HISpidarG: public HIForceInterface6D, public HISpidarCalc6Dof{
protected:
	bool bGood;
public:
	std::vector<HISpidarMotor> motors;	///<	���[�^
	std::vector<DVPioBaseIf*> buttons;	///<	�{�^��

	///	�f�o�C�X�̃^�C�v
	SPR_OBJECTDEF(HISpidarG);

	HISpidarG();
	virtual ~HISpidarG();
	///	�f�o�C�X�̏�����(�g�p�O�ɌĂ�)
	virtual bool Init(const void* desc);
	///	�L�����u���[�V�����̑O�ɍs������
	virtual bool BeforeCalib(){ SetMinForce(); return true; }
	///	�L�����u���[�V����
	virtual bool Calib();
	///	�Œᒣ�͂��o��
	void SetMinForce();
	
	///	���[�^
	HISpidarMotorIf* GetMotor(size_t i);
	///	���[�^�̐�
	size_t NMotor() const;
	
	///	�ʒu�̎擾
	Vec3f GetPosition(){ return pos; }
	///	�p�x�̎擾
	Quaternionf GetOrientation(){ return ori; }
	
	///	�͂̐ݒ�
	void SetForce(const Vec3f& f){ SetForce(f, Vec3f()); }
	///	�g���N�̐ݒ�
	void SetForce(const Vec3f& f, const Vec3f& t){ HISpidarCalc6Dof::SetForce(f, t); }
	///	�d�ݕt���̐ݒ�
	void SetWeight(float s=0.3f, float t=1.0f, float r=6.0f){ smooth=s; lambda_t=t; lambda_r=r; }

	///	�͂̎擾
	Vec3f GetForce(){ return trnForce; }
	///	�g���N�̎擾
	Vec3f GetTorque(){ return rotForce; }

	/// �{�^�����̎擾
	int HISpidarG::GetButton(int ch){
		if(buttons[ch]) return buttons[ch]->Get();
		else return 0;
	}

	///	�f�o�C�X�̏�Ԃ��X�V����.
	virtual void Update(float dt);
	virtual void Update(){ HISpidarCalc6Dof::Update(); }

protected:
	virtual void MakeWireVec();					///<	���C�������x�N�g���̌v�Z
	virtual void UpdatePos();					///<	���C���ݒu�ʒu���W�̍X�V
	virtual void MeasureWire();					///<	���C�����̌v��
};

}	//	namespace Spr
#endif
