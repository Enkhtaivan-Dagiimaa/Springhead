/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PH_FEM_THERMO_H
#define PH_FEM_THERMO_H

#include "PHFemBase.h"
#include "../Foundation/Object.h"
#include "PhysicsDecl.hpp"

namespace Spr{;

///	�M�`��FEM�v�Z�N���X
class PHFemThermo: public PHFemThermoDesc, public PHFemBase{
	SPR_OBJECTDEF(PHFemThermo);
	//ACCESS_DESC(PHFemThermo);
	SPR_DECLMEMBEROF_PHFemThermoDesc;

public:
	//	�����o�ϐ��錾
	double tdt;
	//	���_
	struct StateVertex{
		double temp;		//	���x
		double k;			//	�S�̍����s��K�̗v�f�@�v�Z�ɗ��p
		double c;			//	����
		double Tc;			//	�ߓ_���̗͂��̉��x
		double heatTransRatio;		//	���̐ߓ_�ł̔M�`�B��
		bool vtxDeformed;		//	�l�ʑ̂̕ό`�ł��̐ߓ_�����[�J�����W��ňړ��������ǂ���
		double thermalEmissivity;	///	�M���˗��@�ߓ_�ł�
		double disFromOrigin;		//>	x-z���ʂł̃��[�J�����W�̌��_����̋���
		//double heatFluxValue;		//>	�M�����l
		Vec3d normal;				// ������facce����Z�o�����@���i���݁Aface�@���̒P���a�𐳋K�������@�������A����������face�@������{������\��F�v���P
		std::vector<int> nextSurfaceVertices;
	};
	//��
	struct StateEdge{
		double k;		//�S�̍����s��K�̗v�f
		double c;
		double b;		//�K�E�X�U�C�f���v�Z�ŗp����萔b
	};
	//��
	struct StateFace{
		double area;				///	�l�ʑ̂̊e�ʂ̖ʐ�
		double iharea;				//	IH���M����face���̖ʐ�
		double ihareaRatio;			//	sigma(iharea):���b�V���̑����M�ʐ� �ɐ�߂銄��
		double ihdqdt;				//  face���󂯎��IH�R�C������̔M������ = IHdqdtAll * ihareaRatio; 	
		//>	�M�v�Z���L�̃p�����[�^�@�p�����Ďg��
		///	���_����߂�����vertices[3]����בւ������_ID
		int ascendVtx[3];			///	���MStep�Ŏg���H�g��Ȃ��H
		double heatTransRatio;		///	���̖ʂɂ�����M�`�B��		///	�\���ߓ_�̔M�`�B���̑������ς�		///	���ׂĂ̊֐��ŁA���̒l���X�V�ł��Ă��Ȃ��̂ŁA�M�p�ł��Ȃ��B
		bool alphaUpdated;			///	�����钸�_�̔M�`�B�����X�V���ꂽ�ۂ�true��	
		bool deformed;				///	�����钸�_�̈ړ��ɂ��A�ό`���ꂽ�Ƃ�
		double thermalEmissivity;	///	�M���˗�
		//double heatflux;			///	�M�����l�@�\������R�ߓ_�̑������ςŌv�Z?	��΁A���΁A���΂̏�(vector�������ق�����������)
		double heatflux[4];	// 
		double fluxarea;			//>	 �M�����̌v�Z�ɗp����Aface�̃G���A
		bool mayIHheated;				//	IH�ŉ��M����\���̂���ʁ@�i�K�I�ɍi��
		std::vector<Vec2d>	ihvtx;//[12];	//	vector�ɂ��Ȃ��Ă��A���͍ő�A�ȉ����炢�Bvector���Ɨ̈���g��������̂�.
		//Vec2d ihvtx[12];			//	IH���M�̈���ɓ���_��(x,z)���W (x,z)
		//unsigned ninnerVtx;			//	IH���M�̈���ɓ���_��(x,z)���W�̐�
		std::vector<Vec3d> shapefunc;//[12];		//	IH���M�̈���ɓ���_�̌`��֐�(f1,f2,f3)
		//face�̖@��Vec3d fnormal
		Vec3d normal;
		Vec3d normal_origin;		// �@���̎n�_
	};
	//�l�ʑ�
	struct StateTet{
		double volume;
		PTM::TMatrixRow<4,4,double> matk[3];
		//���������v�Z
		double wratio;			//	�ܐ���
		double wmass;			//	������
		double wmass_start;		//	�����ʂ̏����l
		double tetsMg;			//	����
	};

	enum HEATING_MODE{
		OFF,
		WEEK,
		MIDDLE,
		HIGH
	};

	std::vector<StateVertex> vertexVars;
	std::vector<StateEdge> edgeVars;
	std::vector<StateFace> faceVars;
	std::vector<StateTet> tetVars;

protected:

	
	//	�s��̐錾
	//PHFemThermo���݂̂ŗp����v�Z

	//%%%%%%%%		�s��̐錾�E��`		%%%%%%%%//	
	//�s��̐����@�������͗v�f�����s��A�啶���͑S�̍����s��
	//�ߓ_���x�x�N�g����	���x��	TVec:�v�f�i�c�j�x�N�g��(4x1)	TVecall:�S�́i�c�j�x�N�g��(nx1)
	//�M�`���}�g���N�X��	k:k1~k3�̉��Z��������,	k1:�̐ϕ���,	�ʐϕ���	k2:�M�`�B���E����,	k3:�M�t�ˋ��E����	�ϕ�
	//�M�e�ʃ}�g���N�X��	c:�̐ϕ���
	//�M�����x�N�g����		f:f1~f4�̉��Z��������,	�̐ϕ���	f1:�������M�ɂ�鍀,	�ʐϕ���	f2:�M�������E����,	f3:�M�`�B���E����,	f4:�M�t�ˋ��E����
	//�̐ϕ��̏ꍇ:�v�f1�ɂ�1�̍s��A�ʐϕ��̏ꍇ:�v�f���̊e�ʂ��Ƃ�1�Ōv4�̍s��Ȃ̂Ŕz��ɓ����
	//k��f�Ɋւ��ẮA�ʂ��ƂɌv�Z�����W���s����i�[����z��Mat(k/f)array���`
	//	Col:��P�ʂ̍s��	Row:�s�P�ʂ̍s��	hogeVector:�x�N�g��

	//�ߓ_���x�x�N�g��
	PTM::TVector<4,double> TVec;				///>	�v�f�̐ߓ_���x�x�N�g��		//_		//�s�v?
	PTM::VVector<double> TVecAll;				///>	�S�̂̐ߓ_���x�x�N�g��

	//�v�f�̌W���s��
//	PTM::TMatrixRow<4,4,double> matk1;			//CreateMatk1k() / k1b				///	struct Tet�ֈڐA
//	PTM::TMatrixRow<4,4,double> matk2;			//CreateMatk2()						///	
	//int Matk2array[4];						//matk2���������z��		//CreateMatk2array()
	//Km��3��4�~4�s��̓��ꕨ�@Matk1�����܂ł̊Ԃ̈ꎞ�I�ȃf�[�^�u��
	PTM::TMatrixRow<4,4,double> matk1array[4];
	//k21,k22,k23,k24��4�~4�s��̓��ꕨ�@Matk�����܂ł̊Ԃ̈ꎞ�I�ȃf�[�^�u��
	PTM::TMatrixRow<4,4,double> matk2array[4];
	//k21,k22,k23,k24��4�~4�s��̓��ꕨ�@Matk�����܂ł̊Ԃ̈ꎞ�I�ȃf�[�^�u��
	PTM::TMatrixRow<4,4,double> matk3array[4];

	//Matk=Matk1+Matk2+Matk3	matk1~3�����������v�f�����s��	CreateMatkLocal()
	PTM::TMatrixRow<4,4,double> matk;			
	PTM::TMatrixRow<4,4,double> matc;			//
//	PTM::TMatrixCol<4,1,double> Vecf3;			//f3:�O���̖ʂɖʂ��Ă���ʂ̂݌v�Z����@�v����
//	PTM::TMatrixCol<4,1,double> Vecf3array[4];	//f31,f32,f33,f34��4�~1�x�N�g���̓��ꕨ		Matk�����܂ł̊Ԃ̈ꎞ�I�ȃf�[�^�u��
//	PTM::TMatrixCol<4,1,double> Vecf;			//f1~f4�����Z�����c�x�N�g��
	
	//f3:�O���̖ʂɖʂ��Ă���ʂ̂݌v�Z����@�v����
	PTM::TVector<4,double> vecf3;				// �M�`�B���E����
	//f31,f32,f33,f34��4�~1�x�N�g���̓��ꕨ		Matk�����܂ł̊Ԃ̈ꎞ�I�ȃf�[�^�u��
	PTM::TVector<4,double> vecf3array[4];
	//f1~f4�����Z�����c�x�N�g��
	PTM::TVector<4,double> vecf;				// �M
	PTM::TVector<4,double> vecf2;				// �M�������E����
	PTM::TVector<4,double> vecf2array[4];		//
//	PTM::VVector<double> Vechoge;
	//	�ϐ��͏������@�֐��͑啶��

	//�s��k�̌v�Z�p�̌W���s��
	PTM::TMatrixRow<3,3,double> mata;
	PTM::TMatrixRow<3,3,double> matb;
	PTM::TMatrixRow<3,3,double> matcc;
	PTM::TMatrixRow<3,3,double> matd;


	// �S�̂̌W���s��	//SciLab�Ŏg�p
	// ..[K]�̑S�̍����s��		//CreateMatKall()
	PTM::VMatrixRow<double> matKAll;
	// ..[C]
	PTM::VMatrixRow<double> matCAll;

	PTM::VMatrixRow<double> tempMat;

	PTM::VMatrixRow<double> keisu;			//	���ږ@�Ōv�Z����T(t+dt)�W���s��
	PTM::VMatrixRow<double> keisuInv;

	PTM::VVector<double> TVecAll2;




	// �S�̂̍����s��̑���
	// ..�S�̍����s��K�̑Ίp�����ɂȂ�͂��̒l�����̂܂ܑ��		�������ł̏������̎d��	DMatKAll.resize(1,vertices.size());
	PTM::VMatrixRow<double> dMatKAll;
	// ..�S�̍����s��C�̑Ίp����
	PTM::VMatrixRow<double> dMatCAll;
	// ..�S�̍����s��K��C�̑Ίp�����̒萔�{�a�̋t�����Ƃ�������	�K�E�X�U�C�f���̌v�Z�ɗ��p����
	PTM::VMatrixRow<double> _dMatAll;
	// ..�K�E�X�U�C�f���̌v�Z�ɗp����萔�s��b�̏c�x�N�g��	Row�ł���K�v�͂���̂��H��Col�ɂ���
	PTM::VMatrixCol<double> bVecAll;
	PTM::VMatrixCol<double> bVecAll_IH;
		//double *constb;								//�K�E�X�U�C�f���̌W��b������z��̃|�C���^	��ŏ�芷����
	//	..{F}�� �S�̍����s��(�x�N�g��)
	PTM::VVector<double> vecFAllSum;			//	PTM::VMatrixCol<double> vecFAll;	// ����ύX				
	PTM::VVector<double> vecFAll[4];			// �S�̍����x�N�g�����i�[���Ă����x�N�g��
	PTM::VMatrixCol<double> vecFAll_f2IHw;		// ��΃x�N�g��
	PTM::VMatrixCol<double> vecFAll_f2IHm;		// ����
	PTM::VMatrixCol<double> vecFAll_f2IHs;		// ����
	PTM::VMatrixCol<double> vecFAll_f2IH[4];		// �S�̍����x�N�g������΁A���΁A���΂̏��ɓ����z��
	PTM::VMatrixCol<double> vecFAll_f3;			// f3

	// �\���p	//�f�o�b�O
	Vec2d IHLineVtxX;		//	IH���M�̋��E��������	x���̍ŏ��n�A�ő�l���i�[

public:
	//%%%		�֐��̐錾		%%%%%%%%//
	//%%%%%%		�M�`���v�Z�{��		%%%//
	//	�M�`�B���E�����̎��͂��ׂĂ̈����𖞂����K�v������B
	//	���x�Œ苫�E������p�������Ƃ��ɂ́A�M�`�B���i�Ō�̈����j����͂��Ȃ��B�܂��A��Step���s���ɓ���ߓ_�̉��x����艷�x�ɕۂ悤�ɂ���B
	void SetInitThermoConductionParam(
		double thConduct,		// thConduct:�M�`����
		double rho,				// roh:���x
		double specificHeat,	// specificHeat:��M J/ (K�Ekg):1960
		double heatTrans		// heatTrans:�M�`�B�� W/(m^2�EK)
		);

	void SetThermalEmissivityToVtx(unsigned id,double thermalEmissivity);			///	�M���˗���ߓ_ id �ɐݒ肷��֐�
	void SetThermalEmissivityToVerticesAll(double thermalEmissivity);					///	�M���˗���S�ߓ_�ɐݒ�

	void SetHeatTransRatioToAllVertex();	//SetInit �Őݒ肵�Ă���M�`�B�W�����A�ߓ_(FemVertex)�̍\���̂̃����o�ϐ��ɑ��

	void InitCreateMatC();			///	�s��쐬�ŗp������ꕨ�Ȃǂ̏�����
	void InitVecFAlls();			///	Vecf�̍쐬�O�Ɏ��s���鏉�������� modify@ 2012.10.4
	void InitCreateMatk();			///	Matk�̍쐬�O�Ɏ��s���鏉��������

	///	�M�`�B�����ω��������Ȃǂ̍Čv�Z�p�̏������֐�
	void InitCreateVecf_();				
	void InitCreateMatk_();

	//	[K]:�M�`���}�g���N�X�����֐��Q
	//	..kimura�����Q�l�ɂ���(�f���Ȃ���)���o�����v�Z�@			//>	k1kt�ɉ��̂���
	void CreateMatk1k(unsigned id);
	//	..yagawa1983�̌v�Z�@��3�����g�������v�Z�@ b:book �̈Ӗ�	//>	k1bt�ɉ��̂���
	void CreateMatk1b(unsigned id);
	//	..�l�ʑ̂��Ƃɍ��̂ŁA�l�ʑ̂������Ɏ�� ���O���ׂĂ̎l�ʑ̂ɂ��čs��
	void CreateMatk2(unsigned id,FemTet tets);
	//void CreateMatk2f(Face faces);		// �l�ʑ̂��Ƃɍ�鎮�ɂȂ��Ă��邪�A�O�k�̎O�p�`face���ɍ��@faces��f
	//	..�l�ʑ̂��Ƃɍ��@tets��t
	void CreateMatk2t(unsigned id);
	//	..�l�ʑ�(tets)��t ���ɐ���
	void CreateMatk3t(unsigned id);

	void CreateMatk2array();
	void CreateMatkLocal(unsigned i);			//	edges�ɓ���A�`�F�b�N�p�̑S�̍����s����Aifdef�X�C�b�`�ō���d�l
//	void CreateDumMatkLocal();					//	�S�v�f��0�̃_�~�[k
	void CreateMatKall();
	void CreateMatKAll();						//	K�̑S�̍����s��	//	SciLab	�ŗp����
	

	//	[C]:�M�e�ʃ}�g���N�X�����֐�
	void CreatedMatCAll(unsigned id);			//	matC1,C2,C3�E�E�E���ɕ������ׂ��H
	void CreateMatc(unsigned id);				// c�̗v�f�����s������֐�

	void CreateMatCAll();						//	C�̑S�̍����s	//	SciLab	�ŗp����

	//	{F}:�M�����x�N�g�������֐�
	void CreateVecFAll(unsigned id);				//	�l�ʑ̃��b�V����ID��������
	void CreateVecf3(unsigned id);					//	�M�`�B���͑������ρA���͗��̉��x�͊e�X���`��֐��ɁH
	void CreateVecf3_(unsigned id);					//	�M�`�B�����A���͗��̉��x����������
	void CreateVecf2(unsigned id);					//	�l�ʑ̂�ID��������
	void CreateVecf2surface(unsigned id);			//> �l�ʑ�ID�Ɋ܂܂��face�̓��A�\�ʂ�face�ɂ��Ă����v�Z ���M�ʂ͎�΃��[�h�Őݒ�
	void CreateVecf2surface(unsigned id,unsigned mode);			//> ����@�����āAvecFAll_f2IH[num]�Ɋi�[�AOFF�A��΁A���΁B���΂̎��́Amode = 4
	void CreateVecF2surfaceAll();					//	IH���̉��M�����ݒ肩��A�S�̍����x�N�g��(�E�s��)(���~���H)�����֐��@2012.08.30�ǋL
		//CreateVecfLocal(unsigned id);������
	void CreateVecF3surfaceAll();
	

	//	{T}:�ߓ_���x�x�N�g�������֐�
	void CreateTempVertex();					//�ߓ_�̉��x���������ߓ_�z�񂩂�A�S�̏c�x�N�g�������B	���̏c�s��̐ߓ_�̕��я��́Ai�Ԗڂ̐ߓ_ID��i�Ȃ̂���
	void CreateLocalMatrixAndSet();				//K,C,F���ׂĂ̍s��E�x�N�g���ɂ��ėv�f�����s�������āA�G�b�W�ɓ����	����	�S�̍����s������֐�
	
	//	������
	void InitTcAll(double temp);							//	Tc�̉��x��������

public:
	void SetTimeStep(double dt){ tdt = dt; }
	double GetTimeStep(){ return tdt; }
	//	��Step�Ăяo��
	void UpdateIHheat(unsigned heatingMODE);	// ��Step�ĂԁF�M�����x�N�g��{F}�𐶐��E�ۑ�	//.heatingMODE�͉��M�����F4�i�K�i0:OFF�E1:���(WEEK)�E2:����(MIDDLE)�E3:����(HIGH)�j
	void UpdateVecF_frypan();			// �������������O�ɁA�M�����x�N�g�������[�h���āA��������Ȃǃx�N�g�������Bmode�ɂ͉��M���[�h������
	void UpdateVecF();		// �H�ޗp

protected:
	//	���p�ɗp����H	�s��쐬�̊֐����܂Ƃ߂邾���H
	void CreateMatrix();					

	//%%%		k,c,f���ʂŗp����v�Z�֐�		%%%//
	// �G�b�W�Ⓒ�_��k,c,f�̗v�f�����s��̌W����ݒ�	���ׂĂ̎l�ʑ̂ɂ��ėv�f�����s������߁Ak,c,f�ɒl������ƁA���̗v�f�ɂ��ď��������s
	void SetkcfParam(FemTet tets); // shiba
	// ���_ID�@�R�_���琬��O�p�`�̋��ρ@(�l�ʑ̂̎O�p�`�ʐϕ����ŗ��p)
	double CalcTriangleArea(int id0, int id2, int id3);		
	double CalcTetrahedraVolume(FemTet tet); 		// �l�ʑ̂�ID������ƁA���̑̐ς��v�Z���Ă����֐�
	double CalcTetrahedraVolume2(unsigned id);			// �l�ʑ̂�ID������ƁA���̑̐ς��v�Z���Ă����֐�
	//void PrepareStep();							// Step()�ŕK�v�ȕϐ����v�Z����֐�
	//double CalcbVec(int vtxid,
	
	//	�i�N�����N�j�R���\���@��p�����j�K�E�X�U�C�f���@�ŔM�`�����v�Z
	void CalcHeatTransUsingGaussSeidel(
		unsigned NofCyc,	// NofCyc:�J��Ԃ��v�Z��
		double dt			// dt:�X�e�b�v����
		);
	// �i�O�i�E�N�����N�j�R���\���E��ސϕ��ɑΉ�)�K�E�X�U�C�f���@�ŔM�`�����v�Z 
	void CalcHeatTransUsingGaussSeidel(
		unsigned NofCyc,		// NofCyc:�J��Ԃ��v�Z��
		double dt,				// dt:�X�e�b�v����
		double eps				// eps:�ϕ��̎�� 0.0:�O�i�ϕ�,0.5:�N�����N�j�R���\��������,1.0:��ސϕ��E�A��@
		);
	void CalcHeatTransDirect(
		double dt			// dt:�X�e�b�v����
		);
	void CalcHeatTransDirect2(
		double dt			// dt:�X�e�b�v����
		);
	void SetTempAllToTVecAll(unsigned size);		//	TVecAll�ɑS�ߓ_�̉��x��ݒ肷��֐�
	void SetTempToTVecAll(unsigned vtxid);			//	TVecAll�ɓ���̐ߓ_�̉��x��ݒ肷��֐�
	void UpdateVertexTemp(unsigned vtxid);			//	�v�Z���ʂƂ��Ẳ��x��TVecAll����ߓ_�ɍX�V����


	//	SciLab
	void ScilabTest();								//	Scilab���g���Ă݂�֐�



	void TexChange(unsigned id,double tz);		//		GR�ֈڐA�@������搶�ɕ���
	void SetTexZ(unsigned id,double tz);		//	�e�N�X�`�����W��ύX

	void HeatTransFromPanToFoodShell();									//	�`���`�M�F�H�ނ̊O�k�̃t���C�p����S�ɋ߂��ߓ_����M���`���		//	�H�ށA��������������
	
	//	���˓`�M��L���ɂ������H�ނ�FWFemMesh�H��If�A���M����FWFemMesh��If,�M�t�˂̐Z���[������
	//	..���˓`�M�F�H�ނ̊O�k���琔�~���ȓ��̐ߓ_�ɑ΂��āA���M����M���`���
	void HeatRadientToFood(char foodID,char panID,double fluxdepth);
												//
	PTM::TMatrixRow<4,4,double> Create44Mat21();	//���ʂŗp����A4�~4��2��1�łł����s���Ԃ��֐�
	//���邢�́A����������ƁA������ς��Ă����֐�
	PTM::TMatrixCol<4,1,double> Create41Vec1();		//���ʂŗp����A4�~1��1�łł����s��(�x�N�g��)��Ԃ��֐�

	//%%%%%%%%		�o�C�i���X�C�b�`�̐錾		%%%%%%%%//
	///	PHFemMesh�ɑ�����\���́A�N���X�Œ�`����Ă��铯�l��bool���X�V���ꂽ��A��������X�V����
	///	�������p��ɂ�false�ɖ߂��B�����l��true	for	������
	bool deformed;					///	�`�󂪕ς�������ǂ���		///	�\���̂Ɏ����Ă��Ăق���
	bool alphaUpdated;				///	�M�`�B�����X�V�Amatk2,Vecf3�����X�V���ꂽ��
	bool doCalc;					///	�K�E�X�U�C�f���v�Z����t���O

	std::ofstream templog;		// ���_�̉��x�ω�
	std::ofstream cpslog;		//	CPS�̕ω�


	//%%%	�A�v���P�[�V����	%%%//

	PHSolidIf* phFloor;
	PHSolidIf* phFood;
	//FWObjectIf					fwpan;
	//FWObjectIf*					fwpan;
	//UTRef<FWObjectIf*>			fwpan_;


public:
	//	PHFemMEsh.h���̃N���X����M�`�����L�̃p�����[�^���p�����Ďg��

	//	�p����@�ȉ��̂悤�Ɍp�����āA�M�`�����L�̃p�����[�^��p����
	//class thFemVertex :public FemVertex{
	//	double heatflux2;
	//};
	//std::vector<thFemVertex> thvertices;
	//	��L�p���ŋߓ����ɁA�M�v�Z�p�����[�^���p������B�U��FEM��͂ł��A��ʃN���X�E�\���̂��g����悤�ɏ������Ă���

	/////	FemMeshThermo
	//struct FemMeshThermo{		
	//	bool alphaUpdated;			///	���b�V�����̈�ł��A�ߓ_�̔M�`�B�����X�V���ꂽ���ǂ�����K2,F3�ɉe��
	//	bool hUpdated;				///	�M�t�˗����X�V���ꂽ��
	//};

	
	// ���M�v�Z�����Ăяo�� Set�n�֐�
	PHFemThermo(const PHFemThermoDesc& desc=PHFemThermoDesc(), SceneIf* s=NULL);
	virtual void Init();
	void AfterSetDesc();		//	�`�M�s��̌v�Z�ȂǁAIH���M���[�h�͎��̊֐��Őݒ�
	//void Setup();
	/// �M�`���V�~�����[�V�����ŃG���W�����p����X�e�b�v���I�[�o�[���C�h	
	virtual void Step();
	void Step(double dt);
	//�i�ߓ_���x�̍s����쐬����O�Ɂj���_�̉��x��ݒ肷��i�P�ʐێ����j
	void SetVerticesTempAll(double temp);
	// �ߓ_i�̉��x��temp�x�ɐݒ肵�A�����TVEcAll�ɔ��f
	void SetVertexTemp(unsigned i,double temp);
	//	�ړ_i�̎��͂̐ߓ_���x��temp�x�ɐݒ�
	void SetLocalFluidTemp(unsigned i,double temp);
	///	���_�̔M�`�B�����X�V���A�s�����蒼��
	void UpdateheatTransRatio(unsigned id,double heatTransRatio);
	//	�v�Z���ʂƂ��Ẳ��x��TVecAll����S�ߓ_�ɍX�V����
	void UpdateVertexTempAll();	

	// If �Ƃ��Ă̎���
	void PHFemThermo::AddvecFAll(unsigned id,double dqdt);
	// AddVecFAll������������A����
	void PHFemThermo::SetvecFAll(unsigned id,double dqdt);

	//%%%%%%%%		�M�`�����E�����̐ݒ�֐��̐錾		%%%%%%%%//
	void SetThermalBoundaryCondition();				//	�M�`�����E�����̐ݒ�

	//%%%%%%	�g����		%%%%%//
	//	���x���E�����@��ݒ�֐��Q
	//	..���x�Œ苫�E����:S_1		�w��ߓ_�̉��x��ς���
	void UsingFixedTempBoundaryCondition(unsigned id,double temp);
	//	..�M�`�B���E����:S_3		�w��ߓ_�̎��͗��̉��x��ς���
	void UsingHeatTransferBoundaryCondition(unsigned id,double temp);
	//	�M�`�B�����ݒ�\�Ȋ֐�	//>	���O��ς���ׂ��@�v���P
	void UsingHeatTransferBoundaryCondition(unsigned id,double temp,double heatTransratio);
	
	//	�ȉ��A�l���r��
	//	���̈�
	//SetVtxtoFixedBC
	//SetVtxtoTranferBC
	//SetVtxHTRtoTranferBC
	//	���x�����������ߓ_��ߓ_����̗��̂́A���̐ߓ_��x,y,z���W������肷��΂����H	���x���E����Tc�̐ݒ�Ȃ�

	///	�}��->wiki
	//BC=BoundaryCondition
	//HTR=HeatTransferRatio

	int GetSurfaceVertex(int id){return GetPHFemMesh()->surfaceVertices[id];}; //shiba
	int NSurfaceVertices(){return int(GetPHFemMesh()->surfaceVertices.size());};
	//	���͗��̉��x���X�V	�M�`�B���͕ς��Ȃ�
	void SetVertexTc(int id,double temp){
		// ...PHFemThermo::SetLocalFluidTemp()�Ŏ��͗��̉��x�̐ݒ�ACreateVecfLocal()�̍X�V
		UsingHeatTransferBoundaryCondition(id,temp);
	};
	///	���͗��̉��x���X�V		�M�`�B����ύX�ł���If
	void SetVertexTc(int id,double temp,double heatTransRatio){
		//	..�M�`�B���E������ݒ�F�M�`�B�����܂ލs�񂪑Ώ�
		UsingHeatTransferBoundaryCondition(id,temp,heatTransRatio);	
	};
	// �ߓ_�̔M������ݒ肷��
	void SetVertexHeatFlux(int id,double heatFlux);
	// �S�ߓ_�̔M������ݒ肷��
	void SetVtxHeatFluxAll(double heatFlux);

	Vec3d GetPose(unsigned id){ return GetPHFemMesh()->vertices[id].pos; };
	Vec3d GetSufVtxPose(unsigned id){return GetPHFemMesh()->vertices[GetPHFemMesh()->surfaceVertices[id]].pos; };
	// ���b�V���ߓ_�̉��x���擾
	double GetVertexTemp(unsigned id){ return vertexVars[id].temp; };
	// ���b�V���\�ʂ̐ߓ_���x���擾
	double GetSufVertexTemp(unsigned id){ return vertexVars[GetPHFemMesh()->surfaceVertices[id]].temp; };

	//%%%	Step�J�E���g�̂��߂̕ϐ��A�֐���`		%%%//
	unsigned long StepCount;			//	Step���J�E���g�A�b�v�̉��Z�W��	
	unsigned long StepCount_;			//	Step���J�E���g�A�b�v�̉��Z�W��	StepCount�����T�ڂ���\���J�E���g 
	//	StepCount���J�E���g�A�b�v
	unsigned long GetStepCount(){return StepCount;};
	//	StepCount�̎���J�E���g�A�b�v
	unsigned long GetStepCountCyc(){return StepCount_;};


	//	�K�E�X�U�C�f���v�Z�ŗp����֐��Q

	///	���b�V���\�ʐߓ_�̌��_����̋������v�Z���āAstruct FemVertex �� disFromOrigin �Ɋi�[
	void CalcVtxDisFromOrigin();
	void CalcVtxDisFromVertex(Vec2d originVertexIH);
	Vec2d originVertexIH;

	//	IH�ɂ�l�ʑ̂�face�ʂ̔M�������M�̂��߂̍s�񐬕��v�Z�֐�
	void CalcIHdqdt(double r,double R,double dqdtAll,unsigned num);				//	IH�q�[�^�[�̐ݒ�
	void CalcIHdqdt_atleast(double r,double R,double dqdtAll,unsigned num);		//	face�ʂł̔M�����ʂ��v�Z�F�����ł��~�̈�ɂ������Ă�����A����face�̖ʐϑS����IH���M��������
	void CalcIHdqdtband(double xS,double xE,double dqdtAll,unsigned num);		//	�я�ɉ��M�Ax���Ő؂�
	void CalcIHdqdtband_(double xS,double xE,double dqdtAll,unsigned num);		//	�я�ɉ��M�Ax���Ő؂� mayIHheated���g��Ȃ�
	void CalcIHdqdt2(double r,double R,double dqdtAll,unsigned num);				//	IH�q�[�^�[�̐ݒ�  num�͉Η͕�(0:week, 1:middle, 2:high )
	void CalcIHdqdt3(double r,double R,double dqdtAll,unsigned num);				//	IH�q�[�^�[�̐ݒ�	1���_�ł��̈���ɓ����Ă���Ƃ��ɂ́A�����IH�v�Z�̗̈�ɉ��Z����
	void CalcIHdqdt4(double radius,double Radius,double dqdtAll,unsigned num);	//	IH�q�[�^�[�̐ݒ�	2�����A�Ή��ł���O�p�`�̏ꍇ�����L���B�������A�O�p�`�̑傫���̊��ɁA���M�~���a���������ꍇ�́A�l�����Ȃ��B
	//	face���_��IH���M���̍s�񐬕����v�Z	���a10cm���x�̉~�ʂƁA�~����cm�����܂������x�̎O�p�`�T�C�Y��z��
	void CalcIHarea(double radius,double Radius,double dqdtAll);
	//	DSTR �Ɍ�_�v�Z���ʂ�\������
	void ShowIntersectionVtxDSTR(unsigned faceID,unsigned faceVtxNum,double radius);
	//	�������O��vector����O�p�`�����ʐϘa������
	double CalcDivTriArea(std::vector<double> trivec);

	// face���_�𕨑̌��_����߂����ɕ��ׂ����Aface�N���X�ϐ�:ascendVtx[0~2]�ɋ߂����̒��_ID���i�[;ascendVtx[0]�����_�Ŋ��	
	void ArrangeFacevtxdisAscendingOrder(int faceID);

	// 1�_�ƌ���邱�Ƃ�z��:id0.id1�����񂾐����ƁA���ar�܂���R�̉~��������_�����߂�B(���C��)���W��Ԃ�
	Vec2d CalcIntersectionPoint(unsigned id0,unsigned id1,double r,double R);
	// ���ar,R�̉~�ʂ�2�_�ō������̌�_�����߂�
	//	...r�AR��2�~�ʂƌ����2��_
	std::vector<Vec2d> CalcIntersectionPoint2(unsigned id0,unsigned id1,double r,double R);
	// ���ar�̉~�ʂƐ����̌�_���W���P�v�Z
	Vec2d CalcIntersectionOfCircleAndLine(
		unsigned id0,	//	�����̗��[�_�̒��_�ԍ��i0 ~ vertices.size()�j
		unsigned id1,
		double radius	//	���a
		);
	//Vec4d CalcIntersectionPoint4(unsigned id0,unsigned id1,double r,double R);		/// 4�_�ƌ���邱�Ƃ�z��:			����
	///	���b�V���ŗL�̃p�����[�^�̐ݒ�
	void SetRhoSpheat(double rho,double Cp);	// �f�ތŗL�̖��x�A��M�̐ݒ�

	/// dt��萔�{����
	unsigned Ndt;
	double dNdt;


	//�G�b�W�̕`��
	void DrawEdge(unsigned id0, unsigned id1);

	//	x���Ɛ�����IH���M�̑ї̈�ɐ����������߂ɕK�v
	void SetIHbandDrawVtx(double xS, double xE);
	Vec2d GetIHbandDrawVtx();

	//���ԕ\��
	double		cpstime;		//[sec] �@1�@/�@phfemthermo�Ăяo����[1/sec]

public:
	void DecrMoist();
	void DecrMoist_vel(double dt);
	void DecrMoist_velo(double vel);
	void InitMoist();
	void InitAllVertexTemp();

	void SetParamAndReCreateMatrix(double thConduct0,double roh0,double specificHeat0);//�M�`�B���ȊO�i�M�`�B���͌��smain.cpp�ōs���Ă��邽��
	double GetArbitraryPointTemp(Vec3d temppos);							//	�l�ʑ̓��C�ӓ_�̉��x���擾	temppose:�m�肽���_�̃��[�J�����W
	Vec3d GetDistVecDotTri(Vec3d Dotpos,Vec3d trivtx[3]);					//	�_����O�p�`��(2�ӂ̃x�N�g���Œ�`)�ւ̃x�N�g�����v�Z
	double GetVtxTempInTets(Vec3d temppos);											//	�C�ӓ_�̎l�ʑ̓��O����Ftemppos������faceID��Ԃ��B�����疳����΁A-1��Ԃ��B
	double CalcTempInnerTets(unsigned id,PTM::TVector<4,double> N);			//	�^����ꂽ�`��֐��ł̎l�ʑ̓��̉��x��Ԃ�

	std::ofstream matCAllout;
	std::ofstream matKAllout;
	std::ofstream checkTVecAllout;
	std::ofstream FEMLOG;
	unsigned long long COUNT;

	///	���b�V���̖ʂ̑�����Ԃ�
	//int NFaces();
	///	���b�V����Face�ӂ̗��[�_�̍��W��Ԃ�?
	//std::vector<Vec3d> GetFaceEdgeVtx(unsigned id);
	///	���b�V����Face�ӂ̗��[�_�̍��W��Ԃ�?
	//Vec3d GetFaceEdgeVtx(unsigned id, unsigned vtx);
};




}	//	namespace Spr
#endif