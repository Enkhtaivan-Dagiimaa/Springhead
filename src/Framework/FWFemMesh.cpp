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
	//	呼び出された時点で grMesh にグラフィクスのメッシュが入っている
	//	grMeshを変換して、phMeshをつくる。
	std::cout << "メッシュ生成" << std::endl;
	
	//	以下で、grMeshからtetgenを呼び出して変換して、pmdに値を入れていけば良い。


	PHFemMeshDesc pmd;
	
	//TetGenで四面体メッシュ化


	//定義を加えながら変換していく
	//int i;
	//頂点の開始番号
	ThermalFEM FEM;
	tetgenio::facet *f;
	tetgenio::polygon *p;

	FEM.in.firstnumber = 1;
	////頂点座標と数の入力
	FEM.in.numberofpoints = grMesh->NVertex();		//grMeshの頂点サイズの代入
	FEM.in.pointlist = DBG_NEW REAL[FEM.in.numberofpoints * 3];
	for(int j=0; j < grMesh->NVertex(); j++){	//ThermoMeshの頂点番号はj / 3の整数部分
		for(int k=0; k<3; ++k)
			FEM.in.pointlist[j*3+k] = grMesh->GetVertices()[j][k] ;
	}
	//for(int j=0; j < grMesh->NVertex(); j++){	//ThermoMeshの頂点番号はj / 3の整数部分
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

	////面の数の代入
	FEM.in.numberoffacets = grMesh->NFace();
	FEM.in.facetlist = DBG_NEW tetgenio::facet[FEM.in.numberoffacets];
	FEM.in.facetmarkerlist = DBG_NEW int[FEM.in.numberoffacets];
	////面の情報の入力
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
	////四面体メッシュ化
	//FEM.TFEMTetrahedralize("pqa100");
	////メッシュ化したファイルの保存
	//FEM.out.save_nodes("barpq1.4a0.01out");			
	//FEM.out.save_elements("barpqa100out");
	//FEM.out.save_faces("barpqa0.5out");
	//return FEM.outに入っているメッシュファイル⇒これをPHに入れる
	FEM.in.save_nodes("barpqin");
	FEM.in.save_poly("barpqain");
	FEM.in.save_elements("barpqain");

	FEM.TFEMTetrahedralize("pqa");
	
	FEM.out.save_nodes("barpqaout");			
	FEM.out.save_elements("barpqaout");
	FEM.out.save_faces("barpqaout");

	
	//pmdに値を入れていく
	for(int i=0; i < FEM.out.numberofpoints; i++){
		pmd.vertices.push_back(Vec3d(FEM.out.pointlist+i*3));
	} 
	pmd.tets.assign(FEM.out.tetrahedronlist, FEM.out.tetrahedronlist + FEM.out.numberoftetrahedra*4);
	
	//phMeshに必要な値は何？熱計算に必要な物理量PHのソリッド関係は、SPRのものを使う。
	//熱伝導の処理を書き加えながら、必要なパラメータを、descに加えて行く。
	phMesh = DBG_NEW PHFemMesh(pmd);
	
	//テクスチャの処理は、FEなどで行えばよい。
	return true;
}


}
