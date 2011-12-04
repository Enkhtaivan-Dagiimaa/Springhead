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

	//negitest
	//�ł��e�N�X�`���̖���
	unsigned kogetex =5;
	//�T�[���e�N�X�`���̖���
	unsigned thtex = 5;
	//�����e�N�X�`���̖���
	unsigned watex = 2;
	
	double dtex =(double) 1.0 / ( kogetex + thtex + watex);		//	�e�N�X�`�����s���W�̑w�Ԋu
	double texstart = dtex /2.0;								//	�e�N�X�`�����W�̏����l = �ł��e�N�X�`���̃X�^�[�g���W
	double thstart = texstart + kogetex * dtex ;				//	�T�[���̃e�N�X�`���̃X�^�[�g���W
	double wastart = texstart + kogetex * dtex + thtex * dtex;	//	�����ʕ\���e�N�X�`���̃X�^�[�g���W

	//	50�x����:�e�N�X�`���̐[���v�Z(0~50)	( vertices.temp - 50.0 ) * dtex / 50.0
	//	50�x����:�e�N�X�`���̐[���v�Z(50~100)	( vertices.temp - 100.0 ) * dtex / 50.0
	//	50�x����:�e�N�X�`���̐[���v�Z(100~150)	( vertices.temp - 150.0 ) * dtex / 50.0
	//	����𖞂����悤�ɁA50,100,150�x�Ȃǂ�ϐ��ɂ��Ă��悢�B���A���ɗ��p���Ȃ����A��ڂł킩��₷���̂ŁA���̂܂܂ł��������B
	//	50�x���݂��Ƃ�dtex�����Z�����ɁAgvtx[stride*gv + tex + 2] = (temp - 50.0 ) * dtex / 50.0 + thstart;�����ł��̂���

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
						//ph����ߓ_�̉��x������Ă���
				//PHFemMeshThermo�̊e�ߓ_�̉��x������Ă���B
				//���x�̒l�ɉ����āA���̌W����p���āA�e�N�X�`�����W���v�Z����
				//	value = phMesh�̔h���N���X->thermo[pv];
				//	GR�̃e�N�X�`�����W�Ƃ��Đݒ肷��B	s t r q �� r��ݒ�
				//gvtx[stride*gv + tex + 2] = value + gvtx[stride*gv];	//	gvtx[stride*gv]�ŏꏊ�ɂ���ĈႤ�[�x���E���Ă��邱�Ƃ�
				//gvtx[stride*gv + tex + 2] = 0.1 + value;
				////gvtx[stride*gv + tex + 2] = thstart;
//				gvtx[stride*gv + tex + 2] = thstart;

				//	�ǂ̃e�N�X�`���ɂ��邩�̏�����������
				//	���O�̃e�N�X�`�����W��ۑ����Ă����B�Ȃ���΁A�����l��ݒ�
				//	�e�N�X�`���̕\�����[�h��؂�ւ���SW���L�[�{�[�h����؂�ւ���SampleApp.h��AddHotkey�AAddAction�����������
				
				//	CADThermo�̊Y�������̃\�[�X
				//if(tvtxs[j].temp <= tvtxs[j].temp5){		//tvtxs[j].wmass > wmass * ratio1
				//	texz	= texbegin;
				//	double texznew =diff * grad + texz;//����,�e�N�X�`�����W�̏����l
				//	////�O�̃e�N�X�`�������W��������̌v�Z�l���[��������A���̌v�Z���ʂ𔽉f������
				//	if(tvtxs[j].tex1memo <= texznew){			//���߂͂��̏������Ȃ��Ă���������ǁA��x���x���オ���āA��߂Ă���͕K�v�ɂȂ�
				//		tvtxs[j].SetTexZ(tratio * dl + texz);	//�e�N�X�`����Z���W�����肷��B//�\���e�N�X�`���͂��̐��`�a��\��������̂ŁAZ���W�����`�a�ŕ\������̂ŁA���`�a�̌v�Z���g�p
				//		tvtxs[j].tex1memo = tratio * dl + texz;	//tex1memo���X�V����
				//	}
				//}

				double temp = phMesh->vertices[pv].temp;
				//	��O�̃e�N�X�`���H�˗�
				if( temp < 0){				//	�b��I�ɂЂƂO�̃e�N�X�`���ɂ���?�����A�F�����蓖�Ă�B��ڂł킩��F�ɁB
					gvtx[stride*gv + tex + 2] = thstart - dtex /10.0;			//thstart���O�͐^�����ł��e�N�X�`���Ȃ̂ŁA���̓K���l�B�T�[������e�N�X�`�������ꂽ��A�ʂ̐F�ɁB
				}
				//	�΁ˉ��F
				else if(temp <= 50.0 ){		//temp <0�̎��A�ǂ����悤���H	0.0 < temp	�̏����𖳂�����
					double yellow = temp * dtex / 50.0 + thstart;
					gvtx[stride*gv + tex + 2] = temp * dtex / 50.0 + thstart;
				}
				//	���F�˃I�����W
				else if(50.0 < temp && temp <= 100.0){
					gvtx[stride*gv + tex + 2] = (temp - 50.0 ) * dtex /	 50.0 + thstart;
				}
				//	�I�����W�ː�
				else if(100.0 < temp && temp <= 150.0){
					gvtx[stride*gv + tex + 2] = (temp - 50.0 ) * dtex / 50.0 + thstart;
				}
				//	�ԁ˃s���N
				else if(150.0 < temp && temp <= 200.0){
					double pinkc = (temp - 50.0 ) * dtex / 50.0 + thstart ;
					gvtx[stride*gv + tex + 2] = (temp - 50.0 ) * dtex / 50.0 + thstart ;
				}
				//	�s���N	:�F�Œ�
				else if(200.0 < temp){
					gvtx[stride*gv + tex + 2] = dtex * 4.0 + thstart;
				}
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
	sprTetgen(nVtxsOut, vtxsOut, nTetsOut, tetsOut, grMesh->vertices.size(), &vtxsIn[0], grMesh->faces.size(), &grMesh->faces[0], "pq1.2a0.3");//a0.003
	
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
			GRMeshFace& gFaceNormal = grMesh->faceNormals[pFaceMap[pf]];
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
					if (weight[j] <= -0.001){
						DSTR << "�O���t�B�N�X��3���_\t"; 
						for(unsigned k=0; k<3; ++k){
							DSTR << grMesh->vertices[gFace.indices[k]] << "\t";
						}
						DSTR << "�̊O���ɕ����̒��_:\t" << gmd.vertices[pv] << "������܂�" << std::endl;
					}
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
					if (weight[maxId][j] <= -0.001){
						DSTR << "�O���t�B�N�X��3���_\t"; 
						for(unsigned k=0; k<3; ++k){
							DSTR << grMesh->vertices[gFace.indices[k]] << "\t";
						}
						DSTR << "�̊O���ɕ����̒��_:\t" << gmd.vertices[pv] << "������܂�" << std::endl;
					}
					texCoord += weight[maxId][j] * grMesh->texCoords[gFace.indices[j<maxId?j:j+1]];
					if(gNormal){
						normal += weight[maxId][j] * grMesh->normals[gFaceNormal.indices[j<maxId?j:j+1]];
					}
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
