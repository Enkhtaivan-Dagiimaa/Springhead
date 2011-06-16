/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef DR_USBH8SIMPLE_H
#define DR_USBH8SIMPLE_H

#include <HumanInterface/HIDevice.h>

namespace Spr {

/**
	Cyverse��USB�W�����[�^�h���C�oH8�ł̃h���C�o.
 **/
class SPR_DLL DRUsbH8Simple: public HIRealDevice{
public:
	SPR_OBJECTDEF_NOIF(DRUsbH8Simple);

	///	���z�f�o�C�X
	class Da: public DVDa{
	public:
		Da(DRUsbH8Simple* r, int c):DVDa(r, c){}
		DRUsbH8Simple* GetRealDevice() { return realDevice->Cast(); }

		virtual void Voltage(float v){ GetRealDevice()->Voltage(portNo, v); }
		virtual void Digit(int v){ GetRealDevice()->Digit(portNo, v); }
	};
	class Counter: public DVCounter{
	public:
		Counter(DRUsbH8Simple* r, int c):DVCounter(r, c){}
		DRUsbH8Simple* GetRealDevice() { return realDevice->Cast(); }

		///	�J�E���^�l�̐ݒ�
		virtual void Count(long c){ GetRealDevice()->Count(portNo, c); }
		///	�J�E���^�l�̓ǂݏo��
		virtual long Count(){ return GetRealDevice()->Count(portNo); }
	};

protected:
	int		channel;
	void*	hSpidar;
	
	int		sign[8];
	long	count[8];
	long	countOffset[8];
	int		daOut[8];
public:
	DRUsbH8Simple(int ch=0);

	///	������
	virtual bool Init();
	///	���z�f�o�C�X�̓o�^
	//virtual void Register(HIVirtualDevicePool& vpool);

	///	�d���o��
	void Voltage(int ch, float v);
	///	�d���o��(���l)
	void Digit(int ch, int v);
	///	�J�E���^�l�̐ݒ�
	void Count(int ch, long c);
	///	�J�E���^�l�̓ǂݏo��
	long Count(int ch);
	///	��Ԃ̍X�V
	virtual void Update();
};

}	//	namespace Spr;

#endif
