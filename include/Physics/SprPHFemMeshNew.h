/*
 *  Copyright (c) 2003-2012, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 *	@file SprPHFemMeshNew.h
 *	@brief ����
*/
#ifndef SPR_PH_FEMMESH_NEW_IF_H
#define SPR_PH_FEMMESH_NEW_IF_H

#include <Foundation/SprObject.h>

/**	\addtogroup gpPhysics	*/
//@{
namespace Spr{;

struct PHSolidIf;
struct PHFemVibrationIf;
struct PHFemThermoIf;
//struct PHFemWOMoveIf;

///	FemMesh�̃X�e�[�g
struct PHFemMeshNewState{};
///	FemMesh�̃f�B�X�N���v�^
struct PHFemMeshNewDesc: public PHFemMeshNewState{
	std::vector<Vec3d> vertices;
	std::vector<int> tets;
	std::vector<int> faces;
};
/// PHFemMeshNew�ւ̃C���^�t�F�[�X
struct PHFemMeshNewIf : public SceneObjectIf{
	SPR_IFDEF(PHFemMeshNew);
	void SetPHSolid(PHSolidIf* s);
	PHSolidIf* GetPHSolid();
	PHFemVibrationIf* GetPHFemVibration();
	PHFemThermoIf* GetPHFemThermo();
	//PHFemWOMoveIf* GetPHFemWOMove();
	int NVertices();
	int NFaces();
	double CompTetVolume(int tetID, bool bDeform);
	bool AddVertexDisplacementW(int vtxId, Vec3d disW);
	bool AddVertexDisplacementL(int vtxId, Vec3d disL);
	bool SetVertexPositionW(int vtxId, Vec3d posW);
	bool SetVertexPositionL(int vtxId, Vec3d posL);
};

/// Fem�̃f�X�N���v�^
struct PHFemBaseDesc{};
/// Fem�̋��ʌv�Z����
struct PHFemBaseIf : public SceneObjectIf{
	SPR_IFDEF(PHFemBase);
};

/// �U���v�Z�̃f�X�N���v�^
struct PHFemVibrationDesc : public PHFemBaseDesc{
	enum ANALYSIS_MODE{
		ANALYSIS_DIRECT,
		ANALYSIS_MODAL
	};
	enum INTEGRATION_MODE{
		INT_EXPLICIT_EULER,
		INT_IMPLICIT_EULER,
		INT_SIMPLECTIC,
		INT_NEWMARK_BETA,
	};
	double young;		///< �����O��(Pa, N/m2)
	double poisson;		///< �|�A�\����(*��l�ȗ����̂̏ꍇ�A-1 <= v <= 0.5)
	double density;		///< ���x(kg/m3)
	double alpha;		///< �S��������
	double beta;		///< �\��������
	std::vector<int> fixedVertices;	///< �Œ蒸�_��IDs
	PHFemVibrationDesc();
};

/// �U���v�Z
struct PHFemVibrationIf : public PHFemBaseIf{
	SPR_IFDEF(PHFemVibration);
	void SetTimeStep(double dt);
	double GetTimeStep();
	void SetYoungModulus(double value);
	double GetYoungModulus();
	void SetPoissonsRatio(double value);
	double GetPoissonsRatio();
	void SetDensity(double value);
	double GetDensity();
	void SetAlpha(double value);
	double GetAlpha();
	void SetBeta(double value);
	double GetBeta();
	void SetBoundary(int vtxIds);
	std::vector<int>  GetBoundary();
	void SetAnalysisMode(PHFemVibrationDesc::ANALYSIS_MODE mode);
	void SetIntegrationMode(PHFemVibrationDesc::INTEGRATION_MODE mode);
	bool AddBoundaryCondition(int vtxId, Vec3i dof);
	void DeleteBoundaryCondition();
#ifndef SWIG


	// ���E������������(���_���jAdd the voudnary conditions (vertex order)
	bool AddBoundaryCondition(std::vector< Vec3i >& bcs); 
	// ���_�ɗ͂�������i���[���h���W�n�j  Applying force to the vertex (world coordinate system)
	bool AddVertexForceW(int vtxId, Vec3d fW);
	// ���_�Q�ɗ͂�������i���[���h���W�n�j Applying force to a group of vertices (world coordinate system)
	bool AddVertexForceW(std::vector< Vec3d >& fWs);
#endif
	void SetbRecomp();	
	void Init();
};

///FemThermo�̃f�X�N���v�^
struct PHFemThermoDesc: public PHFemBaseDesc{
	double rho;						//	���x
	double thConduct;				//�M�`����
	double heatTrans;				//�M�`�B��			//class �ߓ_�ɂ́AheatTransRatio�����݂���
	double specificHeat;			//��M
	PHFemThermoDesc();
	void Init();
};

///	���x��FEM�p�̃��b�V��
struct PHFemThermoIf : public PHFemBaseIf{
	SPR_IFDEF(PHFemThermo);
	void SetTimeStep(double dt);
	double GetTimeStep();
	int GetSurfaceVertex(int id);
	int NSurfaceVertices();
	void SetVertexTc(int id,double temp);
	void SetVertexTc(int id,double temp,double heatTrans);
	Vec3d GetPose(int id);
	Vec3d GetSufVtxPose(unsigned id);
	unsigned long GetStepCount();					///	�J�E���g�P
	unsigned long GetStepCountCyc();				///	�J�E���g�P�����T�ڂ�	�v�Z��:TotalCount = GetStepCount() + GetStepCountCyc() * (1000 * 1000 * 1000) 
	double GetVertexTemp(unsigned id);				// ���b�V���ߓ_�̉��x���擾
	double GetSufVertexTemp(unsigned id);			// ���b�V���\�ʂ̐ߓ_���x���擾
	double GetVertexHeatTransRatio(int vtxId);
	void SetVertexHeatTransRatio(int vexId, double heatTransRatio);
	void SetVertexTemp(unsigned id,double temp);
	void SetVerticesTempAll(double temp);
	void AddvecFAll(unsigned id,double dqdt);		//�Z�b�g���ƁA�l����������ւ��������Ȗ��O�Ŋ댯�B���ۂɂ́Aadd���́AIH���M�x�N�g���݂̂�Set����B�x�N�g����Set����֐�������āA���[�h���Ă������Ǝv���B
	void SetvecFAll(unsigned id,double dqdt);		//FAll�̐����ɉ��Z�����A�댯
	void SetRhoSpheat(double rho,double Cp);		//�f�ތŗL�̕���
	//int NFaces();
	//std::vector<Vec3d> GetFaceEdgeVtx(unsigned id);
	//Vec3d GetFaceEdgeVtx(unsigned id, unsigned	 vtx);
	Vec2d GetIHbandDrawVtx();
	void CalcIHdqdt_atleast(double r,double R,double dqdtAll,unsigned num);
	void UpdateIHheat(unsigned heating);	//	IH���M��Ԃ̍X�V
	void UpdateVecF();						//	����M���̂̔M�������Z�b�g
	void UpdateVecF_frypan();				//	����M���̂̔M�������Z�b�g
	void DecrMoist();						//	
	void DecrMoist_velo(double vel);						//
	void DecrMoist_vel(double dt);						//
	void InitAllVertexTemp();
	void SetInitThermoConductionParam(
		double thConduct,		// thConduct:�M�`����
		double rho,				// roh:���x
		double specificHeat,	// specificHeat:��M J/ (K�Ekg):1960
		double heatTrans		// heatTrans:�M�`�B�� W/(m^2�EK)
		);
	void SetParamAndReCreateMatrix(double thConduct0,double roh0,double specificHeat0);
	void InitVecFAlls();
	double GetVtxTempInTets(Vec3d temppos);
	void UpdateVertexTempAll();		//	�v�Z���ʂƂ��Ẳ��x��TVecAll����S�ߓ_�ɍX�V����
	float calcGvtx(std::string fwfood, int pv, unsigned texture_mode);

};

/*
struct PHFemWOMoveDesc: public PHFemBaseDesc{
	double wDiff;	//�H�i���ł̐����̊g�U�W��(m^2/s)
	double wDiffAir; //��C���ł̐����̊g�U�W��(m^2/s)
	double wDens;	//���̖��x(g/m^3)
	double oDens;	//���̖��x(g/m^3)
	double wwInit;	//�ܐ����̏����l
	double woInit;	//�ܖ����̏����l
	double evapoRate;	//�������x�萔
	double wTrans;	//���̓��ߌW��(g/m)
	double oTrans;	//���̓��ߌW��
	double denatTemp;	//�ϐ����x	���_�����̉��x�ɒB����ƁA���������ܐ����x�N�g���ɉ�����
	double boundWaterRatio;	//�������̊��� 0.15�`0.25
	double equilWaterCont;	//������1�i�������̕��t�ܐ���
	double limitWaterCont;	//���E������ �P���������ƌ�����1�i��������臒l
	double boundaryThick;	//�����̌���
	PHFemWOMoveDesc();
	void Init();
};

struct PHFemWOMoveIf: public PHFemBaseIf{
	SPR_IFDEF(PHFemWOMove);
	void SetTimeStep(double dt);
	double GetTimeStep();
	void UpdateVertexWwAll();
	void UpdateVertexWoAll();
};
*/

//@}

}	//	namespace Spr
#endif