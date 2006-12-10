/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CDCONVEX_H
#define CDCONVEX_H

#include <SprCollision.h>
#include <Foundation/Object.h>
#include "CDShape.h"
#include "CDCutRing.h"

namespace Spr{;
class CDConvex : public CDShape{
public:
	OBJECTDEF_ABST(CDConvex, CDShape);
	///	�T�|�[�g�|�C���g(�����x�N�g��p�Ƃ̓��ς��ő�̒��_)�����߂�D
	virtual Vec3f Support(const Vec3f& p) const =0;
	///	�؂�������߂�D�ڐG��͂Ɏg���D
	virtual bool FindCutRing(CDCutRing& r, const Posed& toW) =0;
	///	�o�E���f�B���O�{�b�N�X�����߂�D
	virtual void CalcBBox(Vec3f& bbmin, Vec3f& bbmax, const Posed& pose);
	///	���S�i��̐^�񒆁j�̈ʒu��Ԃ�
	virtual Vec3f GetCenter(){ return Vec3f(); }
};

}	//	namespace Spr
#endif
