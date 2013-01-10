/*
 *  Copyright (c) 2003-2012, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include <Physics/PHFemVibration.h>
#include <SciLab/SprSciLab.h>

namespace Spr{;

//* �������ƍs��̍쐬
/////////////////////////////////////////////////////////////////////////////////////////
PHFemVibration::PHFemVibration(const PHFemVibrationDesc& desc){
	SetDesc(&desc);
	integration_mode = PHFemVibrationDesc::MODE_EXPLICIT_EULER;
	integration_mode = PHFemVibrationDesc::MODE_NEWMARK_BETA;
	//integration_mode = PHFemVibrationDesc::MODE_MODAL_ANALYSIS;
	IsScilabStarted = false;
} 

void PHFemVibration::Init(){
	// Scilab�̋N��
	IsScilabStarted = ScilabStart();
	if(!IsScilabStarted){
		DSTR << "Scilab can not start." << std::endl;
	}

	DSTR << "Initializing PHFemVibration" << std::endl;
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
	matKIni.resize(NDof, NDof, 0.0);
	matMIni.resize(NDof, NDof, 0.0);
	matCIni.resize(NDof, NDof, 0.0);
	for(int i = 0; i < NTets; i++){
		// �v�f�s��̌v�Z
		/// tet�������_��
		/// �v�f�����s�� u = (u0, v0, w0,  ..., un-1, vn-2, wn-1)�Ƃ��Čv�Z
#if 0
		/// ��{�ό`�ʔ�
		/// �s��C(�ό`-��{�ό`�ʁj�̌v�Z�i���_���W�ɉ����ĕς��j
		PTM::TMatrixRow< 1, 4, element_type > vec[4];
		for(int j = 0; j < 4; j++){
			Vec3d pos = mesh->vertices[mesh->tets[i].vertexIDs[j]].pos;
			vec[j].item(0, 0) = 1.0;
			vec[j].item(0, 1) = pos.x;
			vec[j].item(0, 2) = pos.y;
			vec[j].item(0, 3) = pos.z;
		}
		PTM::TMatrixRow< 12, 12, element_type > matCk;
		matCk.clear(0.0);
		for(int j = 0; j < 4; j++){
			int id = 3 * j;
			matCk.vsub_matrix(id, 0, 1, 4) = vec[j];
			matCk.vsub_matrix(id + 1, 4, 1, 4)= vec[j];
			matCk.vsub_matrix(id + 2, 8, 1, 4)= vec[j];
		}
		PTM::TMatrixRow< 12, 12, element_type > matCkInv;
		matCkInv.assign(matCk.inv());

		/// �s��B(�Ђ���-��{�ό`�ʁj�i�萔�j
		PTM::TMatrixRow< 6, 12, element_type > matB;
		matB.clear(0.0);
		matB[0][1] = 1.0;
		matB[1][6] = 1.0;
		matB[2][11] = 1.0;
		matB[3][2] = 1.0;	matB[3][5] = 1.0;
		matB[4][7] = 1.0;	matB[4][10] = 1.0;
		matB[5][3] = 1.0;	matB[5][9] = 1.0;

		/// �e���W���s��D�i����-�Ђ��݁j�̌v�Z
		/// �i�����O���A�|�A�\����ɉ����Ă����j
		double E = GetYoungModulus();
		double v = GetPoissonsRatio();
		double a = 1 - v;
		double b = 1 - 2 * v;
		double c = 0.5 - v;
		double Em;
		if(b == 0.0) Em = DBL_MAX; /// �ό`���Ȃ��B�ق�Ƃ���+���ɂȂ�B
		else Em = E / ((1 + v) * b);
		PTM::TMatrixRow< 6, 6, element_type > matD;
		matD.clear(0.0);
		matD[0][0] = a; matD[0][1] = v; matD[0][2] = v;
		matD[1][0] = v; matD[1][1] = a; matD[1][2] = v;
		matD[2][0] = v; matD[2][1] = v; matD[2][2] = a;
		matD[3][3] = c;
		matD[4][4] = c;
		matD[5][5] = c;
		matD *= Em;
		
		/// BtDB�̌v�Z
		PTM::TMatrixRow< 12, 12, element_type > matBtDB;
		matBtDB.clear(0.0);
		matBtDB = matB.trans() * matD * matB;

		/// �v�f�����s��̌v�Z(�G�l���M�[�����j
		TMatrixRow< 12, 12, element_type > matKe; // �v�f�����s��
		matKe.clear(0.0);
		matKe = matCkInv.trans() * matBtDB * matCkInv * mesh->GetTetrahedronVolume(i);
#else
		/// �`��֐���
		/// �`��֐��̌v�Z�i���_���W�ɉ����ĕς��j
		PTM::TMatrixRow< 4, 4, element_type > matPos;
		for(int j = 0; j < 4; j++){
			TVector< 3, element_type > pos = mesh->vertices[mesh->tets[i].vertexIDs[j]].pos;
			matPos.item(j, 0) = 1.0;
			matPos.item(j, 1) = pos[0];
			matPos.item(j, 2) = pos[1];
			matPos.item(j, 3) = pos[2];
			//DSTR << mesh->tets[i].vertexIDs[j] << std::endl;
		}
		PTM::TMatrixRow< 4, 4, element_type > matCofact;		// mat�̗]���q�s��
		matCofact = (matPos.det() * matPos.inv()).trans();
		TVector<4, element_type > b, c, d;	// �`��֐��̌W��
		b.assign(matCofact.col(1));
		c.assign(matCofact.col(2));
		d.assign(matCofact.col(3));

		/// �s��B�i�Ђ���-�ψʁj
		PTM::TMatrixRow< 6, 12, element_type > matB;
		matB.clear(0.0);
		matB[0][0] = b[0];	matB[0][3] = b[1];	matB[0][6] = b[2];	matB[0][9] = b[3];
		matB[1][1] = c[0];	matB[1][4] = c[1];	matB[1][7] = c[2];	matB[1][10] = c[3];
		matB[2][2] = d[0];	matB[2][5] = d[1];	matB[2][8] = d[2];	matB[2][11] = d[3];
		matB[3][0] = c[0];	matB[3][1] = b[0];	matB[3][3] = c[1];	matB[3][4] = b[1];	matB[3][6] = c[2];	matB[3][7] = b[2];	matB[3][9] = c[3];	matB[3][10] = b[3];
		matB[4][1] = d[0];	matB[4][2] = c[0];	matB[4][4] = d[1];	matB[4][5] = c[1];	matB[4][7] = d[2];	matB[4][8] = c[2];	matB[4][10] = d[3];	matB[4][11] = c[3];
		matB[5][0] = d[0];	matB[5][2] = b[0];	matB[5][3] = d[1];	matB[5][5] = b[1];	matB[5][6] = d[2];	matB[5][8] = b[2];	matB[5][9] = d[3];	matB[5][11] = b[3];
		element_type div = 1.0 / (6.0 * mesh->GetTetrahedronVolume(i));
		matB *= div;

		/// �e���W���s��D�̌v�Z�i����-�Ђ��݁j
		/// �i�����O���A�|�A�\����ɉ����Ă����j
		double E = GetYoungModulus();
		double v = GetPoissonsRatio();
		double av = 1.0 - v;
		double bv = 1.0 - 2.0 * v;
		double cv = 0.5 - v;
		double Em;
		if(bv == 0.0) Em = DBL_MAX; /// �ό`���Ȃ��B�ق�Ƃ���+���ɂȂ�B
		else Em = E / ((1.0 + v) * bv);
		PTM::TMatrixRow< 6, 6, element_type > matD;
		matD.clear(0.0);
		matD[0][0] = av;	matD[0][1] = v;		matD[0][2] = v;
		matD[1][0] = v;		matD[1][1] = av;	matD[1][2] = v;
		matD[2][0] = v;		matD[2][1] = v;		matD[2][2] = av;
		matD[3][3] = cv;
		matD[4][4] = cv;
		matD[5][5] = cv;
		matD *= Em;

		/// �v�f�����s��̌v�Z(�G�l���M�[�����j
		TMatrixRow< 12, 12, element_type > matKe;
		matKe.clear(0.0);
		matKe = matB.trans() * matD * matB * mesh->GetTetrahedronVolume(i);
#endif
		/// ���ʍs��̌v�Z
		TMatrixRow< 12, 12, element_type > matMe;
		matMe.clear(0.0);
		TMatrixRow< 3, 3, element_type > I;
		I.clear(0.0);
		I[0][0] = 1.0; I[1][1] = 1.0; I[2][2] = 1.0;
		for(int j = 0; j < 4; j++){
			for(int k = 0; k < 4; k++){
				int t = j * 3;
				int l = k * 3;
				int h = 3;
				int w = 3;
				if(j == k){
					matMe.vsub_matrix(t, l, h, w) = 2.0 * I;
				}else{
					matMe.vsub_matrix(t, l, h, w) = I;
				}
			}
		}
		matMe *= GetDensity() * mesh->GetTetrahedronVolume(i) / 20.0;

		/// �����s��i��ጸ���j
		TMatrixRow< 12, 12, element_type > matCe;
		matCe.clear(0.0);
		matCe = GetAlpha() * matMe + GetBeta() * matKe;
		
		//�S�̍s��̌v�Z
		//���_�ԍ��� 
		// u = (u0, v0, w0,  ..., un-1, vn-2, wn-1)�Ƃ��Čv�Z
		// j:�u���b�N�ԍ�, k:�u���b�N�ԍ�
		for(int j = 0; j < 4; j++){
			for(int k = 0; k < 4; k++){
				int id = mesh->tets[i].vertexIDs[j];
				int id2 = mesh->tets[i].vertexIDs[k];
				int t = id * 3;	int l = id2 * 3;
				int h = 3;		int w = 3;
				int te = j * 3;	int le = k * 3;
				// �S�̍����s��
				matKIni.vsub_matrix(t, l, h, w) += matKe.vsub_matrix(te, le, h, w);
				// �S�̎��ʍs��
				matMIni.vsub_matrix(t, l, h, w) += matMe.vsub_matrix(te, le, h, w);
				// �S�̌����s��
				matCIni.vsub_matrix(t, l, h, w) += matCe.vsub_matrix(te, le, h, w);
			}
		}
		//DSTR << "matCk1" << std::endl;	DSTR << matCk1 << std::endl;
		//DSTR << "matCk1Inv" << std::endl;	DSTR << matCk1Inv << std::endl;
		//DSTR << "matCkInv" << std::endl;	DSTR << matCkInv << std::endl;
		//DSTR << "matB" << std::endl;		DSTR << matB << std::endl;
		//DSTR << "matD" << std::endl;		DSTR << matD << std::endl;
		//DSTR << "matBtDB" << std::endl;	DSTR << matBtDB << std::endl;
		//DSTR << "det matKe : "<< matKe.det() << std::endl;
		//DSTR << "matKe" << std::endl;		DSTR << matKe << std::endl;
		//DSTR << "matMe" << std::endl;		DSTR << matMe << std::endl;
		//if(i == 0){
		//	MatrixFileOut(matKe, "matKe0.csv");
		//}else if( i == 1){
		//	MatrixFileOut(matKe, "matKe1.csv");		
		//}
	}	
	DSTR << "det(matKIni) = " << matKIni.det() << std::endl;
	DSTR << "matKIni" << std::endl;	DSTR << matKIni << std::endl;
	ScilabDeterminant(matKIni, "matKIni");
	ScilabFileOut(matKIni, "matKIni.dat");
	MatrixFileOut(matKIni, "matKini.csv");
	//DSTR << "det(matMIni) = " << matMIni.det() << std::endl;
	//DSTR << "matMIni" << std::endl;	DSTR << matMIni << std::endl;
	//DSTR << "det(matCIni) = " << matCIni.det() << std::endl;
	//DSTR << "matCIni" << std::endl;	DSTR << matCIni << std::endl;

	/// �e��ϐ��̏�����
	matMp.assign(matMIni);
	matKp.assign(matKIni);
	matCp.assign(matCIni);
	xdl.resize(NDof, 0.0);
	vl.resize(NDof, 0.0);
	fl.resize(NDof, 0.0);
	boundary.resize(NDof, 0.0);
	/// FemVertex����ψʂ�����Ă���
	GetVerticesDisplacement(xdl);
	//DSTR << "initial xdl" << std::endl;	DSTR << xdl << std::endl;
	//DSTR << "initial vl" << std::endl;	DSTR << vl << std::endl;
	DSTR << "Initializing Completed." << std::endl;

	// �e�X�g�i���E�����̕t���j
	vdt = 0.001;
	Vec3i con = Vec3i(1,1,1);
	AddBoundaryCondition(0, con);
	AddBoundaryCondition(3, con);
	AddBoundaryCondition(1, con);
	ReduceMatrixSize(matMp, matKp, matCp, boundary);
	DSTR << "After Reducing" << std::endl;
	//DSTR << "matMp" << std::endl;	DSTR << matMp << std::endl;
	DSTR << "matKp" << std::endl;	DSTR << matKp << std::endl;
	//DSTR << "matCp" << std::endl;	DSTR << matCp << std::endl;
	MatrixFileOut(matKp, "matKp.csv");
	ScilabDeterminant(matKp, "matKp");
	ScilabEigenValueAnalysis(matMp, matKp);
#if 0
	// �ŗL�l�̃T�C���{�e�X�g
	VMatrixRe matk;
	matk.resize(5, 5, 0.0);
	matk.item(0, 0) = 200;		matk.item(0, 1) = -100;
	matk.item(1, 0) = -100;	    matk.item(1, 1) = 100;
	matk.item(2, 2) = 20;
	matk.item(3, 3) = 20;
	matk.item(4,4) = 1;

	VMatrixRe matm;
	matm.resize(5, 5, 0.0);
	matm.item(0, 0) = 3;	matm.item(0, 2) = 2;
	matm.item(1, 1) = 3;	matm.item(1, 3) = 2;
	matm.item(2, 0) = 2;	matm.item(2, 2) = 4;
	matm.item(3, 1) = 2;	matm.item(3, 3) = 4;
	matm.item(4,4) = 1;
	DSTR << "ScilabTest" << std::endl;
	ScilabEigenValueAnalysis(matm, matk);
#endif
} 

void PHFemVibration::Step(){
	// FemVertex����ψʂ��Ƃ��Ă���
	GetVerticesDisplacement(xdl);
	VVectord xdlp;
	xdlp.assign(xdl);
	VVectord vlp;
	vlp.assign(vl);
	VVectord flp;
	flp.assign(fl);
	ReduceVectorSize(xdlp, vlp, flp, boundary);
#if 1
	///�ϕ�
	switch(integration_mode){
		case PHFemVibrationDesc::MODE_EXPLICIT_EULER:
			ExplicitEuler(matMp.inv(), matKp, matCp, flp, vdt, xdlp, vlp);
			break;
		case PHFemVibrationDesc::MODE_IMPLICIT_EULER:
			//ImplicitEuler(matMp.inv(), matKIni, matCIni,fl,vdt, xdl, vl);
			break;
		case PHFemVibrationDesc::MODE_NEWMARK_BETA:
			NewmarkBeta(matMp, matKp, matCp, flp, vdt, xdlp, vlp);
			break;
		case PHFemVibrationDesc::MODE_MODAL_ANALYSIS:
			ModalAnalysis(matMp, matKp, matCp, flp,vdt, xdlp, vlp, matMp.height());
			break;
		default:
			break;
	}
#endif
	fl.clear(0.0);
	// �v�Z���ʂ�FemVertex�ɔ��f
	GainVectorSize(xdlp, vlp, boundary);
	xdl.assign(xdlp);
	vl.assign(vlp);
	UpdateVerticesPosition(xdl);

#if 0
	DSTR << "vl updated" << std::endl;	DSTR << vl << std::endl;
	DSTR << "xdl updated" << std::endl;	DSTR << xdl << std::endl;
	//for(int i = 0; i < NVertices(); i++){
		Vec3d pos = GetPHFemMesh()->GetVertexPositionL(2);
		//CSVOUT << pos[0] << "," << pos[1] << "," << pos[3] << std::endl;
		pos = GetPHFemMesh()->GetVertexDisplacementL(2);
		CSVOUT << pos[0] << "," << pos[1] << "," << pos[3] << std::endl;
	//}
	DSTR << "///////////////////////////////////////////////" << std::endl;
#endif
}

//* �ϕ��֐�
/////////////////////////////////////////////////////////////////////////////////////////
void PHFemVibration::ExplicitEuler(const VMatrixRe& _MInv, const VMatrixRe& _K, const VMatrixRe& _C, 
		const VVectord& _f, const double& _dt, VVectord& _xd, VVectord& _v){
	int NDof = NVertices() * 3;
	VVectord tmp;
	tmp.resize(NDof, 0.0);
	tmp = -1 * (_K * _xd) - (_C * _v) + _f;
	_v += _MInv * tmp * _dt;
	_xd += _v * _dt;
}

// �莮�����Ȃ����Ȃ��Ƃ����Ȃ�.2013.1.3
void PHFemVibration::ImplicitEuler(const VMatrixRe& _MInv, const VMatrixRe& _K, const VMatrixRe& _C, 
		const VVectord& _f, const double& _dt, VVectord& _xd, VVectord& _v){
#if 0
	// �O�͂��܂��g�ݍ���łȂ�
	int NDof = GetPHFemMesh()->vertices.size() * 3;
	VMatrixRe E;	// �P�ʍs��
	E.resize(NDof, NDof);
	E.clear(0.0);
	for(int i = 0; i < NDof; i++){
		E[i][i] = 1.0;
	}
	VMatrixRe _Kt;
	_Kt.resize(NDof, NDof);
	_Kt.clear(0.0);	
	VMatrixRe _CtInv;
	_CtInv.resize(NDof, NDof);
	_CtInv.clear(0.0);
	VMatrixRe _DInv;
	_DInv.resize(NDof, NDof);
	_DInv.clear(0.0);

	_Kt = _MInv * _K * _dt;
	_CtInv = (E + _MInv * _C * _dt).inv();
	_DInv = (E + _CtInv * _Kt * vdt).inv();

	xdl = _DInv * (xdl + _CtInv * (vl + _Kt * xdlInit) * vdt);
	vl = _CtInv * (vl - _Kt * (xdl - xdlInit));
	//DSTR << "Integrate" << std::endl;
	//DSTR << "_K" << std::endl;
	//DSTR << _K << std::endl;
	//DSTR << "_CInv" << std::endl;
	//DSTR << _CInv << std::endl;
	//DSTR << "_DInv" << std::endl;
	//DSTR << _DInv << std::endl;
#endif
}

void PHFemVibration::NewmarkBeta(const VMatrixRe& _M, const VMatrixRe& _K, const VMatrixRe& _C, 
		const VVectord& _f, const double& _dt, VVectord& _xd, VVectord& _v,  const double b){
	int NDof = (int)_xd.size();
	static VVectord _a;		// ����̉����x
	_a.resize(NDof, 0.0);
	static VVectord _al;	// �O��̉����x
	_al.resize(NDof, 0.0);

	VMatrixRe _SInv;
	_SInv.resize(NDof, NDof, 0.0);
	_SInv = (_M + (0.5 * _C * vdt) + (b * _K * pow(vdt, 2))).inv();

	VVectord _Ct;
	_Ct.resize(NDof, 0.0);
	VVectord _Kt;
	_Kt.resize(NDof, 0.0);

	static bool bFirst = true;
	if(bFirst){
		_a = _M.inv() * (- 1 * ( _C * _v) - 1 * (_K * _xd) + _f);
		bFirst = false;
	}

	_Ct = _C * (_v + (0.5 * _a * _dt));
	_Kt = _K * (_xd + (_v * _dt) + (_a * pow(_dt, 2) * (0.5 - b)));
	_al = _a;
	_a = _SInv * (_f - _Ct - _Kt);
	_v += 0.5 * (_al + _a) * _dt;
	_xd += (_v * _dt) + ((0.5 - b) * _al * pow(_dt, 2)) + (_a * b * pow(_dt, 2));
	//DSTR << "Ct" << std::endl;		DSTR << Ct << std::endl;
	//DSTR << "Kt" << std::endl;		DSTR << Kt << std::endl;
	//DSTR << "alocal" << std::endl;	DSTR << alocal << std::endl;
	//DSTR << "_MInv" << std::endl;	DSTR << _MInv << std::endl;
}

void PHFemVibration::ModalAnalysis(const VMatrixRe& _M, const VMatrixRe& _K, const VMatrixRe& _C, 
		const VVectord& _f, const double& _dt, VVectord& _xd, VVectord& _v, const int nmode){
	//DSTR << "//////////////////////////////////" << std::endl;
	// �܂��������܂�ł��Ȃ�
	// n:���R�x�Am:���[�h����
	static double time = 0.0;		// �o�ߎ���
	static VVectord evalue;			// �ŗL�U����(m)
	static VMatrixRe evector;		// �ŗL�x�N�g��(n*m)
	static VVectord w;				// �ŗL�p�U����(m)
	static VVectord q0;				// ���[�h�U���x�N�g���̏����l(m)
	static VVectord qv0;			// ���[�h�U�����x�x�N�g���̏����l(m)
	static VMatrixRe fC;			// ���[�h�O�͂̐ϕ�cos����(0��:����, 1��O��)(m)
	static VMatrixRe fS;			// ���[�h�O�͂̐ϕ�sin����(0��:����, 1��O��)(m)

	static bool bFirst = true;
	if(bFirst){
		evalue.resize(nmode, 0.0);
		evector.resize(_M.height(), nmode, 0.0);
		SubSpace(_M, _K, nmode, 1e-10, evalue, evector);
		DSTR << "eigenvalue" << std::endl;
		DSTR << evalue << std::endl;
		DSTR << "eigenvector" << std::endl;
		DSTR << evector << std::endl;

		// �����ψʁA���x�����[�h���W�n�ɂ���
		q0.resize(nmode, 0.0);
		q0 = evector.trans() * _M * _xd;
		qv0.resize(nmode, 0.0);
		qv0 = evector.trans() * _M * _v / _dt;
		fC.resize(nmode, 2, 0.0);
		fS.resize(nmode, 2, 0.0);
		w.resize(evalue.size());
		for(int i = 0; i < (int)w.size(); i++){
			w[i] = sqrt(evalue[i]);	// �ŗL�p�U����
		}
		bFirst = false;
	}

	// �ϕ�
	VVectord q;		// �X�V��̃��[�h�U���x�N�g��(m)
	q.resize(nmode, 0.0);
	VVectord qv;	// �X�V��̃��[�h�U�����x�x�N�g��(m)
	qv.resize(nmode, 0.0);
	VVectord fM;	// ���[�h�O��(m)
	fM.resize(nmode, 0.0);
	fM =  evector.trans() * _f;

	for(int i = 0; i < nmode; i++){
		double wt = w[i] * time;
		double ftemp = fM[i] * cos(wt);
		fC.item(i, 0) += 0.5 * (ftemp + fC.item(i, 1)) * _dt;
		fC.item(i, 1) = ftemp;
		ftemp = fM[i] * sin(wt);
		fS.item(i, 0) += 0.5 * (ftemp + fS.item(i, 1)) * _dt;
		fS.item(i, 1) = ftemp;
		q[i] = q0[i] * cos(wt) + qv0[i] * sin(wt) / w[i] + (fC.item(i, 0) * sin(wt) - fS.item(i, 0) * cos(wt))/ w[i];
	}
	//DSTR << _f << std::endl;
	//DSTR << fM << std::endl;
	//DSTR << fC << std::endl;
	//DSTR << fS << std::endl;
	//DSTR << time << std::endl;

	_xd = evector * q;
	time += _dt;
}

//* �v�Z�֐��i���̂���TMatirix�ցj
/////////////////////////////////////////////////////////////////////////////////////////
/// �T�u�X�y�[�X�@�i�����t�����@�H�j
/// VMatrixRe& _M:���ʍs��i���l�Ώ́j, VMatrixRe& _K:�����s��i���l�Ώ́j
/// int nmode:���[�h����, double epsilon:��������, VVectord& e:�ŗL�U����[Hz], VMatrixRe& v:�ŗL�x�N�g��(�񏇁j
void PHFemVibration::SubSpace(const VMatrixRe& _M, const VMatrixRe& _K, 
	const int nmode, const double epsilon, VVectord& evalue, VMatrixRe& evector){
	const int size = _M.height();
	if(_K.det() <= 0){
		DSTR << "Stiffness Matrix is not regular matrix." << std::endl;
		return;
	}
	if(nmode > size) assert(0);
	/// ������
	evalue.resize(nmode, 0.0);			// �ŗL�l
	evector.resize(size, nmode, 0.0);	// �ŗL�x�N�g��
	// �����l�x�N�g��
	VMatrixRe y;
	y.resize(size, nmode, 0.0);
	VMatrixRe ylast;
	ylast.resize(size, nmode, 0.0);
	VVectord yini;
	yini.resize(size, 1.0);		// �����l��1.0
	yini.unitize();
	for(int i = 0; i < nmode; i++){
		y.col(i) = yini;
		ylast.col(i) = yini;
	}	

	/// _M, _K���R���X�L�[����
	// _AInv�̌v�Z�̓R���X�L�[����l���g����for���Ōv�Z�����ق��������͂��B
	// ���͑������C�ɂ����t�s����v�Z���Ă�B
	VMatrixRe _Mc;
	_Mc.resize(size, size, 0.0);
	cholesky(_M, _Mc);	
	VMatrixRe _Kc;
	_Kc.resize(size, size, 0.0);
	cholesky(_K, _Kc);
	//DSTR << "cholesky" << std::endl;
	//DSTR << _Mc << std::endl;
	//DSTR << _Kc  << std::endl;

	VMatrixRe _AInv;			
	_AInv.resize(size, size, 0.0);
	_AInv = _Mc.trans() * (_Kc.inv()).trans() * _Kc.inv() * _Mc;

	/// �����v�Z
	for(int k = 0; k < nmode; k++){
		VVectord z;
		z.resize(size, 0.0);
		int cnt = 0;
		while(1){
			// z�̌v�Z
			z = _AInv * y.col(k);	 
			// �O�����E�V���~�b�g�̒�����
			VVectord c;
			c.resize(size, 0.0);
			for(int i = 0; i < k; i++){
				double a = y.col(i) * z;
				c += a * y.col(i);
			}
			y.col(k) = z - c;
			y.col(k).unitize();

			double error = 0.0;
			error = sqrt((ylast.col(k) - y.col(k)) * (ylast.col(k) - y.col(k)));
			ylast.col(k) = y.col(k);
			if(abs(error) < epsilon) break;
			cnt++;
			if(cnt > 1e5){
				DSTR << "Can not converge in subspace" << std::endl;
				break;
			}
		}
		evector.col(k) = _Mc.trans().inv() * y.col(k);		// �ŗL�x�N�g��
		evalue[k] = 1.0 / (y.col(k) * _AInv * y.col(k));	// �ŗL�l
	}
}

//* �e��ݒ�֐�
/////////////////////////////////////////////////////////////////////////////////////////
void PHFemVibration::SetIntegrationMode(PHFemVibrationDesc::INTEGRATION_MODE mode){
	integration_mode = mode;
}

void PHFemVibration::GetVerticesDisplacement(VVectord& _xd){
	/// FemVertex����ψʂ�����Ă���
	// u = (u0, v0, w0, ...., un-1, vn-1, wn-1)�̏�
	int NVer = NVertices();
	for(int i = 0; i < NVer; i++){
		int id = i * 3;
		Vec3d disp = GetPHFemMesh()->GetVertexDisplacementL(i);
		_xd[id] = disp.x;
		_xd[id + 1] = disp.y;		
		_xd[id + 2] = disp.z;
	}
}

void PHFemVibration::UpdateVerticesPosition(VVectord& _xd){
	/// �v�Z���ʂ�FemVertex�ɖ߂�
	// u = (u0, v0, w0, ...., un-1, vn-1, wn-1)�̏�
	int NVer = NVertices();
	PHFemMeshNew* mesh = GetPHFemMesh();
	for(int i = 0; i < NVer; i++){
		int id = i * 3;
		Vec3d initialPos = mesh->GetVertexInitPositionL(i);
		GetPHFemMesh()->vertices[i].pos.x = _xd[id] + initialPos.x;
		GetPHFemMesh()->vertices[i].pos.y = _xd[id + 1] + initialPos.y;		
		GetPHFemMesh()->vertices[i].pos.z = _xd[id + 2] + initialPos.z;
	}
}

bool PHFemVibration::AddBoundaryCondition(VMatrixRe& mat, const int id){
	int n = mat.height();
	if(id > n - 1) return false;
	// base, mat�̗����ɋ��E�����������
	mat.col(id).clear(0.0);
	mat.row(id).clear(0.0);
	mat.item(id, id) = 1.0;
	return true;
}

bool PHFemVibration::AddBoundaryCondition(const int vtxId, const Vec3i dof = Vec3i(1, 1, 1)){
	int NVer = NVertices();
	if(0 <= vtxId && vtxId <= NVer -1){
		for(int i = 0; i < 3; i++){
			if(dof[i] == 1){
				const int id = vtxId * 3 + i;
				//AddBoundaryCondition(matMp, id);
				//AddBoundaryCondition(matKp, id);
				//AddBoundaryCondition(matCp, id);
				boundary[id] = 1;
			}
		}
		return true;
	}
	return false;
}

bool PHFemVibration::AddBoundaryCondition(const VVector< Vec3i > bcs){ 
	int NVer = NVertices();
	if(NVer != bcs.size()) return false;
	for(int i = 0; i < (int)bcs.size(); i++){
		for(int j = 0; j < 3; j++){
			if(bcs[i][j] == 1){
				const int id = i + j;
				//AddBoundaryCondition(matMp, id);
				//AddBoundaryCondition(matKp, id);
				//AddBoundaryCondition(matCp, id);
				boundary[id] = 1;
			}
		}
	}
	return true;
}

void PHFemVibration::CompBoundaryMatrix(VMatrixRe& _L, VMatrixRe& _R, const VVector< int > bc){
	int n = bc.size();		// �S���R�x
	int m = 0;				// ���E����������̎��R�x
	for(int i = 0; i < n; i++){
		if(bc[i] == 0) m++;
	}
	_L.resize(m, n, 0.0);
	_R.resize(n, m, 0.0);
	int index = 0;
	for(int i = 0; i < n; i++){
		if(bc[i] == 0){
			_L[index][i] = 1.0;
			_R[i][index] = 1.0;
			index++;		
		}
	}
}
void PHFemVibration::ReduceMatrixSize(VMatrixRe& mat, const VVector< int > bc){
	VMatrixRe matL, matR;
	CompBoundaryMatrix(matL, matR, bc);
	VMatrixRe tmp;
	tmp.assign(matL * mat * matR);
	mat.assign(tmp);
}

void PHFemVibration::ReduceMatrixSize(VMatrixRe& _M, VMatrixRe& _K, VMatrixRe& _C, const VVector< int > bc){
	VMatrixRe matL, matR;
	CompBoundaryMatrix(matL, matR, bc);
	VMatrixRe tmp;
	tmp.assign(matL * _M * matR);
	_M.assign(tmp);
	tmp.assign(matL * _K * matR);
	_K.assign(tmp);
	tmp.assign(matL * _C * matR);
	_C.assign(tmp);

}

void PHFemVibration::ReduceVectorSize(VVectord& r, const VVector< int > bc){
	VMatrixRe matL, matR;
	CompBoundaryMatrix(matL, matR, bc);
	VVectord tmp;
	tmp.assign(matL * r);
	r.assign(tmp);
}

void PHFemVibration::ReduceVectorSize(VVectord& _xd, VVectord& _v, VVectord& _f,const VVector< int > bc){
	VMatrixRe matL, matR;
	CompBoundaryMatrix(matL, matR, bc);
	VVectord tmp;
	tmp.assign(matL * _xd);
	_xd.assign(tmp);
	tmp.assign(matL * _v);
	_v.assign(tmp);	
	tmp.assign(matL * _f);
	_f.assign(tmp);
}

void PHFemVibration::GainVectorSize(VVectord& r, const VVector< int > bc){
	VMatrixRe matL, matR;
	CompBoundaryMatrix(matL, matR, bc);
	VVectord tmp;
	tmp.assign(matR * r);
	r.assign(tmp);
}

void PHFemVibration::GainVectorSize(VVectord& _xd, VVectord& _v, const VVector< int > bc){
	VMatrixRe matL, matR;
	CompBoundaryMatrix(matL, matR, bc);
	VVectord tmp;
	tmp.assign(matR * _xd);
	_xd.assign(tmp);
	tmp.assign(matR * _v);
	_v.assign(tmp);
}

bool PHFemVibration::AddVertexForce(int vtxId, Vec3d fW){
	if(0 <= vtxId && vtxId <= NVertices() -1){
		Vec3d fL = GetPHFemMesh()->GetPHSolid()->GetPose() * fW;
		int id = vtxId * 3;
		fl[id] += fL.x;
		fl[id + 1] += fL.y;
		fl[id + 2] += fL.z;
		return true;
	}
	return false;
}

bool PHFemVibration::AddVertexForce(VVector< Vec3d > fWs){
	if(NVertices() != fWs.size()) return false;
	for(int i = 0; i < (int)fWs.size(); i++){
		Vec3d fL = GetPHFemMesh()->GetPHSolid()->GetPose().Inv() * fWs[i];
		int id = i * 3;
		fl[id] += fL.x;
		fl[id + 1] += fL.y;
		fl[id + 2] += fL.z;
	}
	return true;
}

//* scilab�f�o�b�N
/////////////////////////////////////////////////////////////////////////////////////////

void PHFemVibration::ScilabEigenValueAnalysis(VMatrixRe& _M, VMatrixRe& _K){
	DSTR << "////////////////////////////////////////////////////////////////////////////////////////" << std::endl;
	DSTR << "ScilabEigenValueAnalysis Start." << std::endl;
	DSTR << "det(M) springhead2 : " << _M.det() << std::endl;
	DSTR << "det(K) springhead2 : " << _K.det() << std::endl;
#if 1
	VVectord evalue;
	VMatrixRe evector;
	SubSpace(_M, _K, _M.height(), 1e-5, evalue, evector);
	DSTR << "eigenvalue springhead2" << std::endl;
	DSTR << evalue << std::endl;
	DSTR << "eigen vibration value springhead2" << std::endl;
	for(int i = 0; i < (int)evalue.size(); i++){
		evalue[i] = sqrt(evalue[i])/2/M_PI;
	}
	DSTR << evalue << std::endl;
	DSTR << "eigenvector springhead2" << std::endl;
	DSTR << evector << std::endl;
#endif
	if(!IsScilabStarted){
		DSTR << "Scilab has not started" << std::endl;
		return;
	}
	ScilabJob("clear;");
	ScilabSetMatrix("M", _M);
	ScilabJob("detM = det(M);");
	std::cout << "det(M) scilab : ";
	ScilabJob("disp(detM);");	
	ScilabSetMatrix("K", _K);
	ScilabJob("detK = det(K);");
	std::cout << "det(K) scilab : ";
	ScilabJob("disp(detK);");

	ScilabJob("[P D] = spec(inv(M) * K);");
	VMatrixRe D;
	ScilabGetMatrix(D, "D");
	DSTR << "eigenvalue scilab" << std::endl;
	DSTR << D << std::endl;
	VMatrixRe P;
	ScilabGetMatrix(P, "P");
	DSTR << "eigenvector scilab" << std::endl;
	DSTR << P << std::endl;
	DSTR << "ScilabEigenValueAnalysis End." << std::endl;
	DSTR << "////////////////////////////////////////////////////////////////////////////////////////" << std::endl;
}

void PHFemVibration::MatrixFileOut(VMatrixRe mat, std::string filename){
	std::ofstream ofs(filename);
	if (!ofs){
		DSTR << "Can not open the file : " << filename << std::endl;
		return;
	}

	ofs.precision(element_limit);
	for (int i = 0; i < (int)mat.height(); i++){
		for(int j = 0; j < (int)mat.width(); j ++){
			ofs << mat.item(i, j) << ",";
		}
		ofs << std::endl;
    }
    ofs.close();
}


}