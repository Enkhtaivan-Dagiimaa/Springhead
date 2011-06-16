/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef VR_DRCONTECISACOUNTER_H
#define VR_DRCONTECISACOUNTER_H

#include <HumanInterface/HIDevice.h>

namespace Spr {

///	Contec��ISA D/A�J�[�h�p�̃h���C�o.
class SPR_DLL DRContecIsaCounter: public HIRealDevice{
public:
	SPR_OBJECTDEF_NOIF(DRContecIsaCounter);

	///	���z�f�o�C�X
	class DV: public DVCounter{
	public:
		DV(DRContecIsaCounter* r, int c):DVCounter(r, c){}
		DRContecIsaCounter* GetRealDevice() { return realDevice->Cast(); }

		///	�J�E���^�l�̐ݒ�
		virtual void Count(long c){ GetRealDevice()->Count(portNo, c); }
		///	�J�E���^�l�̓ǂݏo��
		virtual long Count(){ return GetRealDevice()->Count(portNo); }
	};
protected:
	///	�J�E���^�{�[�h��IO�A�h���X
	int address;

public:
	/**	�R���X�g���N�^
		@param address		�J�E���^�{�[�h��I/O�A�h���X
	*/
	DRContecIsaCounter(int address = 0);

	///	������
	virtual bool Init();
	///	���z�f�o�C�X�̓o�^
	//virtual void Register(HIVirtualDevicePool& vpool);
	///	�J�E���^�l�̐ݒ�
	void Count(int ch, long c);
	///	�J�E���^�l�̓ǂݏo��
	long Count(int ch);
};
}	//	namespace Spr

#endif