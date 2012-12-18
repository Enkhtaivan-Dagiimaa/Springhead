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
} 

void PHFemVibration::Init(){
	/// ���ݎ��Ԃ̐ݒ�
	PHSceneIf* scene = GetPHFemMesh()->GetScene()->Cast();
	if(scene) vdt = scene->GetTimeStep();
	else vdt = 0.001;

	/// �S�̍����s��A�S�̎��ʍs��A�S�̌����s��̌v�Z
	/// �����͂��ׂă��[�J���n
	PHFemMeshNew* mesh = GetPHFemMesh();
	int NTets = mesh->tets.size();
	int NVer= NVertices();
	int NDof = NVer * 3;
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
				matK[NVer + id][NVer + id2] += matKe[j+4][k+4];
				matK[NVer * 2 + id][NVer * 2 + id2] += matKe[j+8][k+8];

				// �S�̎��ʍs��
				matM[id][id2] += matMe[j][k];
				matM[NVer + id][NVer + id2] += matMe[j+4][k+4];
				matM[NVer * 2 + id][NVer * 2 + id2] += matMe[j+8][k+8];

				// �S�̌����s��
				matC[id][id2] += matCe[j][k];
				matC[NVer + id][NVer + id2] += matCe[j+4][k+4];
				matC[NVer * 2 + id][NVer * 2 + id2] += matCe[j+8][k+8];
			}
		}
		// ���X�e�b�v�v�Z����K�v�̂Ȃ����̂�ۑ�
		matMInv = matM.inv();

		//DSTR << "matCk1" << std::endl;	DSTR << matCk1 << std::endl;
		//DSTR << "matCk1Inv" << std::endl;	DSTR << matCk1Inv << std::endl;
		//DSTR << "matCInv" << std::endl;	DSTR << matCkInv << std::endl;
		//DSTR << "matB" << std::endl;		DSTR << matB << std::endl;
		//DSTR << "matD" << std::endl;		DSTR << matD << std::endl;
		//DSTR << "matBtDB" << std::endl;	DSTR << matBtDB << std::endl;
		//DSTR << "matKe" << std::endl;		DSTR << matKe << std::endl;
		//DSTR << "matMe" << std::endl;		DSTR << matMe << std::endl;
	}
	//DSTR << "matK" << std::endl;		DSTR << matK << std::endl;
	//DSTR << "det matK" << std::endl;	DSTR << matK.det() << std::endl;
	//DSTR << "matM" << std::endl;		DSTR << matM << std::endl;
	//DSTR << "matC" << std::endl;		DSTR << matC << std::endl;

	// �e��ϐ��̏�����
	xlocalInit.resize(NDof);
	xlocalInit.clear(0.0);
	xlocal.resize(NDof);
	xlocal.clear(0.0);
	vlocal.resize(NDof);
	vlocal.clear(0.0);
	flocal.resize(NDof);
	flocal.clear(0.0);
	/// FemVertex����l������Ă���
	for(int i = 0; i < NVer; i++){
		for(int j = 0; j < 3; j++){
			// u = (x1, ..., xn-1, y1, ..., yn-1, z1, ..., zn-1)�̏�
			xlocalInit[i + NVer * j] = mesh->vertices[i].pos[j];
			xlocal[i + NVer * j] = mesh->vertices[i].pos[j];
		}
	}
	DSTR << "initial xlocal" << std::endl;	DSTR << xlocalInit << std::endl;
	DSTR << "initial vlocal" << std::endl;	DSTR << vlocal << std::endl;

	// �e�X�g�R�[�h
	vdt = 0.001;
	AddBoundaryCondition(0, Vec3d(1, 1, 1));
	AddBoundaryCondition(5, Vec3d(1, 1, 1));
	//AddVertexForce(1, Vec3d(100000.0, 0.0, 0.0));
	//mesh->AddLocalDisplacement(1, Vec3d(0.1, 0.0, 0.0));
	DSTR << "flocal" << std::endl;	DSTR << flocal << std::endl;
} 

#define DEBUG
void PHFemVibration::Step(){
	//DSTR << "Step" << std::endl;
#ifdef DEBUG
	DSTR << "///////////////////////////////////////////////" << std::endl;
#endif
	int NVer = NVertices();
	/// FemVertex����l������Ă���
	for(int i = 0; i < NVer; i++){
		for(int j = 0; j < 3; j++){
			// u = (x1, ..., xn-1, y1, ..., yn-1, z1, ..., zn-1)�̏�
			xlocal[i + NVer * j] = GetPHFemMesh()->vertices[i].pos[j];
		}
	}

#ifdef DEBUG
	DSTR << "vlocal last" << std::endl;	DSTR << vlocal << std::endl;
	DSTR << "xlocal last" << std::endl;	DSTR << xlocal  << std::endl;
#endif

	///�ϕ�
	//ExplicitEuler();
	//ImplicitEuler();
	NewmarkBeta();
	
	flocal.clear(0.0);

	/// �v�Z���ʂ�FemVertex�ɖ߂�
	for(int i = 0; i < NVer; i++){
		for(int j = 0; j < 3; j++){
			// u = (x1, ..., xn-1, y1, ..., yn-1, z1, ..., zn-1)�̏�
			GetPHFemMesh()->vertices[i].pos[j] = xlocal[i + NVer * j];
		}
	}
#ifdef DEBUG
	DSTR << "vlocal updated" << std::endl;	DSTR << vlocal << std::endl;
	DSTR << "xlocal updated" << std::endl;	DSTR << xlocal << std::endl;
#endif
}

void PHFemVibration::ExplicitEuler(){
	int NDof = NVertices() * 3;
	VVector< double > xlocalDisp;
	xlocalDisp.resize(NDof);
	xlocalDisp.clear(0.0);
	VVector< double > tmp;
	tmp.resize(NDof);
	tmp.clear(0.0);
	xlocalDisp = xlocal - xlocalInit;
	tmp = -1 * matK * xlocalDisp - matC * vlocal + flocal;
	vlocal += matMInv * tmp * vdt;
	xlocal += vlocal * vdt;
}

void PHFemVibration::ImplicitEuler(){
	// �O�͂��܂��g�ݍ���łȂ�
	// ���܂������Ă��Ȃ�
	int NDof = GetPHFemMesh()->vertices.size() * 3;
	VMatrixRow< double > E;	// �P�ʍs��
	E.resize(NDof, NDof);
	E.clear(0.0);
	for(int i = 0; i < NDof; i++){
		E[i][i] = 1.0;
	}
	VMatrixRow< double > _K;
	_K.resize(NDof, NDof);
	_K.clear(0.0);	
	VMatrixRow< double > _CInv;
	_CInv.resize(NDof, NDof);
	_CInv.clear(0.0);
	VMatrixRow< double > D;
	D.resize(NDof, NDof);
	D.clear(0.0);

	_K = matMInv * matK * vdt;
	_CInv = (E + matMInv * matC * vdt).inv();
	D = E + _CInv * _K * vdt;

	xlocal += D.inv() * vlocal * vdt;
	vlocal += -1 * _CInv * _K * (xlocal - xlocalInit);
	//DSTR << "Integrate" << std::endl;
	//DSTR << "_K" << std::endl;
	//DSTR << _K << std::endl;
	//DSTR << "_CInv" << std::endl;
	//DSTR << _CInv << std::endl;
	//DSTR << "D.inv()" << std::endl;
	//DSTR << D.inv() << std::endl;
}

void PHFemVibration::NewmarkBeta(const double b){
	int NDof = NVertices() * 3;
	static VVector< double > alocal;
	alocal.resize(NDof);
	alocal.clear(0.0);

	VVector< double > alocalLast;
	alocalLast.resize(NDof);
	alocalLast.clear(0.0);

	VMatrixRow< double > _MInv;
	_MInv.resize(NDof, NDof);
	_MInv.clear(0.0);
	_MInv = (matM + 0.5 * matC * vdt + b * matK * pow(vdt, 2)).inv();

	VVector< double > Ct;
	Ct.resize(NDof);
	Ct.clear(0.0);

	VVector< double > Kt;
	Kt.resize(NDof);
	Kt.clear(0.0);

	VVector< double > xlocalDisp;
	xlocalDisp.resize(NDof);
	xlocalDisp.clear(0.0);
	xlocalDisp = xlocal - xlocalInit;

	static bool bflag = true;
	if(bflag){
		alocal = _MInv * (- 1 * matC * vlocal -1 * matK * xlocalDisp + flocal);
		bflag = false;
	}

	Ct = matC * (vlocal + 0.5 * alocal * vdt);
	Kt = matK * (xlocalDisp + vlocal * vdt + alocal * pow(vdt, 2) * (0.5-b));
	alocalLast = alocal;
	alocal = _MInv * (flocal - Ct - Kt);
	vlocal += 0.5 * (alocalLast + alocal) * vdt;
	xlocal += vlocal * vdt + (0.5 - b)* alocalLast * pow(vdt, 2) + alocal * b * pow(vdt, 2);
	//DSTR << "Ct" << std::endl;		DSTR << Ct << std::endl;
	//DSTR << "Kt" << std::endl;		DSTR << Kt << std::endl;
	//DSTR << "alocal" << std::endl;	DSTR << alocal << std::endl;
	//DSTR << "_MInv" << std::endl;	DSTR << _MInv << std::endl;
}

bool PHFemVibration::AddBoundaryCondition(int vtxId, Vec3i dof){
	PHFemMeshNew* mesh = GetPHFemMesh();
	int NVertices = mesh->vertices.size();
	if(0 <= vtxId && vtxId <= NVertices -1){
		if(dof[0] == 1){
			// x�����̍S��
			const int xId = vtxId;
			matK.row(xId).clear(0.0);
			matK[xId][xId] = 1.0;
			matC.row(xId).clear(0.0);
			matC[xId][xId] = 1.0;
			matM.row(xId).clear(0.0);
			matM[xId][xId] = 1.0;
			matMInv.row(xId).clear(0.0);
			matMInv[xId][xId] = 1.0;
		}
		if(dof[1] == 1){
			// y�����̍S��
			const int yId = vtxId + NVertices;
			matK.row(yId).clear(0.0);
			matK[yId][yId] = 1.0;
			matC.row(yId).clear(0.0);
			matC[yId][yId] = 1.0;
			matM.row(yId).clear(0.0);
			matM[yId][yId] = 1.0;	
			matMInv.row(yId).clear(0.0);
			matMInv[yId][yId] = 1.0;		
		}
		if(dof[2] == 1){
			// z�����̍S��
			const int zId = vtxId + NVertices * 2;
			matK.row(zId).clear(0.0);
			matK[zId][zId] = 1.0;
			matMInv.row(zId).clear(0.0);			
			matC.row(zId).clear(0.0);
			matC[zId][zId] = 1.0;
			matM.row(zId).clear(0.0);
			matM[zId][zId] = 1.0;			
			matMInv.row(zId).clear(0.0);
			matMInv[zId][zId] = 1.0;		
		}
		DSTR << "matK with boundary condition" << std::endl;
		DSTR << matK << std::endl;
		DSTR << "matC with boundary condition" << std::endl;
		DSTR << matC << std::endl;
		DSTR << "matMInv with boundary condition" << std::endl;
		DSTR << matMInv << std::endl;
		return true;
	}
	return false;
}

bool PHFemVibration::AddBoundaryCondition(VVector< Vec3i > bcs){ 
	if(NVertices() != bcs.size()) return false;
	for(int i = 0; i < (int)bcs.size(); i++){
		if(bcs[i].x == 1){
			// x�����̍S��
			const int xId = i;
			matK.row(xId).clear(0.0);
			matK[xId][xId] = 1.0;
			matC.row(xId).clear(0.0);
			matC[xId][xId] = 1.0;
			matM.row(xId).clear(0.0);
			matM[xId][xId] = 1.0;
			matMInv.row(xId).clear(0.0);
			matMInv[xId][xId] = 1.0;
		}
		if(bcs[i].y == 1){
			// y�����̍S��
			const int yId = i + NVertices();
			matK.row(yId).clear(0.0);
			matK[yId][yId] = 1.0;
			matC.row(yId).clear(0.0);
			matC[yId][yId] = 1.0;
			matM.row(yId).clear(0.0);
			matM[yId][yId] = 1.0;
			matMInv.row(yId).clear(0.0);
			matMInv[yId][yId] = 0.0;		
		}
		if(bcs[i].z == 1){
			// z�����̍S��
			const int zId = i + NVertices() * 2;
			matK.row(zId).clear(0.0);
			matK[zId][zId] = 1.0;
			matMInv.row(zId).clear(0.0);			
			matC.row(zId).clear(0.0);
			matC[zId][zId] = 1.0;
			matM.row(zId).clear(0.0);
			matM[zId][zId] = 1.0;
			matMInv.row(zId).clear(0.0);
			matMInv[zId][zId] = 0.0;		
		}		
	}
	return true;
}

bool PHFemVibration::AddVertexForce(int vtxId, Vec3d fW){
	if(0 <= vtxId && vtxId <= NVertices() -1){
		Vec3d fL = GetPHFemMesh()->GetPHSolid()->GetPose() * fW;
		flocal[vtxId] += fL.x;
		flocal[vtxId + NVertices()] += fL.y;
		flocal[vtxId + NVertices() * 2] += fL.z;
		return true;
	}
	return false;
}

bool PHFemVibration::AddVertexForce(VVector< Vec3d > fWs){
	if(NVertices() != fWs.size()) return false;
	for(int i = 0; i < (int)fWs.size(); i++){
		Vec3d fL = GetPHFemMesh()->GetPHSolid()->GetPose().Inv() * fWs[i];
		flocal[i] += fL.x;
		flocal[i + NVertices()] += fL.y;
		flocal[i + NVertices() * 2] += fL.z;
	}
	return true;
}

}