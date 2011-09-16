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
	///�M�`���V�~�����[�V�����ŃG���W�����p����X�e�b�v
	void Step(double dt);

	//�s��̐����@�������͗v�f�����s��A�啶���͑S�̍����s��
	//�ߓ_���x�x�N�g����	���x��	TVec:�v�f�i�c�j�x�N�g��(4x1)	TVecall:�S�́i�c�j�x�N�g��(nx1)
	//�M�`���}�g���N�X��	k:k1~k3�̉��Z��������,	k1:�̐ϕ���,	�ʐϕ���	k2:�M�`�B���E����,	k3:�M�t�ˋ��E����	�ϕ�
	//�M�e�ʃ}�g���N�X��	c:�̐ϕ���
	//�M�����x�N�g����		f:f1~f4�̉��Z��������,	�̐ϕ���	f1:�������M�ɂ�鍀,	�ʐϕ���	f2:�M�������E����,	f3:�M�`�B���E����,	f4:�M�t�ˋ��E����
	//�̐ϕ��̏ꍇ:�v�f1�ɂ�1�̍s��A�ʐϕ��̏ꍇ:�v�f���̊e�ʂ��Ƃ�1�Ōv4�̍s��Ȃ̂Ŕz��ɓ����
	//k��f�Ɋւ��ẮA�ʂ��ƂɌv�Z�����W���s����i�[����z��Mat(k/f)array���`
	
	//�ߓ_���x�x�N�g��
	PTM::TMatrixCol<4,1,double> TVec;		//�v�f�̐ߓ_���x�x�N�g��
	PTM::VMatrixCol<double> TVecAll;		//�S�̂̐ߓ_���x�x�N�g��

	//�v�f�̌W���s��
	PTM::TMatrixRow<4,4,double> Matk1;		//CreateMatk1()
	PTM::TMatrixRow<4,4,double> Matk2;		//CreateMatk2()
	//int Matk2array[4];						//matk2���������z��		//CreateMatk2array()
	PTM::TMatrixRow<4,4,double> Matk2array[4];	//k21,k22,k23,k24��4�~4�s��̓��ꕨ
	PTM::TMatrixRow<4,4,double> Matk;		//Matk=Matk1+Matk2+Matk3	matk1~3�����������v�f�����s��	CreateMatkLocal()
	PTM::TMatrixRow<4,4,double> Matc;		//
	PTM::TMatrixCol<4,1,double> Matf3;		//f3:�O���̖ʂɖʂ��Ă���ʂ̂݌v�Z����@�v����
	int Matf3array[4];						//
	PTM::TMatrixCol<4,1,double> Matf;		//f1~f4�����Z�����c�x�N�g��

	//�S�̂̌W���s��
	PTM::VMatrixRow<double> MatKall;		//[K]�̑S�̍����s��		//CreateMatKall()
	PTM::VMatrixRow<double> MatCall;		//[C]
	PTM::VMatrixRow<double> MatFall;		//{F}�̑S�̍����x�N�g��

	void SetVerticesTemp(double temp);		//�i�ߓ_���x�̍s����쐬����O�Ɂj���_�̉��x��ݒ肷��i�P�ʐێ����j
	//�M�`�B���E�����̎��͂��ׂĂ̈����𖞂����@���x�Œ苫�E������p�������Ƃ��ɂ́A�M�`�B���i�Ō�̈����j����͂��Ȃ��B�܂��A��Step���s���ɓ���ߓ_�̉��x����艷�x�ɕۂ悤�ɂ���B
	void SetInitThermoConductionParam(double thConduct,double roh,double specificHeat,double heatTrans);		//�M�`�����A���x�A��M�A�M�`�B���Ȃǂ̃p�����[�^�[��ݒ�E���


protected:
	//�M�`���v�Z�{��
	//[K]�M�`���}�g���N�X�����֐��Q
	void CreateMatk1();
	void CreateMatk2(Tet tet);		//�l�ʑ̂��Ƃɍ��̂ŁA�l�ʑ̂������Ɏ��
	void CreateMatk2array();
	void CreateMatkLocal();
	void CreateMatKall();

	void CreateMatTest();
	void CreteC();
	void CreateF();
	void CreateTempMatrix();	//�ߓ_�̉��x���������ߓ_�z�񂩂�A�S�̏c�x�N�g�������B	���̏c�s��̐ߓ_�̕��я��́Ai�Ԗڂ̐ߓ_ID��i�Ȃ̂���
	void CreateMatrix();
	double CalcTriangleArea(int id0, int id2, int id3);		//�ߓ_ID������ƁA���̓_�ō\�����ꂽ�O�p�`�̖ʐς����߂�@�l�ʑ̂ł̖ʐϕ��Ŏg�p
	double CalcTetrahedraVol(int tetra);					//�l�ʑ̂�ID������ƁA���̑̐ς��v�Z���Ă����֐�

	//�M�v�Z�ɗp����p�����[�^
	double thConduct;		//�M�`����
	double heatTrans;		//�M�`�B��
	double roh;				//���x
	double specificHeat;	//��M
	double dt;				//���ԍ��ݕ�
	
};




}	//	namespace Spr
#endif
