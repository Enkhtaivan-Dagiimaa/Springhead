#ifndef CDCONVEX_H
#define CDCONVEX_H

#include <SprCollision.h>
#include <Foundation/Object.h>
#include "CDShape.h"

namespace Spr{;
class CDConvex : public CDShape{
public:
	OBJECTDEFABST(CDConvex);
	///	�T�|�[�g�|�C���g(�����x�N�g��p�Ƃ̓��ς��ő�̒��_)�����߂�D
	virtual Vec3f Support(const Vec3f& p) const =0;
	virtual void CalcBBox(Vec3f& bbmin, Vec3f& bbmax);
};

}	//	namespace Spr
#endif
