#ifndef CDCONVEX_H
#define CDCONVEX_H
#include "CDGeometry.h"
#include "CDPhysicalMaterial.h"

namespace Spr {;

class CDConvex:public CDGeometry{
public:
	SGOBJECT_DEF_ABST(CDConvex);
	///	�T�|�[�g�|�C���g�����߂�D
	virtual Vec3f Support(const Vec3f& p) const =0;
};

}
#endif
