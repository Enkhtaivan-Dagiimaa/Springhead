/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include <Springhead.h>
#include "CDConvexMeshInterpolate.h"
#include <set>
#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

#define CD_EPSILON	HULL_EPSILON
#define CD_INFINITE	HULL_INFINITE

namespace Spr {;
const double epsilon = 1e-8;
const double epsilon2 = epsilon*epsilon;

CDConvexMeshInterpolate::CDConvexMeshInterpolate(){
}
CDConvexMeshInterpolate::CDConvexMeshInterpolate(const CDConvexMeshInterpolateDesc& desc):
	CDConvexMesh(desc){
}

Vec3f CDConvexMeshInterpolate::Support(const Vec3f& v) const {
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
	const std::vector<int>& cn = neighbor[curPos];
	struct Near{
		int id;
		float dist;
		Near(int i, double d):id(i), dist(d){
		}
	};
	typedef std::vector<Near> Nears;
	Nears nears;
	const float minDist = 0.005;
	float sum = minDist;
	nears.push_back(Near(curPos, minDist));
	for (unsigned i=0; i<cn.size(); ++i){
		double d = base[cn[i]] * v;
		if (h-d < minDist){
			nears.push_back(Near(i, minDist - (h-d)));
			sum += nears.back().dist;
		}
	}
	Vec3f rv;
	for(unsigned i=0; i<nears.size(); ++i){
		rv += base[nears[i].id] * nears[i].dist / sum;
	}
	return rv;
}


}