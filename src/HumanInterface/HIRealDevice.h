#ifndef HIREALDEVICEBASE_H
#define HIREALDEVICEBASE_H

#include <SprHumanInterface.h>
#include <Foundation/Object.h>
#include <HumanInterface/IfStubHumanInterface.h>

namespace Spr {;
class HIVirtualDevicePool;

///	���f�o�C�X�̊�{�N���X�DISA�J�[�h1���Ȃǂɑ�������.
class SPR_DLL HIRealDevice:public Object, public HIRealDeviceIfInit{
public:
	OBJECTDEF_ABST(HIRealDevice, Object);
	HIRealDevice(){}
	virtual ~HIRealDevice(){}
	///	�f�o�C�X�̖��O
	virtual const char* Name() const { return ""; }
	///	������
	virtual bool Init(){ return false; }
	///	���z�f�o�C�X�̓o�^
	virtual void Register(HISdkIf* sdk){}
	///	��Ԃ̍X�V
	virtual void Update(){}
	///	�_���v�o��
	virtual void Print(std::ostream& o) const;
};
}	//	namespace Spr

#endif
