/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CDCONVEXMESH_H
#define CDCONVEXMESH_H

#include <SprCollision.h>
#include <Foundation/Object.h>
#include "CDConvex.h"

namespace Spr{;

class CDVertexIDs:public std::vector<int>{
public:
	int FindPos(int id) const;
};

/**	�ʑ��ʑ̖̂ʂ�\���N���X�DCDConvexMesh �����L�D
	CDContactAnalysis �Ŏg����D
*/
class CDFace: public CDFaceIf{
public:
	int vtxs[3];	///< �ʂ̒��_ID

	/// CDFace�̖ʂ̃C���f�b�N�X��
	virtual int NIndex(){ return 3; }
	/// CDFace�̃C���f�b�N�X�z����擾
	virtual int* GetIndices(){ return vtxs; }
};

class CDFaces:public std::vector<CDFace>{
};

///	�ʑ��ʑ�
class CDConvexMesh : public InheritCDShape<CDConvexMeshIf, CDConvex>{
public:
	OBJECT_DEF(CDConvexMesh);

	///	�T���J�n���_�ԍ�
	mutable int curPos;
	///	���_�̍��W(���[�J�����W�n)
	std::vector<Vec3f> base;

	///	���_�ׂ̗̒��_�̈ʒu���������z��
	std::vector< std::vector<int> > neighbor;
	///	��(3�p�` 0..nPlanes-1 ���Ɨ��ȖʁC����ȍ~��MargeFace�ō폜����铯�ꕽ�ʏ�̖�)
	CDFaces faces;
	///	��(3�p�`�̂����CMergeFace()�Ŏc������)
	int nPlanes;

public:
	CDConvexMesh();
	CDConvexMesh(const CDConvexMeshDesc& desc);

	///	ShapeType
	virtual int ShapeType(){ return CDShapeDesc::CONVEXMESH; }

	///	���_����ʂ�ڑ����𐶐�����D
	void CalcFace();
	///	���ꕽ�ʏ�Őڑ�����Ă���3�p�`���}�[�W����
	void MergeFace();
	
	///	�T�|�[�g�|�C���g�����߂�D
	virtual Vec3f Support(const Vec3f& p) const;
	
	///	�؂�������߂�D�ڐG��͂Ɏg���D
	virtual bool FindCutRing(CDCutRing& r, const Posed& toW);

	CDFaceIf* GetFace(size_t i);
	size_t NFace();
	Vec3f* GetVertices();
	size_t NVertex();

	virtual bool GetDesc(void *desc) const ;
};

}	//	namespace Spr
#endif
