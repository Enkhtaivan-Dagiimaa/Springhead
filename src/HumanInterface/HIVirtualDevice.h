#ifndef HIVIRTUALDEVICE_H
#define HIVIRTUALDEVICE_H

#include <Foundation/Object.h>
#include <SprHumanInterface.h>
#include <HumanInterface/IfStubHumanInterface.h>

namespace Spr {

//----------------------------------------------------------------------------
//	HIVirtualDevice

class HIRealDevice;

///	���z�f�o�C�X�̊�{�N���X�DDA,�J�E���^�Ȃǂ�1�`�����l���ɑ�������.
class SPR_DLL HIVirtualDevice:public Object, public HIVirtualDeviceIfInit{
public:
	OBJECTDEF_ABST(HIVirtualDevice, Object);

	bool used;
	virtual bool GetUsed(){return used;}
	virtual void SetUsed(bool b){used = b;}
	friend class HIVirtualDevicePool;
public:
	HIVirtualDevice(){ used = false; }
	virtual ~HIVirtualDevice(){}
	///	�f�o�C�X�̖��O
	virtual const char* Name() const { return ""; }
	///	�f�o�C�X�̎��
	virtual const char* Type() const { return ""; }
	///	���f�o�C�X�ւ̃|�C���^
	virtual HIRealDeviceIf* RealDevice(){ return NULL; }
	///	�_���v�o��
	virtual void Print(std::ostream& o) const;
	///
	virtual void Update(){}
};
inline std::ostream& operator << (std::ostream& o, const HIVirtualDevice& d){
	d.Print(o);
	return o;
}
}	//	namespace Spr

#endif
