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
