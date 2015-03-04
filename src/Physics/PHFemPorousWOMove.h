/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PH_FEM_WOMOVE_H
#define PH_FEM_WOMOVE_H

#include "PHFemBase.h"
#include "../Foundation/Object.h"
#include "PhysicsDecl.hpp"

#define constRateDrying 0
#define fallRateDrying1st 1
#define fallRateDrying2nd 2

namespace Spr{;

class PHFemPorousWOMove: public PHFemPorousWOMoveDesc, public PHFemBase {
	SPR_OBJECTDEF(PHFemPorousWOMove);
	SPR_DECLMEMBEROF_PHFemPorousWOMoveDesc;

public:
	double tdt;
	
	struct StateVertex{
		double T;		//���x
		double preT;	//1step�O�̉��x
		double Tc;		//�ߓ_����̗��̉��x
		double rhoWInit;//���̋�Ԗ��x�̏����l
		double rhoOInit;//�ܖ����̏����l
		double rhoW;	//���̋�Ԗ��x
		double rhoO;	//���̋�Ԗ��x
		double Pc;		//�ъǃ|�e���V����
		double muW;		//���̔S�x
		double muO;		//���̔S�x
		double vVolume;	//���_���x�z����̐�
		double mw;		//���_�������̎���
		double mo;		//���_�������̎���
		double porosity;//�Ԍ���
		double saturation;//�O�a��
		double rhoS;		//�Ō`���̖��x
		double rhoS0;		//�Ō`���̖��x�̏����l
		double preRhoS;		//1step�O�̌Ō`���̖��x
		double outflowWater;	//���o���Ă��鐅����[kg]
		double outflowOil;		//���o���Ă��������[kg]
		Vec3d normal;
		bool denaturated;	//�ϐ��������ǂ���
		double boundWaterMass;
	};

	struct StateEdge{
		double ww;
		double ow;
		double wo;
		double oo;
		double c;
		double b;
	};

	struct StateFace{
		int dryingStep;			//�����̒i�K
		double area;			//�l�ʑ̂̊e�ʂ̖ʐ�
		double surroundFlux;	//����̗��̂̑��x
		double surroundHu;		//����̎��x
		double vaporPress;		//����̐����C��
		double vaporCont;		//����̐����C��(g/m^3)
		double maxVaporPress;	//�\�ʉ��x�ł̖O�a�����C��
		double maxVaporCont;	//�\�ʉ��x�ł̖O�a�����C��(g/m^3)
		double K1;				//������1�i�������ł̔��萔
		bool evapoRateUpdated;	//�������x�萔���ω������Ƃ���true
		bool deformed;			//	�����钸�_�̈ړ��ɂ��A�ό`���ꂽ�Ƃ�
		Vec3d normal;
		Vec3d normal_origin;	//�@���̎n�_
	};

	struct StateTet{
		double volume;
		double tetPorosity;	//�l�ʑ̂̊Ԍ����F4���_�̊Ԍ����̑�������
		double preTetPorosity;
		double tetMuW;		//�l�ʑ̒��̐��̔S�x�F4���_�̐��̔S�x�̑�������
		double tetMuO;		//�l�ʑ̒��̖��̔S�x�F4���_�̖��̔S�x�̑�������
		double preTetMuW;	//1step�O�̐��̔S�x
		double preTetMuO;	//1step�O�̖��̔S�x
		double rhoS;		//�Ō`���̖��x
		double rhoS0;		//�Ō`���̖��x�̏����l
		double preRhoS;		//1step�O�̌Ō`���̖��x
		double wFlux[4];
		double oFlux[4];
		bool sDensChanged;	//�Ō`���̖��x�ω��t���O

		PTM::TMatrixRow<4,4,double> matWw;
		PTM::TMatrixRow<4,4,double> matOw;
		PTM::TMatrixRow<4,4,double> matWo;
		PTM::TMatrixRow<4,4,double> matOo;
		PTM::TMatrixRow<4,4,double> matC;
		PTM::TMatrixRow<4,4,double> matPcw;
		PTM::TMatrixRow<4,4,double> matPco;
		PTM::TVector<4,double> vecFw[2];
		PTM::TVector<4,double> vecFo[2];
		PTM::TVector<4,double> vecPc;
	};

	std::vector<StateVertex> vertexVars;
	std::vector<StateEdge> edgeVars;
	std::vector<StateFace> faceVars;
	std::vector<StateTet> tetVars;

protected:

	//�ߓ_�ܐ����x�N�g��
	PTM::TVector<4,double> rhoWVecEle;
	PTM::VMatrixRow<double> rhoWVecAll;
	//�ߓ_�ܖ����x�N�g��
	PTM::TVector<4,double> rhoOVecEle;
	PTM::VMatrixRow<double> rhoOVecAll;
	//�ߓ_�ъǃ|�e���V�����x�N�g��
	PTM::TVector<4,double> PcEle;
	PTM::VMatrixRow<double> PcVecAll;

	//�S�̂̌W���s��
	PTM::VMatrixRow<double> matWwAll;
	PTM::VMatrixRow<double> matOwAll;
	PTM::VMatrixRow<double> matWoAll;
	PTM::VMatrixRow<double> matOoAll;
	PTM::VMatrixRow<double> matCAll;
	PTM::VMatrixRow<double> matPcwAll;
	PTM::VMatrixRow<double> matPcoAll;

	PTM::VMatrixRow<double> vecFwAll;
	PTM::VMatrixRow<double> vecFoAll;

	PTM::VMatrixRow<double> vecFwFinal;	//vecFw + vecPcw * matPc
	PTM::VMatrixRow<double> vecFoFinal;	//vecFo + vecPco * matPc
	
	//�S�̍����s��̑���
	PTM::VMatrixRow<double> dMatWwAll;
	PTM::VMatrixRow<double> dMatOwAll;
	PTM::VMatrixRow<double> dMatWoAll;
	PTM::VMatrixRow<double> dMatOoAll;
	PTM::VMatrixRow<double> dMatCAll;
	PTM::VMatrixRow<double> _dMatWAll;
	PTM::VMatrixRow<double> _dMatOAll;
	PTM::VMatrixCol<double> bwVecAll;
	PTM::VMatrixCol<double> boVecAll;

	//Km��3��4�~4�s��̓��ꕨ�@Matk1�����܂ł̊Ԃ̈ꎞ�I�ȃf�[�^�u��
	PTM::TMatrixRow<4,4,double> matk1array[4];
	//k21,k22,k23,k24��4�~4�s��̓��ꕨ�@Matk�����܂ł̊Ԃ̈ꎞ�I�ȃf�[�^�u��
	PTM::TMatrixRow<4,4,double> matk2array[4];
	//matc�쐬�p
	PTM::TMatrixRow<4,4,double> matCTemp;
	//F��4�~1�x�N�g���̓��ꕨ
	PTM::TVector<4,double> vecFarray[4];
	//�P�ʍs�� vertex���~vertex��
	PTM::VMatrixRow<double> idMat;
	//dt�p�̍s��(1�~1)
	PTM::TMatrixRow<1,1,double> dtMat;

	Vec3d gravity;	//�d�͉����x�x�N�g��

	//%%%%%%%%		�o�C�i���X�C�b�`�̐錾		%%%%%%%%//
	bool doCalc;
	bool matWOPcVecF2Changed;
	bool matCChanged;
	bool vecFChanged;
	bool matVecChanged;
	bool keisuChanged;

public:

	PHFemPorousWOMove(const PHFemPorousWOMoveDesc& desc=PHFemPorousWOMoveDesc(), SceneIf* s=NULL);
	virtual void Init();
	virtual void Step();
	void Step(double dt);

	double eps;
	//�֐��̐錾

	void InitMatWO();
	void InitMatC();
	void InitMatPc();
	void InitVecF();

	//[Ww],[Ow],[Wo],[Oo]�����
	void CreateMatWOPcVecF2Local(unsigned tetid);
	////[K1W2]�����
	//void CreateMatk2(unsigned id);
	//void CreateMatWOLocal(unsigned i);			//	edges�ɓ���A�`�F�b�N�p�̑S�̍����s����Aifdef�X�C�b�`�ō���d�l
	//void CreateMatWOPcVecF2All();

	//[C]�����
	//void CreateMatCAll();			//C�̑S�̍����s������֐�
	void CreateMatCLocal(unsigned tetid);	//C�̗v�f�����s������֐�

	//{FWw},{Fo1}�����
	void CreateVecF1Local(unsigned tetid);
	////{F2W}�����
	//void CreateVecF2(unsigned tetid);
	//void CreateVecFLocal(unsigned id);
	//void CreateVecFAll();				//	�l�ʑ̃��b�V����ID��������

	void CreateMatVecAll();

	void CreateRhoWVecAll();
	void CreateRhoOVecAll();
	void InitPcVecAll();

	void InitTcAll(double temp);							//	Tc�̉��x��������

	void SetTimeStep(double dt){ tdt = dt; }
	double GetTimeStep(){ return tdt; }


	PTM::TMatrixRow<4,4,double> Create44Mat21();	//���ʂŗp����A4�~4��2��1�łł����s���Ԃ��֐�
	PTM::TMatrixCol<4,1,double> Create41Vec1();


	PTM::VMatrixRow<double> keisuW;			//	���ږ@�Ōv�Z����Ww(t+dt)�W���s��
	PTM::VMatrixRow<double> keisuWInv;
	PTM::VMatrixRow<double> keisuO;			//	���ږ@�Ōv�Z����Wo(t+dt)�W���s��
	PTM::VMatrixRow<double> keisuOInv;
	PTM::VVector<double> uhenW;
	PTM::VVector<double> uhenO;

	PTM::VMatrixRow<double> leftKeisuW;		//�ܐ����̑Q�������ӂ�Ww(t+dt)�̌W��
	PTM::VMatrixRow<double> rightKeisuWWw;	//�ܐ����̑Q�����E�ӂ�Ww(t)�̌W��
	PTM::VMatrixRow<double> rightKeisuWWo;	//�ܐ����̑Q�����E�ӂ�Wo(t)�̌W��
	PTM::VMatrixRow<double> rightKeisuWFw;	//�ܐ����̑Q�����E�ӂ�Fw�̌W��
	PTM::VMatrixRow<double> rightKeisuWFo;	//�ܐ����̑Q�����E�ӂ�Fo�̌W��
	PTM::VMatrixRow<double> leftKeisuO;		//�ܖ����̑Q�������ӂ�Wo(t+dt)�̌W��
	PTM::VMatrixRow<double> rightKeisuOWw;	//�ܖ����̑Q�����E�ӂ�Ww(t)�̌W��
	PTM::VMatrixRow<double> rightKeisuOWo;	//�ܖ����̑Q�����E�ӂ�Wo(t)�̌W��
	PTM::VMatrixRow<double> rightKeisuOFw;	//�ܖ����̑Q�����E�ӂ�Fw�̌W��
	PTM::VMatrixRow<double> rightKeisuOFo;	//�ܖ����̑Q�����E�ӂ�Fo�̌W��


	std::ofstream matWwAllout;
	std::ofstream matOwAllout;
	std::ofstream matWoAllout;
	std::ofstream matOoAllout;
	std::ofstream matCAllout;
	std::ofstream matPcwAllout;
	std::ofstream matPcoAllout;
	std::ofstream vecFwAllout;
	std::ofstream vecFoAllout;
	std::ofstream checkRhoWVecAllout;
	std::ofstream checkRhoOVecAllout;
	std::ofstream FEMLOG;
	std::ofstream keisuWLog;
	std::ofstream keisuOLog;
	std::ofstream invCheck;
	std::ofstream wAll;
	std::ofstream oAll;
	std::ofstream SAll;
	std::ofstream topS;
	std::ofstream sideS;
	std::ofstream bottomS;
	std::ofstream internalS;
	std::ofstream topOutflowWater;
	std::ofstream topOutflowOil;
	std::ofstream sideOutflowWater;
	std::ofstream sideOutflowOil;
	std::ofstream bottomOutflowWater;
	std::ofstream bottomOutflowOil;
	std::ofstream PcAll;
	std::ofstream tempAll;
	std::ofstream paramout;
	std::ofstream tempAndWater;
	unsigned long long COUNT;
	
	unsigned long StepCount;			//	Step���J�E���g�A�b�v�̉��Z�W��	
	unsigned long StepCount_;			//	Step���J�E���g�A�b�v�̉��Z�W��	StepCount�����T�ڂ���\���J�E���g 

	/// dt��萔�{����
	unsigned Ndt;
	double dNdt;

	double initSolidMass;

protected:

	double CalcTriangleArea(int id0, int id1, int id2);
	double CalcTetrahedraVolume(FemTet tet); 		// �l�ʑ̂�ID������ƁA���̑̐ς��v�Z���Ă����֐�
	void CalcWOContentDirect(double dt, double eps);			// dt:�X�e�b�v����
	void CalcWOContentDirect2(double dt, double eps);
	void CalcWOContentUsingGaussSeidel(unsigned NofCyc, double dt, double eps);
	void CalcWOContentUsingScilab(double dt);
	void SetRhoWAllToRhoWVecAll();			//WwVecAll�ɑS�ړ_�̊ܐ�����ݒ�
	void SetRhoOAllToRhoOVecAll();			//WoVecAll�ɑS�ړ_�̊ܖ�����ݒ�
	void InitAllVertexRhoW();
	void InitAllVertexRhoO();

public:

	void UpdateVertexRhoWAll();
	void UpdateVertexRhoOAll();

	//���_�����ϐ��փA�N�Z�X
	void SetVertexTemp(unsigned vtxid, double temp);
	void SetVertexTc(unsigned vtxid, double tc);
	void SetVertexRhoW(unsigned vtxid, double rhow);
	void SetVertexRhoWInit(unsigned vtxid, double rhowInit);
	void SetVertexRhoO(unsigned vtxid, double rhoo);
	void SetVertexRhoOInit(unsigned vtxid, double rhooInit);
	void SetVertexMw(unsigned vtxid, double mw);
	void SetVertexMo(unsigned vtxid, double mo);
	void CalcVertexWOMu(unsigned vtxid);
	void CalcVertexVolume(unsigned vtxid);
	void CalcVertexMwo(unsigned vtxid);
	void CalcVertexSaturation(unsigned vtxid);
	void CalcVertexPc(unsigned vtxid);
	void CalcRhoWVecFromVertexMw(unsigned vtxid);
	double GetVertexTemp(unsigned vtxid);
	double GetVertexTc(unsigned vtxid);
	double GetVertexRhoW(unsigned vtxid);
	double GetVertexRhoWInit(unsigned vtxid);
	double GetVertexRhoO(unsigned vtxid);
	double GetVertexRhoOInit(unsigned vtxid);
	double GetVertexMw(unsigned vtxid);
	double GetVertexMo(unsigned vtxid);
	
	//�ʂ����ϐ��փA�N�Z�X
	void CalcFaceArea(unsigned faceid);
	void SetFaceEvaporationRate(unsigned faceid, double evapoRate);
	void SetFaceSurroundFlux(unsigned faceid, double surroundFlux);
	void SetFaceVaporPress(unsigned faceid, double vaporPress);
	void CalcFaceMaxVaporPress(unsigned faceid);
	void CalcFaceMaxVaporCont(unsigned faceid);
	double GetFaceArea(unsigned faceid);
	double GetFaceEvaporationRate(unsigned faceid);
	double GetFaceSurroundFlux(unsigned faceid);
	double GetFaceVaporPress(unsigned faceid);
	double GetFaceMaxVaporPress(unsigned faceid);

	//�l�ʑ̂����ϐ��փA�N�Z�X
	void CalcTetVolume(unsigned tetid);
	void CalcTetPorosity(unsigned tetid);
	void CalcTetWOMu(unsigned tetid);
	void CalcTetRhoS(unsigned tetid);
	void SetTetSolidDensity(unsigned tetid, double rhos);
	void SetTetContractionRate(unsigned tetid, double contRate);
	double GetTetVolume(unsigned tetid);
	double GetTetSolidDensity(unsigned tetid);

	PTM::VMatrixRow<double> inv(PTM::VMatrixRow<double> mat);
	PTM::VMatrixRow<double> inv2(PTM::VMatrixRow<double> mat);

	double GetVtxWaterInTets(Vec3d temppos);
	double GetVtxOilInTets(Vec3d temppos);
	double CalcWaterInnerTets(unsigned id,PTM::TVector<4,double> N);
	double CalcOilInnerTets(unsigned id,PTM::TVector<4,double> N);

	float calcGvtx(std::string fwfood, int pv, unsigned texture_mode);
	
	void matWwOut();
	void matWoOut();
	void matOwOut();
	void matOoOut();
	void matPcwOut();
	void matPcoOut();
	void matCOut();
	void vecFwAllOut();
	void vecFoAllOut();
	void vecFwFinalOut();
	void vecFoFinalOut();
	void vecPcAllOut();

	std::vector<unsigned> topVertices;
	std::vector<unsigned> sideVertices;
	std::vector<unsigned> bottomVertices;
	std::vector<unsigned> internalVertices;

	void setGravity(Vec3d g){gravity = g;}
	void vertexDenatProcess(unsigned vtxid);
	void tetDenatProcess(unsigned vtxid);
	void outflowOverSaturation(unsigned vtxid);

	double decideWetValue(unsigned vtxid);
	double GetVtxSaturation(unsigned vtxid){return vertexVars[vtxid].saturation;}

	void decrhoW(double dec);
};
}	//	namespace Spr

#endif