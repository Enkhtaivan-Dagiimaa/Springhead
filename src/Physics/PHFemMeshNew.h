/*
 *  Copyright (c) 2003-2012, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

#ifndef PH_FEMMESH_NEW_H
#define PH_FEMMESH_NEW_H

#include <Physics/SprPHFemMeshNew.h>
#include <Foundation/Object.h>
#include <Physics/PHScene.h>

namespace Spr{;

/// �v�Z���W���[���̋��ʕ���
class PHFem: public SceneObject{
public:
	SPR_OBJECTDEF_ABST(PHFem);
	SPR_DECLMEMBEROF_PHFemDesc;

	PHFem(){}

};

/* �l�ʑ̂�\�����߂̃N���X�A�\���̂̐錾 */
//	���_
class FemVertex{
public:
	Vec3d pos;
	std::vector<int> tets;
	std::vector<int> edges;
	std::vector<int> faces;
	bool vtxDeformed;		///< �l�ʑ̂̕ό`�ł��̐ߓ_�����[�J�����W��ňړ��������ǂ���
	double disFromOrigin;	///< x-z���ʂł̃��[�J�����W�̌��_����̋���
};
//	�l�ʑ�
class Tet{
public:
	int vertices[4];	///< ���_ID																																	  
	int faces[4];		///< �\�ʂS��
	int edges[6];		///< �Ή�����ӂ�ID�B0:��01, 1:��12, 2:��20, 3:��03, 4:��13, 5:��23
	double volume;		///< �ϕ��v�Z�ŗp���邽�߂̑̐�
	PTM::TMatrixRow<4,4,double> matk[3];	///<	
	PTM::TVector<4,double> vecf[4];			///<	{f1}:vecf[0],{f2}:vecf[1],{f3}:vecf[2],...
	int& edge(int i, int j);
};

// ��
class FemFace{
	int sorted[3];		///< ��r���邽�߂́A�\�[�g�ς݂̒��_id�BUpdate()�ōX�V�B
public:
	int vertices[3];	///<���_ID�B���ԂŖʂ̕\����\���B
	void Update();
	bool operator < (const FemFace& f2);	///< ���_ID�Ŕ�r
	bool operator == (const FemFace& f2);	///< ���_ID�Ŕ�r
	double area;						///< �l�ʑ̂̊e�ʂ̖ʐ�
};
//	��
struct FemEdge{
	int vertices[2];
	bool operator < (const FemEdge& e2); 	///< ���_ID�Ŕ�r
	bool operator == (const FemEdge& e2);	///< ���_ID�Ŕ�r
	double k;	///< �S�̍����s��K�̗v�f
	double c;	///<
	double b;	///< �K�E�X�U�C�f���v�Z�ŗp����萔b
	FemEdge(int v1=-1, int v2=-1);
};


class PHFemVibration;
class PHFemMeshNew : public SceneObject{//, public PHFemMeshNewDesc{
public:
	/* PHFemMeshNew�̃����o�ϐ��A�֐��Q */
	SPR_OBJECTDEF(PHFemMeshNew);
	//SPR_DECLMEMBEROF_PHFemMeshNewDesc;
protected:
	///	��{���(�������Ƀf�X�N���v�^����^��������)
	PHSolidIf* solid;					///< �֘A�Â����Ă��鍄��
public:
	std::vector<FemVertex> vertices;	///< ���_
	std::vector<Tet> tets;				///< �l�ʑ�
	
	/// �ǉ����	��{��񂩂�SetDesc()���v�Z���ċ��߂�B
	std::vector<int> surfaceVertices;	///< ���̕\�ʂ̒��_��ID
	std::vector<FemFace> faces;	///< ��
	unsigned nSurfaceFace;		///< ���̕\�ʂɑ��݂���ʂ̐��B�\�ʁFfaces[0],..,faces[nSurfaceFace-1]�A����:faces[nSurfaceFace],..,faces[faces.size()]
	std::vector<FemEdge> edges;	///< ��
	unsigned nSurfaceEdge;		///< ���̕\�ʂɑ��݂���ӁB�\��:edges[0],..,edges[nSurfaceEdge-1]�A����:edges[nSurfaceEdge],..,edges[edges.size()]

	/// �v�Z���W���[��
	//PHFems** fems;
	UTRef< PHFemVibration > femVibration;
	//PHFemThermo*	thermo;

	///	�f�X�N���v�^
	PHFemMeshNew(const PHFemMeshNewDesc& desc = PHFemMeshNewDesc(), SceneIf* s=NULL);
	///	�f�X�N���v�^�̃T�C�Y
	virtual size_t GetDescSize() const ;
	///	�f�X�N���v�^�̓ǂݏo��(�R�s�[��)
	virtual bool GetDesc(void* p) const ;
	///	�f�X�N���v�^�̐ݒ�B�����ŁA���_���W�Ǝl�ʑ̂�4���_��ID�̏�񂩂�A�ʂ�ӂɂ��Ă̏����v�Z���Ă����B
	virtual void SetDesc(const void* p);
	/// �q�I�u�W�F�N�g�̒ǉ�
	virtual bool AddChildObject(ObjectIf* o);

	///	������dt�i�߂�BPHFemEngine���Ăяo���B
	virtual void Step(double dt);
	/// ���̂��֘A�Â���
	void SetPHSolid(PHSolidIf* s);
	/// �֘A�t������Ă��鍄�̂�Ԃ�
	PHSolidIf* GetPHSolid();
	///	�ʂ̑�����Ԃ�
	unsigned GetNFace();
	///	Face�ӂ̗��[�_�̍��W��Ԃ�?
	std::vector<Vec3d> GetFaceEdgeVtx(unsigned id);
	//	Face�ӂ̗��[�_�̍��W��Ԃ�?
	Vec3d GetFaceEdgeVtx(unsigned id, unsigned vtx);
	/// �l�ʑ̂̌v�Z(�Ώۂɂ�炸�ɕK�v�ɂȂ�`��֐���XX���v�Z����֐�)
	void UpdateJacobian();
};

}

#endif
