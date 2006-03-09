#ifndef SPR_CDSHAPEIF_H
#define SPR_CDSHAPEIF_H
#include <Foundation/Object.h>

namespace Spr{;
/*	memo
	shape�ɊK�w�\�����������邩�ǂ����D
	Novodex ODE�Ƃ� �K�w�\���͎����Ă��Ȃ��D
	�K�w�𓮂����Ƃ��ɂ́C�d�S�� �������[�����g�����������Ȃ���΂Ȃ�Ȃ��̂ŁC
	����ȂɎg��Ȃ��Ƃ������f���Ǝv���D
	���Ȃ��Ƃ��C�V�~�����[�V�������ɊK�w�œ������͍̂���Ǝv���D

	�Ȃ̂ŁC�K�w�͎����Ȃ��Ƃ������j�ō��D

	Solid �͕����� Shape �������Ƃ��ł���D
	Shape �͊K�w�\���͎����Ȃ��D

	�K�w�\���͕������C�u�����Ƃ͕ʂɁC�V�[���O���t���C�u����������
*/

struct CDShapeIf : public NamedObjectIf{
	IF_DEF(CDShape);
};

struct CDConvexIf : public CDShapeIf{
	IF_DEF(CDConvex);
};

/**	��	*/
struct CDFaceIf{
	virtual int NIndex()=0;
	virtual int* GetIndices()=0;
protected:
	virtual ~CDFaceIf() {};
};



struct PHMaterial{
	PHMaterial(){
		mu = mu0 = e = 0.2f;
	}
	float mu;		///<	�����C���C�W��
	float mu0;		///<	�Î~���C�W��
	float e;		///<	���˕Ԃ�W��
};

struct CDShapeDesc{
	enum ShapeType{
		CONVEX,
		CONVEXMESH,
	} type;
	CDShapeDesc(ShapeType t=CONVEXMESH): type(t){}
};


/**	�ʌ`��̃��b�V���̐����p
*/
struct CDConvexMeshDesc: public CDShapeDesc{
	CDConvexMeshDesc():CDShapeDesc(CONVEXMESH){}
	std::vector<Vec3f> vertices;	///<	���_�̍��W
	PHMaterial material;			///<	�ގ�
};
/**
	�ʌ`��̃��b�V��	���_��ǉ�
*/
struct CDConvexMeshIf: public CDConvexIf{
	IF_DEF(CDConvexMesh);
	virtual CDFaceIf* GetFace(size_t i)=0;
	virtual size_t NFace()=0;
	virtual Vec3f* GetVertices()=0;
	virtual size_t NVertex()=0;
};

}

#endif
