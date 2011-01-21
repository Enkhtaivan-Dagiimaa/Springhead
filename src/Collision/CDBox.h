/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CDBOX_H
#define CDBOX_H

#include <SprCollision.h>
#include <Foundation/Object.h>
#include "CDConvex.h"

namespace Spr{;

/**	�����̖̂�(�l�p�`)��\���N���X�DCDBox�����L�D
	CDContactAnalysis �Ŏg����D
*/
class CDQuadFace: public Object{
public:
	SPR_OBJECTDEF(CDQuadFace);
	int vtxs[4];	///< �ʂ̒��_ID
	Vec3f normal;	///< �ʂ̖@��

	/// CDQuadFace�̖ʂ̃C���f�b�N�X��
	virtual int NIndex(){ return 4; }
	/// CDQuadFace�C���f�b�N�X�z����擾
	virtual int* GetIndices(){ return vtxs; }
};

class CDQuadFaces:public std::vector<CDQuadFace>{
};

/// ������
class CDBox: public CDConvex{
public:
	mutable int curPos;
	SPR_OBJECTDEF(CDBox);
	SPR_DECLMEMBEROF_CDBoxDesc;

	std::vector<Vec3f> base;				///< ���_�̍��W(���[�J�����W�n)
	static CDQuadFaces qfaces;				///< �ʁi�l�p�`:quadrangular face�j
	static std::vector<int> neighbor[8];	///< ���_�ׂ̗̓_
	
	CDBox();
	CDBox(const CDBoxDesc& desc);
	
	virtual bool IsInside(const Vec3f& p);
	virtual float CalcVolume();
	virtual Matrix3f CalcMomentOfInertia();
	
	///	�T�|�[�g�|�C���g�����߂�D
	int Support(Vec3f& w, const Vec3f& v) const ;
	///	�؂�������߂�D�ڐG��͂Ɏg���D
	/// �������A���̂Ɋւ��ẮA�؂���͋��߂Ȃ��B�ڐG��͎��ɂ͍ŋߖT�̂P�_��������΂����B
	/// ���̂Ɋւ���FindCutRing()���Ăяo���ꂽ�ꍇ�ɂ́Aassertion����������B
	virtual bool FindCutRing(CDCutRing& r, const Posed& toW);
	///	�w��̒��_ vtx �ׂ̗̒��_�ԍ���Ԃ�
	virtual std::vector<int>& FindNeighbors(int vtx);
	///	���_�o�b�t�@��Ԃ��B
	virtual Vec3f* GetBase(){return &*base.begin();}	
	
	/// �����̂̃T�C�Y���擾
	Vec3f GetBoxSize();
	Vec3f* GetVertices();
	CDFaceIf* GetFace(size_t i);

	/// �����̂̃T�C�Y��ݒ�
	Vec3f SetBoxSize(Vec3f boxSize);

	/// �������̍Čv�Z
	void Recalc();

	virtual int LineIntersect(const Vec3f& origin, const Vec3f& dir, Vec3f* result, float* offset);
};
	

}	//	namespace Spr

#endif	// CDBOX_H
