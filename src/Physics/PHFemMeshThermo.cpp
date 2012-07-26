/*
 *  Copyright (c) 2003 - 2011, Fumihiro Kato, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include <SciLab/SprSciLab.h>
#include <Physics/PHFemMeshThermo.h>
#include <Base/Affine.h>
//#include <Framework/FWObject.h>
//#include <Framework/sprFWObject.h>

#include <GL/glew.h>
#include <GL/glut.h>

#include "windows.h"

using namespace PTM;

namespace Spr{;

#define UseMatAll
//#define DEBUG
//#define DumK



PHFemMeshThermoDesc::PHFemMeshThermoDesc(){
	Init();
}
void PHFemMeshThermoDesc::Init(){
	thConduct = 0.574;
	rho = 970;
	heatTrans = 25;
	specificHeat = 0.196;		//1960
}

///////////////////////////////////////////////////////////////////
//	PHFemMeshThermo

PHFemMeshThermo::PHFemMeshThermo(const PHFemMeshThermoDesc& desc, SceneIf* s){
	deformed = true;			//�ϐ��̏������A�`�󂪕ς�������ǂ���
	SetDesc(&desc);
	if (s){ SetScene(s); }
	StepCount =0;				// �X�e�b�v���J�E���^
	StepCount_ =0;				// �X�e�b�v���J�E���^
	//phFloor =  DCAST(FWObjectIf, GetSdk()->GetScene()->FindObject("fwPan"));
	//GetFramePosition();
}

void PHFemMeshThermo::CalcVtxDisFromOrigin(){
	//>	nSurface�̓��Ax,z���W���狗�������߂�sqrt(2��a)�A�����FemVertex�Ɋi�[����
	//> ���S�~�n�̌v�Z�ɗ��p����@distance from origin
	
	/// ����t���O�̏�����
	for(unsigned i=0; i<nSurfaceFace; i++){
		faces[i].mayIHheated = false;
	}
	/// ������
	for(unsigned i =0;i<vertices.size();i++){
		vertices[i].disFromOrigin =0.0;
	}

	/// debug
	//DSTR << "faces.size(): " << faces.size() << std::endl;

	//> �\��face�̓��A���_����eface�̐ߓ_�̃��[�J��(x,z)���W�n�ł̕��ʏ�̋����̌v�Z���Aface�̑S�ߓ_��y���W�����̂��̂ɑ΂��āAIH���M�̉\���������t���O��ݒ�
	for(unsigned i=0;i<nSurfaceFace;i++){
		//> �\�ʂ�face�̑S�ߓ_��y���W�����Ȃ�΁A����face��IH���M��face�ʂƔ��肵�A�t���O��^����
		if(vertices[faces[i].vertices[0]].pos.y < 0.0 && vertices[faces[i].vertices[1]].pos.y < 0.0 && vertices[faces[i].vertices[2]].pos.y < 0.0){
			faces[i].mayIHheated = true;
			//	(x,z)���ʂɂ�����mayIHheated��face�S�ߓ_�̌��_����̋������v�Z����
			for(unsigned j=0; j<3; j++){
				vertices[faces[i].vertices[j]].disFromOrigin = sqrt(vertices[faces[i].vertices[j]].pos.x * vertices[faces[i].vertices[j]].pos.x + vertices[faces[i].vertices[j]].pos.z * vertices[faces[i].vertices[j]].pos.z);
			}
		}
	}

	//	debug		//>	�������Ή����ɂ̓R�����g�A�E�g����
	//>	���W�l���m�F����
	for(unsigned i=0; i < nSurfaceFace; i++){
		if(faces[i].mayIHheated){
			/// 3�̒��_�̑g�݁��ӂ�x,z�������ŁAy���W�������قȂ�_�̑g�݂��Ȃ����Ƃ��m�F����
			for(unsigned j=0;j<3;j++){
				if(vertices[faces[i].vertices[j]].pos.x == vertices[faces[i].vertices[(j+1)%3]].pos.x
					&& vertices[faces[i].vertices[j]].pos.z == vertices[faces[i].vertices[(j+1)%3]].pos.z
					 && vertices[faces[i].vertices[j]].pos.y != vertices[faces[i].vertices[(j+1)%3]].pos.y){
					DSTR <<i << "th: " << vertices[faces[i].vertices[j]].pos << " : " << vertices[faces[i].vertices[(j+1)%3]].pos << " : " << vertices[faces[i].vertices[(j+2)%3]].pos<<std::endl;
					DSTR << "error" << std::endl;
					assert(0);
				}
			}
		}
	}

	int kadoon =0;
	/// debug	
	//unsigned katoonNum =0;
	//for(unsigned i=0;i < nSurfaceFace;i++){
	//	if(faces[i].mayIHheated){katoonNum +=1;}
	//}
	//DSTR << "number of faces.mayIHheated: " << katoonNum << std::endl;		///		174/980(nSurfaceFace)


	int debughensu = 0;
}

void PHFemMeshThermo::SetThermalBoundaryCondition(){
	
}

void PHFemMeshThermo::CreateMatKAll(){

}

void PHFemMeshThermo::CreateMatCAll(){

}

void PHFemMeshThermo::ScilabTest(){
	if (!ScilabStart()) std::cout << "Error : ScilabStart \n";

	//	�s��̓ǂݏ���
	Matrix2f A;
	A.Ex() = Vec2f(1,2);
	A.Ey() = Vec2f(3,4);
	std::cout << "A:  " << A ;
	std::cout << "A00:" << A[0][0] << std::endl;
	std::cout << "A01:" << A[0][1] << std::endl;
	std::cout << "A10:" << A[1][0] << std::endl;
	std::cout << "A11:" << A[1][1] << std::endl;
	
	ScilabSetMatrix("A", A);
	ScilabJob("b=[4;5]");
	std::cout << "A=";
	ScilabJob("disp(A);");
	std::cout << "b=";
	ScilabJob("disp(b);");
	std::cout << "x=A\\b" << std::endl;
	ScilabJob("A,b,x=A\\b;");
	
	ScilabGetMatrix(A, "A");
	SCMatrix b = ScilabMatrix("b");
	SCMatrix x = ScilabMatrix("x");
	std::cout << "x:" << x << std::endl;

	ScilabJob("y = A;");
	SCMatrix y = ScilabMatrix("y");
	std::cout << "y=" << y;
	y = 2*A;
	std::cout << "y = 2*A is done by C++ code" << std::endl;
	std::cout << "y=";
	ScilabJob("disp(y);");
	std::cout << A;
	std::cout << y;
	ScilabJob("clear;");

	//	�O���t�`��
	ScilabJob("t = 0:0.01:2*3.141592653;");
	ScilabJob("x = sin(t);");
	ScilabJob("y = cos(t);");
	ScilabJob("plot2d(x, y);");
	for(int i=0; i<100000; ++i){
		ScilabJob("");
	}
//	ScilabEnd();
}

void PHFemMeshThermo::UsingFixedTempBoundaryCondition(unsigned id,double temp){
	//���x�Œ苫�E����
	SetVertexTemp(id,temp);
	//for(unsigned i =0;i < vertices.size()/3; i++){
	//	SetVertexTemp(i,temp);
	//}
}

void PHFemMeshThermo::UsingHeatTransferBoundaryCondition(unsigned id,double temp,double heatTransRatio){
	//�M�`�B���E����
	//�ߓ_�̎��͗��̉��x�̐ݒ�(K,C,F�Ȃǂ̍s��x�N�g���̍쐬��Ɏ��s�K�v����)
//	if(vertices[id].Tc != temp){					//�X�V����ߓ_��Tc���ω������������ATc��F�x�N�g�����X�V����
		SetLocalFluidTemp(id,temp);
		vertices[id].heatTransRatio = heatTransRatio;
		//�M�`�B���E�������g����悤�ɁA����B				///	�{�����ăx�N�g��������Ă���̂ŁA���̃R�[�h�ł́A�]�v�ɑ����Ă��܂��Ă��āA�������s������Ȃ��B
		//for(unsigned i =0;i < vertices[id].tets.size();i++){
		//	CreateVecfLocal(tets[vertices[id].tets[i]]);		//	Tc���܂ރx�N�g�����X�V����
		//}

		///	�M�`�B�����܂ލ�(K2,f3)�̂ݍČv�Z
		InitCreateVecf_();				///	�ύX����K�v�̂��鍀�̂݁A���ꕨ��������
		InitCreateMatk_();
		for(unsigned i =0; i < edges.size();i++){
			edges[i].k = 0.0;
		}
		for(unsigned i=0; i< this->tets.size();i++){
			//DSTR << "this->tets.size(): " << this->tets.size() <<std::endl;			//12
			CreateVecfLocal(i);				///	VecF�̍č쐬
			CreateMatkLocal(i);				///	MatK2�̍č쐬 ��if(deformed==true){matk1�𐶐�}		matK1��matk1�̕ϐ��ɓ���Ă����āAmatk2�����A����āA���Z
			//DSTR <<"tets["<< i << "]: " << std::endl;
			//DSTR << "this->tets[i].matk1: " <<std::endl;
			//DSTR << this->tets[i].matk1  <<std::endl;
			//DSTR << "this->tets[i].matk2: " <<std::endl;
			//DSTR << this->tets[i].matk2  <<std::endl;
			//
			//DSTR << "dMatCAll: " << dMatCAll << std::endl;
			int hogehogehoge =0;
		}
//	}
		///	�ߓ_�̑�����ʂ�alphaUpdated��true�ɂ���
		for(unsigned i=0;i<vertices[id].faces.size();i++){
			faces[vertices[id].faces[i]].alphaUpdated = true;
			alphaUpdated = true;
		}
}

void PHFemMeshThermo::UsingHeatTransferBoundaryCondition(unsigned id,double temp){
	//�M�`�B���E����
	//�ߓ_�̎��͗��̉��x�̐ݒ�(K,C,F�Ȃǂ̍s��x�N�g���̍쐬��Ɏ��s�K�v����)
//	if(vertices[id].Tc != temp){					//�X�V����ߓ_��Tc���ω������������ATc��F�x�N�g�����X�V����
		SetLocalFluidTemp(id,temp);
		//�M�`�B���E�������g����悤�ɁA����B
		//for(unsigned i =0;i < vertices[id].tets.size();i++){
		//	CreateVecfLocal(tets[vertices[id].tets[i]]);		//	Tc���܂ރx�N�g�����X�V����
		//}
		InitCreateVecf_();
		for(unsigned i=0; i < this->tets.size();i++){
			CreateVecfLocal(i);				///	VeecF�̍č쐬
													///	MatK2�̍č쐬��matK1��matk1�̕ϐ��ɓ���Ă����āAmatk2�����A����āA���Z
		}
//	}
}

void PHFemMeshThermo::SetRohSpheat(double r,double Spheat){
	//> ���x�A��M of ���b�V���̃O���[�o���ϐ�(=���b�V���ŗL�̒l)���X�V
	rho = r;
	specificHeat = Spheat;
}

std::vector<Vec2d> PHFemMeshThermo::CalcIntersectionPoint2(unsigned id0,unsigned id1,double r,double R){
	//	2�_��ʂ钼����1��	2�̒萔�����߂�
	double constA = 0.0;
	double constB = 0.0;
	///	r�ƌ�_
	double constX1 = 0.0;
	double constX1_ = 0.0;
	double constY1 = 0.0;
	///	R�ƌ�_
	double constX2 = 0.0;
	double constX2_ = 0.0;
	double constY2 = 0.0;

	//> �����̑�֏���	�֐��������Ƃ��ɁA�ϊ����遫
	unsigned vtxId0 = id0;
	unsigned vtxId1 = id1;
	/// ���_�ɋ߂����ɕ��ёւ�
	if(vertices[vtxId0].disFromOrigin > vertices[vtxId1].disFromOrigin){
		unsigned tempId = vtxId0;
		vtxId0 = vtxId1;
		vtxId1 = tempId;
	}
	/// vtxId0 < vtxId1 ���ۏ؂���Ă���

	//	2�_�Ō���邱�Ƃ��O��
	//> 2�_��disFromOrigin��r,R�Ɣ�r���Ăǂ���ƌ���邩�𔻕ʂ���B
	//> �~�Ƃ̌�_�����߂�
	// x-z���ʂōl���Ă���
	/// constA,B:vtxId0.vtxId1��ʂ钼���̌X���ƐؕЁ@/// aconsta,constb�͐����\��Ȃ�
	DSTR << "id0: " << id0 << ", id1: " << id1 <<std::endl;
	constA = ( vertices[vtxId0].pos.z - vertices[vtxId1].pos.z) / ( vertices[vtxId0].pos.x - vertices[vtxId1].pos.x);
	DSTR << "vertices[vtxId0].pos.z: " << vertices[vtxId0].pos.z <<std::endl;
	DSTR << "vertices[vtxId0].pos.z: " << vertices[vtxId1].pos.z <<std::endl;
	DSTR << "dz: vertices[vtxId0].pos.z - vertices[vtxId1].pos.z : " << vertices[vtxId0].pos.z - vertices[vtxId1].pos.z << std::endl;

	DSTR << "vertices[vtxId0].pos.x: " << vertices[vtxId0].pos.x << std::endl;
	DSTR << "vertices[vtxId1].pos.x: " << vertices[vtxId1].pos.x << std::endl;
	DSTR << "dx: vertices[vtxId0].pos.x - vertices[vtxId1].pos.x: " << vertices[vtxId0].pos.x - vertices[vtxId1].pos.x << std::endl;

	DSTR << "constA = dz / dx: " << constA << std::endl;
	if(vertices[vtxId0].pos.z == vertices[vtxId1].pos.z && vertices[vtxId0].pos.x == vertices[vtxId1].pos.x){
		DSTR << "vertices[vtxId0].pos.y: " << vertices[vtxId0].pos.y << ", vertices[vtxId1].pos.y: " << vertices[vtxId1].pos.y << std::endl;
		if(vertices[vtxId0].pos.y == vertices[vtxId1].pos.y)
			DSTR << "id[" << id0 <<"], id[" << id1 << "] �͓������_ !" << std::endl;
	}
	
	constB = vertices[vtxId0].pos.z - constA * vertices[vtxId0].pos.x;
	DSTR << "constB = vertices[vtxId0].pos.z - constA * vertices[vtxId0].pos.x : " << vertices[vtxId0].pos.z - constA * vertices[vtxId0].pos.x << std::endl;

	//DSTR << "constA: " << constA << std::endl;
	//DSTR << "constB: " << constB << std::endl;
	//DSTR << std::endl;

	///	��_�̍��W���v�Z
	if(vertices[vtxId0].disFromOrigin < r){		/// ���ar�̉~�ƌ����Ƃ�
		//CalcYfromXatcross(vtxId0,vtxId1,r);	//�֐������Ȃ�
		//> �ȉ��A�֐���,vtxId0,1,r:�����AconstY��Ԃ�
		constX1 = (- constA * constB + sqrt(r * r *(constA * constA + 1.0) - constB * constB));
		constX1_ = (- constA * constB - sqrt(r * r *(constA * constA + 1.0) - constB * constB));
		// �ǂ��炩�����_�̊Ԃɂ���@�召���킩��Ȃ��̂ŁAor�ŁA�_1��x���W�A2��x���W�ƁA���̓���ւ��ƁA���
		//> �����̗��[�̓_�̊Ԃɂ���Ƃ�
		if( (vertices[vtxId0].pos.x <= constX1 && constX1 <= vertices[vtxId1].pos.x) || (vertices[vtxId1].pos.x <= constX1 && constX1 <= vertices[vtxId0].pos.x) ){
			constY1 = sqrt(r * r - constX1 * constX1 );
		}else{
			constY1 = sqrt(r * r - constX1_ * constX1_ );
			constX1 = constX1_;		///		�_��x���W��constX_�������������킩�����B
		}
	}else if(vertices[vtxId0].disFromOrigin < R){		/// ���aR�̉~�ƌ����Ƃ�
		constX1 = (- constA * constB + sqrt(R * R *(constA * constA + 1.0) - constB * constB));
		constX1_ = (- constA * constB - sqrt(R * R *(constA * constA + 1.0) - constB * constB));
		// �ǂ��炩�����_�̊Ԃɂ���@�召���킩��Ȃ��̂ŁAor�ŁA�_1��x���W�A2��x���W�ƁA���̓���ւ��ƁA���
		//> �����̗��[�̓_�̊Ԃɂ���Ƃ�
		if( (vertices[vtxId0].pos.x <= constX1 && constX1 <= vertices[vtxId1].pos.x) || (vertices[vtxId1].pos.x <= constX1 && constX1 <= vertices[vtxId0].pos.x) ){
			constY1 = sqrt(R * R - constX1 * constX1 );
		}else{
			constY1 = sqrt(R * R - constX1_ * constX1_ );
			constX1 = constX1_;		///		�_��x���W��constX_�������������킩�����B
		}
		
	}
	//> �ǂ���Ƃ������Ƃ�
	else if(vertices[vtxId0].disFromOrigin < r && R < vertices[vtxId1].disFromOrigin){
		//> �萔��2�~����
		constX1 = (- constA * constB + sqrt(r * r *(constA * constA + 1.0) - constB * constB));
		constX1_ = (- constA * constB - sqrt(r * r *(constA * constA + 1.0) - constB * constB));
		// �ǂ��炩�����_�̊Ԃɂ���@�召���킩��Ȃ��̂ŁAor�ŁA�_1��x���W�A2��x���W�ƁA���̓���ւ��ƁA���
		//> �����̗��[�̓_�̊Ԃɂ���Ƃ�
		if( (vertices[vtxId0].pos.x <= constX1 && constX1 <= vertices[vtxId1].pos.x) || (vertices[vtxId1].pos.x <= constX1 && constX1 <= vertices[vtxId0].pos.x) ){
			constY1 = sqrt(r * r - constX1 * constX1 );
		}else{
			constY1 = sqrt(r * r - constX1_ * constX1_ );
			constX1 = constX1_;		///		�_��x���W��constX_�������������킩�����B
		}
		constX2 = (- constA * constB + sqrt(R * R *(constA * constA + 1.0) - constB * constB));
		constX2_ = (- constA * constB - sqrt(R * R *(constA * constA + 1.0) - constB * constB));
		//> �����̗��[�̓_�̊Ԃɂ���Ƃ�
		if( (vertices[vtxId0].pos.x <= constX2 && constX2 <= vertices[vtxId1].pos.x) || (vertices[vtxId1].pos.x <= constX2 && constX2 <= vertices[vtxId0].pos.x) ){
			constY2 = sqrt(R * R - constX2 * constX2 );
		}else{
			constY2 = sqrt(R * R - constX2_ * constX2_ );
			constX2 = constX2_;		///		�_��x���W��constX_�������������킩�����B
		}
	}
	std::vector<Vec2d> intersection;
	intersection.push_back(Vec2d(constX1,constY1));
	if(constX2 && constY2){
		intersection.push_back(Vec2d(constX2,constY2));
	}
		//Vec4d interSection;
		//interSection[0] = constX1;
		//interSection[1] = constY1;
		//interSection[2] = constX2;
		//interSection[3] = constY2;
		DSTR << __FILE__  << "(" <<  __LINE__ << "):"<< "intersection Vtx: " << intersection[0] << std::endl;
		//":" <<  __TIME__ << 
		//return interSection;
		return intersection;
}

Vec2d PHFemMeshThermo::CalcIntersectionPoint(unsigned id0,unsigned id1,double r,double R){
	double constA = 0.0;
	double constB = 0.0;
	double constX = 0.0;
	double constX_ = 0.0;
	double constY = 0.0;

	//> �����̑�֏���	�֐��������Ƃ��ɁA�ϊ����遫
	unsigned vtxId0 = id0;
	unsigned vtxId1 = id1;
	/// ���_�ɋ߂����ɕ��ёւ�
	if(vertices[vtxId0].disFromOrigin > vertices[vtxId1].disFromOrigin){
		unsigned tempId = vtxId0;
		vtxId0 = vtxId1;
		vtxId1 = tempId;
	}
	/// vtxId0 < vtxId1 ���ۏ؂���Ă���

	//> 2�_��disFromOrigin��r,R�Ɣ�r���Ăǂ���ƌ���邩�𔻕ʂ���B
	if( (r <= vertices[vtxId0].disFromOrigin && vertices[vtxId0].disFromOrigin <= R) ^ (r <= vertices[vtxId1].disFromOrigin && vertices[vtxId1].disFromOrigin <= R)){
		//> �~�Ƃ̌�_�����߂�
		// x-z���ʂōl���Ă���
		/// constA,B:vtxId0.vtxId1��ʂ钼���̌X���ƐؕЁ@/// aconsta,constb�͐����\��Ȃ�
		constA = ( vertices[vtxId0].pos.z - vertices[vtxId1].pos.z) / ( vertices[vtxId0].pos.x - vertices[vtxId1].pos.x);
		constB = vertices[vtxId0].pos.z - constA * vertices[vtxId0].pos.x;

		///	��_�̍��W���v�Z
		if(vertices[vtxId0].disFromOrigin < r){		/// ���ar�̉~�ƌ����Ƃ�
			//CalcYfromXatcross(vtxId0,vtxId1,r);	//�֐������Ȃ�
			//> �ȉ��A�֐���,vtxId0,1,r:�����AconstY��Ԃ�
			constX = (- constA * constB + sqrt(r * r *(constA * constA + 1.0) - constB * constB));
			constX_ = (- constA * constB - sqrt(r * r *(constA * constA + 1.0) - constB * constB));
			// �ǂ��炩�����_�̊Ԃɂ���@�召���킩��Ȃ��̂ŁAor�ŁA�_1��x���W�A2��x���W�ƁA���̓���ւ��ƁA���
			//> �����̗��[�̓_�̊Ԃɂ���Ƃ�
			if( (vertices[vtxId0].pos.x <= constX && constX <= vertices[vtxId1].pos.x) || (vertices[vtxId1].pos.x <= constX && constX <= vertices[vtxId0].pos.x) ){
				constY = sqrt(r * r - constX * constX );
			}else{
				constY = sqrt(r * r - constX_ * constX_ );
				constX = constX_;		///		�_��x���W��constX_�������������킩�����B
			}
		}else if(r < vertices[vtxId0].disFromOrigin && vertices[vtxId0].disFromOrigin < R){		/// ���aR�̉~�ƌ����Ƃ�
			constX = (- constA * constB + sqrt(r * R *(constA * constA + 1.0) - constB * constB));
			constX_ = (- constA * constB - sqrt(r * R *(constA * constA + 1.0) - constB * constB));
			// �ǂ��炩�����_�̊Ԃɂ���@�召���킩��Ȃ��̂ŁAor�ŁA�_1��x���W�A2��x���W�ƁA���̓���ւ��ƁA���
			//> �����̗��[�̓_�̊Ԃɂ���Ƃ�
			if( (vertices[vtxId0].pos.x <= constX && constX <= vertices[vtxId1].pos.x) || (vertices[vtxId1].pos.x <= constX && constX <= vertices[vtxId0].pos.x) ){
				constY = sqrt(R * R - constX * constX );
			}else{
				constY = sqrt(R * R - constX_ * constX_ );
				constX = constX_;		///		�_��x���W��constX_�������������킩�����B
			}
		}
		//> �ǂ���Ƃ������Ƃ�
		else if(vertices[vtxId0].disFromOrigin < r && R < vertices[vtxId1].disFromOrigin){
			//> �萔��2�~����
		}
	}
		Vec2d interSection;
		interSection[0] = constX;
		interSection[1] = constY;
		DSTR << __FILE__  << "(" <<  __LINE__ << "):"<< "interSection: " << interSection << std::endl;
		//":" <<  __TIME__ << 
		return interSection;
}

void PHFemMeshThermo::ArrangeFacevtxdisAscendingOrder(int faceID){
	///	3�_�����_�ɋ߂����ɕ��ׂ�		//>	�N�C�b�N�\�[�g�ɂ����������H
	int vtxmin[3];		///	�ʂ��̒��_�ԍ�������
	vtxmin[0] = faces[faceID].vertices[0];
	vtxmin[1] = 0;
	vtxmin[2] = 0;
	if(vertices[faces[faceID].vertices[1]].disFromOrigin < vertices[faces[faceID].vertices[0]].disFromOrigin ){
		vtxmin[0] = faces[faceID].vertices[1];
		vtxmin[1] = faces[faceID].vertices[0];
	}else{
		vtxmin[1] = faces[faceID].vertices[1];
	}
	if(vertices[faces[faceID].vertices[2]].disFromOrigin < vertices[vtxmin[0]].disFromOrigin){
		vtxmin[2] = vtxmin[1];
		vtxmin[1] = vtxmin[0];
		vtxmin[0] = faces[faceID].vertices[2];
	}else if(vertices[vtxmin[0]].disFromOrigin < vertices[faces[faceID].vertices[2]].disFromOrigin && vertices[faces[faceID].vertices[2]].disFromOrigin < vertices[vtxmin[1]].disFromOrigin){
		vtxmin[2] = vtxmin[1];
		vtxmin[1] = faces[faceID].vertices[2];
	}else if(vertices[vtxmin[1]].disFromOrigin < vertices[faces[faceID].vertices[2]].disFromOrigin ){
		vtxmin[2] = faces[faceID].vertices[2];
	}
	//>	���������ɂȂ��Ă��Ȃ��Ƃ��́Aassert(0)
	if( !(vertices[vtxmin[0]].disFromOrigin < vertices[vtxmin[1]].disFromOrigin && vertices[vtxmin[1]].disFromOrigin < vertices[vtxmin[2]].disFromOrigin )
		){	assert(0);}
	/// debug
	//DSTR << "�������� ";
	//for(unsigned j=0; j <3; j++){
	//	DSTR << vertices[vtxmin[j]].disFromOrigin;
	//	if(j<2){ DSTR << ", ";}
	//}
	//DSTR << std::endl;
	/// face���̔z���face���ł̌��_����߂����Ԃ��i�[
	for(unsigned i=0;i<3;i++){
		faces[faceID].ascendVtx[i] = vtxmin[i];
	}
	//DSTR << "vtxmin[0~2]:  " << vtxmin[0] <<" ," << vtxmin[1] << " ,"  << vtxmin[2] << std::endl;	
	//vtxmin[0~2]:  11 ,324 ,281 ,vtxmin[0~2]:  353 ,11 ,324 ���̏��Ɍ��_�ɋ߂�

	//	�ȉ��A�폜
	//> �Ԃ�����
	//Vec3i vtxarray = Vec3i(vtxmin[0],vtxmin[1],vtxmin[2]);
	//	ex	vtxarray: (    11    324    281)	,vtxarray: (   353     11    324)
	//DSTR << "vtxarray: " << vtxarray << std::endl; 
	//return vtxarray;		///	�ʂ��̒��_�ԍ���Ԃ�
}
Vec2d PHFemMeshThermo::CalcIntersectionOfCircleAndLine(unsigned id0,unsigned id1,double radius){
	//	x-z���ʂł̔��ar�̉~�Ɛ����̌�_�̍��W�����߂�
	//	�֐����Ă΂������F����邱�Ƃ������ȂƂ��A�����~�ʂ̔��a�ƁA�~�ʂ̓��E�O���̒��_�����󂯌v�Z

	// ...2�_��ʂ钼���̕������̌X���ƐؕЂ����߂�
	double constA = 0.0;	//	�X��
	double constB = 0.0;	//	�ؕ�
	double constX1 = 0.0;	//	��_��x ���W�P
	double constX1_ = 0.0;	//		  x ���W���Q
	double constZ1 = 0.0;	//		  Y ���W
	double constZ1_ = 0.0;	//		  Y ���W���Q
	double constx = 0.0;	//	1���֐��ɂȂ�Ȃ��ꍇ�̒萔
	double constz = 0.0;	//�@		����

	//	...���_�ԍ������_�ɋ߂�����������( id0 < id1 )�ɕ��ёւ�
	if( vertices[ id1 ].disFromOrigin	<	vertices[ id0 ].disFromOrigin ){
		unsigned farfromOriginId = id0;
		id0 = id1;
		id1 = farfromOriginId;
	}	// id0 < id1 ��ۏ�

	// 1���֐��ɂȂ�Ȃ��ꍇ
	// .. x == const
	if(vertices[id0].pos.x == vertices[id1].pos.x && vertices[id0].pos.z != vertices[id1].pos.z ){
		//	x == const �̎�

	}
	// .. z == const

	// ...�X���ƐؕЂ����߂�
	//
	constA = ( vertices[id0].pos.z - vertices[id1].pos.z) / ( vertices[id0].pos.x - vertices[id1].pos.x);
	constB = vertices[id0].pos.z - constA * vertices[id0].pos.x;
	
	
	// ..���̏����́A�ȍ~�̏����ŃG���[���o��ꍇ��m�点�邽�߂́A�f�o�b�O�p
	if(!constA && !constB){
		//if( id0 != id1)
		//	vertices[id0].pos.z != 0 && vertices[id1].pos.z!= 0 && vertices[id0].pos.x
		// z == 0�Ƃ��������̎��ł���B�Ƃ������O�Ƃ��������̎��̎������낤�B
		//	���̏�������̈Ӗ��́A�����̗��[�̓_��x,z���W��(0,0)��A�����̎����P���֐��ɂȂ�Ȃ��ƋL���������Ƃ��ł���Ƃ��ɁA�p����B
		DSTR << "�����̎��̐ؕЂƌX��������0" << std::endl;
		DSTR << "id0: " << id0 << ", id1: " << id1 << "radius: " << radius << std::endl;
		DSTR << "vertices[id0].pos.x: " << vertices[id0].pos.x << ", vertices[id1].pos.x: " << vertices[id1].pos.x << std::endl;
		DSTR << "vertices[id0].pos.z: " << vertices[id0].pos.z << ", vertices[id1].pos.z: " << vertices[id1].pos.z << std::endl;
		assert(0);
	}
	//DSTR << "constA: " << constA << ",  " << "constB: " << constB <<std::endl; 

	//debug
	//DSTR << "id0: " << id0 << ", id1: " << id1 <<std::endl;
	//DSTR << "vertices[id0].pos.z: " << vertices[id0].pos.z <<std::endl;
	//DSTR << "vertices[id0].pos.z: " << vertices[id1].pos.z <<std::endl;
	//DSTR << "dz: vertices[id0].pos.z - vertices[id1].pos.z : " << vertices[id0].pos.z - vertices[id1].pos.z << std::endl;
	//DSTR << "vertices[id0].pos.x: " << vertices[id0].pos.x << std::endl;
	//DSTR << "vertices[id1].pos.x: " << vertices[id1].pos.x << std::endl;
	//DSTR << "dx: vertices[id0].pos.x - vertices[id1].pos.x: " << vertices[id0].pos.x - vertices[id1].pos.x << std::endl;

	// for Check Debug
	//DSTR << "constA = dz / dx: " << constA << std::endl;
	//if(vertices[id0].pos.z == vertices[id1].pos.z && vertices[id0].pos.x == vertices[id1].pos.x){
	//	DSTR << "vertices[id0].pos.y: " << vertices[id0].pos.y << ", vertices[id1].pos.y: " << vertices[id1].pos.y << std::endl;
	//	if(vertices[id0].pos.y == vertices[id1].pos.y)
	//		DSTR << "id[" << id0 <<"], id[" << id1 << "] �͓������_ !" << std::endl;
	//}
	//DSTR << "constB = vertices[id0].pos.z - constA * vertices[id0].pos.x : " << vertices[id0].pos.z - constA * vertices[id0].pos.x << std::endl;
	//DSTR << "constA: " << constA << std::endl;
	//DSTR << "constB: " << constB << std::endl;
	//DSTR << std::endl;

	///	.��_�̍��W���v�Z
	// .�P���֐��̏ꍇ�Ax,z���ɕ��s�Ȓ����̏ꍇ������
	if(vertices[id0].disFromOrigin <= radius && radius <= vertices[id1].disFromOrigin ){		/// ���ar�̉~�ƌ����ׂ�
		double radius2 = radius * radius;
		constX1  = (- constA * constB + sqrt(radius2 * (constA * constA + 1.0) - constB * constB )  )  /  (constA * constA + 1);
		constX1_ = (- constA * constB - sqrt(radius2 * (constA * constA + 1.0) - constB * constB )  )  /  (constA * constA + 1);
		// ��_��x���W�������̗��[�_��x���W�Ԃɂ���Ƃ�
		if( (vertices[id0].pos.x <= constX1 && constX1 <= vertices[id1].pos.x) || (vertices[id1].pos.x <= constX1 && constX1 <= vertices[id0].pos.x) ){
			constZ1 = sqrt(radius2 - constX1  * constX1  );
			constZ1_ = - sqrt(radius2 - constX1  * constX1  );
			//	��_��z���W�����[�_��z���W�Ԃɂ���Ƃ�
			if( (vertices[id0].pos.z <= constZ1_ && constZ1_ <= vertices[id1].pos.z) || (vertices[id1].pos.z <= constZ1_ && constZ1_ <= vertices[id0].pos.z) ){
				constZ1 = constZ1_;
			}
		}else{
			constX1 = constX1_;		///		�_��x���W��constX_�������������킩�����B
			constZ1  =   sqrt(radius2 - constX1  * constX1  );
			constZ1_ = - sqrt(radius2 - constX1  * constX1  );
			//	��_��z���W�����[�_��z���W�Ԃɂ���Ƃ�
			if( (vertices[id0].pos.z <= constZ1_ && constZ1_ <= vertices[id1].pos.z) || (vertices[id1].pos.z <= constZ1_ && constZ1_ <= vertices[id0].pos.z) ){
				constZ1 = constZ1_;
			}
		}
	}else{
		DSTR << "CalcVtxCircleAndLine()�֐��̂��̒��_�g�݂Ɖ~�ʂ͌����܂���" << std::endl;
		constX1 = 0.0;
		constZ1 = 0.0;
		DSTR << "(id0, vertices[id0].disFromOrigin): (" << id0 << ", " << vertices[id0].disFromOrigin << "), (id1, vertices[id1].disFromOrigin): (" << id1 << ", " << vertices[id1].disFromOrigin << "), radius: " << radius << std::endl;  
		assert(0);
	}
	Vec2d intersection = Vec2d(constX1,constZ1);
	//":" <<  __TIME__ << 
	return intersection;
}		//	CalcVtxCircleAndLine() :difinition

void PHFemMeshThermo::ShowIntersectionVtxDSTR(unsigned faceID,unsigned faceVtxNum,double radius){
	unsigned i = faceID;
	unsigned j = faceVtxNum;
	DSTR << "ascendVtx[" << j << "]: " << faces[i].ascendVtx[j] << ", " << "[ " << (j+1)%3 << "]: " << faces[i].ascendVtx[(j+1)%3] << "; ";
	DSTR << " (vertices[" << faces[i].ascendVtx[j] << "].pos.x, .z) = ( " <<  vertices[faces[i].ascendVtx[j]].pos.x << ", "<<  vertices[faces[i].ascendVtx[j]].pos.z  << "), " ;
	DSTR << " (vertices[" << faces[i].ascendVtx[(j+1)%3] << "].pos.x, .z) : ( " <<  vertices[faces[i].ascendVtx[(j+1)%3]].pos.x << ", "<<  vertices[faces[i].ascendVtx[(j+1)%3]].pos.z << "), " <<std::endl;
	DSTR <<"face[i].[(" << j << "], [" << (j+1)%3 << "]�F�e�X�̌��_����̋���" << vertices[faces[i].ascendVtx[j]].disFromOrigin << ", " << vertices[faces[i].ascendVtx[(j+1)%3]].disFromOrigin << ", "; 
	DSTR << " radius: " << radius <<" ��2�_�ō\�����������Ƃ̌�_�͉��L"<< std::endl;
	DSTR << __FILE__  << "(" <<  __LINE__ << "):"<< "Intersection Vtx (x,z)= " << CalcIntersectionOfCircleAndLine( faces[i].ascendVtx[ j ] , faces[i].ascendVtx[ (j+1)%3 ] , radius) << std::endl;
	DSTR << std::endl;
}

void PHFemMeshThermo::CalcIHarea(double radius,double Radius,double dqdtAll){
	//	face�\���̃����oiharea���v�Z
	//�A���b�V���S�̂�iharea�̍��v���v�Z
	//	...dqdtRatio(�M������)�𑍖ʐςɑ΂���iharea�ʐϔ䂩���΂̎��ɁAface���󂯎��ׂ�dqdt���v�Z���ĕԂ�

	//	�`��֐��̌v�Z�́Aiharea ��p���āACalcVecf2surface()���ōs��,face.shapefunk�ōs��		���N���X�����悤���H
		//	.... �`��֐����i�[����		// �����ŕ�����`��֐��́A���_�Ԃ̋������番������`��ԌW���@���Ȃ킿�A�����@�O�����`�����P�ŗǂ��āA�Ō�ɁA�s��ɓ����O�ɁA�����ȊO������΂����̂��ȁH

	// radius value check
	if(Radius <= radius){
		DSTR << "inner radius size is larger than outer Radius " << std::endl;
		DSTR << "check and set another value" << std::endl;
		assert(0);
	}
	///	�����a�ƊO���a�̊Ԃ̐ߓ_�ɔM�������E������ݒ�
	//> �l�ʑ̖ʂ̎O�p�`�Ɖ~�̈�̏d�������̌`��E�ʐς����߂铖���蔻����v�Z����B
	//>	�؂���`��ɉ������`��֐������߁A�M�����x�N�g���̐����ɑ�����A�v�Z����

	//> 1.�~�̈�Əd�Ȃ�face�O�p�`�̌`����Z�o����B�̈�Ɋ܂܂�钸�_�Aface�O�p�`�̕ӂƂ̌�_�����߂�vecteor�Ɋi�[����
	//>	2.vector�ɂ́A��0,1,2�̏��ɗ̈���̒��_���_�������Ă��邪�A��������ɎO�p�`�������s���B�O�p�`�������ł�����A�e�O�p�`�����߂�B�O�p�`�̑��a���A����face�̉��M�̈�Ƃ���B
	//>	3.vector�̓_�ɂ�����`��֐������߂āA�[���̐ρi�d�Ȃ��Ă���ʐρ~�`��֐��̒l�j���g���āA�l�ʑ̓��̊e�_�ɂ�����`��֐��̖ʐϕ������߂�B���߂��l�́A�M�����x�N�g���̐����Ƃ��ėv�f�����s��̐����ɑ������B
	//>	4.���X�e�b�v�A�����M�����̒l���x�N�g�������ɉ�����
	
	/// debug
	//unsigned numIHheated0 = 0; 
	//for(unsigned i=0; i < nSurfaceFace;i++){
	//	if(faces[i].mayIHheated){	
	//		DSTR << i << " ; "  << std::endl;
	//		numIHheated0 +=1;
	//	}
	//}
	//DSTR << "numIHheated0 / nSurfaceFace: " << numIHheated0 << " / " << nSurfaceFace << std::endl;	////	761 / 980	���ĂقƂ�ǂ���Ȃ����I�����ʂɂȂ�Ȃ��Ƃ��������͂������E�E�E�@���[�J��y���W�l���}�C�i�X�̂��̂�I��ł���̂�

	//	debug	mayIHheated�̊m�x���グ��O�̐���m�肽��
	unsigned numIHheated0 = 0; 
	for(unsigned i=0; i < nSurfaceFace;i++){
		if(faces[i].mayIHheated){	
			//DSTR << i << " ; "  << std::endl;
			numIHheated0 +=1;
		}
	}
	DSTR << "numIHheated0 / nSurfaceFace: " << numIHheated0 << " / " << nSurfaceFace << std::endl;

	//	face���_�̂ǂꂩ1���A�~�̈�ɓ����Ă���face�����Atrue�ɁA����ȊO�́Afalse��
	//> raius,Radius�ɂ���mayIHheated�̊m�x���グ�Ă���A�~�̈�Əd�Ȃ��Ă���`������߂�
	for(unsigned i=0;i < nSurfaceFace; i++){
		if(faces[i].mayIHheated){			// face�̐ߓ_��y���W�����̏ꍇ��IH���M�̑Ώېߓ_ �~�͈͓̔��ɓ����Ă���Ƃ͌���Ȃ�
			for(unsigned j=0;j<3;j++){
				/// �~�̈����face���_���܂܂��
				if(radius <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= Radius){
					faces[i].mayIHheated = true;
					break;		//>	����������A�����true�̂܂܂ŗǂ��B�œ�����for�𔲂���
				}
				else{
					faces[i].mayIHheated = false;
				}
				//> �i�~�̈�ɂ͊܂܂ꂸ�j�~�̈�������ƊO����face�̕ӂ̒��_������	vertices[j%3] �� vertices[(j+1)%3]�@�ō��ӂ�����Ƃ�
				if(vertices[faces[i].vertices[j]].disFromOrigin < radius && Radius < vertices[faces[i].vertices[(j+1)%3]].disFromOrigin 
					|| vertices[faces[i].vertices[(j+1)%3]].disFromOrigin < radius && Radius < vertices[faces[i].vertices[j]].disFromOrigin){
						faces[i].mayIHheated = true;
						break;		//>	����
				}else{
					faces[i].mayIHheated = false;
				}
				//>	�~�̈����face�ӂ̂ǂ��炩�̒��_���܂܂��Ƃ�(r<P1<R<P2,P1<r<P2<R,(��Pa1,P2�����ւ�������))
				if(radius <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin < Radius && Radius < vertices[faces[i].vertices[(j+1)%3]].disFromOrigin
					|| radius <= vertices[faces[i].vertices[(j+1)%3]].disFromOrigin && vertices[faces[i].vertices[(j+1)%3]].disFromOrigin < Radius && Radius < vertices[faces[i].vertices[j]].disFromOrigin
					|| vertices[faces[i].vertices[j]].disFromOrigin <= radius && radius < vertices[faces[i].vertices[(j+1)%3]].disFromOrigin && vertices[faces[i].vertices[(j+1)%3]].disFromOrigin < Radius
					|| vertices[faces[i].vertices[(j+1)%3]].disFromOrigin <= radius && radius < vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin < Radius){
						faces[i].mayIHheated = true;
						break;		//>	����
				}else{
					faces[i].mayIHheated = false;
				}
			}		//	for
		}		//	if
	}		//	for
	//> debug
	//>	mayIHheated�̃t���O�������Ă���face�ɂ��̖ʐς̌`��֐���^���Ă݂�B	�d�Ȃ�ʐς�������ƌv�Z�ƁA�����ł������������Ă���΁A���M�ʂɓ���Ă��܂��v�Z�A����
	//> CalcIHdqdt3 or 4

	/// debug
	unsigned numIHheated = 0; 
	for(unsigned i=0; i < nSurfaceFace;i++){
		if(faces[i].mayIHheated){	
			//DSTR << i << " ; "  << std::endl;
			numIHheated +=1;
		}
	}
	DSTR << "numIHheated / nSurfaceFace: " << numIHheated << " / " << nSurfaceFace << std::endl;		///:	�\��face�̓��A���M�ߓ_���܂�face�̐��A�S��:264/980�@����Ȃ��񂩂� 
	
	//	.. ��_�����߁Afaces�\���̂�vector�ɗ̈���̒��_���_���i�[
	for(unsigned i=0;i < nSurfaceFace; i++){
		if(faces[i].mayIHheated){		//	may �� �u�m���v�ɕω��ς݂̃t���O
			//	area:face�ʐς��v�Z����ĂȂ���΁A�v�Z
			if(faces[i].area==0) faces[i].area = CalcTriangleArea(faces[i].vertices[0],faces[i].vertices[1],faces[i].vertices[2]);
			////	face���̒��_��disFromOrigin�̒l�Ń\�[�g
			//unsigned nearestvtxnum		=	0;				///	���_�Ɉ�ԋ߂����_��face���_�ԍ�(0~2)
			//for(unsigned j=0;j<3;j++){
			//	double hikaku = DBL_MAX;
			//	if(hikaku > vertices[faces[i].vertices[j]].disFromOrigin){	hikaku = vertices[faces[i].vertices[j]].disFromOrigin;	nearestvtxnum = j;}
			//}
			
			// ... 3�_�����_�ɋ߂����ɕ��ׂ�		vtxOrder:�߂����Ɋi�[,�Ԗڂ��߂����ԁA�i�[�l�����_�ԍ� ==	faces[i].ascendVtx[3]
			ArrangeFacevtxdisAscendingOrder(i);		///	ArrangeVtxdisAscendingOrder(int faceID,int vtx0,int vtx1,int vtx2)
			//DSTR <<  "�����������m�F: " << vertices[vtxOrder[0]].disFromOrigin << ", "<< vertices[vtxOrder[1]].disFromOrigin << ", "<< vertices[vtxOrder[2]].disFromOrigin << std::endl;
			
			// ... face���̊e���_�������Ă���̈�𔻒� 0 | 1 | 2	///	faces[i].ascendVtx[0~2]�ɊY�����钸�_���@�~�̈�̑O��̂ǂ��ɑ��݂��Ă��邩
			// ... vtxdiv[0~2]�ɋ߂����ɕ��񂾒��_�̗̈�ID(0~2)������U�� �� faces[i].ascendVtx[j] �̏��ƑΉ�
			unsigned vtxdiv[3];		//	���_����߂���:0~2�ɕ��בւ���ꂽ���_ID�ɑΉ�����̈���O�̋敪���@�z��
			//DSTR << "faces[i].ascendVtx[0~2]:" ;
			for(unsigned j=0;j<3;j++){
				if( vertices[faces[i].ascendVtx[j]].disFromOrigin < radius){			vtxdiv[j] = 0;
				/// �~�ʏ���܂݁A�~�ʏ���~�̈���ƒ�`����
				}else if(radius <= vertices[faces[i].ascendVtx[j]].disFromOrigin && vertices[faces[i].ascendVtx[j]].disFromOrigin <= Radius ){	vtxdiv[j] = 1;
				}else if(Radius < vertices[faces[i].ascendVtx[j]].disFromOrigin){		vtxdiv[j] = 2;	}
				//DSTR << faces[i].ascendVtx[j] ;
				//if(j <  2) DSTR << ", " ;
			}
			//DSTR << std::endl;

			//> debug
			//DSTR << "���_�̗̈�ԍ�: " ;
			//for(unsigned j =0;j<3;j++){
			//	DSTR << vtxexistarea[j];
			//	if(j<2) DSTR << ", ";
			//}
			//DSTR << std::endl;
			//int vtxexistareadebug =0;
			//... 2012.2.14�����܂�...

			//...	�z��̐����̒l�̕ω������āA�n�_�A��_�A�ӑ΂ƂȂ�_������vector�Ɋi�[���Ă���
			
			//	vector�ɓ����ۂ̒���!!! %%%%%%%%%%%%%%%%%%
			// %%%	vtxOrder[ ���_����߂����_�̏�(0,1,2) ]:���̒��_ID�̌��_�ɋ߂����ɕ��בւ���ID���i�[		(��:vtxOrder[0] = (ID)278, [1] = (ID)35, [2] = (ID)76 etc)
			// %%%	vtxdiv[ ���_����߂����_�̏�(0,1,2) ]:���̒��_���~�̈�̓����i1�j���O��(0,2)����\��

			//>	face�̕ӂ��Ƃɏꍇ����
			///	 j ��(�ׂ�) (j+1)%3 �Ƃő΂𐬂��ӂɂ���
			for(unsigned j=0;j<3;j++){
				double f[3]={0.0, 0.0, 0.0};	// ���_0,1,2,3���猩���`��֐� 
				//debug
				//DSTR <<"j: " << j << ", faces[i].ascendVtx[j]: " << faces[i].ascendVtx[j] << ", faces[i].ascendVtx[(j+1)%3]: " << faces[i].ascendVtx[(j+1)%3] << std::endl;
				//DSTR << "vertices[faces[i].ascendVtx[j]].pos: (" << vertices[faces[i].ascendVtx[j]].pos.x  << ", "<< vertices[faces[i].ascendVtx[j]].pos.z << ") " << std::endl;
				//DSTR << "vertices[faces[i].ascendVtx[(j+1)%3]].pos: (" << vertices[faces[i].ascendVtx[(j+1)%3]].pos.x  << ", "<< vertices[faces[i].ascendVtx[(j+1)%3]].pos.z << ") " << std::endl; 
				//DSTR << std::endl;

				//	0�̗̈�ɂ����:
				if(vtxdiv[j] == 0 && vtxdiv[(j+1)%3] == 0){
					//	������̓_�����̈��vector�ɂ͓���Ȃ�
				}
				//	�����a�Ƃ���������(��:0���O:1�ƊO:1����:0):
				else if(vtxdiv[j] == 0 && vtxdiv[(j+1)%3] - vtxdiv[j] > 0 || vtxdiv[j] == 1 && vtxdiv[(j+1)%3] - vtxdiv[j] < 0){
					//	(�n�_�i���œ���Ă���ꍇ�ɂ͕s�v�j�j�Ɠ����a�Ƃ��A�΂̓_�ɓ����
					if(vtxdiv[(j+1)%3] - vtxdiv[j] > 0){	//�����O
						//	�����a�Ƃ̌�_�����߂āA���W������
						//%%%	�������\�����钸�_�Ɣ��a�A��_�̃`�F�b�N�֐���DSTR�\��	%%%//		//faceID,face���ߓ_�ԍ��A���a��p���āA�ׂ荇���ߓ_�ō������Ɖ~�ʂ̌�_�����߂ĕ\��
						ShowIntersectionVtxDSTR(i,j,radius);
						//	..�����a�Ƃ̌�_��x,z���W������
						faces[i].ihvtx.push_back( CalcIntersectionOfCircleAndLine( faces[i].ascendVtx[ j ] , faces[i].ascendVtx[ (j+1)%3 ] , radius) );
						//	..�g�Γ_�̍��W��intersection�ɓ����
						faces[i].ihvtx.push_back( Vec2d( vertices[ faces[i].ascendVtx[ (j+1)%3 ] ].pos.x, vertices[faces[i].ascendVtx[ (j+1)%3 ] ].pos.z) );
						//	....���̓_�ʒu�ł̌`��֐��𓱏o
					}else if(vtxdiv[(j+1)%3] - vtxdiv[j] < 0){		//�O����
						//	�����a�Ƃ̌�_�����߁A��_�̍��W������
						faces[i].ihvtx.push_back( CalcIntersectionOfCircleAndLine( faces[i].ascendVtx[ j ] , faces[i].ascendVtx[ (j+1)%3 ] , radius) );
						//..�g�Γ_�́A�����a�̓����F�~�̈�O�Ȃ̂ŁA�g�Γ_��vector�ɂ͓���Ȃ�
					}
				}
				//	�~�̈�(:1)���ɂ����
				else if(vtxdiv[j] == 1 && vtxdiv[(j+1)%3] == 1){
					//	(�n�_�����Ă���̂Ȃ�A)�ӑΓ_��vector�ɓ����
					//	�n�_�͓��ꂸ�Ƃ��A�Ō�ɓ���͂�
					////intersection.push_back(Vec2d(vertices[vtxdiv[(j+1)%3]].pos.x,vertices[vtxdiv[(j+1)%3]].pos.z));
					faces[i].ihvtx.push_back(Vec2d(vertices[faces[i].ascendVtx[(j+1)%3]].pos.x,	vertices[faces[i].ascendVtx[(j+1)%3]].pos.z));
				}
				//	�O���a�ƌ�����(�����O�A�O����)
				else if(vtxdiv[j] == 1 && vtxdiv[(j+1)%3] == 2 || vtxdiv[j] == 2 && vtxdiv[(j+1)%3] == 1){
					// ���������O���������A�����Ŕ��肷�邱�ƂŁA���	or	�̂ǂ��炩�𔻒肵�Avector�ɓ���鏇�Ԃ�ς���
					//	�O���a�Ƃ̌�_�����߂�
					//	�����O 2 - 1 = 1 > 0
					if(vtxdiv[(j+1)%3] - vtxdiv[j] > 0){
						//	��_���i�[
						faces[i].ihvtx.push_back( CalcIntersectionOfCircleAndLine( faces[i].ascendVtx[ j ] , faces[i].ascendVtx[ (j+1)%3 ] , Radius) );
						////	�g�Γ_��X,Z���W���i�[���i�[������_���ł���I
						//faces[i].ihvtx.push_back(Vec2d(vertices[faces[i].ascendVtx[(j+1)%3]].pos.x,	vertices[faces[i].ascendVtx[(j+1)%3]].pos.z));
						////	?��	intersection.push_back(Vec2d(vertices[vtxOrder[(j+1)%3]].pos.x,vertices[vtxOrder[(j+1)%3]].pos.z));
						
					}
					//	�O���� 1 - 2 = -1 < 0
					else if(vtxdiv[(j+1)%3] - vtxdiv[j] < 0){
						// �O���a�Ƃ̌�_�̍��W���i�[����
						faces[i].ihvtx.push_back( CalcIntersectionOfCircleAndLine( faces[i].ascendVtx[ j ] , faces[i].ascendVtx[ (j+1)%3 ] , Radius) );
						// �g�Γ_���i�[����
						faces[i].ihvtx.push_back( Vec2d( vertices[ faces[i].ascendVtx[ (j+1)%3 ] ].pos.x, vertices[faces[i].ascendVtx[ (j+1)%3 ] ].pos.z) );
					}
					else if(vtxdiv[(j+1)%3] - vtxdiv[j] == 0) assert(0);
					//	(�n�_��vector�ɓ����Ă��邱�Ƃ��m�F����)��_��vector�ɓ����
					faces[i].ihvtx.push_back( CalcIntersectionOfCircleAndLine( faces[i].ascendVtx[ j ] , faces[i].ascendVtx[ (j+1)%3 ] , Radius) );
				}
				//	�����a�ƊO���a�Ƃ̂ǂ���Ƃ�������(�����O�A�O����)
				else if(vtxdiv[j] == 0 && vtxdiv[(j+1)%3] == 2 || vtxdiv[j] == 2 && vtxdiv[(j+1)%3] == 0){
					// ���������O���������A�����Ŕ��肷�邱�ƂŁA���	or	�̂ǂ��炩�𔻒肵�Avector�ɓ���鏇�Ԃ�ς���
					//%%%	�ǂ���Ƃ���������
					// �����a�~�ʂ�茴�_��聨�O�F0���Q: 2 - 0 > 0
					if(vtxdiv[(j+1)%3] - vtxdiv[j] > 0){
						//�����a�ƌ����A��_���i�[
						faces[i].ihvtx.push_back( CalcIntersectionOfCircleAndLine( faces[i].ascendVtx[ j ] , faces[i].ascendVtx[ (j+1)%3 ] , radius) );
						//�O�n�ƌ����A��_���i�[
						faces[i].ihvtx.push_back( CalcIntersectionOfCircleAndLine( faces[i].ascendVtx[ j ] , faces[i].ascendVtx[ (j+1)%3 ] , Radius) );
					}else{	// �O�������a�~�ʂ�茴�_���F2���O
						//�O���a�ƌ����A��_���i�[
						faces[i].ihvtx.push_back( CalcIntersectionOfCircleAndLine( faces[i].ascendVtx[ j ] , faces[i].ascendVtx[ (j+1)%3 ] , Radius) );
						//���n�ƌ����A��_���i�[
						faces[i].ihvtx.push_back( CalcIntersectionOfCircleAndLine( faces[i].ascendVtx[ j ] , faces[i].ascendVtx[ (j+1)%3 ] , radius) );
					}
				}
				//	�����a�ƊO���a�ƌ�����(�O�����������)
				//else if(){
				//}
					//	if���̒��ŁA�������v���X���}�C�i�X�Ŕ���ł�����
				
				//	�O���a�̊O���ɂ����
				else if(vtxdiv[j] == 2 && vtxdiv[(j+1)%3] == 2){
					//	������̓_�����̈��vector�ɂ͓���Ȃ�
				}
				//	��������O���ɍs���ӂ͏�ŋL�q�ł��邪�A�O����������Ɍ������ӂ�����ŋL�q�ł���̂��H
			}	//	for(unsigned j=0;j<3;j++){

			//	..vector���O�p�`��������
			//	..���������O�p�`�̖ʐς��e�X���߂�
			// ..IH�ʐςɒǉ�

			//faces[i].iharea = 

			//	�f�o�b�O���ځFface��ihvtx(vector)�ɁAmayihheated�Ńt���O�̗�����face���͐ߓ_���S�Ċ܂܂�Ă��邩�B


// �ȉ��@���R�[�h
			/// ���R�[�h
			/// �̈���ɃX�^�[�g���_������Ƃ�
			//if(radius < vertices[faces[i].vertices[nearestvtxnum]].disFromOrigin && vertices[faces[i].vertices[(nearestvtxnum+1)%3]].disFromOrigin < Radius){	///	���_���̈���ɂ���Ƃ�
			//	intersection.push_back( Vec2d(vertices[faces[i].vertices[nearestvtxnum]].pos.x,vertices[faces[i].vertices[nearestvtxnum]].pos.z) );
			//}
			///// ���~�Ƃ�����_�����Ƃ�
			//else if(vertices[faces[i].vertices[nearestvtxnum]].disFromOrigin < radius && vertices[faces[i].vertices[(nearestvtxnum+1)%3]].disFromOrigin < Radius){
			//	//Vec2d vtxXZ = CalcIntersectionPoint(faces[i].vertices[nearestvtxnum],faces[i].vertices[(nearestvtxnum+1)%3],radius,Radius);
			//	tempXZ.push_back( CalcIntersectionPoint(faces[i].vertices[nearestvtxnum],faces[i].vertices[(nearestvtxnum+1)%3],radius,Radius) );
			//	DSTR << "case1 tempXZ[0]" << tempXZ[0] << std::endl;
			//}
			////>	�O�~�Ƃ�����_�����Ƃ�
			//else if(radius < vertices[faces[i].vertices[nearestvtxnum]].disFromOrigin && vertices[faces[i].vertices[(nearestvtxnum+1)%3]].disFromOrigin < Radius){
			//	//Vec2d vtxXZ = CalcIntersectionPoint(faces[i].vertices[nearestvtxnum],faces[i].vertices[(nearestvtxnum+1)%3],radius,Radius);
			//	tempXZ.push_back( CalcIntersectionPoint(faces[i].vertices[nearestvtxnum],faces[i].vertices[(nearestvtxnum+1)%3],radius,Radius) );
			//	DSTR << "case2 tempXZ[0]" << tempXZ[0] << std::endl;
			//}
			////>		���~�ƊO�~�ƌ�_�����ꍇ
			//else if(vertices[faces[i].vertices[nearestvtxnum]].disFromOrigin < radius  && Radius < vertices[faces[i].vertices[(nearestvtxnum + 1)%3]].disFromOrigin ){	//>	���_���̈�0��1�ɂ���Ƃ�
			//	//> ��_�����߁A��_�̒l��vector�ɑ������
			//	//Vec4d vtxXZ = CalcIntersectionPoint2(faces[i].vertices[nearestvtxnum],faces[i].vertices[(nearestvtxnum+1)%3],radius,Radius);
			//	DSTR << "nearestvtxnum: " << nearestvtxnum <<std::endl;
			//	DSTR << "(nearestvtxnum+1)%3: " << (nearestvtxnum+1)%3 <<std::endl;
			//	tempXZ = CalcIntersectionPoint2(faces[i].vertices[nearestvtxnum],faces[i].vertices[(nearestvtxnum+1)%3],radius,Radius);
			//	DSTR << "case3 tempXZ[0]" << tempXZ[0] << std::endl;
			//	if(tempXZ.size() >2){DSTR << "case3 tempXZ[1]" << tempXZ[1] << std::endl;}
			//	//> ��_�́A���~�ƁA�O�~��2���݂���
			//	// 2�̌�_�ɕ���
			//	//Vec2d vtxXZ0;
			//	//vtxXZ0[0] = vtxXZ[0];
			//	//vtxXZ0[1] = vtxXZ[1];
			//	//Vec2d vtxXZ1;
			//	//vtxXZ1[2] = vtxXZ[2];
			//	//vtxXZ1[3] = vtxXZ[3];
			//}
			//for(unsigned j=0; j< tempXZ.size();j++){
			//	DSTR << "tempXZ[" << j << "]" << tempXZ[j] << std::endl;
			//}
			
		}	//	if(mayIHheated)
	}	//	for(faces[i]


	//>	Step	1.�̎���
	//>	radius:�����a�ARadius:�O���a,�@dqdtAll:�P�ʎ��Ԃ�����ɓ����Ă���S�M��

	//%%	�菇 
	//> isHeated(->nearIHHeater)��faces�̓��A3�̐ߓ_�S���ɂ��āA���蔼�a�̒��ɓ����Ă�����̂����邩�ǂ������`�F�b�N����B
	//>	�����Ă�����̂��A��������A�ʐόv�Z�����������A�����ŁA���̐ߓ_�������Ă��邩�ɂ���ďꍇ�������s���B
	//>	�R��:face�̖ʐς�faceS�ɑ������	1~2��:�ʐς��ߎ��v�Z���邽�߂ɁA3�ߓ_�̓��A�ǂ̐ߓ_�Ɛߓ_�ō\�����ꂽ�ӂ������ar�܂��́A�O���aR�ƌ�����Ă��邩����
	//>	������Ă���ꍇ�ɂ́A���̌�_�����߂�B���߂���_�Ɠ����a�ȏ�O���a�ȓ��̗̈�ɂ���face���̐ߓ_��p���āA�O�p�`�����A���̖ʐς����߂�B

//> �ȉ��R�s�y���̃\�[�X�A������ɏ���
	//>	1.1�̈�Əd������ʐς�face���ɋ��߂�
	double faceS = 0.0;
	for(unsigned i=0;i < nSurfaceFace; i++){
		if(faces[i].mayIHheated){			// face�̐ߓ_��y���W�����̏ꍇ��IH���M�̑Ώېߓ_ �~�͈͓̔��ɓ����Ă���Ƃ͌���Ȃ�
			unsigned nObinnerVtx = 0;

			unsigned innervtx[4] = {0,0,0,0};
			//>	���_������vector or �z��		//>	�~�̈�Əd�Ȃ�`����v�Z���邽�߂ɁA�d�Ȃ�̈���ɂ���face���_�A�ӂƂ̌�_��face�̕�0~2�̏���vector�Ɋi�[���Ă����B�i�[���ɏd���������悤�ɂ���
			std::vector<Vec2d> intersectVtx;
			//Vec2d innertVtx[12];		//>	vector����z��ɕύX���遨�ÓI�������m��,���ǁAvector�̕����R�[�h�͊y?�������̊J����Y�ꂸ��
			unsigned judge[2] = {0,0};		///	judge[0],[1]�̏��Ɍ��_�ɋ߂��_�̔��茋��
			if(faces[i].area==0) faces[i].area = CalcTriangleArea(faces[i].vertices[0],faces[i].vertices[1],faces[i].vertices[2]);		// �ʐόv�Z���ς�łȂ���Όv�Z����
			
			//> �̈���ɓ����Ă���_�������莟��Aface�̒��_���܂߁Aface.vector�ɓ����Bface�̒��_������ꍇ�ɂ́A�d�����Ȃ����𒲂ׂ�B

			/// face�̊e�ӂ̗��[�̓_�ɂ��ď������� ////	j��0���珇�ɏ����Ă��邾���ŁAface�̕Ӕԍ��ł͂Ȃ�
			for(unsigned j=0; j < 3; j++){
				///	face���̓_�̑g(��)�ɂ��āA���̕ӂ̑g��
				/// vtx[(0+j)%3]�ɂ���,���_�ɋ߂����ɕ��ёւ�	//> == :���������̎��A�������Ȃ��B
				unsigned vtx0 = faces[i].vertices[(j+0)%3];
				unsigned vtx1 = faces[i].vertices[(j+1)%3];
				if(vertices[vtx0].disFromOrigin > vertices[vtx1].disFromOrigin){
					vtx0 = faces[i].vertices[(j+1)%3];
					vtx1 = faces[i].vertices[(j+0)%3];
				}
				///		   �_      �_	
				///  �E	  �E )  �E   )	 �E
				///			/       /	 	
				///  ��     ��   �@��	 ��	
				/// ���_�@�����a�@�O���a face���_	
				/// judge[0]:vtx0, judge[1]:vtx1���̈���ɂ��邩�ǂ��� (O(=Origin) < vtx0.disFromOrigin < vtx1.disFromOrigin )
				///	vtx0�� r<, < R, else �̈悲�Ƃ�judge[]

				///	3�_�S�����̈���ɂ���Ƃ��A����vector�Ɋi�[���邾���ł́A�d�����钸�_���o�Ă��Ă��܂��B
				///	�̂ŁA3�_���S���̈���ɂ���Ƃ��́Apush_back�̑O�ɁA���O����K�v������B
				///	�������W�̓_��vector�ɓ����Ă��Ȃ����m�F���Ă���i�[����

				//> �֐�������
				if(vertices[vtx0].disFromOrigin < radius){judge[0] =0;}
				else if(Radius < vertices[vtx0].disFromOrigin){judge[0] =2;}
				else{
					///	faces�O�p�`���_�̊i�[
					judge[0] = 1;
					//> �̈���̒��_���i�[
					Vec2d tempcoord = Vec2d(vertices[vtx0].pos.x,vertices[vtx0].pos.z);		
					//>	�������W�l�̓_���Ȃ���΁A�i�[�Bfind���Ă����֐����������悤�ȋC������.	map ? list?
					unsigned tempjudge =0;
					for(unsigned k =0; k < intersectVtx.size(); k++){
						if(intersectVtx[k] == tempcoord){
							tempjudge += 1;
						}
					}
					if(tempjudge == 0){
						//faces[i].innerIH.push_back(tempcoord);
						faces[i].ihvtx.push_back(tempcoord);
//						DSTR << "tempcoord: " << tempcoord <<std::endl;
					}
					for(unsigned k=0;k < intersectVtx.size();k++){
						DSTR << k << "th: " << intersectVtx[k] << std::endl;
					}
					//DSTR << "faces[i].innerIH: " << faces[i].innerIH.end() << std::endl;
				}
				
				///	vtx1�� r<, < R, else �̈悲�Ƃ�judge[]
				if(vertices[vtx1].disFromOrigin < radius){judge[1] =0;}
				else if(Radius < vertices[vtx1].disFromOrigin){judge[1] =2;}
				else{
					///	faces�O�p�`���_�̊i�[
					judge[1] = 1;
					//> �̈���̒��_���i�[
					Vec2d tempcoord = Vec2d(vertices[vtx1].pos.x,vertices[vtx1].pos.z);
					//>	�������W�l�̓_���Ȃ���΁A�i�[�Bfind���Ă����֐����������悤�ȋC������.	map ? list?
					unsigned tempjudge =0;
					for(unsigned k =0; k < intersectVtx.size(); k++){
						if(intersectVtx[k] == tempcoord){
							tempjudge += 1;
						}
					}
					if(tempjudge == 0){
						faces[i].ihvtx.push_back(tempcoord);
//						DSTR << "tempcoord: " << tempcoord <<std::endl;
					}
					for(unsigned k=0;k < intersectVtx.size();k++){
						DSTR << k << "th: " << intersectVtx[k] << std::endl;
					}
					//DSTR << "faces[i].innerIH: " << faces[i].innerIH.end() << std::endl;					//DSTR << "faces[i].innerIH: " << faces[i].innerIH[i].end() << std::endl;
				}

				//> ��_�̊i�[
				/// face�̕ӂ̗��[�̒��_��judge�̒l���قȂ�	��	�~�Ƃ̌�_�����߂�
				/// �����a(r),�O���a(R)�Ƃ̌�_�����߂�	[0]!=[1]:��_�����A�ǂ��炩���Ar,R�Ƃ̌�_������
				if(judge[0] == 0 && judge [1]==1 )
				{
					//> radius�Ƃ̌�_�����߂�
					//> ���߂���_��������
					Vec2d vtxXZ = Vec2d(0.0,0.0);		//> ��_�̍��W����
					//> ��_��vector�Ɋi�[
					faces[i].ihvtx.push_back(vtxXZ);
					//faces[i].innerIH.push_back(vtxXZ);
					//intersectVtx.push_back(vtxXZ);
				}
				else if(judge[0] == 1 && judge [1]==2 )
				{
					//> R�Ƃ̌�_�����߂�
					//> ���߂���_��vector�Ɋi�[����
				}
				else if(judge[0] == 1 && judge [1]==2 )
				{
					//> r,R�Ƃ̌�_�����߂�
					//> ���߂���_��vector�Ɋi�[����
				}
			}
		}
	}
}


//void PHFemMeshThermo::CalcIHdqdt4(double radius,double Radius,double dqdtAll){
//	///	�����a�ƊO���a�̊Ԃ̐ߓ_�ɔM�������E������ݒ�
//	//> �l�ʑ̖ʂ̎O�p�`�Ɖ~�̈�̏d�������̌`��E�ʐς����߂铖���蔻����v�Z����B
//	//>	�؂���`��ɉ������`��֐������߁A�M�����x�N�g���̐����ɑ�����A�v�Z����
//
//	//> 1.�~�̈�Əd�Ȃ�face�O�p�`�̌`����Z�o����B�̈�Ɋ܂܂�钸�_�Aface�O�p�`�̕ӂƂ̌�_�����߂�vecteor�Ɋi�[����
//	//>	2.vector�ɂ́A��0,1,2�̏��ɗ̈���̒��_���_�������Ă��邪�A��������ɎO�p�`�������s���B�O�p�`�������ł�����A�e�O�p�`�����߂�B�O�p�`�̑��a���A����face�̉��M�̈�Ƃ���B
//	//>	3.vector�̓_�ɂ�����`��֐������߂āA�[���̐ρi�d�Ȃ��Ă���ʐρ~�`��֐��̒l�j���g���āA�l�ʑ̓��̊e�_�ɂ�����`��֐��̖ʐϕ������߂�B���߂��l�́A�M�����x�N�g���̐����Ƃ��ėv�f�����s��̐����ɑ������B
//	//>	4.���X�e�b�v�A�����M�����̒l���x�N�g�������ɉ�����
//
//	//>	Step	1.�̎���
//	//>	radius:�����a�ARadius:�O���a,�@dqdtAll:�P�ʎ��Ԃ�����ɓ����Ă���S�M��
//
//	//%%	�菇 
//	//> isHeated(->nearIHHeater)��faces�̓��A3�̐ߓ_�S���ɂ��āA���蔼�a�̒��ɓ����Ă�����̂����邩�ǂ������`�F�b�N����B
//	//>	�����Ă�����̂��A��������A�ʐόv�Z�����������A�����ŁA���̐ߓ_�������Ă��邩�ɂ���ďꍇ�������s���B
//	//>	�R��:face�̖ʐς�faceS�ɑ������	1~2��:�ʐς��ߎ��v�Z���邽�߂ɁA3�ߓ_�̓��A�ǂ̐ߓ_�Ɛߓ_�ō\�����ꂽ�ӂ������ar�܂��́A�O���aR�ƌ�����Ă��邩����
//	//>	������Ă���ꍇ�ɂ́A���̌�_�����߂�B���߂���_�Ɠ����a�ȏ�O���a�ȓ��̗̈�ɂ���face���̐ߓ_��p���āA�O�p�`�����A���̖ʐς����߂�B
//
//	//>	1.1�̈�Əd������ʐς�face���ɋ��߂�
//	double faceS = 0.0;
//	for(unsigned i=0;i < nSurfaceFace; i++){
//		if(faces[i].mayIHheated){			// face�̐ߓ_��y���W�����̏ꍇ��IH���M�̑Ώېߓ_ �~�͈͓̔��ɓ����Ă���Ƃ͌���Ȃ�
//			unsigned nObinnerVtx = 0;
//
//			unsigned innervtx[4] = {0,0,0,0};
//			//>	���_������vector or �z��		//>	�~�̈�Əd�Ȃ�`����v�Z���邽�߂ɁA�d�Ȃ�̈���ɂ���face���_�A�ӂƂ̌�_��face�̕�0~2�̏���vector�Ɋi�[���Ă����B�i�[���ɏd���������悤�ɂ���
//			std::vector<Vec2d> intersectVtx;
//			//Vec2d innertVtx[12];		//>	vector����z��ɕύX���遨�ÓI�������m��,���ǁAvector�̕����R�[�h�͊y?�������̊J����Y�ꂸ��
//			unsigned judge[2] = {0,0};		///	judge[0],[1]�̏��Ɍ��_�ɋ߂��_�̔��茋��
//			if(faces[i].area==0) faces[i].area = CalcTriangleArea(faces[i].vertices[0],faces[i].vertices[1],faces[i].vertices[2]);		// �ʐόv�Z���ς�łȂ���Όv�Z����
//			
//			//> �̈���ɓ����Ă���_�������莟��Aface�̒��_���܂߁Aface.vector�ɓ����Bface�̒��_������ꍇ�ɂ́A�d�����Ȃ����𒲂ׂ�B
//
//			/// face�̊e�ӂ̗��[�̓_�ɂ��ď������� ////	j��0���珇�ɏ����Ă��邾���ŁAface�̕Ӕԍ��ł͂Ȃ�
//			for(unsigned j=0; j < 3; j++){
//				///	face���̓_�̑g(��)�ɂ��āA���̕ӂ̑g��
//				/// vtx[(0+j)%3]�ɂ���,���_�ɋ߂����ɕ��ёւ�	//> == :���������̎��A�������Ȃ��B
//				unsigned vtx0 = faces[i].vertices[(j+0)%3];
//				unsigned vtx1 = faces[i].vertices[(j+1)%3];
//				if(vertices[vtx0].disFromOrigin > vertices[vtx1].disFromOrigin){
//					vtx0 = faces[i].vertices[(j+1)%3];
//					vtx1 = faces[i].vertices[(j+0)%3];
//				}
//				///		   �_      �_	
//				///  �E	  �E )  �E   )	 �E
//				///			/       /	 	
//				///  ��     ��   �@��	 ��	
//				/// ���_�@�����a�@�O���a face���_	
//				/// judge[0]:vtx0, judge[1]:vtx1���̈���ɂ��邩�ǂ��� (O(=Origin) < vtx0.disFromOrigin < vtx1.disFromOrigin )
//				///	vtx0�� r<, < R, else �̈悲�Ƃ�judge[]
//
//				///	3�_�S�����̈���ɂ���Ƃ��A����vector�Ɋi�[���邾���ł́A�d�����钸�_���o�Ă��Ă��܂��B
//				///	�̂ŁA3�_���S���̈���ɂ���Ƃ��́Apush_back�̑O�ɁA���O����K�v������B
//				///	�������W�̓_��vector�ɓ����Ă��Ȃ����m�F���Ă���i�[����
//
//				//> �֐�������
//				if(vertices[vtx0].disFromOrigin < radius){judge[0] =0;}
//				else if(Radius < vertices[vtx0].disFromOrigin){judge[0] =2;}
//				else{
//					///	faces�O�p�`���_�̊i�[
//					judge[0] = 1;
//					//> �̈���̒��_���i�[
//					Vec2d tempcoord = Vec2d(vertices[vtx0].pos.x,vertices[vtx0].pos.z);		
//					//>	�������W�l�̓_���Ȃ���΁A�i�[�Bfind���Ă����֐����������悤�ȋC������.	map ? list?
//					unsigned tempjudge =0;
//					for(unsigned k =0; k < intersectVtx.size(); k++){
//						if(intersectVtx[k] == tempcoord){
//							tempjudge += 1;
//						}
//					}
//					if(tempjudge == 0){
//						faces[i].ihvtx.push_back(tempcoord);
//						DSTR << "tempcoord: " << tempcoord <<std::endl;
//					}
//					for(unsigned k=0;k < intersectVtx.size();k++){
//						DSTR << k << "th: " << intersectVtx[k] << std::endl;
//					}
//					//DSTR << "faces[i].innerIH: " << faces[i].innerIH.end() << std::endl;
//				}
//				
//				///	vtx1�� r<, < R, else �̈悲�Ƃ�judge[]
//				if(vertices[vtx1].disFromOrigin < radius){judge[1] =0;}
//				else if(Radius < vertices[vtx1].disFromOrigin){judge[1] =2;}
//				else{
//					///	faces�O�p�`���_�̊i�[
//					judge[1] = 1;
//					//> �̈���̒��_���i�[
//					Vec2d tempcoord = Vec2d(vertices[vtx1].pos.x,vertices[vtx1].pos.z);
//					//>	�������W�l�̓_���Ȃ���΁A�i�[�Bfind���Ă����֐����������悤�ȋC������.	map ? list?
//					unsigned tempjudge =0;
//					for(unsigned k =0; k < intersectVtx.size(); k++){
//						if(intersectVtx[k] == tempcoord){
//							tempjudge += 1;
//						}
//					}
//					if(tempjudge == 0){
//						faces[i].ihvtx.push_back(tempcoord);
//						DSTR << "tempcoord: " << tempcoord <<std::endl;
//					}
//					for(unsigned k=0;k < intersectVtx.size();k++){
//						DSTR << k << "th: " << intersectVtx[k] << std::endl;
//					}
//					//DSTR << "faces[i].innerIH: " << faces[i].innerIH.end() << std::endl;					//DSTR << "faces[i].innerIH: " << faces[i].innerIH[i].end() << std::endl;
//				}
//
//				//> ��_�̊i�[
//				/// face�̕ӂ̗��[�̒��_��judge�̒l���قȂ�	��	�~�Ƃ̌�_�����߂�
//				/// �����a(r),�O���a(R)�Ƃ̌�_�����߂�	[0]!=[1]:��_�����A�ǂ��炩���Ar,R�Ƃ̌�_������
//				if(judge[0] == 0 && judge [1]==1 )
//				{
//					//> radius�Ƃ̌�_�����߂�
//					//> ���߂���_��������
//					Vec2d vtxXZ = Vec2d(0.0,0.0);		//> ��_�̍��W����
//					//> ��_��vector�Ɋi�[
//					faces[i].ihvtx.push_back(vtxXZ);
//					//intersectVtx.push_back(vtxXZ);
//				}
//				else if(judge[0] == 1 && judge [1]==2 )
//				{
//					//> R�Ƃ̌�_�����߂�
//					//> ���߂���_��vector�Ɋi�[����
//				}
//				else if(judge[0] == 1 && judge [1]==2 )
//				{
//					//> r,R�Ƃ̌�_�����߂�
//					//> ���߂���_��vector�Ɋi�[����
//				}
//			}
//			
//
//
//			/// �ȉ��A�R�s�y�O�̃R�[�h�̂��߁A�����A�s�v�B��_�����߂�A���S���Y�������A��ɓ]�p����
////
////			//> �u��������0,1�����ւ��邾��(=�|�����[�t�B�Y��)�Ŏc��̎��������
////				// ���L�������֐�������?�B�_��ID������΁A��������������֐�
////				//> 0-1�Ō�_������Ă���Ƃ� true false �͘_���Z�̔r���I�_���aXOR��true���A��_���v�Z���� 00=0,01=10=1,11=0; �Е��̓_���͈͓��ŁA����������͈͊O�̏ꍇ�Atrue�ɂȂ�
////			
////				//> �~�͈͓̔��ɓ����Ă�����AmayIHheated��true�ɁA���邢�́Afalse�ɂ���(�ꉞ)
////
////				/// faces[i]�̒��_�����A�̈���ɓ����Ă��邩��������
////			for(unsigned j=0;j<3;j++){
////				if( radius <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= Radius){
////					nObinnerVtx += 1;
////					innervtx[j] = 1;
////				}
////			}
////
////			//> nObinnerVtx�̒l�ŏ�������
////			
////			///	face�̕ӂ��A�~�ʂƌ�����_�����߂�
////			if(nObinnerVtx == 1){
////				Vec2d vtxXY[3];			/// face�̕ӂƔ��ar,R�Ƃ̌�_��x,z���W�@�ő�R�_
////				for(unsigned j=0;j<3;j++){		///	face���\������3���_�ɂ���
////					unsigned k = 0;
////					unsigned m = 0;
////					k = (j+1)%3;		///	j=0;k=1, j=1;k=2, j=2;k=0 
////					m = (j+2)%3;
////					//vtxXY[j] = CalcIntersectionPoint(faces[i].vertices[j],faces[i].vertices[k],r,R);
////					//DSTR << "vtxXY[" << j << "]: " << vtxXY[j] << std::endl; 
////					if(innervtx[j] ==1){			/// faces[i]��j�Ԗڂ̐ߓ_���~�̈���ɓ����Ă���
////					/// j�Ԗڂ̒��_�ƃG�b�W���\������_(���̂Q�_)���g���āA���ar,R�ƌ����_���Z�o����						
////					//> j 0,1,2
////						if(j == 0){	/// ���_j�ƕӂ���钸�_���g���āA�ӂƌ�����_�����߂�
////							//k = (j+1)%3;		///	j=0;k=1, j=1;k=2, j=2;k=0 
////						vtxXY[j] = CalcIntersectionPoint(faces[i].vertices[j],faces[i].vertices[k],radius,Radius);
////						vtxXY[k] = CalcIntersectionPoint(faces[i].vertices[j],faces[i].vertices[m],radius,Radius);
////						}
////						else if(j == 1){
////						vtxXY[j] = CalcIntersectionPoint(faces[i].vertices[j],faces[0].vertices[1],radius,Radius);
////						vtxXY[j] = CalcIntersectionPoint(faces[i].vertices[j],faces[2].vertices[2],radius,Radius);
////						}
////						else if(j == 2){
////						vtxXY[j] = CalcIntersectionPoint(faces[i].vertices[j],faces[0].vertices[1],radius,Radius);
////						vtxXY[j] = CalcIntersectionPoint(faces[i].vertices[j],faces[1].vertices[2],radius,Radius);
////						}
////						else{
////							assert(0);
////							DSTR << "Error in PHFemMeshThermo::CalcIHdqdt2(hogehoge) �̈攻��Ƀ~�X" << std::endl;
////						}
////					}
////				}
////			}
////				//for(unsigned j=0;j<3;j++){
////				//	/// face���̐ߓ_�����ԂɃ`�F�b�N����
////				//	unsigned k =0;
////				//	k = (j+1)%3;		///	j=0;k=1, j=1;k=2, j=2;k=0 
////				//	if( r <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= R){
////				//	}
////				//	if( r <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= R){
////				//		//> j,k��faces[i].vertices[j],faces[i].vertices[k]�Ƃ��đ��
////				//		CalcIntersectionPoint(faces[i].vertices[0],faces[i].vertices[1],r,R);
////				//	}
////				//}
////
////				//> r -> radius
////				//> R -> Radius
////			
////				/// ���W(consX ,constY)���~�ƎO�p�`�̕ӂƂ̌�_
////			
////				//unsigned vtxId0 = faces[i].vertices[0];
////				//unsigned vtxId1 = faces[i].vertices[1];
////		
//////			free(innervtx);		//> �������̉��	//�z��͓��I�������ł͂Ȃ��̂�free�ŊJ���ł��Ȃ�
////		
////		
////			////> 1-2�Ō�_������Ă���Ƃ�	//
////			//else if( (r <= vertices[faces[i].vertices[1]].disFromOrigin && vertices[faces[i].vertices[1]].disFromOrigin <= R) ^ (vertices[faces[i].vertices[2]].disFromOrigin && vertices[faces[i].vertices[2]].disFromOrigin <= R)){
////			//	//> �~�Ƃ̌�_�����߂�
////			//	int katoon00 =0;
////			//}
////			////> 0-2�Ō�_������Ă���Ƃ�
////			//else if( (r <= vertices[faces[i].vertices[0]].disFromOrigin && vertices[faces[i].vertices[0]].disFromOrigin <= R) ^ (vertices[faces[i].vertices[2]].disFromOrigin && vertices[faces[i].vertices[2]].disFromOrigin <= R)){
////			//	//> �~�Ƃ̌�_�����߂�
////			//	int katoon000 =0;
////			//}
////
////			//for(unsigned j=0;j<3;j++){
////			//	if( (r <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= R) * (r <= vertices[faces[i].vertices[j+1]].disFromOrigin && vertices[faces[i].vertices[j+1]].disFromOrigin <= R)){
////			//		if(r <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= R)
////			//		nObinnerVtx += 1;
////			//	}
////			//}
////			//if( nObinnerVtx == 1)			faces[i].fluxarea = 1.5/3.0 * faces[i].area;//faces[i].fluxarea = 1.0/3.0 * faces[i].area;
////			//else if(nObinnerVtx == 2)		faces[i].fluxarea = 2.8/3.0 * faces[i].area;//faces[i].fluxarea = 2.0/3.0 * faces[i].area;
////			//else if(nObinnerVtx == 3)		faces[i].fluxarea = faces[i].area;
////			//else if(nObinnerVtx == 0)		faces[i].fluxarea = 0;
////
////			//if(faces[i].fluxarea >= 0){	
////			//	faceS += faces[i].fluxarea;
////			//}else{	assert(0);	}		//	faces[i].fluxarea��0�����̐����������Ă���̂ɉ��Z���悤�Ƃ��Ă���
////			//DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;	
//	}
//
//	//for(unsigned i=0;i < nSurfaceFace; i++){
//	//	DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
//	//}
//
//		if(faceS > 0){
//			//> dqdt ��P�ʖʐς�����ɒ���([1/m^2])
//			double dqdtds = dqdtAll / faceS;
//	//		DSTR << "dqdtds:  " << dqdtds << std::endl;
//			//>	�ȉ��A�M������faces�Ɋi�[����
//			//>	�M�����̖ʐόv�Z��fluxarea��p���čs��
//			for(unsigned i=0;i < nSurfaceFace; i++){
//				if(faces[i].mayIHheated){
//					faces[i].heatflux = dqdtds * faces[i].fluxarea;		//	�M�����̗ʂ�heatflux�̖ʐς���v�Z
//	//				DSTR << "faces[" << i <<"].heatflux: " << faces[i].heatflux <<std::endl;
//				}
//			}
//		}
//	
//		//�@�ȏ�A�l�͓����Ă���悤��
//
//		int katoon =0;
//		//���������āACreateMatk2t���R�s�[�����֐��ŁAVecf2?�������
//
//		//>	�M�ʂ́Adqdtds��p����
//
//		//> r <= <= R�̒��S������ˏ�ɉ��M
//
//		//	�ߓ_��dqdt�̒l���X�V����
//
//		//�@�ȉ��́A�x�N�g�������֐��̎d��
//		//	�ߓ_�̑�����\�ʂ̖ʂŁA�v�Z����
//		//  vertices[].heatFluxValue����Ɍv�Z��i�߂�
//		//	�K�E�X�U�C�f���v�Z�ł���悤�ɏ����ȂǁA��������
//	}
//}


void PHFemMeshThermo::CalcIHdqdt3(double r,double R,double dqdtAll){
	///	�����a�ƊO���a�̊Ԃ̐ߓ_�ɔM�������E������ݒ�

	//> ���M����l�ʑ̖ʂ̖ʐς̑��a�����߂�
	double faceS = 0.0;
	for(unsigned i=0;i < nSurfaceFace; i++){
		if(faces[i].mayIHheated){			// face�̐ߓ_��y���W�����̏ꍇ��IH���M�̑Ώېߓ_
			unsigned nObinnerVtx = 0;
			if(faces[i].area==0) faces[i].area = CalcTriangleArea(faces[i].vertices[0],faces[i].vertices[1],faces[i].vertices[2]);
			for(unsigned j=0;j<3;j++){
				if( r <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= R){
					nObinnerVtx += 1;
				}
			}
			if( nObinnerVtx == 1)			faces[i].fluxarea = 1.5/3.0 * faces[i].area;//faces[i].fluxarea = 1.0/3.0 * faces[i].area;
			else if(nObinnerVtx == 2)		faces[i].fluxarea = 2.8/3.0 * faces[i].area;//faces[i].fluxarea = 2.0/3.0 * faces[i].area;
			else if(nObinnerVtx == 3)		faces[i].fluxarea = faces[i].area;
			else if(nObinnerVtx == 0)		faces[i].fluxarea = 0;

			if(faces[i].fluxarea >= 0){	
				faceS += faces[i].fluxarea;
			}else{		assert(0);	}		//	faces[i].fluxarea��0�����̐����������Ă���̂ɉ��Z���悤�Ƃ��Ă���
			//DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
		}
	}

	//for(unsigned i=0;i < nSurfaceFace; i++){
	//	DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
	//}

	if(faceS > 0){
		//> dqdt ��P�ʖʐς�����ɒ���([1/m^2])
		double dqdtds = dqdtAll / faceS;
//		DSTR << "dqdtds:  " << dqdtds << std::endl;
		//>	�ȉ��A�M������faces�Ɋi�[����
		//>	�M�����̖ʐόv�Z��fluxarea��p���čs��
		for(unsigned i=0;i < nSurfaceFace; i++){
			if(faces[i].mayIHheated){
				faces[i].heatflux = dqdtds * faces[i].fluxarea;		//	�M�����̗ʂ�heatflux�̖ʐς���v�Z
//				DSTR << "faces[" << i <<"].heatflux: " << faces[i].heatflux <<std::endl;
			}
		}
	}
	
	//�@�ȏ�A�l�͓����Ă���悤��

	int katoon =0;
	//���������āACreateMatk2t���R�s�[�����֐��ŁAVecf2?�������

	//>	�M�ʂ́Adqdtds��p����

	//> r <= <= R�̒��S������ˏ�ɉ��M

	//	�ߓ_��dqdt�̒l���X�V����

	//�@�ȉ��́A�x�N�g�������֐��̎d��
	//	�ߓ_�̑�����\�ʂ̖ʂŁA�v�Z����
	//  vertices[].heatFluxValue����Ɍv�Z��i�߂�
	//	�K�E�X�U�C�f���v�Z�ł���悤�ɏ����ȂǁA��������

}

void PHFemMeshThermo::CalcIHdqdt2(double r,double R,double dqdtAll){
	///	�����a�ƊO���a�̊Ԃ̐ߓ_�ɔM�������E������ݒ�
	//> �~�ŋ�؂���l�ʑ̖ʂ̗̈���O�p�`�ŋߎ�����

	//> ���M����l�ʑ̖ʂ̖ʐς̑��a�����߂�

	//%%	�菇 
	//> isHeated(->nearIHHeater)��faces�̓��A3�̐ߓ_�S���ɂ��āA���蔼�a�̒��ɓ����Ă�����̂����邩�ǂ������`�F�b�N����B
	//>	�����Ă�����̂��A��������A�ʐόv�Z�����������A�����ŁA���̐ߓ_�������Ă��邩�ɂ���ďꍇ�������s���B
	//>	�R��:face�̖ʐς�faceS�ɑ������	1~2��:�ʐς��ߎ��v�Z���邽�߂ɁA3�ߓ_�̓��A�ǂ̐ߓ_�Ɛߓ_�ō\�����ꂽ�ӂ������ar�܂��́A�O���aR�ƌ�����Ă��邩����
	//>	������Ă���ꍇ�ɂ́A���̌�_�����߂�B���߂���_�Ɠ����a�ȏ�O���a�ȓ��̗̈�ɂ���face���̐ߓ_��p���āA�O�p�`�����A���̖ʐς����߂�B

	double faceS = 0.0;
	for(unsigned i=0;i < nSurfaceFace; i++){
		if(faces[i].mayIHheated){			// face�̐ߓ_��y���W�����̏ꍇ��IH���M�̑Ώېߓ_ �~�͈͓̔��ɓ����Ă���Ƃ͌���Ȃ�
			unsigned nObinnerVtx = 0;
			unsigned inner[3] = {0,0,0};
			unsigned judge[2] = {0,0};		///	judge[0],[1]�̏��Ɍ��_�ɋ߂��_�̔��茋��
			if(faces[i].area==0) faces[i].area = CalcTriangleArea(faces[i].vertices[0],faces[i].vertices[1],faces[i].vertices[2]);		// �ʐόv�Z���ς�łȂ���Όv�Z����
				//> �u��������0,1�����ւ��邾��(=�|�����[�t�B�Y��)�Ŏc��̎��������
				// ���L�������֐�������?�B�_��ID������΁A��������������֐�
				//> 0-1�Ō�_������Ă���Ƃ� true false �͘_���Z�̔r���I�_���aXOR��true���A��_���v�Z���� 00=0,01=10=1,11=0; �Е��̓_���͈͓��ŁA����������͈͊O�̏ꍇ�Atrue�ɂȂ�
			
				//> �~�͈͓̔��ɓ����Ă�����AmayIHheated��true�ɁA���邢�́Afalse�ɂ���(�ꉞ)

				/// faces[i]�̒��_�����A�̈���ɓ����Ă��邩��������
				for(unsigned j=0;j<3;j++){
					if( r <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= R){
						nObinnerVtx += 1;
						inner[j] = 1;
					}
				}

				//> nObinnerVtx�̒l�ŏ�������
			
				///	
				if(nObinnerVtx == 1){
					Vec2d vtxXY[3];			/// face�̕ӂƔ��ar,R�Ƃ̌�_��x,z���W�@�ő�R�_
					for(unsigned j=0;j<3;j++){		///	face���\������3���_�ɂ���
						unsigned k = 0;
						unsigned m = 0;
						k = (j+1)%3;		///	j=0;k=1, j=1;k=2, j=2;k=0 
						m = (j+2)%3;
						//vtxXY[j] = CalcIntersectionPoint(faces[i].vertices[j],faces[i].vertices[k],r,R);
						//DSTR << "vtxXY[" << j << "]: " << vtxXY[j] << std::endl; 
						if(inner[j] ==1){			/// faces[i]��j�Ԗڂ̐ߓ_���~�̈���ɓ����Ă���
						/// j�Ԗڂ̒��_�ƃG�b�W���\������_(���̂Q�_)���g���āA���ar,R�ƌ����_���Z�o����						
						//> j 0,1,2
							if(j == 0){	/// ���_j�ƕӂ���钸�_���g���āA�ӂƌ�����_�����߂�
								//k = (j+1)%3;		///	j=0;k=1, j=1;k=2, j=2;k=0 
							vtxXY[j] = CalcIntersectionPoint(faces[i].vertices[j],faces[i].vertices[k],r,R);
							vtxXY[k] = CalcIntersectionPoint(faces[i].vertices[j],faces[i].vertices[m],r,R);
							}
							else if(j == 1){
							vtxXY[j] = CalcIntersectionPoint(faces[i].vertices[j],faces[0].vertices[1],r,R);
							vtxXY[j] = CalcIntersectionPoint(faces[i].vertices[j],faces[2].vertices[2],r,R);
							}
							else if(j == 2){
							vtxXY[j] = CalcIntersectionPoint(faces[i].vertices[j],faces[0].vertices[1],r,R);
							vtxXY[j] = CalcIntersectionPoint(faces[i].vertices[j],faces[1].vertices[2],r,R);
							}
							else{
								assert(0);
								DSTR << "Error in PHFemMeshThermo::CalcIHdqdt2(hogehoge) �̈攻��Ƀ~�X" << std::endl;
							}
						 }
					}
				}

				//for(unsigned j=0;j<3;j++){
				//	/// face���̐ߓ_�����ԂɃ`�F�b�N����
				//	unsigned k =0;
				//	k = (j+1)%3;		///	j=0;k=1, j=1;k=2, j=2;k=0 
				//	if( r <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= R){
				//	}
				//	if( r <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= R){
				//		//> j,k��faces[i].vertices[j],faces[i].vertices[k]�Ƃ��đ��
				//		CalcIntersectionPoint(faces[i].vertices[0],faces[i].vertices[1],r,R);
				//	}
				//}

				//> r -> radius
				//> R -> Radius
			
				/// ���W(consX ,constY)���~�ƎO�p�`�̕ӂƂ̌�_
			
				//unsigned vtxId0 = faces[i].vertices[0];
				//unsigned vtxId1 = faces[i].vertices[1];
			}
			////> 1-2�Ō�_������Ă���Ƃ�	//
			//else if( (r <= vertices[faces[i].vertices[1]].disFromOrigin && vertices[faces[i].vertices[1]].disFromOrigin <= R) ^ (vertices[faces[i].vertices[2]].disFromOrigin && vertices[faces[i].vertices[2]].disFromOrigin <= R)){
			//	//> �~�Ƃ̌�_�����߂�
			//	int katoon00 =0;
			//}
			////> 0-2�Ō�_������Ă���Ƃ�
			//else if( (r <= vertices[faces[i].vertices[0]].disFromOrigin && vertices[faces[i].vertices[0]].disFromOrigin <= R) ^ (vertices[faces[i].vertices[2]].disFromOrigin && vertices[faces[i].vertices[2]].disFromOrigin <= R)){
			//	//> �~�Ƃ̌�_�����߂�
			//	int katoon000 =0;
			//}

			//for(unsigned j=0;j<3;j++){
			//	if( (r <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= R) * (r <= vertices[faces[i].vertices[j+1]].disFromOrigin && vertices[faces[i].vertices[j+1]].disFromOrigin <= R)){
			//		if(r <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= R)
			//		nObinnerVtx += 1;
			//	}
			//}
			//if( nObinnerVtx == 1)			faces[i].fluxarea = 1.5/3.0 * faces[i].area;//faces[i].fluxarea = 1.0/3.0 * faces[i].area;
			//else if(nObinnerVtx == 2)		faces[i].fluxarea = 2.8/3.0 * faces[i].area;//faces[i].fluxarea = 2.0/3.0 * faces[i].area;
			//else if(nObinnerVtx == 3)		faces[i].fluxarea = faces[i].area;
			//else if(nObinnerVtx == 0)		faces[i].fluxarea = 0;

			//if(faces[i].fluxarea >= 0){	
			//	faceS += faces[i].fluxarea;
			//}else{	assert(0);	}		//	faces[i].fluxarea��0�����̐����������Ă���̂ɉ��Z���悤�Ƃ��Ă���
			//DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
		}

	//for(unsigned i=0;i < nSurfaceFace; i++){
	//	DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
	//}

	if(faceS > 0){
		//> dqdt ��P�ʖʐς�����ɒ���([1/m^2])
		double dqdtds = dqdtAll / faceS;
//		DSTR << "dqdtds:  " << dqdtds << std::endl;
		//>	�ȉ��A�M������faces�Ɋi�[����
		//>	�M�����̖ʐόv�Z��fluxarea��p���čs��
		for(unsigned i=0;i < nSurfaceFace; i++){
			if(faces[i].mayIHheated){
				faces[i].heatflux = dqdtds * faces[i].fluxarea;		//	�M�����̗ʂ�heatflux�̖ʐς���v�Z
//				DSTR << "faces[" << i <<"].heatflux: " << faces[i].heatflux <<std::endl;
			}
		}
	}
	//�@�ȏ�A�l�͓����Ă���悤��
	int katoon =0;
	//���������āACreateMatk2t���R�s�[�����֐��ŁAVecf2?�������

	//>	�M�ʂ́Adqdtds��p����

	//> r <= <= R�̒��S������ˏ�ɉ��M

	//	�ߓ_��dqdt�̒l���X�V����

	//�@�ȉ��́A�x�N�g�������֐��̎d��
	//	�ߓ_�̑�����\�ʂ̖ʂŁA�v�Z����
	//  vertices[].heatFluxValue����Ɍv�Z��i�߂�
	//	�K�E�X�U�C�f���v�Z�ł���悤�ɏ����ȂǁA��������
}

Vec2d PHFemMeshThermo::GetIHbandDrawVtx(){
	return IHLineVtxX;
}

void PHFemMeshThermo::SetIHbandDrawVtx(double xS, double xE){
	IHLineVtxX = Vec2d(xS,xE);
}

void PHFemMeshThermo::CalcIHdqdtband_(double xS,double xE,double dqdtAll){
	///	x���W�FxS~xE�̊Ԃ̐ߓ_�ɔM�������E������ݒ�
	// xS,��E�̊Ԃɂ����ꂩ��_������A
	// 
	// mayIHheated�͎g��Ȃ�	:���̏������ŁAface�̑S�ߓ_��y���W�����̂��̂ɂ��Ă̂݁AfacesS�ɉ��Z

	//> ���M����l�ʑ̖ʂ̖ʐς̑��a�����߂�
	double faceS = 0.0;			// initialize
	for(unsigned i=0;i < nSurfaceFace; i++){
		//if(faces[i].mayIHheated){			// face�̐ߓ_��y���W�����̏ꍇ��IH���M�̑Ώېߓ_
			unsigned nObinnerVtx = 0;
			if(faces[i].area==0) faces[i].area = CalcTriangleArea(faces[i].vertices[0],faces[i].vertices[1],faces[i].vertices[2]);
			//..�S�ߓ_��y<0�Ȃ�Afluxarea�ɉ��Z
			if(vertices[faces[i].vertices[0]].pos.y < 0 && vertices[faces[i].vertices[1]].pos.y < 0 && vertices[faces[i].vertices[2]].pos.y < 0){
				for(unsigned j=0;j<3;j++){
					// ||.. :x, |.|.:o , .|.|:o ,   .||.:o  , ..||:x 
					//. �ǂꂩ�P�_��xS~xE�̊Ԃɂ���A���́A�ׂ荇��2�_���o���h�̊O���ɂ���AIHarea�ɎZ��
					if( xS <= vertices[faces[i].vertices[j]].pos.x && vertices[faces[i].vertices[j]].pos.x <= xE){
						faces[i].fluxarea = faces[i].area;
						break;
					}
					//.���_�΂��o���h�����ނƂ�
					if(vertices[faces[i].vertices[j]].pos.x <= xS && xE <= vertices[faces[i].vertices[(j+1)%3]].pos.x 
						|| vertices[faces[i].vertices[(j+1)%3]].pos.x <= xS && xE <= vertices[faces[i].vertices[j]].pos.x ){
							faces[i].fluxarea = faces[i].area;
						break;
					}
				}
			}
			if(faces[i].fluxarea >= 0){	
				faceS += faces[i].fluxarea;
			}else{		assert(0);	}		//	faces[i].fluxarea��0�����̐����������Ă���̂ɉ��Z���悤�Ƃ��Ă���
			//DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
		//}
	}

	//..face�ʐςɉ������M������S�̖̂ʐϊ�����苁�߂�
	if(faceS > 0){
		//> dqdt ��P�ʖʐς�����ɒ���([1/m^2])
		//double dqdt_ds = dqdtAll / faceS;
//		DSTR << "dqdtds:  " << dqdtds << std::endl;
		//>	�ȉ��A�M������faces�Ɋi�[����
		//>	�M�����̖ʐόv�Z��fluxarea��p���čs��
		for(unsigned i=0;i < nSurfaceFace; i++){
			if(faces[i].fluxarea){
				faces[i].heatflux = faces[i].fluxarea / faceS * dqdtAll;		//	�M�����̗ʂ�heatflux�̖ʐς���v�Z
//				DSTR << "faces[" << i <<"].heatflux: " << faces[i].heatflux <<std::endl;			
				// debug
				//for(unsigned j = 0; j < 3;++j){
 				//	vertices[faces[i].vertices[j]].temp = 100.0;
				//}
			}
		}
	}

	//	�f�o�b�O�E�\���p
	SetIHbandDrawVtx(xS,xE);
}

void PHFemMeshThermo::CalcIHdqdtband(double xS,double xE,double dqdtAll){
	///	x���W�FxS~xE�̊Ԃ̐ߓ_�ɔM�������E������ݒ�

	//> ���M����l�ʑ̖ʂ̖ʐς̑��a�����߂�
	double faceS = 0.0;
	for(unsigned i=0;i < nSurfaceFace; i++){
		if(faces[i].mayIHheated){			// face�̐ߓ_��y���W�����̏ꍇ��IH���M�̑Ώېߓ_
			unsigned nObinnerVtx = 0;
			if(faces[i].area==0) faces[i].area = CalcTriangleArea(faces[i].vertices[0],faces[i].vertices[1],faces[i].vertices[2]);
			for(unsigned j=0;j<3;j++){
				if( xS <= vertices[faces[i].vertices[j]].pos.x && vertices[faces[i].vertices[j]].pos.x <= xE){
					nObinnerVtx += 1;
				}
			}
			if( nObinnerVtx == 1)			faces[i].fluxarea = faces[i].area;//faces[i].fluxarea = 1.0/3.0 * faces[i].area;
			else if(nObinnerVtx == 2)		faces[i].fluxarea = faces[i].area;//faces[i].fluxarea = 2.0/3.0 * faces[i].area;
			else if(nObinnerVtx == 3)		faces[i].fluxarea = faces[i].area;
			else if(nObinnerVtx == 0)		faces[i].fluxarea = 0.0;

			if(faces[i].fluxarea >= 0){	
				faceS += faces[i].fluxarea;
			}else{		assert(0);	}		//	faces[i].fluxarea��0�����̐����������Ă���̂ɉ��Z���悤�Ƃ��Ă���
			//DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
		}
	}

	//for(unsigned i=0;i < nSurfaceFace; i++){
	//	DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
	//}



	if(faceS > 0){
		//> dqdt ��P�ʖʐς�����ɒ���([1/m^2])
		double dqdtds = dqdtAll / faceS;
//		DSTR << "dqdtds:  " << dqdtds << std::endl;
		//>	�ȉ��A�M������faces�Ɋi�[����
		//>	�M�����̖ʐόv�Z��fluxarea��p���čs��
		for(unsigned i=0;i < nSurfaceFace; i++){
			if(faces[i].mayIHheated){
				faces[i].heatflux = dqdtds * faces[i].fluxarea;		//	�M�����̗ʂ�heatflux�̖ʐς���v�Z
//				DSTR << "faces[" << i <<"].heatflux: " << faces[i].heatflux <<std::endl;
			}
		}
	}
	
	//�@�ȏ�A�l�͓����Ă���悤��

	int katoon =0;

	}

void PHFemMeshThermo::CalcIHdqdt_atleast(double r,double R,double dqdtAll){
	///	�����a�ƊO���a�̊Ԃ̐ߓ_�ɔM�������E������ݒ�
	//	�k��V���{�F��F�O�R�����g
	//	�����ł��̈�ɂ������Ă���΁AIH���M�Ɋ܂߂�

	//> ���M����l�ʑ̖ʂ̖ʐς̑��a�����߂�
	double faceS = 0.0;
	for(unsigned i=0;i < nSurfaceFace; i++){
		if(faces[i].mayIHheated){			// face�̐ߓ_��y���W�����̏ꍇ��IH���M�̑Ώېߓ_
			unsigned nObinnerVtx = 0;
			if(faces[i].area==0) faces[i].area = CalcTriangleArea(faces[i].vertices[0],faces[i].vertices[1],faces[i].vertices[2]);
			for(unsigned j=0;j<3;j++){
				if( r <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= R){
					//nObinnerVtx += 1;
					faces[i].fluxarea = faces[i].area;
					break;
				}
			}
			//if( nObinnerVtx == 1)			faces[i].fluxarea = faces[i].area;//faces[i].fluxarea = 1.0/3.0 * faces[i].area;
			//else if(nObinnerVtx == 2)		faces[i].fluxarea = faces[i].area;//faces[i].fluxarea = 2.0/3.0 * faces[i].area;
			//else if(nObinnerVtx == 3)		faces[i].fluxarea = faces[i].area;
			//else if(nObinnerVtx == 0)		faces[i].fluxarea = 0.0;

			//if(faces[i].fluxarea >= 0){	
			faceS += faces[i].fluxarea;
			//}else{		assert(0);	}		//	faces[i].fluxarea��0�����̐����������Ă���̂ɉ��Z���悤�Ƃ��Ă���
			//DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
		}
	}

	//for(unsigned i=0;i < nSurfaceFace; i++){
	//	DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
	//}

	if(faceS > 0){
		//> dqdt ��P�ʖʐς�����ɒ���([1/m^2])
		double dqdtds = dqdtAll / faceS;
//		DSTR << "dqdtds:  " << dqdtds << std::endl;
		//>	�ȉ��A�M������faces�Ɋi�[����
		//>	�M�����̖ʐόv�Z��fluxarea��p���čs��
		for(unsigned i=0;i < nSurfaceFace; i++){
			if(faces[i].mayIHheated){
				faces[i].heatflux = dqdtds * faces[i].fluxarea;		//	�M�����̗ʂ�heatflux�̖ʐς���v�Z
//				DSTR << "faces[" << i <<"].heatflux: " << faces[i].heatflux <<std::endl;
			}
		}
	}
	
	//�@�ȏ�A�l�͓����Ă���悤��

	int katoon =0;
	//���������āACreateMatk2t���R�s�[�����֐��ŁAVecf2?�������

	//>	�M�ʂ́Adqdtds��p����

	//> r <= <= R�̒��S������ˏ�ɉ��M

	//	�ߓ_��dqdt�̒l���X�V����

	//�@�ȉ��́A�x�N�g�������֐��̎d��
	//	�ߓ_�̑�����\�ʂ̖ʂŁA�v�Z����
	//  vertices[].heatFluxValue����Ɍv�Z��i�߂�
	//	�K�E�X�U�C�f���v�Z�ł���悤�ɏ����ȂǁA��������

}

void PHFemMeshThermo::CalcIHdqdt(double r,double R,double dqdtAll){
	///	�����a�ƊO���a�̊Ԃ̐ߓ_�ɔM�������E������ݒ�

	//> ���M����l�ʑ̖ʂ̖ʐς̑��a�����߂�
	double faceS = 0.0;
	for(unsigned i=0;i < nSurfaceFace; i++){
		if(faces[i].mayIHheated){			// face�̐ߓ_��y���W�����̏ꍇ��IH���M�̑Ώېߓ_
			unsigned nObinnerVtx = 0;
			if(faces[i].area==0) faces[i].area = CalcTriangleArea(faces[i].vertices[0],faces[i].vertices[1],faces[i].vertices[2]);
			for(unsigned j=0;j<3;j++){
				if( r <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= R){
					nObinnerVtx += 1;
				}
			}
			if( nObinnerVtx == 1)			faces[i].fluxarea = 1.5/3.0 * faces[i].area;//faces[i].fluxarea = 1.0/3.0 * faces[i].area;
			else if(nObinnerVtx == 2)		faces[i].fluxarea = 2.8/3.0 * faces[i].area;//faces[i].fluxarea = 2.0/3.0 * faces[i].area;
			else if(nObinnerVtx == 3)		faces[i].fluxarea = faces[i].area;
			else if(nObinnerVtx == 0)		faces[i].fluxarea = 0;

			if(faces[i].fluxarea >= 0){	
				faceS += faces[i].fluxarea;
			}else{		assert(0);	}		//	faces[i].fluxarea��0�����̐����������Ă���̂ɉ��Z���悤�Ƃ��Ă���
			//DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
		}
	}

	//for(unsigned i=0;i < nSurfaceFace; i++){
	//	DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
	//}

	if(faceS > 0){
		//> dqdt ��P�ʖʐς�����ɒ���([1/m^2])
		double dqdtds = dqdtAll / faceS;
//		DSTR << "dqdtds:  " << dqdtds << std::endl;
		//>	�ȉ��A�M������faces�Ɋi�[����
		//>	�M�����̖ʐόv�Z��fluxarea��p���čs��
		for(unsigned i=0;i < nSurfaceFace; i++){
			if(faces[i].mayIHheated){
				faces[i].heatflux = dqdtds * faces[i].fluxarea;		//	�M�����̗ʂ�heatflux�̖ʐς���v�Z
//				DSTR << "faces[" << i <<"].heatflux: " << faces[i].heatflux <<std::endl;
			}
		}
	}
	
	//�@�ȏ�A�l�͓����Ă���悤��

	int katoon =0;
	//���������āACreateMatk2t���R�s�[�����֐��ŁAVecf2?�������

	//>	�M�ʂ́Adqdtds��p����

	//> r <= <= R�̒��S������ˏ�ɉ��M

	//	�ߓ_��dqdt�̒l���X�V����

	//�@�ȉ��́A�x�N�g�������֐��̎d��
	//	�ߓ_�̑�����\�ʂ̖ʂŁA�v�Z����
	//  vertices[].heatFluxValue����Ɍv�Z��i�߂�
	//	�K�E�X�U�C�f���v�Z�ł���悤�ɏ����ȂǁA��������

}
/// face���ɍ���Ă��܂��̂��ǂ��̂��Avertices���Ƃɂ��̂������̂��B�ǂ�����������������Ȃ��̂ŁA�ЂƂ܂��Avertices���ɍ���Ă��܂���

void PHFemMeshThermo::SetVertexHeatFlux(int id,double heatFlux){
	vertices[id].heatFluxValue = heatFlux;
}

void PHFemMeshThermo::SetVtxHeatFluxAll(double heatFlux){
	for(unsigned i=0; i < vertices.size() ;i++){
		SetVertexHeatFlux(i,heatFlux);
	}
}

void PHFemMeshThermo::CalcHeatTransUsingGaussSeidel(unsigned NofCyc,double dt){
	//dt = 0.0000000000001 * dt;		//�f�o�b�O�p�ɁAdt�����̂��������������Ă��A�ߓ_0���}�C�i�X�ɂȂ�̂��A���ׂ�
	
	//dt��PHFemEngine.cpp�Ŏ擾���铮�͊w�V�~�����[�V�����̃X�e�b�v����
	bool DoCalc =true;											//���񂾂��萔�x�N�g��b�̌v�Z���s��bool		//NofCyc��0�̎��ɂ���΂����̂���
	std::ofstream ofs("log.txt");
	for(unsigned i=0; i < NofCyc; i++){							//�K�E�X�U�C�f���̌v�Z���[�v
		if(DoCalc){		
			if(deformed || alphaUpdated){												//D_ii�̍쐬�@�`���M�`�B�����X�V���ꂽ�ۂ�1�x�����s���΂悢
				for(unsigned j =0; j < vertices.size() ; j++){
					//for(unsigned k =0;k < vertices.size(); k++){
					//	DSTR << "dMatCAll "<< k << " : " << dMatCAll[0][k] << std::endl;
					//}
					_dMatAll.resize(1,vertices.size());
					_dMatAll[0][j] = 1.0/ ( 1.0/2.0 * dMatKAll[0][j] + 1.0/dt * dMatCAll[0][j] );		//1 / D__ii	�����߂�
					//1.0/dt = 500 d
					//DSTR << "dMatKAll : "  << dMatKAll << std::endl;
					//DSTR << "dMatCAll : "  << dMatCAll << std::endl;
					//DSTR << "1.0/dt : " << 1.0/dt <<std::endl;
					//DSTR <<  1.0/dt *dMatCAll[0][j] << std::endl;		//0.001�̃I�[�_�[
					//DSTR << 1.0/2.0 * dMatKAll[0][j] << std::endl;		//0.0003�O��̃I�[�_�[
					//�l�������Ă��邩���`�F�b�N
					//DSTR << "_dMatAll[0][" << j << "] : " << _dMatAll[0][j]  << std::endl;
					int debughogeshi =0;
				}
				deformed = false;
				alphaUpdated =false;
				//	for DEBUG
				//DSTR << "_dMatAll : " << std::endl;
				//for(unsigned j =0; j < vertices.size() ;j++){
				//	DSTR << j << " : " << _dMatAll[0][j] << std::endl;
				//}
				//int hogeshi=0;
			}
			//	 1      1        1  
			//	--- ( - - [K] + ---[C] ){T(t)} + {F} 
			//	D_jj    2       ��t
			//

			for(unsigned j =0; j < vertices.size() ; j++){		//���񃋁[�v����	�W���x�N�g��bVecAll�̐������v�Z
				bVecAll[j][0] = 0.0;							//bVecAll[j][0]�̏�����
				//�ߓ_������edges���Ɂ@�Ίp����(j,j)�Ɣ�Ίp����(j,?)���Ɍv�Z
				//�Ίp�����́Avertices[j].k or .c �ɓ����Ă���l���A��Ίp������edges[hoge].vertices[0] or vertices[1] .k or .c�ɓ����Ă���l��p����
				//�@)��Ίp�����ɂ���
				for(unsigned k =0;k < vertices[j].edges.size() ; k++){
					unsigned edgeId = vertices[j].edges[k];
					//���t�@�N�^�����O	�ȉ��̏�������ɂ���j>edges[edgeId].vertices[0] �Ƃ����łȂ����Ƃŕ������ق����R�ꂪ�o��S�z�͂Ȃ��H
					if( j != edges[edgeId].vertices[0]){					//�ߓ_�ԍ�j��edges.vertices[0]���قȂ�ߓ_�ԍ��̎�:��Ίp����
						unsigned vtxid0 = edges[edgeId].vertices[0];
						bVecAll[j][0] += (-1.0/2.0 * edges[edgeId].k + 1.0/dt * edges[edgeId].c ) * TVecAll[vtxid0];
					}
					else if( j != edges[edgeId].vertices[1] ){			//�ߓ_�ԍ�j��edges.vertices[1]���قȂ�ߓ_�ԍ��̎�:��Ίp����
						unsigned vtxid1 = edges[edgeId].vertices[1];
						bVecAll[j][0] += (-1.0/2.0 * edges[edgeId].k + 1.0/dt * edges[edgeId].c ) * TVecAll[vtxid1];

					}
					else{
						//��L�̂ǂ���ł��Ȃ��ꍇ�A�G���[
						DSTR << "edges.vertex has 3 vertexies or any other problem" <<std::endl;
					}
					//	for Debug
					//DSTR << "edges[" << edgeId << "].vertices[0] : " << edges[edgeId].vertices[0] << std::endl;
					//DSTR << "edges[" << edgeId << "].vertices[1] : " << edges[edgeId].vertices[1] << std::endl;
					//int hogeshi =0;
				}
				//�A)�Ίp�����ɂ���
				bVecAll[j][0] += (-1.0/2.0 * dMatKAll[0][j] + 1.0/dt * dMatCAll[0][j] ) * TVecAll[j];
				ofs << "bVecAll[" << j <<"][0] : " << bVecAll[j][0] << std::endl;			// DSTR
				//{F}�����Z
				bVecAll[j][0] += vecFAll[j][0];		//F�����Z
				//DSTR << " vecFAll[" << j << "][0] : "  << vecFAll[j][0] << std::endl;
				//DSTR << std::endl;
				//D_ii�Ŋ��� �˂��̏ꏊ�́A�����ŗǂ��́H�ǂ��܂Ŋ|���Z����́H
				bVecAll[j][0] = bVecAll[j][0] * _dMatAll[0][j];
				ofs << "bVecAll[" << j <<"][0] * _dMatAll : " << bVecAll[j][0] << std::endl;
				//	DSTR <<  "bVecAll[" << j <<"][0] * _dMatAll : " << bVecAll[j][0] << std::endl;
				ofs << "TVecAll[" << j <<"] : " << TVecAll[j] << std::endl;
				//	DSTR << "TVecAll[" << j <<"] : " << TVecAll[j] << std::endl;
			}
			DoCalc = false;			//����̃��[�v�����ŗ��p
			//�l�������Ă��邩�A���킻�������`�F�b�N
			//DSTR << "bVecAll[j][0] : " << std::endl;
			//for(unsigned j =0;j <vertices.size() ; j++){
			//	DSTR << j << " : "<< bVecAll[j][0] << std::endl;
			//}
			int debughogeshi =0;
		}		//if(DoCalc){...}

#ifdef DEBUG
		//	�O�̂��߁A�v�Z�O�̏������x��0�ɂ��Ă���B
		if(i == 0){
				for(unsigned j=0;j <vertices.size() ;j++){
					TVecAll[j] = 0.0;
				}
		}
#endif
		//	 1      
		//	--- [F]{T(t+dt)}
		//	D_jj 		
		//[F] = 1/2 [K] +1/dt [C] ����Ίp����������(-1)������������
		//�G�b�W�ɓ����Ă��鐬����-1��������̂ł͂Ȃ��A�Ō��-1��������B
		//
		for(unsigned j =0; j < vertices.size() ; j++){
			//T(t+dt) = �̎�
			//	�܂�tempkj�����
			double tempkj = 0.0;			//�K�E�X�U�C�f���̓r���v�Z�ŏo�Ă���F�̐����v�Z�Ɏg�p����ꎞ�ϐ�
			for(unsigned k =0;k < vertices[j].edges.size() ; k++){
				unsigned edgeId = vertices[j].edges[k]; 
				if( j != edges[edgeId].vertices[0]){					//�ߓ_�ԍ�j��edges.vertices[0]���قȂ�ߓ_�ԍ��̎�:��Ίp����		//OK
					unsigned vtxid0 = edges[edgeId].vertices[0];
					//DSTR << "TVecAll["<< vtxid0<<"] : " << TVecAll[vtxid0] <<std::endl;
					//TVecAll[j] +=_dMatAll[j][0] * -(1.0/2.0 * edges[edgeId].k + 1.0/dt * edges[edgeId].c ) * TVecAll[vtxid0] + bVecAll[j][0]; 
					//DSTR << "j : " << j << ", vtxid0 : " << vtxid0 <<", edges[edgeId].vertices[0] : " << edges[edgeId].vertices[0] <<  std::endl;
					tempkj += (1.0/2.0 * edges[edgeId].k + 1.0/dt * edges[edgeId].c ) * TVecAll[vtxid0];
				}
				else if( j != edges[edgeId].vertices[1] ){			//�ߓ_�ԍ�j��edges.vertices[1]���قȂ�ߓ_�ԍ��̎�:��Ίp����
					unsigned vtxid1 = edges[edgeId].vertices[1];
					//DSTR << "TVecAll["<< vtxid1<<"] : " << TVecAll[vtxid1] <<std::endl;
					tempkj += (1.0/2.0 * edges[edgeId].k + 1.0/dt * edges[edgeId].c ) * TVecAll[vtxid1];
				}
				else{
					//��L�̂ǂ���ł��Ȃ��ꍇ�A�G���[
					DSTR << "edges.vertex has 3 vertexies or any other problem" <<std::endl;
				}
				//	for Debug
				//DSTR << "TVecAll:"
				//DSTR << "edges[" << edgeId << "].vertices[0] : " << edges[edgeId].vertices[0] << std::endl;
				//DSTR << "edges[" << edgeId << "].vertices[1] : " << edges[edgeId].vertices[1] << std::endl;
				//int hogeshi =0;
			}
			//	TVecAll�̌v�Z
			TVecAll[j] =	_dMatAll[0][j] * ( -1.0 * tempkj) + bVecAll[j][0];			//	-b = D^(-1) [ (-1/2 * K + 1/dt * C ){T(t+dt)} + {F} ]�Ȃ̂ŁAbVecAll�͂����̉��Z�ł悢
			//TVecAll[j] =	_dMatAll[0][j] * ( -1.0 * tempkj) + bVecAll[j][0];   // -b = D^(-1) [ (-1/2 * K + 1/dt * C ){T(t+dt)} + {F} ]�Ȃ̂ŁAbVecAll�͂����̉��Z�ł悢
//			TVecAll[j] =	_dMatAll[0][j] * ( -1.0 * tempkj) + bVecAll[j][0];			//���̌v�Z�����ƁA�܂Ƃ������Ȓl���o�邪�E�E�E���_�I�ɂ͂ǂ��Ȃ̂��A������Ȃ��B�B�B
			////	for DEBUG
			//int hofgeshi =0;
			//if(TVecAll[j] != 0.0){
			//	DSTR << "!=0 TVecAll["<< j<<"] : " << TVecAll[j] <<std::endl;
			//}
			//DSTR << i << "��ڂ̌v�Z�A" << j <<"�s�ڂ�tempkj: " << tempkj << std::endl;
			//tempkj =0.0;
			ofs << j << std::endl;
			ofs << "tempkj: "<< tempkj << std::endl;
			ofs << "DMatAll[0][j] * ( -1.0 * tempkj) :" <<_dMatAll[0][j] * ( -1.0 * tempkj) << std::endl;
			ofs << "bVecAll[j][0] :  " <<  bVecAll[j][0] << std::endl;
			ofs << "  TVecAll[j] : " << TVecAll[j] << std::endl;
			ofs << std::endl;
		}
		////	for Debug
		//for(unsigned j=0;j < vertices.size();j++){
		//	//DSTR << "tempk" << j << " : " << tempkj << std::endl;
		//	int hogeshi__ =0;
		//	//TVecAll[j]�̌v�Z���ʂ�������
		//	//�萔�x�N�g��b����Ōv�Z�A���s��bVecAll�����Z����΂悢�B
		//	DSTR << i << "��ڂ̌v�Z�� " << "TVecAll[" << j << "] : " << TVecAll[j] << std::endl;
		//}
		//for(unsigned j=0;j < vertices.size();j++){
		//	//DSTR << "tempk" << j << " : " << tempkj << std::endl;
		//	int hogeshi__ =0;
		//	//TVecAll[j]�̌v�Z���ʂ�������
		//	//�萔�x�N�g��b����Ōv�Z�A���s��bVecAll�����Z����΂悢�B
		//	DSTR << i << "��ڂ̌v�Z�� " << "bVecAll[" << j << "][0] : " << bVecAll[j][0] << std::endl;
		//}

		//DSTR << i <<  "th Cyc" << std::endl; 
		//DSTR << i << "��ڂ̌v�Z�ATVecAll : " <<std::endl;
		//DSTR << TVecAll << std::endl;
		ofs << i <<  "th Cyc" << std::endl;
		ofs << i << "��ڂ̌v�Z�ATVecAll : " <<std::endl;
		ofs << TVecAll << std::endl;
		ofs << "bVecAll: " <<std::endl;
		ofs << bVecAll << std::endl;
		ofs << "_dMatAll: " <<std::endl; 
		ofs << _dMatAll <<std::endl;
		int piyopiyoyo =0;
		double tempTemp=0.0;
		for(unsigned j=0;j <vertices.size() ; j++){
			tempTemp += TVecAll[j];
		}
		//	DSTR
		ofs << i <<"��ڂ̌v�Z���́@�S�ߓ_�̉��x�̘a : " << tempTemp << std::endl;
		ofs << std::endl;
	}
//	deformed = true;
}

void PHFemMeshThermo::CalcHeatTransUsingGaussSeidel(unsigned NofCyc,double dt,double eps){
	//dt = 0.0000000000001 * dt;		//�f�o�b�O�p�ɁAdt�����̂��������������Ă��A�ߓ_0���}�C�i�X�ɂȂ�̂��A���ׂ�
	double _eps = 1-eps;			// 1-eps�̌v�Z�ɗ��p
	//dt��PHFemEngine.cpp�Ŏ擾���铮�͊w�V�~�����[�V�����̃X�e�b�v����
	bool DoCalc =true;											//���񂾂��萔�x�N�g��b�̌v�Z���s��bool		//NofCyc��0�̎��ɂ���΂����̂���
	std::ofstream ofs("log.txt");
	for(unsigned i=0; i < NofCyc; i++){							//�K�E�X�U�C�f���̌v�Z���[�v
		if(DoCalc){												
			if(deformed || alphaUpdated ){												//D_ii�̍쐬�@�`�󂪍X�V���ꂽ�ۂ�1�x�����s���΂悢
				for(unsigned j =0; j < vertices.size() ; j++){
					//for(unsigned k =0;k < vertices.size(); k++){
					//	DSTR << "dMatCAll "<< k << " : " << dMatCAll[0][k] << std::endl;
					//}
					_dMatAll.resize(1,vertices.size());
					_dMatAll[0][j] = 1.0/ ( eps * dMatKAll[0][j] + 1.0/dt * dMatCAll[0][j] );		//1 / D__ii	�����߂�
					//1.0/dt = 500 d
					//DSTR << "dMatKAll : "  << dMatKAll << std::endl;
					//DSTR << "dMatCAll : "  << dMatCAll << std::endl;
					//DSTR << "1.0/dt : " << 1.0/dt <<std::endl;
					//DSTR <<  1.0/dt *dMatCAll[0][j] << std::endl;		//0.001�̃I�[�_�[
					//DSTR << 1.0/2.0 * dMatKAll[0][j] << std::endl;		//0.0003�O��̃I�[�_�[
					//�l�������Ă��邩���`�F�b�N
					//DSTR << "_dMatAll[0][" << j << "] : " << _dMatAll[0][j]  << std::endl;
					int debughogeshi =0;
				}
				deformed = false;
				//	for DEBUG
				//DSTR << "_dMatAll : " << std::endl;
				//for(unsigned j =0; j < vertices.size() ;j++){
				//	DSTR << j << " : " << _dMatAll[0][j] << std::endl;
				//}
				//int hogeshi=0;
			}
			//	 1      1        1  
			//	--- ( - - [K] + ---[C] ){T(t)} + {F} 
			//	D_jj    2       ��t
			//

			for(unsigned j =0; j < vertices.size() ; j++){		//���񃋁[�v����	�W���x�N�g��bVecAll�̐������v�Z
				bVecAll[j][0] = 0.0;							//bVecAll[j][0]�̏�����
				//�ߓ_������edges���Ɂ@�Ίp����(j,j)�Ɣ�Ίp����(j,?)���Ɍv�Z
				//�Ίp�����́Avertices[j].k or .c �ɓ����Ă���l���A��Ίp������edges[hoge].vertices[0] or vertices[1] .k or .c�ɓ����Ă���l��p����
				//�@)��Ίp�����ɂ���
				for(unsigned k =0;k < vertices[j].edges.size() ; k++){
					unsigned edgeId = vertices[j].edges[k];
					//���t�@�N�^�����O	�ȉ��̏�������ɂ���j>edges[edgeId].vertices[0] �Ƃ����łȂ����Ƃŕ������ق����R�ꂪ�o��S�z�͂Ȃ��H
					if( j != edges[edgeId].vertices[0]){					//�ߓ_�ԍ�j��edges.vertices[0]���قȂ�ߓ_�ԍ��̎�:��Ίp����
						unsigned vtxid0 = edges[edgeId].vertices[0];
						bVecAll[j][0] += (-_eps * edges[edgeId].k + 1.0/dt * edges[edgeId].c ) * TVecAll[vtxid0];
					}
					else if( j != edges[edgeId].vertices[1] ){			//�ߓ_�ԍ�j��edges.vertices[1]���قȂ�ߓ_�ԍ��̎�:��Ίp����
						unsigned vtxid1 = edges[edgeId].vertices[1];
						bVecAll[j][0] += (-_eps * edges[edgeId].k + 1.0/dt * edges[edgeId].c ) * TVecAll[vtxid1];

					}
					else{
						//��L�̂ǂ���ł��Ȃ��ꍇ�A�G���[
						DSTR << "edges.vertex has 3 vertexies or any other problem" <<std::endl;
					}
					//	for Debug
					//DSTR << "edges[" << edgeId << "].vertices[0] : " << edges[edgeId].vertices[0] << std::endl;
					//DSTR << "edges[" << edgeId << "].vertices[1] : " << edges[edgeId].vertices[1] << std::endl;
					//int hogeshi =0;
				}
				//�A)�Ίp�����ɂ���
				bVecAll[j][0] += (-_eps * dMatKAll[0][j] + 1.0/dt * dMatCAll[0][j] ) * TVecAll[j];
				ofs << "bVecAll[" << j <<"][0] : " << bVecAll[j][0] << std::endl;			// DSTR
				ofs << "dMatKAll[0][" << j <<"] : " << dMatKAll[0][j] << std::endl;			// DSTR
				ofs << "dMatCAll[0][" << j <<"] : " << dMatCAll[0][j] << std::endl;			// DSTR
				//{F}�����Z
				bVecAll[j][0] += vecFAll[j][0];		//F�����Z
				//DSTR << " vecFAll[" << j << "][0] : "  << vecFAll[j][0] << std::endl;
				//DSTR << std::endl;
				//D_ii�Ŋ��� �˂��̏ꏊ�́A�����ŗǂ��́H�ǂ��܂Ŋ|���Z����́H
				bVecAll[j][0] = bVecAll[j][0] * _dMatAll[0][j];
				ofs << "bVecAll[" << j <<"][0] * _dMatAll : " << bVecAll[j][0] << std::endl;
				//	DSTR <<  "bVecAll[" << j <<"][0] * _dMatAll : " << bVecAll[j][0] << std::endl;
				ofs << "TVecAll[" << j <<"] : " << TVecAll[j] << std::endl;
				//	DSTR << "TVecAll[" << j <<"] : " << TVecAll[j] << std::endl;
			}
			DoCalc = false;			//����̃��[�v�����ŗ��p
			//�l�������Ă��邩�A���킻�������`�F�b�N
			//DSTR << "bVecAll[j][0] : " << std::endl;
			//for(unsigned j =0;j <vertices.size() ; j++){
			//	DSTR << j << " : "<< bVecAll[j][0] << std::endl;
			//}
			int debughogeshi =0;
		}		//if(DoCalc){...}

#ifdef DEBUG
		//	�O�̂��߁A�v�Z�O�̏������x��0�ɂ��Ă���B
		if(i == 0){
				for(unsigned j=0;j <vertices.size() ;j++){
					TVecAll[j] = 0.0;
				}
		}
#endif
		//	 1      
		//	--- [F]{T(t+dt)}
		//	D_jj 		
		//[F] = eps(ilon) [K] +1/dt [C] ����Ίp����������(-1)������������
		//�G�b�W�ɓ����Ă��鐬����-1��������̂ł͂Ȃ��A�Ō��-1��������B
		//
		for(unsigned j =0; j < vertices.size() ; j++){
			//T(t+dt) = �̎�
			//	�܂�tempkj�����
			double tempkj = 0.0;			//�K�E�X�U�C�f���̓r���v�Z�ŏo�Ă���F�̐����v�Z�Ɏg�p����ꎞ�ϐ�
			for(unsigned k =0;k < vertices[j].edges.size() ; k++){
				unsigned edgeId = vertices[j].edges[k]; 
				if( j != edges[edgeId].vertices[0]){					//�ߓ_�ԍ�j��edges.vertices[0]���قȂ�ߓ_�ԍ��̎�:��Ίp����		//OK
					unsigned vtxid0 = edges[edgeId].vertices[0];
					//DSTR << "TVecAll["<< vtxid0<<"] : " << TVecAll[vtxid0] <<std::endl;
					//TVecAll[j] +=_dMatAll[j][0] * -(1.0/2.0 * edges[edgeId].k + 1.0/dt * edges[edgeId].c ) * TVecAll[vtxid0] + bVecAll[j][0]; 
					//DSTR << "j : " << j << ", vtxid0 : " << vtxid0 <<", edges[edgeId].vertices[0] : " << edges[edgeId].vertices[0] <<  std::endl;
					tempkj += (eps * edges[edgeId].k + 1.0/dt * edges[edgeId].c ) * TVecAll[vtxid0];
				}
				else if( j != edges[edgeId].vertices[1] ){			//�ߓ_�ԍ�j��edges.vertices[1]���قȂ�ߓ_�ԍ��̎�:��Ίp����
					unsigned vtxid1 = edges[edgeId].vertices[1];
					//DSTR << "TVecAll["<< vtxid1<<"] : " << TVecAll[vtxid1] <<std::endl;
					tempkj += (eps * edges[edgeId].k + 1.0/dt * edges[edgeId].c ) * TVecAll[vtxid1];
				}
				else{
					//��L�̂ǂ���ł��Ȃ��ꍇ�A�G���[
					DSTR << "edges.vertex has 3 vertexies or any other problem" <<std::endl;
				}
				//	for Debug
				//DSTR << "TVecAll:"
				//DSTR << "edges[" << edgeId << "].vertices[0] : " << edges[edgeId].vertices[0] << std::endl;
				//DSTR << "edges[" << edgeId << "].vertices[1] : " << edges[edgeId].vertices[1] << std::endl;
				//int hogeshi =0;
			}
			//	TVecAll�̌v�Z
			TVecAll[j] =	_dMatAll[0][j] * ( -1.0 * tempkj) + bVecAll[j][0];			//	-b = D^(-1) [ (-1/2 * K + 1/dt * C ){T(t+dt)} + {F} ]�Ȃ̂ŁAbVecAll�͂����̉��Z�ł悢
			//TVecAll[j] =	_dMatAll[0][j] * ( -1.0 * tempkj) + bVecAll[j][0];   // -b = D^(-1) [ (-1/2 * K + 1/dt * C ){T(t+dt)} + {F} ]�Ȃ̂ŁAbVecAll�͂����̉��Z�ł悢
//			TVecAll[j] =	_dMatAll[0][j] * ( -1.0 * tempkj) + bVecAll[j][0];			//���̌v�Z�����ƁA�܂Ƃ������Ȓl���o�邪�E�E�E���_�I�ɂ͂ǂ��Ȃ̂��A������Ȃ��B�B�B
			////	for DEBUG
			//int hofgeshi =0;
			//if(TVecAll[j] != 0.0){
			//	DSTR << "!=0 TVecAll["<< j<<"] : " << TVecAll[j] <<std::endl;
			//}
			//DSTR << i << "��ڂ̌v�Z�A" << j <<"�s�ڂ�tempkj: " << tempkj << std::endl;
			//tempkj =0.0;
			ofs << j << std::endl;
			ofs << "tempkj: "<< tempkj << std::endl;
			ofs << "DMatAll[0][j] * ( -1.0 * tempkj) :" <<_dMatAll[0][j] * ( -1.0 * tempkj) << std::endl;
			ofs << "bVecAll[j][0] :  " <<  bVecAll[j][0] << std::endl;
			ofs << "  TVecAll[j] : " << TVecAll[j] << std::endl;
			ofs << std::endl;
		}
		////	for Debug
		//for(unsigned j=0;j < vertices.size();j++){
		//	//DSTR << "tempk" << j << " : " << tempkj << std::endl;
		//	int hogeshi__ =0;
		//	//TVecAll[j]�̌v�Z���ʂ�������
		//	//�萔�x�N�g��b����Ōv�Z�A���s��bVecAll�����Z����΂悢�B
		//	DSTR << i << "��ڂ̌v�Z�� " << "TVecAll[" << j << "] : " << TVecAll[j] << std::endl;
		//}
		//for(unsigned j=0;j < vertices.size();j++){
		//	//DSTR << "tempk" << j << " : " << tempkj << std::endl;
		//	int hogeshi__ =0;
		//	//TVecAll[j]�̌v�Z���ʂ�������
		//	//�萔�x�N�g��b����Ōv�Z�A���s��bVecAll�����Z����΂悢�B
		//	DSTR << i << "��ڂ̌v�Z�� " << "bVecAll[" << j << "][0] : " << bVecAll[j][0] << std::endl;
		//}

		//DSTR << i <<  "th Cyc" << std::endl; 
		//DSTR << i << "��ڂ̌v�Z�ATVecAll : " <<std::endl;
		//DSTR << TVecAll << std::endl;
		ofs << i <<  "th Cyc" << std::endl;
		ofs << i << "��ڂ̌v�Z�ATVecAll : " <<std::endl;
		ofs << TVecAll << std::endl;
		ofs << "bVecAll: " <<std::endl;
		ofs << bVecAll << std::endl;
		ofs << "_dMatAll: " <<std::endl; 
		ofs << _dMatAll <<std::endl;
		int piyopiyoyo =0;
		double tempTemp=0.0;
		for(unsigned j=0;j <vertices.size() ; j++){
			tempTemp += TVecAll[j];
		}
		//	DSTR
		ofs << i <<"��ڂ̌v�Z���́@�S�ߓ_�̉��x�̘a : " << tempTemp << std::endl;
		ofs << std::endl;
	}
//	deformed = true;
}

void PHFemMeshThermo::UpdateVertexTempAll(unsigned size){
	for(unsigned i=0;i < size;i++){
		vertices[i].temp = TVecAll[i];
	}
}
void PHFemMeshThermo::UpdateVertexTemp(unsigned vtxid){
		vertices[vtxid].temp = TVecAll[vtxid];
}

void PHFemMeshThermo::TexChange(unsigned id,double tz){

}

void PHFemMeshThermo::HeatTransFromPanToFoodShell(){
	//if(pan){
	//	Affinef afPan = pan->GetGRFrame()->GetWorldTransform();
	//	Affinef afMesh = tmesh.obj->GetGRFrame()->GetWorldTransform();
	//	Affinef afMeshToPan = afPan.inv() * afMesh;	
	//}

	//shape pair solid pair
	

	//	2���̂̐ڐG�ʂ���A���M����ߓ_�����߂�B
	//	Shape pair ��Solid pare�ӂ�ɋL�q������

	//	�ŊO�k�̐ߓ_�𐢊E���W�ɕϊ����A���̃t���C�p�����́A�S����ɂ������W�n�ɕϊ��������W�l���A������W�ȉ��Ȃ�
	//	�ŊO�k�̐ߓ_�ɔM�`�B����
	//PHSolidIf* phs ;
//	Affinef afPan = phs
	//Affinef afPan = pan->GetGRFrame()->GetWorldTransform();
	//Affinef afMesh = tmesh.obj->GetGRFrame()->GetWorldTransform();
	//Affinef afMeshToPan = afPan.inv() * afMesh;	
//	for(unsigned i=-0; i < surfaceVertices.size();i++){
//		if(vertices[surfaceVertices[i]].pos){};
//		//vertices[surfaceVertices[i]].Tc;
//	}

	//	�ڐG�ʂ���̋�������苗���ȓ��Ȃ�

	//	�M�`�B���E�����ŁA�M�`�B

	//	�M�`�B���E�����ŔM�`�B��A�t���C�p���̔M�͋z�M�Ƃ������A���邪�A����͗L���v�f�@���ǂ̂悤�Ɍ������Ă��΂����̂��H
	
	//	�ߓ_���͂�Tc�ƔM�`�B�����ɂ���āA�M���`���̂ŁA�����ŁA�t���C�p������M��`�������ߓ_��Tc�ƔM�`�B����ݒ肷��

	//	�M�`�B���́A����Ƃ̊֌W�ɂ���ĈقȂ�̂ŁA�ߓ_�����Ɛڂ��Ă��邩�ɂ���ĕύX����


}

void PHFemMeshThermo::DrawEdge(unsigned id0, unsigned id1){
	//Vec3d pos0 = vertices[id0].pos;
	//Vec3d pos1 = vertices[id1].pos;
	//
	//Vec3d wpos0 = 
	//	GetWorldTransform() * pos0; //* ���[�J�����W�� ���E���W�ւ̕ϊ����đ��
	//Vec3d wpos1 = GetWorldTransform() * pos1; //* ���[�J�����W�� ���E���W�ւ̕ϊ����đ��
	//glColor3d(1.0,0.0,0.0);
	//glBegin(GL_LINES);
	//	glVertex3f(wpos0[0],wpos0[1],wpos0[2]);
	//	glVertex3f(wpos0[0] + wpos1[0], wpos0[1] + wpos1[1], wpos0[2] + wpos1[2]);
	//glEnd();
	//glFlush();
}

void PHFemMeshThermo::Step(double dt){

	// cps�\���p
	static bool bOneSecond = false;
	{
		static DWORD lastTick = GetTickCount();
		static int cpsCount = 0;
		int ellapsed = GetTickCount() - lastTick;
		++cpsCount;
		bOneSecond = false;
		if (ellapsed > 1000) {
			std::cout << "cps : " << cpsCount << std::endl;
			cpslog << StepCount<< "," << cpsCount << "," ;	
			lastTick = GetTickCount();
			cpsCount = 0;
			bOneSecond = true;
		}
		if(cpsCount){	cpstime	= 1 / cpsCount;		}
	}
	static DWORD stepStart = GetTickCount();
	//�r������
	if (bOneSecond) {
		std::cout << "1: " << GetTickCount() - stepStart << std::endl;
		cpslog << GetTickCount() - stepStart << ",";
	} 
	//stepStart = GetTickCount();
	//�r������
	//if (bOneSecond) { std::cout << "1: " << GetTickCount() - stepStart << std::endl; }
	// %%% CPS�\��

	//std::ofstream templog("templog.txt");

	std::ofstream ofs_("log_.txt");
//	ScilabTest();									//	Scilab���g���e�X�g
	//���E������ݒ�:���x�̐ݒ�
//	UsingFixedTempBoundaryCondition(0,200.0);

	//	�����p�̃R�[�h
	//	a) ���ԗ��U�����ɑ�`�������p�A�O�i�E��ލ����ŐU��������M����
	//if(StepCount == 0){
	//	unsigned texid_ =7;
	//	UsingFixedTempBoundaryCondition(texid_,200.0);
	//}

	//	b) �f�M�ߒ��̎���
	//	�M�`�B����0�ɂ���B���x�Œ苫�E�����ŉ��M�B

	//	UsingFixedTempBoundaryCondition(3,50.0);
	//for(unsigned i=0 ;i<1;i++){
	//	UsingFixedTempBoundaryCondition(i,200.0);
	//}

	//%%%%		�M�`�B���E����		%%%%//
	//	�H�ރ��b�V���̕\�ʂ̐ߓ_�ɁA���̗͂��̉��x��^����
	//	���̗͂��̉��x�́A�t���C�p���̕\�ʉ��x��A�H�ނ�UsingFixedTempBoundaryCondition(0,200.0);�t�̓��̉��x�̕��z����A���̏ꏊ�ł̎��͗��̉��x�𔻕ʂ���B
	//	�ʒu���W���画�ʂ���R�[�h�������ɋL�q
	//UsingHeatTransferBoundaryCondition(unsigned id,double temp);
	//�G�l���M�[�ۑ������A���͗��̉��x�̒ቺ��A���̂ւ̋����M�ʂ͐��������ׂ�

	//for(unsigned i =0; i < 2; i++){
	//	UsingHeatTransferBoundaryCondition(surfaceVertices[i],200.0);
	//}
//	UsingHeatTransferBoundaryCondition(7,200.0);
//	UsingHeatTransferBoundaryCondition(0,200.0);
	//for(unsigned i =0; i < surfaceVertices.size(); i++){
	//	UsingHeatTransferBoundaryCondition(surfaceVertices[i],150.0);
	//}
	//DSTR << "vecFAll : " <<std::endl;
	//DSTR << vecFAll << std::endl;

	//	test�@shapepair������Ă���
	//GetScene()->
	
	//dt = dt *0.01;		�덷1�x���x�ɂȂ�
	//dt = dt;				�����������́A�v�Z�덷�H�i�}�C�i�X�ɂȂ��Ă���ߓ_���x������Ȃ�ɑ傫���Ȃ�B�j
	
	///>	�K�E�X�U�C�f���@�̐ݒ�
	//	CalcHeatTransUsingGaussSeidel(20,dt);			//�K�E�X�U�C�f���@�ŔM�`���v�Z�������@�N�����N�j�R���\���@�݂̂��g�������ꍇ

//	dNdt = 10.0 * dt;
		CalcHeatTransUsingGaussSeidel(1,dt,1.0);			//�K�E�X�U�C�f���@�ŔM�`���v�Z������ ��O�����́A�O�i�E�N�����N�j�R���\���E��ސϕ��̂����ꂩ�𐔒l�őI��

		//���x��\�����Ă݂�
		//DSTR << "vertices[3].temp : " << vertices[3].temp << std::endl;

		//���x�̃x�N�g������ߓ_�։��x�̔��f
		UpdateVertexTempAll((unsigned)vertices.size());
	//for(unsigned i =0;i<vertices.size();i++){
	//	DSTR << "vertices[" << i << "].temp : " << vertices[i].temp << std::endl;
	//}
	int templogkatoon =0;

	for(unsigned i =0;i<vertices.size();i++){
		if(vertices[i].temp !=0){
			ofs_ << "vertices[" << i << "].temp : " << vertices[i].temp << std::endl;
		}
	}
	int hogehoge=0;

	//	�ߓ_�̉��x�̐��ڗ����̕ۑ�
	if(StepCount ==0){
		templog <<"�X�e�b�v��"<<",";
		templog <<"�M�V�~�����[�V��������"<<",";
		for(unsigned i=0; i<vertices.size();i++){
			templog << "�ߓ_" << i << "�̉��x" << ",";
		}
		templog <<"," << std::endl;
		}
	templog << StepCount << ", ";
	templog << StepCount * dt << ", ";
	for(unsigned i=0; i<vertices.size();i++){
		templog << vertices[i].temp << ",";
	}
	templog <<"," << std::endl;
	//templog;
	//�ߓ_���x����ʂɕ\�������3�����e�N�X�`�����g�����e�N�X�`���؂�ւ��ɒl��n���ˊ֐���

	//DSTR << "TVecAll : " <<std::endl;
	//DSTR << TVecAll << std::endl;
	//DSTR << "bVecAll : " <<std::endl;
	//DSTR << bVecAll << std::endl;


	//	for	DEBUG	�ߓ_3�ƃG�b�W�΂����ߓ_��\��
	//for(unsigned i =0; i < vertices[3].edges.size();i++){
	//	DSTR << "edges[vertices[3].edges[" << i << "]].vertices[0] : " << edges[vertices[3].edges[i]].vertices[0] << std::endl;
	//	DSTR << "edges[vertices[3].edges[" << i << "]].vertices[1] : " << edges[vertices[3].edges[i]].vertices[1] << std::endl;
	//}
	//for(unsigned i =0; i < vertices[3].edges.size();i++){
	//	DSTR << "vertices[3].edges[" << i << "] : " << vertices[3].edges[i] << std::endl;
	//}
	int hogeshi = 0;
	//	for DEBUG
	//�i�`�󂪕ς������A�}�g���N�X��x�N�g������蒼���j
	//���x�ω��E�ŐV�̎��Ԃ�{T}�c�x�N�g���ɋL�ڂ���Ă���ߓ_���x����ɉ��w�ω��V�~�����[�V�������s��
		//SetChemicalSimulation();
		//���w�ω��V�~�����[�V�����ɕK�v�ȉ��x�Ȃǂ̃p�����[�^��n��
	//���x�ω��≻�w�V�~�����[�V�����̌��ʂ̓O���t�B�N�X�\�����s��
	StepCount += 1;
	if(StepCount >= 1000*1000*1000){
		StepCount = 0;
		StepCount_ += 1;
	}
	int temphogeshi =0;

}

void PHFemMeshThermo::CreateMatrix(){
}

void PHFemMeshThermo::InitTcAll(){
	for(unsigned i =0; i <vertices.size();i++){
		vertices[i].Tc = 0.0;
	}
}

void PHFemMeshThermo::InitCreateMatC(){
	/// MatC�ɂ���	//	�g�p����s��̏�����
	//dMatCAll�F�Ίp�s��̐����̓������s��̃T�C�Y���`:�z��Ƃ��ė��p	��:vertices.size(),����:1
	dMatCAll.resize(1,vertices.size()); //(h,w)
	dMatCAll.clear();								///	�l�̏�����
	//matc�̏������́Amatc�����֐��ł���Ă���̂ŁA�ȗ�
}

void PHFemMeshThermo::InitCreateMatk_(){
	///	MatK�ɂ���
	//matk�̏�����
	for(unsigned i =0; i < 4 ; i++){
		for(unsigned j =0; j < 4 ; j++){
			matk[i][j] = 0.0;
		}
	}
	dMatKAll.clear();		///	������
#ifdef UseMatAll
	//matKAll.resize(vertices.size(),vertices.size());	///	matKAll�̃T�C�Y�̑��
	matKAll.clear();									///	matKAll�̏�����
	//DSTR << "matKAll: " << matKAll <<std::endl;
#endif UseMatAll

#ifdef DumK
	//matKAll�̏�����
	matKAll.resize(vertices.size(),vertices.size());
	matkAll.clear();					///	�������A���̏������R�[�h�͕s�v
#endif


}
void PHFemMeshThermo::InitCreateMatk(){
	///	MatK�ɂ���
	//matk�̏�����
	for(unsigned i =0; i < 4 ; i++){
		for(unsigned j =0; j < 4 ; j++){
			matk[i][j] = 0.0;
		}
	}
	//for(unsigned i=0;i<3;i++){
	//tets.matk[i]
	//}
	//DMatAll�F�Ίp�s��̐����̓������s��̃T�C�Y���`:�z��Ƃ��ė��p	��:vertices.size(),����:1
	dMatKAll.resize(1,vertices.size()); //(h,w)
	dMatKAll.clear();		///	������
	////�l�̏�����
	//for(unsigned i=0;i<vertices.size();i++){
	//	dMatKAll[0][i] = 0.0;
	//}
#ifdef UseMatAll
	
	matKAll.resize(vertices.size(),vertices.size());	///	matKAll�̃T�C�Y�̑��
	matKAll.clear();									///	matKAll�̏�����
	//for(unsigned i=0;i<vertices.size();i++){
	//	for(unsigned j=0;j<vertices.size();j++){
	//		matKAll[i][j] = 0.0;
	//	}
	//}
#endif UseMatAll

#ifdef DumK
	//matKAll�̏�����
	matKAll.resize(vertices.size(),vertices.size());
	matkAll.clear();					///	�������A���̏������R�[�h�͕s�v
	//for(unsigned i=0;i<vertices.size();i++){
	//	for(unsigned j=0;j<vertices.size();j++){
	//		matKAll[i][j] = 0.0;
	//	}
	//}
#endif
}

void PHFemMeshThermo::InitCreateVecf_(){
	///	Vecf�ɂ���
	//Vecf�̏�����
	for(unsigned i =0; i < 4 ; i++){
			vecf[i] = 0.0;
	}
	vecFAll.clear();						///	������
}

void PHFemMeshThermo::InitCreateVecf(){
	///	Vecf�ɂ���
	//Vecf�̏�����
	for(unsigned i =0; i < 4 ; i++){
			vecf[i] = 0.0;
	}
	vecFAll.resize(vertices.size(),1);		///	�S�̍����x�N�g��F�̃T�C�Y���K��
	vecFAll.clear();						///	������
}

void PHFemMeshThermo::AfterSetDesc() {	
	////���ԍ��ݕ�	dt�̐ݒ�
	//PHFemMeshThermo::dt = 0.01;

	//�V�[������A�V�[���̎��ԃX�e�b�v���擾����ˈȉ��̃R�[�h�ł́AGetTimeStep���悤�Ƃ���ƁANULL���Ԃ��Ă���̂ŁAPHEngine�ōs��
	//DSTR << DCAST(PHSceneIf, GetScene()) << std::endl;
	//DSTR << DCAST(PHSceneIf, GetScene())->GetTimeStep() << std::endl;
	//int hogeshimitake =0;

	

	//%%%	��������		%%%//

	//�e�탁���o�ϐ��̏������˃R���X�g���N�^�łł����ق���������������Ȃ��B
	///	Edges
	for(unsigned i =0; i < edges.size();i++){
		edges[i].c = 0.0;
		edges[i].k = 0.0;
	}

	///	faces
	for(unsigned i=0;i<faces.size();i++){
		faces[i].alphaUpdated = true;
		faces[i].area = 0.0;
		faces[i].heatTransRatio = 0.0;
		faces[i].deformed = true;				//������Ԃ́A�ό`��Ƃ���
		faces[i].fluxarea =0.0;
		faces[i].heatflux =0.0;
	}

	///	vertex

	///	tets

	//�s��̐������Ȃǂ�������
	bVecAll.resize(vertices.size(),1);

	//�s������
		//�s�����邽�߂ɕK�v�Ȑߓ_��l�ʑ̂̏��́APHFemMeshThermo�̍\���̂ɓ����Ă���B
			//PHFemMeshThermo�̃I�u�W�F�N�g�����ۂɁA�f�B�X�N���v�^�ɒl��ݒ肵�č��
		
	//�ߓ_���x�̏����ݒ�(�s������O�ɍs��)
	SetVerticesTempAll(0.0);			///	�������x�̐ݒ�

	//���͗��̉��x�̏�����(0.0�x�ɂ���)
	InitTcAll();

	//�ߓ_�̏������x��ݒ肷���{T}�c�x�N�g���ɑ��
		//{T}�c�x�N�g�����쐬����B�ȍ~��K,C,F�s��E�x�N�g���̐ߓ_�ԍ��́A���̏c�x�N�g���̐ߓ_�̕��я��ɍ��킹��?
		
	//dmnN �����̉��x�̏c�i��j�x�N�g��
	CreateTempMatrix();

	//�M�`�����A���x�A��M�A�M�`�B���@�̃p�����[�^�[��ݒ�E���
		//PHFemMEshThermo�̃����o�ϐ��̒l���� CADThermo���A0.574;//�ʂ˂��̒l//�M�`����[W/(���EK)]�@Cp = 1.96 * (Ndt);//�ʂ˂��̔�M[kJ/(kg�EK) 1.96kJ/(kg K),�i�ʂ˂��̖��x�j�H�i���M�̉Ȋwp64���970kg/m^3
		//�M�`�B���̒P�ʌn�@W/(m^2 K)�˂����SI�P�ʌn�Ȃ̂��H�@25�͘_��(MEAT COOKING SIMULATION BY FINITE ELEMENTS)�̃I�[�u�����M���̎����l
		//SetInitThermoConductionParam(0.574,970,1.96,25);
	SetInitThermoConductionParam(0.574,970,0.1960,25 * 0.01);		//> thConduct:�M�`���� ,roh:���x,	specificHeat:��M J/ (K�Ekg):1960 ,�@heatTrans:�M�`�B�� W/(m^2�EK)
		//�����A�ϐ��l�͌ォ��v�Z�̓r���ŕύX�ł���悤��SetParam()�֐�������Ă������ق����������ȁH

	//> �M�����̏�����
	SetVtxHeatFluxAll(0.0);

	//>	�M���˗��̐ݒ�
	SetThermalEmissivityToVerticesAll(0.0);				///	�b��l0.0�ŏ�����	�F�M���˂͂��Ȃ���

	//>	�s��̍쐬�@�s��̍쐬�ɕK�v�ȕϐ��͂��̍s�ȑO�ɐݒ肪�K�v
		//�v�Z�ɗp����}�g���N�X�A�x�N�g�����쐬�i���b�V�����Ƃ̗v�f�����s��/�x�N�g���ˑS�̍����s��/�x�N�g���j
		//{T}�c�x�N�g���̐ߓ_�̕��я��ɕ��Ԃ悤�ɁA�W���s������Z����B�W���s��ɂ́A�ʐς�̐ρA�M�`�B���Ȃǂ̃p�����[�^�̐ς����Ă��܂������̂�����B


	//> IH���M����face��������x(�\��face && �����)�i��A�֌W��������face�ߓ_�̌��_����̋������v�Z���Aface[].mayIHheated�𔻒�
	CalcVtxDisFromOrigin();
	//>	IH����̒P�ʎ��ԓ�����̉��M�M��
	//�P�ʎ��ԓ�����̑����M�M��	231.9; //>	J/sec
	
	//�~���M�FIH
	//CalcIHdqdt(0.04,0.095,231.9 * 0.005 * 1e6);		/// �P�� m,m,J/sec		//> 0.002:dt�̕�;Step�ŗp����dt�{��������...	// 0.05,0.11�͓K���l
	//CalcIHdqdt_atleast(0.06,0.095,231.9 * 0.005 * 1e5);		///	�����ł��~�̈�ɂ������Ă�����A����face�̖ʐϑS����IH���M��������
	CalcIHdqdt_atleast(0.11,0.14,231.9 * 0.005 * 1e5);
	//..debug 

	//�o���h����M
//	CalcIHdqdtband_(-0.02,0.20,231.9 * 0.005 * 1e6);

	
	//%%	IH���M�̃��[�h�ؑ�
	//	���C����ɉ��M
	//	CalcIHdqdtband_(0.09,0.10,231.9 * 5e3);		//*0.5*1e4	�l��ς��Ď���	//*1e3�@//*1e4 //5e3
	//	�~��ɉ��M
	
	//CalcIHarea(0.04,0.095,231.9 * 0.005 * 1e6);


	//	���̌�ŁA�M�����x�N�g�����v�Z����֐����Ăяo��

	InitCreateMatC();					///	CreateMatC�̏�����
	InitCreateVecf();					///	CreateVecf�̏�����
	InitCreateMatk();					///	CreateMatK�̏�����
	//..	CreateLocalMatrixAndSet();			//> �ȏ�̏������A���̊֐��ɏW��

	///	�M�`�B�����e�ߓ_�Ɋi�[
	SetHeatTransRatioToAllVertex();
	for(unsigned i=0; i < this->tets.size(); i++){
		//�e�s�������āA�K�E�X�U�C�f���Ōv�Z���邽�߂̌W���̊�{�����BTimestep�̓����Ă��鍀�́A���̃\�[�X(SetDesc())�ł́A�����ł��Ȃ����Ƃ���������(NULL���Ԃ��Ă���)
		CreateMatkLocal(i);				///	Matk1 Matk2(�X�V���K�v�ȏꍇ������)�����
		//CreateMatKall();		//CreateMatkLocal();�Ɏ��������̂ŁA���������B
		CreateMatcLocal(i);
		CreateVecfLocal(i);
	}
	int hogeshidebug =0;
	//	�ߓ_���x���ڂ̏����o��
	templog.open("templog.csv");

	//	CPS�̌o���ω��������o��
	cpslog.open("cpslog.csv");



	// �J�E���g�̏�����
	Ndt =0;

	
}

//void PHFemMeshThermo::CreateLocalMatrixAndSet(){
//	//K,C,F�̍s������֐����Ăяo���āA��点��
//	for(unsigned i = 0; i< tets.size() ; i++){
//		//tets�������ɂ����ƁA���̍s��E�x�N�g��������Ă���āA�ł����s��A�x�N�g������ɌW����ݒ肵�����
//		//��������΁A�e�v�f�����s���for�����񂳂Ȃ��Ă��悭�Ȃ�
//		//CreateMatkLocal(tets);
//		//CreateMatcLocal(tets);
//		//CreateVecfLocal(tets);
//
//		//tets�����č�点��
////		SetkcfParam(tets);
//
//	}
//
//}
void PHFemMeshThermo::SetkcfParam(Tet tets){
}

void PHFemMeshThermo::CreateMatc(unsigned id){
	//�Ō�ɓ����s���������
	for(unsigned i =0; i < 4 ;i++){
		for(unsigned j =0; j < 4 ;j++){
			matc[i][j] = 0.0;
		}
	}
	//matc ��21�łł����s�������
	matc = Create44Mat21();
	//	for debug
		//DSTR << "matc " << matc << " �� ";
	matc = rho * specificHeat * CalcTetrahedraVolume(tets[id]) / 20.0 * matc;
	//	debug	//�W���̐ς��Ƃ�
		//DSTR << matc << std::endl;
		//int hogemat =0 ;
}

void PHFemMeshThermo::CreateMatcLocal(unsigned id){
	//���ׂĂ̗v�f�ɂ��ČW���s������
		//c
	CreateMatc(id);
	int mathoge=0;
	//	(�K�E�X�U�C�f�����g�����v�Z��)�v�f���ɍ�����s��̐������A�G�b�W�ɌW�����i�[����
	//	or	(�K�E�X�U�C�f�����g��Ȃ��v�Z��)�v�f���Ƃ̌v�Z���I��邽�тɁA�v�f�����s��̐����������G�b�W��_�ɍ��ϐ��Ɋi�[���Ă���	#ifedef�Ń��[�h����āA�ǂ�����ł���悤�ɂ��Ă����Ă��ǂ�����w
	for(unsigned j=1; j < 4; j++){
		int vtxid0 = tets[id].vertices[j];
		//	���O�p�s�񕔕��ɂ��Ă̂ݎ��s
		//	j==1:k=0, j==2:k=0,1, j==3:k=0,1,2
		for(unsigned k = 0; k < j; k++){
			int vtxid1 = tets[id].vertices[k];
				for(unsigned l =0; l < vertices[vtxid0].edges.size(); l++){
					for(unsigned m =0; m < vertices[vtxid1].edges.size(); m++){
						if(vertices[vtxid0].edges[l] == vertices[vtxid1].edges[m]){
							edges[vertices[vtxid0].edges[l]].c += matc[j][k];		//�������̂������͂������甼���ɂ���B��O�p�����O�p������������ɂ́A�ǂ�����for�����ɂ���Ηǂ��̂��H
							//DSTR << edges[vertices[vtxid0].edges[l]].k << std::endl;
						}
					}
				}
		}
	}
	//�Ίp������Ίp�����̑S�̍����s�񂩂甲���o����1�~n�̍s��ɑ������
	//j=0~4�܂ő��(��̃��[�v�ł́Aj�͑Ίp�����͈̔͂����Ȃ��̂ŁA�l������Ȃ��������o�Ă��܂�)
	for(unsigned j =0;j<4;j++){
		dMatCAll[0][tets[id].vertices[j]] += matc[j][j];
	}

	////	for debug
	//DSTR << "dMatCAll : " << std::endl;
	//for(unsigned j =0;j < vertices.size();j++){
	//	DSTR << j << "th : " << dMatCAll[0][j] << std::endl;
	//}
	// �l�M�ɂ��Ĕ�0�����ɂȂ����B

	//	���ׂ�
	//dMatKAll�̐����̂����A0�ƂȂ�v�f����������A�G���[�\��������R�[�h������
	// try catch���ɂ���
	//for(unsigned j = 0; j < vertices.size() ; j++){
	//	if(dMatCAll[0][j] ==0.0){
	//		DSTR << "dMatCAll[0][" << j << "] element is blank" << std::endl;
	//	}
	//}
	int piyodebug =0;
}

void PHFemMeshThermo::CreateVecfLocal(unsigned id){
	
	//���ׂĂ̗v�f�ɂ��ČW���s������
	//f1�����
	//>	�M�������E����	vecf2�����			
	//CreateVecf2(id);				//>	tets[id].vecf[1] ��������,���		�M�����͑������ςŋ��߂�
	CreateVecf2surface(id);			//
	//>	�M�`�B���E����	f3�����
	CreateVecf3(id);			//>	tets[id].vecf[2] ��������,���		�M�`�B���͑������ρA���͗��̉��x�͐ߓ_�̌`��֐��H���Ƃɋ��߂�
	//CreateVecf3_(id);			//>	tets[id].vecf[2] ��������,���		�M�`�B���A���͗��̉��x�𑊉����ςŋ��߂�
	//f4�����
	//f1:vecf[0],f2:vecf[1],f3:vecf[2],f4:vecf[3]�����Z����
	vecf = tets[id].vecf[1] + tets[id].vecf[2];		//>	+ tets[id].vecf[0] +  tets[id].vecf[3] �̗\��
	//	for debug
	//DSTR << "vecf : " << std::endl;
	//DSTR << vecf << std::endl;
	//DSTR << "vecf3 : " << std::endl;
	//DSTR << vecf3 << std::endl;
	//	(�K�E�X�U�C�f�����g�����v�Z��)�v�f���ɍ�����s��̐������A�G�b�W�ɌW�����i�[����
	//	or	(�K�E�X�U�C�f�����g��Ȃ��v�Z��)�v�f���Ƃ̌v�Z���I��邽�тɁA�v�f�����s��̐����������G�b�W��_�ɍ��ϐ��Ɋi�[���Ă���	#ifedef�Ń��[�h����āA�ǂ�����ł���悤�ɂ��Ă����Ă��ǂ�����w

	//�v�f�̐ߓ_�ԍ��̏ꏊ�ɁA���̐ߓ_��f�̒l������
	//j:�v�f�̒��̉��Ԗڂ�
	for(unsigned j =0;j < 4; j++){
		int vtxid0 = tets[id].vertices[j];
		vecFAll[vtxid0][0] = vecf[j];
	}
	//	for debug
	//vecFAll�ɒl���������̂��ǂ����𒲂ׂ� 2011.09.21�S���ɒl�������Ă��邱�Ƃ��m�F����
	//DSTR << "vecFAll : " << std::endl;
	//for(unsigned j =0; j < vertices.size() ; j++){
	//	DSTR << j << " ele is :  " << vecFAll[j][0] << std::endl;
	//}
	int hogeshi =0;

	////	���ׂ�
	////vecFAll�̐����̂����A0�ƂȂ�v�f����������A�G���[�\��������R�[�h������
	//// try catch���ɂ���
	//for(unsigned j = 0; j < vertices.size() ; j++){
	//	if(vecFAll[j][0] ==0.0){
	//		DSTR << "vecFAll[" << j << "][0] element is blank" << std::endl;
	//	}
	//}


	//	����
	//	f3���g�p����ꍇ:���͗��̉��xTc��0�̐ߓ_�̗v�f��0�ɂȂ邽�߁A���x�̐ݒ肪�K�v
	//int hogef =0;
}

void PHFemMeshThermo::CreateMatkLocal(unsigned id){
	//���ׂĂ̎l�ʑ̗v�f�ɂ��ČW���s������

	//	k1�����	k1k�ł��Ak1b�ł��ǂ���ł��\��Ȃ�	�ǂ�炪���������ׂ�
	///	�ό`������������������
	if(deformed){	CreateMatk1k(id);}			//	k���_�������ɁA���M���āA�`��֐��𓱏o	
//	if(deformed){	CreateMatk1b(id);}			//	���Ђ̗��_�������ɁA������p���Č`��֐��𓱏o
	//DSTR << "tets[id].matk1: " << tets[id].matk1 << std::endl;

	//�M�`�B���E�����ɕK�v�ȁAk2����邩�ۂ�
	CreateMatk2t(id);					///	�M�`�B���E����
	//CreateMatk2(id,tets[id]);			///	���̊֐��͎g�p���Ȃ�
	//DSTR << "tets[id].matk2: " << tets[id].matk2 << std::endl;
	int hogehogehoge=0;

	//k1,k2,k3�����Z����(�g���Ă��鐔�l����)
	matk = tets[id].matk[0] + tets[id].matk[1];	
	//DSTR << "matk: " << matk << std::endl;

	//	(�K�E�X�U�C�f�����g�����v�Z��)�v�f���ɍ�����s��̐������A�G�b�W�ɌW�����i�[����
	//	or	(�K�E�X�U�C�f�����g��Ȃ��v�Z��)�v�f���Ƃ̌v�Z���I��邽�тɁA�v�f�����s��̐����������G�b�W��_�ɍ��ϐ��Ɋi�[���Ă���	#ifedef�Ń��[�h����āA�ǂ�����ł���悤�ɂ��Ă����Ă��ǂ�����w
	//	Edges �̂��̗v�f�Ōv�Z����K�s��̐�����k�ɌW���Ƃ��Ċi�[����
		
	//matk�̑Ίp�����ȊO�ŁA���O�p�̕����̒l���Aedge��k�ɑ������
	//
	//DSTR << i <<"th tetrahedra element'edges[vertices[vtxid0].edges[l]].k (All edges.k' value): " << std::endl;
	for(unsigned j=1; j < 4; j++){
		int vtxid0 = tets[id].vertices[j];
		//	���O�p�s�񕔕��ɂ��Ă̂ݎ��s
		//	j==1:k=0, j==2:k=0,1, j==3:k=0,1,2
		for(unsigned k = 0; k < j; k++){
			int vtxid1 = tets[id].vertices[k];
			for(unsigned l =0; l < vertices[vtxid0].edges.size(); l++){
				for(unsigned m =0; m < vertices[vtxid1].edges.size(); m++){
					if(vertices[vtxid0].edges[l] == vertices[vtxid1].edges[m]){
						edges[vertices[vtxid0].edges[l]].k += matk[j][k];		//�������̂������͂������甼���ɂ���B��O�p�����O�p������������ɂ́A�ǂ�����for�����ɂ���Ηǂ��̂��H
						//DSTR << edges[vertices[vtxid0].edges[l]].k << std::endl;
#ifdef DumK
						edges[vertices[vtxid0].edges[l]].k = 0.0;
#endif DumK
					}
				}
			}
		}
	}

#ifdef UseMatAll
	//SciLab�Ŏg�����߂ɁA�S�̍����s������
	//matk������
	for(unsigned j=0; j<4 ; j++){
		for(unsigned k=0; k<4 ;k++){
			matKAll[tets[id].vertices[j]][tets[id].vertices[k]] += matk[j][k];
		}
	}

	////edges�ɓ������W��������
	//for(unsigned j=1; j < 4; j++){
	//	int vtxid0 = tets[i].vertices[j];
	//	//	���O�p�s�񕔕��ɂ��Ă̂ݎ��s
	//	//	j==1:k=0, j==2:k=0,1, j==3:k=0,1,2
	//	for(unsigned k = 0; k < j; k++){
	//		int vtxid1 = tets[i].vertices[k];
	//			for(unsigned l =0; l < vertices[vtxid0].edges.size(); l++){
	//				for(unsigned m =0; m < vertices[vtxid1].edges.size(); m++){
	//					if(vertices[vtxid0].edges[l] == vertices[vtxid1].edges[m]){
	//						edges[vertices[vtxid0].edges[l]].k += matk[j][k];		//�������̂������͂������甼���ɂ���B��O�p�����O�p������������ɂ́A�ǂ�����for�����ɂ���Ηǂ��̂��H
	//						//DSTR << edges[vertices[vtxid0].edges[l]].k << std::endl;
	//					}
	//				}
	//			}
	//	}
	//}

#endif UseMatAll

#ifdef DumK
	//SciLab�Ŏg�����߂ɁA�S�̍����s������
	//matk������
	for(unsigned j=0; j<4 ; j++){
		for(unsigned k=0; k<4 ;k++){
			matKAll[tets.vertices[j]][tets.vertices[k]] = 0.0;;
		}
	}
#endif

	//�Ίp������Ίp�����̑S�̍����s�񂩂甲���o����1�~n�̍s��ɑ������
	//j=0~4�܂ő��(��̃��[�v�ł́Aj�͑Ίp�����͈̔͂����Ȃ��̂ŁA�l������Ȃ��������o�Ă��܂�)
	for(unsigned j =0;j<4;j++){
		dMatKAll[0][tets[id].vertices[j]] += matk[j][j];
		//DSTR << "matk[" << j << "][" << j << "] : " << matk[j][j] << std::endl;
		//DSTR << "dMatKAll[0][" << tets[i].vertices[j] << "] : " << dMatKAll[0][tets[i].vertices[j]] << std::endl;
		int hoge4 =0;
	}
	//DSTR << std::endl;	//���s

	//std::ofstream matKAll("matKAll.csv");
	//for(){
	//	matKAll
	//	}


#ifdef DumK
	for(unsigned j=0;j<4;j++){
		dMatKAll[0][tets.vertices[j]] = 0.0;
		int hogeshi =0;
	} 
#endif DumK

	
	//	for debug
	//�v�f25��0~3�Ԗڂ̐ߓ_�������\������
	//if(i == 25){
	//		for(unsigned n=0;n < 4;n++){
	//			DSTR << n << " : " << tets[25].vertices[n] << std::endl;	//�v�f25��0�Ԗڂ̐ߓ_��63�ł���B
	//		}
	//}
	//�ߓ_�ԍ���63�̓_���ǂ̗v�f�ɓ����Ă���̂��𒲂ׂ��25,57������
	//for(unsigned j=0;j < vertices[63].tets.size();j++){
	//	DSTR << vertices[63].tets[j] <<std::endl;
	//}
		
	//	���ׂ�
	//dMatKAll�̐����̂����A0�ƂȂ�v�f����������A�G���[���H�@�����Ă��Ȃ������������Ă��A���Ȃ��C������
	// try catch���ɂ���
//	for(unsigned j = 0; j < vertices.size() ; j++){
//		if(dMatKAll[0][j] ==0.0){
//			DSTR << "Creating dMatKAll error!? : dMatKAll[0][" << j << "] == 0.0 " << std::endl;
////			DSTR << "If " << j <<" 's blank eroors didn't banished until display simulation scene, I recommened Source Code Check!" <<std::endl;  
//		}
//	}

	//DSTR << "matKAll : " << matKAll <<std::endl;
	//for(unsigned i=0;i<vertices.size();i++){
	//	for(unsigned j=0;j <  vertices.size();j++){
	//		if(matKAll[i][j] !=0){
	//			DSTR << "matKAll[" << i <<"][" << j << "]: " << matKAll[i][j] <<std::endl;
	//		}
	//	}
	//}
	//DSTR << "dMatKAll : " <<dMatKAll << std::endl;
#ifdef UseMatAll
	for(unsigned j =0;j<vertices.size();j++){
		if(matKAll[j][j] != dMatKAll[0][j]){
			DSTR << j <<" �����̗v�f�͂��������I�������K�v�ł���B " <<std::endl;
		}
	}
#endif UseMatAll
	int hoge5 =0;

}

void PHFemMeshThermo::CreateMatk1b(unsigned id){
	//yagawa1983����Ƀm�[�g�Ɏ��W�J�����v�Z��
	unsigned i=0;
	unsigned j=0;
	unsigned k=0;
	unsigned l=0;

	//a_1~a_4, ... , c_4	���쐬
	//�W��(����)�~�s�񎮂̓��ꕨ
	double a[4];
	double b[4];
	double c[4];

	//�v�f���\������ߓ_�̍��W�̓��ꕨ
	double x[4];
	double y[4];
	double z[4];
	//�v�f����4�ߓ_��x,y,z���W���i�[
	for(unsigned m=0; m < 4;m++){
		x[m] = vertices[tets[id].vertices[m]].pos.x;
		y[m] = vertices[tets[id].vertices[m]].pos.y;
		z[m] = vertices[tets[id].vertices[m]].pos.z;
	}

	//mata~matc
	//a[i]
	for(unsigned i =0;i<4;i++){
		double fugou =0.0;				// (-1)^i �̕����̒�`

		//	fugou �̕�������
		if(i == 0 || i == 2){		//0,2�̎��A(-1)^1,3 = -1
			fugou = -1.0;
		}
		else{					//1,3�̎��A(-1)^0,2 = 1
			fugou = 1.0;
		}
		
		//i,j,k,l�̊֌W�Z�b�g�˔z��̗v�f�ɂ��Ă����������Bi[4],if(i[0]=0){i[1](=j)=1, i[2](=k)=2, i[3](=l)=3}	if(i[0]=1){i[1](=j)=2, i[2](=k)=3, i[3](=l)=0}
		if(i==0){		j=1;	k=2;	l=3;	}
		else if(i==1){	j=2;	k=3;	l=0;	}
		else if(i==2){	j=3;	k=0;	l=1;	}
		else if(i==3){	j=0;	k=1;	l=2;	}

		// a_i�̍쐬
		for(unsigned m =0;m<3;m++){						//	1�̐����ւ̑���͂���
			mata[m][0] = 1.0;
		}
		mata[0][1] = y[j];
		mata[1][1] = y[k];
		mata[2][1] = y[l];

		mata[0][2] = z[j];
		mata[1][2] = z[k];
		mata[2][2] = z[l];

		a[i] = fugou * mata.det();

		//DSTR << "mata : " <<std::endl;
		//DSTR << mata << std::endl;

		//DSTR << "mata.det() : " <<std::endl;
		//DSTR << mata.det() << std::endl;

		//DSTR << "a[" << i << "] : " <<std::endl;
		//DSTR << a[i] << std::endl;


		// b_i�̍쐬
		matb[0][0]=x[j];
		matb[1][0]=x[k];
		matb[2][0]=x[l];

		for(unsigned m =0;m<3;m++){						//	1�̐����ւ̑���͂���
			matb[m][1] = 1.0;
		}

		matb[0][2]=z[j];
		matb[1][2]=z[k];
		matb[2][2]=z[l];

		b[i] = fugou * matb.det();

		//DSTR << "matb : " <<std::endl;
		//DSTR << matb << std::endl;
		//DSTR << "matb.det() : " <<std::endl;
		//DSTR << matb.det() << std::endl;

		//DSTR << "b[" << i << "] : " <<std::endl;
		//DSTR << b[i] << std::endl;

		// c_i�̍쐬
		matcc[0][0]=x[j];
		matcc[1][0]=x[k];
		matcc[2][0]=x[l];

		matcc[0][1]=y[j];
		matcc[1][1]=y[k];
		matcc[2][1]=y[l];

		for(unsigned m =0;m<3;m++){						//	1�̐����ւ̑���͂���
			matcc[m][2] = 1.0;
		}

		//DSTR << "matcc : " <<std::endl;
		//DSTR << matcc << std::endl;
		//DSTR << "matcc.det() : " <<std::endl;
		//DSTR << matcc.det() << std::endl;

		c[i] = fugou * matcc.det();
		
		//	for debug�@�v�f���Ƃ�a_i~c_i�̎Z�o
		//DSTR << "a["<< i << "] : " << a[i] << std::endl;
		//DSTR << "b["<< i << "] : " << b[i] << std::endl;
		//DSTR << "c["<< i << "] : " << c[i] << std::endl;
		//DSTR << std::endl;
		int debughogeshi =0;
	}
	
	//	matk1�̐�����a_i ~ c_i�̑���������	���ꂢ�����邪�A���̃R�[�h�ŗǂ��I	�Ίp��������Ίp�������A�S���A���̃R�[�h
	//	���P��		���O�p�ƑΊp���������A�v�Z���A��O�p�͉��O�p�����ł��悢�B
	for(unsigned i =0;i<4;i++){
		for(unsigned j =0;j<4;j++){
			tets[id].matk[0][i][j] = a[i] * a[j] +b[i] * b[j] + c[i] * c[j];
		}
	}

	////	��L���R�X�g�̏��Ȃ��R�[�h?
	//matk[0][0] = a[0] * a[0] +b[0] * b[0] + c[0] * c[0];
	//matk[1][1] = a[1] * a[1] +b[1] * b[1] + c[1] * c[1];
	//matk[2][2] = a[2] * a[2] +b[2] * b[2] + c[2] * c[2];
	//matk[3][3] = a[3] * a[3] +b[3] * b[3] + c[3] * c[3];

	//matk[0][1] = a[0] * a[1] + b[0] * b[1] + c[0] * c[1];
	//matk[1][0] = matk[0][1];

	//matk[0][2] = a[0] * a[2] + b[0] * b[2] + c[0] * c[2];
	//matk[2][0] = matk[0][2];

	//matk[0][3] = a[0] * a[3] + b[0] * b[3] + c[0] * c[3];
	//matk[3][0] = matk[0][3];

	//	for DEBUG
	//DSTR << "matk1 : " << std::endl;
	//DSTR << matk1 << std::endl;
	//int debughogeshi2 =0;
	
	//�W���̐�
	tets[id].matk[0]= thConduct / (36 *  CalcTetrahedraVolume(tets[id])) * tets[id].matk[0];		//���_���Ԉ���Ă����̂ŁA�C��

	//	for DEBUG
	//DSTR << "�W���ό�� matk1 : " << std::endl;
	//DSTR << matk1 << std::endl;
	int debughogeshi3 =0;

	DSTR << "Inner Function matk1b _ tets["<< id << "].matk[0] "<< tets[id].matk[0] << std::endl;  
	//a~c�̑�������K1�ɑ��
	//matk1(4x4)�ɑ��

}

void PHFemMeshThermo::CreateMatk1k(unsigned id){
	//���̌v�Z���Ăяo���Ƃ��ɁA�e�l�ʑ̂��ƂɌv�Z���邽�߁A�l�ʑ̂�0�Ԃ��珇�ɂ��̌v�Z���s��
	//�l�ʑ̂��\������4�ߓ_��ߓ_�̔z��(Tets�ɂ́A�ߓ_�̔z�񂪍���Ă���)�ɓ����Ă��鏇�Ԃ��g���āA�ʂ̌v�Z���s������A�s��̌v�Z���s�����肷��B
	//���̂��߁A���̊֐��̈����ɁA�l�ʑ̗v�f�̔ԍ������

	//�Ō�ɓ����s���������
	tets[id].matk[0].clear();
	//for(unsigned i =0; i < 4 ;i++){
	//	for(unsigned j =0; j < 4 ;j++){
	//		//tets[id].matk1[i][j] = 0.0;
	//		tets[id].matk[0][i][j] = 0.0;
	//	}
	//}

	//	A�s��@=	a11 a12 a13
	//				a21 a22 a23
	//				a31 a32 a33
	//�𐶐�
	PTM::TMatrixRow<4,4,double> matk1A;
	FemVertex p[4];
	for(unsigned i = 0; i< 4 ; i++){
		p[i]= vertices[tets[id].vertices[i]];
	}
	
	matk1A[0][0] = (p[2].pos.y - p[0].pos.y) * (p[3].pos.z - p[0].pos.z) - (p[2].pos.z - p[0].pos.z) * (p[3].pos.y - p[0].pos.y);
	matk1A[0][1] = (p[1].pos.z - p[0].pos.z) * (p[3].pos.y - p[0].pos.y) - (p[1].pos.y - p[0].pos.y) * (p[3].pos.z - p[0].pos.z);
	matk1A[0][2] = (p[1].pos.y - p[0].pos.y) * (p[2].pos.z - p[0].pos.z) - (p[1].pos.z - p[0].pos.z) * (p[2].pos.y - p[0].pos.y);

	matk1A[1][0] = (p[2].pos.z - p[0].pos.z) * (p[3].pos.x - p[0].pos.x) - (p[2].pos.x - p[0].pos.x) * (p[3].pos.z - p[0].pos.z);
	matk1A[1][1] = (p[1].pos.x - p[0].pos.x) * (p[3].pos.z - p[0].pos.z) - (p[1].pos.z - p[0].pos.z) * (p[3].pos.x - p[0].pos.x);
	matk1A[1][2] = (p[1].pos.z - p[0].pos.z) * (p[2].pos.x - p[0].pos.x) - (p[1].pos.x - p[0].pos.x) * (p[2].pos.z - p[0].pos.z);

	matk1A[2][0] = (p[2].pos.x - p[0].pos.x) * (p[3].pos.y - p[0].pos.y) - (p[2].pos.y - p[0].pos.y) * (p[3].pos.x - p[0].pos.x);
	matk1A[2][1] = (p[1].pos.y - p[0].pos.y) * (p[3].pos.x - p[0].pos.x) - (p[1].pos.x - p[0].pos.x) * (p[3].pos.y - p[0].pos.y);
	matk1A[2][2] = (p[1].pos.x - p[0].pos.x) * (p[2].pos.y - p[0].pos.y) - (p[1].pos.y - p[0].pos.y) * (p[2].pos.x - p[0].pos.x);

	//	for	debug
	//DSTR << matk1A[0][0] << std::endl;
	//DSTR << "matk1A : " << matk1A << std::endl; 
	//int hogeshi =0;

	//a11 ~ a33 ���s��ɓ���āA[N~T] [N] ���v�Z������
	
	PTM::TMatrixRow<1,4,double> Nx;
	PTM::TMatrixRow<1,4,double> Ny;
	PTM::TMatrixRow<1,4,double> Nz;

	Nx[0][0] = -matk1A[0][0] - matk1A[0][1] -matk1A[0][2];
	Nx[0][1] = matk1A[0][0];
	Nx[0][2] = matk1A[0][1];
	Nx[0][3] = matk1A[0][2];

	Ny[0][0] = -matk1A[1][0] - matk1A[1][1] -matk1A[1][2];
	Ny[0][1] = matk1A[1][0];
	Ny[0][2] = matk1A[1][1];
	Ny[0][3] = matk1A[1][2];

	Nz[0][0] = -matk1A[2][0] - matk1A[2][1] -matk1A[2][2];
	Nz[0][1] = matk1A[2][0];
	Nz[0][2] = matk1A[2][1];
	Nz[0][3] = matk1A[2][2];

	//	Km �̎Z�o
	//tets[id].matk1 = Nx.trans() * Nx + Ny.trans() * Ny + Nz.trans() * Nz;
	tets[id].matk[0] = Nx.trans() * Nx + Ny.trans() * Ny + Nz.trans() * Nz;

	////	for debug
	//DSTR << "Nx : " << Nx << std::endl;
	//DSTR << "Nx^T : " << Nx.trans() << std::endl;
	//DSTR << "Nx^T * Nx : " << Nx.trans() * Nx << std::endl;
	//DSTR << "Ny^T * Ny : " << Ny.trans() * Ny << std::endl;
	//DSTR << "Nz^T * Nz : " << Nz.trans() * Nz << std::endl;
	//DSTR << "matk1 : " << matk1 << std::endl;
	//int hogehoge =0;

	//	for	DEBUG
	//DSTR << "matk1 : " << matk1 << std::endl;

	//K1
//	matk1 = thConduct / (36 * CalcTetrahedraVolume(tets) ) * matk1;
	
	//tets[id].matk1 = thConduct / (36 * CalcTetrahedraVolume(tets[id]) ) * tets[id].matk1;
	tets[id].matk[0] = thConduct / (36 * CalcTetrahedraVolume(tets[id]) ) * tets[id].matk[0];
	//DSTR << "Inner Function _tets[id].matk1 : " << tets[id].matk1 << std::endl;

}

void PHFemMeshThermo::CreateVecf2surface(unsigned id){
	// ������
	tets[id].vecf[1].clear();
	//l=0�̎�f21,1�̎�:f22, 2�̎�:f23, 3�̎�:f24	�𐶐�
	///	..j�Ԗڂ̍s��̐�����0�̃x�N�g�����쐬
	for(unsigned l= 0 ; l < 4; l++){
		vecf2array[l] = Create41Vec1();
		vecf2array[l][l] = 0.0;			//	l�s��0��
	}
	for(unsigned l= 0 ; l < 4; l++){
		///	�l�ʑ̂̊e��(l = 0 �` 3) �ɂ��ă��b�V���\�ʂ��ǂ������`�F�b�N����B�\�ʂȂ�A�s��������vecf2array�ɓ����
		//faces[tets.faces[i]].sorted;		/// 1,24,58�݂����Ȑߓ_�ԍ��������Ă���
		///	..�s��^�̓��ꕨ��p��
		//faces[tets.faces[l]].vertices;
		if(tets[id].faces[l] < (int)nSurfaceFace && faces[tets[id].faces[l]].fluxarea > 0 ){			///	�O�k�̖� ���� �M�`�B�����X�V���ꂽ�� matk2���X�V����K�v������
			///	�l�ʑ̂̎O�p�`�̖ʐς��v�Z		///	���̊֐��̊O�Ŗʐϕ��̖ʐόv�Z����������B�ړ�����
			if(faces[tets[id].faces[l]].area ==0 || faces[tets[id].faces[l]].deformed ){		///	�ʐς��v�Z����Ă��Ȃ����i�͂��߁j or deformed(�ό`�������E�������)��true�̎�		///	�����̒ǉ�	�ʐς�0�� ||(OR) �����X�V���ꂽ��
				faces[tets[id].faces[l]].area = CalcTriangleArea(faces[tets[id].faces[l]].vertices[0], faces[tets[id].faces[l]].vertices[1], faces[tets[id].faces[l]].vertices[2]);
				faces[tets[id].faces[l]].deformed = false;
			}
			///	�v�Z���ʂ��s��ɑ��
			///	area�̌v�Z�Ɏg���Ă��Ȃ��_�������Ă���s���������x�N�g���̐ς��Ƃ�
			///	�ϕ��v�Z�����{����l����
			unsigned vtx = tets[id].vertices[0] + tets[id].vertices[1] + tets[id].vertices[2] + tets[id].vertices[3];			
			///	area�v�Z�Ɏg���Ă��Ȃ��ߓ_ID�FID
			unsigned ID = vtx -( faces[tets[id].faces[l]].vertices[0] + faces[tets[id].faces[l]].vertices[1] + faces[tets[id].faces[l]].vertices[2] );
			for(unsigned j=0;j<4;j++){
				if(tets[id].vertices[j] == ID){					///	�`��֐����P�A�i���Ȃ킿�j����face�ɑΖʂ��钸�_�@�ƈ�v������@���̎���face�Ŗʐϕ�����
					///	�O�k�ɂȂ����b�V���ʂ̖ʐς�0�ŏ��������Ă���
					///	�ȉ���[]�͏�܂ł�[l]�ƈقȂ�B
					///	ID�����Ԗڂ��ɂ���āA�`��֐��̌W�����قȂ�̂ŁA
					tets[id].vecf[1] += faces[tets[id].faces[l]].heatflux * (1.0/3.0) * faces[tets[id].faces[l]].area * vecf2array[j];
					//DSTR << "tets[id].matk2��faces[tets[id].faces[l]].heatTransRatio * (1.0/12.0) * faces[tets[id].faces[l]].area * matk2array[" << j << "]"<< "�����Z: " <<faces[tets[id].faces[l]].heatTransRatio * (1.0/12.0) * faces[tets[id].faces[l]].area * matk2array[j] << std::endl;
					//DSTR << "tets[id].matk2 +=  " << tets[id].matk2 << std::endl;
				}
				//else{
				//	///	ID�ƈ�v���Ȃ��ꍇ�ɂ́Amatk2array[j]�ɂ͑S����0������
				//	///	�Ƃ������Ƃ��낾���A
				//	//matk2array[j] =0.0 * matk2array[j];
				//	//DSTR << "matk2array[" << j << "]: " << matk2array[j] << std::endl;
				//}
			}
		}
		///	SurfaceFace����Ȃ�������Amatk2array�ɂ�0������
		//else{
		//	//matk2array[l];
		//}
	}


}

void PHFemMeshThermo::CreateVecf2(unsigned id){
	//	������
	for(unsigned i =0; i < 4 ;i++){
		//�Ō�ɓ����s���������
		tets[id].vecf[1][i] =0.0;				//>	f3 = vecf[1] 
	}	
	//l=0�̎�f31,1:f32, 2:f33, 3:f34	�𐶐�
	for(unsigned l= 0 ; l < 4; l++){
		//matk2array[l] = matk2temp;
		vecf2array[l] = Create41Vec1();
		//	l�s��0��
		vecf2array[l][l] = 0.0;

		//array[n][m][l]	= narray[n],m�sl��
		//	f_3	(vecf3array[0], vecf3array[1],..)
		// =	| 0 | + | 1 |+...
		//		| 1 |   | 0 |
		//		| 1 |   | 1 |
		//		| 1 |   | 1 |

		//	for debug
		//DSTR << "vecf3array[" << l << "] : " << std::endl;
		//DSTR << vecf3array[l] << std::endl;

		//�W���̐ς��Ƃ�
		//���̐ߓ_�ō\�������l�ʑ̖̂ʐς̐ς��Ƃ�
		//�l�ʑ̂̐ߓ_1,2,3(0�ȊO)�ō��O�p�`�̖ʐ�
		//l==0�Ԗڂ̎��A 123	��������
		//l==1			0 23
		//l==2			01 3
		//l==3			012
		//��CalcTriangleArea�ɓ���邱�Ƃ��ł���悤�ɃA���S���Y�����l����B

		//>	CreateMatk2t�̂悤�ȃA���S���Y���ɕύX�\��
		//k21
		if(l==0){
			//>	�O�p�`�ʂ��\������3���_�̔M�����̑�������
			double tempHF = (vertices[tets[id].vertices[1]].heatFluxValue + vertices[tets[id].vertices[2]].heatFluxValue + vertices[tets[id].vertices[3]].heatFluxValue ) / 3.0;		//HTR:HeatTransRatio
			vecf2array[l] = tempHF * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[1],tets[id].vertices[2],tets[id].vertices[3] ) * vecf2array[l];
			//DSTR << "vecf2array[" << l << "] : " << vecf2array[l] << std::endl;
			//Vec�̐ߓ_���ɂ��̐ߓ_�ł̎��͗��̉��xTc�Ƃ̐ς��s��
			
			////>	�s�v�H
			//for(unsigned m=0; m<4; m++){
			//	vecf2array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf2array[l][m];
			//}
		}
		//	k22
		else if(l==1){
			double tempHF = (vertices[tets[id].vertices[0]].heatFluxValue + vertices[tets[id].vertices[2]].heatFluxValue + vertices[tets[id].vertices[3]].heatFluxValue ) / 3.0;		//HTR:HeatTransRatio
			vecf2array[l] = tempHF * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[2],tets[id].vertices[3] ) * vecf2array[l];
			//vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[2],tets[id].vertices[3] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vec�̐ߓ_���ɂ��̐ߓ_�ł̎��͗��̉��xTc�Ƃ̐ς��s��
			//for(unsigned m=0; m<4; m++){
			//	vecf2array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf2array[l][m];
			//}
		}
		//	k23
		else if(l==2){
			double tempHF = (vertices[tets[id].vertices[0]].heatFluxValue + vertices[tets[id].vertices[1]].heatFluxValue + vertices[tets[id].vertices[3]].heatFluxValue ) / 3.0;		//HTR:HeatTransRatio
			vecf2array[l] = tempHF * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[3] ) * vecf2array[l];
			//vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[3] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vec�̐ߓ_���ɂ��̐ߓ_�ł̎��͗��̉��xTc�Ƃ̐ς��s��
			//for(unsigned m=0; m<4; m++){
			//	vecf2array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf2array[l][m];
			//}
		}
		//	k24
		else if(l==3){
			double tempHF = (vertices[tets[id].vertices[0]].heatFluxValue + vertices[tets[id].vertices[1]].heatFluxValue + vertices[tets[id].vertices[2]].heatFluxValue ) / 3.0;		//HTR:HeatTransRatio
			vecf2array[l] = tempHF * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[2] ) * vecf2array[l];
			//vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[2] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vec�̐ߓ_���ɂ��̐ߓ_�ł̎��͗��̉��xTc�Ƃ̐ς��s��
			//for(unsigned m=0; m<4; m++){
			//	vecf2array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf2array[l][m];
			//}
		}
		//for debug
		//DSTR << "vecf3array[" << l << "]�̊����ł́�" << std::endl;
		//DSTR << vecf3array[l] << std::endl;
		//if(dMatCAll == NULL){
		//	//DSTR <<"i : "<< i << ", l : " << l << std::endl;
		//	DSTR << "dMatCAll == NULL" <<std::endl;
		//	DSTR <<"l : " << l << std::endl;
		//}
	}

	//f3 = f31 + f32 + f33 + f34
	for(unsigned i=0; i < 4; i++){
		//vecf3 += vecf3array[i];
		tets[id].vecf[1] += vecf2array[i];
		//	for debug
		//DSTR << "vecf3 �� vecf3array = f3" << i+1 <<"�܂ŉ��Z�����s��" << std::endl;
		//DSTR << vecf3 << std::endl;
	}
	
	//	f1,f2,f3,f4	���v�Z����ۂɁA[0][0]��������[3][0]�����܂ł̔�0�����ɂ��āA���Tc�������Ă��܂�


	//for debug
	//DSTR << "�ߓ_�i";
	//for(unsigned i =0; i < 4; i++){
	//	DSTR << tets[id].vertices[i] << "," ;
	//}
	//DSTR << ")�ō\�������l�ʑ̂�" << std::endl;
	//DSTR << "vecf3 : " << std::endl;
	//DSTR << vecf3 << std::endl;
	//int hogeshishi =0;
}

void PHFemMeshThermo::CreateVecf3_(unsigned id){
	//	������
	for(unsigned i =0; i < 4 ;i++){
		//vecf3[i] = 0.0;		//�Ō�ɓ����s���������
		tets[id].vecf[2][i] =0.0;
	}	
	//l=0�̎�f31,1:f32, 2:f33, 3:f34	�𐶐�
	for(unsigned l= 0 ; l < 4; l++){
		//matk2array[l] = matk2temp;
		vecf3array[l] = Create41Vec1();
		//	l�s��0��
		//for(int i=0;i<4;i++){
		//	vecf3array[l][l][i] = 0.0;
		//}
		vecf3array[l][l] = 0.0;
		//array[n][m][l]	= narray[n],m�sl��
		//	f_3	(vecf3array[0], vecf3array[1],..)
		// =	| 0 | + | 1 |+...
		//		| 1 |   | 0 |
		//		| 1 |   | 1 |
		//		| 1 |   | 1 |
		//	for debug
		//DSTR << "vecf3array[" << l << "] : " << std::endl;
		//DSTR << vecf3array[l] << std::endl;

		//�W���̐ς��Ƃ�
		//���̐ߓ_�ō\�������l�ʑ̖̂ʐς̐ς��Ƃ�
		//�l�ʑ̂̐ߓ_1,2,3(0�ȊO)�ō��O�p�`�̖ʐ�
		//l==0�Ԗڂ̎��A 123	��������
		//l==1			0 23
		//l==2			01 3
		//l==3			012
		//��CalcTriangleArea�ɓ���邱�Ƃ��ł���悤�ɃA���S���Y�����l����B
		//k21
		if(l==0){
			//�O�p�`�ʂ��\������3���_�̔M�`�B���̑�������
			double tempHTR = (vertices[tets[id].vertices[1]].heatTransRatio + vertices[tets[id].vertices[2]].heatTransRatio + vertices[tets[id].vertices[3]].heatTransRatio ) / 3.0;		//HTR:HeatTransRatio
			double avgTc = (vertices[tets[id].vertices[1]].Tc + vertices[tets[id].vertices[2]].Tc + vertices[tets[id].vertices[3]].Tc ) / 3.0;
			vecf3array[l] = tempHTR * avgTc * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[1],tets[id].vertices[2],tets[id].vertices[3] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			
			//>	���͖{���H
			//Vec�̐ߓ_���ɂ��̐ߓ_�ł̎��͗��̉��xTc�Ƃ̐ς��s��
			//for(unsigned m=0; m<4; m++){
			//	vecf3array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf3array[l][m];
			//}
		}
		//	k22
		else if(l==1){
			double tempHTR = (vertices[tets[id].vertices[0]].heatTransRatio + vertices[tets[id].vertices[2]].heatTransRatio + vertices[tets[id].vertices[3]].heatTransRatio ) / 3.0;		//HTR:HeatTransRatio
			double avgTc = (vertices[tets[id].vertices[0]].Tc + vertices[tets[id].vertices[2]].Tc + vertices[tets[id].vertices[3]].Tc ) / 3.0;		//HTR:HeatTransRatio
			vecf3array[l] = tempHTR * avgTc * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[2],tets[id].vertices[3] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vec�̐ߓ_���ɂ��̐ߓ_�ł̎��͗��̉��xTc�Ƃ̐ς��s��
			//for(unsigned m=0; m<4; m++){
			//	vecf3array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf3array[l][m];
			//}
		}
		//	k23
		else if(l==2){
			double tempHTR = (vertices[tets[id].vertices[0]].heatTransRatio + vertices[tets[id].vertices[1]].heatTransRatio + vertices[tets[id].vertices[3]].heatTransRatio ) / 3.0;		//HTR:HeatTransRatio
			double avgTc = (vertices[tets[id].vertices[0]].Tc + vertices[tets[id].vertices[1]].Tc + vertices[tets[id].vertices[3]].Tc ) / 3.0;		//HTR:HeatTransRatio
			vecf3array[l] = tempHTR * avgTc * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[3] ) * vecf3array[l];
			//vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[3] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vec�̐ߓ_���ɂ��̐ߓ_�ł̎��͗��̉��xTc�Ƃ̐ς��s��
			//for(unsigned m=0; m<4; m++){
			//	vecf3array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf3array[l][m];
			//}
		}
		//	k24
		else if(l==3){
			double tempHTR = (vertices[tets[id].vertices[0]].heatTransRatio + vertices[tets[id].vertices[1]].heatTransRatio + vertices[tets[id].vertices[2]].heatTransRatio ) / 3.0;		//HTR:HeatTransRatio
			double avgTc = (vertices[tets[id].vertices[0]].Tc + vertices[tets[id].vertices[1]].Tc + vertices[tets[id].vertices[2]].Tc ) / 3.0;		//HTR:HeatTransRatio
			vecf3array[l] = tempHTR * avgTc * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[2] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vec�̐ߓ_���ɂ��̐ߓ_�ł̎��͗��̉��xTc�Ƃ̐ς��s��
			//for(unsigned m=0; m<4; m++){
			//	vecf3array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf3array[l][m];
			//}
		}
		//for debug
		//DSTR << "vecf3array[" << l << "]�̊����ł́�" << std::endl;
		//DSTR << vecf3array[l] << std::endl;
		//if(dMatCAll == NULL){
		//	//DSTR <<"i : "<< i << ", l : " << l << std::endl;
		//	DSTR << "dMatCAll == NULL" <<std::endl;
		//	DSTR <<"l : " << l << std::endl;
		//}
	}

	//f3 = f31 + f32 + f33 + f34
	for(unsigned i=0; i < 4; i++){
		//vecf3 += vecf3array[i];
		tets[id].vecf[2] += vecf3array[i];
		//	for debug
		//DSTR << "vecf3 �� vecf3array = f3" << i+1 <<"�܂ŉ��Z�����s��" << std::endl;
		//DSTR << vecf3 << std::endl;
	}
	
	//	f1,f2,f3,f4	���v�Z����ۂɁA[0][0]��������[3][0]�����܂ł̔�0�����ɂ��āA���Tc�������Ă��܂�


	//for debug
	//DSTR << "�ߓ_�i";
	//for(unsigned i =0; i < 4; i++){
	//	DSTR << tets[id].vertices[i] << "," ;
	//}
	//DSTR << ")�ō\�������l�ʑ̂�" << std::endl;
	//DSTR << "vecf3 : " << std::endl;
	//DSTR << vecf3 << std::endl;
	//int hogeshishi =0;
}

void PHFemMeshThermo::CreateVecf3(unsigned id){
	//	������
	for(unsigned i =0; i < 4 ;i++){
		//vecf3[i] = 0.0;		//�Ō�ɓ����s���������
		tets[id].vecf[2][i] =0.0;
	}	
	//l=0�̎�f31,1:f32, 2:f33, 3:f34	�𐶐�
	for(unsigned l= 0 ; l < 4; l++){
		//matk2array[l] = matk2temp;
		vecf3array[l] = Create41Vec1();
		//	l�s��0��
		//for(int i=0;i<4;i++){
		//	vecf3array[l][l][i] = 0.0;
		//}
		vecf3array[l][l] = 0.0;
		//array[n][m][l]	= narray[n],m�sl��
		//	f_3	(vecf3array[0], vecf3array[1],..)
		// =	| 0 | + | 1 |+...
		//		| 1 |   | 0 |
		//		| 1 |   | 1 |
		//		| 1 |   | 1 |
		//	for debug
		//DSTR << "vecf3array[" << l << "] : " << std::endl;
		//DSTR << vecf3array[l] << std::endl;

		//�W���̐ς��Ƃ�
		//���̐ߓ_�ō\�������l�ʑ̖̂ʐς̐ς��Ƃ�
		//�l�ʑ̂̐ߓ_1,2,3(0�ȊO)�ō��O�p�`�̖ʐ�
		//l==0�Ԗڂ̎��A 123	��������
		//l==1			0 23
		//l==2			01 3
		//l==3			012
		//��CalcTriangleArea�ɓ���邱�Ƃ��ł���悤�ɃA���S���Y�����l����B
		//k21
		if(l==0){
			//�O�p�`�ʂ��\������3���_�̔M�`�B���̑�������
			double tempHTR = (vertices[tets[id].vertices[1]].heatTransRatio + vertices[tets[id].vertices[2]].heatTransRatio + vertices[tets[id].vertices[3]].heatTransRatio ) / 3.0;		//HTR:HeatTransRatio
			vecf3array[l] = tempHTR * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[1],tets[id].vertices[2],tets[id].vertices[3] ) * vecf3array[l];
			//vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets.vertices[1],tets.vertices[2],tets.vertices[3] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vec�̐ߓ_���ɂ��̐ߓ_�ł̎��͗��̉��xTc�Ƃ̐ς��s��
			for(unsigned m=0; m<4; m++){
				vecf3array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf3array[l][m];
			}
		}
		//	k22
		else if(l==1){
			double tempHTR = (vertices[tets[id].vertices[0]].heatTransRatio + vertices[tets[id].vertices[2]].heatTransRatio + vertices[tets[id].vertices[3]].heatTransRatio ) / 3.0;		//HTR:HeatTransRatio
			vecf3array[l] = tempHTR * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[2],tets[id].vertices[3] ) * vecf3array[l];
			//vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[2],tets[id].vertices[3] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vec�̐ߓ_���ɂ��̐ߓ_�ł̎��͗��̉��xTc�Ƃ̐ς��s��
			for(unsigned m=0; m<4; m++){
				vecf3array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf3array[l][m];
			}
		}
		//	k23
		else if(l==2){
			double tempHTR = (vertices[tets[id].vertices[0]].heatTransRatio + vertices[tets[id].vertices[1]].heatTransRatio + vertices[tets[id].vertices[3]].heatTransRatio ) / 3.0;		//HTR:HeatTransRatio
			vecf3array[l] = tempHTR * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[3] ) * vecf3array[l];
			//vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[3] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vec�̐ߓ_���ɂ��̐ߓ_�ł̎��͗��̉��xTc�Ƃ̐ς��s��
			for(unsigned m=0; m<4; m++){
				vecf3array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf3array[l][m];
			}
		}
		//	k24
		else if(l==3){
			double tempHTR = (vertices[tets[id].vertices[0]].heatTransRatio + vertices[tets[id].vertices[1]].heatTransRatio + vertices[tets[id].vertices[2]].heatTransRatio ) / 3.0;		//HTR:HeatTransRatio
			vecf3array[l] = tempHTR * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[2] ) * vecf3array[l];
			//vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[2] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vec�̐ߓ_���ɂ��̐ߓ_�ł̎��͗��̉��xTc�Ƃ̐ς��s��
			for(unsigned m=0; m<4; m++){
				vecf3array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf3array[l][m];
			}
		}
		//for debug
		//DSTR << "vecf3array[" << l << "]�̊����ł́�" << std::endl;
		//DSTR << vecf3array[l] << std::endl;
		//if(dMatCAll == NULL){
		//	//DSTR <<"i : "<< i << ", l : " << l << std::endl;
		//	DSTR << "dMatCAll == NULL" <<std::endl;
		//	DSTR <<"l : " << l << std::endl;
		//}
	}

	//f3 = f31 + f32 + f33 + f34
	for(unsigned i=0; i < 4; i++){
		//vecf3 += vecf3array[i];
		tets[id].vecf[2] += vecf3array[i];
		//	for debug
		//DSTR << "vecf3 �� vecf3array = f3" << i+1 <<"�܂ŉ��Z�����s��" << std::endl;
		//DSTR << vecf3 << std::endl;
	}
	
	//	f1,f2,f3,f4	���v�Z����ۂɁA[0][0]��������[3][0]�����܂ł̔�0�����ɂ��āA���Tc�������Ă��܂�


	//for debug
	//DSTR << "�ߓ_�i";
	//for(unsigned i =0; i < 4; i++){
	//	DSTR << tets[id].vertices[i] << "," ;
	//}
	//DSTR << ")�ō\�������l�ʑ̂�" << std::endl;
	//DSTR << "vecf3 : " << std::endl;
	//DSTR << vecf3 << std::endl;
	//int hogeshishi =0;
}

double PHFemMeshThermo::CalcTetrahedraVolume(Tet tets){
	PTM::TMatrixRow<4,4,double> tempMat44;
	for(unsigned i =0; i < 4; i++){
		for(unsigned j =0; j < 4; j++){
			if(i == 0){
				tempMat44[i][j] = 1.0;
			}
			else if(i == 1){
				tempMat44[i][j] = vertices[tets.vertices[j]].pos.x;
			}
			else if(i == 2){
				tempMat44[i][j] = vertices[tets.vertices[j]].pos.y;
			}
			else if(i == 3){
				tempMat44[i][j] = vertices[tets.vertices[j]].pos.z;
			}
			
		}
	}
	//	for debug
	//DSTR << tempMat44 << std::endl;
	//for(unsigned i =0; i < 4 ;i++){
	//	DSTR << vertices[tets.vertices[i]].pos.x << " , " << vertices[tets.vertices[i]].pos.y << " , " << vertices[tets.vertices[i]].pos.z << std::endl; 
	//}
	//DSTR << tempMat44.det() << std::endl;
	//int hogever = 0;
	return tempMat44.det() / 6.0;
}

double PHFemMeshThermo::CalcTriangleArea(int id0, int id1, int id2){
	double area=0.0;								///	�v���P	faces[id].area�ɒl������ 

	//�s�񎮂̐�����p���Ėʐς����߂�
	//triarea =
	//|  1     1     1   |
	//|x2-x1 y2-y1 z2-z1 |
	//|x3-x1 y3-y1 z3-z1 |
	//|
	PTM::TMatrixRow<3,3,double> triarea;		//�O�p�`�̖ʐρ@= tri + area
	for(unsigned i =0 ; i < 3 ; i++){
		triarea[0][i] = 1.0;
	}
	for(unsigned i =0 ; i < 3 ; i++){
		//					x2(�ߓ_2��x(pos��i����)��)	-	x1(�V)
		// i==0�̎�	vertices[id1].pos[i]	=>	 pos[0] == pos.x
		triarea[1][i] = vertices[id1].pos[i] - vertices[id0].pos[i];
	}
	for(unsigned i =0 ; i < 3 ; i++){
		//					x3(�ߓ_3��x(pos��i����)��)	-	x1(�V)
		triarea[2][i] = vertices[id2].pos[i] - vertices[id0].pos[i];
	}
	double m1,m2,m3 = 0.0;
	m1 = triarea[1][1] * triarea[2][2] - triarea[1][2] * triarea[2][1];
	m2 = triarea[2][0] * triarea[1][2] - triarea[1][0] * triarea[2][2];
	m3 = triarea[1][0] * triarea[2][1] - triarea[2][0] * triarea[1][1];

	area = sqrt(m1 * m1 + m2 * m2 + m3 * m3) / 2.0;

	//	for debug
	//DSTR << "�O�p�`�̖ʐς� : " << area << std::endl; 

	//0�Ԗڂ̐ߓ_��40,1�Ԗڂ̐ߓ_��134,2�Ԗڂ̐ߓ_��79 �̍��W�Ōv�Z���Ă݂�
	//�O�p�`�����߂�s�� : 2.75949e-005 * 1 = 2.75949 �~ 10-5(byGoogle�v�Z�@) [m^2] = 2.75949 �~ 10-1 [cm^2]�Ȃ̂ŁA�l�M�̃��b�V���̃X�P�[���Ȃ��̂����Ă���͂�

	return area;
}

PTM::TMatrixCol<4,1,double> PHFemMeshThermo::Create41Vec1(){
	PTM::TMatrixCol<4,1,double> Mat1temp;
	for(int i =0; i <4 ; i++){
				Mat1temp[i][0] = 1.0;
	}
	return Mat1temp;
}

PTM::TMatrixRow<4,4,double> PHFemMeshThermo::Create44Mat21(){
	//|2 1 1 1 |
	//|1 2 1 1 |
	//|1 1 2 1 |
	//|1 1 1 2 |	�����
	PTM::TMatrixRow<4,4,double> MatTemp;
	for(int i =0; i <4 ; i++){
		for(int j=0; j < 4 ; j++){
			if(i==j){
				MatTemp[i][j] = 2.0;
			}else{
				MatTemp[i][j] = 1.0;
			}
		}
	}
	return MatTemp;
}
void PHFemMeshThermo::CreateMatk3t(unsigned id){
	//l=0�̎�k21,1�̎�:k22, 2�̎�:k23, 3�̎�:k24	�𐶐�
	for(unsigned l= 0 ; l < 4; l++){
		//matk2array[l] = matk2temp;
		matk3array[l] = Create44Mat21();
		//	1�si���0��
		for(int i=0;i<4;i++){
			matk3array[l][l][i] = 0.0;
		}
		//	i�s1���0��
		for(int i=0;i<4;i++){
			matk3array[l][i][l] = 0.0;
		}
	}

	///	������
	tets[id].matk[2].clear();
	
	for(unsigned l= 0 ; l < 4; l++){
		///	�l�ʑ̂̊e��(l = 0 �` 3) �ɂ��ă��b�V���\�ʂ��ǂ������`�F�b�N����B�\�ʂȂ�A�s��������matk2array�ɓ����
		//faces[tets.faces[i]].sorted;		/// 1,24,58�݂����Ȑߓ_�ԍ��������Ă���
		///	�s��^�̓��ꕨ��p��

		//faces[tets.faces[l]].vertices;
		if(tets[id].faces[l] < (int)nSurfaceFace && faces[tets[id].faces[l]].alphaUpdated ){			///	�O�k�̖� ���� �M�`�B�����X�V���ꂽ�� matk2���X�V����K�v������
			//�Ō�ɓ����s���������
			for(unsigned i =0; i < 4 ;i++){
				for(unsigned j =0; j < 4 ;j++){
					tets[id].matk[2][i][j] = 0.0;
				}
			}
			///	�l�ʑ̂̎O�p�`�̖ʐς��v�Z		///	���̊֐��̊O�Ŗʐϕ��̖ʐόv�Z����������B�ړ�����
			if(faces[tets[id].faces[l]].area ==0 || faces[tets[id].faces[l]].deformed ){		///	�ʐς��v�Z����Ă��Ȃ����i�͂��߁j or deformed(�ό`�������E�������)��true�̎�		///	�����̒ǉ�	�ʐς�0�� ||(OR) �����X�V���ꂽ��
				faces[tets[id].faces[l]].area = CalcTriangleArea(faces[tets[id].faces[l]].vertices[0], faces[tets[id].faces[l]].vertices[1], faces[tets[id].faces[l]].vertices[2]);
				faces[tets[id].faces[l]].deformed = false;
			}
			///	�v�Z���ʂ��s��ɑ��
			///	area�̌v�Z�Ɏg���Ă��Ȃ��_�������Ă���s�Ɨ���������s��̐ς��Ƃ�
			///	�ϕ��v�Z�����{����l����
			unsigned vtx = tets[id].vertices[0] + tets[id].vertices[1] + tets[id].vertices[2] + tets[id].vertices[3];
			//DSTR << "vtx: " << vtx <<std::endl;
				///	area�v�Z�Ɏg���Ă��Ȃ��ߓ_ID�FID
			unsigned ID = vtx -( faces[tets[id].faces[l]].vertices[0] + faces[tets[id].faces[l]].vertices[1] + faces[tets[id].faces[l]].vertices[2] );
			//DSTR << "���b�V���\�ʂ̖ʂ͎���3���_����Ȃ�B" << std::endl;
			//DSTR << "faces[tets.faces[l]].vertices[0]: " << faces[tets.faces[l]].vertices[0] <<std::endl;
			//DSTR << "faces[tets.faces[l]].vertices[1]: " << faces[tets.faces[l]].vertices[1] <<std::endl;
			//DSTR << "faces[tets.faces[l]].vertices[2]: " << faces[tets.faces[l]].vertices[2] <<std::endl;
			//DSTR << "ID: " << ID <<"�̂Ƃ��̐ߓ_�ƑΖʂ���ʂŖʐϕ����v�Z����"<<std::endl;
			for(unsigned j=0;j<4;j++){
				if(tets[id].vertices[j] == ID){					///	�`��֐����P�A�i���Ȃ킿�j����face�ɑΖʂ��钸�_�@�ƈ�v������@���̎���face�Ŗʐϕ�����
					///	j�Ԗڂ̍s��̐�����0�ɂ���matk2array�Ōv�Z����
					///	�O�k�ɂȂ����b�V���ʂ̖ʐς�0�ŏ��������Ă���
					faces[tets[id].faces[l]].thermalEmissivity = (vertices[faces[tets[id].faces[l]].vertices[0]].thermalEmissivity + vertices[faces[tets[id].faces[l]].vertices[1]].thermalEmissivity 
						+ vertices[faces[tets[id].faces[l]].vertices[2]].thermalEmissivity ) / 3.0;		///	���Yface�̔M�`�B�����\���ߓ_�ł̒l�̑������ς��Ƃ�
					///	�M�t�˗����v�Z�A�M�t�˗����A�ߓ_�̔M�t�˗�����v�Z�H

					///	�ȉ���[]�͏�܂ł�[l]�ƈقȂ�B
					///	ID�����Ԗڂ��ɂ���āA�`��֐��̌W�����قȂ�̂ŁA
					tets[id].matk[2] += faces[tets[id].faces[l]].thermalEmissivity * (1.0/12.0) * faces[tets[id].faces[l]].area * matk2array[j];
					//DSTR << "tets[id].matk2��faces[tets[id].faces[l]].heatTransRatio * (1.0/12.0) * faces[tets[id].faces[l]].area * matk2array[" << j << "]"<< "�����Z: " <<faces[tets[id].faces[l]].heatTransRatio * (1.0/12.0) * faces[tets[id].faces[l]].area * matk2array[j] << std::endl;
					//DSTR << "tets[id].matk2 +=  " << tets[id].matk2 << std::endl;
				}
			}
		}
	}
}
void PHFemMeshThermo::CreateMatk2t(unsigned id){

	//l=0�̎�k21,1�̎�:k22, 2�̎�:k23, 3�̎�:k24	�𐶐�
	for(unsigned l= 0 ; l < 4; l++){
		//matk2array[l] = matk2temp;
		matk2array[l] = Create44Mat21();
		//	1�si���0��
		for(int i=0;i<4;i++){
			matk2array[l][l][i] = 0.0;
		}
		//	i�s1���0��
		for(int i=0;i<4;i++){
			matk2array[l][i][l] = 0.0;
		}
	}

	///	������
	tets[id].matk[1].clear();
	//for(unsigned i =0; i < 4 ;i++){
	//	for(unsigned j =0; j < 4 ;j++){
	//		//tets[id].matk2[i][j] = 0.0;
	//		tets[id].matk[1][i][j] = 0.0;
	//	}
	//}

	//	Check
	//DSTR << "matk2array:" << std::endl;
	//for(unsigned i=0;i<4;i++){
	//	DSTR <<i <<": " << matk2array[i] << std::endl;
	//}
	//DSTR << "++i" <<std::endl;
	//for(unsigned i=0;i<4;++i){
	//	DSTR <<i <<": " << matk2array[i] << std::endl;
	//}


	for(unsigned l= 0 ; l < 4; l++){
		///	�l�ʑ̂̊e��(l = 0 �` 3) �ɂ��ă��b�V���\�ʂ��ǂ������`�F�b�N����B�\�ʂȂ�A�s��������matk2array�ɓ����
		//faces[tets.faces[i]].sorted;		/// 1,24,58�݂����Ȑߓ_�ԍ��������Ă���
		///	�s��^�̓��ꕨ��p��

		//faces[tets.faces[l]].vertices;
		if(tets[id].faces[l] < (int)nSurfaceFace && faces[tets[id].faces[l]].alphaUpdated ){			///	�O�k�̖� ���� �M�`�B�����X�V���ꂽ�� matk2���X�V����K�v������
			//�Ō�ɓ����s���������
			for(unsigned i =0; i < 4 ;i++){
				for(unsigned j =0; j < 4 ;j++){
					//matk2[i][j] = 0.0;
					//tets[id].matk2[i][j] = 0.0;
					tets[id].matk[1][i][j] = 0.0;
				}
			}
			///	�l�ʑ̂̎O�p�`�̖ʐς��v�Z		///	���̊֐��̊O�Ŗʐϕ��̖ʐόv�Z����������B�ړ�����
			if(faces[tets[id].faces[l]].area ==0 || faces[tets[id].faces[l]].deformed ){		///	�ʐς��v�Z����Ă��Ȃ����i�͂��߁j or deformed(�ό`�������E�������)��true�̎�		///	�����̒ǉ�	�ʐς�0�� ||(OR) �����X�V���ꂽ��
				faces[tets[id].faces[l]].area = CalcTriangleArea(faces[tets[id].faces[l]].vertices[0], faces[tets[id].faces[l]].vertices[1], faces[tets[id].faces[l]].vertices[2]);
				faces[tets[id].faces[l]].deformed = false;
			}
			///	�v�Z���ʂ��s��ɑ��
			///	area�̌v�Z�Ɏg���Ă��Ȃ��_�������Ă���s�Ɨ���������s��̐ς��Ƃ�
			///	�ϕ��v�Z�����{����l����
			unsigned vtx = tets[id].vertices[0] + tets[id].vertices[1] + tets[id].vertices[2] + tets[id].vertices[3];
			//DSTR << "vtx: " << vtx <<std::endl;
			
			///	area�v�Z�Ɏg���Ă��Ȃ��ߓ_ID�FID
			unsigned ID = vtx -( faces[tets[id].faces[l]].vertices[0] + faces[tets[id].faces[l]].vertices[1] + faces[tets[id].faces[l]].vertices[2] );
			//DSTR << "���b�V���\�ʂ̖ʂ͎���3���_����Ȃ�B" << std::endl;
			//DSTR << "faces[tets.faces[l]].vertices[0]: " << faces[tets.faces[l]].vertices[0] <<std::endl;
			//DSTR << "faces[tets.faces[l]].vertices[1]: " << faces[tets.faces[l]].vertices[1] <<std::endl;
			//DSTR << "faces[tets.faces[l]].vertices[2]: " << faces[tets.faces[l]].vertices[2] <<std::endl;
			//DSTR << "ID: " << ID <<"�̂Ƃ��̐ߓ_�ƑΖʂ���ʂŖʐϕ����v�Z����"<<std::endl;
			for(unsigned j=0;j<4;j++){
				if(tets[id].vertices[j] == ID){					///	�`��֐����P�A�i���Ȃ킿�j����face�ɑΖʂ��钸�_�@�ƈ�v������@���̎���face�Ŗʐϕ�����
					///	j�Ԗڂ̍s��̐�����0�ɂ���matk2array�Ōv�Z����
					///	�O�k�ɂȂ����b�V���ʂ̖ʐς�0�ŏ��������Ă���
					faces[tets[id].faces[l]].heatTransRatio = (vertices[faces[tets[id].faces[l]].vertices[0]].heatTransRatio + vertices[faces[tets[id].faces[l]].vertices[1]].heatTransRatio 
						+ vertices[faces[tets[id].faces[l]].vertices[2]].heatTransRatio ) / 3.0;		///	���Yface�̔M�`�B�����\���ߓ_�ł̒l�̑������ς��Ƃ�
					///	�ȉ���[]�͏�܂ł�[l]�ƈقȂ�B
					///	ID�����Ԗڂ��ɂ���āA�`��֐��̌W�����قȂ�̂ŁA
					//tets[id].matk2 += faces[tets[id].faces[l]].heatTransRatio * (1.0/12.0) * faces[tets[id].faces[l]].area * matk2array[j];
					tets[id].matk[1] += faces[tets[id].faces[l]].heatTransRatio * (1.0/12.0) * faces[tets[id].faces[l]].area * matk2array[j];
					//DSTR << "tets[id].matk2��faces[tets[id].faces[l]].heatTransRatio * (1.0/12.0) * faces[tets[id].faces[l]].area * matk2array[" << j << "]"<< "�����Z: " <<faces[tets[id].faces[l]].heatTransRatio * (1.0/12.0) * faces[tets[id].faces[l]].area * matk2array[j] << std::endl;
					//DSTR << "tets[id].matk2 +=  " << tets[id].matk2 << std::endl;
				}
				//else{
				//	///	ID�ƈ�v���Ȃ��ꍇ�ɂ́Amatk2array[j]�ɂ͑S����0������
				//	///	�Ƃ������Ƃ��낾���A
				//	//matk2array[j] =0.0 * matk2array[j];
				//	//DSTR << "matk2array[" << j << "]: " << matk2array[j] << std::endl;
				//}
			}
		}
		///	SurfaceFace����Ȃ�������Amatk2array�ɂ�0������
		//else{
		//	//matk2array[l];
		//}
	}

	//DSTR << "matk2array:" << std::endl;
	//for(unsigned i=0;i<4;i++){
	//	DSTR <<i <<": " << matk2array[i] << std::endl;
	//}

	//k2 = k21 + k22 + k23 + k24
	//for(unsigned i=0; i < 4; i++){
	//	matk2 += matk2array[i];
	//	//	for debug
	//	//DSTR << "matk2 �� matk2array = k2" << i+1 <<"�܂ŉ��Z�����s��" << std::endl;
	//	//DSTR << matk2 << std::endl;
	//}
	
	//for debug
	//DSTR << "�ߓ_�i";
	//for(unsigned i =0; i < 4; i++){
	//	DSTR << tets[id].vertices[i] << "," ;
	//}
	//DSTR << ")�ō\�������l�ʑ̂�" << std::endl;
	//DSTR << "matk2 : " << std::endl;
	//DSTR << matk2 << std::endl;
	//int hogeshishi =0;
	
	//DSTR << "Inner Function _ matk2t tets[id].matk2: " <<tets[id].matk2 << std::endl;
}
void PHFemMeshThermo::CreateMatk2(unsigned id,Tet tets){
	//���̌v�Z���Ăяo���Ƃ��ɁA�e�l�ʑ̂��ƂɌv�Z���邽�߁A�l�ʑ̂�0�Ԃ��珇�ɂ��̌v�Z���s��
	//�l�ʑ̂��\������4�ߓ_��ߓ_�̔z��(Tets�ɂ́A�ߓ_�̔z�񂪍���Ă���)�ɓ����Ă��鏇�Ԃ��g���āA�ʂ̌v�Z���s������A�s��̌v�Z���s�����肷��B
	//���̂��߁A���̊֐��̈����ɁA�l�ʑ̗v�f�̔ԍ������

	//�Ō�ɓ����s���������
	for(unsigned i =0; i < 4 ;i++){
		for(unsigned j =0; j < 4 ;j++){
			//matk2[i][j] = 0.0;
			tets.matk[1][i][j] = 0.0;
		}
	}

	//2)	
	//l=0�̎�k21,1:k22, 2:k23, 3:k24	�𐶐�
	for(unsigned l= 0 ; l < 4; l++){
		//matk2array[l] = matk2temp;
		matk2array[l] = Create44Mat21();
		//	1�si���0��
		for(int i=0;i<4;i++){
			matk2array[l][l][i] = 0.0;
		}
		//	i�s1���0��
		for(int i=0;i<4;i++){
			matk2array[l][i][l] = 0.0;
		}
		//	k_21	
		// =	|0 0 0 0 |
		//		|0 2 1 1 |
		//		|0 1 2 1 |
		//		|0 1 1 2 |
		//	for debug
		//DSTR <<"matk2array[" << l << "] : " << std::endl;
		//DSTR << matk2array[l] << std::endl;

		//�W���̐ς��Ƃ�
		//���̐ߓ_�ō\�������l�ʑ̖̂ʐς̐ς��Ƃ�

		//�ߓ_�����Ă݂悤��
		//for(unsigned i =0; i < 4 ; i++){
		//	DSTR << "k2"<< l << "�s��� "<< i <<"�Ԗڂ̐ߓ_��" << tets.vertices[i] << std::endl;
		//}
	
		//�l�ʑ̂̐ߓ_1,2,3(0�ȊO)�ō��O�p�`�̖ʐ�
		//l==0�Ԗڂ̎��A 123	��������
		//l==1			0 23
		//l==2			01 3
		//l==3			012
		//��CalcTriangleArea�ɓ���邱�Ƃ��ł���悤�ɃA���S���Y�����l����B
		//k21
		//���̖ʂ͊O�k���H�O�k�Ȃ�A�v�Z�����s
			///	vertices
			///	surfaceVertices���ǂ����Ŕ���
		//area�͌v�Z����Ă��Ȃ����H����Ă���̂Ȃ�Afaces.area�̒l���g��
		if(l==0){
			///	�ʂ̔M�`�B�W�����A
			double tempHTR = (vertices[tets.vertices[1]].heatTransRatio + vertices[tets.vertices[2]].heatTransRatio + vertices[tets.vertices[3]].heatTransRatio ) / 3.0;
			matk2array[l] = tempHTR * (1.0/12.0) * CalcTriangleArea( tets.vertices[1],tets.vertices[2],tets.vertices[3] ) * matk2array[l];
		}
		//	k22
		else if(l==1){
			double tempHTR = (vertices[tets.vertices[0]].heatTransRatio + vertices[tets.vertices[2]].heatTransRatio + vertices[tets.vertices[3]].heatTransRatio ) / 3.0;
			matk2array[l] = tempHTR * (1.0/12.0) * CalcTriangleArea( tets.vertices[0],tets.vertices[2],tets.vertices[3] ) * matk2array[l];
		}
		//	k23
		else if(l==2){
			double tempHTR = (vertices[tets.vertices[0]].heatTransRatio + vertices[tets.vertices[1]].heatTransRatio + vertices[tets.vertices[3]].heatTransRatio ) / 3.0;
			matk2array[l] = tempHTR * (1.0/12.0) * CalcTriangleArea( tets.vertices[0],tets.vertices[1],tets.vertices[3] ) * matk2array[l];
		}
		//	k24
		else if(l==3){
			double tempHTR = (vertices[tets.vertices[0]].heatTransRatio + vertices[tets.vertices[1]].heatTransRatio + vertices[tets.vertices[2]].heatTransRatio ) / 3.0;
			matk2array[l] = tempHTR * (1.0/12.0) * CalcTriangleArea( tets.vertices[0],tets.vertices[1],tets.vertices[2] ) * matk2array[l];
		}
		//for debug
		//DSTR << "matk2array[" << l << "]�̊����ł́�" << std::endl;
		//DSTR << matk2array[l] << std::endl;
	}

	//k2 = k21 + k22 + k23 + k24
	for(unsigned i=0; i < 4; i++){
		tets.matk[1] += matk2array[i];
		//	for debug
		//DSTR << "matk2 �� matk2array = k2" << i+1 <<"�܂ŉ��Z�����s��" << std::endl;
		//DSTR << matk2 << std::endl;
	}
	
	//for debug
	//DSTR << "�ߓ_�i";
	//for(unsigned i =0; i < 4; i++){
	//	DSTR << tets.vertices[i] << "," ;
	//}
	//DSTR << ")�ō\�������l�ʑ̂�" << std::endl;
	//DSTR << "matk2 : " << std::endl;
	//DSTR << matk2 << std::endl;
	//int hogeshishi =0;
}

void PHFemMeshThermo::SetInitThermoConductionParam(double thConduct0,double roh0,double specificHeat0,double heatTrans0){
	thConduct = thConduct0;
	rho = roh0;
	specificHeat = specificHeat0;
	heatTrans = heatTrans0;
}
void PHFemMeshThermo::SetThermalEmissivityToVtx(unsigned id,double thermalEmissivity){
	vertices[id].thermalEmissivity = thermalEmissivity;
}
void PHFemMeshThermo::SetThermalEmissivityToVerticesAll(double thermalEmissivity){
	for(unsigned i =0; i < vertices.size(); i++){
		vertices[i].thermalEmissivity = thermalEmissivity;
	}
}
void PHFemMeshThermo::SetHeatTransRatioToAllVertex(){
	for(unsigned i =0; i < vertices.size() ; i++){
		vertices[i].heatTransRatio = heatTrans;
	}
}

void PHFemMeshThermo::SetTempAllToTVecAll(unsigned size){
	for(unsigned i =0; i < size;i++){
		TVecAll[i] = vertices[i].temp;
	}
}

void PHFemMeshThermo::CreateTempMatrix(){
	unsigned dmnN = (unsigned)vertices.size();
	TVecAll.resize(dmnN);
	SetTempAllToTVecAll(dmnN);
	//for(std::vector<unsigned int>::size_type i=0; i < dmnN ; i++){
	//	TVecAll[i] = vertices[i].temp;
	//}

	//for Debug
	//for(unsigned int i =0; i < dmnN; i++){
	//	DSTR << i <<" : " << TVecAll[i] << std::endl;
	//}
	//for debug
	//for(std::vector<unsigned int>::size_type i =0; i < vertices.size(); i++){
	//	DSTR << i << " : " << &vertices[i] << std::endl;
	//}

}

void PHFemMeshThermo::SetTempToTVecAll(unsigned vtxid){
	TVecAll[vtxid] = vertices[vtxid].temp;
}

void PHFemMeshThermo::UpdateheatTransRatio(unsigned id,double heatTransRatio){
	//if(vertices[id].heatTransRatio != heatTransRatio){	//�قȂ��Ă�����
	//	vertices[id].heatTransRatio = heatTransRatio;	
	//	///	�����܂ލs��̍X�V	K2,f3
	//	///	f3
	//	for(unsigned i =0; i < vertices[1].tets.size(); i++){
	//		CreateVecf3(tets[vertices[id].tets[i]]);
	//	}
	//	///	K3

	//}
	///	�����Ȃ牽�����Ȃ�
}

void PHFemMeshThermo::SetLocalFluidTemp(unsigned i,double temp){
	vertices[i].Tc = temp;			//�ߓ_�̎��͗��̉��x�̐ݒ�
}

void PHFemMeshThermo::SetVertexTemp(unsigned i,double temp){
	vertices[i].temp = temp;
	SetTempToTVecAll(i);
}

void PHFemMeshThermo::SetVerticesTempAll(double temp){
	for(std::vector<unsigned int>::size_type i=0; i < vertices.size() ; i++){
			vertices[i].temp = temp;
		}
}

void PHFemMeshThermo::SetvecFAll(unsigned id,double dqdt){
	vecFAll[id][0] = dqdt;
}

}


