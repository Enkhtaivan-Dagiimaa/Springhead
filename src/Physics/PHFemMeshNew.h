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
#include "../Foundation/Object.h"
#include "PHScene.h"

namespace Spr{;
using namespace PTM;

/* �l�ʑ̂�\�����߂̃N���X�A�\���̂̐錾 */
//	���_
class FemVertex{
public:
	Vec3d pos;
	Vec3d initialPos;
	Vec3d vel;
	std::vector<int> tetIDs;
	std::vector<int> edgeIDs;
	std::vector<int> faceIDs;
	bool bUpdated;

	//For multiple FEM objects
	double centerDist;    //The distance to the closest neighbour
};
//	�l�ʑ�
class FemTet{
public:
	int vertexIDs[4];	///< ���_ID																																	  
	int faceIDs[4];		///< �\�ʂS��
	int edgeIDs[6];		///< �Ή�����ӂ�ID�B0:��01, 1:��12, 2:��20, 3:��03, 4:��13, 5:��23
	PTM::TVector<4,double> vecf[4];		//>	{f1}:vecf[0],{f2(�M����)}:vecf[1],{f3(�M�`�B)}:vecf[2],{f4(�M�t��)}:vecf[3]
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

	//added for the FEM implementation
	Vec3d centroid;  /// Face centroid
	double area;	/// Face area
	Vec3d normal;	//Face normal
	int tetraId;    //Saves the FaceId
};
//	��
struct FemEdge{
	int vertexIDs[2];
	bool operator < (const FemEdge& e2); 	///< ���_ID�Ŕ�r
	bool operator == (const FemEdge& e2);	///< ���_ID�Ŕ�r
	FemEdge(int v1=-1, int v2=-1);
};

//Data structures for the multiple FEM implementation
//This structure is used for the KDTree Search
struct data {
    int faceId;
    Vec3d cpoint;
	Vec3d median;
	Vec3d fnormal;
	int vtxIds[3];
	int tetraId;
    data() {}
};

//class to define the KDTree data structure
class KDVertex{
public:
	KDVertex *left;
	KDVertex *right;
	data val;

	KDVertex() {
		left = NULL;
		right = NULL;
	}
};

//This structure is used to match Tetrahedra and vertex in a collision
struct FemFVPair {
	int vertexId;
	int faceId;
	int tetraId;
	int femIndex;
	Vec3d projection;
	double dist;
	Vec3d initialPos[2];

	Vec3d debugVertex; //DEBUG
	int debugMaster;   //DEBUG
	int debugSlave;    //DEBUG

	FemFVPair () {}
};

class PHFemBase;
typedef std::vector< UTRef< PHFemBase > > PHFemBases;
class PHFemVibration;
class PHFemThermo;
class PHFemPorousWOMove;
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
	UTRef< PHFemThermo > femThermo;
	UTRef< PHFemPorousWOMove > femPorousWOMove;

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
	/// PHFemThermoIf��Ԃ�
	PHFemThermoIf* GetPHFemThermo();
	/// PHFemPorousWOMoveIf��Ԃ�
	PHFemPorousWOMoveIf* GetPHFemPorousWOMove();

	/// ���_�̑�����Ԃ�
	int NVertices();
	///	�ʂ̑�����Ԃ�
	int NFaces();
	/// �l�ʑ̂̑�����Ԃ�
	int NTets();

	///////////////////////////////////////////////////////////////////////////////////////////
	//* ���_�Ɋւ���֐� */
	/// ���_�̏����ʒu���擾����i���[�J�����W�n�j
	Vec3d GetVertexInitalPositionL(int vtxId);
	/// ���_�̈ʒu���擾����i���[�J�����W�n�j
	Vec3d GetVertexPositionL(int vtxId);
	/// ���_�̕ψʂ��擾����i���[�J�����W�n�j
	Vec3d GetVertexDisplacementL(int vtxId);
	/// ���_�̑��x���擾����i���[�J�����W�n�j
	Vec3d GetVertexVelocityL(int vtxId);
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
	unsigned GetnSurfaceFace(){return nSurfaceFace;}

	//For multiple object implementation VARIABLES
	bool *contactVector;	//saves the contact configuration of the scene
	int femIndex;			//stores the fem loading Index
	KDVertex* root;         //stores the KDTree root  
	RingBuffer<Vec3d>* ringVel;    //ring buffer to save the instant vel 
	bool spheric;			//indicates the shape of the model (from SPR file)


	//For DEBUG FEM multiple collision
	Vec3d debugFoundPoint;  //debug
	Vec3d debugSearchPoint;  //debug
	Vec3d debugClosestPoint;  //debug
	Vec3d debugContactFace; //debug
	Vec3d debugPivotPoint;  //debug
	std::vector<int> *debugVertexInside;  //debug
	std::vector<int> *debugFacesInside;  //debug
	std::vector<int> *debugFixedPoints;  //debug
	std::vector<FemFVPair> *debugPairs;   //debug

};

}

#endif
