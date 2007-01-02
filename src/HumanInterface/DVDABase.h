#ifndef VR_DVDABASE_H
#define VR_DVDABASE_H

#include <HumanInterface/HIVirtualDevice.h>

namespace Spr {

//----------------------------------------------------------------------------
//	DVDaBase

///	D/A��1�`�����l����\���D
class SPR_DLL DVDaBase:public HIVirtualDevice{
public:
	DVDaBase();
	///	�o�͂���f�W�^���l�̐ݒ�
	virtual void Digit(int d)=0;
	///	�o�͓d���̐ݒ�
	virtual void Voltage(float volt)=0;
	///	�f�o�C�X�̖��O
	virtual const char* Name() const =0;
	///	�f�o�C�X�̎��
	static const char* TypeS() { return "D/A"; }
	///	�f�o�C�X�̎��
	virtual const char* Type() const{ return TypeS(); }
};

}	//	namespace Spr

#endif
