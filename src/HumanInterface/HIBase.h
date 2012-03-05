/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef HIBASE_H
#define HIBASE_H

#include <Foundation/Object.h>
#include <SprHumanInterface.h>
#include <set>

namespace Spr{;
class HIRealDevice;

///	The base class of human interface classes.
class HIBase: public NamedObject{
public:
	struct RealDevices:public std::set<HIRealDevice*>, public UTRefCount{};
protected:
	static UTRef<RealDevices> GetRealDevices();
	static int deviceUpdateStep;
	int updateStep;
	bool isGood;
public:
	SPR_OBJECTDEF_ABST(HIBase);
	///
	HIBase(){ updateStep = deviceUpdateStep; isGood=false;}

	///	SDK�̎擾
	HISdkIf* GetSdk();
	///	�L�����u���[�V�����̑O�ɍs������
	virtual bool BeforeCalibration(){ return true; }
	///	�L�����u���[�V��������
	virtual bool Calibration(){ return true; }
	///	�L�����u���[�V�����̂��Ƃɍs������
	virtual bool AfterCalibration(){ return true; }
	///	Is this human interface good for use?
	bool IsGood(){ return isGood; }
	///	Update state of the interface; read from/write to real device.
	virtual void Update(float dt);
	/**	Add an real device to the dependency list of real devices. 
		Update() will call real device's Update() function.	*/
	void AddDeviceDependency(HIRealDeviceIf* rd);
	///	Clear the dependency list.
	static void ClearDeviceDependency();
	///	Init human interface
	virtual bool Init(const void* desc){ return false; }
};

/*class HIPosition:public virtual HIBase{
public:
	SPR_OBJECTDEF(HIPosition);
	virtual Vec3f GetPosition();
};

class HIOrientation:public virtual HIBase{
public:
	SPR_OBJECTDEF(HIOrientation);
	virtual Quaternionf GetOrientation();
};*/

//class HIPose:public HIPosition, public HIOrientation{
class HIPose: public HIBase{
public:
	SPR_OBJECTDEF(HIPose);
	virtual Vec3f		GetPosition(){ return Vec3f(); }
	virtual Quaternionf GetOrientation(){ return Quaternionf(); }
	virtual Posef		GetPose(){
		Posef rv;
		rv.Ori() = GetOrientation();
		rv.Pos() = GetPosition();
		return rv;
	}
	virtual Affinef		GetAffine(){
		Affinef aff;
		GetPose().ToAffine(aff);
		return aff;
	}
	virtual Vec3f GetVelocity(){ return Vec3f(); }
	virtual Vec3f GetAngularVelocity(){ return Vec3f(); }
};


class SPR_DLL HIHaptic: public HIPose{
protected:
	Vec3f vel, angVel;
	float alpha;
	Vec3f lastPos;
	Quaternionf lastOri;
public:
	SPR_OBJECTDEF_ABST(HIHaptic);
	///
	HIHaptic(){ alpha = 0.8f; }
	///
	virtual ~HIHaptic(){}
	virtual void Update(float dt);

	/**@name	�f�o�C�X�̏��	*/
	//@{
	///	�f�o�C�X�̎��ۂ̒񎦃g���N��Ԃ�
	virtual Vec3f GetTorque(){ return Vec3f(); }
	///	�f�o�C�X�̎��ۂ̒񎦗͂�Ԃ�
	virtual Vec3f GetForce(){ return Vec3f(); }
	///	�f�o�C�X�̖ڕW�o�͂ƃg���N�o�͂�ݒ肷��
	virtual void SetForce(const Vec3f& f, const Vec3f& t = Vec3f()){}
	///	�f�o�C�X�̑��x��Ԃ�
	virtual Vec3f GetVelocity(){ return vel; }
	///	�f�o�C�X�̊p���x��Ԃ�
	virtual Vec3f GetAngularVelocity(){ return angVel; }
	//@}
};
	
}


#endif

