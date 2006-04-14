#ifndef SPR_CDSHAPEIF_H
#define SPR_CDSHAPEIF_H
#include <Foundation/SprObject.h>

namespace Spr{;

/**	\addtogroup	gpShape	�`��E�ڐG����N���X	*/	
//@{

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

///	�`��̊�{�N���X
struct CDShapeIf : public NamedObjectIf{
	IF_DEF(CDShape);
	virtual void SetPose(const Posef& p)=0;
	virtual Posef GetPose() const=0;
};

///	�ʌ`��̊�{�N���X
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



///	�����V�~�����[�V�����Ɋ֌W����ގ�
struct PHMaterial{
	PHMaterial(){
		mu = mu0 = e = 0.2f;
	}
	float mu;		///<	�����C���C�W��
	float mu0;		///<	�Î~���C�W��
	float e;		///<	���˕Ԃ�W��
};

///	�`��̃f�B�X�N���v�^(��{�N���X)
struct CDShapeDesc{
	enum ShapeType{
		CONVEX,			// �ʌ`��
		CONVEXMESH,		// �ʌ`�󃁃b�V��
		SPHERE,			// ����
		BOX				// ������
	} type;
	CDShapeDesc(ShapeType t=CONVEXMESH): type(t){}
};


/**	�ʌ`��̃��b�V���̃f�B�X�N���v�^	*/
struct CDConvexMeshDesc: public CDShapeDesc{
	CDConvexMeshDesc():CDShapeDesc(CONVEXMESH){}
	std::vector<Vec3f> vertices;	///<	���_�̍��W
	PHMaterial material;			///<	�ގ�
};
	
/**	�ʌ`��̃��b�V��*/
struct CDConvexMeshIf: public CDConvexIf{
	IF_DEF(CDConvexMesh);
	virtual CDFaceIf* GetFace(size_t i)=0;
	virtual size_t NFace()=0;
	virtual Vec3f* GetVertices()=0;
	virtual size_t NVertex()=0;
};
	
/** ���̂̃f�B�X�N���v�^�@*/
struct CDSphereDesc: public CDShapeDesc{
	CDSphereDesc():CDShapeDesc(SPHERE){}
	float radius;					// ���̂̔��a
	PHMaterial material;			// �}�e���A��
};	

/** ���́@*/
struct CDSphereIf: public CDConvexIf{
	IF_DEF(CDSphere);
	virtual float GetRadius()=0;
};	

/** �����̂̃f�B�X�N���v�^ */
struct CDBoxDesc: public CDShapeDesc{
	CDBoxDesc():CDShapeDesc(BOX){}
	Vec3f boxsize;					// �����̂̃T�C�Y�i�e�ӂ̒����j
	PHMaterial material;
};	
	
/** ������ */
struct CDBoxIf: public CDConvexIf{
	IF_DEF(CDBox);
	virtual Vec3f GetBoxSize()=0;
};


//@}

}

#endif
