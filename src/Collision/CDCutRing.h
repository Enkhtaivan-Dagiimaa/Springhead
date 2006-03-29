#ifndef CDCUTRING_H
#define CDCUTRING_H

#include <SprCollision.h>
#include <Foundation/Object.h>

namespace Spr{;

///	�ڐG��͂Ɏg���N���X�D�ڐG�����̐؂���̐�����\���D
class CDCutLine{
public:
	CDCutLine(Vec2d n, double d):normal(n), dist(d){}
	Vec2d normal;
	double dist;
};
///	�ڐG��͂Ɏg���N���X�D�ڐG�����̐؂��
class CDCutRing{
public:
	///	�؂���̒���1�_�ƁC�؂���ʂ̍��W�n
//	Vec3d center;
//	Matrix3d local, localInv;
	Posed local, localInv;
	///	�؂�����\�����钼��
	std::vector<CDCutLine> lines;
	CDCutRing(Vec3d c, Matrix3d ccs){
		local.Ori().FromMatrix(ccs);
		local.Pos() = c;
		localInv = local.Inv();
	}
	void MakeRing();
	void Print(std::ostream& os);
};

}

#endif
