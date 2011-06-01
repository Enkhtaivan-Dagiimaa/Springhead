#include "Framework.h"
#include <Framework/SprFWFemMesh.h>
#include <Graphics/GRFrame.h>
#include <Physics/PHConstraint.h>
#include <Foundation/UTLoadHandler.h>
#include "FWFemMesh.h"
#include "tetgen.h"
#include "ThermalFEM.h"


#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

using namespace std;
using namespace Spr;

namespace Spr{;

FWFemMesh::FWFemMesh(const FWFemMeshDesc& d):grMesh(NULL){
	SetDesc(&d);
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
	if (!phMesh) GeneratePHFemMesh();
	if (grFrame){
		grFrame->DelChildObject(grMesh->Cast());
		grFrame->AddChildObject(CreateGRFromPH()->Cast());
	}
}
bool FWFemMesh::GeneratePHFemMesh(){
	//	�Ăяo���ꂽ���_�� grMesh �ɃO���t�B�N�X�̃��b�V���������Ă���
	//	grMesh��ϊ����āAphMesh������B
	std::cout << "���b�V������" << std::endl;
	
	//	�ȉ��ŁAgrMesh����tetgen���Ăяo���ĕϊ����āApmd�ɒl�����Ă����Ηǂ��B


	PHFemMeshDesc pmd;
	
	//TetGen�Ŏl�ʑ̃��b�V����


	//��`�������Ȃ���ϊ����Ă���
	//int i;
	//���_�̊J�n�ԍ�
	ThermalFEM FEM;
	tetgenio::facet *f;
	tetgenio::polygon *p;

	FEM.in.firstnumber = 0;
	////���_���W�Ɛ��̓���
	FEM.in.numberofpoints = grMesh->NVertex();		//grMesh�̒��_�T�C�Y�̑��
	FEM.in.pointlist = DBG_NEW REAL[FEM.in.numberofpoints * 3];
	for(int j=0; j < grMesh->NVertex(); j++){	//ThermoMesh�̒��_�ԍ���j / 3�̐�������
		for(int k=0; k<3; ++k)
			FEM.in.pointlist[j*3+k] = grMesh->GetVertices()[j][k] ;
	}
	//for(int j=0; j < grMesh->NVertex(); j++){	//ThermoMesh�̒��_�ԍ���j / 3�̐�������
	//	for(int k=0; k<3; ++k){
	//		DSTR << FEM.in.pointlist[j*3+k];
	//	}
	//	DSTR << std::endl;
	//}
	//-0.50.50.5
	//0.50.50.5
	//0.5-0.50.5
	//-0.5-0.50.5
	//0.50.5-0.5
	//0.5-0.5-0.5
	//-0.50.5-0.5
	//-0.5-0.5-0.5

	////�ʂ̐��̑��
	FEM.in.numberoffacets = grMesh->NFace();
	FEM.in.facetlist = DBG_NEW tetgenio::facet[FEM.in.numberoffacets];
	FEM.in.facetmarkerlist = DBG_NEW int[FEM.in.numberoffacets];
	////�ʂ̏��̓���
	GRMeshFace* faces = grMesh->GetFaces();
	for(int j =0; j < FEM.in.numberoffacets ; j++){
		f = &FEM.in.facetlist[(int)j];
		f->numberofpolygons = 1;
		f->polygonlist = DBG_NEW tetgenio::polygon[f->numberofpolygons];
		f->numberofholes = 0;
		f->holelist = NULL;
		p = &f->polygonlist[0];
		p->numberofvertices = faces[j].nVertices;
		p->vertexlist = DBG_NEW int[p->numberofvertices];
		for(int k =0; k < p->numberofvertices; k++){
			p->vertexlist[k] = faces[j].indices[k];
		}
	}
	for(int j = 0; j < FEM.in.numberoffacets ;j++){
		FEM.in.facetmarkerlist[j] = 0;
	}
	//FEM.in.save_nodes("barpq1.4a1.0in");
	//FEM.in.save_poly("barpq1.4a1.0in");
	//FEM.in.save_elements("barpq1.4a0.5in");
	////�l�ʑ̃��b�V����
	//FEM.TFEMTetrahedralize("pqa100");
	////���b�V���������t�@�C���̕ۑ�
	//FEM.out.save_nodes("barpq1.4a0.01out");			
	//FEM.out.save_elements("barpqa100out");
	//FEM.out.save_faces("barpqa0.5out");
	//return FEM.out�ɓ����Ă��郁�b�V���t�@�C���˂����PH�ɓ����
	FEM.in.save_nodes("barpqin");
	FEM.in.save_poly("barpqain");
	FEM.in.save_elements("barpqain");

	FEM.TFEMTetrahedralize("pq1.1a0.0001");
	
	FEM.out.save_nodes("spherepq1.1a0.0001out");			
	FEM.out.save_elements("spherepq1.1a0.0001out");
	FEM.out.save_faces("spherepq1.1a0.0001out");

	
	//pmd�ɒl�����Ă���
	for(int i=0; i < FEM.out.numberofpoints; i++){
		pmd.vertices.push_back(Vec3d(FEM.out.pointlist+i*3));
	} 
	pmd.tets.assign(FEM.out.tetrahedronlist, FEM.out.tetrahedronlist + FEM.out.numberoftetrahedra*4);
	
	//phMesh�ɕK�v�Ȓl�͉��H�M�v�Z�ɕK�v�ȕ�����PH�̃\���b�h�֌W�́ASPR�̂��̂��g���B
	//�M�`���̏��������������Ȃ���A�K�v�ȃp�����[�^���Adesc�ɉ����čs���B
	phMesh = DBG_NEW PHFemMesh(pmd);
	
	return true;
}

GRMesh* FWFemMesh::CreateGRFromPH(){
	//	���_�̑Ή��\��p��
	std::vector<int> vtxMap;
	vtxMap.resize(phMesh->vertices.size(), -1);
	for(unsigned i=0; i<phMesh->surfaceVertices.size(); ++i){
		vtxMap[phMesh->surfaceVertices[i]] = i;
	}
	GRMeshDesc gmd;	//	�V����GRMesh�̃f�X�N���v�^
	//	�\�ʂ̒��_��ݒ�
	for(unsigned i=0; i< phMesh->surfaceVertices.size(); ++i) 
		gmd.vertices.push_back(phMesh->vertices[ phMesh->surfaceVertices[i] ].pos);
	//	�\�ʂ̎O�p�`��ݒ�
	for(unsigned i=0; i< phMesh->surfaceFaces.size(); ++i) {
		GRMeshFace f;
		f.nVertices = 3;
		for(int j=0; j<3; ++j){
			f.indices[j] = vtxMap[phMesh->surfaceFaces[i].vertices[j]];
		}
		gmd.faces.push_back(f);
	}
	//	phMesh�̒��_�ɑΉ�����GRMesh�̎O�p�`�ƂR�_�̏d�݂Â������߂�B
	unsigned* gTris = grMesh->GetTriangles();
	Vec3f* gVtxs = grMesh->GetVertices();
	
	std::vector<Vec3f> gNormals, gWalls, gDirs;
	std::vector<double> gEdgeLens;
	gNormals.resize(grMesh->NTriangle());
	gDirs.resize(gNormals.size() * 3);
	gWalls.resize(gNormals.size() * 3);
	gEdgeLens.resize(gNormals.size() * 3);
	for(int j=0; j<grMesh->NTriangle(); ++j){
		gNormals[j] = ((gVtxs[gTris[j*3+2]] - gVtxs[gTris[j*3]]) % (gVtxs[gTris[j*3+1]] - gVtxs[gTris[j*3]])).unit();
		for(int k=0; k<3; ++k){
			gDirs[j*3+k] = (gVtxs[gTris[j*3+(k+1)%3]] - gVtxs[gTris[j*3+k]]).unit();
			gWalls[j*3+k] = (gNormals[j] % gDirs[j*3+k]).unit();
		}
	}
	std::vector<double> dists;
	std::vector<int> nearestTris;
	dists.resize(phMesh->surfaceVertices.size());
	nearestTris.resize(phMesh->surfaceVertices.size());
	for(unsigned i=0; i<phMesh->surfaceVertices.size(); ++i){
		Vec3f pos = phMesh->vertices[phMesh->surfaceVertices[i]].pos;
		double minDist = DBL_MAX;
		int minId = -1;
		for(int j=0; j<grMesh->NTriangle(); ++j){
			double h = (pos - gVtxs[gTris[j*3]]) * gNormals[j];
			Vec3f ph = pos - h*gNormals[j];
			double dist;
			int k;
			for(k=0; k<3; ++k){
				double d = (ph - gVtxs[gTris[j*3+k]]) * gWalls[j*3+k];
				if (d > 0){
					Vec3d pw = ph - d*gWalls[j*3+k];
					double dl = (pw - gVtxs[gTris[j*3]+k]) * gDirs[gTris[j*3]+k];
					if (dl < 0){
						dist = sqrt(h*h + d*d + dl*dl);
					}else if (dl > gEdgeLens[gTris[j*3]+k]){
						dl -= gEdgeLens[gTris[j*3]+k];
						dist = sqrt(h*h + d*d + dl*dl);
					}else{
						dist = sqrt(h*h + d*d);
					}
					break;
				}
			}
			if (k == 3){
				dist = h;
			}
			if (dist < minDist){
				minDist = dist;
				minId = j;
			}
		}
		dists[i] = minDist;
		nearestTris[i] = minId;
	}
	std::vector<Vec3d> weights;
	weights.resize(phMesh->surfaceVertices.size());
	for(unsigned i=0; i<phMesh->surfaceVertices.size(); ++i){
		Matrix3f vtxs;
		for(unsigned j=0; j<3; ++j){
			vtxs[j] = gVtxs[gTris[nearestTris[i]*3+j]];
		}
		int tmp[3];
		vtxs.trans().gauss(weights[i], phMesh->vertices[phMesh->surfaceVertices[i]].pos, tmp);
	}
	//	���߂��d�݂ɂ��������āA�e�N�X�`�����W�����߂�
	for(unsigned i=0; i<gmd.vertices.size(); ++i){
		gmd.texCoords.resize(gmd.vertices.size(), Vec2f());
		for(int j=0; j<3; ++j) gmd.texCoords[i] += weights[i][j] * grMesh->texCoords[gTris[nearestTris[i]*3+j]];
	}
	//	���߂��d�݂ɂ��������āA�@���x�N�g�������߂�
	if (grMesh->normals.size() == grMesh->vertices.size()){
		gmd.normals.resize(gmd.vertices.size(), Vec2f());
		for(unsigned i=0; i<gmd.vertices.size(); ++i){
			for(int j=0; j<3; ++j) gmd.normals[i] += weights[i][j] * grMesh->normals[gTris[nearestTris[i]*3+j]];
		}
	}
	//	��ԋ߂��̖ʂ̃}�e���A����ݒ肵�����B
	//	�܂��O�p�`�Ɩʂ̑Ή��\�����
	std::vector<int> gTri2FaceMap;
	for(unsigned i=0; i<grMesh->faces.size(); ++i){
		gTri2FaceMap.push_back(i);
		if (grMesh->faces[i].nVertices == 4){
			gTri2FaceMap.push_back(i);
		}
	}
	//	��ԋ߂��ʂ̃}�e���A����K�p����B
	if (!grMesh->materialList.empty())
		//	���_�Ɉ�ԋ߂��ʂ������A�����������
		for(unsigned i=0; i<gmd.faces.size(); ++i){
			int nearestFace[3];
			for(unsigned j=0; j<3; ++j) nearestFace[j] = gTri2FaceMap[nearestTris[gmd.faces[i].indices[j]]];
			if (nearestFace[1] == nearestFace[2]) gmd.materialList.push_back(grMesh->materialList[nearestFace[1]]);
			else gmd.materialList.push_back(grMesh->materialList[nearestFace[0]]);
		}
	//	GRMesh���쐬
	GRMesh* rv = grMesh->GetNameManager()->CreateObject(GRMeshIf::GetIfInfoStatic(), &gmd)->Cast();
	//	�}�e���A���̒ǉ�
	for (unsigned i=0; i<grMesh->NChildObject(); ++i){
		rv->AddChildObject(grMesh->GetChildObject(i));
	}
	//	�e�N�X�`�����[�h���R�s�[
	rv->tex3d = grMesh->tex3d;
	return rv;
}

}
