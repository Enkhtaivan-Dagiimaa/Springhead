/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_CDSHAPEIF_H
#define SPR_CDSHAPEIF_H
#include <Foundation/SprObject.h>

namespace Spr{;

/**	\defgroup	gpShape	�`��E�ڐG����N���X	*/	
//@{
/**	@file SprCDShape.h
	���̂̌`��
 */	

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
	SPR_IFDEF(CDShape);

	void	SetStaticFriction(float mu0);
	float	GetStaticFriction();
	void	SetDynamicFriction(float mu);
	float	GetDynamicFriction();
	void	SetElasticity(float e);
	float	GetElasticity();
	void	SetDensity(float d);
	float	GetDensity();

	float	CalcVolume();			///< �̐ς��v�Z
	Vec3f	CalcCenterOfMass();		///< ���ʒ��S
	Matrix3f CalcMomentOfInertia();	///< �P�ʖ��x�ɂ����鎿�ʒ��S�Ɋւ��銵���s��
};

///	�ʌ`��̊�{�N���X
struct CDConvexIf : public CDShapeIf{
	SPR_IFDEF(CDConvex);
};

/**	��	*/
struct CDFaceIf: public ObjectIf{
	SPR_IFDEF(CDFace);
	int NIndex();
	int* GetIndices();
};
struct CDQuadFaceIf: public ObjectIf{
	SPR_IFDEF(CDQuadFace);
	int NIndex();
	int* GetIndices();
};


///	�����V�~�����[�V�����Ɋ֌W����ގ�
struct PHMaterial{
	PHMaterial();
	float mu;				///< �����C���C�W��
	float mu0;				///< �Î~���C�W��
	float e;				///< ���˕Ԃ�W��
	float density;			///< ���x

	float vibA;
	float vibB;
	float vibW;
};

///	�`��̃f�B�X�N���v�^(��{�N���X)
struct CDShapeDesc{
	PHMaterial material;	///<	�ގ�
};


/**	�ʌ`��̃��b�V��*/
struct CDConvexMeshIf: public CDConvexIf{
	SPR_IFDEF(CDConvexMesh);
	CDFaceIf* GetFace(size_t i);
	size_t NFace();
	Vec3f* GetVertices();
	size_t NVertex();
};
/**	�ʌ`��̃��b�V���̃f�B�X�N���v�^	*/	
struct CDConvexMeshDesc: public CDShapeDesc{
	SPR_DESCDEF(CDConvexMesh);
	CDConvexMeshDesc():CDShapeDesc(){}
	std::vector<Vec3f> vertices;	///<	���_�̍��W
};
	
/** ���́@*/
struct CDSphereIf: public CDConvexIf{
	SPR_IFDEF(CDSphere);
	float GetRadius();
};	
/** ���̂̃f�B�X�N���v�^�@*/
struct CDSphereDesc: public CDShapeDesc{
	SPR_DESCDEF(CDSphere);
	CDSphereDesc():CDShapeDesc(){
		radius = 1.0f;
	}
	float radius;					// ���̂̔��a
};	

/** �J�v�Z���@*/
struct CDCapsuleIf: public CDConvexIf{
	SPR_IFDEF(CDCapsule);
	float GetRadius();
	float GetLength();
};	
/** �J�v�Z���̃f�B�X�N���v�^�@*/
struct CDCapsuleDesc: public CDShapeDesc{
	SPR_DESCDEF(CDCapsule);
	CDCapsuleDesc():CDShapeDesc(){
		radius = 1.0f;
		length = 1.0f;
	}
	float radius;					///< �J�v�Z���̋��̔��a
	float length;					///< �J�v�Z���̒��� Z������������
};	

	
/** ������ */
struct CDBoxIf: public CDConvexIf{
	SPR_IFDEF(CDBox);
	Vec3f GetBoxSize();
	Vec3f* GetVertices();
	CDFaceIf* GetFace(size_t i);
	Vec3f SetBoxSize(Vec3f boxsize);
};
/** �����̂̃f�B�X�N���v�^ */
struct CDBoxDesc: public CDShapeDesc{
	SPR_DESCDEF(CDBox);
	CDBoxDesc():CDShapeDesc(){
		boxsize = Vec3f(1.0f, 1.0f, 1.0f);
	}
	Vec3f boxsize;					// �����̂̃T�C�Y�i�e�ӂ̒����j
};	


//@}

}

#endif
