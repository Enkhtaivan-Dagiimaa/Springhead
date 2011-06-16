/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef DRADAPIO_H
#define DRADAPIO_H

#include <HumanInterface/HIDevice.h>
#include "tuadapio.h"

namespace Spr {

///	�^�[�g���H�Ƃ�A/D�ED/A�EPIO�p�̃h���C�o.
class SPR_DLL DRAdapio: public HIRealDevice{
public:
	SPR_OBJECTDEF_NOIF(DRAdapio);

	///	���z�f�o�C�X
	class Da: public DVDa{
	public:
		Da(DRAdapio* r, int c):DVDa(r, c){}
		DRAdapio* GetRealDevice() { return realDevice->Cast(); }

		virtual void Voltage(float v){ GetRealDevice()->DAVoltage(portNo, v); }
		virtual void Digit(int d){ GetRealDevice()->DADigit(portNo, d); }
	};
	class Ad: public DVAd{
	public:
		Ad(DRAdapio* r, int c):DVAd(r, c){}
		DRAdapio* GetRealDevice() { return realDevice->Cast(); }

		virtual int Digit(){ return GetRealDevice()->ADDigit(portNo); }
		virtual float Voltage(){ return GetRealDevice()->ADVoltage(portNo); }
	};
protected:
	int id;
public:
	/**	�R���X�g���N�^
		@param id		ADAPIO����������ꍇ�C���Ԗڂ̃f�o�C�X�����w��D*/
	DRAdapio(int id=0);
	virtual ~DRAdapio();

	///	������
	virtual bool Init();
	///	���z�f�o�C�X�̓o�^
	//virtual void Register(HIVirtualDevicePool& vpool);

	///	�d���o��
	void DAVoltage(int ch, float v);
	void DADigit(int ch, int d);
	
	///	�d������
	float ADVoltage(int ch);
	int ADDigit(int ch);
	
	/// �I������
	void CloseDevice();
};

}	//	namespace Spr

#endif
