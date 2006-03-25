#ifndef CDCUTRING_H
#define CDCUTRING_H

#include <SprCollision.h>
#include <Foundation/Object.h>

namespace Spr{;

///	�ڐG��͂Ɏg���N���X�D�ڐG�����̐؂���̐�����\���D
class CDCutLine{
public:
	Vec2d normal;
	double dist;
};
///	�ڐG��͂Ɏg���N���X�D�ڐG�����̐؂��
class CDCutRing{
public:
	///	�؂���̒���1�_�ƁC�؂���ʂ̍��W�n
	Vec3d center;
	Matrix3d local, local_inv;
	///	�؂�����\�����钼��
	std::vector<CDCutLine> lines;
	CDCutRing(Vec3d c, Matrix3d ccs): center(c), local(ccs){
		local_inv = local.inv();
	}
	void MakeRing();
};

}

#endif
