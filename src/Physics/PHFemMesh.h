/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PHFEMMESH_H
#define PHFEMMESH_H

#include <SprPhysics.h>
#include <Foundation/Object.h>
#include <Physics/PHScene.h>

namespace Spr{;

///	�L���v�f�@�V�~�����[�V�����̂��߂̃��b�V��(4�ʑ̃��b�V��)

class PHFemMesh: public SceneObject{
public:
	//	���_
	struct FemVertex{
		Vec3d pos;
		std::vector<int> tets;
		std::vector<int> edges;
		FemVertex();
	};
	//	�l�ʑ�
	struct Tet{
		int vertices[4];	//	���_ID
		int edges[6];		//	�Ή�����ӂ�ID�B0:��01, 1:��12, 2:��20, 3:��03, 4:��13, 5:��23
		int& edge(int i, int j);
	};
	//	�l�ʑ̖̂ʁB
	class Face{
		///	��r���邽�߂́A�\�[�g�ς݂̒��_id�BUpdate()�ōX�V�B
		int sorted[3];
	public:
		///	���_ID�B���ԂŖʂ̕\����\���B
		int vertices[3];
		void Update();
		///	���_ID�Ŕ�r
		bool operator < (const Face& f2);
		///	���_ID�Ŕ�r
		bool operator == (const Face& f2);
	};
	//	��
	struct Edge{
		Edge(int v1=-1, int v2=-1);
		int vertices[2];
		///	���_ID�Ŕ�r
		bool operator < (const Edge& e2);
		///	���_ID�Ŕ�r
		bool operator == (const Edge& e2);
	};
	//	��{���(�������ɗ^��������)
	///	���_
	std::vector<FemVertex> vertices;
	///	�l�ʑ�
	std::vector<Tet> tets;
	
	//	�ȉ��͊�{��񂩂�v�Z���ċ��߂�B
	///	���̕\�ʂ̒��_��ID
	std::vector<int> surfaceVertices;
	///	���̕\�ʂ̖�
	std::vector<Face> faces;
	///	�ʂ̂������̕\�ʂ̂��̂��Afaces[0]..faces[nSurfaceFace-1]
	unsigned nSurfaceFace;
	///	���̕\�ʂ̕�
	std::vector<Edge> edges;
	///	�ӂ̂������̕\�ʂ̂��̂��Aedges[0]..edges[nSurfaceEdge]
	unsigned nSurfaceEdge;

	
	SPR_OBJECTDEF(PHFemMesh);
	PHFemMesh(const PHFemMeshDesc& desc=PHFemMeshDesc(), SceneIf* s=NULL);
	//	�f�X�N���v�^�̃T�C�Y
	virtual size_t GetDescSize() const { return sizeof(PHFemMeshDesc); };
	//	�f�X�N���v�^�̓ǂݏo��(�R�s�[��)
	virtual bool GetDesc(void* desc) const ;
	// �f�X�N���v�^�̐ݒ�
	virtual void SetDesc(const void* desc);
	//	�l�ʑ̒P�ʂ̌W���s����A���_�P�ʂ̌W���s��֕ϊ�����B
	void Tet2Vertex();

};




}	//	namespace Spr
#endif
