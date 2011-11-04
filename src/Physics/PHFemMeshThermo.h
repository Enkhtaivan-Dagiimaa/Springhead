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

	void SetVerticesTemp(double temp);					//�i�ߓ_���x�̍s����쐬����O�Ɂj���_�̉��x��ݒ肷��i�P�ʐێ����j
	void SetVerticesTemp(unsigned i,double temp);		// �ߓ_i�̉��x��temp�x�ɐݒ肵�A�����TVEcAll�ɔ��f
	void SetLocalFluidTemp(unsigned i,double temp);		//	�ړ_i�̎��͂̐ߓ_���x��temp�x�ɐݒ�

	//%%%%%%%%		�M�`�����E�����̐ݒ�֐��̐錾		%%%%%%%%//
	void SetThermalBoundaryCondition();				//	�M�`�����E�����̐ݒ�

	//	�g����
	//	���x�����������ߓ_��ߓ_����̗��̂́A���̐ߓ_��x,y,z���W������肷��B
	//	���̍ۂɈȉ��̊֐���p����Ηǂ��B
	void UsingFixedTempBoundaryCondition(unsigned id,double temp);			//	���x�Œ苫�E����:S_1	�w��ߓ_�̉��x��ς���
	void UsingHeatTransferBoundaryCondition(unsigned id,double temp);		//	�M�`�B���E����:S_3		�w��ߓ_�̎��͗��̉��x��ς���	




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
	//	Col:��P�ʂ̍s��	Row:�s�P�ʂ̍s��

	//�ߓ_���x�x�N�g��
	PTM::TMatrixCol<4,1,double> TVec;			//�v�f�̐ߓ_���x�x�N�g��
	PTM::VMatrixCol<double> TVecAll;			//�S�̂̐ߓ_���x�x�N�g��

	//�v�f�̌W���s��
	PTM::TMatrixRow<4,4,double> Matk1;			//CreateMatk1()
	PTM::TMatrixRow<4,4,double> Matk2;			//CreateMatk2()
	//int Matk2array[4];						//matk2���������z��		//CreateMatk2array()
	PTM::TMatrixRow<4,4,double> Matk1array[4];	//Km��3��4�~4�s��̓��ꕨ�@Matk1�����܂ł̊Ԃ̈ꎞ�I�ȃf�[�^�u��
	PTM::TMatrixRow<4,4,double> Matk2array[4];	//k21,k22,k23,k24��4�~4�s��̓��ꕨ�@Matk�����܂ł̊Ԃ̈ꎞ�I�ȃf�[�^�u��
	PTM::TMatrixRow<4,4,double> Matk;			//Matk=Matk1+Matk2+Matk3	matk1~3�����������v�f�����s��	CreateMatkLocal()
	PTM::TMatrixRow<4,4,double> Matc;			//
	PTM::TMatrixCol<4,1,double> Vecf3;			//f3:�O���̖ʂɖʂ��Ă���ʂ̂݌v�Z����@�v����
	PTM::TMatrixCol<4,1,double> Vecf3array[4];	//f31,f32,f33,f34��4�~1�x�N�g���̓��ꕨ		Matk�����܂ł̊Ԃ̈ꎞ�I�ȃf�[�^�u��
	PTM::TMatrixCol<4,1,double> Vecf;			//f1~f4�����Z�����c�x�N�g��

	//�s��k�̌v�Z�p�̌W���s��
	PTM::TMatrixRow<3,3,double> mata;
	PTM::TMatrixRow<3,3,double> matb;
	PTM::TMatrixRow<3,3,double> matc;
	PTM::TMatrixRow<3,3,double> matd;


	//�S�̂̌W���s��	//SciLab�Ŏg�p
	PTM::VMatrixRow<double> MatKAll;			//[K]�̑S�̍����s��		//CreateMatKall()
	PTM::VMatrixRow<double> MatCAll;			//[C]

	//	�K�E�X�U�C�f���v�Z���Ɏg�p���Ă���
	PTM::VMatrixCol<double> VecFAll;			//{F}�̑S�̍����x�N�g��

	//�S�̂̍����s��̑���
	PTM::VMatrixRow<double> DMatKAll;			//�S�̍����s��K�̑Ίp�����ɂȂ�͂��̒l�����̂܂ܑ��		�������ł̏������̎d��	DMatKAll.resize(1,vertices.size());
	PTM::VMatrixRow<double> DMatCAll;			//�S�̍����s��C�̑Ίp����
	PTM::VMatrixRow<double> _DMatAll;			//�S�̍����s��K��C�̑Ίp�����̒萔�{�a�̋t�����Ƃ�������	�K�E�X�U�C�f���̌v�Z�ɗ��p����
	PTM::VMatrixCol<double> bVecAll;			//�K�E�X�U�C�f���̌v�Z�ɗp����萔�s��b�̏c�x�N�g��	Row�ł���K�v�͂���̂��H��Col�ɂ���
	//double *constb;								//�K�E�X�U�C�f���̌W��b������z��̃|�C���^	��ŏ�芷����

	//%%%		�֐��̐錾		%%%%%%%%//
	//%%%%%%		�M�`���v�Z�{��		%%%//
	//	�M�`�B���E�����̎��͂��ׂĂ̈����𖞂����K�v������B
	//	���x�Œ苫�E������p�������Ƃ��ɂ́A�M�`�B���i�Ō�̈����j����͂��Ȃ��B�܂��A��Step���s���ɓ���ߓ_�̉��x����艷�x�ɕۂ悤�ɂ���B
	void SetInitThermoConductionParam(double thConduct,double roh,double specificHeat,double heatTrans);		//�M�`�����A���x�A��M�A�M�`�B���Ȃǂ̃p�����[�^�[��ݒ�E���

	//	[K]:�M�`���}�g���N�X�����֐��Q
	void CreateMatk1k(Tet tets);				//kimura�����̌v�Z�@
	void CreateMatk1b(Tet tets);				//yagawa1983�̌v�Z�@��3�����g�������v�Z�@ b:book �̈Ӗ�
	void CreateMatk2(Tet tets);					//�l�ʑ̂��Ƃɍ��̂ŁA�l�ʑ̂������Ɏ��
	void CreateMatk2array();
	void CreateMatkLocal();
//	void CreateDumMatkLocal();					//	�S�v�f��0�̃_�~�[k
	void CreateMatKall();
	//	[C]:�M�e�ʃ}�g���N�X�����֐�
	void CreateMatcLocal();						//	matC1,C2,C3�E�E�E���ɕ������ׂ��H
	void CreateMatc(Tet tets);					//c�̗v�f�����s������֐�
	//	{F}:�M�����x�N�g�������֐�
	void CreateVecfLocal();						//
	void CreateVecf3(Tet tets);					//
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
	void PrepareStep();							//Step()�ŕK�v�ȕϐ����v�Z����֐�
	//double CalcbVec(int vtxid,
	void CalcHeatTransUsingGaussSeidel(unsigned NofCyc,double dt);		//	�K�E�X�U�C�f���@�ŔM�`�����v�Z NofCyc:�J��Ԃ��v�Z��,dt:�X�e�b�v����

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


	PTM::TMatrixRow<4,4,double> Create44Mat21();	//���ʂŗp����A4�~4��2��1�łł����s���Ԃ��֐�
	//���邢�́A����������ƁA������ς��Ă����֐�
	PTM::TMatrixCol<4,1,double> Create41Vec1();		//���ʂŗp����A4�~1��1�łł����s��(�x�N�g��)��Ԃ��֐�

	//%%%%%%%%		�ϐ��̐錾		%%%%%%%%//
	//�M�v�Z�ɗp����p�����[�^
	double thConduct;				//�M�`����
	double heatTrans;				//�M�`�B��
	double roh;						//���x
	double specificHeat;			//��M
//	double dt;						//���ԍ��ݕ�

	//%%%%%%%%		�o�C�i���X�C�b�`�̐錾		%%%%%%%%//
	bool deformed;					//�`�󂪕ς�������ǂ���
	
	unsigned long StepCount;			//	Step���̃J�E���g�A�b�v		// 
	unsigned long StepCount_;			//	Step���̃J�E���g�A�b�v		// 
	std::ofstream templog;

};




}	//	namespace Spr
#endif
