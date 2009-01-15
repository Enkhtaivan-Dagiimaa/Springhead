#ifndef HINOVINTFALCON_H
#define HINOVINTFALCON_H

#include "HIHaptic.h"

namespace Spr {;

/**	@JA	Novint�Ђ�Falcon�B�R���R�x�͊o�C���^�t�F�[�X
	@EN	A Novint company's Falcon interface. A 3 DOF haptic interface. @@*/
class SPR_DLL HINovintFalcon: public HIForceInterface6D{
protected:
	bool good;
	Vec3d pos;
	Vec3d force;
	int button;
	int deviceHandle;
public:
	///	�f�o�C�X�̃^�C�v
	SPR_OBJECTDEF(HINovintFalcon);

	HINovintFalcon();
	virtual ~HINovintFalcon();
	///	�f�o�C�X�̏�����(�g�p�O�ɌĂ�)
	virtual bool Init(const void* desc);
	///	�L�����u���[�V�����̑O�ɍs������
	virtual bool BeforeCalibration();
	///	�L�����u���[�V����
	virtual bool Calibration();
	///	�ʒu�̎擾
	Vec3f GetPosition(){ return pos; }
	Quaternionf GetOrientation(){ return Quaternionf(); }
	
	///	�͂̐ݒ�
	void SetForce(const Vec3f& f, const Vec3f& t=Vec3f()){ force = f; }
	///	���ۂɏo�����͂̎擾
	Vec3f GetForce(){ return force; }
	Vec3f GetTorque(){ return Vec3f(); }

	/// �{�^�����̎擾
	int GetButton(int ch);
	int GetButtonBit();

	///	�f�o�C�X�̏�Ԃ��X�V����.
	virtual void Update(float dt);
};

}	//	namespace Spr
#endif
