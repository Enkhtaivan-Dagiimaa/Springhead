/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef HIVIRTUALDEVICEPOOL_H
#define HIVIRTUALDEVICEPOOL_H

#include <HumanInterface/HIVirtualDevice.h>
#include <vector>

namespace Spr {

//----------------------------------------------------------------------------
//	HIVirtualDevicePool

/**	���z�f�o�C�X�̃v�[��. ISA�J�[�h��PC�J�[�h�Ȃǂ̃f�o�C�X�́C
	�@�\(D/A,�J�E���^�Ȃ�)�ʂɕ������C���z�f�o�C�X�Ƃ��Ă���
	�v�[���ɓo�^�����D
	�o�^���ꂽ�f�o�C�X�́CSPIDAR�Ȃǂ̃V�X�e���ɗ��p�����D	*/

class SPR_DLL HIVirtualDevicePool: public UTRefCount, public std::vector< UTRef<HIVirtualDevice> >{
public:
	///	Rent device.
#ifdef __BORLANDC__
	template <class CT> bool Rent(CT*& t);
#else
	template <class CT> bool Rent(CT*& t){
		CT* pt = (CT*)Rent(CT::TypeS());
		t = pt;
		return t != NULL;
	}
#endif
	///	Rent device.
	HIVirtualDevice* Rent(const char* type, const char* name=NULL);
	HIVirtualDevice* RentNo(const char* type, int No=0, const char* name=NULL);
	///	return device.
	bool Return(HIVirtualDevice* dev);
	///	Register virutal device.
	void Register(HIVirtualDevice* dev){ push_back(dev); }
	///	�_���v�o��
	void Print(std::ostream& o) const;
	///
	void Clear(){ clear(); }
};
#ifdef __BORLANDC__
template <class CT> bool HIVirtualDevicePool::Rent(CT*& t){
		CT* pt = (CT*)Rent(CT::TypeS());
		t = pt;
		return t != NULL;
}
#endif
inline std::ostream& operator << (std::ostream& o, const HIVirtualDevicePool& d){
	d.Print(o);
	return o;
}

}	//	namespace Spr

#endif
