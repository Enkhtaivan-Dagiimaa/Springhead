#include "Framework.h"
#include <Framework/SprFWFemMesh.h>
#include <Graphics/GRFrame.h>
#include <Graphics/GRMesh.h>
#include <Physics/PHConstraint.h>
#include <Physics/PHFemMeshThermo.h>
#include <Foundation/UTLoadHandler.h>
#include "FWFemMesh.h"
#include "FWSprTetgen.h"



#include <Collision/CDQuickHull2DImp.h>


#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

#ifdef _DEBUG
# define PDEBUG(x)	x
#else
# define PDEBUG(x)
#endif

namespace Spr{;

FWFemMesh::FWFemMesh(const FWFemMeshDesc& d):grMesh(NULL){
	SetDesc(&d);
}
void FWFemMesh::Sync(bool ph2gr){	
	//	�e�X�g�p
	static double value, delta;
	if (value <= 0) delta = 0.01;
	if (value >= 1) delta = -0.01;
	value += delta;

	//	��������
	FWObject::Sync(ph2gr);
	if (ph2gr && grMesh->IsTex3D()){
		float* gvtx = grMesh->GetVertexBuffer();
		if (gvtx){
			int tex = grMesh->GetTexOffset();
			int stride = grMesh->GetStride();
			for(unsigned gv=0; gv<vertexIdMap.size(); ++gv){
				int pv = vertexIdMap[gv];
				//	PH���牽�炩�̕����ʂ�����Ă���
				//	value = phMesh�̔h���N���X->thermo[pv];
				//	GR�̃e�N�X�`�����W�Ƃ��Đݒ肷��B	s t r q �� r��ݒ�
				gvtx[stride*gv + tex + 2] = value + gvtx[stride*gv];	//	gvtx[stride*gv]�ŏꏊ�ɂ���ĈႤ�[�x���E���Ă��邱�Ƃ�
			}
		}	
	}else{
		assert(0);	//	not supported.
	}
}


size_t FWFemMesh::NChildObject() const{
	return FWObject::NChildObject() + (grMesh ? 1 : 0);
}
ObjectIf* FWFemMesh::GetChildObject(size_t pos){
	if (pos < FWObject::NChildObject()){
		return FWObject::GetChildObject(pos);
	}
	if (grMesh && pos < NChildObject()) return grMesh->Cast();
	return NULL;
}
bool FWFemMesh::AddChildObject(ObjectIf* o){
	GRMesh* mesh = o->Cast();
	if (mesh){
		grMesh = mesh;
		return true;
	}else{
		return FWObject::AddChildObject(o);
	}
}
void FWFemMesh::Loaded(UTLoadContext*){
	if (!phMesh) CreatePHFromGR();
	if (grFrame){
		grFrame->DelChildObject(grMesh->Cast());
		CreateGRFromPH();
		grFrame->AddChildObject(grMesh->Cast());
	}
}
bool FWFemMesh::CreatePHFromGR(){
	//	�Ăяo���ꂽ���_�� grMesh �ɃO���t�B�N�X�̃��b�V���������Ă���
	//	grMesh��ϊ����āAphMesh������B

	//	�ȉ��ŁAgrMesh����tetgen���Ăяo���ĕϊ����āApmd�ɒl�����Ă����Ηǂ��B
	PHFemMeshThermoDesc pmd;
	
	//TetGen�Ŏl�ʑ̃��b�V����
	Vec3d* vtxsOut=NULL;
	int* tetsOut=NULL;
	int nVtxsOut=0, nTetsOut=0;
	std::vector<Vec3d> vtxsIn;
	for(unsigned i=0; i<grMesh->vertices.size(); ++i) vtxsIn.push_back(grMesh->vertices[i]);
	sprTetgen(nVtxsOut, vtxsOut, nTetsOut, tetsOut, grMesh->vertices.size(), &vtxsIn[0], grMesh->faces.size(), &grMesh->faces[0], "pq1.2a0.003");
	
	//	PHMesh�p�̃f�B�X�N���v�^pmd�ɒl�����Ă���
	for(int i=0; i < nVtxsOut; i++){
		pmd.vertices.push_back(vtxsOut[i]);
	} 
	pmd.tets.assign(tetsOut, tetsOut + nTetsOut*4);
	//	PHMesh�̐���
	phMesh = DBG_NEW PHFemMeshThermo(pmd);
	if (GetPHSolid() && GetPHSolid()->GetScene())
		GetPHSolid()->GetScene()->AddChildObject(phMesh->Cast());
	return true;
}
void FWFemMesh::CreateGRFromPH(){
	//	���_�̑Ή��\��p��
	std::vector<int> vtxMap;
	DSTR<< "phMesh->vertices.size()" << phMesh->vertices.size() <<std::endl;
	vtxMap.resize(phMesh->vertices.size(), -1);
	for(unsigned i=0; i<phMesh->surfaceVertices.size(); ++i){
		vtxMap[phMesh->surfaceVertices[i]] = i;
	}
	GRMeshDesc gmd;	//	�V����GRMesh�̃f�X�N���v�^
	//	�\�ʂ̒��_��ݒ�
	for(unsigned i=0; i< phMesh->surfaceVertices.size(); ++i) 
		gmd.vertices.push_back(phMesh->vertices[ phMesh->surfaceVertices[i] ].pos);
	//	�\�ʂ̎O�p�`��ݒ�
	for(unsigned i=0; i< phMesh->nSurfaceFace; ++i) {
		GRMeshFace f;
		f.nVertices = 3;
		for(int j=0; j<3; ++j){
			f.indices[j] = vtxMap[phMesh->faces[i].vertices[j]];
		}
		gmd.faces.push_back(f);
	}
	//	phMesh�̎O�p�`��grMesh�̎O�p�`�̑Ή��\������	�d�Ȃ��Ă���ʐς��ő�̂��̂��Ή������
	//	�܂��A�@�����߂����̂�T���A��1�Ɩ�2��̒��_�̋������߂����̂Ɍ���A�d�Ȃ��Ă���ʐς����߂�B
	std::vector<Vec3f> pnormals(gmd.faces.size());
	for(unsigned pf=0; pf<gmd.faces.size(); ++pf){
		assert(gmd.faces[pf].nVertices == 3);
		pnormals[pf] = ((gmd.vertices[gmd.faces[pf].indices[2]] - gmd.vertices[gmd.faces[pf].indices[0]]) %
		(gmd.vertices[gmd.faces[pf].indices[1]] - gmd.vertices[gmd.faces[pf].indices[0]])).unit();
	}
	std::vector<Vec3f> gnormals(grMesh->faces.size());
	struct FaceWall{
		Vec3f wall[4];
	};
	std::vector<FaceWall> gWalls(gnormals.size());
	for(unsigned gf=0; gf<gnormals.size(); ++gf){
		gnormals[gf] = ((grMesh->vertices[grMesh->faces[gf].indices[2]] - grMesh->vertices[grMesh->faces[gf].indices[0]]) %
			(grMesh->vertices[grMesh->faces[gf].indices[1]] - grMesh->vertices[grMesh->faces[gf].indices[0]])).unit();
		int nv = grMesh->faces[gf].nVertices;
		for(int i=0; i<nv; ++i){
			gWalls[gf].wall[i] = ((grMesh->vertices[grMesh->faces[gf].indices[(i+1)%nv]] - grMesh->vertices[grMesh->faces[gf].indices[i]]) % gnormals[gf]).unit();
		}
	}
	std::vector<int> pFaceMap(pnormals.size());
	for(unsigned pf=0; pf<pnormals.size(); ++pf){
		std::vector<int> cand;
		for(unsigned gf=0; gf<gnormals.size(); ++gf){
			if (pnormals[pf] * gnormals[gf] > 0.8){	//	�@���������̂͂���
				int i;
				for(i=0; i<3; ++i){
					double d = gnormals[gf] * (gmd.vertices[gmd.faces[pf].indices[i]] - grMesh->vertices[grMesh->faces[gf].indices[0]]);
					if (d*d > 0.01) break;	//	���������ꂷ���Ă���̂͂���
				}	
				if (i==3){
					//	ph�̎O�p�`�̏d�S���Agr�̒��̖ʂɓ����Ă��镨��������
					Vec3f center;
					for(int v=0;v<3; ++v) center += gmd.vertices[gmd.faces[pf].indices[v]];
					center /= 3;
					int v;
					for(v=0; v<grMesh->faces[gf].nVertices; ++v){
						double d = gWalls[gf].wall[v] * (center - grMesh->vertices[grMesh->faces[gf].indices[v]]);
						if (d<0) break;
					}
					if (v == grMesh->faces[gf].nVertices){
						pFaceMap[pf] = gf;
						//	DEBUG�o��
						//	DSTR << "center:" << center <<std::endl;
						//	for(int v=0; v<grMesh->faces[gf].nVertices; ++v)
						//		DSTR << "vtx" << grMesh->vertices[grMesh->faces[gf].indices[v]] << "wall:" << gWalls[gf].wall[v] << std::endl;
						break;
					}
				}
			}
		}
	}
	PDEBUG(	DSTR << "FaceMap PHtoGR:"; )
	PDEBUG(	for(unsigned i=0; i<pFaceMap.size(); ++i) DSTR << pFaceMap[i] << " "; )
	PDEBUG(	DSTR << std::endl; )
	//	�Ή��\�ɉ����ă}�e���A�����X�g��ݒ�B
	gmd.materialList.resize(grMesh->materialList.size() ? pFaceMap.size() : 0);
	for(unsigned pf=0; pf<gmd.materialList.size(); ++pf){
		gmd.materialList[pf] = grMesh->materialList[pFaceMap[pf]];
	}
	//	�V�������GRMesh�̒��_����phMesh�̒��_�ւ̑Ή��\
	vertexIdMap.resize(gmd.vertices.size(), -1);
	//	�Ή��\�ɉ����āA���_�̃e�N�X�`�����W���쐬
	//		ph�̂P�_��gr�̒��_�����ɑΉ�����ꍇ������B
	//		���̏ꍇ�͒��_�̃R�s�[�����K�v������B
	std::vector<bool> vtxUsed(gmd.vertices.size(), false);
	for(unsigned pf=0; pf<pFaceMap.size(); ++pf){		
		for(unsigned i=0; i<3; ++i){
			int pv = gmd.faces[pf].indices[i];
			//	�e�N�X�`�����W���v�Z
			Vec2f texCoord;
			Vec3f normal;
			GRMeshFace& gFace = grMesh->faces[pFaceMap[pf]];
			GRMeshFace* gNormal = NULL;
			if (grMesh->normals.size()){
				gNormal = &gFace;
				if (grMesh->faceNormals.size()) gNormal = &grMesh->faceNormals[pFaceMap[pf]];
			}
			if (gFace.nVertices == 3){
				Vec3f weight;
				Matrix3f vtxs;
				for(unsigned j=0; j<3; ++j){
					vtxs.col(j) = grMesh->vertices[gFace.indices[j]];
				}
				int tmp[3];
				vtxs.gauss(weight, gmd.vertices[pv], tmp);
				for(unsigned j=0; j<3; ++j){
					assert(weight[j] > -0.001);
					texCoord += weight[j] * grMesh->texCoords[gFace.indices[j]];
					if(gNormal) normal += weight[j] * grMesh->normals[gNormal->indices[j]];
				}
			}else{	//	4���_
				//	�ǂ̂R���_�ŋߎ�����ƈ�ԗǂ����𒲂ׁA���̂R���_����
				Vec3f weight[4];
				Matrix3f vtxs[4];
				double wMin[4];
				double wMinMax = -DBL_MAX;
				int maxId=-1;
				for(int j=0; j<4; ++j){
					for(int k=0; k<3; ++k){
						vtxs[j].col(k) = grMesh->vertices[gFace.indices[k<j ? k : k+1]];
					}
					int tmp[3];
					vtxs[j].gauss(weight[j], gmd.vertices[pv], tmp);
					wMin[j] = DBL_MAX;
					for(int l=0; l<3; ++l) if (wMin[j] > weight[j][l]) wMin[j] = weight[j][l];
					if (wMin[j] > wMinMax){
						wMinMax = wMin[j];
						maxId = j;
					}
				}
				for(int j=0; j<3; ++j){
					texCoord += weight[maxId][j] * grMesh->texCoords[gFace.indices[j<maxId?j:j+1]];
					if(gNormal)
						normal += weight[maxId][j] * grMesh->normals[gFace.indices[j<maxId?j:j+1]];
				}
			}
			gmd.texCoords.resize(gmd.vertices.size());
			if (grMesh->normals.size()) gmd.normals.resize(gmd.vertices.size());
			//	�d�����_�̏ꍇ�̓R�s�[�����Ȃ�����
			if (vtxUsed[pv]){
				if (gmd.texCoords[pv] != texCoord || 
					(grMesh->normals.size() && gmd.normals[pv] != normal)){	
					//	���_�̃R�s�[�̍쐬
					gmd.vertices.push_back(gmd.vertices[pv]);
					gmd.texCoords.push_back(texCoord);
					if (gmd.normals.size()) gmd.normals.push_back(normal);
					gmd.faces[pf].indices[i] = gmd.vertices.size()-1;
					vertexIdMap.push_back(phMesh->surfaceVertices[pv]);
				}
			}else{	//	�����łȂ���΁A���ڑ��
				gmd.texCoords[pv] = texCoord;
				if (gmd.normals.size()) gmd.normals[pv] = normal; 
				vertexIdMap[pv] = phMesh->surfaceVertices[pv];
				vtxUsed[pv] = true;
			}
		}
	}
	//	GRMesh���쐬
	GRMesh* rv = grMesh->GetNameManager()->CreateObject(GRMeshIf::GetIfInfoStatic(), &gmd)->Cast();
	//	�}�e���A���̒ǉ�
	for (unsigned i=0; i<grMesh->NChildObject(); ++i){
		rv->AddChildObject(grMesh->GetChildObject(i));
	}
	//	�e�N�X�`�����[�h���R�s�[
	rv->tex3d = grMesh->tex3d;
	grMesh = rv;
}
}
