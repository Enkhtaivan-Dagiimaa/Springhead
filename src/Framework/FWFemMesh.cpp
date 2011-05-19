#include "Framework.h"
#include <Framework/SprFWFemMesh.h>
#include <Graphics/GRFrame.h>
#include <Physics/PHConstraint.h>
#include "FWFemMesh.h"
#include "../Samples/Physics/FEMThermo/tetgen.h"
#include "../Samples/Physics/FEMThermo/ThermalFEM.h"





#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

namespace Spr{;

FWFemMesh::FWFemMesh(const FWFemMeshDesc& d):FWFemMeshDesc(d), grMesh(NULL){
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
		GRMesh* grMesh = (GRMesh*)o;
		//
		GRFrameIf* grf = GetGRFrame();
		//
		//	tetgen�Ƃ������PH�����			//�������ɋL�q���鏈���Ȃ̂��H
		//grMesh��x�t�@�C��������
		//Tetgen�ɓ����
		//PHMesh = IntoTetGen(grf->Cast());							//TetGen�g���Ȃ�AGRThermoMesh.cpp�ŋL�q�����������s���֐������B
		IntoTetGen(grf->Cast());
		//
		//Tetrahedralize()���Ăł����t�@�C�����APH��vector���͗L���̔z��ɓ����B
		return true;
	}else{
		return FWObject::AddChildObject(o);
	}
}
bool FWFemMesh::IntoTetGen(GRMesh* grm){
	
	//��`�������Ȃ���ϊ����Ă���
	tetgenio::facet *f;
	tetgenio::polygon *p;
	int i;
	//���_�̊J�n�ԍ�
	//FEM.in.firstnumber = 1;
	////���_���W�Ɛ��̓���
	//FEM.in.numberofpoints = tvtxs.size();//���_�̃T�C�Y�̑��
	//FEM.in.pointlist = new REAL[FEM.in.numberofpoints * 3];
	//for(unsigned int j=0; j < tvtxs.size(); j++){	//ThermoMesh�̒��_�ԍ���j / 3�̐�������
	//	Vec3d rv;			//���_�̈ʒu���W
	//	rv = tvtxs[j].GetPos(1);	
	//	DSTR << "rv ["<<j<<"]"<< rv <<endl;
	//	FEM.in.pointlist[3*j] = rv.x;	//input x coordinate	// thermovertex�̊e���W�����������Ă�����悤�Ƀ����o�ϐ��ɒǉ�����			
	//	FEM.in.pointlist[3*j+1] = rv.y;	//input y coordinate
	//	FEM.in.pointlist[3*j+2] = rv.z;	//input z coordinate
	//	//DSTR<<j<<"\t"<<FEM.in.pointlist[3*j]<<"\t"<<FEM.in.pointlist[3*j+1]<<"\t"<<FEM.in.pointlist[3*j+2]<<endl;		
	//}
	//DSTR<<"tvtxs.size() *3: " << tvtxs.size() *3 <<endl;
	////�ʂ̐��̑��
	//FEM.in.numberoffacets = faces.size()/3;
	//FEM.in.facetlist = new tetgenio::facet[FEM.in.numberoffacets];
	//FEM.in.facetmarkerlist = new int[FEM.in.numberoffacets];
	////�ʂ̏��̓���
	//for(vector<int>::size_type i =0; i < faces.size() / 3; i++){
	//	f = &FEM.in.facetlist[(int)i];
	//	f->numberofpolygons = 1;
	//	f->polygonlist = new tetgenio::polygon[f->numberofpolygons];
	//	f->numberofholes = 0;
	//	f->holelist = NULL;
	//	p = &f->polygonlist[0];
	//	p->numberofvertices = 3;
	//	p->vertexlist = new int[p->numberofvertices];
	//	p->vertexlist[0] = faces[3 * i]+1;
	//	p->vertexlist[1] = faces[3 * i + 1]+1;
	//	p->vertexlist[2] = faces[3 * i + 2]+1;
	//}
	//for(vector<int>::size_type i = 0; i < faces.size() /3 ;i++){
	//	FEM.in.facetmarkerlist[i] = 0;
	//}
	//FEM.in.save_nodes("barin");
	//FEM.in.save_poly("barin");
	////�l�ʑ̃��b�V����
	//FEM.TFEMTetrahedralize("pq100");
	////���b�V���������t�@�C���̕ۑ�
	//FEM.out.save_nodes("bar100out");			
	//FEM.out.save_elements("bar100out");
	//FEM.out.save_faces("bar100out");
	//return FEM.out�ɓ����Ă��郁�b�V���t�@�C���˂����PH�ɓ����
	std::cout << "���b�V��GET����?" << std::endl;
	return true;
}


}
