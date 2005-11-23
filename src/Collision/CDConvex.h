#ifndef CDCONVEX_H
#define CDCONVEX_H

#include <Springhead.h>
#include <Foundation/Object.h>
#include "CDShape.h"

namespace Spr{;
class CDConvex : public CDShape{
public:
	OBJECTDEFABST(CDConvex);
	///	�T�|�[�g�|�C���g�����߂�D
	virtual Vec3f Support(const Vec3f& p) const =0;
};

}	//	namespace Spr
#endif
