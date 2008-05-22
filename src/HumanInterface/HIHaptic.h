#ifndef HI_HAPITC_INTERFACE_H
#define HI_HAPITC_INTERFACE_H

#include "HIBase.h"
#include <Device/DRRealDevice.h>

namespace Spr {
//----------------------------------------------------------------------------
//	HIForceInterface6D

///	���i��]6���R�x�̃f�o�C�X
class SPR_DLL HIForceInterface6D: public HIPose{
protected:
	float alpha;
	Vec3f vel, angVel;
	Vec3f lastPos;
	Quaternionf lastOri;
public:
	SPR_OBJECTDEF_ABST(HIForceInterface6D);
	///
	HIForceInterface6D();
	///
	virtual ~HIForceInterface6D(){}
	virtual void Update(float dt);

	/**@name	�f�o�C�X�̏��	*/
	//@{
	///	�f�o�C�X�̌��݈ʒu��Ԃ�
	virtual Vec3f GetPosition()=0;
	///	�f�o�C�X�̌�����Ԃ�
	virtual Quaternionf GetOrientation()=0;
	///	�f�o�C�X�̑��x��Ԃ�
	virtual Vec3f GetVelocity(){ return vel; }
	///	�f�o�C�X�̊p���x��Ԃ�
	virtual Vec3f GetAngularVelocity(){ return angVel; }
	///	�f�o�C�X�̎��ۂ̒񎦃g���N��Ԃ�
	virtual Vec3f GetTorque()=0;
	///	�f�o�C�X�̎��ۂ̒񎦗͂�Ԃ�
	virtual Vec3f GetForce()=0;
	///	�f�o�C�X�̖ڕW�o�͂ƃg���N�o�͂�ݒ肷��
	virtual void SetForce(const Vec3f& f, const Vec3f& t)=0;
	//@}
};

}	//	namespace Spr
#endif
