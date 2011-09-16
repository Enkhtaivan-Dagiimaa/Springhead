/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include <Physics/PHFemMeshThermo.h>
#include<Base/Affine.h>


using namespace PTM;

namespace Spr{;


PHFemMeshThermoDesc::PHFemMeshThermoDesc(){
	Init();
}
void PHFemMeshThermoDesc::Init(){
	//�f�B�X�N���v�^�ɓ����l�̏�����?
	

}

///////////////////////////////////////////////////////////////////
//	PHFemMeshThermo


PHFemMeshThermo::PHFemMeshThermo(const PHFemMeshThermoDesc& desc, SceneIf* s){
	SetDesc(&desc);
	if (s){ SetScene(s); }
}
void PHFemMeshThermo::Step(double dt){
	//	
}
void PHFemMeshThermo::CreateMatrix(){
}


void PHFemMeshThermo::SetDesc(const void* p) {
	PHFemMeshThermoDesc* d = (PHFemMeshThermoDesc*)p;
	PHFemMesh::SetDesc(d);
	
	//���ԍ��ݕ�	dt�̐ݒ�
	PHFemMeshThermo::dt = 0.01;

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
	CreateMatkLocal();
	
	//CreateMatKall();

	//CreateC();
	//CreateF();
	//PrepareGaussSeidel();
		//�K�E�X�U�C�f���Ōv�Z���邽�߂ɁA�N�����N�j�R���\���̍������̌`�ōs������B�s��D��F�A-b�Ȃǂ����A�K�E�X�U�C�f���Ōv�Z�X�e�b�v�����s���O�܂�
	//�K�E�X�U�C�f���̌v�Z��P�ʃX�e�b�v���Ԃ��Ƃɍs��
		//�K�E�X�U�C�f���̌v�Z
		//CalcGaussSeidel();
	//�i�`�󂪕ς������A�}�g���N�X��x�N�g������蒼���j
	//���x�ω��E�ŐV�̎��Ԃ�{T}�c�x�N�g���ɋL�ڂ���Ă���ߓ_���x����ɉ��w�ω��V�~�����[�V�������s��
		//SetChemicalSimulation();
		//���w�ω��V�~�����[�V�����ɕK�v�ȉ��x�Ȃǂ̃p�����[�^��n��
	//���x�ω��≻�w�V�~�����[�V�����̌��ʂ̓O���t�B�N�X�\�����s��
	

}


bool PHFemMeshThermo::GetDesc(void* p) const {
	PHFemMeshThermoDesc* d = (PHFemMeshThermoDesc*)p;
	return PHFemMesh::GetDesc(d);
}

void PHFemMeshThermo::CreateMatkLocal(){
	//k1�����
	//CreateMatk1();

	//k2�����
	for(unsigned i = 0; i< tets.size() ; i++){
		CreateMatk2(tets[i]);
		int hogehogehoge=0;
	}
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
	//1)	�ꎞ�g�p�s��temp�ɂ��ׂē���Ă��܂�
	//|2 1 1 1 |
	//|1 2 1 1 |
	//|1 1 2 1 |
	//|1 1 1 2 |	�����
	PTM::TMatrixRow<4,4,double> Matk2temp;
	for(int i =0; i <4 ; i++){
		for(int j=0; j < 4 ; j++){
			if(i==j){
				Matk2temp[i][j] = 2.0;
			}else{
				Matk2temp[i][j] = 1.0;
			}
		}
	}
	//for debug
	//DSTR << Matk2temp << std::endl;
	
	//2)	
	//l=0�̎�k21,1:k22, 2:k23, 3:k24	�𐶐�
	for(unsigned l= 0 ; l < 4; l++){
		Matk2array[l] = Matk2temp;
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
	
	DSTR << "�ߓ_�i";
	for(unsigned i =0; i < 4; i++){
		DSTR << tets.vertices[i] << "," ;
	}
	DSTR << ")�ō\�������l�ʑ̂�" << std::endl;
	DSTR << "Matk2 : " << std::endl;
	DSTR << Matk2 << std::endl;
	int hogeshishi =0;
}

void PHFemMeshThermo::SetInitThermoConductionParam(double thConduct0,double roh0,double specificHeat0,double heatTrans0){
	thConduct = thConduct0;
	roh = roh0;
	specificHeat = specificHeat0;
	heatTrans = heatTrans0;
}

void PHFemMeshThermo::CreateTempMatrix(){
	unsigned int dmnN = vertices.size();
	TVecAll.resize(dmnN,1);
	for(std::vector<unsigned int>::size_type i=0; i < dmnN ; i++){
		TVecAll[i][0] = vertices[i].temp;
	}
	//for Debug
	//for(unsigned int i =0; i < dmnN; i++){
	//	DSTR << i <<" : " << TVecAll[i][0] << std::endl;
	//}
	//for debug
	//for(std::vector<unsigned int>::size_type i =0; i < vertices.size(); i++){
	//	DSTR << i << " : " << &vertices[i] << std::endl;
	//}

}

void PHFemMeshThermo::SetVerticesTemp(double temp){
	for(std::vector<unsigned int>::size_type i=0; i < vertices.size() ; i++){
			vertices[i].temp = temp;
		}
}

void PHFemMeshThermo::CreateMatTest(){

	TMatrixRow<4,4,double> mat;              // 2�s2��̍s���l�����s���錾
				                               // TMatrixCol���Ɨ񂪋l�����s��ɂȂ�
	//�l�ʑ�vector�Ȃǂ���ċA���߂Ȃǂœ���
    mat[0][0] = 1;  mat[0][1] = 1;	mat[0][2] = 1;	mat[0][3] = 1;          // 0�s0�� = 1;  0�s1�� = 2;
    mat[1][0] = 3;  mat[1][1] = 4;	mat[1][2] = 2;	mat[1][3] = 2;          // 1�s0�� = 3;  1�s1�� = 4;
	mat[2][0] = 2;	mat[2][1] = 2;	mat[2][2] = 2;	mat[2][3] = 2;
	mat[3][0] = 2;	mat[3][1] = 2;	mat[3][2] = 2;	mat[3][3] = 2;

    TVector<2,float> vec;                   // 2�����̃x�N�g����錾
    vec[0] = 1; vec[1] = 0;
    std::cout << mat;
    std::cout << vec << std::endl;
    std::cout << mat * vec << std::endl;    // �|���Z
    std::cout << mat + mat << std::endl;    // �����Z
    std::cout << mat - mat << std::endl;    // �����Z
    std::cout << mat.trans() << std::endl;  // �]�u
    std::cout << mat.inv() << std::endl;    // �t�s��
    //return 0;

	double elemK2[4][4];
	double elemK3[4][4];
	//��=0 �O�s�ƎO���0�ɂ���B
	double alpha;	//�M�`�B�����ǂ����Œ�`
	int hogehoge;//�̃ŃĂ̂ǂ��0�ɂ��邩�˂ǂ������������肪�����̂��m���B
	for(int i=0; i<4; i++){
		for(int j=0; j<4 ;j++){
			if( i = j){
				elemK2[i][j] = 1/12;
			}
			elemK2[i][j] = 1/24;
			if(j=hogehoge){
				elemK2[i][j] =0.0;	//j���0��
			}
			if(i=hogehoge){
				elemK2[i][j] =0.0;		//i�s��0��
			}
		}
		
	}
}

}
