/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
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
class CDQuadFace: public CDFaceIf{
public:
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
class CDBox: public CDConvex, public CDBoxIfInit{
public:
	mutable int curPos;
	OBJECTDEF(CDBox, CDConvex);
	
	Vec3f boxsize;				/// �����̂̃T�C�Y�i�e�ӂ̒����j
	CDQuadFaces qfaces;			/// �ʁi�l�p�`:quadrangular face�j
	std::vector<Vec3f> base;	/// ���_�̍��W(���[�J�����W�n)
	
	CDBox();
	CDBox(const CDBoxDesc& desc);
	///	�T�|�[�g�|�C���g�����߂�D
	virtual Vec3f Support(const Vec3f& p) const;
	///	�؂�������߂�D�ڐG��͂Ɏg���D
	/// �������A���̂Ɋւ��ẮA�؂���͋��߂Ȃ��B�ڐG��͎��ɂ͍ŋߖT�̂P�_��������΂����B
	/// ���̂Ɋւ���FindCutRing()���Ăяo���ꂽ�ꍇ�ɂ́Aassertion����������B
	virtual bool FindCutRing(CDCutRing& r, const Posed& toW);
	
	/// �����̂̃T�C�Y���擾
	Vec3f GetBoxSize();
	Vec3f* GetVertices();
	CDFaceIf* GetFace(size_t i);
};
	

}	//	namespace Spr

#endif	// CDBOX_H
