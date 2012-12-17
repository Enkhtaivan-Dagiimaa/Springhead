/*
 *  Copyright (c) 2003-2012, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include <Physics/PHFemVibration.h>

namespace Spr{;

PHFemVibration::PHFemVibration(const PHFemVibrationDesc& desc){
	SetDesc(&desc);
	// �A���~�̕���
	// �|�A�\����:0.35,�����O�� 70GPa, ���x2.70g/cm3
	poisson = 0.35;
	young = 70 * 1e6;
	density =  2.7 * 1e3; 
	alpha = 0.001;
	beta = 0.0001;
	timeStep = 0.001;
} 

void PHFemVibration::Init(){
	/// �S�̍����s��A�S�̎��ʍs��A�S�̌����s��̌v�Z
	/// �����͂��ׂă��[�J���n
	PHFemMeshNew* mesh = GetPHFemMesh();
	int NTets = mesh->tets.size();
	int NVertices = mesh->vertices.size();
	int NDof = NVertices * 3;
	// �e�S�̍s��̏�����
	matK.resize(NDof, NDof);
	matK.clear(0.0);
	matM.resize(NDof, NDof);
	matM.clear(0.0);
	matMInv.resize(NDof, NDof);
	matMInv.clear(0.0);
	matC.resize(NDof, NDof);
	matC.clear(0.0);
	for(int i = 0; i < NTets; i++){
		// �v�f�s��̌v�Z
		/// tet�������_��
		/// �v�f�����s�� u = (u0, u1, ..., u3, v0, v1, ..., v3, w0, w1, ..., w3)�Ƃ��Čv�Z
		Vec3d pos[4];
		for(int j = 0; j < 4; j++){
			pos[j] = mesh->vertices[mesh->tets[i].vertexIDs[j]].pos;
		}
		/// �s��C�̌v�Z�i���_���W�ɉ����ĕς��j
		PTM::TMatrixRow< 4,4,double > matCk1;	// matCk��1�u���b�N��
		for(int j = 0; j < 4; j++){
				matCk1.item(j, 0) = 1.0;
				matCk1.item(j, 1) = pos[j].x;
				matCk1.item(j, 2) = pos[j].y;
				matCk1.item(j, 3) = pos[j].z;
		}
		PTM::TMatrixRow< 4,4,double > matCk1Inv;
		matCk1Inv = matCk1.inv();
		PTM::TMatrixRow< 12, 12, double > matCkInv;
		matCkInv.clear(0.0);
		for(int j = 0; j < 4; j++){
			for(int k = 0; k < 4; k++){
				matCkInv[j][k] = matCk1Inv[j][k];
				matCkInv[j+4][k+4] = matCk1Inv[j][k];
				matCkInv[j+8][k+8] = matCk1Inv[j][k];
			}
		}

		/// �s��B�i�萔�j
		PTM::TMatrixRow< 6,12,double > matB;
		matB.clear(0.0);
		matB[0][1] = 1.0;
		matB[1][6] = 1.0;
		matB[2][11] = 1.0;
		matB[3][2] = 1.0;
		matB[3][5] = 1.0;
		matB[4][7] = 1.0;
		matB[4][10] = 1.0;
		matB[5][3] = 1.0;
		matB[5][9] = 1.0;

		/// �e���W���s��D�̌v�Z
		/// �i�����O���A�|�A�\����ɉ����Ă����j
		const double E = GetYoungModulus();
		const double v = GetPoissonsRatio();
		const double a = 1 - v;
		const double b = (1 - 2 * v);
		const double c = b / 2;
		double Em;
		if(b == 0.0) Em = DBL_MAX; /// �ό`���Ȃ��B�ق�Ƃ���+���ɂȂ�B
		else Em = E / (b * (v + 1));
		PTM::TMatrixRow< 6, 6,double > matD;
		matD.clear(0.0);
		matD[0][0] = a; matD[0][1] = v; matD[0][2] = v;
		matD[1][0] = v; matD[1][1] = a; matD[1][2] = v;
		matD[2][0] = v; matD[2][1] = v; matD[2][2] = a;
		matD[3][3] = c;
		matD[4][4] = c;
		matD[5][5] = c;
		matD *= Em;
		
		/// BtDB�̌v�Z
		PTM::TMatrixRow< 12, 12,double > matBtDB;
		matBtDB.clear(0.0);
		matBtDB = matB.trans() * matD * matB;

		/// �v�f�����s��̌v�Z
		TMatrixRow< 12, 12,double > matKe; // �v�f�����s��
		matKe.clear(0.0);
		matKe = matCkInv.trans() * matBtDB * matCkInv * mesh->GetTetrahedronVolume(i);	

		/// ���ʍs��̌v�Z
		TMatrixRow< 12, 12, double > matMe;
		matMe.clear(0.0);
		for(int j = 0; j < 3; j++){
			for(int k = 0; k < 4; k++){
				for(int l = 0; l < 4; l++){
					int id = j * 4;
					matMe[id + k][id + l] = 1.0;
					if(k == l) matMe[id + k][id + l] = 2.0;
				}
			}
		}
		matMe *= GetDensity() * mesh->GetTetrahedronVolume(i) / 20;

		/// �����s��i��ጸ���j
		TMatrixRow< 12, 12, double > matCe;
		matCe.clear(0.0);
		matCe = GetAlpha() * matMe + GetBeta() * matKe;

		/// �S�̍s��̌v�Z
		/// ���_�ԍ���
		/// u = (u0, ..., un-1, v0, ..., vn-1, w0, ..., wn-1)�Ƃ��Čv�Z 
		for(int j = 0; j < 4; j++){
			for(int k = 0; k < 4; k++){
				int id = mesh->tets[i].vertexIDs[j];
				int id2 = mesh->tets[i].vertexIDs[k];
				//DSTR << "ID1:ID2 " << id << " : " << id2 << std::endl;
				// �S�̍����s��
				matK[id][id2] += matKe[j][k];
				matK[NVertices + id][NVertices + id2] += matKe[j+4][k+4];
				matK[NVertices * 2 + id][NVertices * 2 + id2] += matKe[j+8][k+8];

				// �S�̎��ʍs��
				matM[id][id2] += matMe[j][k];
				matM[NVertices + id][NVertices + id2] += matMe[j+4][k+4];
				matM[NVertices * 2 + id][NVertices * 2 + id2] += matMe[j+8][k+8];

				// �S�̌����s��
				matC[id][id2] += matCe[j][k];
				matC[NVertices + id][NVertices + id2] += matCe[j+4][k+4];
				matC[NVertices * 2 + id][NVertices * 2 + id2] += matCe[j+8][k+8];
			}
		}
		// ���X�e�b�v�v�Z����K�v�̂Ȃ����̂�ۑ�
		matMInv = matM.inv();

		//DSTR << "matCk1" << std::endl;		DSTR << matCk1 << std::endl;
		//DSTR << "matCk1Inv" << std::endl;	DSTR << matCk1Inv << std::endl;
		//DSTR << "matCInv" << std::endl;		DSTR << matCkInv << std::endl;
		//DSTR << "matB" << std::endl;		DSTR << matB << std::endl;
		//DSTR << "matD" << std::endl;		DSTR << matD << std::endl;
		//DSTR << "matBtDB" << std::endl;		DSTR << matBtDB << std::endl;
		//DSTR << "matKe" << std::endl;		DSTR << matKe << std::endl;
		//DSTR << "matMe" << std::endl;		DSTR << matMe << std::endl;
	}
	//DSTR << "matK" << std::endl;		DSTR << matK << std::endl;
	//DSTR << "det matK" << std::endl;	DSTR << matK.det() << std::endl;
	//DSTR << "matM" << std::endl;		DSTR << matM << std::endl;
	//DSTR << "matC" << std::endl;		DSTR << matC << std::endl;

	// �e��ϐ��̏�����
	xlocalLast.resize(NDof);
	xlocalLast.clear(0.0);
	xlocal.resize(NDof);
	xlocal.clear(0.0);
	vlocalLast.resize(NDof);
	vlocalLast.clear(0.0);
	vlocal.resize(NDof);
	vlocal.clear(0.0);
	flocal.resize(NDof);
	flocal.clear(0.0);
	/// FemVertex����l������Ă���
	for(int i = 0; i < NVertices; i++){
		for(int j = 0; j < 3; j++){
			// u = (x1, ..., xn-1, y1, ..., yn-1, z1, ..., zn-1)�̏�
			xlocalLast[i + NVertices * j] = mesh->vertices[i].pos[j];
			xlocal[i + NVertices * j] = mesh->vertices[i].pos[j];
			//flocal[i + NVertices * j] = ;
			//DSTR << i+NVertices*j << " " << xlocal[i+NVertices*j] << std::endl;
		}
	}
	DSTR << "initial xlocal" << std::endl;
	DSTR << xlocal << std::endl;
	DSTR << "initial vlocal" << std::endl;
	DSTR << vlocal << std::endl;

	// �e�X�g�R�[�h
	//AddBoundaryCondition(0, Vec3d(1, 1, 1));
	mesh->AddLocalDisplacement(1, Vec3d(0.1, 0.0, 0.0));
}

#define DEBUG
void PHFemVibration::Step(){
	//DSTR << "Step" << std::endl;
#ifdef DEBUG
	DSTR << "///////////////////////////////////////////////" << std::endl;
#endif
	PHFemMeshNew* mesh = GetPHFemMesh();
	int NVertices = mesh->vertices.size();
	/// FemVertex����l������Ă���
	for(int i = 0; i < NVertices; i++){
		for(int j = 0; j < 3; j++){
			// u = (x1, ..., xn-1, y1, ..., yn-1, z1, ..., zn-1)�̏�
			xlocal[i + NVertices * j] = mesh->vertices[i].pos[j];
			//flocal[i + 4 * j] = mesh->vertices[i].pos[j];
			//DSTR << i+4*j << " " << xlocal[i+4*j] << std::endl;
		}
	}
#ifdef DEBUG
	DSTR << "vlocal last" << std::endl;
	DSTR << vlocal << std::endl;
	DSTR << "xlocal last" << std::endl;
	DSTR << xlocal  << std::endl;
#endif

	///�ϕ�
	ExplicitEuler();
	//ImplicitEuler();

	/// �v�Z���ʂ�FemVertex�ɖ߂�
	for(int i = 0; i < NVertices; i++){
		for(int j = 0; j < 3; j++){
			// u = (x1, ..., xn-1, y1, ..., yn-1, z1, ..., zn-1)�̏�
			mesh->vertices[i].pos[j] = xlocal[i + NVertices * j];
		}
	}
#ifdef DEBUG
	//DSTR << "tmp" << std::endl;
	//DSTR << tmp << std::endl;
	DSTR << "vlocal" << std::endl;
	DSTR << vlocal << std::endl;
	DSTR << "xlocal updated" << std::endl;
	DSTR << xlocal << std::endl;
#endif
}

void PHFemVibration::ExplicitEuler(){
	int NDof = GetPHFemMesh()->vertices.size() * 3;
	VVector< double > tmp;
	tmp.resize(NDof);
	tmp.clear(0.0);
	tmp = -1 * matK * (xlocal - xlocalLast) - matC * (vlocal - vlocalLast) + flocal;
#if 0
	// �O��̈ʒu���x���g���Ǝ������Ă����B
	// ����͐������Ȃ�����
	xlocalLast = xlocal;
	vlocalLast = vlocal;
#endif 
	vlocal += matMInv * tmp * timeStep;
	xlocal += vlocal * timeStep;
	//DSTR << matK * (xlocal - xlocalLast) << std::endl;
	//DSTR << "tmp" << std::endl;
	//DSTR << tmp << std::endl;
	//DSTR << "matMinv * tmp" << std::endl;
	//DSTR << matMInv * tmp << std::endl;
	//DSTR << matMInv << std::endl;
}

void PHFemVibration::ImplicitEuler(){
	// xlocal�͈ʒu�Ȃ񂾂��ǁA�ψʂ��l������ĂȂ��B�B�B
	// �O�͂��܂��g�ݍ���łȂ�
	int NDof = GetPHFemMesh()->vertices.size() * 3;
	VMatrixRow< double > E;
	E.resize(NDof, NDof);
	E.clear(0.0);
	for(int i = 0; i < NDof; i++){
		E[i][i] = 1.0;
	}
	VMatrixRow< double > _K;
	_K.resize(NDof, NDof);
	_K.clear(0.0);	
	VMatrixRow< double > _C;
	_C.resize(NDof, NDof);
	_C.clear(0.0);
	VMatrixRow< double > D;
	D.resize(NDof, NDof);
	D.clear(0.0);

	_K = matMInv * matK * timeStep;
	_C = E + matMInv * matC * timeStep;
	D = E + _C.inv() * _K * timeStep;

	xlocal = D.inv() * (xlocal + _C.inv() * vlocal * timeStep);
	vlocal = _C.inv() * (vlocal - _K * xlocal);

	DSTR << "Integrate" << std::endl;
	DSTR << "_K" << std::endl;
	DSTR << _K << std::endl;
	DSTR << "_C" << std::endl;
	DSTR << _C << std::endl;
	DSTR << "D.inv()" << std::endl;
	DSTR << D.inv() << std::endl;
}

bool PHFemVibration::AddBoundaryCondition(int vtxId, Vec3i dof){
	PHFemMeshNew* mesh = GetPHFemMesh();
	if(0 <= vtxId && vtxId <= mesh->vertices.size() -1){
		if(dof[0] == 1){
			// x�����̍S��
			const int xId = vtxId;
			matK.row(xId).clear(0.0);
			matK[xId][xId] = 1.0;
			matC.row(xId).clear(0.0);
			matC[xId][xId] = 1.0;
			matMInv.row(xId).clear(0.0);
			matMInv[xId][xId] = 1.0;
		}
		if(dof[1] == 1){
			// y�����̍S��
			const int yId = vtxId + 4;
			matK.row(yId).clear(0.0);
			matK[yId][yId] = 1.0;
			matC.row(yId).clear(0.0);
			matC[yId][yId] = 1.0;
			matMInv.row(yId).clear(0.0);
			matMInv[yId][yId] = 1.0;		
		}
		if(dof[2] == 1){
			// z�����̍S��
			const int zId = vtxId + 8;
			matK.row(zId).clear(0.0);
			matK[zId][zId] = 1.0;
			matMInv.row(zId).clear(0.0);			
			matC.row(zId).clear(0.0);
			matC[zId][zId] = 1.0;
			matMInv.row(zId).clear(0.0);
			matMInv[zId][zId] = 1.0;		
		}
		//DSTR << "matK with boundary condition" << std::endl;
		//DSTR << matK << std::endl;
		//DSTR << "matC with boundary condition" << std::endl;
		//DSTR << matC << std::endl;
		//DSTR << "matMInv with boundary condition" << std::endl;
		//DSTR << matMInv << std::endl;
		return true;
	}
	return false;
}

}