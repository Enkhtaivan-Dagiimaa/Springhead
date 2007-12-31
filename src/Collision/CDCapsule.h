/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CDCapsule_H
#define CDCapsule_H

#include <SprCollision.h>
#include <Foundation/Object.h>
#include "CDConvex.h"

namespace Spr{;
	
/// ����
class CDCapsule: public CDConvex, public CDCapsuleIfInit, CDCapsuleDesc{
public:
	OBJECTDEF(CDCapsule, CDConvex);

	CDCapsule();
	//CDCapsule(float r = 0.0f, Vec3f c = Vec3f()):radius(r), center(c){}
	//CDCapsule(Vec3f c = Vec3f(), float r = 0.0f):center(c), radius(r){}
	CDCapsule(const CDCapsuleDesc& desc);
	
	///	�T�|�[�g�|�C���g�����߂�D
	virtual Vec3f Support(const Vec3f& p) const;
	///	�؂�������߂�D�ڐG��͂Ɏg���D
	/// �������A���̂Ɋւ��ẮA�؂���͋��߂Ȃ��B�ڐG��͎��ɂ͍ŋߖT�̂P�_��������΂����B
	/// ���̂Ɋւ���FindCutRing()���Ăяo���ꂽ�ꍇ�ɂ́Aassertion����������B
	virtual bool FindCutRing(CDCutRing& r, const Posed& toW);
	
	float GetRadius();
	float GetLength();
};

	
}	//	namespace Spr

#endif	// CDCapsule_H
