/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CDRoundCone_H
#define CDRoundCone_H

#include <SprCollision.h>
#include <Foundation/Object.h>
#include "CDConvex.h"

namespace Spr{;
	
/// �J�v�Z��
class CDRoundCone: public CDConvex, public CDRoundConeDesc{
public:
	SPR_OBJECTDEF(CDRoundCone);
	SPR_DECLMEMBEROF_CDRoundConeDesc;

	CDRoundCone();
	//CDRoundCone(float r = 0.0f, Vec3f c = Vec3f()):radius(r), center(c){}
	//CDRoundCone(Vec3f c = Vec3f(), float r = 0.0f):center(c), radius(r){}
	CDRoundCone(const CDRoundConeDesc& desc);
	
	virtual bool IsInside(const Vec3f& p);
	virtual float CalcVolume();
	
	///	�T�|�[�g�|�C���g�����߂�D
	virtual Vec3f Support(const Vec3f& p) const;
	///	�؂�������߂�D�ڐG��͂Ɏg���D
	/// �������A���̂Ɋւ��ẮA�؂���͋��߂Ȃ��B�ڐG��͎��ɂ͍ŋߖT�̂P�_��������΂����B
	/// ���̂Ɋւ���FindCutRing()���Ăяo���ꂽ�ꍇ�ɂ́Aassertion����������B
	virtual bool FindCutRing(CDCutRing& r, const Posed& toW);
	/// �\�ʏ�̓_p�ɂ�����ȗ����a�����߂�
	virtual double CurvatureRadius(Vec3d p);
	///< �\�ʏ�̓_p�ɂ�����@��
	virtual Vec3d Normal(Vec3d p);
	
	Vec2f GetRadius();
	float GetLength();
	void SetRadius(Vec2f r);
	void SetLength(float l);
	//length��ς��Ȃ��ŃJ�v�Z���̑��������ύX����
	void SetWidth(Vec2f r);
};

	
}	//	namespace Spr

#endif	// CDRoundCone_H
