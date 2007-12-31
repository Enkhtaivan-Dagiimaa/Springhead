/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef HIREALDEVICEPOOL_H
#define HIREALDEVICEPOOL_H

#include "HIRealDevice.h"

namespace Spr {

/**	���f�o�C�X�̃v�[��. 
	���[�U��ISA�J�[�h��PC�J�[�h�Ȃǂ̎��f�o�C�X�������ɓo�^����D
	�f�o�C�X�͏��������ɁC���z�f�o�C�X(DVVirtualDevices)�ɓo�^�����D	*/
class SPR_DLL HIRealDevicePool: public UTRefCount, public std::vector< UTRef<HIRealDevice> >{
public:
	HIRealDevicePool();
	~HIRealDevicePool();
	///	�o�^
	void Register(HIRealDevice* dev);
	///	������
	void Init(HISdkIf* sdk);
	///	�_���v�o��
	void Print(std::ostream& o) const;
	///
	void Clear(){ clear(); }
};
inline std::ostream& operator << (std::ostream& o, const HIRealDevicePool& d){
	d.Print(o);
	return o;
}
}	//	namespace Spr

#endif
