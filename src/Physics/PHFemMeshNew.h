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
#include "Foundation/Object.h"
#include "PHScene.h"

namespace Spr{;
using namespace PTM;

/* �l�ʑ̂�\�����߂̃N���X�A�\���̂̐錾 */
//	���_
class FemVertex{
public:
	Vec3d pos;
	Vec3d initialPos;
	std::vector<int> tetIDs;
	std::vector<int> edgeIDs;
	std::vector<int> faceIDs;
};
//	�l�ʑ�
class FemTet{
public:
	int vertexIDs[4];	///< ���_ID																																	  
	int faceIDs[4];		///< �\�ʂS��
	int edgeIDs[6];		///< �Ή�����ӂ�ID�B0:��01, 1:��12, 2:��20, 3:��03, 4:��13, 5:��23
	PTM::TVector<4,double> vecf[4];			///<	{f1}:vecf[0],{f2}:vecf[1],{f3}:vecf[2],...
	int& edge(int i, int j);
};

// ��
class FemFace{
	int sorted[3];		///< ��r���邽�߂́A�\�[�g�ς݂̒��_id�BUpdate()�ōX�V�B
public:
	int vertexIDs[3];	///<���_ID�B���ԂŖʂ̕\����\���B
	//�i*�\���猩�Ď��v���B�������S�̌`��nSurfaceFace�̕\�ʂ̂ݐ������B������2�̎l�ʑ̂����L���邽�߁A�\���������Ȃ��B�j
	void Update();
	bool operator < (const FemFace& f2);	///< ���_ID�Ŕ�r
	bool operator == (const FemFace& f2);	///< ���_ID�Ŕ�r
};
//	��
struct FemEdge{
	int vertexIDs[2];
	bool operator < (const FemEdge& e2); 	///< ���_ID�Ŕ�r
	bool operator == (const FemEdge& e2);	///< ���_ID�Ŕ�r
	FemEdge(int v1=-1, int v2=-1);
};

class PHFemBase;
typedef std::vector< UTRef< PHFemBase > > PHFemBases;
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
	std::vector<FemTet> tets;			///< �l�ʑ�
	
	/// �ǉ����	��{��񂩂�SetDesc()���v�Z���ċ��߂�B
	std::vector<int> surfaceVertices;	///< ���̕\�ʂ̒��_��ID
	std::vector<FemFace> faces;	///< ��
	unsigned nSurfaceFace;		///< ���̕\�ʂɑ��݂���ʂ̐��B�\�ʁFfaces[0],..,faces[nSurfaceFace-1]�A����:faces[nSurfaceFace],..,faces[faces.size()]
	std::vector<FemEdge> edges;	///< ��
	unsigned nSurfaceEdge;		///< ���̕\�ʂɑ��݂���ӁB�\��:edges[0],..,edges[nSurfaceEdge-1]�A����:edges[nSurfaceEdge],..,edges[edges.size()]

	/// �v�Z���W���[��
	PHFemBases femBases;
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

	/// ������
	virtual void Init();
	///	������dt�i�߂�BPHFemEngine���Ăяo���B
	virtual void Step(double dt);
	/// ���̂��֘A�Â���
	void SetPHSolid(PHSolidIf* s);
	/// �֘A�t������Ă��鍄�̂�Ԃ�
	PHSolidIf* GetPHSolid();
	/// PHFemVibrationIf��Ԃ�
	PHFemVibrationIf* GetPHFemVibration();

	/// ���_�̑�����Ԃ�
	int NVertices();
	///	�ʂ̑�����Ԃ�
	int NFaces();

	///////////////////////////////////////////////////////////////////////////////////////////
	//* ���_�Ɋւ���֐� */
	/// ���_�̏����ʒu���擾����i���[�J�����W�n�j
	Vec3d GetVertexInitalPositionL(int vtxId);
	/// ���_�̈ʒu���擾����i���[�J�����W�n�j
	Vec3d GetVertexPositionL(int vtxId);
	/// ���_�̕ψʂ��擾����i���[�J�����W�n�j
	Vec3d GetVertexDisplacementL(int vtxId);
	/// ���_�ɕψʂ�^����i���[���h���W�n�j
	bool AddVertexDisplacementW(int vtxId, Vec3d disW);
	/// ���_�ɕψʂ�^����i���[�J�����W�n�j
	bool AddVertexDisplacementL(int vtxId, Vec3d disL);
	/// ���_�̈ʒu���w�肷��i���[���h���W�n�j
	bool SetVertexPositionW(int vtxId, Vec3d posW);
	/// ���_�̈ʒu���w�肷��i���[�J�����W�n�j
	bool SetVertexPositionL(int vtxId, Vec3d posL);

	///////////////////////////////////////////////////////////////////////////////////////////
	//* �l�ʑ̂Ɋւ���֐� */
	/// �l�ʑ̂̌v�Z(�Ώۂɂ�炸�ɕK�v�ɂȂ�`��֐���XX���v�Z����֐�)

	/// �l�ʑ̂̑̐ς�Ԃ�
	double CompTetVolume(const Vec3d pos[4]);
	double CompTetVolume(const int& tetID, const bool& bDeform);

	/// �`��֐��̌W����Ԃ�
	/*
		(���L��a, b, c, d�̍s��j
		|N0|			|a0 b0 c0 d0||1|
		|N1|=	1/6V *	|a1 b1 c1 d1||x|
		|N2|			|a2 b2 c2 d2||y|
		|N3|			|a3 b3 c3 d3||z|
	*/
	TMatrixRow< 4, 4, double > CompTetShapeFunctionCoeff(Vec3d pos[4]);
	TMatrixRow< 4, 4, double > CompTetShapeFunctionCoeff(const int& tetId, const bool& bDeform);

	/// �l�ʑ̓��̂���_�ɂ�����`��֐��l��Ԃ�
	/// (�l�ʑ̊O�̈ʒu���w�肷��ƕ��l���ł��false��Ԃ��j
	bool CompTetShapeFunctionValue(const TMatrixRow< 4, 4, double >& sf, const double& vol, const Vec3d& posL, Vec4d& value);
	bool CompTetShapeFunctionValue(const int& tetId, const Vec3d& posL, Vec4d& value, const bool& bDeform);

	/// �ʂ���l�ʑ̂�T��
	int FindTetFromFace(int faceId);

	///////////////////////////////////////////////////////////////////////////////////////////
	//* �ʂɊւ���֐� */
	///	Face�ӂ̗��[�_�̍��W��Ԃ�?
	std::vector<Vec3d> GetFaceEdgeVtx(unsigned id);
	///	Face�ӂ̗��[�_�̍��W��Ԃ�?
	Vec3d GetFaceEdgeVtx(unsigned id, unsigned vtx);

	/// �ʐς�Ԃ�
	double CompFaceArea(const Vec3d pos[3]);		// �C�ӂ̒��_
	double CompFaceArea(const int& faceId, const bool& bDeform);

	/// �ʂ̖@����Ԃ�
	Vec3d CompFaceNormal(const Vec3d pos[3]);		// �C�ӂ̒��_
	Vec3d CompFaceNormal(const int& faceId, const bool& bDeform);

};

}

#endif
