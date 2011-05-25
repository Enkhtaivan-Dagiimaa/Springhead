#include "Framework.h"
#include <Framework/SprFWFemMesh.h>
#include <Graphics/GRFrame.h>
#include <Physics/PHConstraint.h>
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
	if (DCAST(GRMesh, o)){
		grMesh = (GRMesh*)o;
		if (!phMesh) GeneratePHFemMesh();
		return true;
	}else{
		return FWObject::AddChildObject(o);
	}
}
bool FWFemMesh::GeneratePHFemMesh(){
	//	�Ăяo���ꂽ���_�� grMesh �ɃO���t�B�N�X�̃��b�V���������Ă���
	//	grMesh��ϊ����āAphMesh������B
	std::cout << "���b�V������" << std::endl;
	
	PHFemMeshDesc pmd;
	

	
	//	�ȉ��ŁAgrMesh����tetgen���Ăяo���ĕϊ����āApmd�ɒl�����Ă����Ηǂ��B
	//	PHFemMeshDesc�ɂ́A�L���v�f�@�̌v�Z�ɕK�v�ȃ��b�V���̏�������Ηǂ��B
	
	//	�ȉ��A����Ă��炦�܂����H PHFemMeshDesc �ɕK�v�ɉ����ă����o�������Ă��������B
	//  ���܂��B



	//��`�������Ȃ���ϊ����Ă���
	//int i;
	//���_�̊J�n�ԍ�
	ThermalFEM FEM;
	tetgenio::facet *f;
	tetgenio::polygon *p;

	FEM.in.firstnumber = 1;
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
			p->vertexlist[k] = faces[j].indices[k] + 1;
		}
	}
	for(int j = 0; j < FEM.in.numberoffacets ;j++){
		FEM.in.facetmarkerlist[j] = 0;
	}
	//FEM.in.save_nodes("barpq1.4a1.0in");
	//FEM.in.save_poly("barpq1.4a1.0in");
	//FEM.in.save_elements("barpq1.4a0.5in");
	////�l�ʑ̃��b�V����
	FEM.TFEMTetrahedralize("pqa");
	////���b�V���������t�@�C���̕ۑ�
	//FEM.out.save_nodes("barpq1.4a0.01out");			
	//FEM.out.save_elements("barpqa0.0001out");
	//FEM.out.save_faces("barpqa0.5out");
	//return FEM.out�ɓ����Ă��郁�b�V���t�@�C���˂����PH�ɓ����
	
	phMesh = DBG_NEW PHFemMesh(pmd);
	return true;
}


}
