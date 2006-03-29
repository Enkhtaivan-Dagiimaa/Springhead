#ifndef CDCUTRING_H
#define CDCUTRING_H

#include <SprCollision.h>
#include <Collision/CDQuickHull2D.h>
#include <Foundation/Object.h>

namespace Spr{;

///	�ڐG��͂Ɏg���N���X�D�ڐG�����̐؂���̐�����\���D
class CDCutLine{
protected:
	Vec2d normal;
	double dist;
	double distInv;
public:
	CDCutLine(Vec2d n, double d):normal(n), dist(d){
		const double epsilon = 1e-10;
		if (dist < epsilon) dist = epsilon;
		distInv = 1/dist;
	}
	Vec2d GetPos() const { 
		return normal*distInv; 
	}
	friend class CDCutRing;
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
	///	�o�Εϊ� �� QuickHull �ň�ԓ����̓ʑ��p�`�̒��_�����߂�D
	static CDQHLines<CDCutLine> vtxs;	//	��ԓ����̓ʑ��p�`�̒��_

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
