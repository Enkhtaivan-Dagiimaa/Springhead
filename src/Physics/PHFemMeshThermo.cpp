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
	
	//�s������
		//���ߓ_��nVtxsOut�A���v�f��nTetsOut�A�ߓ_���W(Vec3d* vtxsOut)�A�v�f�̍\��(tetsOut?)�Ȃǂ�o�^
			//PHFemMeshThermo�̃I�u�W�F�N�g�����ۂɁA�f�B�X�N���v�^�ɒl��ݒ肵�č��
		
		//�ߓ_�̉��x�Ȃǂ̏����ݒ�
			
		//�ߓ_�̏������x��ݒ肷���{T}�c�x�N�g���ɑ��
			//{T}�c�x�N�g�����쐬����B�ȍ~��K,C,F�s��E�x�N�g���̐ߓ_�ԍ��́A���̏c�x�N�g���̐ߓ_�̕��я��ɍ��킹��B
			//CreateMeshTempVec();
			//������n
			unsigned int dmnN = vertices.size();
			Tvec.resize(dmnN,1);
			for(std::vector<unsigned int>::size_type i=0; i < dmnN ; i++){
				Tvec[i][0] = vertices[i].temp;
			}


		//�M�`�����A�M�`�B���A���x�A��M�Ȃǂ̃p�����[�^�[��ݒ�E���
			//PHFemMEshThermo�̃����o�ϐ��̒l����
			//SetThermoConductionParam();
			//�����A�ϐ��l�͌ォ��v�Z�̓r���ŕύX�ł���悤��SetParam()�֐�������Ă������ق����������ȁH

		//�v�Z�ɗp����}�g���N�X�A�x�N�g�����쐬�i���b�V�����Ƃ̗v�f�����s��/�x�N�g���ˑS�̍����s��/�x�N�g���j
			//{T}�c�x�N�g���̐ߓ_�̕��я��ɕ��Ԃ悤�ɁA�W���s������Z����B�W���s��ɂ́A�ʐς�̐ρA�M�`�B���Ȃǂ̃p�����[�^�̐ς����Ă��܂������̂�����B
		//CreateK1();
		//CreateK2();
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

void PHFemMeshThermo::CreateK(){

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
