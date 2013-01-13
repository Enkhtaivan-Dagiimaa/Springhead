/*
 *  Copyright (c) 2003-2012, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

#include <Graphics/GRMesh.h>
#include <Foundation/UTLoadHandler.h>
#include "FWFemMeshNew.h"
#include "FWSprTetgen.h"

#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

#ifdef _DEBUG
# define PDEBUG(x)	x
#else
# define PDEBUG(x)
#endif

namespace Spr{;

FWFemMeshNew::FWFemMeshNew(const FWFemMeshNewDesc& d){
	grFemMesh = NULL;
	// p: piecewise linear comlex, q:2.1�����l�ʑ̘̂c��(1�ȏ�`�H�ȉ��j�Aa:�e��
	//meshRoughness = "pq2.1a0.002";
	meshRoughness = "pq2.1a0.01";
	//meshRoughness = "pq2.1a1.0";	
	SetDesc(&d);
	texture_mode = 2;		//	�e�N�X�`���\���̏����l�F���x
}

size_t FWFemMeshNew::NChildObject() const{
	return FWObject::NChildObject() + (grFemMesh ? 1 : 0) + (phFemMesh ? 1 : 0);
}

ObjectIf* FWFemMeshNew::GetChildObject(size_t pos){
	if(pos < FWObject::NChildObject()){
		return FWObject::GetChildObject(pos);
	}else{
		pos -= FWObject::NChildObject();
	}	
	if(grFemMesh){
		if (pos == 0) return grFemMesh->Cast();
		else pos --;
	}
	if(phFemMesh){
		if (pos == 0) return phFemMesh->Cast();
		else pos --;
	}
	return NULL;
}

bool FWFemMeshNew::AddChildObject(ObjectIf* o){
	PHFemMeshNew* pm = o->Cast();
	if (pm){
		phFemMesh = pm;
		phFemMesh->SetPHSolid(GetPHSolid());
		return true;
	}
	GRMesh* mesh = o->Cast();
	if (mesh){
		grFemMesh = mesh;
		return true;
	}
	return FWObject::AddChildObject(o);
}

void FWFemMeshNew::Loaded(UTLoadContext*){
	if(grFemMesh){
		if(!phFemMesh){
			// phFemMesh��NULL�̏ꍇ�A�V���ɐ�������B
			// �v�Z���W���[���͍���Ȃ��̂ŕʂɍ��K�v������i�������j�B
			phFemMesh = DBG_NEW PHFemMeshNew();
			// PHFemEngine�ɒǉ��o�^	
			if(GetPHSolid() && GetPHSolid()->GetScene()){
				GetPHSolid()->GetScene()->AddChildObject(phFemMesh->Cast());
				phFemMesh->SetPHSolid(GetPHSolid());
			}
			CreatePHFemMeshFromGRMesh();		
		}else if(phFemMesh->vertices.size() == 0){
			// phFemMesh�̒��_����̏ꍇ
			// tetgen�ɂ�郁�b�V���̐���
			CreatePHFemMeshFromGRMesh();
		}
	}

	if (grFrame){
		grFrame->DelChildObject(grFemMesh->Cast());
		CreateGRFromPH();
		grFrame->AddChildObject(grFemMesh->Cast());
	}

	phFemMesh->Init();
}

bool FWFemMeshNew::CreatePHFemMeshFromGRMesh(){
	//	�Ăяo���ꂽ���_��grFemMesh �ɃO���t�B�N�X�̃��b�V���������Ă���B
	//	grFemMesh���l�ʑ̃��b�V���ɕϊ����āAphFemMesh������B
	
	//TetGen�Ŏl�ʑ̃��b�V����
	Vec3d* vtxsOut=NULL;
	int* tetsOut=NULL;
	int nVtxsOut=0, nTetsOut=0;
	std::vector<Vec3d> vtxsIn;
	for(unsigned i = 0; i < grFemMesh->vertices.size(); ++i) vtxsIn.push_back(grFemMesh->vertices[i]);
	// swithes q+(���a/�ŒZ��) (e.g. = q1.0~2.0) a �ő�̑̐� 
	sprTetgen(nVtxsOut, vtxsOut, nTetsOut, tetsOut, (int)grFemMesh->vertices.size(), &vtxsIn[0], (int)grFemMesh->faces.size(), &grFemMesh->faces[0], meshRoughness);
	
	//	phFemMesh�p�̃f�B�X�N���v�^pmd�ɒl�����Ă���
	PHFemMeshNewDesc pmd;
	for(int i=0; i < nVtxsOut; i++){
		pmd.vertices.push_back(vtxsOut[i]);
	} 
	pmd.tets.assign(tetsOut, tetsOut + nTetsOut*4);
	phFemMesh->SetDesc(&pmd);

	return true;
}

struct FaceMap{
	FaceMap(){
		vtxs[0] = vtxs[1] = vtxs[2] = -1;
		face = -1;
	}
	int vtxs[3];
	int& operator [](int i) {return vtxs[i]; }
	int face;
};

void FWFemMeshNew::CreateGRFromPH(){
	// 3D�e�N�X�`�����g����悤�ɂ��邽�߂ɁAgrFemMesh���č\�z����H
	// 3D�e�N�X�`�����g��Ȃ��ꍇ�͕K�v�Ȃ��H

	//	���_�̑Ή��\��p��
	std::vector<int> vtxMap;
	vtxMap.resize(phFemMesh->vertices.size(), -1);
	for(unsigned i=0; i<phFemMesh->surfaceVertices.size(); ++i){
		vtxMap[phFemMesh->surfaceVertices[i]] = i;
	}
	GRMeshDesc gmd;	//	�V����GRMesh�̃f�X�N���v�^
	//	�\�ʂ̒��_��ݒ�
	for(unsigned i=0; i< phFemMesh->surfaceVertices.size(); ++i) 
		gmd.vertices.push_back(phFemMesh->vertices[ phFemMesh->surfaceVertices[i] ].pos);
	//	�\�ʂ̎O�p�`��ݒ�
	for(unsigned i=0; i< phFemMesh->nSurfaceFace; ++i) {
		GRMeshFace f;
		f.nVertices = 3;
		for(int j=0; j<3; ++j){
			f.indices[j] = vtxMap[phFemMesh->faces[i].vertexIDs[j]];
		}
		gmd.faces.push_back(f);
	}
	//	phFemMesh�̎O�p�`��grFemMesh�̎O�p�`�̑Ή��\������	�d�Ȃ��Ă���ʐς��ő�̂��̂��Ή������
	//	�܂��A�@�����߂����̂�T���A��1�Ɩ�2��̒��_�̋������߂����̂Ɍ���A�d�Ȃ��Ă���ʐς����߂�B
	std::vector<Vec3f> pnormals(gmd.faces.size());
	for(unsigned pf=0; pf<gmd.faces.size(); ++pf){
		assert(gmd.faces[pf].nVertices == 3);
		pnormals[pf] = ((gmd.vertices[gmd.faces[pf].indices[2]] - gmd.vertices[gmd.faces[pf].indices[0]]) %
		(gmd.vertices[gmd.faces[pf].indices[1]] - gmd.vertices[gmd.faces[pf].indices[0]])).unit();
	}
	std::vector<Vec3f> gnormals(grFemMesh->faces.size());
	struct FaceWall{
		Vec3f wall[4];
	};
	std::vector<FaceWall> gWalls(gnormals.size());
	for(unsigned gf=0; gf<gnormals.size(); ++gf){
		gnormals[gf] = ((grFemMesh->vertices[grFemMesh->faces[gf].indices[2]] - grFemMesh->vertices[grFemMesh->faces[gf].indices[0]]) %
			(grFemMesh->vertices[grFemMesh->faces[gf].indices[1]] - grFemMesh->vertices[grFemMesh->faces[gf].indices[0]])).unit();
		int nv = grFemMesh->faces[gf].nVertices;
		for(int i=0; i<nv; ++i){
			gWalls[gf].wall[i] = ((grFemMesh->vertices[grFemMesh->faces[gf].indices[(i+1)%nv]] - grFemMesh->vertices[grFemMesh->faces[gf].indices[i]]) % gnormals[gf]).unit();
		}
	}
	std::vector< FaceMap > pFaceMap(pnormals.size());
	for(unsigned pf=0; pf<pnormals.size(); ++pf){
		//	�����̕���(pf)�Ɠ��ꕽ�ʂɍڂ��Ă���O���t�B�N�X�̕��ʂ��
		std::vector<int> gfCands;
		for(unsigned gf=0; gf<gnormals.size(); ++gf){
			if (pnormals[pf] * gnormals[gf] > 0.999){	//	�@���������̂͂���
				int pv;
				for(pv=0; pv<3; ++pv){
					double d = gnormals[gf] * (gmd.vertices[gmd.faces[pf].indices[pv]] - grFemMesh->vertices[grFemMesh->faces[gf].indices[0]]);
					if (d*d > 1e-8) break;	//	���������ꂷ���Ă���̂͂���
				}
				if (pv==3) gfCands.push_back(gf);
			}
		}
		//	pf�̊e���_�ɑΉ�����gf��������
		std::vector<int> gfForPv[3];
		for(unsigned pv=0; pv<3; ++pv){
			for(unsigned i=0; i<gfCands.size(); ++i){
				int gf = gfCands[i];
				int gv;
				for(gv=0; gv<grFemMesh->faces[gf].nVertices; ++gv){
					double d = gWalls[gf].wall[gv] * (gmd.vertices[gmd.faces[pf].indices[pv]] - grFemMesh->vertices[grFemMesh->faces[gf].indices[gv]]);
					if (d < -1e-6) break;
				}
				if (gv == grFemMesh->faces[gf].nVertices){
					gfForPv[pv].push_back(gf);
				}
			}
		}
		//	�R�������_���A�P�̃O���t�B�N�X�O�p�`�����L����ꍇ�A���L������̂�D��
		std::vector< std::pair<int, int> > votes;
		for(int i=0; i<3; ++i){
			for(unsigned j=0; j<gfForPv[i].size(); ++j){
				unsigned k;
				for(k=0; k<votes.size(); ++k){
					if (votes[k].second == gfForPv[i][j]){
						votes[k].first ++;
						break;
					}
				}
				if (k==votes.size()) votes.push_back(std::make_pair(1, gfForPv[i][j]));
			}
		}
		std::sort(votes.begin(), votes.end(), std::greater<std::pair<int, int> >());
		for(int i=0; i<3; ++i){
			for(unsigned j=0; j<votes.size(); ++j){
				for(unsigned k=0; k<gfForPv[i].size(); ++k){
					if (votes[j].second == gfForPv[i][k]){
						pFaceMap[pf][i] = gfForPv[i][k];
						goto nextPv;
					}
				}
			}
			nextPv:;
		}
		if (pFaceMap[pf][0] == pFaceMap[pf][1]) pFaceMap[pf].face = pFaceMap[pf][0];
		else if (pFaceMap[pf][1] == pFaceMap[pf][2]) pFaceMap[pf].face = pFaceMap[pf][1];
		else if (pFaceMap[pf][2] == pFaceMap[pf][0]) pFaceMap[pf].face = pFaceMap[pf][2];
		else pFaceMap[pf].face = pFaceMap[pf][0];
		if (pFaceMap[pf].face == -1){
			return;
		}
	}
#if 0
	DSTR << "FaceMap PHtoGR:\n";
	for(unsigned i=0; i<pFaceMap.size(); ++i)
		DSTR << pFaceMap[i].face << ":" << pFaceMap[i][0] << " " << pFaceMap[i][1] << " " << pFaceMap[i][2] << ", ";
	DSTR << std::endl;
#endif

	//	�Ή��\�ɉ����ă}�e���A�����X�g��ݒ�B
	gmd.materialList.resize(grFemMesh->materialList.size() ? pFaceMap.size() : 0);
	for(unsigned pf=0; pf<gmd.materialList.size(); ++pf){
		gmd.materialList[pf] = grFemMesh->materialList[pFaceMap[pf].face];
	}

	//	�V�������GRMesh�̒��_����phFemMesh�̒��_�ւ̑Ή��\
	vertexIdMap.resize(gmd.vertices.size(), -1);
	//	�Ή��\�ɉ����āA���_�̃e�N�X�`�����W���쐬
	//		ph�̂P�_��gr�̒��_�����ɑΉ�����ꍇ������B
	//		���̏ꍇ�͒��_�̃R�s�[�����K�v������B
	assert(grFemMesh->faceNormals.size());	// ���[�h�����t�@�C���ɖ@����񂪊܂܂�Ă��Ȃ�
	std::vector<bool> vtxUsed(gmd.vertices.size(), false);
	for(unsigned pf=0; pf<pFaceMap.size(); ++pf){		
		for(unsigned i=0; i<3; ++i){
			int pv = gmd.faces[pf].indices[i];
			//	�e�N�X�`�����W���v�Z
			Vec2f texCoord;
			Vec3f normal;
			GRMeshFace& gFace = grFemMesh->faces[pFaceMap[pf][i]];
			GRMeshFace& gFaceNormal = grFemMesh->faceNormals[pFaceMap[pf].face];
			GRMeshFace* gNormal = NULL;
			if (grFemMesh->normals.size()){
				gNormal = &gFace;
				if (grFemMesh->faceNormals.size()) gNormal = &grFemMesh->faceNormals[pFaceMap[pf].face];
			}
			if (gFace.nVertices == 3){
				Vec3f weight;
				Matrix3f vtxs;
				for(unsigned j=0; j<3; ++j){
					vtxs.col(j) = grFemMesh->vertices[gFace.indices[j]];
				}
				int tmp[3];
				vtxs.gauss(weight, gmd.vertices[pv], tmp);
				for(unsigned j=0; j<3; ++j){
					if (weight[j] <= -0.001){
						DSTR << "�O���t�B�N�X��3���_�̊O���ɕ����̒��_������܂��B" << std::endl; 
						for(unsigned k=0; k<3; ++k){
							for(unsigned ax=0; ax<3; ++ax){
								DSTR << grFemMesh->vertices[gFace.indices[k]][ax];
								DSTR << (ax==2 ? "\n" : "\t");
							}
						}
						for(unsigned ax=0; ax<3; ++ax){
							DSTR << gmd.vertices[pv][ax];
							DSTR << (ax==2 ? "\n" : "\t");
						}
					}
					//assert(grFemMesh->texCoords.size() == 0);	// ���[�h�����t�@�C����UV�}�b�s���O�̏�񂪂Ȃ�
					texCoord += weight[j] * grFemMesh->texCoords[gFace.indices[j]];
					if(gNormal) normal += weight[j] * grFemMesh->normals[gNormal->indices[j]];
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
						vtxs[j].col(k) = grFemMesh->vertices[gFace.indices[k<j ? k : k+1]];
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
					if (weight[maxId][j] <= -0.001){
						DSTR << "�O���t�B�N�X��3���_\t"; 
						for(unsigned k=0; k<3; ++k){
							DSTR << grFemMesh->vertices[gFace.indices[k]] << "\t";
						}
						DSTR << "�̊O���ɕ����̒��_:\t" << gmd.vertices[pv] << "������܂�" << std::endl;
					}
					//assert(grFemMesh->texCoords.size() == 0);	// ���[�h�����t�@�C����UV�}�b�s���O�̏�񂪂Ȃ�
					texCoord += weight[maxId][j] * grFemMesh->texCoords[gFace.indices[j<maxId?j:j+1]];
					if(gNormal){
						normal += weight[maxId][j] * grFemMesh->normals[gFaceNormal.indices[j<maxId?j:j+1]];
					}
				}
			}
			gmd.texCoords.resize(gmd.vertices.size());
			if (grFemMesh->normals.size()) gmd.normals.resize(gmd.vertices.size());
			//	�d�����_�̏ꍇ�̓R�s�[�����Ȃ�����
			if (vtxUsed[pv]){
				if (gmd.texCoords[pv] != texCoord || 
					(grFemMesh->normals.size() && gmd.normals[pv] != normal)){	
					//	���_�̃R�s�[�̍쐬
					gmd.vertices.push_back(gmd.vertices[pv]);
					gmd.texCoords.push_back(texCoord);
					if (gmd.normals.size()) gmd.normals.push_back(normal);
					gmd.faces[pf].indices[i] = (int)gmd.vertices.size()-1;
					vertexIdMap.push_back(phFemMesh->surfaceVertices[pv]);
				}
			}else{	//	�����łȂ���΁A���ڑ��
				gmd.texCoords[pv] = texCoord;
				if (gmd.normals.size()) gmd.normals[pv] = normal; 
				vertexIdMap[pv] = phFemMesh->surfaceVertices[pv];
				vtxUsed[pv] = true;
			}
		}
	}
#if 0	
	// ���_�J���[�̐ݒ�
	gmd.colors.resize(grFemMesh->colors.size() ? vertexIdMap.size() : 0);
	for(unsigned pv=0; pv<gmd.colors.size(); ++pv){
		gmd.colors[pv] = grFemMesh->colors[vertexIdMap[pv]];
	}
#endif
	//	GRMesh���쐬
	GRMesh* rv = grFemMesh->GetNameManager()->CreateObject(GRMeshIf::GetIfInfoStatic(), &gmd)->Cast();
	//	�}�e���A���̒ǉ�
	for (unsigned i=0; i<grFemMesh->NChildObject(); ++i){
		rv->AddChildObject(grFemMesh->GetChildObject(i));
	}
	//	�e�N�X�`�����[�h���R�s�[
	rv->tex3d = grFemMesh->tex3d;
	grFemMesh = rv;
}

void FWFemMeshNew::Sync(){		
	//	��������
	FWObject::Sync();
	if(syncSource == FWObjectDesc::PHYSICS){
		if(GetPHFemMesh()->GetPHFemVibration()){
			SyncVibrationInfo();
		}
		//if(phFemThermo){
		//		SyncThermoInfo();
		//}
	}
}

void FWFemMeshNew::SyncVibrationInfo(){
	if(!grFemMesh) return;
	grFemMesh->EnableAlwaysCreateBuffer();
	// ���_�ʒu�̓���
	Vec3f* grVertices = grFemMesh->GetVertices();
	for(int i = 0; i < (int)vertexIdMap.size(); i++){
		int pId = vertexIdMap[i];
		grVertices[i] = (Vec3f)phFemMesh->vertices[pId].pos;
	}
	// �ψʂŐF�ω�
	if(grFemMesh->NColors() < 1) return;
	Vec4f* vc = grFemMesh->GetColors();
	float base = 0.1f;
	float offset = 0.0001f;
	for(int i = 0; i < (int)vertexIdMap.size(); i++){
		int pId = vertexIdMap[i];
		float value = phFemMesh->GetVertexDisplacementL(pId).norm();
		//DSTR << value << std::endl;
		//DSTR << value/base + offset << std::endl;
		vc[i] = CompThermoColor(value/base + offset);
	}
}

Vec4f FWFemMeshNew::CompThermoColor(float value){
	float cos = -0.5 * std::cos( 4 * M_PI * value) + 0.5;
	Vec4f color;
	if(value > 1.0){
		// red
		color = Vec4f(1.0, 0.0, 0.0, 1.0);
	}else if(value > 3.0 / 4.0){
		// red + green
		color = Vec4f(1.0, cos, 0.0, 1.0);
	}else if(value > 2.0 / 4.0){
		// green
		color = Vec4f(cos, 1.0, 0.0, 1.0);
	}else if(value > 1.0 / 4.0){
		// green + blue
		color = Vec4f(0.0, 1.0, cos, 1.0);
	}else if(value > 0.0){
		// blue
		color = Vec4f(0.0, 0.0, 1.0, 1.0);
	}
	return color;
}


}
