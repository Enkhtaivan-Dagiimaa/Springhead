/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef VR_DRCONTECISADA_H
#define VR_DRCONTECISADA_H

#include <HumanInterface/HIDevice.h>

namespace Spr {

///	Contec��ISA D/A�J�[�h�p�̃h���C�o.
class SPR_DLL DRContecIsaDa: public HIRealDevice{
public:
	SPR_OBJECTDEF_NOIF(DRContecIsaDa);

	///	���z�f�o�C�X
	class Da: public DVDa{
	public:
		Da(DRContecIsaDa* r, int c):DVDa(r, c){}
		DRContecIsaDa* GetRealDevice() { return realDevice->Cast(); }
		
		virtual void Voltage(float v){ GetRealDevice()->Voltage(portNo, v); }
		virtual void Digit(int d){ GetRealDevice()->Digit(portNo, d); }
	};
protected:
	///	D/A��IO�A�h���X
	int address;
public:
	/**	�R���X�g���N�^
		@param address		D/A��I/O�A�h���X	*/
	DRContecIsaDa(int address = 0);
	///	������
	virtual bool Init();
	///	���z�f�o�C�X�̓o�^
	//virtual void Register(HIVirtualDevicePool& vpool);

	///	�d���o��
	void Voltage(int ch, float v);
	///	�d���o��(���l�w��)
	void Digit(int ch, int v);
};


}	//	namespace Spr

#endif