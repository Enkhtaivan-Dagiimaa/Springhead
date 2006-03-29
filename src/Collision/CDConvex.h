#ifndef CDCONVEX_H
#define CDCONVEX_H

#include <SprCollision.h>
#include <Foundation/Object.h>
#include "CDShape.h"
#include "CDCutRing.h"

namespace Spr{;
class CDConvex : public CDShape{
public:
	OBJECTDEFABST(CDConvex);
	///	�T�|�[�g�|�C���g(�����x�N�g��p�Ƃ̓��ς��ő�̒��_)�����߂�D
	virtual Vec3f Support(const Vec3f& p) const =0;
	///	�؂�������߂�D�ڐG��͂Ɏg���D
	virtual void FindCutRing(CDCutRing& r, const Posed& toW) =0;
	///	�o�E���f�B���O�{�b�N�X�����߂�D
	virtual void CalcBBox(Vec3f& bbmin, Vec3f& bbmax);
};

}	//	namespace Spr
#endif
