/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CDCUTRING_H
#define CDCUTRING_H

#include <Collision/CDQuickHull2DImp.h>

namespace Spr{;

///	�ڐG��͂Ɏg���N���X�D�ڐG�����̐؂���̐�����\���D
class CDCutLine{
public:
	Vec2d normal;
	double dist;
	double distInv;
public:
	CDCutLine(Vec2d n, double d):normal(n), dist(d){
		const double epsilon = 1e-10;
		assert(finite(n.x));
		assert(finite(n.y));
		assert(finite(d));
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
	///	�؂���̒���1�_�ƁC�؂���ʂ̍��W�n�Dex ���@��
	Posed local, localInv;
	///	�؂�����\�����钼��
	std::vector<CDCutLine> lines;
	///	�o�Εϊ� �� QuickHull �ň�ԓ����̓ʑ��p�`�̒��_�����߂�D
	static CDQHLines<CDCutLine> vtxs;	//	��ԓ����̓ʑ��p�`�̒��_

	CDCutRing(Vec3d c, Matrix3d l){
		local.Ori().FromMatrix(l);
		local.Pos() = c;
		localInv = local.Inv();
	}
	void MakeRing();
	void Print(std::ostream& os);
};

}

#endif
