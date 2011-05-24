/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CDCONVEXMESH_H
#define CDCONVEXMESH_H

#include <SprCollision.h>
#include <vector>
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
class CDFace:public Object{
public:
	SPR_OBJECTDEF(CDFace);
	int vtxs[3];	///< �ʂ̒��_ID
	Vec3f	normal;

	/// CDFace�̖ʂ̃C���f�b�N�X��
	virtual int NIndex(){ return 3; }
	/// CDFace�̃C���f�b�N�X�z����擾
	virtual int* GetIndices(){ return vtxs; }
};

class CDFaces:public std::vector<CDFace>{
};

///	�ʑ��ʑ�
class CDConvexMesh : public CDConvex{
protected:
	/// �S���_�̕���
	Vec3f average;
public:
	SPR_OBJECTDEF(CDConvexMesh);
	//	Desc�̃����o�ASPR_DECLMEMBEROF_CDConvexMeshDesc �͎g��Ȃ��B�����GetDesc, SetDesc, GetDescSize���g��

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

	///	���_����ʂ�ڑ����𐶐�����D
	void CalcFace();

	///
	virtual bool IsInside(const Vec3f& p);
	
	///	�T�|�[�g�|�C���g�����߂�D
	int Support(Vec3f& w, const Vec3f& v) const;
	
	///	�؂�������߂�D�ڐG��͂Ɏg���D
	virtual bool FindCutRing(CDCutRing& r, const Posed& toW);
	///	�w��̒��_ vtx �ׂ̗̒��_�ԍ���Ԃ�
	virtual std::vector<int>& FindNeighbors(int vtx);
	///	���_�o�b�t�@��Ԃ��B
	virtual Vec3f* GetBase(){return &*base.begin();}	

	CDFaceIf* GetFace(size_t i);
	size_t NFace();
	Vec3f* GetVertices();
	size_t NVertex();

	///	�f�X�N���v�^CDConvexMeshDesc�̓ǂݏ���	
	virtual bool GetDesc(void *desc) const;
	virtual void SetDesc(const void* desc);
	virtual size_t GetDescSize() const { return sizeof(CDConvexMeshDesc); }

	virtual int LineIntersect(const Vec3f& origin, const Vec3f& dir, Vec3f* result, float* offset);
	virtual void Print(std::ostream& os) const;

protected:
	///	���ꕽ�ʏ�Őڑ�����Ă���3�p�`���}�[�W����
	void MergeFace();
	/// �ʂ̖@�����v�Z(Inside�p)
	void CalcFaceNormals();

	///	���ύ��W���v�Z����B
	void CalcAverage();

};

}	//	namespace Spr
#endif
