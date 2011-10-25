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


using namespace PTM;

namespace Spr{;

#define UseMatAll

PHFemMeshThermoDesc::PHFemMeshThermoDesc(){
	Init();
}
void PHFemMeshThermoDesc::Init(){
	//�f�B�X�N���v�^�̐������ɌĂ΂��R���X�g���N�^�ŌĂ΂��
	//�f�B�X�N���v�^�������̏��������������Ƃ���������
	//�ȉ��̊֐��̒��ōs���Ă��鏉�����̗ނ������ł��̂��ǂ�����
	//�f�B�X�N���v�^�ɓ����l�̏�����?
	

}

///////////////////////////////////////////////////////////////////
//	PHFemMeshThermo


PHFemMeshThermo::PHFemMeshThermo(const PHFemMeshThermoDesc& desc, SceneIf* s){
	deformed = true;			//�ϐ��̏������A�`�󂪕ς�������ǂ���
	SetDesc(&desc);
	if (s){ SetScene(s); }
}

void PHFemMeshThermo::SetThermalBoundaryCondition(){
	
}

void PHFemMeshThermo::PrepareStep(){
	//�K�E�X�U�C�f���ɕK�v�ȁA�v�Z���̌W�����v�Z����
	double dt = DCAST(PHSceneIf, GetScene())->GetTimeStep();
	//�W��b��DMatAll_�Ȃǂ������ō��
	//bVecAll�̃��T�C�Y
	//bVecAll�Ɍv�Z���ʂ��i�[
	//�������A[K],[C]�Ȃǂ͑S�̍����s�������Ă���̂ł͂Ȃ��A�������Ƃ�Edges�\���̂ɓ����Ă���̂ŁA���̒l��p����
	//�W���s��b�������[�v�˂��̃��[�v���K�E�X�U�C�f���v�Z�̍ŏ��̈�񂾂�������ق����Afor���[�v��1�񏭂Ȃ��Ȃ�̂ŁA�v�Z�������B���ǁAif�����K�v
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
	SetVerticesTemp(id,temp);
	//for(unsigned i =0;i < vertices.size()/3; i++){
	//	SetVerticesTemp(i,temp);
	//}
}

void PHFemMeshThermo::UsingHeatTransferBoundaryCondition(){
	//�M�`�B���E����
	//�ߓ_�̎��͗��̉��x�̐ݒ�(K,C,F�Ȃǂ̍s��x�N�g���̍쐬��Ɏ��s�K�v����)
	//SetLocalFluidTemp(2,200.0);
}

void PHFemMeshThermo::CalcHeatTransUsingGaussSeidel(unsigned NofCyc,double dt){
	//dt = 0.0000000000001 * dt;		//�f�o�b�O�p�ɁAdt�����̂��������������Ă��A�ߓ_0���}�C�i�X�ɂȂ�̂��A���ׂ�
	
	//dt��PHFemEngine.cpp�Ŏ擾���铮�͊w�V�~�����[�V�����̃X�e�b�v����
	bool DoCalc =true;											//���񂾂��萔�x�N�g��b�̌v�Z���s��bool		//NofCyc��0�̎��ɂ���΂����̂���
	std::ofstream ofs("log.txt");
	for(unsigned i=0; i < NofCyc; i++){							//�K�E�X�U�C�f���̌v�Z���[�v
		if(DoCalc){												
			if(deformed){												//D_ii�̍쐬�@�`�󂪍X�V���ꂽ�ۂ�1�x�����s���΂悢
				for(unsigned j =0; j < vertices.size() ; j++){
					//for(unsigned k =0;k < vertices.size(); k++){
					//	DSTR << "DMatCAll "<< k << " : " << DMatCAll[0][k] << std::endl;
					//}
					_DMatAll.resize(1,vertices.size());
					_DMatAll[0][j] = 1.0/ ( 1.0/2.0 * DMatKAll[0][j] + 1.0/dt * DMatCAll[0][j] );		//1 / D__ii	�����߂�
					//1.0/dt = 500 
					//DSTR << "DMatKAll : "  << DMatKAll << std::endl;
					//DSTR << "DMatCAll : "  << DMatCAll << std::endl;
					//DSTR << "1.0/dt : " << 1.0/dt <<std::endl;
					//DSTR <<  1.0/dt *DMatCAll[0][j] << std::endl;		//0.001�̃I�[�_�[
					//DSTR << 1.0/2.0 * DMatKAll[0][j] << std::endl;		//0.0003�O��̃I�[�_�[
					//�l�������Ă��邩���`�F�b�N
					//DSTR << "_DMatAll[0][" << j << "] : " << _DMatAll[0][j]  << std::endl;
					int debughogeshi =0;
				}
				deformed = false;
				//	for DEBUG
				//DSTR << "_DMatAll : " << std::endl;
				//for(unsigned j =0; j < vertices.size() ;j++){
				//	DSTR << j << " : " << _DMatAll[0][j] << std::endl;
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
						bVecAll[j][0] += (-1.0/2.0 * edges[edgeId].k + 1.0/dt * edges[edgeId].c ) * TVecAll[vtxid0][0];
					}
					else if( j != edges[edgeId].vertices[1] ){			//�ߓ_�ԍ�j��edges.vertices[1]���قȂ�ߓ_�ԍ��̎�:��Ίp����
						unsigned vtxid1 = edges[edgeId].vertices[1];
						bVecAll[j][0] += (-1.0/2.0 * edges[edgeId].k + 1.0/dt * edges[edgeId].c ) * TVecAll[vtxid1][0];
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
				bVecAll[j][0] += (-1.0/2.0 * DMatKAll[0][j] + 1.0/dt * DMatCAll[0][j] ) * TVecAll[j][0];
				DSTR << "bVecAll[" << j <<"][0] : " << bVecAll[j][0] << std::endl;
				//{F}�����Z
				bVecAll[j][0] += VecFAll[j][0];		//F�����Z
				//DSTR << " VecFAll[" << j << "][0] : "  << VecFAll[j][0] << std::endl;
				DSTR << std::endl;
				//D_ii�Ŋ��� �˂��̏ꏊ�́A�����ŗǂ��́H�ǂ��܂Ŋ|���Z����́H
				bVecAll[j][0] = bVecAll[j][0] * _DMatAll[0][j];
				DSTR << "bVecAll[" << j <<"][0] * _DMatAll : " << bVecAll[j][0] << std::endl;
				DSTR << "TVecAll[" << j <<"][0] : " << TVecAll[j][0] << std::endl;
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
					TVecAll[j][0] = 0.0;
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
					//DSTR << "TVecAll["<< vtxid0<<"][0] : " << TVecAll[vtxid0][0] <<std::endl;
					//TVecAll[j][0] +=_DMatAll[j][0] * -(1.0/2.0 * edges[edgeId].k + 1.0/dt * edges[edgeId].c ) * TVecAll[vtxid0][0] + bVecAll[j][0]; 
					//DSTR << "j : " << j << ", vtxid0 : " << vtxid0 <<", edges[edgeId].vertices[0] : " << edges[edgeId].vertices[0] <<  std::endl;
					tempkj += (1.0/2.0 * edges[edgeId].k + 1.0/dt * edges[edgeId].c ) * TVecAll[vtxid0][0];
				}
				else if( j != edges[edgeId].vertices[1] ){			//�ߓ_�ԍ�j��edges.vertices[1]���قȂ�ߓ_�ԍ��̎�:��Ίp����
					unsigned vtxid1 = edges[edgeId].vertices[1];
					//DSTR << "TVecAll["<< vtxid1<<"][0] : " << TVecAll[vtxid1][0] <<std::endl;
					tempkj += (1.0/2.0 * edges[edgeId].k + 1.0/dt * edges[edgeId].c ) * TVecAll[vtxid1][0];
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
//			TVecAll[j][0] =	_DMatAll[0][j] * ( -1.0 * tempkj) - bVecAll[j][0];
			TVecAll[j][0] =	_DMatAll[0][j] * ( -1.0 * tempkj) + bVecAll[j][0];
			////	for DEBUG
			//int hofgeshi =0;
			//if(TVecAll[j][0] != 0.0){
			//	DSTR << "!=0 TVecAll["<< j<<"][0] : " << TVecAll[j][0] <<std::endl;
			//}
			//DSTR << i << "��ڂ̌v�Z�A" << j <<"�s�ڂ�tempkj: " << tempkj << std::endl;
			//tempkj =0.0;
			ofs << j << std::endl;
			ofs << "tempkj: "<< tempkj << std::endl;
			ofs << "DMatAll[0][j] * ( -1.0 * tempkj) :" <<_DMatAll[0][j] * ( -1.0 * tempkj) << std::endl;
			ofs << "bVecAll[j][0] :  " <<  bVecAll[j][0] << std::endl;
			ofs << "  TVecAll[j][0] : " << TVecAll[j][0] << std::endl;
			ofs << std::endl;
		}
		////	for Debug
		//for(unsigned j=0;j < vertices.size();j++){
		//	//DSTR << "tempk" << j << " : " << tempkj << std::endl;
		//	int hogeshi__ =0;
		//	//TVecAll[j][0]�̌v�Z���ʂ�������
		//	//�萔�x�N�g��b����Ōv�Z�A���s��bVecAll�����Z����΂悢�B
		//	DSTR << i << "��ڂ̌v�Z�� " << "TVecAll[" << j << "][0] : " << TVecAll[j][0] << std::endl;
		//}
		//for(unsigned j=0;j < vertices.size();j++){
		//	//DSTR << "tempk" << j << " : " << tempkj << std::endl;
		//	int hogeshi__ =0;
		//	//TVecAll[j][0]�̌v�Z���ʂ�������
		//	//�萔�x�N�g��b����Ōv�Z�A���s��bVecAll�����Z����΂悢�B
		//	DSTR << i << "��ڂ̌v�Z�� " << "bVecAll[" << j << "][0] : " << bVecAll[j][0] << std::endl;
		//}
		DSTR << i <<  "th Cyc" << std::endl; 
		DSTR << i << "��ڂ̌v�Z�ATVecAll : " <<std::endl;
		DSTR << TVecAll << std::endl;
		ofs << i <<  "th Cyc" << std::endl;
		ofs << i << "��ڂ̌v�Z�ATVecAll : " <<std::endl;
		ofs << TVecAll << std::endl;
		ofs << "bVecAll: " <<std::endl;
		ofs << bVecAll << std::endl;
		ofs << "_DMatAll: " <<std::endl; 
		ofs << _DMatAll <<std::endl;
		int piyopiyoyo =0;
		double tempTemp=0.0;
		for(unsigned j=0;j <vertices.size() ; j++){
			tempTemp += TVecAll[j][0];
		}
		DSTR << i <<"��ڂ̌v�Z���́@�S�ߓ_�̉��x�̘a : " << tempTemp << std::endl;
		DSTR << std::endl;
	}
}

void PHFemMeshThermo::UpdateVertexTempAll(unsigned size){
	for(unsigned i=0;i < size;i++){
		vertices[i].temp = TVecAll[i][0];
	}
}
void PHFemMeshThermo::UpdateVertexTemp(unsigned vtxid){
		vertices[vtxid].temp = TVecAll[vtxid][0];
}


void PHFemMeshThermo::Step(double dt){
	
//	ScilabTest();									//	Scilab���g���e�X�g
	//���E������ݒ�:���x�̐ݒ�
	UsingFixedTempBoundaryCondition(0,1.0);
	//
	CalcHeatTransUsingGaussSeidel(20,dt);			//�K�E�X�U�C�f���@�ŔM�`���v�Z������

	//���x��\�����Ă݂�
	//DSTR << "vertices[3].temp : " << vertices[3].temp << std::endl;

	//���x�̃x�N�g������ߓ_�։��x�̔��f
	UpdateVertexTempAll(vertices.size());


	for(unsigned i =0;i<vertices.size();i++){
		if(vertices[i].temp !=0){
			DSTR << "vertices[" << i << "].temp : " << vertices[i].temp << std::endl;
		}
	}
	int hogehoge=0;


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
}

void PHFemMeshThermo::CreateMatrix(){
}


void PHFemMeshThermo::SetDesc(const void* p) {
	PHFemMeshThermoDesc* d = (PHFemMeshThermoDesc*)p;
	PHFemMesh::SetDesc(d);
	
	////���ԍ��ݕ�	dt�̐ݒ�
	//PHFemMeshThermo::dt = 0.01;

	//�V�[������A�V�[���̎��ԃX�e�b�v���擾����ˈȉ��̃R�[�h�ł́AGetTimeStep���悤�Ƃ���ƁANULL���Ԃ��Ă���̂ŁAPHEngine�ōs��
	//DSTR << DCAST(PHSceneIf, GetScene()) << std::endl;
	//DSTR << DCAST(PHSceneIf, GetScene())->GetTimeStep() << std::endl;
	//int hogeshimitake =0;


	//�e�탁���o�ϐ��̏������˃R���X�g���N�^�łł����ق���������������Ȃ��B
	//Edges
	for(unsigned i =0; i < edges.size();i++){
		edges[i].c = 0.0;
		edges[i].k = 0.0;
	}

	//�s��̐������Ȃǂ�������
	bVecAll.resize(vertices.size(),1);

	//�s������
		//�s�����邽�߂ɕK�v�Ȑߓ_��l�ʑ̂̏��́APHFemMeshThermo�̍\���̂ɓ����Ă���B
			//PHFemMeshThermo�̃I�u�W�F�N�g�����ۂɁA�f�B�X�N���v�^�ɒl��ݒ肵�č��
		
	//�ߓ_���x�̏����ݒ�(�s������O�ɍs��)
	SetVerticesTemp(0.0);

			
	//�ߓ_�̏������x��ݒ肷���{T}�c�x�N�g���ɑ��
		//{T}�c�x�N�g�����쐬����B�ȍ~��K,C,F�s��E�x�N�g���̐ߓ_�ԍ��́A���̏c�x�N�g���̐ߓ_�̕��я��ɍ��킹��?
		
	//dmnN �����̉��x�̏c�i��j�x�N�g��
	CreateTempMatrix();

	//�M�`�����A���x�A��M�A�M�`�B���@�̃p�����[�^�[��ݒ�E���
		//PHFemMEshThermo�̃����o�ϐ��̒l���� CADThermo���A0.574;//�ʂ˂��̒l//�M�`����[W/(���EK)]�@Cp = 1.96 * (Ndt);//�ʂ˂��̔�M[kJ/(kg�EK) 1.96kJ/(kg K),�i�ʂ˂��̖��x�j�H�i���M�̉Ȋwp64���970kg/m^3
		//�M�`�B���̒P�ʌn�@W/(m^2 K)�˂����SI�P�ʌn�Ȃ̂��H�@25�͘_��(MEAT COOKING SIMULATION BY FINITE ELEMENTS)�̃I�[�u�����M���̎����l
		SetInitThermoConductionParam(0.574,970,1.96,25);
		//�����A�ϐ��l�͌ォ��v�Z�̓r���ŕύX�ł���悤��SetParam()�֐�������Ă������ق����������ȁH

	//�v�Z�ɗp����}�g���N�X�A�x�N�g�����쐬�i���b�V�����Ƃ̗v�f�����s��/�x�N�g���ˑS�̍����s��/�x�N�g���j
		//{T}�c�x�N�g���̐ߓ_�̕��я��ɕ��Ԃ悤�ɁA�W���s������Z����B�W���s��ɂ́A�ʐς�̐ρA�M�`�B���Ȃǂ̃p�����[�^�̐ς����Ă��܂������̂�����B
	
	//k2�̍s������
	//CreateMatk2array();
//	CreateLocalMatrixAndSet();

	//�e�s�������āA�K�E�X�U�C�f���Ōv�Z���邽�߂̌W���̊�{�����BTimestep�̓����Ă��鍀�́A���̃\�[�X(SetDesc())�ł́A�����ł��Ȃ����Ƃ���������(NULL���Ԃ��Ă���)
	CreateMatkLocal();
	
	//CreateMatKall();		//CreateMatkLocal();�Ɏ��������̂ŁA���������B
	CreateMatcLocal();
	CreateVecfLocal();
	int hogeshidebug =0;
}



void PHFemMeshThermo::CreateLocalMatrixAndSet(){
	//K,C,F�̍s������֐����Ăяo���āA��点��
	for(unsigned i = 0; i< tets.size() ; i++){
		//tets�������ɂ����ƁA���̍s��E�x�N�g��������Ă���āA�ł����s��A�x�N�g������ɌW����ݒ肵�����
		//��������΁A�e�v�f�����s���for�����񂳂Ȃ��Ă��悭�Ȃ�
		//CreateMatkLocal(tets);
		//CreateMatcLocal(tets);
		//CreateVecfLocal(tets);

		//tets�����č�点��
//		SetkcfParam(tets);

	}

}
void PHFemMeshThermo::SetkcfParam(Tet tets){
}

bool PHFemMeshThermo::GetDesc(void* p) const {
	PHFemMeshThermoDesc* d = (PHFemMeshThermoDesc*)p;
	return PHFemMesh::GetDesc(d);
}

void PHFemMeshThermo::CreateMatc(Tet tets){
	//�Ō�ɓ����s���������
	for(unsigned i =0; i < 4 ;i++){
		for(unsigned j =0; j < 4 ;j++){
			Matc[i][j] = 0.0;
		}
	}
	//Matc ��21�łł����s�������
	Matc = Create44Mat21();
	//	for debug
		//DSTR << "Matc " << Matc << " �� ";
	Matc = roh * specificHeat * CalcTetrahedraVolume(tets) / 20.0 * Matc;
	//	debug	//�W���̐ς��Ƃ�
		//DSTR << Matc << std::endl;
		//int hogemat =0 ;
}

void PHFemMeshThermo::CreateMatcLocal(){
	//	�g�p����s��̏�����
	//DMatCAll�F�Ίp�s��̐����̓������s��̃T�C�Y���`:�z��Ƃ��ė��p	��:vertices.size(),����:1
	DMatCAll.resize(1,vertices.size()); //(h,w)
	//�l�̏�����
	for(unsigned i=0;i<vertices.size();i++){
		DMatCAll[0][i] = 0.0;
	}

	//Matc�̏������́AMatc�����֐��ł���Ă���̂ŁA�ȗ�
	//���ׂĂ̗v�f�ɂ��ČW���s������
	for(unsigned i = 0; i< tets.size() ; i++){
		//c
		CreateMatc(tets[i]);
		int mathoge=0;
		//	(�K�E�X�U�C�f�����g�����v�Z��)�v�f���ɍ�����s��̐������A�G�b�W�ɌW�����i�[����
		//	or	(�K�E�X�U�C�f�����g��Ȃ��v�Z��)�v�f���Ƃ̌v�Z���I��邽�тɁA�v�f�����s��̐����������G�b�W��_�ɍ��ϐ��Ɋi�[���Ă���	#ifedef�Ń��[�h����āA�ǂ�����ł���悤�ɂ��Ă����Ă��ǂ�����w
		for(unsigned j=1; j < 4; j++){
			int vtxid0 = tets[i].vertices[j];
			//	���O�p�s�񕔕��ɂ��Ă̂ݎ��s
			//	j==1:k=0, j==2:k=0,1, j==3:k=0,1,2
			for(unsigned k = 0; k < j; k++){
				int vtxid1 = tets[i].vertices[k];
					for(unsigned l =0; l < vertices[vtxid0].edges.size(); l++){
						for(unsigned m =0; m < vertices[vtxid1].edges.size(); m++){
							if(vertices[vtxid0].edges[l] == vertices[vtxid1].edges[m]){
								edges[vertices[vtxid0].edges[l]].c += Matc[j][k];		//�������̂������͂������甼���ɂ���B��O�p�����O�p������������ɂ́A�ǂ�����for�����ɂ���Ηǂ��̂��H
								//DSTR << edges[vertices[vtxid0].edges[l]].k << std::endl;
							}
						}
					}
			}
		}
		//�Ίp������Ίp�����̑S�̍����s�񂩂甲���o����1�~n�̍s��ɑ������
		//j=0~4�܂ő��(��̃��[�v�ł́Aj�͑Ίp�����͈̔͂����Ȃ��̂ŁA�l������Ȃ��������o�Ă��܂�)
		for(unsigned j =0;j<4;j++){
			DMatCAll[0][tets[i].vertices[j]] += Matc[j][j];
		}
	}

	//	for debug
	//DSTR << "DMatCAll : " << std::endl;
	//for(unsigned j =0;j < vertices.size();j++){
	//	DSTR << j << "th : " << DMatCAll[0][j] << std::endl;
	//}
	// �l�M�ɂ��Ĕ�0�����ɂȂ����B

	//	���ׂ�
	//DMatKAll�̐����̂����A0�ƂȂ�v�f����������A�G���[�\��������R�[�h������
	// try catch���ɂ���
	for(unsigned j = 0; j < vertices.size() ; j++){
		if(DMatCAll[0][j] ==0.0){
			DSTR << "DMatCAll[0][" << j << "] element is blank" << std::endl;
		}
	}
	int piyodebug =0;
}


void PHFemMeshThermo::CreateVecfLocal(){
	//Vecf�̏�����
	for(unsigned i =0; i < 4 ; i++){
			Vecf[i][0] = 0.0;
	}
	//�S�̍����x�N�g��F�̃T�C�Y���K��
	VecFAll.resize(vertices.size(),1);

	//���ׂĂ̗v�f�ɂ��ČW���s������
	for(unsigned i = 0; i< tets.size() ; i++){
		//f1�����
		//f2�����
		//f3�����
		CreateVecf3(tets[i]);
		//f4�����
		int hogehoge=0;
		//f1,f2,f3,f4�����Z����
		Vecf = Vecf3;	
		//	for debug
		//DSTR << "Vecf : " << std::endl;
		//DSTR << Vecf << std::endl;
		//DSTR << "Vecf3 : " << std::endl;
		//DSTR << Vecf3 << std::endl;
		//	(�K�E�X�U�C�f�����g�����v�Z��)�v�f���ɍ�����s��̐������A�G�b�W�ɌW�����i�[����
		//	or	(�K�E�X�U�C�f�����g��Ȃ��v�Z��)�v�f���Ƃ̌v�Z���I��邽�тɁA�v�f�����s��̐����������G�b�W��_�ɍ��ϐ��Ɋi�[���Ă���	#ifedef�Ń��[�h����āA�ǂ�����ł���悤�ɂ��Ă����Ă��ǂ�����w

		//�v�f�̐ߓ_�ԍ��̏ꏊ�ɁA���̐ߓ_��f�̒l������
		//j:�v�f�̒��̉��Ԗڂ�
		for(unsigned j =0;j < 4; j++){
			int vtxid0 = tets[i].vertices[j];
			VecFAll[vtxid0][0] = Vecf[j][0];
		}
	}
	//	for debug
	//VecFAll�ɒl���������̂��ǂ����𒲂ׂ� 2011.09.21�S���ɒl�������Ă��邱�Ƃ��m�F����
	//DSTR << "VecFAll : " << std::endl;
	//for(unsigned j =0; j < vertices.size() ; j++){
	//	DSTR << j << " ele is :  " << VecFAll[j][0] << std::endl;
	//}
	//int hogeshi =0;

	////	���ׂ�
	////DVecFAll�̐����̂����A0�ƂȂ�v�f����������A�G���[�\��������R�[�h������
	//// try catch���ɂ���
	//for(unsigned j = 0; j < vertices.size() ; j++){
	//	if(VecFAll[j][0] ==0.0){
	//		DSTR << "VecFAll[" << j << "][0] element is blank" << std::endl;
	//	}
	//}


	//	����
	//	f3���g�p����ꍇ:���͗��̉��xTc��0�̐ߓ_�̗v�f��0�ɂȂ邽�߁A���x�̐ݒ肪�K�v
	//int hogef =0;
}

void PHFemMeshThermo::CreateMatkLocal(){
	//Matk�̏�����
	for(unsigned i =0; i < 4 ; i++){
		for(unsigned j =0; j < 4 ; j++){
			Matk[i][j] = 0.0;
		}
	}
	//DMatAll�F�Ίp�s��̐����̓������s��̃T�C�Y���`:�z��Ƃ��ė��p	��:vertices.size(),����:1
	DMatKAll.resize(1,vertices.size()); //(h,w)
		//�l�̏�����
	for(unsigned i=0;i<vertices.size();i++){
		DMatKAll[0][i] = 0.0;
	}

#ifdef UseMatAll
	//MatKAll�̏�����
	MatKAll.resize(vertices.size(),vertices.size());
	for(unsigned i=0;i<vertices.size();i++){
		for(unsigned j=0;j<vertices.size();j++){
			MatKAll[i][j] = 0.0;
		}
	}
#endif


	//���ׂĂ̗v�f�ɂ��ČW���s������
	for(unsigned i = 0; i< tets.size() ; i++){
		//	k1�����	�̐ς̋��ϊ֐�
		CreateMatk1k(tets[i]);
//		CreateMatk1b(tets[i]);
		//k2�����
		CreateMatk2(tets[i]);
		int hogehogehoge=0;
		//k1,k2,k3�����Z����(�g���Ă��鐔�l����)
		Matk = Matk1 + Matk2;	
		//	for debug
		//DSTR << "Matk1 : " << std::endl;
		//DSTR << Matk1 << std::endl;
		//DSTR << "Matk2 : " << std::endl;
		//DSTR << Matk2 << std::endl;
		//DSTR << "Matk : " << std::endl;
		//DSTR << Matk << std::endl;
		//int hogeshi =0;


		//	(�K�E�X�U�C�f�����g�����v�Z��)�v�f���ɍ�����s��̐������A�G�b�W�ɌW�����i�[����
		//	or	(�K�E�X�U�C�f�����g��Ȃ��v�Z��)�v�f���Ƃ̌v�Z���I��邽�тɁA�v�f�����s��̐����������G�b�W��_�ɍ��ϐ��Ɋi�[���Ă���	#ifedef�Ń��[�h����āA�ǂ�����ł���悤�ɂ��Ă����Ă��ǂ�����w
		//	Edges �̂��̗v�f�Ōv�Z����K�s��̐�����k�ɌW���Ƃ��Ċi�[����
		
		//Matk�̑Ίp�����ȊO�ŁA���O�p�̕����̒l���Aedge��k�ɑ������
		//
		//DSTR << i <<"th tetrahedra element'edges[vertices[vtxid0].edges[l]].k (All edges.k' value): " << std::endl;
		for(unsigned j=1; j < 4; j++){
			int vtxid0 = tets[i].vertices[j];
			//	���O�p�s�񕔕��ɂ��Ă̂ݎ��s
			//	j==1:k=0, j==2:k=0,1, j==3:k=0,1,2
			for(unsigned k = 0; k < j; k++){
				int vtxid1 = tets[i].vertices[k];
					for(unsigned l =0; l < vertices[vtxid0].edges.size(); l++){
						for(unsigned m =0; m < vertices[vtxid1].edges.size(); m++){
							if(vertices[vtxid0].edges[l] == vertices[vtxid1].edges[m]){
								edges[vertices[vtxid0].edges[l]].k += Matk[j][k];		//�������̂������͂������甼���ɂ���B��O�p�����O�p������������ɂ́A�ǂ�����for�����ɂ���Ηǂ��̂��H
								//DSTR << edges[vertices[vtxid0].edges[l]].k << std::endl;
							}
						}
					}
			}
		}

#ifdef UseMatAll
		//SciLab�Ŏg�����߂ɁA�S�̍����s������
		//Matk������
		for(unsigned j=0; j<4 ; j++){
			for(unsigned k=0; k<4 ;k++){
				MatKAll[tets[i].vertices[j]][tets[i].vertices[k]] += Matk[j][k];
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
		//						edges[vertices[vtxid0].edges[l]].k += Matk[j][k];		//�������̂������͂������甼���ɂ���B��O�p�����O�p������������ɂ́A�ǂ�����for�����ɂ���Ηǂ��̂��H
		//						//DSTR << edges[vertices[vtxid0].edges[l]].k << std::endl;
		//					}
		//				}
		//			}
		//	}
		//}

#endif

		//�Ίp������Ίp�����̑S�̍����s�񂩂甲���o����1�~n�̍s��ɑ������
		//j=0~4�܂ő��(��̃��[�v�ł́Aj�͑Ίp�����͈̔͂����Ȃ��̂ŁA�l������Ȃ��������o�Ă��܂�)
		for(unsigned j =0;j<4;j++){
			DMatKAll[0][tets[i].vertices[j]] += Matk[j][j];
			DSTR << "Matk[" << j << "][" << j << "] : " << Matk[j][j] << std::endl;
			DSTR << "DMatKAll[0][" << tets[i].vertices[j] << "] : " << DMatKAll[0][tets[i].vertices[j]] << std::endl;
			int hoge4 =0;
		}
		DSTR << std::endl;	//���s
	}//	�l�ʑ̂�for���̍Ō�
	
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
	//DMatKAll�̐����̂����A0�ƂȂ�v�f����������A�G���[�\��������R�[�h������
	// try catch���ɂ���
	for(unsigned j = 0; j < vertices.size() ; j++){
		if(DMatKAll[0][j] ==0.0){
			DSTR << "DMatKAll[0][" << j << "] element is blank" << std::endl;
		}
	}

	DSTR << "MatKAll : " << MatKAll <<std::endl;
	DSTR << "DMatKAll : " <<DMatKAll << std::endl;
	for(unsigned j =0;j<vertices.size();j++){
		if(MatKAll[j][j] != DMatKAll[0][j]){
			DSTR << j <<" �����̗v�f�͂��������I�������K�v�ł���B " <<std::endl;
		}
	}
	int hoge5 =0;

}

void PHFemMeshThermo::CreateMatk1b(Tet tets){
	//yagawa1983����Ƀm�[�g�Ɏ��W�J�����v�Z��
	unsigned i=0;
	unsigned j=0;
	unsigned k=0;
	unsigned l=0;
	//double a[4];
	DSTR << i <<std::endl;
	DSTR << j <<std::endl;
	DSTR << k <<std::endl;
	DSTR << l <<std::endl;
	//double a[i]=0.0;


}

void PHFemMeshThermo::CreateMatk1k(Tet tets){
	//���̌v�Z���Ăяo���Ƃ��ɁA�e�l�ʑ̂��ƂɌv�Z���邽�߁A�l�ʑ̂�0�Ԃ��珇�ɂ��̌v�Z���s��
	//�l�ʑ̂��\������4�ߓ_��ߓ_�̔z��(Tets�ɂ́A�ߓ_�̔z�񂪍���Ă���)�ɓ����Ă��鏇�Ԃ��g���āA�ʂ̌v�Z���s������A�s��̌v�Z���s�����肷��B
	//���̂��߁A���̊֐��̈����ɁA�l�ʑ̗v�f�̔ԍ������

	//�Ō�ɓ����s���������
	for(unsigned i =0; i < 4 ;i++){
		for(unsigned j =0; j < 4 ;j++){
			Matk1[i][j] = 0.0;
		}
	}

	//	A�s��@=	a11 a12 a13
	//				a21 a22 a23
	//				a31 a32 a33
	//�𐶐�
	PTM::TMatrixRow<4,4,double> Matk1A;
	FemVertex p[4];
	for(unsigned i = 0; i< 4 ; i++){
		p[i]= vertices[tets.vertices[i]];
	}
	
	Matk1A[0][0] = (p[2].pos.y - p[0].pos.y) * (p[3].pos.z - p[0].pos.z) - (p[2].pos.z - p[0].pos.z) * (p[3].pos.y - p[0].pos.y);
	Matk1A[0][1] = (p[1].pos.z - p[0].pos.z) * (p[3].pos.y - p[0].pos.y) - (p[1].pos.y - p[0].pos.y) * (p[3].pos.z - p[0].pos.z);
	Matk1A[0][2] = (p[1].pos.y - p[0].pos.y) * (p[2].pos.z - p[0].pos.z) - (p[1].pos.z - p[0].pos.z) * (p[2].pos.y - p[0].pos.y);

	Matk1A[1][0] = (p[2].pos.z - p[0].pos.z) * (p[3].pos.x - p[0].pos.x) - (p[2].pos.x - p[0].pos.x) * (p[3].pos.z - p[0].pos.z);
	Matk1A[1][1] = (p[1].pos.x - p[0].pos.x) * (p[3].pos.z - p[0].pos.z) - (p[1].pos.z - p[0].pos.z) * (p[3].pos.x - p[0].pos.x);
	Matk1A[1][2] = (p[1].pos.z - p[0].pos.z) * (p[2].pos.x - p[0].pos.x) - (p[1].pos.x - p[0].pos.x) * (p[2].pos.z - p[0].pos.z);

	Matk1A[2][0] = (p[2].pos.x - p[0].pos.x) * (p[3].pos.y - p[0].pos.y) - (p[2].pos.y - p[0].pos.y) * (p[3].pos.x - p[0].pos.x);
	Matk1A[2][1] = (p[1].pos.y - p[0].pos.y) * (p[3].pos.x - p[0].pos.x) - (p[1].pos.x - p[0].pos.x) * (p[3].pos.y - p[0].pos.y);
	Matk1A[2][2] = (p[1].pos.x - p[0].pos.x) * (p[2].pos.y - p[0].pos.y) - (p[1].pos.y - p[0].pos.y) * (p[2].pos.x - p[0].pos.x);

	//	for	debug
	//DSTR << Matk1A[0][0] << std::endl;
	//DSTR << "Matk1A : " << Matk1A << std::endl; 
	//int hogeshi =0;

	//a11 ~ a33 ���s��ɓ���āA[N~T] [N] ���v�Z������
	
	PTM::TMatrixRow<1,4,double> Nx;
	PTM::TMatrixRow<1,4,double> Ny;
	PTM::TMatrixRow<1,4,double> Nz;

	Nx[0][0] = -Matk1A[0][0] - Matk1A[0][1] -Matk1A[0][2];
	Nx[0][1] = Matk1A[0][0];
	Nx[0][2] = Matk1A[0][1];
	Nx[0][3] = Matk1A[0][2];

	Ny[0][0] = -Matk1A[1][0] - Matk1A[1][1] -Matk1A[1][2];
	Ny[0][1] = Matk1A[1][0];
	Ny[0][2] = Matk1A[1][1];
	Ny[0][3] = Matk1A[1][2];

	Nz[0][0] = -Matk1A[2][0] - Matk1A[2][1] -Matk1A[2][2];
	Nz[0][1] = Matk1A[2][0];
	Nz[0][2] = Matk1A[2][1];
	Nz[0][3] = Matk1A[2][2];

	//	Km �̎Z�o
	Matk1 = Nx.trans() * Nx + Ny.trans() * Ny + Nz.trans() * Nz;
	
	////	for debug
	//DSTR << "Nx : " << Nx << std::endl;
	//DSTR << "Nx^T : " << Nx.trans() << std::endl;
	//DSTR << "Nx^T * Nx : " << Nx.trans() * Nx << std::endl;
	//DSTR << "Ny^T * Ny : " << Ny.trans() * Ny << std::endl;
	//DSTR << "Nz^T * Nz : " << Nz.trans() * Nz << std::endl;
	//DSTR << "Matk1 : " << Matk1 << std::endl;
	//int hogehoge =0;

	//K1
	Matk1 = thConduct / (36 * CalcTetrahedraVolume(tets) ) * Matk1;
	//DSTR << "Matk1 : " << Matk1 << std::endl;
	//int hogedebug =0;
}

void PHFemMeshThermo::CreateVecf3(Tet tets){
	//	������
	for(unsigned i =0; i < 4 ;i++){
		Vecf3[i][0] = 0.0;		//�Ō�ɓ����s���������
		vertices[tets.vertices[i]].Tc =0.0;	//
	}	
	//l=0�̎�f31,1:f32, 2:f33, 3:f34	�𐶐�
	for(unsigned l= 0 ; l < 4; l++){
		//Matk2array[l] = Matk2temp;
		Vecf3array[l] = Create41Vec1();
		//	l�s��0��
		//for(int i=0;i<4;i++){
		//	Vecf3array[l][l][i] = 0.0;
		//}
		Vecf3array[l][l][0] = 0.0;
		//array[n][m][l]	= narray[n],m�sl��
		//	f_3	(Vecf3array[0], Vecf3array[1],..)
		// =	| 0 | + | 1 |+...
		//		| 1 |   | 0 |
		//		| 1 |   | 1 |
		//		| 1 |   | 1 |
		//	for debug
		//DSTR << "Vecf3array[" << l << "] : " << std::endl;
		//DSTR << Vecf3array[l] << std::endl;

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
			Vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets.vertices[1],tets.vertices[2],tets.vertices[3] ) * Vecf3array[l];
			//DSTR << "Vecf3array[" << l << "] : " << Vecf3array[l] << std::endl;
			//Vec�̐ߓ_���ɂ��̐ߓ_�ł̎��͗��̉��xTc�Ƃ̐ς��s��
			for(unsigned m=0; m<4; m++){
				Vecf3array[l][m][0] = vertices[tets.vertices[m]].Tc * Vecf3array[l][m][0];
			}
		}
		//	k22
		else if(l==1){
			Vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets.vertices[0],tets.vertices[2],tets.vertices[3] ) * Vecf3array[l];
			//DSTR << "Vecf3array[" << l << "] : " << Vecf3array[l] << std::endl;
			//Vec�̐ߓ_���ɂ��̐ߓ_�ł̎��͗��̉��xTc�Ƃ̐ς��s��
			for(unsigned m=0; m<4; m++){
				Vecf3array[l][m][0] = vertices[tets.vertices[m]].Tc * Vecf3array[l][m][0];
			}
		}
		//	k23
		else if(l==2){
			Vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets.vertices[0],tets.vertices[1],tets.vertices[3] ) * Vecf3array[l];
			//DSTR << "Vecf3array[" << l << "] : " << Vecf3array[l] << std::endl;
			//Vec�̐ߓ_���ɂ��̐ߓ_�ł̎��͗��̉��xTc�Ƃ̐ς��s��
			for(unsigned m=0; m<4; m++){
				Vecf3array[l][m][0] = vertices[tets.vertices[m]].Tc * Vecf3array[l][m][0];
			}
		}
		//	k24
		else if(l==3){
			Vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets.vertices[0],tets.vertices[1],tets.vertices[2] ) * Vecf3array[l];
			//DSTR << "Vecf3array[" << l << "] : " << Vecf3array[l] << std::endl;
			//Vec�̐ߓ_���ɂ��̐ߓ_�ł̎��͗��̉��xTc�Ƃ̐ς��s��
			for(unsigned m=0; m<4; m++){
				Vecf3array[l][m][0] = vertices[tets.vertices[m]].Tc * Vecf3array[l][m][0];
			}
		}
		//for debug
		//DSTR << "Vecf3array[" << l << "]�̊����ł́�" << std::endl;
		//DSTR << Vecf3array[l] << std::endl;
		if(DMatCAll == NULL){
			//DSTR <<"i : "<< i << ", l : " << l << std::endl;
			DSTR <<"l : " << l << std::endl;
		}
	}

	//f3 = f31 + f32 + f33 + f34
	for(unsigned i=0; i < 4; i++){
		Vecf3 += Vecf3array[i];
		//	for debug
		//DSTR << "Vecf3 �� Vecf3array = f3" << i+1 <<"�܂ŉ��Z�����s��" << std::endl;
		//DSTR << Vecf3 << std::endl;
	}
	
	//	f1,f2,f3,f4	���v�Z����ۂɁA[0][0]��������[3][0]�����܂ł̔�0�����ɂ��āA���Tc�������Ă��܂�


	//for debug
	//DSTR << "�ߓ_�i";
	//for(unsigned i =0; i < 4; i++){
	//	DSTR << tets.vertices[i] << "," ;
	//}
	//DSTR << ")�ō\�������l�ʑ̂�" << std::endl;
	//DSTR << "Vecf3 : " << std::endl;
	//DSTR << Vecf3 << std::endl;
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
	double area=0.0;

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

void PHFemMeshThermo::CreateMatk2(Tet tets){
	//���̌v�Z���Ăяo���Ƃ��ɁA�e�l�ʑ̂��ƂɌv�Z���邽�߁A�l�ʑ̂�0�Ԃ��珇�ɂ��̌v�Z���s��
	//�l�ʑ̂��\������4�ߓ_��ߓ_�̔z��(Tets�ɂ́A�ߓ_�̔z�񂪍���Ă���)�ɓ����Ă��鏇�Ԃ��g���āA�ʂ̌v�Z���s������A�s��̌v�Z���s�����肷��B
	//���̂��߁A���̊֐��̈����ɁA�l�ʑ̗v�f�̔ԍ������

	//�Ō�ɓ����s���������
	for(unsigned i =0; i < 4 ;i++){
		for(unsigned j =0; j < 4 ;j++){
			Matk2[i][j] = 0.0;
		}
	}

	//2)	
	//l=0�̎�k21,1:k22, 2:k23, 3:k24	�𐶐�
	for(unsigned l= 0 ; l < 4; l++){
		//Matk2array[l] = Matk2temp;
		Matk2array[l] = Create44Mat21();
		//	1�si���0��
		for(int i=0;i<4;i++){
			Matk2array[l][l][i] = 0.0;
		}
		//	i�s1���0��
		for(int i=0;i<4;i++){
			Matk2array[l][i][l] = 0.0;
		}
		//	k_21	
		// =	|0 0 0 0 |
		//		|0 2 1 1 |
		//		|0 1 2 1 |
		//		|0 1 1 2 |
		//	for debug
		//DSTR <<"Matk2array[" << l << "] : " << std::endl;
		//DSTR << Matk2array[l] << std::endl;

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
		if(l==0){
			Matk2array[l] = heatTrans * (1.0/12.0) * CalcTriangleArea( tets.vertices[1],tets.vertices[2],tets.vertices[3] ) * Matk2array[l];
		}
		//	k22
		else if(l==1){
			Matk2array[l] = heatTrans * (1.0/12.0) * CalcTriangleArea( tets.vertices[0],tets.vertices[2],tets.vertices[3] ) * Matk2array[l];
		}
		//	k23
		else if(l==2){
			Matk2array[l] = heatTrans * (1.0/12.0) * CalcTriangleArea( tets.vertices[0],tets.vertices[1],tets.vertices[3] ) * Matk2array[l];
		}
		//	k24
		else if(l==3){
			Matk2array[l] = heatTrans * (1.0/12.0) * CalcTriangleArea( tets.vertices[0],tets.vertices[1],tets.vertices[2] ) * Matk2array[l];
		}
		//for debug
		//DSTR << "Matk2array[" << l << "]�̊����ł́�" << std::endl;
		//DSTR << Matk2array[l] << std::endl;
	}

	//k2 = k21 + k22 + k23 + k24
	for(unsigned i=0; i < 4; i++){
		Matk2 += Matk2array[i];
		//	for debug
		//DSTR << "Matk2 �� Matk2array = k2" << i+1 <<"�܂ŉ��Z�����s��" << std::endl;
		//DSTR << Matk2 << std::endl;
	}
	
	//for debug
	//DSTR << "�ߓ_�i";
	//for(unsigned i =0; i < 4; i++){
	//	DSTR << tets.vertices[i] << "," ;
	//}
	//DSTR << ")�ō\�������l�ʑ̂�" << std::endl;
	//DSTR << "Matk2 : " << std::endl;
	//DSTR << Matk2 << std::endl;
	//int hogeshishi =0;
}

void PHFemMeshThermo::SetInitThermoConductionParam(double thConduct0,double roh0,double specificHeat0,double heatTrans0){
	thConduct = thConduct0;
	roh = roh0;
	specificHeat = specificHeat0;
	heatTrans = heatTrans0;
}


void PHFemMeshThermo::SetTempAllToTVecAll(unsigned size){
	for(unsigned i =0; i < size;i++){
		TVecAll[i][0] = vertices[i].temp;
	}
}


void PHFemMeshThermo::CreateTempMatrix(){
	unsigned int dmnN = vertices.size();
	TVecAll.resize(dmnN,1);
	SetTempAllToTVecAll(dmnN);
	//for(std::vector<unsigned int>::size_type i=0; i < dmnN ; i++){
	//	TVecAll[i][0] = vertices[i].temp;
	//}

	//for Debug
	//for(unsigned int i =0; i < dmnN; i++){
	//	DSTR << i <<" : " << TVecAll[i][0] << std::endl;
	//}
	//for debug
	//for(std::vector<unsigned int>::size_type i =0; i < vertices.size(); i++){
	//	DSTR << i << " : " << &vertices[i] << std::endl;
	//}

}


void PHFemMeshThermo::SetTempToTVecAll(unsigned vtxid){
	TVecAll[vtxid][0] = vertices[vtxid].temp;
}

void PHFemMeshThermo::SetLocalFluidTemp(unsigned i,double temp){
	vertices[i].Tc = temp;			//�ߓ_�̎��͗��̉��x�̐ݒ�
}

void PHFemMeshThermo::SetVerticesTemp(unsigned i,double temp){
	vertices[i].temp = temp;
	SetTempToTVecAll(i);
}

void PHFemMeshThermo::SetVerticesTemp(double temp){
	for(std::vector<unsigned int>::size_type i=0; i < vertices.size() ; i++){
			vertices[i].temp = temp;
		}
}


}


