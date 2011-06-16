/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include <Collision/CDConvexMesh.h>
#include <Collision/CDQuickHull3DImp.h>
#include <set>

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
CDConvexMesh::CDConvexMesh(){
}
CDConvexMesh::CDConvexMesh(const CDConvexMeshDesc& desc){
	SetDesc(&desc);
}
void CDConvexMesh::SetDesc(const void *ptr){
	CDShape::SetDesc(ptr);
	const CDConvexMeshDesc* desc = (const CDConvexMeshDesc*)ptr;
	base = desc->vertices;
	CalcFace();	
}
bool CDConvexMesh::GetDesc(void *ptr) const {
	CDConvexMeshDesc* desc = (CDConvexMeshDesc*)ptr;
	desc->vertices = base;
	return CDShape::GetDesc(ptr);
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
	// ���ύ��W�̌v�Z
	CalcAverage();

	//	�ʂ̒��_ID��U��Ȃ��� / �@�����v�Z
	for(CDFaces::iterator it = faces.begin(); it != faces.end(); ++it){
		for(int i=0; i<3; ++i){
			it->vtxs[i] = vtxIds.FindPos(it->vtxs[i]);
		}
		it->normal = (base[it->vtxs[2]] - base[it->vtxs[0]]) % (base[it->vtxs[1]] - base[it->vtxs[0]]);
		it->normal.unitize();
		if(it->normal * (base[it->vtxs[0]] - average) < 0.0f)
			it->normal *= -1.0f;
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
	//CalcCenter();
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
			//Vec3f na =  ((base[a.vtxs[2]] - pa) ^ (base[a.vtxs[1]] - pa)).unit();
			//Vec3f nb =  ((base[b.vtxs[2]] - pb) ^ (base[b.vtxs[1]] - pb)).unit();
			float len;
			//len = pa*na - pb*na;
			len = (pa - pb) * a.normal;
			if (/*na*nb*/a.normal * b.normal > 0.998f && (len>0?len:-len) < 1e-5f){
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
void CDConvexMesh::CalcAverage(){
	average.clear();
	for(unsigned i=0; i<base.size(); ++i){
		average += base[i];
	}
	average /= base.size();
}
bool CDConvexMesh::IsInside(const Vec3f& p){
	for(int i = 0; i < (int)faces.size(); i++){
		if(faces[i].normal * (base[faces[i].vtxs[0]] - p) < 0.0f)
			return false;
	}
	return true;
}

int CDConvexMesh::Support(Vec3f& w, const Vec3f& v) const {
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
	w = base[curPos];
	return curPos;
}
std::vector<int>& CDConvexMesh::FindNeighbors(int vtx){
	return neighbor[vtx];
}

CDFaceIf* CDConvexMesh::GetFace(int i){
	return faces.at(i).Cast();
}
int CDConvexMesh::NFace(){
	return faces.size();
}
Vec3f* CDConvexMesh::GetVertices(){
	return &*base.begin();
}
int CDConvexMesh::NVertex(){
	return base.size();
}

int CDConvexMesh::LineIntersect(const Vec3f& origin, const Vec3f& dir, Vec3f* result, float* offset){
	// �S�Ă̖ʂƂ̌����𒲂ׂ���ՂȎ���
	const float eps = 1.0e-10f;
	Matrix2f A;
	Vec2f b, x;
	Vec3f p, u1, u2, diff;
	int num = 0;

	for(int i = 0; i < (int)faces.size(); i++){
		const CDFace& f = faces[i];
		const Vec3f& n = f.normal;
		float tmp = n * dir;
		if(abs(tmp) < eps)
			continue;
		// �����Ɩʂ̌�_p
		float s = ((base[f.vtxs[0]] - origin) * n) / tmp;
		if(s < 0.0)
			continue;
		p = origin + dir * s;
		// 3�p�`�̓����ɂ��邩
		u1 = base[f.vtxs[1]] - base[f.vtxs[0]];
		u2 = base[f.vtxs[2]] - base[f.vtxs[0]];
		A[0][0] = u1.square();
		A[0][1] = A[1][0] = u1 * u2;
		A[1][1] = u2.square();
		if(A.det() < eps)
			continue;
		diff = p - base[f.vtxs[0]];
		b[0] = diff * u1;
		b[1] = diff * u2;
		x = A.inv() * b;	// 2��������������
		if(0.0 <= x[0] && x[0] <= 1.0 && 0.0 <= x[1] && x[1] <= 1.0 && x[0] + x[1] <= 1.0){
			result[num] = p;
			offset[num] = s;
			num++;
		}
		if(num == 2)		// �������3�ȏ�͂��蓾�Ȃ����O�̂���
			break;
	}
	return num;
}
void CDConvexMesh::Print(std::ostream& os) const{
	PrintHeader(os, false);
	int w = os.width();
	os.width(0);

	os << UTPadding(w+2) << "vtxs: " << base.size() << std::endl;
	os << UTPadding(w+2) << "faces:" << faces.size() << std::endl;

	os.width(w);
	PrintFooter(os);
}

}
