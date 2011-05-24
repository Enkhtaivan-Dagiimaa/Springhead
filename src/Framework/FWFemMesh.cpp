#include "Framework.h"
#include <Framework/SprFWFemMesh.h>
#include <Graphics/GRFrame.h>
#include <Physics/PHConstraint.h>
#include "FWFemMesh.h"
//#include "../Samples/Physics/FEMThermo/tetgen.h"
//#include "tetgen.h"
//#include "../Samples/Physics/FEMThermo/ThermalFEM.h"





#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

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
#if 0
	in.firstnumber = 1;
	////���_���W�Ɛ��̓���
	in.numberofpoints = grMesh->NVertex();		//grMesh�̒��_�T�C�Y�̑��
	in.pointlist = new REAL[in.numberofpoints * 3];
	for(int j=0; j < grMesh->NVertex(); j++){	//ThermoMesh�̒��_�ԍ���j / 3�̐�������
		for(int k=0; k<3; ++k)
			in.pointlist[j*3+k] = grMesh->GetVertices()[j][k];
	}

	////�ʂ̐��̑��
	in.numberoffacets = grMesh->NFace() / 3;		//faces.size()/3; /3�ɂ��Ă���̂́A�Ȃ��Ȃ̂��H
	in.facetlist = new tetgenio::facet[in.numberoffacets];
/*	in.numberoffacets
	in.facetlist[in.numberoffacets];*/				//	tetgenio::facet 
	in.facetmarkerlist = new int[in.numberoffacets];
	////�ʂ̏��̓���
	for(int j =0; j < in.numberoffacets ; j++){
		f = &FEM.in.facetlist[(int)j];
		f->numberofpolygons = 1;
		f->polygonlist = new tetgenio::polygon[f->numberofpolygons];
		f->numberofholes = 0;
		f->holelist = NULL;
		p = &f->polygonlist[0];
		p->numberofvertices = 3;
		p->vertexlist = new int[p->numberofvertices];
		for(int k =0; k < 3 ; k++){
			p->vertexlist[k] = grMesh->GetFaces()[3 * j + k]+1;
		}
	}
	for(int j = 0; j < in.numberoffacets ;j++){
		in.facetmarkerlist[j] = 0;
	}
	in.save_nodes("barin");
	in.save_poly("barin");
	////�l�ʑ̃��b�V����
	//FEM.TFEMTetrahedralize("pq100");
	////���b�V���������t�@�C���̕ۑ�
	//FEM.out.save_nodes("bar100out");			
	//FEM.out.save_elements("bar100out");
	//FEM.out.save_faces("bar100out");
	//return FEM.out�ɓ����Ă��郁�b�V���t�@�C���˂����PH�ɓ����
	
	phMesh = DBG_NEW PHFemMesh(pmd);
#endif
	return true;
}


}
