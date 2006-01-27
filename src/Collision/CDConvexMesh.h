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

	///
	virtual int GetNIndices(){ return 3; }
	///
	virtual int* GetIndices(){ return vtxs; }
};

class CDFaces:public std::vector<CDFace>{
};

class CDConvexMesh : public CDConvexMeshIf, public CDConvex{
public:
	OBJECTDEF(CDConvexMesh);
	BASEIMP_CDCONVEX(CDConvex);

	///	�T���J�n���_�ԍ�
	mutable int curPos;
	///	���̓ʑ��ʑ̂��\�����钸�_�����Dpos�͂��̔z����̈ʒu��\���D
	CDVertexIDs vtxIDs;
	///	���_�̍��W(���[�J�����W�n)
	std::vector<Vec3f> base;

	///	���_�ׂ̗̒��_�̈ʒu���������z��
	std::vector< std::vector<int> > neighbor;
	///	��(3�p�` 0..nPlanes-1 ���Ɨ��ȖʁC����ȍ~��MargeFace�ō폜����铯�ꕽ�ʏ�̖�)
	CDFaces faces;
	///	��(3�p�`�̂����CMergeFace()�Ŏc������)
	int nPlanes;

	CDConvexMesh();
	CDConvexMesh(const CDConvexMeshDesc& desc);

	///	ShapeType
	virtual int ShapeType(){ return CDShapeDesc::CONVEXMESH; }
	///	���_�̍��W��Ԃ��D
	Vec3f Vertex(int pos) const { return base[pos]; }

	///	���_����ʂ�ڑ����𐶐�����D
	void CalcFace();
	///	���ꕽ�ʏ�Őڑ�����Ă���3�p�`���}�[�W����
	void MergeFace();
	
	///	�ʂ������Ă��邩�ǂ������ׂ�
/*	bool HasFace(CDFace* face){
		return &*faces.begin() <= face && face <= &*faces.end();
	}
*/	///	�T�|�[�g�|�C���g�����߂�D
	virtual Vec3f Support(const Vec3f& p) const;
private:
	bool VertexNear(int v1, int v2) const;

	virtual void Print(std::ostream& os) const;

	CDFaceIf* GetFace(size_t i);
	size_t GetNFaces();
	Vec3f* GetVertices();
	size_t GetNVertices();
};

}	//	namespace Spr
#endif
