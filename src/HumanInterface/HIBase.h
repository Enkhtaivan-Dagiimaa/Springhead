#ifndef HIBASE_H
#define HIBASE_H

#include <Foundation/Object.h>
#include <SprHumanInterface.h>
#include <HumanInterface/IfStubHumanInterface.h>
#include <set>


namespace Spr{;
class HIRealDevice;

///	The base class of human interface classes.
class SPR_DLL HIBase: public NamedObject, HIBaseIfInit{
protected:
	static std::set<HIRealDevice*> realDevices;
	static int deviceUpdateStep;
	int updateStep;
	bool isGood;
public:
	OBJECTDEF_ABST(HIBase, NamedObject);
	///
	HIBase(){ updateStep = deviceUpdateStep; isGood=false;}

	///	キャリブレーションの前に行う処理
	virtual bool BeforeCalibration(){ return true; }
	///	キャリブレーション処理
	virtual bool Calibration(){ return true; }
	///	キャリブレーションのあとに行う処理
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
	virtual bool Init(HISdkIf* sdk, const void* desc){ return false; }
};
class HIPosition:public HIBase{
public:
	OBJECTDEF(HIPosition, HIBase);
	virtual Vec3f GetPosition();
};
class HIOrientation:public HIBase{
public:
	OBJECTDEF(HIOrientation, HIBase);
	virtual Quaternionf GetOrientation();
};

#define DEF_IF_FOR_OBJ(IF, OBJ)	class OBJ;						\
	struct IF##For##OBJ:public ObjectIfBufWithInit<IF##StubTemplate<IF, IF##For##OBJ, OBJ> >{}

DEF_IF_FOR_OBJ(HIOrientationIf, HIPose);

class HIPose:public HIPosition, HIPoseIfInit, HIOrientationIfForHIPose{
public:
	OBJECTDEF2(HIPose, HIPosition, HIOrientationIf);
	virtual Quaternionf GetOrientation();
	virtual Posef GetPose();
};
	
}


#endif

