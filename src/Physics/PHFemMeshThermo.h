/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PHFEMMESHTHERMO_H
#define PHFEMMESHTHERMO_H

#include <Physics/PHFemMesh.h>

namespace Spr{;

///	�M��FEM�p�̃��b�V��
class PHFemMeshThermo: public PHFemMesh{
public:
	/////	FemMeshThermo
	//struct FemMeshThermo{		
	//	bool alphaUpdated;			///	���b�V�����̈�ł��A�ߓ_�̔M�`�B�����X�V���ꂽ���ǂ�����K2,F3�ɉe��
	//	bool hUpdated;				///	�M�t�˗����X�V���ꂽ��
	//};
	SPR_OBJECTDEF(PHFemMeshThermo);

	//	���_
	struct StateVar{
		double temperature;
	};
	struct Coeff{
	};
	std::vector<StateVar> vertexVars;
	std::vector<Coeff> edgeCoeffs;
	
	PHFemMeshThermo(const PHFemMeshThermoDesc& desc=PHFemMeshThermoDesc(), SceneIf* s=NULL);
	///	�f�X�N���v�^�̃T�C�Y
	virtual size_t GetDescSize() const { return sizeof(PHFemMeshThermoDesc); };
	///	�f�X�N���v�^�̓ǂݏo��(�R�s�[��)
	virtual bool GetDesc(void* desc) const;
	///�f�X�N���v�^�̐ݒ�B
	virtual void SetDesc(const void* desc);
	///�M�`���V�~�����[�V�����ŃG���W�����p����X�e�b�v		//(�I�[�o�[���C�h)
	void Step(double dt);

	void SetVerticesTempAll(double temp);					//�i�ߓ_���x�̍s����쐬����O�Ɂj���_�̉��x��ݒ肷��i�P�ʐێ����j
	void SetVertexTemp(unsigned i,double temp);		// �ߓ_i�̉��x��temp�x�ɐݒ肵�A�����TVEcAll�ɔ��f
	void SetLocalFluidTemp(unsigned i,double temp);		//	�ړ_i�̎��͂̐ߓ_���x��temp�x�ɐݒ�
	void UpdateheatTransRatio(unsigned id,double heatTransRatio);	///	���_�̔M�`�B�����X�V���A�s�����蒼��

	// If �Ƃ��Ă̎���
	void PHFemMeshThermo::SetvecFAll(unsigned id,double dqdt);

	//%%%%%%%%		�M�`�����E�����̐ݒ�֐��̐錾		%%%%%%%%//
	void SetThermalBoundaryCondition();				//	�M�`�����E�����̐ݒ�

	//	�g����
	//	���x�����������ߓ_��ߓ_����̗��̂́A���̐ߓ_��x,y,z���W������肷��B
	//	���̍ۂɈȉ��̊֐���p����Ηǂ��B
	void UsingFixedTempBoundaryCondition(unsigned id,double temp);			//	���x�Œ苫�E����:S_1	�w��ߓ_�̉��x��ς���
	void UsingHeatTransferBoundaryCondition(unsigned id,double temp);		//	�M�`�B���E����:S_3		�w��ߓ_�̎��͗��̉��x��ς���	
	void UsingHeatTransferBoundaryCondition(unsigned id,double temp,double heatTransratio);		///	�M�`�B�����ݒ�\�Ȋ֐�		///	���O��ς���ׂ��@�v���P
	//	���̈�
	//SetVtxtoFixedBC
	//SetVtxtoTranferBC
	//SetVtxHTRtoTranferBC

	///	�}��->wiki
	//BC=BoundaryCondition
	//HTR=HeatTransferRatio

	int GetSurfaceVertex(int id){return surfaceVertices[id];};
	int NSurfaceVertices(){return surfaceVertices.size();};
	void SetVertexTc(int id,double temp){								///	���͗��̉��x���X�V	�M�`�B���͕ς��Ȃ�
		UsingHeatTransferBoundaryCondition(id,temp);					/// PHFemMeshThermo::SetLocalFluidTemp()�Ŏ��͗��̉��x�̐ݒ�ACreateVecfLocal()�̍X�V
	};
	void SetVertexTc(int id,double temp,double heatTransRatio){			///	���͗��̉��x���X�V		�M�`�B����ύX�ł���If
		UsingHeatTransferBoundaryCondition(id,temp,heatTransRatio);		//	�M�`�B���E������ݒ�F�M�`�B�����܂ލs�񂪑Ώ�
	};
	void SetVertexHeatFlux(int id,double heatFlux);						//> �ߓ_�̔M������ݒ肷��
	void SetVtxHeatFluxAll(double heatFlux);							//> �S�ߓ_�̔M������ݒ肷��

	Vec3d GetPose(unsigned id){ return vertices[id].pos; };
	Vec3d GetSufVtxPose(unsigned id){return vertices[surfaceVertices[id]].pos; };

	unsigned long StepCount;			//	Step���̃J�E���g�A�b�v		// 
	unsigned long StepCount_;			//	Step���̃J�E���g�A�b�v	StepCount�����T�ڂ���\���J�E���g	// 

	unsigned long GetStepCount(){return StepCount;};
	unsigned long GetStepCountCyc(){return StepCount_;};
	
	void CalcVtxDisFromOrigin();			///	���b�V���\�ʐߓ_�̌��_����̋������v�Z���āAstruct FemVertex �� disFromOrigin �Ɋi�[
	void CalcIHdqdt(double r,double R,double dqdtAll);			///	IH�q�[�^�[�̐ݒ�

protected:
	//PHFemMeshThermo���݂̂ŗp����v�Z

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
	PTM::TMatrixRow<4,4,double> matk1array[4];	//Km��3��4�~4�s��̓��ꕨ�@Matk1�����܂ł̊Ԃ̈ꎞ�I�ȃf�[�^�u��
	PTM::TMatrixRow<4,4,double> matk2array[4];	//k21,k22,k23,k24��4�~4�s��̓��ꕨ�@Matk�����܂ł̊Ԃ̈ꎞ�I�ȃf�[�^�u��
	PTM::TMatrixRow<4,4,double> matk3array[4];	//k21,k22,k23,k24��4�~4�s��̓��ꕨ�@Matk�����܂ł̊Ԃ̈ꎞ�I�ȃf�[�^�u��

	PTM::TMatrixRow<4,4,double> matk;			//Matk=Matk1+Matk2+Matk3	matk1~3�����������v�f�����s��	CreateMatkLocal()
	PTM::TMatrixRow<4,4,double> matc;			//
//	PTM::TMatrixCol<4,1,double> Vecf3;			//f3:�O���̖ʂɖʂ��Ă���ʂ̂݌v�Z����@�v����
//	PTM::TMatrixCol<4,1,double> Vecf3array[4];	//f31,f32,f33,f34��4�~1�x�N�g���̓��ꕨ		Matk�����܂ł̊Ԃ̈ꎞ�I�ȃf�[�^�u��
//	PTM::TMatrixCol<4,1,double> Vecf;			//f1~f4�����Z�����c�x�N�g��
	PTM::TVector<4,double> vecf3;			//f3:�O���̖ʂɖʂ��Ă���ʂ̂݌v�Z����@�v����
	PTM::TVector<4,double> vecf3array[4];	//f31,f32,f33,f34��4�~1�x�N�g���̓��ꕨ		Matk�����܂ł̊Ԃ̈ꎞ�I�ȃf�[�^�u��
	PTM::TVector<4,double> vecf;			//f1~f4�����Z�����c�x�N�g��
	PTM::TVector<4,double> vecf2;
	PTM::TVector<4,double> vecf2array[4];
//	PTM::VVector<double> Vechoge;
	//	�ϐ��͏������@�֐��͑啶��

	//�s��k�̌v�Z�p�̌W���s��
	PTM::TMatrixRow<3,3,double> mata;
	PTM::TMatrixRow<3,3,double> matb;
	PTM::TMatrixRow<3,3,double> matcc;
	PTM::TMatrixRow<3,3,double> matd;


	//�S�̂̌W���s��	//SciLab�Ŏg�p
	PTM::VMatrixRow<double> matKAll;			//[K]�̑S�̍����s��		//CreateMatKall()
	PTM::VMatrixRow<double> matCAll;			//[C]
public:
	//	�K�E�X�U�C�f���v�Z���Ɏg�p���Ă���
	PTM::VMatrixCol<double> vecFAll;			//{F}�̑S�̍����x�N�g��

protected:
	//�S�̂̍����s��̑���
	PTM::VMatrixRow<double> dMatKAll;			//�S�̍����s��K�̑Ίp�����ɂȂ�͂��̒l�����̂܂ܑ��		�������ł̏������̎d��	DMatKAll.resize(1,vertices.size());
	PTM::VMatrixRow<double> dMatCAll;			//�S�̍����s��C�̑Ίp����
	PTM::VMatrixRow<double> _dMatAll;			//�S�̍����s��K��C�̑Ίp�����̒萔�{�a�̋t�����Ƃ�������	�K�E�X�U�C�f���̌v�Z�ɗ��p����
	PTM::VMatrixCol<double> bVecAll;			//�K�E�X�U�C�f���̌v�Z�ɗp����萔�s��b�̏c�x�N�g��	Row�ł���K�v�͂���̂��H��Col�ɂ���
	//double *constb;								//�K�E�X�U�C�f���̌W��b������z��̃|�C���^	��ŏ�芷����

	//%%%		�֐��̐錾		%%%%%%%%//
	//%%%%%%		�M�`���v�Z�{��		%%%//
	//	�M�`�B���E�����̎��͂��ׂĂ̈����𖞂����K�v������B
	//	���x�Œ苫�E������p�������Ƃ��ɂ́A�M�`�B���i�Ō�̈����j����͂��Ȃ��B�܂��A��Step���s���ɓ���ߓ_�̉��x����艷�x�ɕۂ悤�ɂ���B
	void SetInitThermoConductionParam(double thConduct,double roh,double specificHeat,double heatTrans);		//�M�`�����A���x�A��M�A�M�`�B���Ȃǂ̃p�����[�^�[��ݒ�E���

	void SetThermalEmissivityToVtx(unsigned id,double thermalEmissivity);			///	�M���˗���ߓ_ id �ɐݒ肷��֐�
	void SetThermalEmissivityToVerticesAll(double thermalEmissivity);					///	�M���˗���S�ߓ_�ɐݒ�

	void SetHeatTransRatioToAllVertex();	//SetInit �Őݒ肵�Ă���M�`�B�W�����A�ߓ_(FemVertex)�̍\���̂̃����o�ϐ��ɑ��

	void InitCreateMatC();					///	�s��쐬�ŗp������ꕨ�Ȃǂ̏�����
	void InitCreateVecf();						///	Vecf�̍쐬�O�Ɏ��s���鏉��������
	void InitCreateMatk();						///	Matk�̍쐬�O�Ɏ��s���鏉��������

	///	�M�`�B�����ω��������Ȃǂ̍Čv�Z�p�̏������֐�
	void InitCreateVecf_();				
	void InitCreateMatk_();

	//	[K]:�M�`���}�g���N�X�����֐��Q
	void CreateMatk1k(unsigned id);					//>	kimura�����Q�l�ɂ���(�f���Ȃ���)���o�����v�Z�@													///	k1kt�ɉ��̂���
	void CreateMatk1b(unsigned id);		//>	yagawa1983�̌v�Z�@��3�����g�������v�Z�@ b:book �̈Ӗ�					///	k1bt�ɉ��̂���
	void CreateMatk2(unsigned id,Tet tets);			//>	�l�ʑ̂��Ƃɍ��̂ŁA�l�ʑ̂������Ɏ�� ���O���ׂĂ̎l�ʑ̂ɂ��čs��
	//void CreateMatk2f(Face faces);					//>	�l�ʑ̂��Ƃɍ�鎮�ɂȂ��Ă��邪�A�O�k�̎O�p�`face���ɍ��@faces��f
	void CreateMatk2t(unsigned id);					//>	�l�ʑ̂��Ƃɍ��@tets��t
	void CreateMatk3t(unsigned id);					//>	�l�ʑ�(tets)��t ���ɐ���


	void CreateMatk2array();
	void CreateMatkLocal(unsigned i);
//	void CreateDumMatkLocal();					//	�S�v�f��0�̃_�~�[k
	void CreateMatKall();
	//	[C]:�M�e�ʃ}�g���N�X�����֐�
	void CreateMatcLocal(unsigned id);						//	matC1,C2,C3�E�E�E���ɕ������ׂ��H
	void CreateMatc(unsigned id);					//c�̗v�f�����s������֐�
	//	{F}:�M�����x�N�g�������֐�
	void CreateVecfLocal(unsigned id);						//	�l�ʑ̃��b�V����ID��������
	void CreateVecf3(unsigned id);					//	�M�`�B���͑������ρA���͗��̉��x�͊e�X���`��֐��ɁH
	void CreateVecf3_(unsigned id);					//	�M�`�B�����A���͗��̉��x����������
	void CreateVecf2(unsigned id);					//	�l�ʑ̂�ID��������

	//	{T}:�ߓ_���x�x�N�g�������֐�
	void CreateTempMatrix();					//�ߓ_�̉��x���������ߓ_�z�񂩂�A�S�̏c�x�N�g�������B	���̏c�s��̐ߓ_�̕��я��́Ai�Ԗڂ̐ߓ_ID��i�Ȃ̂���
	void CreateLocalMatrixAndSet();				//K,C,F���ׂĂ̍s��E�x�N�g���ɂ��ėv�f�����s�������āA�G�b�W�ɓ����	����	�S�̍����s������֐�
	
	//	������
	void InitTcAll();							//	Tc�̉��x��������


	//	���p�ɗp����H	�s��쐬�̊֐����܂Ƃ߂邾���H
	void CreateMatrix();					

	//	k,c,f���ʂŗp����v�Z�֐�
	void SetkcfParam(Tet tets);					//�G�b�W�Ⓒ�_��k,c,f�̗v�f�����s��̌W����ݒ肷��֐�	���ׂĂ̎l�ʑ̂ɂ��ėv�f�����s������߁Ak,c,f�ɒl������ƁA���̗v�f�ɂ��ď��������s����	
	double CalcTriangleArea(int id0, int id2, int id3);		//�ߓ_ID������ƁA���̓_�ō\�����ꂽ�O�p�`�̖ʐς����߂�@�l�ʑ̂ł̖ʐϕ��Ŏg�p
	double CalcTetrahedraVolume(Tet tets);		////�l�ʑ̂�ID������ƁA���̑̐ς��v�Z���Ă����֐�
	//void PrepareStep();							//Step()�ŕK�v�ȕϐ����v�Z����֐�
	//double CalcbVec(int vtxid,
	void CalcHeatTransUsingGaussSeidel(unsigned NofCyc,double dt);		//	�i�N�����N�j�R���\���@��p�����j�K�E�X�U�C�f���@�ŔM�`�����v�Z NofCyc:�J��Ԃ��v�Z��,dt:�X�e�b�v����
	void CalcHeatTransUsingGaussSeidel(unsigned NofCyc,double dt,double eps);		//	�i�O�i�E�N�����N�j�R���\���E��ސϕ��ɑΉ�)�K�E�X�U�C�f���@�ŔM�`�����v�Z NofCyc:�J��Ԃ��v�Z��,dt:�X�e�b�v����,e:�ϕ��̎�� 0.0:�O�i�ϕ�,0.5:�N�����N�j�R���\��������,1.0:��ސϕ��E�A��@


	void SetTempAllToTVecAll(unsigned size);		//	TVecAll�ɑS�ߓ_�̉��x��ݒ肷��֐�
	void SetTempToTVecAll(unsigned vtxid);			//	TVecAll�ɓ���̐ߓ_�̉��x��ݒ肷��֐�
	void UpdateVertexTemp(unsigned vtxid);			//	�v�Z���ʂƂ��Ẳ��x��TVecAll����ߓ_�ɍX�V����
	void UpdateVertexTempAll(unsigned size);		//	�v�Z���ʂƂ��Ẳ��x��TVecAll����S�ߓ_�ɍX�V����

	//	SciLab
	void ScilabTest();								//	Scilab���g���Ă݂�֐�

	//		�S�̍����s������		//	SciLab	�ŗp����
	void CreateMatKAll();						//	K�̑S�̍����s��
	void CreateMatCAll();						//	C�̑S�̍����s

	void TexChange(unsigned id,double tz);		//		GR�ֈڐA�@������搶�ɕ���
	void SetTexZ(unsigned id,double tz);		//	�e�N�X�`�����W��ύX


	void HeatTransFromPanToFoodShell();									//	�`���`�M�F�H�ނ̊O�k�̃t���C�p����S�ɋ߂��ߓ_����M���`���		//	�H�ށA��������������
	
	//	���˓`�M��L���ɂ������H�ނ�FWFemMesh�H��If�A���M����FWFemMesh��If,�M�t�˂̐Z���[������
	void HeatRadientToFood(char foodID,char panID,double fluxdepth);						//	���˓`�M�F�H�ނ̊O�k���琔�~���ȓ��̐ߓ_�ɑ΂��āA���M����M���`���
												//
	PTM::TMatrixRow<4,4,double> Create44Mat21();	//���ʂŗp����A4�~4��2��1�łł����s���Ԃ��֐�
	//���邢�́A����������ƁA������ς��Ă����֐�
	PTM::TMatrixCol<4,1,double> Create41Vec1();		//���ʂŗp����A4�~1��1�łł����s��(�x�N�g��)��Ԃ��֐�

	//%%%%%%%%		�ϐ��̐錾		%%%%%%%%//
	//�M�v�Z�ɗp����p�����[�^
	double thConduct;				//�M�`����
	double heatTrans;				//�M�`�B��			//class �ߓ_�ɂ́AheatTransRatio�����݂���
	double roh;						//���x
	double specificHeat;			//��M
//	double dt;						//���ԍ��ݕ�
//	double dqdtAll;					//	����

	//%%%%%%%%		�o�C�i���X�C�b�`�̐錾		%%%%%%%%//
	///	PHFemMesh�ɑ�����\���́A�N���X�Œ�`����Ă��铯�l��bool���X�V���ꂽ��A��������X�V����
	///	�������p��ɂ�false�ɖ߂��B�����l��true	for	������
	bool deformed;					///	�`�󂪕ς�������ǂ���		///	�\���̂Ɏ����Ă��Ăق���
	bool alphaUpdated;				///	�M�`�B�����X�V�Amatk2,Vecf3�����X�V���ꂽ��
	
	std::ofstream templog;


	//%%%	�A�v���P�[�V����	%%%//

	PHSolidIf* phFloor;
	PHSolidIf* phFood;
	//FWObjectIf					fwpan;
	//FWObjectIf*					fwpan;
	//UTRef<FWObjectIf*>			fwpan_;

};




}	//	namespace Spr
#endif
