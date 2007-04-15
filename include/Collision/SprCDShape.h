/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
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
	IF_DEF(CDShape);
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
	float mu;				///< �����C���C�W��
	float mu0;				///< �Î~���C�W��
	float e;				///< ���˕Ԃ�W��
};

///	�`��̃f�B�X�N���v�^(��{�N���X)
struct CDShapeDesc{
	PHMaterial material;	///<	�ގ�
};


/**	�ʌ`��̃��b�V��*/
struct CDConvexMeshIf: public CDConvexIf{
	IF_DEF(CDConvexMesh);
	virtual CDFaceIf* GetFace(size_t i)=0;
	virtual size_t NFace()=0;
	virtual Vec3f* GetVertices()=0;
	virtual size_t NVertex()=0;
};
/**	�ʌ`��̃��b�V���̃f�B�X�N���v�^	*/	
struct CDConvexMeshDesc: public CDShapeDesc{
	DESC_DEF_FOR_OBJECT(CDConvexMesh);
	CDConvexMeshDesc():CDShapeDesc(){}
	std::vector<Vec3f> vertices;	///<	���_�̍��W
};
	
/** ���́@*/
struct CDSphereIf: public CDConvexIf{
	IF_DEF(CDSphere);
	virtual float GetRadius()=0;
};	
/** ���̂̃f�B�X�N���v�^�@*/
struct CDSphereDesc: public CDShapeDesc{
	DESC_DEF_FOR_OBJECT(CDSphere);
	CDSphereDesc():CDShapeDesc(){
		radius = 1.0f;
	}
	float radius;					// ���̂̔��a
};	

/** �J�v�Z���@*/
struct CDCapsuleIf: public CDConvexIf{
	IF_DEF(CDCapsule);
	virtual float GetRadius()=0;
	virtual float GetLength()=0;
};	
/** �J�v�Z���̃f�B�X�N���v�^�@*/
struct CDCapsuleDesc: public CDShapeDesc{
	DESC_DEF_FOR_OBJECT(CDCapsule);
	CDCapsuleDesc():CDShapeDesc(){
		radius = 1.0f;
		length = 1.0f;
	}
	float radius;					///< �J�v�Z���̋��̔��a
	float length;					///< �J�v�Z���̒��� Z������������
};	

	
/** ������ */
struct CDBoxIf: public CDConvexIf{
	IF_DEF(CDBox);
	virtual Vec3f GetBoxSize()=0;
	virtual Vec3f* GetVertices()=0;
	virtual CDFaceIf* GetFace(size_t i)=0;
};
/** �����̂̃f�B�X�N���v�^ */
struct CDBoxDesc: public CDShapeDesc{
	DESC_DEF_FOR_OBJECT(CDBox);
	CDBoxDesc():CDShapeDesc(){
		boxsize = Vec3f(1.0f, 1.0f, 1.0f);
	}
	Vec3f boxsize;					// �����̂̃T�C�Y�i�e�ӂ̒����j
};	


//@}

}

#endif
