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
//#define DEBUG
//#define DumK



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
	StepCount =0;				// �X�e�b�v���J�E���^
	StepCount_ =0;				// �X�e�b�v���J�E���^
}

void PHFemMeshThermo::SetThermalBoundaryCondition(){
	
}

//void PHFemMeshThermo::PrepareStep(){
//	//�K�E�X�U�C�f���ɕK�v�ȁA�v�Z���̌W�����v�Z����
//	double dt = DCAST(PHSceneIf, GetScene())->GetTimeStep();
//	//�W��b��DMatAll_�Ȃǂ������ō��
//	//bVecAll�̃��T�C�Y
//	//bVecAll�Ɍv�Z���ʂ��i�[
//	//�������A[K],[C]�Ȃǂ͑S�̍����s�������Ă���̂ł͂Ȃ��A�������Ƃ�Edges�\���̂ɓ����Ă���̂ŁA���̒l��p����
//	//�W���s��b�������[�v�˂��̃��[�v���K�E�X�U�C�f���v�Z�̍ŏ��̈�񂾂�������ق����Afor���[�v��1�񏭂Ȃ��Ȃ�̂ŁA�v�Z�������B���ǁAif�����K�v
//}

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

void PHFemMeshThermo::UsingHeatTransferBoundaryCondition(unsigned id,double temp){
	//�M�`�B���E����
	//�ߓ_�̎��͗��̉��x�̐ݒ�(K,C,F�Ȃǂ̍s��x�N�g���̍쐬��Ɏ��s�K�v����)
//	if(vertices[id].Tc != temp){					//�X�V����ߓ_��Tc���ω������������ATc��F�x�N�g�����X�V����
		SetLocalFluidTemp(id,temp);
		//�M�`�B���E�������g����悤�ɁA����B
		CreateVecfLocal();		//	Tc���܂ރx�N�g�����X�V����
//	}

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
			if(deformed){												//D_ii�̍쐬�@�`�󂪍X�V���ꂽ�ۂ�1�x�����s���΂悢
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


void PHFemMeshThermo::Step(double dt){

	//std::ofstream templog("templog.txt");
	std::ofstream ofs_("log_.txt");
//	ScilabTest();									//	Scilab���g���e�X�g
	//���E������ݒ�:���x�̐ݒ�
//	UsingFixedTempBoundaryCondition(0,200.0);

	//	�����p�̃R�[�h
	//	a) ���ԗ��U�����ɑ�`�������p�A�O�i�E��ލ����ŐU��������M����
	if(StepCount == 0){
		unsigned texid_ =7;
		UsingFixedTempBoundaryCondition(texid_,200.0);
	}

	//	b) �f�M�ߒ��̎���
	//	�M�`�B����0�ɂ���B���x�Œ苫�E�����ŉ��M�B

	//	UsingFixedTempBoundaryCondition(3,50.0);
//	UsingFixedTempBoundaryCondition(4,150.0);
	
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

	//
	//dt = dt *0.01;		�덷1�x���x�ɂȂ�
	//dt = dt;				�����������́A�v�Z�덷�H�i�}�C�i�X�ɂȂ��Ă���ߓ_���x������Ȃ�ɑ傫���Ȃ�B�j
//	CalcHeatTransUsingGaussSeidel(20,dt);			//�K�E�X�U�C�f���@�ŔM�`���v�Z������
	
	CalcHeatTransUsingGaussSeidel(20,dt,0.5);			//�K�E�X�U�C�f���@�ŔM�`���v�Z������ ��O�����́A�O�i�E�N�����N�j�R���\���E��ސϕ��̂����ꂩ�𐔒l�őI��

	//���x��\�����Ă݂�
	//DSTR << "vertices[3].temp : " << vertices[3].temp << std::endl;

	//���x�̃x�N�g������ߓ_�։��x�̔��f
	UpdateVertexTempAll(vertices.size());

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
	//




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


void PHFemMeshThermo::SetDesc(const void* p) {
	PHFemMeshThermoDesc* d = (PHFemMeshThermoDesc*)p;
	PHFemMesh::SetDesc(d);
	
	////���ԍ��ݕ�	dt�̐ݒ�
	//PHFemMeshThermo::dt = 0.01;

	//�V�[������A�V�[���̎��ԃX�e�b�v���擾����ˈȉ��̃R�[�h�ł́AGetTimeStep���悤�Ƃ���ƁANULL���Ԃ��Ă���̂ŁAPHEngine�ōs��
	//DSTR << DCAST(PHSceneIf, GetScene()) << std::endl;
	//DSTR << DCAST(PHSceneIf, GetScene())->GetTimeStep() << std::endl;
	//int hogeshimitake =0;


	//%%%	��������		%%%//

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
		SetInitThermoConductionParam(0.574,970,0.196,25 * 0.0001);
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

	//	�ߓ_���x���ڂ̏����o��
	templog.open("templog.csv");


	
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
			matc[i][j] = 0.0;
		}
	}
	//matc ��21�łł����s�������
	matc = Create44Mat21();
	//	for debug
		//DSTR << "matc " << matc << " �� ";
	matc = roh * specificHeat * CalcTetrahedraVolume(tets) / 20.0 * matc;
	//	debug	//�W���̐ς��Ƃ�
		//DSTR << matc << std::endl;
		//int hogemat =0 ;
}

void PHFemMeshThermo::CreateMatcLocal(){
	//	�g�p����s��̏�����
	//dMatCAll�F�Ίp�s��̐����̓������s��̃T�C�Y���`:�z��Ƃ��ė��p	��:vertices.size(),����:1
	dMatCAll.resize(1,vertices.size()); //(h,w)
	//�l�̏�����
	for(unsigned i=0;i<vertices.size();i++){
		dMatCAll[0][i] = 0.0;
	}

	//matc�̏������́Amatc�����֐��ł���Ă���̂ŁA�ȗ�
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
			dMatCAll[0][tets[i].vertices[j]] += matc[j][j];
		}
	}

	//	for debug
	//DSTR << "dMatCAll : " << std::endl;
	//for(unsigned j =0;j < vertices.size();j++){
	//	DSTR << j << "th : " << dMatCAll[0][j] << std::endl;
	//}
	// �l�M�ɂ��Ĕ�0�����ɂȂ����B

	//	���ׂ�
	//dMatKAll�̐����̂����A0�ƂȂ�v�f����������A�G���[�\��������R�[�h������
	// try catch���ɂ���
	for(unsigned j = 0; j < vertices.size() ; j++){
		if(dMatCAll[0][j] ==0.0){
			DSTR << "dMatCAll[0][" << j << "] element is blank" << std::endl;
		}
	}
	int piyodebug =0;
}


void PHFemMeshThermo::CreateVecfLocal(){
	//Vecf�̏�����
	for(unsigned i =0; i < 4 ; i++){
			vecf[i] = 0.0;
	}
	//�S�̍����x�N�g��F�̃T�C�Y���K��
	vecFAll.resize(vertices.size(),1);

	//���ׂĂ̗v�f�ɂ��ČW���s������
	for(unsigned i = 0; i< tets.size() ; i++){
		//f1�����
		//f2�����
		//f3�����
		CreateVecf3(tets[i]);
		//f4�����
		int hogehoge=0;
		//f1,f2,f3,f4�����Z����
		vecf = vecf3;	
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
			int vtxid0 = tets[i].vertices[j];
			vecFAll[vtxid0][0] = vecf[j];
		}
	}
	//	for debug
	//vecFAll�ɒl���������̂��ǂ����𒲂ׂ� 2011.09.21�S���ɒl�������Ă��邱�Ƃ��m�F����
	//DSTR << "vecFAll : " << std::endl;
	//for(unsigned j =0; j < vertices.size() ; j++){
	//	DSTR << j << " ele is :  " << vecFAll[j][0] << std::endl;
	//}
	//int hogeshi =0;

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

void PHFemMeshThermo::CreateMatkLocal(){
	//matk�̏�����
	for(unsigned i =0; i < 4 ; i++){
		for(unsigned j =0; j < 4 ; j++){
			matk[i][j] = 0.0;
		}
	}
	//DMatAll�F�Ίp�s��̐����̓������s��̃T�C�Y���`:�z��Ƃ��ė��p	��:vertices.size(),����:1
	dMatKAll.resize(1,vertices.size()); //(h,w)
		//�l�̏�����
	for(unsigned i=0;i<vertices.size();i++){
		dMatKAll[0][i] = 0.0;
	}

#ifdef UseMatAll
	//matKAll�̏�����
	matKAll.resize(vertices.size(),vertices.size());
	for(unsigned i=0;i<vertices.size();i++){
		for(unsigned j=0;j<vertices.size();j++){
			matKAll[i][j] = 0.0;
		}
	}
#endif UseMatAll


	//���ׂĂ̗v�f�ɂ��ČW���s������
	for(unsigned i = 0; i< tets.size() ; i++){
		//	k1�����	k1k�ł��Ak1b�ł��ǂ���ł��\��Ȃ�
		CreateMatk1k(tets[i]);			//	k���_�������ɁA���M���āA�`��֐��𓱏o
		//DSTR << " matk1k : " <<std::endl;
		//DSTR << matk1 <<std::endl;

//		CreateMatk1b(tets[i]);			//	���Ђ̗��_�������ɁA������p���Č`��֐��𓱏o
		//DSTR << " matk1b : " <<std::endl;
		//DSTR << matk1 <<std::endl;

		//k2�����

		CreateMatk2(tets[i]);
		int hogehogehoge=0;
		//k1,k2,k3�����Z����(�g���Ă��鐔�l����)
		matk = matk1 + matk2;	
		//	for debug
		//DSTR << "matk1 : " << std::endl;
		//DSTR << matk1 << std::endl;
		//DSTR << "matk2 : " << std::endl;
		//DSTR << matk2 << std::endl;
		//DSTR << "matk : " << std::endl;
		//DSTR << matk << std::endl;
		//int hogeshi =0;


		//	(�K�E�X�U�C�f�����g�����v�Z��)�v�f���ɍ�����s��̐������A�G�b�W�ɌW�����i�[����
		//	or	(�K�E�X�U�C�f�����g��Ȃ��v�Z��)�v�f���Ƃ̌v�Z���I��邽�тɁA�v�f�����s��̐����������G�b�W��_�ɍ��ϐ��Ɋi�[���Ă���	#ifedef�Ń��[�h����āA�ǂ�����ł���悤�ɂ��Ă����Ă��ǂ�����w
		//	Edges �̂��̗v�f�Ōv�Z����K�s��̐�����k�ɌW���Ƃ��Ċi�[����
		
		//matk�̑Ίp�����ȊO�ŁA���O�p�̕����̒l���Aedge��k�ɑ������
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
				matKAll[tets[i].vertices[j]][tets[i].vertices[k]] += matk[j][k];
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
		//matKAll�̏�����
		matKAll.resize(vertices.size(),vertices.size());
		for(unsigned i=0;i<vertices.size();i++){
			for(unsigned j=0;j<vertices.size();j++){
				matKAll[i][j] = 0.0;
			}
		}
		//SciLab�Ŏg�����߂ɁA�S�̍����s������
		//matk������
		for(unsigned j=0; j<4 ; j++){
			for(unsigned k=0; k<4 ;k++){
				matKAll[tets[i].vertices[j]][tets[i].vertices[k]] = 0.0;;
			}
		}
#endif

		//�Ίp������Ίp�����̑S�̍����s�񂩂甲���o����1�~n�̍s��ɑ������
		//j=0~4�܂ő��(��̃��[�v�ł́Aj�͑Ίp�����͈̔͂����Ȃ��̂ŁA�l������Ȃ��������o�Ă��܂�)
		for(unsigned j =0;j<4;j++){
			dMatKAll[0][tets[i].vertices[j]] += matk[j][j];
			DSTR << "matk[" << j << "][" << j << "] : " << matk[j][j] << std::endl;
			DSTR << "dMatKAll[0][" << tets[i].vertices[j] << "] : " << dMatKAll[0][tets[i].vertices[j]] << std::endl;
			int hoge4 =0;
		}
		DSTR << std::endl;	//���s

		//std::ofstream matKAll("matKAll.csv");
		//for(){
		//	matKAll
		//	}


#ifdef DumK
		for(unsigned j=0;j<4;j++){
			dMatKAll[0][tets[i].vertices[j]] = 0.0;
			int hogeshi =0;
		} 
#endif DumK

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
	//dMatKAll�̐����̂����A0�ƂȂ�v�f����������A�G���[�\��������R�[�h������
	// try catch���ɂ���
	for(unsigned j = 0; j < vertices.size() ; j++){
		if(dMatKAll[0][j] ==0.0){
			DSTR << "dMatKAll[0][" << j << "] element is blank" << std::endl;
		}
	}

	DSTR << "matKAll : " << matKAll <<std::endl;
	DSTR << "dMatKAll : " <<dMatKAll << std::endl;
#ifdef UseMatAll
	for(unsigned j =0;j<vertices.size();j++){
		if(matKAll[j][j] != dMatKAll[0][j]){
			DSTR << j <<" �����̗v�f�͂��������I�������K�v�ł���B " <<std::endl;
		}
	}
#endif UseMatAll
	int hoge5 =0;

}

void PHFemMeshThermo::CreateMatk1b(Tet tets){
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
		x[m] = vertices[tets.vertices[m]].pos.x;
		y[m] = vertices[tets.vertices[m]].pos.y;
		z[m] = vertices[tets.vertices[m]].pos.z;
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
			matk1[i][j] = a[i] * a[j] +b[i] * b[j] + c[i] * c[j];
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
	matk1 = thConduct / (6 *  CalcTetrahedraVolume(tets)) * matk1;

	//	for DEBUG
	//DSTR << "�W���ό�� matk1 : " << std::endl;
	//DSTR << matk1 << std::endl;
	int debughogeshi3 =0;

	//a~c�̑�������K1�ɑ��
	//matk1(4x4)�ɑ��

}

void PHFemMeshThermo::CreateMatk1k(Tet tets){
	//���̌v�Z���Ăяo���Ƃ��ɁA�e�l�ʑ̂��ƂɌv�Z���邽�߁A�l�ʑ̂�0�Ԃ��珇�ɂ��̌v�Z���s��
	//�l�ʑ̂��\������4�ߓ_��ߓ_�̔z��(Tets�ɂ́A�ߓ_�̔z�񂪍���Ă���)�ɓ����Ă��鏇�Ԃ��g���āA�ʂ̌v�Z���s������A�s��̌v�Z���s�����肷��B
	//���̂��߁A���̊֐��̈����ɁA�l�ʑ̗v�f�̔ԍ������

	//�Ō�ɓ����s���������
	for(unsigned i =0; i < 4 ;i++){
		for(unsigned j =0; j < 4 ;j++){
			matk1[i][j] = 0.0;
		}
	}

	//	A�s��@=	a11 a12 a13
	//				a21 a22 a23
	//				a31 a32 a33
	//�𐶐�
	PTM::TMatrixRow<4,4,double> matk1A;
	FemVertex p[4];
	for(unsigned i = 0; i< 4 ; i++){
		p[i]= vertices[tets.vertices[i]];
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
	matk1 = Nx.trans() * Nx + Ny.trans() * Ny + Nz.trans() * Nz;
	
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
	matk1 = thConduct / (6 * CalcTetrahedraVolume(tets) ) * matk1;
	//DSTR << "matk1 : " << matk1 << std::endl;
	//int hogedebug =0;
}

void PHFemMeshThermo::CreateVecf3(Tet tets){
	//	������
	for(unsigned i =0; i < 4 ;i++){
		vecf3[i] = 0.0;		//�Ō�ɓ����s���������
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
			vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets.vertices[1],tets.vertices[2],tets.vertices[3] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vec�̐ߓ_���ɂ��̐ߓ_�ł̎��͗��̉��xTc�Ƃ̐ς��s��
			for(unsigned m=0; m<4; m++){
				vecf3array[l][m] = vertices[tets.vertices[m]].Tc * vecf3array[l][m];
			}
		}
		//	k22
		else if(l==1){
			vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets.vertices[0],tets.vertices[2],tets.vertices[3] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vec�̐ߓ_���ɂ��̐ߓ_�ł̎��͗��̉��xTc�Ƃ̐ς��s��
			for(unsigned m=0; m<4; m++){
				vecf3array[l][m] = vertices[tets.vertices[m]].Tc * vecf3array[l][m];
			}
		}
		//	k23
		else if(l==2){
			vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets.vertices[0],tets.vertices[1],tets.vertices[3] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vec�̐ߓ_���ɂ��̐ߓ_�ł̎��͗��̉��xTc�Ƃ̐ς��s��
			for(unsigned m=0; m<4; m++){
				vecf3array[l][m] = vertices[tets.vertices[m]].Tc * vecf3array[l][m];
			}
		}
		//	k24
		else if(l==3){
			vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets.vertices[0],tets.vertices[1],tets.vertices[2] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vec�̐ߓ_���ɂ��̐ߓ_�ł̎��͗��̉��xTc�Ƃ̐ς��s��
			for(unsigned m=0; m<4; m++){
				vecf3array[l][m] = vertices[tets.vertices[m]].Tc * vecf3array[l][m];
			}
		}
		//for debug
		//DSTR << "vecf3array[" << l << "]�̊����ł́�" << std::endl;
		//DSTR << vecf3array[l] << std::endl;
		if(dMatCAll == NULL){
			//DSTR <<"i : "<< i << ", l : " << l << std::endl;
			DSTR <<"l : " << l << std::endl;
		}
	}

	//f3 = f31 + f32 + f33 + f34
	for(unsigned i=0; i < 4; i++){
		vecf3 += vecf3array[i];
		//	for debug
		//DSTR << "vecf3 �� vecf3array = f3" << i+1 <<"�܂ŉ��Z�����s��" << std::endl;
		//DSTR << vecf3 << std::endl;
	}
	
	//	f1,f2,f3,f4	���v�Z����ۂɁA[0][0]��������[3][0]�����܂ł̔�0�����ɂ��āA���Tc�������Ă��܂�


	//for debug
	//DSTR << "�ߓ_�i";
	//for(unsigned i =0; i < 4; i++){
	//	DSTR << tets.vertices[i] << "," ;
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
			matk2[i][j] = 0.0;
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
		if(l==0){
			matk2array[l] = heatTrans * (1.0/12.0) * CalcTriangleArea( tets.vertices[1],tets.vertices[2],tets.vertices[3] ) * matk2array[l];
		}
		//	k22
		else if(l==1){
			matk2array[l] = heatTrans * (1.0/12.0) * CalcTriangleArea( tets.vertices[0],tets.vertices[2],tets.vertices[3] ) * matk2array[l];
		}
		//	k23
		else if(l==2){
			matk2array[l] = heatTrans * (1.0/12.0) * CalcTriangleArea( tets.vertices[0],tets.vertices[1],tets.vertices[3] ) * matk2array[l];
		}
		//	k24
		else if(l==3){
			matk2array[l] = heatTrans * (1.0/12.0) * CalcTriangleArea( tets.vertices[0],tets.vertices[1],tets.vertices[2] ) * matk2array[l];
		}
		//for debug
		//DSTR << "matk2array[" << l << "]�̊����ł́�" << std::endl;
		//DSTR << matk2array[l] << std::endl;
	}

	//k2 = k21 + k22 + k23 + k24
	for(unsigned i=0; i < 4; i++){
		matk2 += matk2array[i];
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
	roh = roh0;
	specificHeat = specificHeat0;
	heatTrans = heatTrans0;
}


void PHFemMeshThermo::SetTempAllToTVecAll(unsigned size){
	for(unsigned i =0; i < size;i++){
		TVecAll[i] = vertices[i].temp;
	}
}


void PHFemMeshThermo::CreateTempMatrix(){
	unsigned int dmnN = vertices.size();
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


