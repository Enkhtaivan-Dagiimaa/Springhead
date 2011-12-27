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
	//static double value, delta;
	//if (value <= 0) delta = 0.01;
	//if (value >= 1) delta = -0.01;
	//value += delta;

	///	�e�N�X�`���Ɖ��x�A�����ʂƂ̑Ή��\�́ASamples/Physics/FEMThermo/�e�N�X�`���̐F�Ɖ��x�̑Ή�.xls	���Q�Ƃ̂���
	//negitest
	//�ł��e�N�X�`���̖���
	unsigned kogetex	= 5;
	//�T�[���e�N�X�`���̖���
	unsigned thtex		= 6;
	//�����e�N�X�`���̖���
	unsigned watex		= 2;
	
	double dtex =(double) 1.0 / ( kogetex + thtex + watex);		//	�e�N�X�`�����s���W�̑w�Ԋu
	double texstart = dtex /2.0;								//	�e�N�X�`�����W�̏����l = �ł��e�N�X�`���̃X�^�[�g���W
	double wastart = texstart + kogetex * dtex ;	//	�����ʕ\���e�N�X�`���̃X�^�[�g���W
	double thstart = texstart + kogetex * dtex + 1.0 * dtex ;				//	�T�[���̃e�N�X�`���̃X�^�[�g���W �����e�N�X�`����2���ڂ���X�^�[�g
	

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

				//�T�[������e�N�X�`�������ꂽ�ꍇ�A�e�N�X�`���̃��[�h�͕s�v�ɂȂ�̂ŁA�ȉ��̃R�[�h��ύX
				double temp = phMesh->vertices[pv].temp;
				// -50.0~0.0:aqua to blue
				if(temp <= -50.0){
					gvtx[stride*gv + tex + 2] = thstart;
				}
				else if(-50.0 < temp && temp <= 0){	
					gvtx[stride*gv + tex + 2] = (thstart ) + ((temp + 50.0) * dtex /50.0);
				}
				//	0~50.0:blue to green
				else if(0.0 < temp && temp <= 50.0 ){
					//double green = temp * dtex / 50.0 + thstart;
					gvtx[stride*gv + tex + 2] = temp * dtex / 50.0 + thstart + dtex;
				}
				//	50.0~100.0:green to yellow
				else if(50.0 < temp && temp <= 100.0){
					gvtx[stride*gv + tex + 2] = (temp - 50.0 ) * dtex /	 50.0 + thstart + 2 * dtex;
				}
				//	100.0~150:yellow to orange	
				else if(100.0 < temp && temp <= 150.0){
					gvtx[stride*gv + tex + 2] = (temp - 50.0 ) * dtex / 50.0 + thstart + 2 * dtex;
				}
				//	150~200:orange to red
				else if(150.0 < temp && temp <= 200.0){
					double pinkc = (temp - 50.0 ) * dtex / 50.0 + thstart ;
					gvtx[stride*gv + tex + 2] = (temp - 50.0 ) * dtex / 50.0 + thstart + 2 * dtex;
				}
				//	200~250:red to purple
				else if(200.0 < temp && temp <= 250.0){
					gvtx[stride*gv + tex + 2] = (temp - 50.0 ) * dtex / 50.0 + thstart + 2 * dtex;
				}
				///	250~:only purple
				else if(250.0 < temp){
					gvtx[stride*gv + tex + 2] = dtex * 6.0 + thstart;
					//gvtx[stride*gv + tex + 2] = wastart;			//white	 ///	�܂���ɂȂ����Ⴄ
				}
			}
		}	
	}else{
		assert(0);	//	not supported.
	}
}


size_t FWFemMesh::NChildObject() const{
	return FWObject::NChildObject() + (grMesh ? 1 : 0) + (phMesh ? 1 : 0);
}
ObjectIf* FWFemMesh::GetChildObject(size_t pos){
	if (pos < FWObject::NChildObject()){
		return FWObject::GetChildObject(pos);
	}else{
		pos -= FWObject::NChildObject();
	}	
	if (grMesh){
		if (pos == 0) return grMesh->Cast();
		else pos --;
	}
	if (phMesh){
		if (pos == 0) return phMesh->Cast();
		else pos --;
	}
	return NULL;
}
bool FWFemMesh::AddChildObject(ObjectIf* o){
	PHFemMesh* pm = o->Cast();
	if (pm){
		phMesh = pm;
		return true;
	}
	GRMesh* mesh = o->Cast();
	if (mesh){
		grMesh = mesh;
		return true;
	}
	return FWObject::AddChildObject(o);
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
	// swithes q+(���a/�ŒZ��) (e.g. = q1.0~2.0) a �ő�̑̐� 
	sprTetgen(nVtxsOut, vtxsOut, nTetsOut, tetsOut, grMesh->vertices.size(), &vtxsIn[0], grMesh->faces.size(), &grMesh->faces[0], "pq2.1a0.002");//a0.3 //a0.003 
	
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
struct FaceMap{
	FaceMap(){
		vtxs[0] = vtxs[1] = vtxs[2] = -1;
		face = -1;
	}
	int vtxs[3];
	int& operator [](int i) {return vtxs[i]; }
	int face;
};

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
	std::vector< FaceMap > pFaceMap(pnormals.size());
	for(unsigned pf=0; pf<pnormals.size(); ++pf){
		//	�����̕���(pf)�Ɠ��ꕽ�ʂɍڂ��Ă���O���t�B�N�X�̕��ʂ��
		std::vector<int> gfCands;
		for(unsigned gf=0; gf<gnormals.size(); ++gf){
			if (pnormals[pf] * gnormals[gf] > 0.999){	//	�@���������̂͂���
				int pv;
				for(pv=0; pv<3; ++pv){
					double d = gnormals[gf] * (gmd.vertices[gmd.faces[pf].indices[pv]] - grMesh->vertices[grMesh->faces[gf].indices[0]]);
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
				for(gv=0; gv<grMesh->faces[gf].nVertices; ++gv){
					double d = gWalls[gf].wall[gv] * (gmd.vertices[gmd.faces[pf].indices[pv]] - grMesh->vertices[grMesh->faces[gf].indices[gv]]);
					if (d < -1e-6) break;
				}
				if (gv == grMesh->faces[gf].nVertices){
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
	}
#if 0
	DSTR << "FaceMap PHtoGR:\n";
	for(unsigned i=0; i<pFaceMap.size(); ++i)
		DSTR << pFaceMap[i].face << ":" << pFaceMap[i][0] << " " << pFaceMap[i][1] << " " << pFaceMap[i][2] << ", ";
	DSTR << std::endl;
#endif

	//	�Ή��\�ɉ����ă}�e���A�����X�g��ݒ�B
	gmd.materialList.resize(grMesh->materialList.size() ? pFaceMap.size() : 0);
	for(unsigned pf=0; pf<gmd.materialList.size(); ++pf){
		gmd.materialList[pf] = grMesh->materialList[pFaceMap[pf].face];
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
			GRMeshFace& gFace = grMesh->faces[pFaceMap[pf][i]];
			GRMeshFace& gFaceNormal = grMesh->faceNormals[pFaceMap[pf].face];
			GRMeshFace* gNormal = NULL;
			if (grMesh->normals.size()){
				gNormal = &gFace;
				if (grMesh->faceNormals.size()) gNormal = &grMesh->faceNormals[pFaceMap[pf].face];
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
						DSTR << "�O���t�B�N�X��3���_�̊O���ɕ����̒��_������܂��B" << std::endl; 
						for(unsigned k=0; k<3; ++k){
							for(unsigned ax=0; ax<3; ++ax){
								DSTR << grMesh->vertices[gFace.indices[k]][ax];
								DSTR << (ax==2 ? "\n" : "\t");
							}
						}
						for(unsigned ax=0; ax<3; ++ax){
							DSTR << gmd.vertices[pv][ax];
							DSTR << (ax==2 ? "\n" : "\t");
						}
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
