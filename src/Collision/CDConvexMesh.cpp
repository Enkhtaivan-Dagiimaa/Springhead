/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include <Springhead.h>
#include <Foundation/Object.h>
#include "Collision.h"
#include <set>
#ifdef USE_HDRSTOP
#pragma hdrstop
#endif
#include "CDQuickHull3DImp.h"

#define CD_EPSILON	HULL_EPSILON
#define CD_INFINITE	HULL_INFINITE

namespace Spr{;
const double epsilon = 1e-8;
const double epsilon2 = epsilon*epsilon;


int CDVertexIDs::FindPos(int id) const {
	const_iterator lower = begin();
	const_iterator upper = end();
	while(lower != upper){
		const_iterator middle = lower + (upper-lower)/2;
		if (*middle < id){
			lower = middle;
		}else if (*middle > id){
			upper = middle;
		}else{
			return middle - begin();
		}
	}
	return -1;
}



//----------------------------------------------------------------------------
//	CDConvexMesh
IF_OBJECT_IMP(CDConvexMesh, CDConvex);

CDConvexMesh::CDConvexMesh(){
}
CDConvexMesh::CDConvexMesh(const CDConvexMeshDesc& desc){
	material = desc.material;
	base = desc.vertices;
	CalcFace();
}
bool CDConvexMesh::GetDesc(void *ptr) const {
	CDConvexMeshDesc* desc = (CDConvexMeshDesc*)ptr;
	desc->material = material;
	desc->vertices = base;
	return true;
}


bool CDConvexMesh::FindCutRing(CDCutRing& ring, const Posed& toW){
	Posed toL	= toW.Inv();
	//	���_���ǂ������ɂ��邩���ׂ�D
	Vec3d planePosL = toL * ring.local.Pos();
	Vec3d planeNormalL = toL.Ori() * ring.local.Ori() * Vec3d(1,0,0);
	std::vector<int> sign;
	sign.resize(base.size());
	double d = planeNormalL * planePosL;
	for(unsigned i=0; i<base.size(); ++i){
		double vtxDist = planeNormalL * base[i];
		if (vtxDist > d + epsilon) sign[i] = 1;
		else if (vtxDist < d - epsilon) sign[i] = -1;
		else sign[i] = 0;
	}
	bool rv = false;
	//	�܂������Ă���ʂ̏ꍇ�C��������߂�
	for(unsigned i=0; i<faces.size(); ++i){
		if (sign[faces[i].vtxs[0]] == sign[faces[i].vtxs[1]] && //	�S���������̂Ƃ��́C
			sign[faces[i].vtxs[0]] == sign[faces[i].vtxs[2]]) continue;	//	�p�X
		//	�ڐG��(plane,��1)�Ƒ��ʑ̖̂�(face,��2)�̌�������߂�
		/*	�������Ƃ���1�_��������̂�
						|��2
						|n2
						|d2
				   O	|
			-------+----+----��1 n1,d1=0
						|P
			P = a*n1 + b*n2;
				a = (d1 - d2*(n1*n2)) / (1-(n1*n2)^2)
				b = (d2 - d1*(n1*n2)) / (1-(n1*n2)^2)
			����1(plane)�Ɩ�2(face)����钼����ʂ�1�_
			O:		planePosL
			n1,d1	plane�̖@��(planeNormalL)�CO����̋���=0
			n2,d2	face�̖@��(faceNormal)�CO����̋���			
		*/
		Vec3d faceNormal = ((base[faces[i].vtxs[1]] - base[faces[i].vtxs[0]]) ^ (base[faces[i].vtxs[2]] - base[faces[i].vtxs[0]])).unit();
		double faceDist = faceNormal * (base[faces[i].vtxs[0]] - planePosL);
		Vec3d lineDirection = (planeNormalL ^ faceNormal).unit();
		double ip = planeNormalL * faceNormal;
		if ((ip < 1.0-epsilon2) && (ip > -1.0+epsilon2)){	//	���s�Ȗʂ͖���
			double a = -faceDist*ip / (1.0-(ip*ip));
			double b = faceDist / (1.0-(ip*ip));
			Vec3d lineOff = a*planeNormalL + b*faceNormal;
			Vec3d lineNormal = planeNormalL ^ lineDirection;
			double lineDist = lineNormal * lineOff;
			if (finite(lineDist)) {	
				//	local -> world -> ring2�����n�ɕϊ�
				Posed to2D = ring.localInv * toW;
				Vec2d lineNormal2D = (to2D.Ori() * lineNormal).sub_vector(1, Vec2d());
				assert(finite(lineNormal2D.x));
				assert(finite(lineNormal2D.y));

				//	���͓����������������̂ŁC normal, dist �𔽓]���� ring.lines �ɒǉ�
				ring.lines.push_back(CDCutLine(-lineNormal2D, -lineDist));
				rv = true;
			}
		}
	}
	return rv;
}

void CalcBBox(Vec3f& bbmin, Vec3f& bbmax, Posed& pose){
	
}

class CDQhullVtx{
public:
	static Vec3f* base;
	int vtxID;
	int VtxID() const { return vtxID; }
	Vec3f GetPos() const { return base[VtxID()]; }
	CDQHPlane<CDQhullVtx>* horizon;
};

Vec3f* CDQhullVtx::base;
void CDConvexMesh::CalcFace(){
	curPos = 0;
	faces.clear();
	neighbor.clear();
	
	//	base�̓_����ʑ��ʑ̂����D
	std::vector<CDQhullVtx> vtxs;
	std::vector<CDQhullVtx*> pvtxs;
	vtxs.resize(base.size());
	pvtxs.resize(base.size());
	neighbor.resize(base.size());
	for(unsigned int i=0; i<base.size(); ++i){
		vtxs[i].vtxID = i;
		pvtxs[i] = &vtxs[i];
	}
	CDQhullVtx::base = &*base.begin();
	int n = base.size();
//	CDQHPlanes<CDQhullVtx> planes(n*(n-1)*(n-2)/6);
	CDQHPlanes<CDQhullVtx> planes(n*10);
	planes.CreateConvexHull(&*pvtxs.begin(), &*pvtxs.begin() + pvtxs.size());
	std::set<int> usedVtxs;
	for(CDQHPlane<CDQhullVtx>* plane =planes.begin; plane != planes.end; ++plane){
		if (plane->deleted) continue;
		faces.push_back(CDFace());
		for(int i=0; i<3; ++i){
			faces.back().vtxs[i] = plane->vtx[i]->VtxID();
			usedVtxs.insert(plane->vtx[i]->VtxID());
		}
	}
	//	�ʑ��ʑ̂Ɏg��ꂽ���_�������
	CDVertexIDs vtxIds;
	int hoge = vtxIds.size();
	for(std::set<int>::iterator it = usedVtxs.begin(); it != usedVtxs.end(); ++it){
		vtxIds.push_back(*it);
	}
	//	base����s�v�Ȓ��_���폜
	int pos = base.size()-1;
	int i = vtxIds.size()-1;
	while(true){
		while(pos >= 0 && (i < 0 || pos>vtxIds[i])){
			base.erase(base.begin()+pos);
			 --pos;
		}
		if(pos < 0)break;
		--i;
		--pos;
	}
	//	�ʂ̒��_ID��U��Ȃ���
	for(CDFaces::iterator it = faces.begin(); it != faces.end(); ++it){
		for(int i=0; i<3; ++i){
			it->vtxs[i] = vtxIds.FindPos(it->vtxs[i]);
		}
	}
	//	�ׂ̒��_���X�g�����D(GJK��Support�Ɏg�p)
	neighbor.resize(vtxIds.size());
	for(CDFaces::iterator it = faces.begin(); it != faces.end(); ++it){
		//	�e�ӂ�2�̖ʂɋt�����Ɏg����̂ŁC�S���̖ʂ����񂷂�ƁC
		//	���傤�Ǘׂ̒��_���X�g����������D
		for(int i=0; i<3; ++i){
			int pos = it->vtxs[i];
			int next = it->vtxs[(i+1)%3];
			neighbor[pos].push_back(next);
		}
	}
	//	�ʑ��ʑ̖̂ʂ̂����C�����ʕ\���ɕK�v�Ȗʂ�����O���ɏW�߂�D
	MergeFace();
}
void CDConvexMesh::MergeFace(){
	//int nf = faces.size();
	CDFaces erased;
	for(unsigned int i=0; i<faces.size(); ++i){
		for(unsigned int j=i+1; j<faces.size(); ++j){
			CDFace& a = faces[i];
			CDFace& b = faces[j];
			Vec3f& pa = base[a.vtxs[0]];
			Vec3f& pb = base[b.vtxs[0]];
			Vec3f na =  ((base[a.vtxs[2]] - pa) ^ (base[a.vtxs[1]] - pa)).unit();
			Vec3f nb =  ((base[b.vtxs[2]] - pb) ^ (base[b.vtxs[1]] - pb)).unit();
			float len;
			len = pa*na - pb*na;
			if (na*nb > 0.998f && (len>0?len:-len) < 1e-5f){
				erased.push_back(faces[i]);
				faces.erase(faces.begin() + i);
				i--;
				break;
			}
		}
	}
	nPlanes = faces.size();
	faces.insert(faces.end(), erased.begin(), erased.end());
	//	DSTR << "Poly faces:" << nf << "->" << faces.size() << std::endl;
}
Vec3f CDConvexMesh::Support(const Vec3f& v) const {
	int lastPos = -1;
	float h = base[curPos] * v;
	float d=0;
	int count = 0;
	while (1) {
		const std::vector<int>& curNeighbor = neighbor[curPos];
		int i = 0;
		int n = curNeighbor.size();
		while(i!=n){
			++ count;
			if (curNeighbor[i] == lastPos){
				++i;
			}else{
				d = base[curNeighbor[i]]*v;
				if (count > 1000){	//hase	���̏������Ȃ����ƁCVC7.1�ł́C�œK�������������Ȃ��āC�������[�v�ɂȂ�D�Ȃ��D
					DSTR << "d:" << d << " h:" << h;
					DSTR << " CN:" << curNeighbor[i] << " i:" <<i << " n:" << n << std::endl;
				}
				if (d>h) break;
				++i;
			}
		}
		if (i == n) break;
		lastPos = curPos;
		curPos = curNeighbor[i];
		h = d;
	}
	return base[curPos];
}

CDFaceIf* CDConvexMesh::GetFace(size_t i){
	return &faces.at(i);
}
size_t CDConvexMesh::NFace(){
	return faces.size();
}
Vec3f* CDConvexMesh::GetVertices(){
	return &*base.begin();
}
size_t CDConvexMesh::NVertex(){
	return base.size();
}

}
