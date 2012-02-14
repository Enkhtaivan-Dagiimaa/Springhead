/*
 *  Copyright (c) 2003-2011, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PHFEMMESH_H
#define PHFEMMESH_H

#include <Physics/SprPHFemMesh.h>
#include <Foundation/Object.h>
#include <Physics/PHScene.h>

namespace Spr{;



/**	�L���v�f�@�V�~�����[�V�����̂��߂̃��b�V��(4�ʑ̃��b�V��)

	�L���v�f�@�ł́A�ӂ��A���_�E�l�ʑ̂���Q�Ƃł��邱�Ƃ��K�v�ɂȂ�B
	�L���v�f�@�ł́A�v�f�i���l�ʑ́j�̑̐ς�ʐς̐ϕ�����A�e�m�[�h�̕����ʂ����߂�B
	�e�m�[�h(���_)�͕����̎l�ʑ̂ŋ��L����Ă���̂ŁA���_�Ⓒ�_�ƒ��_(����)�́A
	�����l�ʑ̂̌W���������������̂������ƂɂȂ�B

	�ׂ荇�����_�ƒ��_(=��)�̊ԂɌW��������B
	   		    i�� j��
	+------------------+	        
	|		     x     |
	|                  |   �W���s��A�͍��̂悤�ɂȂ�B �v�f�@ii, jj�i�Ίp�����j �� ij, ji�i��Ίp�����j �ɒl������(i,j�͒��_�Aij�͕�)					//(ii,jj)�����_�Aij��ji...etc�͕�
i�s	|         x  d  o  | 
	|                  |
j�s	| 		     o  d  |�@
	|                  |
	+------------------+
	
			  
	Ax + b = 0,  (D-F)x + b = 0 , x = D^{-1} F x -  D^{-1} b, x = D^{-1} F x - b'
	x_i =   (F_i * x)/D_ii - b_i/D_ii ... (1)
	i->j, j->i �œ����W��(F_ij, F_ji)���K�v�����A�v�Z�͏ォ��s���̂ŁA��������W���������Ȃ��ƂȂ�Ȃ��B
	
	1. �S�l�ʑ̂ɂ��ČW�������߁A�ӂɌW�����L�^���Ă���
    2. �K�E�X�U�C�f���̌v�Z���ォ��P�s���A��(1)�̌v�Z������B
	�@ ���̎��AF_ij=�ӂ̌W�� ���K�v�Ȃ̂ŁA���_i�����ix���Q�Ƃ������Ȃ�B
	
	�ӂ̌W��F_ij���v�Z���邽�߂ɂ́A��ij���܂ގl�ʑ̂̌W���̘a�����߂�K�v������B
	�W���̌v�Z�͎l�ʑ̒P�ʂōs���̂ŁA�l�ʑ̂���ӂւ̎Q�Ƃ��K�v�B
	
	���Ӎ\���� Edge ���K�v
**/
class PHFemMesh: public SceneObject{
public:
	//	���_
	struct FemVertex{
		Vec3d pos;
		std::vector<int> tets;
		std::vector<int> edges;
		std::vector<int> faces;
		FemVertex();
		double temp;		//	���x
		double k;			//	�S�̍����s��K�̗v�f�@�v�Z�ɗ��p
		double c;			//	����
		double Tc;			//	�ߓ_���̗͂��̉��x
		double heatTransRatio;		//	���̐ߓ_�ł̔M�`�B��
		bool vtxDeformed;		//	�l�ʑ̂̕ό`�ł��̐ߓ_�����[�J�����W��ňړ��������ǂ���
		double thermalEmissivity;	///	�M���˗��@�ߓ_�ł�
		double disFromOrigin;		//>	x-z���ʂł̃��[�J�����W�̌��_����̋���
		double heatFluxValue;		//>	�M�����l
	};
	//	�l�ʑ�
	struct Tet{
		int vertices[4];	//	���_ID																																	  
		int faces[4];		//	�\�ʂS��
		int edges[6];		//	�Ή�����ӂ�ID�B0:��01, 1:��12, 2:��20, 3:��03, 4:��13, 5:��23
		int& edge(int i, int j);
		double volume;		//	�ϕ��v�Z�ŗp���邽�߂̑̐�
		//matk1,matk2 -> matk[3] �ɓ����\��
		PTM::TMatrixRow<4,4,double> matk[3];	//>	
		//PTM::TMatrixRow<4,4,double> matk1;
		//PTM::TMatrixRow<4,4,double> matk2;
		PTM::TVector<4,double> vecf[4];			//>	
	};
	//	�l�ʑ̖̂ʁB
	class Face{
		///	��r���邽�߂́A�\�[�g�ς݂̒��_id�BUpdate()�ōX�V�B
		int sorted[3];
	public:
		///	���_ID�B���ԂŖʂ̕\����\���B
		int vertices[3];
		///	���_����߂�����vertices[3]����בւ������_ID
		int ascendVtx[3];	///	���MStep�Ŏg���H�g��Ȃ��H
		void Update();
		///	���_ID�Ŕ�r
		bool operator < (const Face& f2);
		///	���_ID�Ŕ�r
		bool operator == (const Face& f2);
		//�s��v�Z�ɗp���邽�߂̖ʐ�
		double area;				///	�l�ʑ̂̊e�ʂ̖ʐ�
		double heatTransRatio;		///	���̖ʂɂ�����M�`�B��		///	�\���ߓ_�̔M�`�B���̑������ς�		///	���ׂĂ̊֐��ŁA���̒l���X�V�ł��Ă��Ȃ��̂ŁA�M�p�ł��Ȃ��B
		bool alphaUpdated;			///	�����钸�_�̔M�`�B�����X�V���ꂽ�ۂ�true��	
		bool deformed;				///	�����钸�_�̈ړ��ɂ��A�ό`���ꂽ�Ƃ�
		double thermalEmissivity;	///	�M���˗�
		double heatflux;			///	�M�����l�@�\������R�ߓ_�̑������ςŌv�Z
		double fluxarea;			//>	 �M�����̌v�Z�ɗp����Aface�̃G���A
		bool mayIHheated;				//	IH�ŉ��M����\���̂���ʁ@�i�K�I�ɍi��
		std::vector<Vec2d> innerIH;	///	IH���M�̈���ɓ�����W�_�̓������x�N�^�[
	};
	//	��
	struct Edge{
		Edge(int v1=-1, int v2=-1);
		int vertices[2];
		///	���_ID�Ŕ�r
		bool operator < (const Edge& e2);
		///	���_ID�Ŕ�r
		bool operator == (const Edge& e2);
		double k;		//�S�̍����s��K�̗v�f
		double c;		//
		double b;		//�K�E�X�U�C�f���v�Z�ŗp����萔b
	};
	///	��{���(�������Ƀf�X�N���v�^����^��������)
	//@{
	///	���_
	std::vector<FemVertex> vertices;
	///	�l�ʑ�
	std::vector<Tet> tets;
	//@}
	
	/// @name �ǉ����	��{��񂩂�SetDesc()���v�Z���ċ��߂�B
	//@{
	///	���̕\�ʂ̒��_��ID
	std::vector<int> surfaceVertices;
	///	��
	std::vector<Face> faces;
	///	�ʂ̂������̕\�ʂ̂��̂��Afaces[0]..faces[nSurfaceFace-1]	�ȍ~�͓����̖�:faces[nSurfaceFace]..faces[faces.size()]
	unsigned nSurfaceFace;
	///	��
	std::vector<Edge> edges;
	///	�ӂ̂������̕\�ʂ̂��̂��Aedges[0]..edges[nSurfaceEdge-1]
	unsigned nSurfaceEdge;
	//@}

	/// @name �l�ʑ̂̌v�Z	�Ώۂɂ�炸�ɕK�v�ɂȂ�`��֐���XX���v�Z����֐�
	//@{
	void UpdateJacobian();
	//@}


	SPR_OBJECTDEF(PHFemMesh);


	PHFemMesh(const PHFemMeshDesc& desc=PHFemMeshDesc(), SceneIf* s=NULL);
	///	�f�X�N���v�^�̃T�C�Y
	virtual size_t GetDescSize() const { return sizeof(PHFemMeshDesc); };
	///	�f�X�N���v�^�̓ǂݏo��(�R�s�[��)
	virtual bool GetDesc(void* desc) const ;
	///�f�X�N���v�^�̐ݒ�B�����ŁA���_���W�Ǝl�ʑ̂�4���_��ID�̏�񂩂�A�ʂ�ӂɂ��Ă̏����v�Z���Ă����B
	virtual void SetDesc(const void* desc);
	///	������dt�i�߂鏈���BPHFemEngine���Ăяo���B
	virtual void Step(double dt);
	///	
	//int GetSurfaceVertex(int id){return surfaceVertices[id];};
	//int NSurfaceVertices(){return surfaceVertices.size();};
	//void SetVertexTc(int id,double temp){
	//	vertices[id].Tc = temp;
	//};
	/////
	//Vec3d GetPose(unsigned id){ vertices[id].pos; };
	//Vec3d GetSufVtxPose(unsigned id){ vertices[surfaceVertices[id]].pos; };
};




}	//	namespace Spr
#endif
