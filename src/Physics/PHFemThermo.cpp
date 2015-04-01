/*
 *  Copyright (c) 2003 - 2011, Fumihiro Kato, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include <SprDefs.h>
#include <Scilab/SprScilab.h>
#include <Physics/PHFemThermo.h>
#include <Base/Affine.h>
#include <iomanip> 
//#include <Framework/FWObject.h>
//#include <Framework/sprFWObject.h>

#include "windows.h"

#include <GL/glew.h>
#include <GL/glut.h>

#include <Foundation/UTClapack.h>

#define FEMLOG(x)

//%%%%%			�����p�ݒ�p�����[�^API�����Ĉړ��\��			%%%%%

// ��Ή��M�p�����[�^
//#define inr 0.048
//#define outR 0.052
//#define weekPow 231.9 * 1e3 * 0.02 // 231.9 * 1e3:�Z�o�����l�A0.02�F�����X�e�b�v���� 
	//#define weekPow 107		//100:[W]=[J/s]  //50.5801 // �S��1/4�����̖ʐςɉ������M�M��  	//�s��쐬���[J/(m^2�Esec)]�Ȃ�

//	�Q�d�����M�̂��߂̓d�C��R�����x�ˑ���������ꍇ�̃��f���B
//#define TempDependHeat		// 

#if 0
	#define THCOND 83.5 // W/(m K) = [J/ (m�EK�Es)] //67
#endif
	#define RHO	7874	//	
#if 0
	#define SPECIFICHEAT 459.94// 298.15K:447.130, 328.15K(30��):459.94, 400K:490.643	
#endif

#define dMatkUp 0.015
#define dMatKinf 0.0001
	//#define inr 0.034	//0.048
	//#define outR 0.079	//0.052
//#define weekPow 231.9 * 0.02

//[K][C]{F}�̓���m�F�����Ŏg�p
//#define weekPow 0.5281 / 0.02		// J/sec -> J /step sec�@�ɕϊ�����K�v������A�ݒ�l��1�b������̒l�ɂ��A�v�Z�ŃX�e�b�v���Ԃɍ��킹��

//�s��̃e�X�g
//#define weekPow 0.0

//#define mai.cpp �� only1deg

//���̒l
//7.874 * 10^3	 // ���x	7.874 * 10^3 [kg m^-3]
//24.97�@// ��M	J/(K�Ekg)
//#define THCOND 0.618//:�� 
//#define RHO	1.0e6
//#define SPECIFICHEAT 4.2

#define WaterLatentHeat	2257000	//���̏������M [J/kg]

//	%%%%%%%		��������@��������

//���E����
#define NOTUSE_HEATTRANS_HERE

//�����M�ʁ~�R�ɂ��t���b�g�����ȃR�C�����M���g�����ۂ�
#define DISABLE_COIL

//#define badMatK
#define UseMatAll
#define UseGaussSeidel
//#define outMatK
//#define UseScilab
//#define DEBUG
//#define DumK		//debug�p��dummyK?	�g���Ă��Ȃ�

//	LMS�\���Ȃǂ�DSTR���邩�ǂ���SW
//#define HYOKA


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

using namespace PTM;

namespace Spr{;

PHFemThermoDesc::PHFemThermoDesc(){
	Init();
}
void PHFemThermoDesc::Init(){
#if 1
	thConduct = 83.5;		//THCOND;
	thConduct_x = 83.5;
	thConduct_y = 83.5;
	thConduct_z = 83.5;
#endif
	rho = 7874;	//RHO;
	heatTrans = 25;//0;//25;
	specificHeat = 459.94;//SPECIFICHEAT;//0.196;		//1960
	radiantHeat =0;
	initial_temp=0.0;
}

///////////////////////////////////////////////////////////////////
//	PHFemThermo

PHFemThermo::PHFemThermo(const PHFemThermoDesc& desc, SceneIf* s){
	deformed = true;			//�ϐ��̏������A�`�󂪕ς�������ǂ���
	doCalc = true;
	//%%%	�K�E�X�U�C�f������
	NofCyc = 100;
	epsilonG = 1.0;
	//%%%	��������
	jout = 22.9;	//A5052�M�`�B������:22.9, SUS430���M������:29.9	//(48.0+30.0)/2.0;		// 150:77.85, 200:94.25, 100:58.7
	ems = 0.0;//3.8e-2;//3.63e-2;//3.58		//	�ߓ_�ł̔M�t�ˌW���F���x�̍����ɔ�Ⴗ��l�Ȃ̂ŁA3.58e-2	SUS430�ł̒l
	ems_const = 0.0;//-1.14;//-1.1507//-1.063;
	temp_c = 30.0;
	temp_out = 20.0;//23.8;	//A5052����������:23.8, SUS430���M������:29.9

	//%%%%%4
	weekPow_FULL = 120.0;
	weekPow_ = 77.0;
	inr_ = 0.0355;
	outR_ = 0.0825;
	//%%%%%
	weekPow_add = 3.1;
	inr_add = 0.009;
	outR_add = 0.03;
	//%%%%%
	weekPow_decr = 2.2;		//�@�l�̓v���X�����A�v�Z�֐����Ń}�C�i�X�ɐ���		//2.1
	inr_decr = 0.039;
	outR_decr = 0.041;
	//%%%
	stopTime = 180.3;

	// �f�B�X�N���v�^�ɃZ�b�g
	SetDesc(&desc);
	if (s){ SetScene(s); }
	StepCount =0;				// �X�e�b�v���J�E���^
	StepCount_ =0;				// �X�e�b�v���J�E���^

}

void PHFemThermo::Init(){

	weekPow_FULL = weekPow_full;
	matkupSwitch = true;
	
	PHFemMeshNew* mesh = GetPHFemMesh();
	PHFemThermoIf* phm = mesh->GetPHFemThermo();

	int nSurfaceVtxs = NSurfaceVertices();
	for(unsigned i=0; i < (unsigned) nSurfaceVtxs; i++){
		int vtxid = GetSurfaceVertex(i);
		//�~�����b�V���̒�ʏ�ɂ��钸�_
		if(GetVertexPose(vtxid).y < -0.00749){
			bottomVertexNums.push_back(vtxid);
		}
		//�~�����b�V���̏�ʏ�ɂ��钸�_
		else if(GetVertexPose(vtxid).y > 0.00749){
			topVertexNums.push_back(vtxid);
		}
	}

	////�e�l�ʑ̂ƒ��_�ԍ����o��
	//if(strcmp(mesh->GetName(), "femNsteak") == 0){
	//	for(unsigned i=0; i < mesh->NTets(); i++){
	//		DSTR << "tetID:" << i << " vtxIDs";
	//		for(unsigned j=0; j < 4; j++){
	//			DSTR << mesh->tets[i].vertexIDs[j] << ",";
	//		}
	//		DSTR << std::endl;
	//	}
	//}

	////�e���_�̍��W���o��
	//if(strcmp(mesh->GetName(), "femNsteak") == 0){
	//	for(unsigned i=0; i < mesh->NVertices(); i++){
	//		FemVertex vtx = mesh->vertices[i];
	//		DSTR << "���_" << i << ":(" << vtx.pos.x << "," << vtx.pos.y << "," << vtx.pos.z << ")" << std::endl;
	//	}
	//}

	////���ׂĂ̒��_����]
	//if(strcmp(mesh->GetName(), "femNsteak") == 0){
	//	Quaterniond rot = Quaterniond::Rot(Rad(80.0), 'x');
	//	for(unsigned i=0; i < mesh->vertices.size(); i++){
	//		mesh->vertices[i].pos = rot * mesh->vertices[i].pos;
	//		//mesh->vertices[i].pos[0] = (-1) * mesh->vertices[i].pos[0];
	//		//mesh->vertices[i].pos[2] = (-1) * mesh->vertices[i].pos[2];
	//	}
	//	//��]��̒��_�̍��W���o��
	//	for(unsigned i=0; i < mesh->NVertices(); i++){
	//		FemVertex vtx = mesh->vertices[i];
	//		DSTR << "���_" << i << ":(" << vtx.pos.x << "," << vtx.pos.y << "," << vtx.pos.z << ")" << std::endl;
	//	}
	//}

#ifdef UseScilab
	if(strcmp(mesh->GetName(), "femNsteak") == 0){
		if (!ScilabStart()) DSTR << "Error : ScilabStart \n";
		else DSTR << "ScilabStart \n";
	}
#endif

	for(unsigned i = 0; i < mesh->vertices.size(); i++){
		StateVertex vars;
		vertexVars.push_back(vars);
	}
	for(unsigned i = 0; i < mesh->edges.size(); i++){
		StateEdge vars;
		edgeVars.push_back(vars);
	}
	for(unsigned i = 0; i < mesh->faces.size(); i++){
		StateFace vars;
		faceVars.push_back(vars);
	}
	for(unsigned i = 0; i < mesh->tets.size(); i++){
		StateTet vars;
		tetVars.push_back(vars);
	}
	//%%%	��������		%%%//

	//�e�탁���o�ϐ��̏������˃R���X�g���N�^�łł����ق���������������Ȃ��B
	///	Edges
	for(unsigned i =0; i < mesh->edges.size();i++){
		edgeVars[i].c = 0.0;	
		edgeVars[i].k = 0.0;
	}
	if(GetPHFemMesh()->GetName() == "fwNsteak"){
		DSTR << "this is fwNsteak" << std::endl;
	}
	if(GetPHFemMesh()->GetName() == "fwPan"){
		DSTR << "this is fwPan" << std::endl;
	}
	if(GetPHFemMesh()->GetName() == "fwNegi"){
		DSTR << "this is fwNegi" << std::endl;
	}
	if(GetPHFemMesh()->GetName() == "metal5x1x2_fem"){
		DSTR << "this is metal5x1x2_fem" << std::endl;
	}
	DSTR<< "My name is ... " << GetPHFemMesh()->GetName() <<std::endl;
	///	faces
	for(unsigned i=0;i<mesh->faces.size();i++){
		faceVars[i].alphaUpdated = true;
		faceVars[i].area = 0.0;
		faceVars[i].heatTransRatio = 0.0;
		faceVars[i].deformed = true;				//������Ԃ́A�ό`��Ƃ���
		for(unsigned j=0;j<4;++j){
			faceVars[i].fluxarea[j] =0.0;
		}
		faceVars[i].map = INT_MAX;	//
		//faces[i].thermalEmissivity =0.0;
		//faces[i].thermalEmissivity_const =0.0;
		//faces[i].heatflux.clear();				// ������
		//faces[i].heatflux[hum]�̗̈�m�ہF�z��Ƃ��āA���Avector�Ƃ��Ă�push_back���A�ǂ��炩���s���B�z��Ȃ炱���ɋL�q�B
		for(unsigned mode =0; mode < HIGH +1 ; mode++){			// ���M���[�h�̐������A�x�N�g���𐶐�
			for(unsigned j=0;j<4;++j){
				faceVars[i].heatflux[mode][j] = 0.0;
			}
		}
	}

	//�s��̐������Ȃǂ�������
	bVecAll.resize(mesh->vertices.size(),1);
	TVecAll.resize(mesh->vertices.size());
	preTVecAll.resize(mesh->vertices.size());

	//	���b�V���ߓ_���x ������
	//SetVerticesTempAll(30.0);

	//	��C�ւ̔M�`�B�ɂ��M�t�ˋ��E�����ɂ��v�Z���s�̐ݒ�
	SetVerticesTempAll(jout);
	SetVerticesTempAll(initial_temp);
	SetVerticesPreTempAll(initial_temp);

	//matkcheck.open("matkcheck.csv",std::fstream::out,std::fstream::app);
	matkcheck.open("matkcheck.csv");
	matkcheck << "jout,ems,ems_const" << std::endl;
	matkcheck << jout <<","<< ems <<","<< ems_const << std::endl;

//	vecFOutPan.open("vecFOutPan.csv");
	vecFOutSteak.open("vecFOutSteak.csv");
	
//	vecFOutSteak << "����" << ",";
//	for(unsigned i=0; i < mesh->vertices.size();i++){
//		if(i != mesh->vertices.size() -1){
//			vecFOutSteak << "���_" << i << ", ";	
//		}
//		else{
//			vecFOutSteak << "���_" << i << std::endl;
//		}
//	}

	
	//�]�������p�R�[�h
	//>	LMSS�̂��߂̃R�[�h
	//	�ߓ_���x�̏����ݒ�(�s������O�ɍs��)

	//	j�ȉ��̉��x�͑S�̂����̉��x�ɂ��Đݒ�
	//SetVerticesTempAll((158.5 + (30.0 + 158.5) /2.0)/2.0 );
	//SetVerticesTempAll((158.5+30.0)/2.0);
	


//	jout = 77.85;
//	ems = 4.9258;
	
	// main.cpp�œ����悤�ɐݒ肷��
	round.clear();
	tempe.clear();
	for(unsigned i=0;i<11;++i){
		round.push_back(i*0.01);
	}
	//%%%		���M�̃p�����[�^
	//	200��������M
	//tempe.push_back(211.3);
	//tempe.push_back(211.2);
	//tempe.push_back(210.2);
	//tempe.push_back(207.6);
	//tempe.push_back(203.9);

	//tempe.push_back(198.0);
	//tempe.push_back(189.3);
	//tempe.push_back(178.7);
	//tempe.push_back(169.8);
	//tempe.push_back(158.5);
	
	//	150��������M
	//double tempe2[10] = {158.6, 158.3, 156.8, 153.9, 150.4, 146.0, 140.3, 134.8, 130.8, 125.7};
	//tempe.push_back(158.6);
	//tempe.push_back(158.3);
	//tempe.push_back(156.8);
	//tempe.push_back(153.9);
	//tempe.push_back(150.4);

	//tempe.push_back(146);
	//tempe.push_back(140.3);
	//tempe.push_back(134.8);
	//tempe.push_back(130.8);
	//tempe.push_back(125.7);

	//150��
	//tempe.push_back(100.0);
	//tempe.push_back(100.0);
	//tempe.push_back(99.4);
	//tempe.push_back(98.2);
	//tempe.push_back(96.8);

	//tempe.push_back(95.0);
	//tempe.push_back(92.6);
	//tempe.push_back(90.6);
	//tempe.push_back(89.3);
	//tempe.push_back(87.4);
	
	//50	10sec
	//tempe.push_back(50.0);
	//tempe.push_back(50.0);
	//tempe.push_back(49.9);
	//tempe.push_back(49.8);
	//tempe.push_back(49.7);

	//tempe.push_back(49.3);
	//tempe.push_back(48.7);
	//tempe.push_back(48.5);
	//tempe.push_back(48.4);
	//tempe.push_back(48.0);

	//	50	�X�V�^�C�~���O 
	//tempe.push_back(50.0);
	//tempe.push_back(49.9);
	//tempe.push_back(49.9);
	//tempe.push_back(49.7);
	//tempe.push_back(49.7);

	//tempe.push_back(49.2);
	//tempe.push_back(48.7);
	//tempe.push_back(48.4);
	//tempe.push_back(48.3);
	//tempe.push_back(48.0);

	//%%%		���M�̃p�����[�^
	//	�͂��߂���@10sec 10:01:56
	//tempe.push_back(29.7);
	//tempe.push_back(29.7);
	//tempe.push_back(29.6);
	//tempe.push_back(29.8);
	//tempe.push_back(29.7);

	//tempe.push_back(29.7);
	//tempe.push_back(29.7);
	//tempe.push_back(29.7);
	//tempe.push_back(29.8);
	//tempe.push_back(29.8);



	//%%%%%		�������x�Ƃ��āA�~��̉��x���z���g�������Ƃ��Ɏg��	SetConcentricHeatMap()�ƈꏏ�Ɏg��
//	SetConcentricHeatMap(round,tempe,Vec2d(0.0, -0.005));		//	-0.001�ɂ��Ă��A�������x�͕s�ς������B 7/4�R�����g�A�E�g shiba
	//DSTR << "���� check it out" <<std::endl;
	//for(unsigned i=0; i< tempe.size();++i){
	//	DSTR << round[i] << "; "<< i*0.01 <<"; " <<  tempe[i] << std::endl;
	//}
	//���͂ւ̔M�`�B���x�̏�����(temp�x�ɂ���)�@���@�M�t��
	InitTcAll(temp_c);
	InitToutAll(temp_out);
	SetThermalEmissivityToVerticesAll(ems,ems_const);	
	for(int id=0;id<NSurfaceVertices();++id){
		vertexVars[id].beRadiantHeat =true;
	}
	///	�M�`�B�����e�ߓ_�Ɋi�[
	SetHeatTransRatioToAllVertex();
	
	DSTR << "temp_c " << temp_c <<std::endl;
	//>	�M���ˊ֌W	LMSS
	DSTR << "temp_out "<< temp_out <<std::endl; 
	DSTR << "ems "<< ems <<std::endl; 
	//SetThermalEmissivityToVerticesAll(radiantHeat);				///	�b��l0.0�ŏ�����	�F�M���˂͂��Ȃ���	

	//vertices.temp�����ׂāATVecAll�֑������
	CreateTempVertex();

	//�M�`�����A���x�A��M�A�M�`�B���@�̃p�����[�^�[��ݒ�E���
		//PHFemThermo�̃����o�ϐ��̒l���� CADThermo���A0.574;//�ʂ˂��̒l//�M�`����[W/(���EK)]�@Cp = 1.96 * (Ndt);//�ʂ˂��̔�M[kJ/(kg�EK) 1.96kJ/(kg K),�i�ʂ˂��̖��x�j�H�i���M�̉Ȋwp64���970kg/m^3
		//�M�`�B���̒P�ʌn�@W/(m^2 K)�˂����SI�P�ʌn�Ȃ̂��H�@25�͘_��(MEAT COOKING SIMULATION BY FINITE ELEMENTS)�̃I�[�u�����M���̎����l
		//SetInitThermoConductionParam(0.574,970,1.96,25);
	//. �M�`�B���� SetInitThermoConductionParam(0.574,970,0.1960,25 * 0.001 );		//> thConduct:�M�`���� ,roh:���x,	specificHeat:��M J/ (K�Ekg):1960 ,�@heatTrans:�M�`�B�� W/(m^2�EK)
	//. �M�`�B���Ȃ�
//	SetInitThermoConductionParam(THCOND,RHO,SPECIFICHEAT,0 );		// �M�`�B��=0;�ɂ��Ă���w	//	SetInitThermoConductionParam(0.574,970,0.1960,0 );
	DSTR << "thConduct:" << thConduct << std::endl;
	

	//�f�M�ߒ�
	//SetInitThermoConductionParam(0.574,970,0.1960,0.0);		//> thConduct:�M�`���� ,roh:���x,	specificHeat:��M J/ (K�Ekg):1960 ,�@heatTrans:�M�`�B�� W/(m^2�EK)
	//�����A�ϐ��l�͌ォ��v�Z�̓r���ŕύX�ł���悤��SetParam()�֐�������Ă������ق����������ȁH

	//> �M�����̏�����
	//SetVtxHeatFluxAll(0.0);


	//>	�s��̍쐬�@�s��̍쐬�ɕK�v�ȕϐ��͂��̍s�ȑO�ɐݒ肪�K�v
		//�v�Z�ɗp����}�g���N�X�A�x�N�g�����쐬�i���b�V�����Ƃ̗v�f�����s��/�x�N�g���ˑS�̍����s��/�x�N�g���j
		//{T}�c�x�N�g���̐ߓ_�̕��я��ɕ��Ԃ悤�ɁA�W���s������Z����B�W���s��ɂ́A�ʐς�̐ρA�M�`�B���Ȃǂ̃p�����[�^�̐ς����Ă��܂������̂�����B


	//> IH���M����face��������x(�\��face && �����)�i��A�֌W��������face�ߓ_�̌��_����̋������v�Z���Aface[].mayIHheated�𔻒�
	//CalcVtxDisFromOrigin();
	CalcVtxDisFromVertex(Vec2d(0.0, -0.005));		// �S�̌��_���W�������ɓ���A�Ώ̐��������悤�ɂȂ��Ă���B
	
	//>	IH����̒P�ʎ��ԓ�����̉��M�M��
	//�P�ʎ��ԓ�����̑����M�M��	231.9; //>	J/sec
		
	//..debug 
	//�o���h����M
	//	CalcIHdqdtband_(-0.02,0.20,231.9 * 0.005 * 1e6);

	
	//%%	IH���M�̃��[�h�ؑ�
	//	���C����ɉ��M
	//	CalcIHdqdtband_(0.09,0.10,231.9 * 5e3);		//*0.5*1e4	�l��ς��Ď���	//*1e3�@//*1e4 //5e3
	//	�~��ɉ��M
	
	//CalcIHarea(0.04,0.095,231.9 * 0.005 * 1e6);		//	�g���Ă���H



	//	���̌�ŁA�M�����x�N�g�����v�Z����֐����Ăяo��
	InitCreateMatC();					///	CreateMatC�̏�����
	InitVecFAlls();					///	VecFAll�ނ̏�����
	InitCreateMatk();					///	CreateMatK�̏�����
	//..	CreateLocalMatrixAndSet();			//> �ȏ�̏������A���̊֐��ɏW��

	keisuInv.resize(mesh->vertices.size(),mesh->vertices.size());
	keisuInv.clear();
	rightkeisu.resize(mesh->vertices.size(),mesh->vertices.size());
	rightkeisu.clear();

	//std::ofstream matktetout;
	////�e�l�ʑ̂�matK���o��
	//if(strcmp(mesh->GetName(), "femNsteak") == 0){
	//	matktetout.open("matktetout.csv");
	//}

	//	�l�ʑ̂��ƂɌv�Z
	for(unsigned i=0; i < mesh->tets.size(); i++){

#if 1
		/*���쌴�ǉ���������--------------------------------------------*/
		//�\��face�̖ʐς��v�Z
		for(unsigned j= 0 ; j < 4; j++){
			if(mesh->tets[i].faceIDs[j] < (int)mesh->nSurfaceFace){			///	�O�k�̖�
				///	�l�ʑ̂̎O�p�`�̖ʐς��v�Z		///	���̊֐��̊O�Ŗʐϕ��̖ʐόv�Z����������B�ړ�����
				faceVars[mesh->tets[i].faceIDs[j]].area = CalcTriangleArea(mesh->faces[mesh->tets[i].faceIDs[j]].vertexIDs[0], mesh->faces[mesh->tets[i].faceIDs[j]].vertexIDs[1], mesh->faces[mesh->tets[i].faceIDs[j]].vertexIDs[2]);
			}
		}
		/*���쌴�ǉ������܂�--------------------------------------------*/
#endif
		tetVars[i].volume = CalcTetrahedraVolume2(i);

		//�e�s�������āA�K�E�X�U�C�f���Ōv�Z���邽�߂̌W���̊�{�����BTimestep�̓����Ă��鍀�́A���̃\�[�X(SetDesc())�ł́A�����ł��Ȃ����Ƃ���������(NULL���Ԃ��Ă���)
		CreateMatkLocal(i);				///	Matk1 Matk2(�X�V���K�v�ȏꍇ������)�����	//ifdef�X�C�b�`�őS�̍����s���(�\���p����)�����\
		//CreateMatKall();		//CreateMatkLocal();�Ɏ��������̂ŁA���������B
		CreatedMatCAll(i);
		CreateVecFAll(i);

	//	//�l�ʑ̂��Ƃ�matk���o��
	//	if(strcmp(mesh->GetName(), "femNsteak") == 0){
	//		//matktetout << i << std::endl;
	//		DSTR << i << std::endl;
	//		for(unsigned k=0; k < 4; k++){
	//			for(unsigned l=0; l < 4; l++){
	//				//matktetout << matk[k][l] << ",";
	//				DSTR << std::setw(13) << matk[k][l] << "\t";
	//			}
	//			//matktetout << std::endl;
	//			DSTR << std::endl;
	//		}
	//		//matktetout << std::endl;
	//		DSTR << std::endl;
	//	}
	}

	////f�𕪊�����䗦��\��fpat���쐬
	//if(strcmp(mesh->GetName(), "femNsteak") == 0){
	//	makeFPartition(1);
	//	DSTR << "fpat" << std::endl;
	//	for(unsigned i=0; i < mesh->vertices.size(); i++){
	//		for(unsigned j=0; j < mesh->vertices.size(); j++){
	//			DSTR << std::setw(5) << fpat[i][j] << "\t";
	//		}
	//		DSTR << std::endl;
	//	}
	//	DSTR << std::endl;
	//}

	//if(strcmp(mesh->GetName(), "femNsteak") == 0){
	//	matktetout.close();
	//}

	//for(unsigned j =0;j<mesh->vertices.size();j++){
	//	if(matKAll[j][j] != dMatKAll[0][j]){
	//		DSTR << j <<" �����̗v�f�͂��������I�������K�v�ł���B matKAll�F" << matKAll[j][j] << " dMatKAll�F" << dMatKAll[0][j] <<std::endl;
	//	}
	//}

	for(unsigned vtxid=0; vtxid < mesh->vertices.size(); vtxid++){
		CalcVertexVolume(vtxid);
		//if(strcmp(mesh->GetName(), "femNsteak") == 0){
		//	DSTR << vtxid << ":" << vertexVars[vtxid].vVolume << std::endl;
		//}
	}

	TVecAllSub.resize(mesh->vertices.size());
	for(unsigned vtxid=0; vtxid < mesh->vertices.size(); vtxid++){
		TVecAllSub[vtxid] = initial_temp;
	}
//	if(strcmp(mesh->GetName(), "femPan") == 0){
//		for(unsigned vtxid=0; vtxid < mesh->vertices.size(); vtxid++){
//			vecFOutPan << vecFAllSum[vtxid] << ",";
//		}
//		vecFOutPan << std::endl;
//	}
	//if(strcmp(mesh->GetName(), "femNsteak") == 0){
	//	//for(unsigned vtxid=0; vtxid < mesh->vertices.size(); vtxid++){
	//	//	vecFOutSteak << vecFAllSum[vtxid] << ",";
	//	//}
	//	//vecFOutSteak << std::endl;
	//	std::ofstream dtKout;
	//	std::ofstream Cout;
	//	dtKout.open("dtKSteak.txt");
	//	Cout.open("CSteak.txt");

	//	//dtKout << "[";
	//	//Cout << "[";
	//	for(unsigned i=0; i < mesh->vertices.size(); i++){
	//		for(unsigned j=0; j < mesh->vertices.size(); j++){
	//			dtKout << tdt * matKAll[i][j] << " ";
	//			Cout << matCAll[i][j] << " ";
	//		}
	//		dtKout/* << ";" */<< std::endl;
	//		Cout/* << ";" */<< std::endl;
	//	}
	//	//dtKout << "]";
	//	//Cout <<"]"; 

	//	dtKout.close();
	//	Cout.close();
	//}

#if 1
	//���_�̕\�ʖʐς��v�Z�i���쌴�ǉ��j
	calcVerticesArea();
#endif

#if 0

	//	���_�P�̒S���̐ςɑ΂��A�M�ʂ������邽�߂ɁA�S���̐ϊ��Z�ŔM�ʂ𒸓_�̉��x�Ƃ��ė^����B{F}���g��Ȃ��̂ŁA�M�������g��Ȃ��B
	double rcv=0.0;
	for(unsigned i=0;i<vertices[0].tets.size();i++){
		rcv += tets[vertices[0].tets[i]].volume * RHO * SPECIFICHEAT * 5 / 20; 
	}
	double kuwae =1.58;	//	������M��
	//vertices[0].temp = kuwae / rcv;
	SetVertexTemp(0,kuwae / rcv);
#endif
	//����temp��TVecAll�ɐݒ�
	//C,K�����̌v�Z�������ėl�q������


	int hogeshidebug =0;
	//	�ߓ_���x���ڂ̏����o��
//	templog.open("templog.csv");

	//matCAllout.open("matCAllout.txt"); 
	//matKAllout.open("matKAllout.txt");
	//
	//matCAllout << matCAll << std::endl;
	//matKAllout << matKAll << std::endl;
	//
	//matCAllout.close();

	//matCAllout.open("matCAll-1out.txt");
	//matCAllout << matCAll.inv() << std::endl;
	//matCAllout.close();
	////scilab���Ă΂Ȃ��ŁASPR�̋@�\�ŁAC.inv() K �����߂�
	//matCAllout.open("matCAll.inv()xmatKAll.txt");
	//matCAllout << matCAll.inv() * matKAll << std::endl; 
	//matCAllout.close();

	//tempMat.resize(vertices.size(),vertices.size());
	//tempMat.clear();
	//tempMat = matCAll.inv() * matKAll;

	//std::vector<double> rowval;
	//double tempval=0;
	//for(unsigned i=0;i<vertices.size();i++){
	//	for(unsigned j=0;j<vertices.size();j++){
	//	tempval += tempMat[i][j];
	//	}
	//	rowval.push_back(tempval);
	//}
	//matCAllout.open("matCAll.inv()xmatKAll�̊e��̘a.txt");
	//for(unsigned i=0;i<vertices.size();i++){
	//	matCAllout << rowval[i] <<std::endl;
	//}
	//matCAllout.close();

	//�J�E���g�̏�����
	COUNT = 0;

	//���x�ω��o��
	checkTVecAllout.open("checkTVecAllout.csv");
	checkTVecAllout <<"����" << COUNT<<", ";
	for(unsigned i=0; i < mesh->vertices.size();i++){
		if(i != mesh->vertices.size() -1){
			checkTVecAllout << "���_" << i << ", ";	
		}
		else{
			checkTVecAllout << "���_" << i << std::endl;
		}
	}
	FEMLOG.open("femLog.csv");

	//	CPS�̌o���ω��������o��
	//cpslog.open("cpslog.csv");

	// �J�E���g�̏�����
	Ndt =0;

	//������������̏�����
	//InitMoist();

	//this->OutputMatKall();

	InitFaceNormalAll();
	CalcFaceNormalAll();
	InitVertexNormalAll();
	CalcVertexNormalAll();

//#ifdef UseMatAll
//	if(strcmp(mesh->GetName(), "femNsteak") == 0){
//		OutEig();
//	}
//#endif

	I.resize(mesh->vertices.size(), mesh->vertices.size());
	I.clear();
	for(unsigned i=0; i < mesh->vertices.size(); i++){
		I[i][i] = 1.0;
	}
	makeAFlag = true;
	if(strcmp(mesh->GetName(), "femNsteak") == 0){
		convTest.resize(mesh->vertices.size());
		ggConvOut.open("ggConvOut.csv");
		for(unsigned i=0; i < mesh->vertices.size(); i++){
			ggConvOut << "���_" << i << ",";
		}
		ggConvOut << std::endl;
	}

}

void PHFemThermo::SetStopTimespan(double timespan){
	stopTime = timespan;
	//	�{���́APHScene�����Ԃ�manage���ׂ��Ȃ��߁A�b��I
}
void PHFemThermo::SetIHParamWEEK(double inr2, double outR2, double weekPow2){
	inr_ = inr2;
	outR_ = outR2;
	weekPow_ = weekPow2;
}
void PHFemThermo::SetWeekPow(double weekPow2){
	weekPow_ = weekPow2;
}

void PHFemThermo::SetOuterTemp(double temp){
	jout = temp;
}
void PHFemThermo::SetThermalRadiation(double ems_,double ems_const_){
	ems = ems_;
	ems_const = ems_const_;
}
void PHFemThermo::SetGaussCalcParam(unsigned cyc,double epsilon_){
	NofCyc = cyc;
	epsilonG = epsilon_;
}

double PHFemThermo::CalcTempInnerTets(unsigned id,PTM::TVector<4,double> N){
	PHFemMeshNew* mesh = GetPHFemMesh();

	double temp = 0.0;
	for(unsigned i=0;i<4;i++){
		 temp += N[i] * vertexVars[mesh->tets[id].vertexIDs[i]].temp;
	}
	return temp;
};


double PHFemThermo::GetVtxTempInTets(Vec3d temppos){
	PHFemMeshNew* mesh = GetPHFemMesh();

	PTM::TMatrixCol<4,4,double> Vertex;		//	�l�ʑ̂𐬂�4�_�̈ʒu���W
	PTM::TVector<4,double> coeffk;			//	�`��֐��I�ȁH
	PTM::TVector<4,double> arbitPos;		//	�C�ӓ_���W
	// [a][x] = [b]������
	Vertex.clear();		//a
	coeffk.clear();		//x
	arbitPos.clear();	//b
	// b: ax =b �F�P�ɋt�s�񂩂狁�߂�Ƃ�
	arbitPos[0] = temppos[0];
	arbitPos[1] = temppos[1];
	arbitPos[2] = temppos[2];
	arbitPos[3] = 1.0;
	for(unsigned id =0;  id < mesh->tets.size(); id++){
		// �l�ʑ̂��Ƃɓ_���܂܂�邩����		
		for(unsigned j=0; j < 4;j++){
			Vertex[0][j] = mesh->vertices[mesh->tets[id].vertexIDs[j]].pos.x;
			Vertex[1][j] = mesh->vertices[mesh->tets[id].vertexIDs[j]].pos.y;
			Vertex[2][j] = mesh->vertices[mesh->tets[id].vertexIDs[j]].pos.z;
			Vertex[3][j] = 1.0;
		}
		// �t�s��ŉ���
		coeffk = Vertex.inv() * arbitPos;
		//	�l�ʑ̂�
		if( 0-1e-8 <= coeffk[0] && coeffk[0] <= 1+1e-8 && 0-1e-8 <= coeffk[1] && coeffk[1] <= 1+1e-8 && 0-1e-8 <= coeffk[2] && coeffk[2] <= 1+1e-8 && 0-1e-8 <= coeffk[3] && coeffk[3] <= 1+1e-8 ){	//	�ߐڎl�ʑ̂ɓ����Ă��܂��ꍇ�����肻���B���̋�ʂ����Ȃ��̂ŁA0��1�ŋ�؂�����ǂ��Ǝv���B
			//	�`��֐�����A�l�ʑ̓��̉��x�����߂�
			return CalcTempInnerTets( id , coeffk);		
		}
		coeffk.clear();
	}
	return DBL_MAX;		//	������Ȃ������T�C��
}



struct ID_LENGTH{
	unsigned id;
	double coord;	//value of axis
};
class LessLength{
public:
	bool operator()(const ID_LENGTH& a, const ID_LENGTH& b)
	{
		return a.coord < b.coord;
	}
};

Vec3d PHFemThermo::GetDistVecDotTri(Vec3d Dotpos,Vec3d trivtx[3]){
	//				 a
	//				/|
	//			   / |
	//		.Q	  / P|
	//			 / . |
	//	.		/____|
	//	O		b   c
	//	triedge[0] = b->a, triedge[1] = b->c,
	//  QP =  OP - OQ = Ob + param[0] * ba + param[1] * bc - OQ
	//	QP��ba,QP��bc => param[0~1]�����߂� 
	double param[2] = {0.0,0.0}; 
	Vec3d triedge[2] = {Vec3d(0.0,0.0,0.0),Vec3d(0.0,0.0,0.0)};
	//
	triedge[0] = trivtx[1] - trivtx[0];
	triedge[1] = trivtx[2] - trivtx[0];
	param[1] = (triedge[0] * trivtx[0]) * (triedge[0] * triedge[1]) - triedge[0].norm() * triedge[0].norm() * (trivtx[0] * triedge[1])
		/ ( (triedge[0].norm() * triedge[0].norm() )  *  (triedge[1].norm() * triedge[1].norm() ) - (triedge[0] * triedge[1]) * (triedge[0] * triedge[1]) );
	param[0] = (-1) / (triedge[0].norm() * triedge[0].norm()) * ((triedge[0] * triedge[1]) * param[1] + (triedge[0] * trivtx[0]));
	Vec3d VecQP = trivtx[0] + param[0] * triedge[0] + param[1] * triedge[1] - Dotpos;
	return VecQP;
}

#if 1		// �ȉ��A�������r��?
double PHFemThermo::GetArbitraryPointTemp(Vec3d temppos){
	PHFemMeshNew* mesh = GetPHFemMesh();

	//temppos���ǂ̎l�ʑ̂ɑ����邩
	//�l�ʑ̂�face�ʂ̌����Ŕ���
	DSTR << "from origin: (0.0,0.0,0.0) " << std::endl;
	for(unsigned i=0;i<mesh->faces.size(); i++){
		Vec3d facevtx[3] = {mesh->vertices[mesh->faces[i].vertexIDs[0]].pos,mesh->vertices[mesh->faces[i].vertexIDs[1]].pos,mesh->vertices[mesh->faces[i].vertexIDs[2]].pos};
		DSTR << "facevtx[0]: " << facevtx[0] << "facevtx[1]: " << facevtx[1] << "facevtx[2]: " << facevtx[2] << std::endl;
		DSTR <<"i: " << i <<", GetDistVecDotTri(temppos,facevtx): " << GetDistVecDotTri(temppos,facevtx) << std::endl;
		DSTR << std::endl;
	}
	//����������sort���Ă����R���e�i���g�� map? list? ���ɂ́A�����ƒ��_id������B�ŏ�����4�߂܂ł̒��_IDEALLY���܂ގl�ʑ̂�������
	//�ŏ��ɁAface�ƃ}�b�`���O���Ƃ��āA���̌�ŁA����face���܂ގl�ʑ̂Ƃ̃}�b�`���O���Ƃ���@�����肻���B��̓I�ȃA���S���Y���������΂Ȃ�
	double length = 0.0;
	for(unsigned id=0; id < mesh->vertices.size(); id++){
		//��ԋ߂����ɂS�̓_���\�[�g
		length = sqrt( (temppos.x - mesh->vertices[id].pos.x) * (temppos.x - mesh->vertices[id].pos.x)
			+ (temppos.y - mesh->vertices[id].pos.y) * (temppos.y - mesh->vertices[id].pos.y)
			+ (temppos.z - mesh->vertices[id].pos.z) * (temppos.z - mesh->vertices[id].pos.z) 
			);
	}
	//AABB�ł��
	//4�_����x,y,z��min/max
	//���������ʕ��ׂ�
	ID_LENGTH idl;
	std::vector<ID_LENGTH> id_length_x;
	//std::vector<double> xarray,yarray,zarray;
	for(unsigned id=0; id < mesh->tets.size(); id++){
		for(unsigned j=0; j < 4; j++){
			idl.id = mesh->tets[id].vertexIDs[j];
			idl.coord = mesh->vertices[mesh->tets[id].vertexIDs[j]].pos.x;
			id_length_x.push_back(idl);
		}
	}
	DSTR << "begfore .sort()" << std::endl;
	for(unsigned i=0; i < id_length_x.size() ; i++){
		DSTR << "id_length_x["<< i <<"].id: " << id_length_x[i].id << " .coord: " << id_length_x[i].coord << std::endl; 
	}
	//temppos�ɋ߂����ɕ��ׂ�
	std::sort( id_length_x.begin(), id_length_x.end(), LessLength());
	
	DSTR << "after .sort()" << std::endl;
	for(unsigned i=0; i < id_length_x.size() ; i++){
		DSTR << "id_length_x["<< i <<"].id: " << id_length_x[i].id << " .coord: " << id_length_x[i].coord << std::endl; 
	}

	//�ŋ߂ƍŉ���Xmin�Axmax�ɓ����B���̍ۂɁA���_id���K�v�Ȃ̂ŁA��L�\���̂ł́A���W�l(param)�𒲂ׂ�\�[�g�A���S���Y�����L�q����B
	//param���ŏ��l�̎���id�A�ő�l�̂Ƃ���id���g���āA���_�̎��ʂ�����
		
	//AABB����Atemppose�����̎l�ʑ̂̒����A�߂����A���肷��B
	//���Ȃ�ǂ��I
	//������Ȃ���΁A�ߗׂ̎l�ʑ̂�T��
	

	//������l�ʑ̂̌`��֐�����A���x���擾 T=NT���

	//���x���擾�ł�����A�Ԃ��B�_���Ȃ�null��DBL_MAX
	//if(temp) return temp:
	//else	return null;
	return 1;
}
#endif

void PHFemThermo::CalcVtxDisFromOrigin(){
	PHFemMeshNew* mesh = GetPHFemMesh();

	//>	nSurface�̓��Ax,z���W���狗�������߂�sqrt(2��a)�A�����FemVertex�Ɋi�[����
	//> ���S�~�n�̌v�Z�ɗ��p����@distance from origin
	
	/// ����t���O�̏�����
	for(unsigned i=0; i<mesh->nSurfaceFace; i++){
		faceVars[i].mayIHheated = false;
	}
	/// ������
	for(unsigned i =0;i<mesh->vertices.size();i++){
		vertexVars[i].disFromOrigin =0.0;
	}

	/// debug
	//DSTR << "faces.size(): " << faces.size() << std::endl;

	//> �\��face�̓��A���_����eface�̐ߓ_�̃��[�J��(x,z)���W�n�ł̕��ʏ�̋����̌v�Z���Aface�̑S�ߓ_��y���W�����̂��̂ɑ΂��āAIH���M�̉\���������t���O��ݒ�
	for(unsigned i=0;i<mesh->nSurfaceFace;i++){
		//> �\�ʂ�face�̑S�ߓ_��y���W�����Ȃ�΁A����face��IH���M��face�ʂƔ��肵�A�t���O��^����
		if(mesh->vertices[mesh->faces[i].vertexIDs[0]].pos.y < 0.0 && mesh->vertices[mesh->faces[i].vertexIDs[1]].pos.y < 0.0 && mesh->vertices[mesh->faces[i].vertexIDs[2]].pos.y < 0.0){
			faceVars[i].mayIHheated = true;
			//	(x,z)���ʂɂ�����mayIHheated��face�S�ߓ_�̌��_����̋������v�Z����
			for(unsigned j=0; j<3; j++){
				vertexVars[mesh->faces[i].vertexIDs[j]].disFromOrigin = sqrt(mesh->vertices[mesh->faces[i].vertexIDs[j]].pos.x * mesh->vertices[mesh->faces[i].vertexIDs[j]].pos.x + mesh->vertices[mesh->faces[i].vertexIDs[j]].pos.z * mesh->vertices[mesh->faces[i].vertexIDs[j]].pos.z);
			}
		}
	}

	//	debug		//>	�������Ή����ɂ̓R�����g�A�E�g����
	//>	���W�l���m�F����
	for(unsigned i=0; i < mesh->nSurfaceFace; i++){
		if(faceVars[i].mayIHheated){
			/// 3�̒��_�̑g�݁��ӂ�x,z�������ŁAy���W�������قȂ�_�̑g�݂��Ȃ����Ƃ��m�F����
			for(unsigned j=0;j<3;j++){
				if(mesh->vertices[mesh->faces[i].vertexIDs[j]].pos.x == mesh->vertices[mesh->faces[i].vertexIDs[(j+1)%3]].pos.x
					&& mesh->vertices[mesh->faces[i].vertexIDs[j]].pos.z == mesh->vertices[mesh->faces[i].vertexIDs[(j+1)%3]].pos.z
					 && mesh->vertices[mesh->faces[i].vertexIDs[j]].pos.y != mesh->vertices[mesh->faces[i].vertexIDs[(j+1)%3]].pos.y){
					DSTR <<i << "th: " << mesh->vertices[mesh->faces[i].vertexIDs[j]].pos << " : " << mesh->vertices[mesh->faces[i].vertexIDs[(j+1)%3]].pos << " : " << mesh->vertices[mesh->faces[i].vertexIDs[(j+2)%3]].pos<<std::endl;
					DSTR << "CalcVtxDisFromOrigin() error" << std::endl;
					assert(0);
				}
			}
		}
	}
}

void PHFemThermo::CalcVtxDisFromVertex(Vec2d originVertexIH){
	PHFemMeshNew* mesh = GetPHFemMesh();

	//>	nSurface�̓��Ax,z���W���狗�������߂�sqrt(2��a)�A�����FemVertex�Ɋi�[����
	//> ���S�~�n�̌v�Z�ɗ��p����@distance from origin
	
	/// ����t���O�̏�����
	for(unsigned i=0; i<mesh->nSurfaceFace; i++){
		faceVars[i].mayIHheated = false;
	}
	/// ������
	for(unsigned i =0;i<mesh->vertices.size();i++){
		vertexVars[i].disFromOrigin =0.0;
	}
	//> �\��face�̓��A���_����eface�̐ߓ_�̃��[�J��(x,z)���W�n�ł̕��ʏ�̋����̌v�Z���Aface�̑S�ߓ_��y���W�����̂��̂ɑ΂��āAIH���M�̉\���������t���O��ݒ�
	for(unsigned i=0;i<mesh->nSurfaceFace;i++){
		//> �\�ʂ�face�̑S�ߓ_��y���W�����Ȃ�΁A����face��IH���M��face�ʂƔ��肵�A�t���O��^����
		if(mesh->vertices[mesh->faces[i].vertexIDs[0]].pos.y < 0.0 && mesh->vertices[mesh->faces[i].vertexIDs[1]].pos.y < 0.0 && mesh->vertices[mesh->faces[i].vertexIDs[2]].pos.y < 0.0){
			faceVars[i].mayIHheated = true;
			//	(x,z)���ʂɂ�����mayIHheated��face�S�ߓ_�̌��_����̋������v�Z����
			for(unsigned j=0; j<3; j++){
				double dx = mesh->vertices[mesh->faces[i].vertexIDs[j]].pos.x - originVertexIH[0];
				double dz = mesh->vertices[mesh->faces[i].vertexIDs[j]].pos.z - originVertexIH[1];	//	�\�L��y�����A����z���W�������Ă���
				vertexVars[mesh->faces[i].vertexIDs[j]].disFromOrigin = sqrt( dx * dx + dz * dz);
			}
		}
	}
}


void PHFemThermo::ScilabTest(){
	if (!ScilabStart()) std::cout << "Error : ScilabStart \n";

	//	�s��̓ǂݏ���
	Matrix2f A;
	A.Ex() = Vec2f(1,2);
	A.Ey() = Vec2f(3,4);
	std::cout << "A:  " << A ;
	std::cout << "A00:" << A[0][0] << std::endl;
	std::cout << "A01:" << A[0][1] << std::endl;
	std::cout << "A10:" << A[1][0] << std::endl;
	std::cout << "A11:" << A[1][1] << std::endl;
	
	ScilabSetMatrix("A", A);
	ScilabJob("b=[4;5]");
	std::cout << "A=";
	ScilabJob("disp(A);");
	std::cout << "b=";
	ScilabJob("disp(b);");
	std::cout << "x=A\\b" << std::endl;
	ScilabJob("A,b,x=A\\b;");
	
	ScilabGetMatrix(A, "A");
	SCMatrix b = ScilabMatrix("b");
	SCMatrix x = ScilabMatrix("x");
	std::cout << "x:" << x << std::endl;

	ScilabJob("y = A;");
	SCMatrix y = ScilabMatrix("y");
	std::cout << "y=" << y;
	y = 2*A;
	std::cout << "y = 2*A is done by C++ code" << std::endl;
	std::cout << "y=";
	ScilabJob("disp(y);");
	std::cout << A;
	std::cout << y;
	ScilabJob("clear;");

	//	�O���t�`��
	ScilabJob("t = 0:0.01:2*3.141592653;");
	ScilabJob("x = sin(t);");
	ScilabJob("y = cos(t);");
	ScilabJob("plot2d(x, y);");
	for(int i=0; i<100000; ++i){
		ScilabJob("");
	}
//	ScilabEnd();
}

void PHFemThermo::UsingFixedTempBoundaryCondition(unsigned id,double temp){
	//���x�Œ苫�E����
	SetVertexTemp(id,temp);
}

void PHFemThermo::UsingHeatTransferBoundaryCondition(unsigned id,double temp,double heatTransRatio){
	PHFemMeshNew* mesh = GetPHFemMesh();

	//�M�`�B���E����
	//�ߓ_�̎��͗��̉��x�̐ݒ�(K,C,F�Ȃǂ̍s��x�N�g���̍쐬��Ɏ��s�K�v����)
//	if(vertices[id].Tc != temp){					//�X�V����ߓ_��Tc���ω������������ATc��F�x�N�g�����X�V����
	SetLocalFluidTemp(id,temp);
	vertexVars[id].heatTransRatio = heatTransRatio;
	//�M�`�B���E�������g����悤�ɁA����B				///	�{�����ăx�N�g��������Ă���̂ŁA���̃R�[�h�ł́A�]�v�ɑ����Ă��܂��Ă��āA�������s������Ȃ��B
	//for(unsigned i =0;i < vertices[id].tets.size();i++){
	//	CreateVecfLocal(tets[vertices[id].tets[i]]);		//	Tc���܂ރx�N�g�����X�V����
	//}

	///	�M�`�B�����܂ލ�(K2,f3)�̂ݍČv�Z
	InitCreateVecf_();				///	�ύX����K�v�̂��鍀�̂݁A���ꕨ��������
	InitCreateMatk_();
	for(unsigned i =0; i < mesh->edges.size();i++){
		edgeVars[i].k = 0.0;
	}
	for(unsigned i=0; i< mesh->tets.size();i++){
		CreateVecFAll(i);				///	VecF�̍č쐬
		CreateMatkLocal(i);				///	MatK2�̍č쐬 ��if(deformed==true){matk1�𐶐�}		matK1��matk1�̕ϐ��ɓ���Ă����āAmatk2�����A����āA���Z
	}
//	}
	///	�ߓ_�̑�����ʂ�alphaUpdated��true�ɂ���
	for(unsigned i=0;i<mesh->vertices[id].faceIDs.size();i++){
		faceVars[mesh->vertices[id].faceIDs[i]].alphaUpdated = true;
		alphaUpdated = true;
	}
}

void PHFemThermo::UsingHeatTransferBoundaryCondition(unsigned id,double temp){
	//�M�`�B���E����
	//�ߓ_�̎��͗��̉��x�̐ݒ�(K,C,F�Ȃǂ̍s��x�N�g���̍쐬��Ɏ��s�K�v����)
//	if(vertices[id].Tc != temp){					//�X�V����ߓ_��Tc���ω������������ATc��F�x�N�g�����X�V����
		SetLocalFluidTemp(id,temp);
		//�M�`�B���E�������g����悤�ɁA����B
		//for(unsigned i =0;i < vertices[id].tets.size();i++){
		//	CreateVecfLocal(tets[vertices[id].tets[i]]);		//	Tc���܂ރx�N�g�����X�V����
		//}
		InitCreateVecf_();
		for(unsigned i=0; i < GetPHFemMesh()->tets.size();i++){
			CreateVecFAll(i);				///	VeecF�̍č쐬
													///	MatK2�̍č쐬��matK1��matk1�̕ϐ��ɓ���Ă����āAmatk2�����A����āA���Z
		}
//	}
}

void PHFemThermo::SetRhoSpheat(double r,double Spheat){
	//> ���x�A��M of ���b�V���̃O���[�o���ϐ�(=���b�V���ŗL�̒l)���X�V
	rho = r;
	specificHeat = Spheat;
}

std::vector<Vec2d> PHFemThermo::CalcIntersectionPoint2(unsigned id0,unsigned id1,double r,double R){
	PHFemMeshNew* mesh = GetPHFemMesh();

	//	2�_��ʂ钼����1��	2�̒萔�����߂�
	double constA = 0.0;
	double constB = 0.0;
	///	r�ƌ�_
	double constX1 = 0.0;
	double constX1_ = 0.0;
	double constY1 = 0.0;
	///	R�ƌ�_
	double constX2 = 0.0;
	double constX2_ = 0.0;
	double constY2 = 0.0;

	//> �����̑�֏���	�֐��������Ƃ��ɁA�ϊ����遫
	unsigned vtxId0 = id0;
	unsigned vtxId1 = id1;
	/// ���_�ɋ߂����ɕ��ёւ�
	if(vertexVars[vtxId0].disFromOrigin > vertexVars[vtxId1].disFromOrigin){
		unsigned tempId = vtxId0;
		vtxId0 = vtxId1;
		vtxId1 = tempId;
	}
	/// vtxId0 < vtxId1 ���ۏ؂���Ă���

	//	2�_�Ō���邱�Ƃ��O��
	//> 2�_��disFromOrigin��r,R�Ɣ�r���Ăǂ���ƌ���邩�𔻕ʂ���B
	//> �~�Ƃ̌�_�����߂�
	// x-z���ʂōl���Ă���
	/// constA,B:vtxId0.vtxId1��ʂ钼���̌X���ƐؕЁ@/// aconsta,constb�͐����\��Ȃ�
	DSTR << "id0: " << id0 << ", id1: " << id1 <<std::endl;
	constA = ( mesh->vertices[vtxId0].pos.z - mesh->vertices[vtxId1].pos.z) / ( mesh->vertices[vtxId0].pos.x - mesh->vertices[vtxId1].pos.x);
	DSTR << "vertices[vtxId0].pos.z: " << mesh->vertices[vtxId0].pos.z <<std::endl;
	DSTR << "vertices[vtxId0].pos.z: " << mesh->vertices[vtxId1].pos.z <<std::endl;
	DSTR << "dz: vertices[vtxId0].pos.z - vertices[vtxId1].pos.z : " << mesh->vertices[vtxId0].pos.z - mesh->vertices[vtxId1].pos.z << std::endl;

	DSTR << "vertices[vtxId0].pos.x: " << mesh->vertices[vtxId0].pos.x << std::endl;
	DSTR << "vertices[vtxId1].pos.x: " << mesh->vertices[vtxId1].pos.x << std::endl;
	DSTR << "dx: vertices[vtxId0].pos.x - vertices[vtxId1].pos.x: " << mesh->vertices[vtxId0].pos.x - mesh->vertices[vtxId1].pos.x << std::endl;

	DSTR << "constA = dz / dx: " << constA << std::endl;
	if(mesh->vertices[vtxId0].pos.z == mesh->vertices[vtxId1].pos.z && mesh->vertices[vtxId0].pos.x == mesh->vertices[vtxId1].pos.x){
		DSTR << "vertices[vtxId0].pos.y: " << mesh->vertices[vtxId0].pos.y << ", vertices[vtxId1].pos.y: " << mesh->vertices[vtxId1].pos.y << std::endl;
		if(mesh->vertices[vtxId0].pos.y == mesh->vertices[vtxId1].pos.y)
			DSTR << "id[" << id0 <<"], id[" << id1 << "] �͓������_ !" << std::endl;
	}
	
	constB = mesh->vertices[vtxId0].pos.z - constA * mesh->vertices[vtxId0].pos.x;
	DSTR << "constB = vertices[vtxId0].pos.z - constA * vertices[vtxId0].pos.x : " << mesh->vertices[vtxId0].pos.z - constA * mesh->vertices[vtxId0].pos.x << std::endl;

	///	��_�̍��W���v�Z
	if(vertexVars[vtxId0].disFromOrigin < r){		/// ���ar�̉~�ƌ����Ƃ�
		//CalcYfromXatcross(vtxId0,vtxId1,r);	//�֐������Ȃ�
		//> �ȉ��A�֐���,vtxId0,1,r:�����AconstY��Ԃ�
		constX1 = (- constA * constB + sqrt(r * r *(constA * constA + 1.0) - constB * constB));
		constX1_ = (- constA * constB - sqrt(r * r *(constA * constA + 1.0) - constB * constB));
		// �ǂ��炩�����_�̊Ԃɂ���@�召���킩��Ȃ��̂ŁAor�ŁA�_1��x���W�A2��x���W�ƁA���̓���ւ��ƁA���
		//> �����̗��[�̓_�̊Ԃɂ���Ƃ�
		if( (mesh->vertices[vtxId0].pos.x <= constX1 && constX1 <= mesh->vertices[vtxId1].pos.x) || (mesh->vertices[vtxId1].pos.x <= constX1 && constX1 <= mesh->vertices[vtxId0].pos.x) ){
			constY1 = sqrt(r * r - constX1 * constX1 );
		}else{
			constY1 = sqrt(r * r - constX1_ * constX1_ );
			constX1 = constX1_;		///		�_��x���W��constX_�������������킩�����B
		}
	}else if(vertexVars[vtxId0].disFromOrigin < R){		/// ���aR�̉~�ƌ����Ƃ�
		constX1 = (- constA * constB + sqrt(R * R *(constA * constA + 1.0) - constB * constB));
		constX1_ = (- constA * constB - sqrt(R * R *(constA * constA + 1.0) - constB * constB));
		// �ǂ��炩�����_�̊Ԃɂ���@�召���킩��Ȃ��̂ŁAor�ŁA�_1��x���W�A2��x���W�ƁA���̓���ւ��ƁA���
		//> �����̗��[�̓_�̊Ԃɂ���Ƃ�
		if( (mesh->vertices[vtxId0].pos.x <= constX1 && constX1 <= mesh->vertices[vtxId1].pos.x) || (mesh->vertices[vtxId1].pos.x <= constX1 && constX1 <= mesh->vertices[vtxId0].pos.x) ){
			constY1 = sqrt(R * R - constX1 * constX1 );
		}else{
			constY1 = sqrt(R * R - constX1_ * constX1_ );
			constX1 = constX1_;		///		�_��x���W��constX_�������������킩�����B
		}
		
	}
	//> �ǂ���Ƃ������Ƃ�
	else if(vertexVars[vtxId0].disFromOrigin < r && R < vertexVars[vtxId1].disFromOrigin){
		//> �萔��2�~����
		constX1 = (- constA * constB + sqrt(r * r *(constA * constA + 1.0) - constB * constB));
		constX1_ = (- constA * constB - sqrt(r * r *(constA * constA + 1.0) - constB * constB));
		// �ǂ��炩�����_�̊Ԃɂ���@�召���킩��Ȃ��̂ŁAor�ŁA�_1��x���W�A2��x���W�ƁA���̓���ւ��ƁA���
		//> �����̗��[�̓_�̊Ԃɂ���Ƃ�
		if( (mesh->vertices[vtxId0].pos.x <= constX1 && constX1 <= mesh->vertices[vtxId1].pos.x) || (mesh->vertices[vtxId1].pos.x <= constX1 && constX1 <= mesh->vertices[vtxId0].pos.x) ){
			constY1 = sqrt(r * r - constX1 * constX1 );
		}else{
			constY1 = sqrt(r * r - constX1_ * constX1_ );
			constX1 = constX1_;		///		�_��x���W��constX_�������������킩�����B
		}
		constX2 = (- constA * constB + sqrt(R * R *(constA * constA + 1.0) - constB * constB));
		constX2_ = (- constA * constB - sqrt(R * R *(constA * constA + 1.0) - constB * constB));
		//> �����̗��[�̓_�̊Ԃɂ���Ƃ�
		if( (mesh->vertices[vtxId0].pos.x <= constX2 && constX2 <= mesh->vertices[vtxId1].pos.x) || (mesh->vertices[vtxId1].pos.x <= constX2 && constX2 <= mesh->vertices[vtxId0].pos.x) ){
			constY2 = sqrt(R * R - constX2 * constX2 );
		}else{
			constY2 = sqrt(R * R - constX2_ * constX2_ );
			constX2 = constX2_;		///		�_��x���W��constX_�������������킩�����B
		}
	}
	std::vector<Vec2d> intersection;
	intersection.push_back(Vec2d(constX1,constY1));
	if(constX2 && constY2){
		intersection.push_back(Vec2d(constX2,constY2));
	}
		//Vec4d interSection;
		//interSection[0] = constX1;
		//interSection[1] = constY1;
		//interSection[2] = constX2;
		//interSection[3] = constY2;
		DSTR << __FILE__  << "(" <<  __LINE__ << "):"<< "intersection Vtx: " << intersection[0] << std::endl;
		//":" <<  __TIME__ << 
		//return interSection;
		return intersection;
}

Vec2d PHFemThermo::CalcIntersectionPoint(unsigned id0,unsigned id1,double r,double R){
	PHFemMeshNew* mesh = GetPHFemMesh();

	double constA = 0.0;
	double constB = 0.0;
	double constX = 0.0;
	double constX_ = 0.0;
	double constY = 0.0;

	//> �����̑�֏���	�֐��������Ƃ��ɁA�ϊ����遫
	unsigned vtxId0 = id0;
	unsigned vtxId1 = id1;
	/// ���_�ɋ߂����ɕ��ёւ�
	if(vertexVars[vtxId0].disFromOrigin > vertexVars[vtxId1].disFromOrigin){
		unsigned tempId = vtxId0;
		vtxId0 = vtxId1;
		vtxId1 = tempId;
	}
	/// vtxId0 < vtxId1 ���ۏ؂���Ă���

	//> 2�_��disFromOrigin��r,R�Ɣ�r���Ăǂ���ƌ���邩�𔻕ʂ���B
	if( (r <= vertexVars[vtxId0].disFromOrigin && vertexVars[vtxId0].disFromOrigin <= R) ^ (r <= vertexVars[vtxId1].disFromOrigin && vertexVars[vtxId1].disFromOrigin <= R)){
		//> �~�Ƃ̌�_�����߂�
		// x-z���ʂōl���Ă���
		/// constA,B:vtxId0.vtxId1��ʂ钼���̌X���ƐؕЁ@/// aconsta,constb�͐����\��Ȃ�
		constA = ( mesh->vertices[vtxId0].pos.z - mesh->vertices[vtxId1].pos.z) / ( mesh->vertices[vtxId0].pos.x - mesh->vertices[vtxId1].pos.x);
		constB = mesh->vertices[vtxId0].pos.z - constA * mesh->vertices[vtxId0].pos.x;

		///	��_�̍��W���v�Z
		if(vertexVars[vtxId0].disFromOrigin < r){		/// ���ar�̉~�ƌ����Ƃ�
			//CalcYfromXatcross(vtxId0,vtxId1,r);	//�֐������Ȃ�
			//> �ȉ��A�֐���,vtxId0,1,r:�����AconstY��Ԃ�
			constX = (- constA * constB + sqrt(r * r *(constA * constA + 1.0) - constB * constB));
			constX_ = (- constA * constB - sqrt(r * r *(constA * constA + 1.0) - constB * constB));
			// �ǂ��炩�����_�̊Ԃɂ���@�召���킩��Ȃ��̂ŁAor�ŁA�_1��x���W�A2��x���W�ƁA���̓���ւ��ƁA���
			//> �����̗��[�̓_�̊Ԃɂ���Ƃ�
			if( (mesh->vertices[vtxId0].pos.x <= constX && constX <= mesh->vertices[vtxId1].pos.x) || (mesh->vertices[vtxId1].pos.x <= constX && constX <= mesh->vertices[vtxId0].pos.x) ){
				constY = sqrt(r * r - constX * constX );
			}else{
				constY = sqrt(r * r - constX_ * constX_ );
				constX = constX_;		///		�_��x���W��constX_�������������킩�����B
			}
		}else if(r < vertexVars[vtxId0].disFromOrigin && vertexVars[vtxId0].disFromOrigin < R){		/// ���aR�̉~�ƌ����Ƃ�
			constX = (- constA * constB + sqrt(r * R *(constA * constA + 1.0) - constB * constB));
			constX_ = (- constA * constB - sqrt(r * R *(constA * constA + 1.0) - constB * constB));
			// �ǂ��炩�����_�̊Ԃɂ���@�召���킩��Ȃ��̂ŁAor�ŁA�_1��x���W�A2��x���W�ƁA���̓���ւ��ƁA���
			//> �����̗��[�̓_�̊Ԃɂ���Ƃ�
			if( (mesh->vertices[vtxId0].pos.x <= constX && constX <= mesh->vertices[vtxId1].pos.x) || (mesh->vertices[vtxId1].pos.x <= constX && constX <= mesh->vertices[vtxId0].pos.x) ){
				constY = sqrt(R * R - constX * constX );
			}else{
				constY = sqrt(R * R - constX_ * constX_ );
				constX = constX_;		///		�_��x���W��constX_�������������킩�����B
			}
		}
		//> �ǂ���Ƃ������Ƃ�
		else if(vertexVars[vtxId0].disFromOrigin < r && R < vertexVars[vtxId1].disFromOrigin){
			//> �萔��2�~����
		}
	}
		Vec2d interSection;
		interSection[0] = constX;
		interSection[1] = constY;
		DSTR << __FILE__  << "(" <<  __LINE__ << "):"<< "interSection: " << interSection << std::endl;
		//":" <<  __TIME__ << 
		return interSection;
}

void PHFemThermo::ArrangeFacevtxdisAscendingOrder(int faceID){
	PHFemMeshNew* mesh = GetPHFemMesh();

	///	3�_�����_�ɋ߂����ɕ��ׂ�		//>	�N�C�b�N�\�[�g�ɂ����������H
	int vtxmin[3];		///	�ʂ��̒��_�ԍ�������
	vtxmin[0] = mesh->faces[faceID].vertexIDs[0];
	vtxmin[1] = 0;
	vtxmin[2] = 0;
	if(vertexVars[mesh->faces[faceID].vertexIDs[1]].disFromOrigin < vertexVars[mesh->faces[faceID].vertexIDs[0]].disFromOrigin ){
		vtxmin[0] = mesh->faces[faceID].vertexIDs[1];
		vtxmin[1] = mesh->faces[faceID].vertexIDs[0];
	}else{
		vtxmin[1] = mesh->faces[faceID].vertexIDs[1];
	}
	if(vertexVars[mesh->faces[faceID].vertexIDs[2]].disFromOrigin < vertexVars[vtxmin[0]].disFromOrigin){
		vtxmin[2] = vtxmin[1];
		vtxmin[1] = vtxmin[0];
		vtxmin[0] = mesh->faces[faceID].vertexIDs[2];
	}else if(vertexVars[vtxmin[0]].disFromOrigin < vertexVars[mesh->faces[faceID].vertexIDs[2]].disFromOrigin && vertexVars[mesh->faces[faceID].vertexIDs[2]].disFromOrigin < vertexVars[vtxmin[1]].disFromOrigin){
		vtxmin[2] = vtxmin[1];
		vtxmin[1] = mesh->faces[faceID].vertexIDs[2];
	}else if(vertexVars[vtxmin[1]].disFromOrigin < vertexVars[mesh->faces[faceID].vertexIDs[2]].disFromOrigin ){
		vtxmin[2] = mesh->faces[faceID].vertexIDs[2];
	}
	//>	���������ɂȂ��Ă��Ȃ��Ƃ��́Aassert(0)
	if( !(vertexVars[vtxmin[0]].disFromOrigin < vertexVars[vtxmin[1]].disFromOrigin && vertexVars[vtxmin[1]].disFromOrigin < vertexVars[vtxmin[2]].disFromOrigin )
		){	assert(0);}
	/// debug
	//DSTR << "�������� ";
	//for(unsigned j=0; j <3; j++){
	//	DSTR << vertices[vtxmin[j]].disFromOrigin;
	//	if(j<2){ DSTR << ", ";}
	//}
	//DSTR << std::endl;
	/// face���̔z���face���ł̌��_����߂����Ԃ��i�[
	for(unsigned i=0;i<3;i++){
		faceVars[faceID].ascendVtx[i] = vtxmin[i];
	}
	//DSTR << "vtxmin[0~2]:  " << vtxmin[0] <<" ," << vtxmin[1] << " ,"  << vtxmin[2] << std::endl;	
	//vtxmin[0~2]:  11 ,324 ,281 ,vtxmin[0~2]:  353 ,11 ,324 ���̏��Ɍ��_�ɋ߂�

	//	�ȉ��A�폜
	//> �Ԃ�����
	//Vec3i vtxarray = Vec3i(vtxmin[0],vtxmin[1],vtxmin[2]);
	//	ex	vtxarray: (    11    324    281)	,vtxarray: (   353     11    324)
	//DSTR << "vtxarray: " << vtxarray << std::endl; 
	//return vtxarray;		///	�ʂ��̒��_�ԍ���Ԃ�
}

Vec2d PHFemThermo::CalcIntersectionOfCircleAndLine(unsigned id0,unsigned id1,double radius){
	PHFemMeshNew* mesh = GetPHFemMesh();

	//	x-z���ʂł̔��ar�̉~�Ɛ����̌�_�̍��W�����߂�
	//	�֐����Ă΂������F����邱�Ƃ������ȂƂ��A�����~�ʂ̔��a�ƁA�~�ʂ̓��E�O���̒��_�����󂯌v�Z

	// ...2�_��ʂ钼���̕������̌X���ƐؕЂ����߂�
	double constA = 0.0;	//	�X��
	double constB = 0.0;	//	�ؕ�
	double constX1 = 0.0;	//	��_��x ���W�P
	double constX1_ = 0.0;	//		  x ���W���Q
	double constZ1 = 0.0;	//		  Y ���W
	double constZ1_ = 0.0;	//		  Y ���W���Q
	double constx = 0.0;	//	1���֐��ɂȂ�Ȃ��ꍇ�̒萔
	double constz = 0.0;	//�@		����

	//	...���_�ԍ������_�ɋ߂�����������( id0 < id1 )�ɕ��ёւ�
	if( vertexVars[ id1 ].disFromOrigin	<	vertexVars[ id0 ].disFromOrigin ){
		unsigned farfromOriginId = id0;
		id0 = id1;
		id1 = farfromOriginId;
	}	// id0 < id1 ��ۏ�

	// 1���֐��ɂȂ�Ȃ��ꍇ
	// .. x == const
	if(mesh->vertices[id0].pos.x == mesh->vertices[id1].pos.x && mesh->vertices[id0].pos.z != mesh->vertices[id1].pos.z ){
		//	x == const �̎�

	}
	// .. z == const

	// ...�X���ƐؕЂ����߂�
	//
	constA = ( mesh->vertices[id0].pos.z - mesh->vertices[id1].pos.z) / ( mesh->vertices[id0].pos.x - mesh->vertices[id1].pos.x);
	constB = mesh->vertices[id0].pos.z - constA * mesh->vertices[id0].pos.x;
	
	
	// ..���̏����́A�ȍ~�̏����ŃG���[���o��ꍇ��m�点�邽�߂́A�f�o�b�O�p
	if(!constA && !constB){
		//if( id0 != id1)
		//	vertices[id0].pos.z != 0 && vertices[id1].pos.z!= 0 && vertices[id0].pos.x
		// z == 0�Ƃ��������̎��ł���B�Ƃ������O�Ƃ��������̎��̎������낤�B
		//	���̏�������̈Ӗ��́A�����̗��[�̓_��x,z���W��(0,0)��A�����̎����P���֐��ɂȂ�Ȃ��ƋL���������Ƃ��ł���Ƃ��ɁA�p����B
		DSTR << "�����̎��̐ؕЂƌX��������0" << std::endl;
		DSTR << "id0: " << id0 << ", id1: " << id1 << "radius: " << radius << std::endl;
		DSTR << "vertices[id0].pos.x: " << mesh->vertices[id0].pos.x << ", vertices[id1].pos.x: " << mesh->vertices[id1].pos.x << std::endl;
		DSTR << "vertices[id0].pos.z: " << mesh->vertices[id0].pos.z << ", vertices[id1].pos.z: " << mesh->vertices[id1].pos.z << std::endl;
		assert(0);
	}
	//DSTR << "constA: " << constA << ",  " << "constB: " << constB <<std::endl; 

	//debug
	//DSTR << "id0: " << id0 << ", id1: " << id1 <<std::endl;
	//DSTR << "vertices[id0].pos.z: " << vertices[id0].pos.z <<std::endl;
	//DSTR << "vertices[id0].pos.z: " << vertices[id1].pos.z <<std::endl;
	//DSTR << "dz: vertices[id0].pos.z - vertices[id1].pos.z : " << vertices[id0].pos.z - vertices[id1].pos.z << std::endl;
	//DSTR << "vertices[id0].pos.x: " << vertices[id0].pos.x << std::endl;
	//DSTR << "vertices[id1].pos.x: " << vertices[id1].pos.x << std::endl;
	//DSTR << "dx: vertices[id0].pos.x - vertices[id1].pos.x: " << vertices[id0].pos.x - vertices[id1].pos.x << std::endl;

	// for Check Debug
	//DSTR << "constA = dz / dx: " << constA << std::endl;
	//if(vertices[id0].pos.z == vertices[id1].pos.z && vertices[id0].pos.x == vertices[id1].pos.x){
	//	DSTR << "vertices[id0].pos.y: " << vertices[id0].pos.y << ", vertices[id1].pos.y: " << vertices[id1].pos.y << std::endl;
	//	if(vertices[id0].pos.y == vertices[id1].pos.y)
	//		DSTR << "id[" << id0 <<"], id[" << id1 << "] �͓������_ !" << std::endl;
	//}
	//DSTR << "constB = vertices[id0].pos.z - constA * vertices[id0].pos.x : " << vertices[id0].pos.z - constA * vertices[id0].pos.x << std::endl;
	//DSTR << "constA: " << constA << std::endl;
	//DSTR << "constB: " << constB << std::endl;
	//DSTR << std::endl;

	///	.��_�̍��W���v�Z
	// .�P���֐��̏ꍇ�Ax,z���ɕ��s�Ȓ����̏ꍇ������
	if(vertexVars[id0].disFromOrigin <= radius && radius <= vertexVars[id1].disFromOrigin ){		/// ���ar�̉~�ƌ����ׂ�
		double radius2 = radius * radius;
		constX1  = (- constA * constB + sqrt(radius2 * (constA * constA + 1.0) - constB * constB )  )  /  (constA * constA + 1);
		constX1_ = (- constA * constB - sqrt(radius2 * (constA * constA + 1.0) - constB * constB )  )  /  (constA * constA + 1);
		// ��_��x���W�������̗��[�_��x���W�Ԃɂ���Ƃ�
		if( (mesh->vertices[id0].pos.x <= constX1 && constX1 <= mesh->vertices[id1].pos.x) || (mesh->vertices[id1].pos.x <= constX1 && constX1 <= mesh->vertices[id0].pos.x) ){
			constZ1 = sqrt(radius2 - constX1  * constX1  );
			constZ1_ = - sqrt(radius2 - constX1  * constX1  );
			//	��_��z���W�����[�_��z���W�Ԃɂ���Ƃ�
			if( (mesh->vertices[id0].pos.z <= constZ1_ && constZ1_ <= mesh->vertices[id1].pos.z) || (mesh->vertices[id1].pos.z <= constZ1_ && constZ1_ <= mesh->vertices[id0].pos.z) ){
				constZ1 = constZ1_;
			}
		}else{
			constX1 = constX1_;		///		�_��x���W��constX_�������������킩�����B
			constZ1  =   sqrt(radius2 - constX1  * constX1  );
			constZ1_ = - sqrt(radius2 - constX1  * constX1  );
			//	��_��z���W�����[�_��z���W�Ԃɂ���Ƃ�
			if( (mesh->vertices[id0].pos.z <= constZ1_ && constZ1_ <= mesh->vertices[id1].pos.z) || (mesh->vertices[id1].pos.z <= constZ1_ && constZ1_ <= mesh->vertices[id0].pos.z) ){
				constZ1 = constZ1_;
			}
		}
	}else{
		DSTR << "CalcVtxCircleAndLine()�֐��̂��̒��_�g�݂Ɖ~�ʂ͌����܂���" << std::endl;
		constX1 = 0.0;
		constZ1 = 0.0;
		DSTR << "(id0, vertices[id0].disFromOrigin): (" << id0 << ", " << vertexVars[id0].disFromOrigin << "), (id1, vertices[id1].disFromOrigin): (" << id1 << ", " << vertexVars[id1].disFromOrigin << "), radius: " << radius << std::endl;  
		assert(0);
	}
	Vec2d intersection = Vec2d(constX1,constZ1);
	//":" <<  __TIME__ << 
	return intersection;
}		//	CalcVtxCircleAndLine() :difinition

void PHFemThermo::ShowIntersectionVtxDSTR(unsigned faceID,unsigned faceVtxNum,double radius){
	PHFemMeshNew* mesh = GetPHFemMesh();

	unsigned i = faceID;
	unsigned j = faceVtxNum;
	DSTR << "ascendVtx[" << j << "]: " << faceVars[i].ascendVtx[j] << ", " << "[ " << (j+1)%3 << "]: " << faceVars[i].ascendVtx[(j+1)%3] << "; ";
	DSTR << " (vertices[" << faceVars[i].ascendVtx[j] << "].pos.x, .z) = ( " <<  mesh->vertices[faceVars[i].ascendVtx[j]].pos.x << ", "<<  mesh->vertices[faceVars[i].ascendVtx[j]].pos.z  << "), " ;
	DSTR << " (vertices[" << faceVars[i].ascendVtx[(j+1)%3] << "].pos.x, .z) : ( " <<  mesh->vertices[faceVars[i].ascendVtx[(j+1)%3]].pos.x << ", "<<  mesh->vertices[faceVars[i].ascendVtx[(j+1)%3]].pos.z << "), " <<std::endl;
	DSTR <<"face[i].[(" << j << "], [" << (j+1)%3 << "]�F�e�X�̌��_����̋���" << vertexVars[faceVars[i].ascendVtx[j]].disFromOrigin << ", " << vertexVars[faceVars[i].ascendVtx[(j+1)%3]].disFromOrigin << ", "; 
	DSTR << " radius: " << radius <<" ��2�_�ō\�����������Ƃ̌�_�͉��L"<< std::endl;
	DSTR << __FILE__  << "(" <<  __LINE__ << "):"<< "Intersection Vtx (x,z)= " << CalcIntersectionOfCircleAndLine( faceVars[i].ascendVtx[ j ] , faceVars[i].ascendVtx[ (j+1)%3 ] , radius) << std::endl;
	DSTR << std::endl;
}

//void PHFemThermo::CalcIHarea(double radius,double Radius,double dqdtAll){
//	//	face�\���̃����oiharea���v�Z
//	//�A���b�V���S�̂�iharea�̍��v���v�Z
//	//	...dqdtRatio(�M������)�𑍖ʐςɑ΂���iharea�ʐϔ䂩���΂̎��ɁAface���󂯎��ׂ�dqdt���v�Z���ĕԂ�
//
//	//	�`��֐��̌v�Z�́Aiharea ��p���āACalcVecf2surface()���ōs��,face.shapefunk�ōs��		���N���X�����悤���H
//		//	.... �`��֐����i�[����		// �����ŕ�����`��֐��́A���_�Ԃ̋������番������`��ԌW���@���Ȃ킿�A�����@�O�����`�����P�ŗǂ��āA�Ō�ɁA�s��ɓ����O�ɁA�����ȊO������΂����̂��ȁH
//
//	// radius value check
//	if(Radius <= radius){
//		DSTR << "inner radius size is larger than outer Radius " << std::endl;
//		DSTR << "check and set another value" << std::endl;
//		assert(0);
//	}
//	///	�����a�ƊO���a�̊Ԃ̐ߓ_�ɔM�������E������ݒ�
//	//> �l�ʑ̖ʂ̎O�p�`�Ɖ~�̈�̏d�������̌`��E�ʐς����߂铖���蔻����v�Z����B
//	//>	�؂���`��ɉ������`��֐������߁A�M�����x�N�g���̐����ɑ�����A�v�Z����
//
//	//> 1.�~�̈�Əd�Ȃ�face�O�p�`�̌`����Z�o����B�̈�Ɋ܂܂�钸�_�Aface�O�p�`�̕ӂƂ̌�_�����߂�vecteor�Ɋi�[����
//	//>	2.vector�ɂ́A��0,1,2�̏��ɗ̈���̒��_���_�������Ă��邪�A��������ɎO�p�`�������s���B�O�p�`�������ł�����A�e�O�p�`�����߂�B�O�p�`�̑��a���A����face�̉��M�̈�Ƃ���B
//	//>	3.vector�̓_�ɂ�����`��֐������߂āA�[���̐ρi�d�Ȃ��Ă���ʐρ~�`��֐��̒l�j���g���āA�l�ʑ̓��̊e�_�ɂ�����`��֐��̖ʐϕ������߂�B���߂��l�́A�M�����x�N�g���̐����Ƃ��ėv�f�����s��̐����ɑ������B
//	//>	4.���X�e�b�v�A�����M�����̒l���x�N�g�������ɉ�����
//	
//	/// debug
//	//unsigned numIHheated0 = 0; 
//	//for(unsigned i=0; i < nSurfaceFace;i++){
//	//	if(faces[i].mayIHheated){	
//	//		DSTR << i << " ; "  << std::endl;
//	//		numIHheated0 +=1;
//	//	}
//	//}
//	//DSTR << "numIHheated0 / nSurfaceFace: " << numIHheated0 << " / " << nSurfaceFace << std::endl;	////	761 / 980	���ĂقƂ�ǂ���Ȃ����I�����ʂɂȂ�Ȃ��Ƃ��������͂������E�E�E�@���[�J��y���W�l���}�C�i�X�̂��̂�I��ł���̂�
//
//	//	debug	mayIHheated�̊m�x���グ��O�̐���m�肽��
//	unsigned numIHheated0 = 0; 
//	for(unsigned i=0; i < nSurfaceFace;i++){
//		if(faces[i].mayIHheated){	
//			//DSTR << i << " ; "  << std::endl;
//			numIHheated0 +=1;
//		}
//	}
//	DSTR << "numIHheated0 / nSurfaceFace: " << numIHheated0 << " / " << nSurfaceFace << std::endl;
//
//	//	face���_�̂ǂꂩ1���A�~�̈�ɓ����Ă���face�����Atrue�ɁA����ȊO�́Afalse��
//	//> raius,Radius�ɂ���mayIHheated�̊m�x���グ�Ă���A�~�̈�Əd�Ȃ��Ă���`������߂�
//	for(unsigned i=0;i < nSurfaceFace; i++){
//		if(faces[i].mayIHheated){			// face�̐ߓ_��y���W�����̏ꍇ��IH���M�̑Ώېߓ_ �~�͈͓̔��ɓ����Ă���Ƃ͌���Ȃ�
//			for(unsigned j=0;j<3;j++){
//				/// �~�̈����face���_���܂܂��
//				if(radius <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= Radius){
//					faces[i].mayIHheated = true;
//					break;		//>	����������A�����true�̂܂܂ŗǂ��B�œ�����for�𔲂���
//				}
//				else{
//					faces[i].mayIHheated = false;
//				}
//				//> �i�~�̈�ɂ͊܂܂ꂸ�j�~�̈�������ƊO����face�̕ӂ̒��_������	vertices[j%3] �� vertices[(j+1)%3]�@�ō��ӂ�����Ƃ�
//				if(vertices[faces[i].vertices[j]].disFromOrigin < radius && Radius < vertices[faces[i].vertices[(j+1)%3]].disFromOrigin 
//					|| vertices[faces[i].vertices[(j+1)%3]].disFromOrigin < radius && Radius < vertices[faces[i].vertices[j]].disFromOrigin){
//						faces[i].mayIHheated = true;
//						break;		//>	����
//				}else{
//					faces[i].mayIHheated = false;
//				}
//				//>	�~�̈����face�ӂ̂ǂ��炩�̒��_���܂܂��Ƃ�(r<P1<R<P2,P1<r<P2<R,(��Pa1,P2�����ւ�������))
//				if(radius <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin < Radius && Radius < vertices[faces[i].vertices[(j+1)%3]].disFromOrigin
//					|| radius <= vertices[faces[i].vertices[(j+1)%3]].disFromOrigin && vertices[faces[i].vertices[(j+1)%3]].disFromOrigin < Radius && Radius < vertices[faces[i].vertices[j]].disFromOrigin
//					|| vertices[faces[i].vertices[j]].disFromOrigin <= radius && radius < vertices[faces[i].vertices[(j+1)%3]].disFromOrigin && vertices[faces[i].vertices[(j+1)%3]].disFromOrigin < Radius
//					|| vertices[faces[i].vertices[(j+1)%3]].disFromOrigin <= radius && radius < vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin < Radius){
//						faces[i].mayIHheated = true;
//						break;		//>	����
//				}else{
//					faces[i].mayIHheated = false;
//				}
//			}		//	for
//		}		//	if
//	}		//	for
//	//> debug
//	//>	mayIHheated�̃t���O�������Ă���face�ɂ��̖ʐς̌`��֐���^���Ă݂�B	�d�Ȃ�ʐς�������ƌv�Z�ƁA�����ł������������Ă���΁A���M�ʂɓ���Ă��܂��v�Z�A����
//	//> CalcIHdqdt3 or 4
//
//	/// debug
//	unsigned numIHheated = 0; 
//	for(unsigned i=0; i < nSurfaceFace;i++){
//		if(faces[i].mayIHheated){	
//			//DSTR << i << " ; "  << std::endl;
//			numIHheated +=1;
//		}
//	}
//	DSTR << "numIHheated / nSurfaceFace: " << numIHheated << " / " << nSurfaceFace << std::endl;		///:	�\��face�̓��A���M�ߓ_���܂�face�̐��A�S��:264/980�@����Ȃ��񂩂� 
//	
//	//	.. ��_�����߁Afaces�\���̂�vector�ɗ̈���̒��_���_���i�[
//	for(unsigned i=0;i < nSurfaceFace; i++){
//		if(faces[i].mayIHheated){		//	may �� �u�m���v�ɕω��ς݂̃t���O
//			//	area:face�ʐς��v�Z����ĂȂ���΁A�v�Z
//			if(faces[i].area==0) faces[i].area = CalcTriangleArea(faces[i].vertices[0],faces[i].vertices[1],faces[i].vertices[2]);
//			////	face���̒��_��disFromOrigin�̒l�Ń\�[�g
//			//unsigned nearestvtxnum		=	0;				///	���_�Ɉ�ԋ߂����_��face���_�ԍ�(0~2)
//			//for(unsigned j=0;j<3;j++){
//			//	double hikaku = DBL_MAX;
//			//	if(hikaku > vertices[faces[i].vertices[j]].disFromOrigin){	hikaku = vertices[faces[i].vertices[j]].disFromOrigin;	nearestvtxnum = j;}
//			//}
//			
//			// ... 3�_�����_�ɋ߂����ɕ��ׂ�		vtxOrder:�߂����Ɋi�[,�Ԗڂ��߂����ԁA�i�[�l�����_�ԍ� ==	faces[i].ascendVtx[3]
//			ArrangeFacevtxdisAscendingOrder(i);		///	ArrangeVtxdisAscendingOrder(int faceID,int vtx0,int vtx1,int vtx2)
//			//DSTR <<  "�����������m�F: " << vertices[vtxOrder[0]].disFromOrigin << ", "<< vertices[vtxOrder[1]].disFromOrigin << ", "<< vertices[vtxOrder[2]].disFromOrigin << std::endl;
//			
//			// ... face���̊e���_�������Ă���̈�𔻒� 0 | 1 | 2	///	faces[i].ascendVtx[0~2]�ɊY�����钸�_���@�~�̈�̑O��̂ǂ��ɑ��݂��Ă��邩
//			// ... vtxdiv[0~2]�ɋ߂����ɕ��񂾒��_�̗̈�ID(0~2)������U�� �� faces[i].ascendVtx[j] �̏��ƑΉ�
//			unsigned vtxdiv[3];		//	���_����߂���:0~2�ɕ��בւ���ꂽ���_ID�ɑΉ�����̈���O�̋敪���@�z��
//			//DSTR << "faces[i].ascendVtx[0~2]:" ;
//			for(unsigned j=0;j<3;j++){
//				if( vertices[faces[i].ascendVtx[j]].disFromOrigin < radius){			vtxdiv[j] = 0;
//				/// �~�ʏ���܂݁A�~�ʏ���~�̈���ƒ�`����
//				}else if(radius <= vertices[faces[i].ascendVtx[j]].disFromOrigin && vertices[faces[i].ascendVtx[j]].disFromOrigin <= Radius ){	vtxdiv[j] = 1;
//				}else if(Radius < vertices[faces[i].ascendVtx[j]].disFromOrigin){		vtxdiv[j] = 2;	}
//				//DSTR << faces[i].ascendVtx[j] ;
//				//if(j <  2) DSTR << ", " ;
//			}
//			//DSTR << std::endl;
//
//			//> debug
//			//DSTR << "���_�̗̈�ԍ�: " ;
//			//for(unsigned j =0;j<3;j++){
//			//	DSTR << vtxexistarea[j];
//			//	if(j<2) DSTR << ", ";
//			//}
//			//DSTR << std::endl;
//			//int vtxexistareadebug =0;
//			//... 2012.2.14�����܂�...
//
//			//...	�z��̐����̒l�̕ω������āA�n�_�A��_�A�ӑ΂ƂȂ�_������vector�Ɋi�[���Ă���
//			
//			//	vector�ɓ����ۂ̒���!!! %%%%%%%%%%%%%%%%%%
//			// %%%	vtxOrder[ ���_����߂����_�̏�(0,1,2) ]:���̒��_ID�̌��_�ɋ߂����ɕ��בւ���ID���i�[		(��:vtxOrder[0] = (ID)278, [1] = (ID)35, [2] = (ID)76 etc)
//			// %%%	vtxdiv[ ���_����߂����_�̏�(0,1,2) ]:���̒��_���~�̈�̓����i1�j���O��(0,2)����\��
//
//			//>	face�̕ӂ��Ƃɏꍇ����
//			///	 j ��(�ׂ�) (j+1)%3 �Ƃő΂𐬂��ӂɂ���
//			for(unsigned j=0;j<3;j++){
//				double f[3]={0.0, 0.0, 0.0};	// ���_0,1,2,3���猩���`��֐� 
//				//debug
//				//DSTR <<"j: " << j << ", faces[i].ascendVtx[j]: " << faces[i].ascendVtx[j] << ", faces[i].ascendVtx[(j+1)%3]: " << faces[i].ascendVtx[(j+1)%3] << std::endl;
//				//DSTR << "vertices[faces[i].ascendVtx[j]].pos: (" << vertices[faces[i].ascendVtx[j]].pos.x  << ", "<< vertices[faces[i].ascendVtx[j]].pos.z << ") " << std::endl;
//				//DSTR << "vertices[faces[i].ascendVtx[(j+1)%3]].pos: (" << vertices[faces[i].ascendVtx[(j+1)%3]].pos.x  << ", "<< vertices[faces[i].ascendVtx[(j+1)%3]].pos.z << ") " << std::endl; 
//				//DSTR << std::endl;
//
//				//	0�̗̈�ɂ����:
//				if(vtxdiv[j] == 0 && vtxdiv[(j+1)%3] == 0){
//					//	������̓_�����̈��vector�ɂ͓���Ȃ�
//				}
//				//	�����a�Ƃ���������(��:0���O:1�ƊO:1����:0):
//				else if(vtxdiv[j] == 0 && vtxdiv[(j+1)%3] - vtxdiv[j] > 0 || vtxdiv[j] == 1 && vtxdiv[(j+1)%3] - vtxdiv[j] < 0){
//					//	(�n�_�i���œ���Ă���ꍇ�ɂ͕s�v�j�j�Ɠ����a�Ƃ��A�΂̓_�ɓ����
//					if(vtxdiv[(j+1)%3] - vtxdiv[j] > 0){	//�����O
//						//	�����a�Ƃ̌�_�����߂āA���W������
//						//%%%	�������\�����钸�_�Ɣ��a�A��_�̃`�F�b�N�֐���DSTR�\��	%%%//		//faceID,face���ߓ_�ԍ��A���a��p���āA�ׂ荇���ߓ_�ō������Ɖ~�ʂ̌�_�����߂ĕ\��
//						ShowIntersectionVtxDSTR(i,j,radius);
//						//	..�����a�Ƃ̌�_��x,z���W������
//						faces[i].ihvtx.push_back( CalcIntersectionOfCircleAndLine( faces[i].ascendVtx[ j ] , faces[i].ascendVtx[ (j+1)%3 ] , radius) );
//						//	..�g�Γ_�̍��W��intersection�ɓ����
//						faces[i].ihvtx.push_back( Vec2d( vertices[ faces[i].ascendVtx[ (j+1)%3 ] ].pos.x, vertices[faces[i].ascendVtx[ (j+1)%3 ] ].pos.z) );
//						//	....���̓_�ʒu�ł̌`��֐��𓱏o
//					}else if(vtxdiv[(j+1)%3] - vtxdiv[j] < 0){		//�O����
//						//	�����a�Ƃ̌�_�����߁A��_�̍��W������
//						faces[i].ihvtx.push_back( CalcIntersectionOfCircleAndLine( faces[i].ascendVtx[ j ] , faces[i].ascendVtx[ (j+1)%3 ] , radius) );
//						//..�g�Γ_�́A�����a�̓����F�~�̈�O�Ȃ̂ŁA�g�Γ_��vector�ɂ͓���Ȃ�
//					}
//				}
//				//	�~�̈�(:1)���ɂ����
//				else if(vtxdiv[j] == 1 && vtxdiv[(j+1)%3] == 1){
//					//	(�n�_�����Ă���̂Ȃ�A)�ӑΓ_��vector�ɓ����
//					//	�n�_�͓��ꂸ�Ƃ��A�Ō�ɓ���͂�
//					////intersection.push_back(Vec2d(vertices[vtxdiv[(j+1)%3]].pos.x,vertices[vtxdiv[(j+1)%3]].pos.z));
//					faces[i].ihvtx.push_back(Vec2d(vertices[faces[i].ascendVtx[(j+1)%3]].pos.x,	vertices[faces[i].ascendVtx[(j+1)%3]].pos.z));
//				}
//				//	�O���a�ƌ�����(�����O�A�O����)
//				else if(vtxdiv[j] == 1 && vtxdiv[(j+1)%3] == 2 || vtxdiv[j] == 2 && vtxdiv[(j+1)%3] == 1){
//					// ���������O���������A�����Ŕ��肷�邱�ƂŁA���	or	�̂ǂ��炩�𔻒肵�Avector�ɓ���鏇�Ԃ�ς���
//					//	�O���a�Ƃ̌�_�����߂�
//					//	�����O 2 - 1 = 1 > 0
//					if(vtxdiv[(j+1)%3] - vtxdiv[j] > 0){
//						//	��_���i�[
//						faces[i].ihvtx.push_back( CalcIntersectionOfCircleAndLine( faces[i].ascendVtx[ j ] , faces[i].ascendVtx[ (j+1)%3 ] , Radius) );
//						////	�g�Γ_��X,Z���W���i�[���i�[������_���ł���I
//						//faces[i].ihvtx.push_back(Vec2d(vertices[faces[i].ascendVtx[(j+1)%3]].pos.x,	vertices[faces[i].ascendVtx[(j+1)%3]].pos.z));
//						////	?��	intersection.push_back(Vec2d(vertices[vtxOrder[(j+1)%3]].pos.x,vertices[vtxOrder[(j+1)%3]].pos.z));
//						
//					}
//					//	�O���� 1 - 2 = -1 < 0
//					else if(vtxdiv[(j+1)%3] - vtxdiv[j] < 0){
//						// �O���a�Ƃ̌�_�̍��W���i�[����
//						faces[i].ihvtx.push_back( CalcIntersectionOfCircleAndLine( faces[i].ascendVtx[ j ] , faces[i].ascendVtx[ (j+1)%3 ] , Radius) );
//						// �g�Γ_���i�[����
//						faces[i].ihvtx.push_back( Vec2d( vertices[ faces[i].ascendVtx[ (j+1)%3 ] ].pos.x, vertices[faces[i].ascendVtx[ (j+1)%3 ] ].pos.z) );
//					}
//					else if(vtxdiv[(j+1)%3] - vtxdiv[j] == 0) assert(0);
//					//	(�n�_��vector�ɓ����Ă��邱�Ƃ��m�F����)��_��vector�ɓ����
//					faces[i].ihvtx.push_back( CalcIntersectionOfCircleAndLine( faces[i].ascendVtx[ j ] , faces[i].ascendVtx[ (j+1)%3 ] , Radius) );
//				}
//				//	�����a�ƊO���a�Ƃ̂ǂ���Ƃ�������(�����O�A�O����)
//				else if(vtxdiv[j] == 0 && vtxdiv[(j+1)%3] == 2 || vtxdiv[j] == 2 && vtxdiv[(j+1)%3] == 0){
//					// ���������O���������A�����Ŕ��肷�邱�ƂŁA���	or	�̂ǂ��炩�𔻒肵�Avector�ɓ���鏇�Ԃ�ς���
//					//%%%	�ǂ���Ƃ���������
//					// �����a�~�ʂ�茴�_��聨�O�F0���Q: 2 - 0 > 0
//					if(vtxdiv[(j+1)%3] - vtxdiv[j] > 0){
//						//�����a�ƌ����A��_���i�[
//						faces[i].ihvtx.push_back( CalcIntersectionOfCircleAndLine( faces[i].ascendVtx[ j ] , faces[i].ascendVtx[ (j+1)%3 ] , radius) );
//						//�O�n�ƌ����A��_���i�[
//						faces[i].ihvtx.push_back( CalcIntersectionOfCircleAndLine( faces[i].ascendVtx[ j ] , faces[i].ascendVtx[ (j+1)%3 ] , Radius) );
//					}else{	// �O�������a�~�ʂ�茴�_���F2���O
//						//�O���a�ƌ����A��_���i�[
//						faces[i].ihvtx.push_back( CalcIntersectionOfCircleAndLine( faces[i].ascendVtx[ j ] , faces[i].ascendVtx[ (j+1)%3 ] , Radius) );
//						//���n�ƌ����A��_���i�[
//						faces[i].ihvtx.push_back( CalcIntersectionOfCircleAndLine( faces[i].ascendVtx[ j ] , faces[i].ascendVtx[ (j+1)%3 ] , radius) );
//					}
//				}
//				//	�����a�ƊO���a�ƌ�����(�O�����������)
//				//else if(){
//				//}
//					//	if���̒��ŁA�������v���X���}�C�i�X�Ŕ���ł�����
//				
//				//	�O���a�̊O���ɂ����
//				else if(vtxdiv[j] == 2 && vtxdiv[(j+1)%3] == 2){
//					//	������̓_�����̈��vector�ɂ͓���Ȃ�
//				}
//				//	��������O���ɍs���ӂ͏�ŋL�q�ł��邪�A�O����������Ɍ������ӂ�����ŋL�q�ł���̂��H
//			}	//	for(unsigned j=0;j<3;j++){
//
//			//	..vector���O�p�`��������
//			//	..���������O�p�`�̖ʐς��e�X���߂�
//			// ..IH�ʐςɒǉ�
//
//			//faces[i].iharea = 
//
//			//	�f�o�b�O���ځFface��ihvtx(vector)�ɁAmayihheated�Ńt���O�̗�����face���͐ߓ_���S�Ċ܂܂�Ă��邩�B
//		}	//	if(mayIHheated)
//	}	//	for(faces[i]
//
//
//	//>	Step	1.�̎���
//	//>	radius:�����a�ARadius:�O���a,�@dqdtAll:�P�ʎ��Ԃ�����ɓ����Ă���S�M��
//
//	//%%	�菇 
//	//> isHeated(->nearIHHeater)��faces�̓��A3�̐ߓ_�S���ɂ��āA���蔼�a�̒��ɓ����Ă�����̂����邩�ǂ������`�F�b�N����B
//	//>	�����Ă�����̂��A��������A�ʐόv�Z�����������A�����ŁA���̐ߓ_�������Ă��邩�ɂ���ďꍇ�������s���B
//	//>	�R��:face�̖ʐς�faceS�ɑ������	1~2��:�ʐς��ߎ��v�Z���邽�߂ɁA3�ߓ_�̓��A�ǂ̐ߓ_�Ɛߓ_�ō\�����ꂽ�ӂ������ar�܂��́A�O���aR�ƌ�����Ă��邩����
//	//>	������Ă���ꍇ�ɂ́A���̌�_�����߂�B���߂���_�Ɠ����a�ȏ�O���a�ȓ��̗̈�ɂ���face���̐ߓ_��p���āA�O�p�`�����A���̖ʐς����߂�B
//}


//
//void PHFemThermo::CalcIHdqdt2(double r,double R,double dqdtAll,unsigned num){
//	///	�����a�ƊO���a�̊Ԃ̐ߓ_�ɔM�������E������ݒ�
//	//> �~�ŋ�؂���l�ʑ̖ʂ̗̈���O�p�`�ŋߎ�����
//
//	//> ���M����l�ʑ̖ʂ̖ʐς̑��a�����߂�
//
//	//%%	�菇 
//	//> isHeated(->nearIHHeater)��faces�̓��A3�̐ߓ_�S���ɂ��āA���蔼�a�̒��ɓ����Ă�����̂����邩�ǂ������`�F�b�N����B
//	//>	�����Ă�����̂��A��������A�ʐόv�Z�����������A�����ŁA���̐ߓ_�������Ă��邩�ɂ���ďꍇ�������s���B
//	//>	�R��:face�̖ʐς�faceS�ɑ������	1~2��:�ʐς��ߎ��v�Z���邽�߂ɁA3�ߓ_�̓��A�ǂ̐ߓ_�Ɛߓ_�ō\�����ꂽ�ӂ������ar�܂��́A�O���aR�ƌ�����Ă��邩����
//	//>	������Ă���ꍇ�ɂ́A���̌�_�����߂�B���߂���_�Ɠ����a�ȏ�O���a�ȓ��̗̈�ɂ���face���̐ߓ_��p���āA�O�p�`�����A���̖ʐς����߂�B
//
//	double faceS = 0.0;
//	for(unsigned i=0;i < nSurfaceFace; i++){
//		if(faces[i].mayIHheated){			// face�̐ߓ_��y���W�����̏ꍇ��IH���M�̑Ώېߓ_ �~�͈͓̔��ɓ����Ă���Ƃ͌���Ȃ�
//			unsigned nObinnerVtx = 0;
//			unsigned inner[3] = {0,0,0};
//			unsigned judge[2] = {0,0};		///	judge[0],[1]�̏��Ɍ��_�ɋ߂��_�̔��茋��
//			if(faces[i].area==0) faces[i].area = CalcTriangleArea(faces[i].vertices[0],faces[i].vertices[1],faces[i].vertices[2]);		// �ʐόv�Z���ς�łȂ���Όv�Z����
//				//> �u��������0,1�����ւ��邾��(=�|�����[�t�B�Y��)�Ŏc��̎��������
//				// ���L�������֐�������?�B�_��ID������΁A��������������֐�
//				//> 0-1�Ō�_������Ă���Ƃ� true false �͘_���Z�̔r���I�_���aXOR��true���A��_���v�Z���� 00=0,01=10=1,11=0; �Е��̓_���͈͓��ŁA����������͈͊O�̏ꍇ�Atrue�ɂȂ�
//			
//				//> �~�͈͓̔��ɓ����Ă�����AmayIHheated��true�ɁA���邢�́Afalse�ɂ���(�ꉞ)
//
//				/// faces[i]�̒��_�����A�̈���ɓ����Ă��邩��������
//				for(unsigned j=0;j<3;j++){
//					if( r <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= R){
//						nObinnerVtx += 1;
//						inner[j] = 1;
//					}
//				}
//
//				//> nObinnerVtx�̒l�ŏ�������
//			
//				///	
//				if(nObinnerVtx == 1){
//					Vec2d vtxXY[3];			/// face�̕ӂƔ��ar,R�Ƃ̌�_��x,z���W�@�ő�R�_
//					for(unsigned j=0;j<3;j++){		///	face���\������3���_�ɂ���
//						unsigned k = 0;
//						unsigned m = 0;
//						k = (j+1)%3;		///	j=0;k=1, j=1;k=2, j=2;k=0 
//						m = (j+2)%3;
//						//vtxXY[j] = CalcIntersectionPoint(faces[i].vertices[j],faces[i].vertices[k],r,R);
//						//DSTR << "vtxXY[" << j << "]: " << vtxXY[j] << std::endl; 
//						if(inner[j] ==1){			/// faces[i]��j�Ԗڂ̐ߓ_���~�̈���ɓ����Ă���
//						/// j�Ԗڂ̒��_�ƃG�b�W���\������_(���̂Q�_)���g���āA���ar,R�ƌ����_���Z�o����						
//						//> j 0,1,2
//							if(j == 0){	/// ���_j�ƕӂ���钸�_���g���āA�ӂƌ�����_�����߂�
//								//k = (j+1)%3;		///	j=0;k=1, j=1;k=2, j=2;k=0 
//							vtxXY[j] = CalcIntersectionPoint(faces[i].vertices[j],faces[i].vertices[k],r,R);
//							vtxXY[k] = CalcIntersectionPoint(faces[i].vertices[j],faces[i].vertices[m],r,R);
//							}
//							else if(j == 1){
//							vtxXY[j] = CalcIntersectionPoint(faces[i].vertices[j],faces[0].vertices[1],r,R);
//							vtxXY[j] = CalcIntersectionPoint(faces[i].vertices[j],faces[2].vertices[2],r,R);
//							}
//							else if(j == 2){
//							vtxXY[j] = CalcIntersectionPoint(faces[i].vertices[j],faces[0].vertices[1],r,R);
//							vtxXY[j] = CalcIntersectionPoint(faces[i].vertices[j],faces[1].vertices[2],r,R);
//							}
//							else{
//								assert(0);
//								DSTR << "Error in PHFemThermo::CalcIHdqdt2(hogehoge) �̈攻��Ƀ~�X" << std::endl;
//							}
//						 }
//					}
//				}
//
//				//for(unsigned j=0;j<3;j++){
//				//	/// face���̐ߓ_�����ԂɃ`�F�b�N����
//				//	unsigned k =0;
//				//	k = (j+1)%3;		///	j=0;k=1, j=1;k=2, j=2;k=0 
//				//	if( r <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= R){
//				//	}
//				//	if( r <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= R){
//				//		//> j,k��faces[i].vertices[j],faces[i].vertices[k]�Ƃ��đ��
//				//		CalcIntersectionPoint(faces[i].vertices[0],faces[i].vertices[1],r,R);
//				//	}
//				//}
//
//				//> r -> radius
//				//> R -> Radius
//			
//				/// ���W(consX ,constY)���~�ƎO�p�`�̕ӂƂ̌�_
//			
//				//unsigned vtxId0 = faces[i].vertices[0];
//				//unsigned vtxId1 = faces[i].vertices[1];
//			}
//			////> 1-2�Ō�_������Ă���Ƃ�	//
//			//else if( (r <= vertices[faces[i].vertices[1]].disFromOrigin && vertices[faces[i].vertices[1]].disFromOrigin <= R) ^ (vertices[faces[i].vertices[2]].disFromOrigin && vertices[faces[i].vertices[2]].disFromOrigin <= R)){
//			//	//> �~�Ƃ̌�_�����߂�
//			//	int katoon00 =0;
//			//}
//			////> 0-2�Ō�_������Ă���Ƃ�
//			//else if( (r <= vertices[faces[i].vertices[0]].disFromOrigin && vertices[faces[i].vertices[0]].disFromOrigin <= R) ^ (vertices[faces[i].vertices[2]].disFromOrigin && vertices[faces[i].vertices[2]].disFromOrigin <= R)){
//			//	//> �~�Ƃ̌�_�����߂�
//			//	int katoon000 =0;
//			//}
//
//			//for(unsigned j=0;j<3;j++){
//			//	if( (r <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= R) * (r <= vertices[faces[i].vertices[j+1]].disFromOrigin && vertices[faces[i].vertices[j+1]].disFromOrigin <= R)){
//			//		if(r <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= R)
//			//		nObinnerVtx += 1;
//			//	}
//			//}
//			//if( nObinnerVtx == 1)			faces[i].fluxarea = 1.5/3.0 * faces[i].area;//faces[i].fluxarea = 1.0/3.0 * faces[i].area;
//			//else if(nObinnerVtx == 2)		faces[i].fluxarea = 2.8/3.0 * faces[i].area;//faces[i].fluxarea = 2.0/3.0 * faces[i].area;
//			//else if(nObinnerVtx == 3)		faces[i].fluxarea = faces[i].area;
//			//else if(nObinnerVtx == 0)		faces[i].fluxarea = 0;
//
//			//if(faces[i].fluxarea >= 0){	
//			//	faceS += faces[i].fluxarea;
//			//}else{	assert(0);	}		//	faces[i].fluxarea��0�����̐����������Ă���̂ɉ��Z���悤�Ƃ��Ă���
//			//DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
//		}
//
//	//for(unsigned i=0;i < nSurfaceFace; i++){
//	//	DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
//	//}
//
//	if(faceS > 0){
//		//> dqdt ��P�ʖʐς�����ɒ���([1/m^2])
//		double dqdtds = dqdtAll / faceS;
////		DSTR << "dqdtds:  " << dqdtds << std::endl;
//		//>	�ȉ��A�M������faces�Ɋi�[����
//		//>	�M�����̖ʐόv�Z��fluxarea��p���čs��
//		for(unsigned i=0;i < nSurfaceFace; i++){
//			if(faces[i].mayIHheated){
//				faces[i].heatflux[num] = dqdtds * faces[i].fluxarea;		//	�M�����̗ʂ�heatflux�̖ʐς���v�Z
////				DSTR << "faces[" << i <<"].heatflux: " << faces[i].heatflux <<std::endl;
//			}
//		}
//	}
//	//�@�ȏ�A�l�͓����Ă���悤��
//	int katoon =0;
//	//���������āACreateMatk2t���R�s�[�����֐��ŁAVecf2?�������
//
//	//>	�M�ʂ́Adqdtds��p����
//
//	//> r <= <= R�̒��S������ˏ�ɉ��M
//
//	//	�ߓ_��dqdt�̒l���X�V����
//
//	//�@�ȉ��́A�x�N�g�������֐��̎d��
//	//	�ߓ_�̑�����\�ʂ̖ʂŁA�v�Z����
//	//  vertices[].heatFluxValue����Ɍv�Z��i�߂�
//	//	�K�E�X�U�C�f���v�Z�ł���悤�ɏ����ȂǁA��������
//}

Vec2d PHFemThermo::GetIHbandDrawVtx(){	return IHLineVtxX;	}

void PHFemThermo::SetIHbandDrawVtx(double xS, double xE){
	IHLineVtxX = Vec2d(xS,xE);
}

void PHFemThermo::CalcIHdqdtband_(double xS,double xE,double dqdtAll,unsigned num){
	PHFemMeshNew* mesh = GetPHFemMesh();

	///	x���W�FxS~xE�̊Ԃ̐ߓ_�ɔM�������E������ݒ�
	// xS,��E�̊Ԃɂ����ꂩ��_������A
	// 
	// mayIHheated�͎g��Ȃ�	:���̏������ŁAface�̑S�ߓ_��y���W�����̂��̂ɂ��Ă̂݁AfacesS�ɉ��Z

	//> ���M����l�ʑ̖ʂ̖ʐς̑��a�����߂�
	double faceS = 0.0;			// initialize
	for(unsigned i=0;i < mesh->nSurfaceFace; i++){
		//if(faces[i].mayIHheated){			// face�̐ߓ_��y���W�����̏ꍇ��IH���M�̑Ώېߓ_
			unsigned nObinnerVtx = 0;
			if(faceVars[i].area==0) faceVars[i].area = CalcTriangleArea(mesh->faces[i].vertexIDs[0],mesh->faces[i].vertexIDs[1],mesh->faces[i].vertexIDs[2]);
			//..�S�ߓ_��y<0�Ȃ�Afluxarea�ɉ��Z
			if(mesh->vertices[mesh->faces[i].vertexIDs[0]].pos.y < 0 && mesh->vertices[mesh->faces[i].vertexIDs[1]].pos.y < 0 && mesh->vertices[mesh->faces[i].vertexIDs[2]].pos.y < 0){
				for(unsigned j=0;j<3;j++){
					// ||.. :x, |.|.:o , .|.|:o ,   .||.:o  , ..||:x 
					//. �ǂꂩ�P�_��xS~xE�̊Ԃɂ���A���́A�ׂ荇��2�_���o���h�̊O���ɂ���AIHarea�ɎZ��
					if( xS <= mesh->vertices[mesh->faces[i].vertexIDs[j]].pos.x && mesh->vertices[mesh->faces[i].vertexIDs[j]].pos.x <= xE){
						faceVars[i].fluxarea[0] = faceVars[i].area;
						break;
					}
					//.���_�΂��o���h�����ނƂ�
					if(mesh->vertices[mesh->faces[i].vertexIDs[j]].pos.x <= xS && xE <= mesh->vertices[mesh->faces[i].vertexIDs[(j+1)%3]].pos.x 
						|| mesh->vertices[mesh->faces[i].vertexIDs[(j+1)%3]].pos.x <= xS && xE <= mesh->vertices[mesh->faces[i].vertexIDs[j]].pos.x ){
							faceVars[i].fluxarea[0] = faceVars[i].area;
						break;
					}
				}
			}
			if(faceVars[i].fluxarea >= 0){	
				faceS += faceVars[i].fluxarea[0];
			}else{		assert(0);	}		//	faces[i].fluxarea��0�����̐����������Ă���̂ɉ��Z���悤�Ƃ��Ă���
			//DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
		//}
	}

	//..face�ʐςɉ������M������S�̖̂ʐϊ�����苁�߂�
	if(faceS > 0){
		//> dqdt ��P�ʖʐς�����ɒ���([1/m^2])
		//double dqdt_ds = dqdtAll / faceS;
//		DSTR << "dqdtds:  " << dqdtds << std::endl;
		//>	�ȉ��A�M������faces�Ɋi�[����
		//>	�M�����̖ʐόv�Z��fluxarea��p���čs��
		for(unsigned i=0;i < mesh->nSurfaceFace; i++){
			if(faceVars[i].fluxarea){
				faceVars[i].heatflux[num][0] = faceVars[i].fluxarea[0] / faceS * dqdtAll;		//	�M�����̗ʂ�heatflux�̖ʐς���v�Z
//				DSTR << "faces[" << i <<"].heatflux: " << faces[i].heatflux <<std::endl;			
				// debug
				//for(unsigned j = 0; j < 3;++j){
 				//	vertices[faces[i].vertices[j]].temp = 100.0;
				//}
			}
		}
	}

	//	�f�o�b�O�E�\���p
	SetIHbandDrawVtx(xS,xE);
}

void PHFemThermo::CalcIHdqdtband(double xS,double xE,double dqdtAll,unsigned num){
	PHFemMeshNew* mesh = GetPHFemMesh();

	///	x���W�FxS~xE�̊Ԃ̐ߓ_�ɔM�������E������ݒ�

	//> ���M����l�ʑ̖ʂ̖ʐς̑��a�����߂�
	double faceS = 0.0;
	for(unsigned i=0;i < mesh->nSurfaceFace; i++){
		if(faceVars[i].mayIHheated){			// face�̐ߓ_��y���W�����̏ꍇ��IH���M�̑Ώېߓ_
			unsigned nObinnerVtx = 0;
			if(faceVars[i].area==0) faceVars[i].area = CalcTriangleArea(mesh->faces[i].vertexIDs[0],mesh->faces[i].vertexIDs[1],mesh->faces[i].vertexIDs[2]);
			for(unsigned j=0;j<3;j++){
				if( xS <= mesh->vertices[mesh->faces[i].vertexIDs[j]].pos.x && mesh->vertices[mesh->faces[i].vertexIDs[j]].pos.x <= xE){
					nObinnerVtx += 1;
				}
			}
			if( nObinnerVtx == 1)			faceVars[i].fluxarea[0] = faceVars[i].area;//faces[i].fluxarea = 1.0/3.0 * faces[i].area;
			else if(nObinnerVtx == 2)		faceVars[i].fluxarea[0] = faceVars[i].area;//faces[i].fluxarea = 2.0/3.0 * faces[i].area;
			else if(nObinnerVtx == 3)		faceVars[i].fluxarea[0] = faceVars[i].area;
			else if(nObinnerVtx == 0)		faceVars[i].fluxarea[0] = 0.0;

			if(faceVars[i].fluxarea >= 0){	
				faceS += faceVars[i].fluxarea[0];
			}else{		assert(0);	}		//	faces[i].fluxarea��0�����̐����������Ă���̂ɉ��Z���悤�Ƃ��Ă���
			//DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
		}
	}

	//for(unsigned i=0;i < nSurfaceFace; i++){
	//	DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
	//}



	if(faceS > 0){
		//> dqdt ��P�ʖʐς�����ɒ���([1/m^2])
		double dqdtds = dqdtAll / faceS;
//		DSTR << "dqdtds:  " << dqdtds << std::endl;
		//>	�ȉ��A�M������faces�Ɋi�[����
		//>	�M�����̖ʐόv�Z��fluxarea��p���čs��
		for(unsigned i=0;i < mesh->nSurfaceFace; i++){
			if(faceVars[i].mayIHheated){
				faceVars[i].heatflux[num][0] = dqdtds * faceVars[i].fluxarea[0];		//	�M�����̗ʂ�heatflux�̖ʐς���v�Z
//				DSTR << "faces[" << i <<"].heatflux: " << faces[i].heatflux <<std::endl;
			}
		}
	}
	
	//�@�ȏ�A�l�͓����Ă���悤��
	int katoon =0;
}

void PHFemThermo::CalcIHdqdt_decrease_high(double r,double R,double dqdtAll,unsigned mode){
	PHFemMeshNew* mesh = GetPHFemMesh();

	//dqdtAll��P�ʖʐϕӂ�ʂɒ���
	double dqdtdsAll =0.0;

	///	�����a�ƊO���a�̊Ԃ̐ߓ_�ɔM�������E������ݒ�
	//	�����ł��̈�ɂ������Ă���΁AIH���M�Ɋ܂߂�
	int cnt = 0;
	//	������
	for(unsigned i=0;i<mesh->faces.size();i++){
		faceVars[i].fluxarea[2] = 0.0;
	}
	//> ���M����l�ʑ̖ʂ̖ʐς̑��a�����߂�
	double faceS = 0.0;
	for(unsigned i=0;i < mesh->nSurfaceFace; i++){
		if(faceVars[i].mayIHheated){			// face�̐ߓ_��y���W�����̏ꍇ��IH���M�̑Ώېߓ_
			if(faceVars[i].area==0) faceVars[i].area = CalcTriangleArea(mesh->faces[i].vertexIDs[0],mesh->faces[i].vertexIDs[1],mesh->faces[i].vertexIDs[2]);
			for(unsigned j=0;j<3;j++){
				if( r <= vertexVars[mesh->faces[i].vertexIDs[j]].disFromOrigin && vertexVars[mesh->faces[i].vertexIDs[j]].disFromOrigin <= R){
					faceVars[i].fluxarea[2] = faceVars[i].area;
					break;
				}
			}
			faceS += faceVars[i].fluxarea[2];
			cnt+=1;
		}
	}
	double debugS=0.0;
	double debugdq=0.0;
	if(faceS > 0){
		//dqdtdsAll = dqdtAll / faceS;
		//> dqdt ��P�ʖʐς�����ɒ���([1/m^2])
		double dqdtds = dqdtAll / faceS;
		//>	�ȉ��A�M������faces�Ɋi�[����
		//>	�M�����̖ʐόv�Z��fluxarea��p���čs��
		for(unsigned i=0;i < mesh->nSurfaceFace; i++){
			if(faceVars[i].mayIHheated){
				double avgTemp=0.0;
				double ohm = 0.0;
				avgTemp = 1.0 / 3.0 * ( vertexVars[mesh->faces[i].vertexIDs[0]].temp + vertexVars[mesh->faces[i].vertexIDs[1]].temp + vertexVars[mesh->faces[i].vertexIDs[2]].temp);
				if(avgTemp >= 25.0 && avgTemp < 102.0){
					ohm = (avgTemp - 25) * (9.92e-10) + 5.86e-7;
				}
				else if(102.0 <= avgTemp && avgTemp < 202.0){
					ohm = (avgTemp -102.0) * 9.2e-10 + 6.87e-7;
				}
				else if(202.0<= avgTemp && avgTemp < 302.0){
					ohm = (avgTemp -202.0) * 8.3e-10 + 7.79e-7;
				}
				else if(302.0 <= avgTemp && avgTemp <402.0){
					ohm = (avgTemp -302.0) * 7.5e-10 + 8.62e-7;
				}
				if(avgTemp > 25.0){
					faceVars[i].heatflux[mode][0] = dqdtds * (5.86e-7)/(ohm);		//	�M�����̗ʂ�heatflux�̖ʐς���v�Z���AJ/m^2�ɒ����Ȃ���΂����Ȃ����s��Ōv�Z���邽�߂ɁB
				}else{
					faceVars[i].heatflux[mode][0] = dqdtds;
				}
				//debug
				debugdq += dqdtds * faceVars[i].fluxarea[2];
				debugS += faceVars[i].fluxarea[2];
			}
		} 
	}
	if(debugS != faceS){ DSTR << "diff between debugS:" << debugS << ", faceS:" << faceS << std::endl; }
	if( debugdq <= dqdtAll - 1e-8 &&  dqdtAll + 1e-8 <= debugdq){	DSTR << "�ʐς���̓����ł͂Ȃ�" <<std::endl;} 	//��̓����ł͂Ȃ��Ƃ��ɁA�x��			��̓����Ƃ��̏�������@dqdtAll - 1e-8 <= debugdq && debugdq <= dqdtAll + 1e-8: ��̓��� 
}		// /*CalcIHdqdt_decrease*/

void PHFemThermo::CalcIHdqdt_decrease(double r,double R,double dqdtAll,unsigned mode){
	PHFemMeshNew* mesh = GetPHFemMesh();

	//dqdtAll��P�ʖʐϕӂ�ʂɒ���
	double dqdtdsAll =0.0;

	///	�����a�ƊO���a�̊Ԃ̐ߓ_�ɔM�������E������ݒ�
	//	�����ł��̈�ɂ������Ă���΁AIH���M�Ɋ܂߂�
	int cnt = 0;
	//	������
	for(unsigned i=0;i<mesh->faces.size();i++){
		faceVars[i].fluxarea[2] = 0.0;
	}
	//> ���M����l�ʑ̖ʂ̖ʐς̑��a�����߂�
	double faceS = 0.0;
	for(unsigned i=0;i < mesh->nSurfaceFace; i++){
		if(faceVars[i].mayIHheated){			// face�̐ߓ_��y���W�����̏ꍇ��IH���M�̑Ώېߓ_
			if(faceVars[i].area==0) faceVars[i].area = CalcTriangleArea(mesh->faces[i].vertexIDs[0],mesh->faces[i].vertexIDs[1],mesh->faces[i].vertexIDs[2]);
			for(unsigned j=0;j<3;j++){
				if( r <= vertexVars[mesh->faces[i].vertexIDs[j]].disFromOrigin && vertexVars[mesh->faces[i].vertexIDs[j]].disFromOrigin <= R){
					faceVars[i].fluxarea[2] = faceVars[i].area;
					break;
				}
			}
			faceS += faceVars[i].fluxarea[2];
			cnt+=1;
		}
	}
	double debugS=0.0;
	double debugdq=0.0;
	if(faceS > 0){
		//dqdtdsAll = dqdtAll / faceS;
		//> dqdt ��P�ʖʐς�����ɒ���([1/m^2])
		double dqdtds = dqdtAll / faceS;
		//>	�ȉ��A�M������faces�Ɋi�[����
		//>	�M�����̖ʐόv�Z��fluxarea��p���čs��
		for(unsigned i=0;i < mesh->nSurfaceFace; i++){
			if(faceVars[i].mayIHheated){
				faceVars[i].heatflux[mode][2] = -dqdtds;		//	�M�����̗ʂ�heatflux�̖ʐς���v�Z���AJ/m^2�ɒ����Ȃ���΂����Ȃ����s��Ōv�Z���邽�߂ɁB
				//debug
				debugdq += dqdtds * faceVars[i].fluxarea[2];
				debugS += faceVars[i].fluxarea[2];
			}
		} 
	}
	if(debugS != faceS){ DSTR << "diff between debugS:" << debugS << ", faceS:" << faceS << std::endl; }
	if( debugdq <= dqdtAll - 1e-8 &&  dqdtAll + 1e-8 <= debugdq){	DSTR << "�ʐς���̓����ł͂Ȃ�" <<std::endl;} 	//��̓����ł͂Ȃ��Ƃ��ɁA�x��			��̓����Ƃ��̏�������@dqdtAll - 1e-8 <= debugdq && debugdq <= dqdtAll + 1e-8: ��̓��� 
}		// /*CalcIHdqdt_decrease*/


void PHFemThermo::CalcIHdqdt_add_high(double r,double R,double dqdtAll,unsigned mode){
	PHFemMeshNew* mesh = GetPHFemMesh();

	//%%%			[1]�ɂ��Čv�Z
	//dqdtAll��P�ʖʐϕӂ�ʂɒ���
	double dqdtdsAll =0.0;

	///	�����a�ƊO���a�̊Ԃ̐ߓ_�ɔM�������E������ݒ�
	//	�����ł��̈�ɂ������Ă���΁AIH���M�Ɋ܂߂�
	int cnt = 0;
	//	������
	for(unsigned i=0;i<mesh->faces.size();i++){
		faceVars[i].fluxarea[1] = 0.0;
	}
	//> ���M����l�ʑ̖ʂ̖ʐς̑��a�����߂�
	double faceS = 0.0;
	for(unsigned i=0;i < mesh->nSurfaceFace; i++){
		if(faceVars[i].mayIHheated){			// face�̐ߓ_��y���W�����̏ꍇ��IH���M�̑Ώېߓ_
			if(faceVars[i].area==0) faceVars[i].area = CalcTriangleArea(mesh->faces[i].vertexIDs[0],mesh->faces[i].vertexIDs[1],mesh->faces[i].vertexIDs[2]);
			for(unsigned j=0;j<3;j++){
				if( r <= vertexVars[mesh->faces[i].vertexIDs[j]].disFromOrigin && vertexVars[mesh->faces[i].vertexIDs[j]].disFromOrigin <= R){
					faceVars[i].fluxarea[1] = faceVars[i].area;
					break;
				}
			}
			faceS += faceVars[i].fluxarea[1];
			cnt+=1;
		}
	}
	double debugS=0.0;
	double debugdq=0.0;
	if(faceS > 0){
		//dqdtdsAll = dqdtAll / faceS;
		//> dqdt ��P�ʖʐς�����ɒ���([1/m^2])
		double dqdtds = dqdtAll / faceS;
		//>	�ȉ��A�M������faces�Ɋi�[����
		//>	�M�����̖ʐόv�Z��fluxarea��p���čs��
		for(unsigned i=0;i < mesh->nSurfaceFace; i++){
			if(faceVars[i].mayIHheated){
				double avgTemp=0.0;
				double ohm = 0.0;
				avgTemp = 1.0 / 3.0 * ( vertexVars[mesh->faces[i].vertexIDs[0]].temp + vertexVars[mesh->faces[i].vertexIDs[1]].temp + vertexVars[mesh->faces[i].vertexIDs[2]].temp);
				if(avgTemp >= 25.0 && avgTemp < 102.0){
					ohm = (avgTemp - 25) * (9.92e-10) + 5.86e-7;
				}
				else if(102.0 <= avgTemp && avgTemp < 202.0){
					ohm = (avgTemp -102.0) * 9.2e-10 + 6.87e-7;
				}
				else if(202.0<= avgTemp && avgTemp < 302.0){
					ohm = (avgTemp -202.0) * 8.3e-10 + 7.79e-7;
				}
				else if(302.0 <= avgTemp && avgTemp <402.0){
					ohm = (avgTemp -302.0) * 7.5e-10 + 8.62e-7;
				}
				if(avgTemp > 25.0){
					faceVars[i].heatflux[mode][0] = dqdtds * (5.86e-7)/(ohm);		//	�M�����̗ʂ�heatflux�̖ʐς���v�Z���AJ/m^2�ɒ����Ȃ���΂����Ȃ����s��Ōv�Z���邽�߂ɁB
				}else{
					faceVars[i].heatflux[mode][0] = dqdtds;
				}
				//debug
				debugdq += dqdtds * faceVars[i].fluxarea[1];
				debugS += faceVars[i].fluxarea[1];
			}
		} 
	}
	if(debugS != faceS){ DSTR << "diff between debugS:" << debugS << ", faceS:" << faceS << std::endl; }
	if( debugdq <= dqdtAll - 1e-8 &&  dqdtAll + 1e-8 <= debugdq){	DSTR << "�ʐς���̓����ł͂Ȃ�" <<std::endl;} 	//��̓����ł͂Ȃ��Ƃ��ɁA�x��			��̓����Ƃ��̏�������@dqdtAll - 1e-8 <= debugdq && debugdq <= dqdtAll + 1e-8: ��̓��� 
}		// /*CalcIHdqdt_add*/

void PHFemThermo::CalcIHdqdt_add(double r,double R,double dqdtAll,unsigned mode){
	PHFemMeshNew* mesh = GetPHFemMesh();

	//%%%			[1]�ɂ��Čv�Z
	//dqdtAll��P�ʖʐϕӂ�ʂɒ���
	double dqdtdsAll =0.0;

	///	�����a�ƊO���a�̊Ԃ̐ߓ_�ɔM�������E������ݒ�
	//	�����ł��̈�ɂ������Ă���΁AIH���M�Ɋ܂߂�
	int cnt = 0;
	//	������
	for(unsigned i=0;i<mesh->faces.size();i++){
		faceVars[i].fluxarea[1] = 0.0;
	}
	//> ���M����l�ʑ̖ʂ̖ʐς̑��a�����߂�
	double faceS = 0.0;
	for(unsigned i=0;i < mesh->nSurfaceFace; i++){
		if(faceVars[i].mayIHheated){			// face�̐ߓ_��y���W�����̏ꍇ��IH���M�̑Ώېߓ_
			if(faceVars[i].area==0) faceVars[i].area = CalcTriangleArea(mesh->faces[i].vertexIDs[0],mesh->faces[i].vertexIDs[1],mesh->faces[i].vertexIDs[2]);
			for(unsigned j=0;j<3;j++){
				if( r <= vertexVars[mesh->faces[i].vertexIDs[j]].disFromOrigin && vertexVars[mesh->faces[i].vertexIDs[j]].disFromOrigin <= R){
					faceVars[i].fluxarea[1] = faceVars[i].area;
					break;
				}
			}
			faceS += faceVars[i].fluxarea[1];
			cnt+=1;
		}
	}
	double debugS=0.0;
	double debugdq=0.0;
	if(faceS > 0){
		//dqdtdsAll = dqdtAll / faceS;
		//> dqdt ��P�ʖʐς�����ɒ���([1/m^2])
		double dqdtds = dqdtAll / faceS;
		//>	�ȉ��A�M������faces�Ɋi�[����
		//>	�M�����̖ʐόv�Z��fluxarea��p���čs��
		for(unsigned i=0;i < mesh->nSurfaceFace; i++){
			if(faceVars[i].mayIHheated){
				faceVars[i].heatflux[mode][1] = dqdtds;		//	�M�����̗ʂ�heatflux�̖ʐς���v�Z���AJ/m^2�ɒ����Ȃ���΂����Ȃ����s��Ōv�Z���邽�߂ɁB
				//debug
				debugdq += dqdtds * faceVars[i].fluxarea[1];
				debugS += faceVars[i].fluxarea[1];
			}
		} 
	}
	if(debugS != faceS){ DSTR << "diff between debugS:" << debugS << ", faceS:" << faceS << std::endl; }
	if( debugdq <= dqdtAll - 1e-8 &&  dqdtAll + 1e-8 <= debugdq){	DSTR << "�ʐς���̓����ł͂Ȃ�" <<std::endl;} 	//��̓����ł͂Ȃ��Ƃ��ɁA�x��			��̓����Ƃ��̏�������@dqdtAll - 1e-8 <= debugdq && debugdq <= dqdtAll + 1e-8: ��̓��� 
}		// /*CalcIHdqdt_add*/

void PHFemThermo::CalcIHdqdt_atleast_map(Vec2d origin,double dqdtAll,unsigned mode){
	PHFemMeshNew* mesh = GetPHFemMesh();

	double faceSq[10] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};								//�@�~���̖ʐϕۑ�
	// 10sec��
	//double ratio[10] = {0.016646849,	0.022592152,	0.051129608,	0.087990488,	0.12960761,	0.162901308,	0.180737218,	0.168846611,	0.122473246,	0.057074911};		//	���M�������S����
	// 5sec��
	double ratio[10] = {0.007653061,	0.015306122,	0.043367347,	0.084183673,	0.130102041,	0.168367347,	0.18877551,	0.181122449,	0.130102041,	0.051020408};
	//	�ŏ���4������0.01�Ђ��āA���ɉ��Z
	//double ratio[10] = {0.002653061,	0.010306122,	0.038367347,	0.079183673,	0.130102041,	0.168367347,	0.18977551,	0.182122449,	0.131102041,	0.052020408};
	ratio[5] += ratio[0];
	ratio[0] -= ratio[0];
	for(unsigned i=1;i<5;++i){
		ratio[i] = ratio[i] - 0.007;
	}
	for(unsigned i=6;i<10;++i){
		ratio[i] = ratio[i] + 0.007;
	}
	double areaQ[10] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
	for(unsigned i=0;i<10;++i){
		areaQ[i] = ratio[i] * dqdtAll;		//	������1sec������̒l�ŗǂ��B���Ԑϕ��Œ������邽��
	}
	//double rsum=0;
	//double Qsum=0;
	//for(unsigned i=0;i<10;++i){
	//	rsum += ratio[i];
	//	Qsum += areaQ[i];
	//}
	//DSTR << "rsum:"<<rsum<<std::endl;
	//DSTR << "Qsum:"<<Qsum<<std::endl;
	//�@�v�Z����Ă��Ȃ��ꍇ�ɔ�����
	for(unsigned i=0;i < mesh->nSurfaceFace; i++){
		if(faceVars[i].mayIHheated){			// face�̐ߓ_��y���W�����̏ꍇ��IH���M�̑Ώېߓ_
			if(faceVars[i].area==0) faceVars[i].area = CalcTriangleArea(mesh->faces[i].vertexIDs[0],mesh->faces[i].vertexIDs[1],mesh->faces[i].vertexIDs[2]);
		}
	}

	//> �\��face�̓��A���_����eface�̐ߓ_�̃��[�J��(x,z)���W�n�ł̕��ʏ�̋����̌v�Z���Aface�̑S�ߓ_��y���W�����̂��̂ɑ΂��āAIH���M�̉\���������t���O��ݒ�
	for(unsigned i=0;i<mesh->nSurfaceFace;i++){
		//	(x,z)���ʂɂ�����mayIHheated��face�S�ߓ_�̌��_����̋������v�Z����
		for(unsigned j=0; j<3; j++){
			double dx = mesh->vertices[mesh->faces[i].vertexIDs[j]].pos.x - origin[0];
			double dz = mesh->vertices[mesh->faces[i].vertexIDs[j]].pos.z - origin[1];	//	�\�L��y�����A����z���W�������Ă���
			vertexVars[mesh->faces[i].vertexIDs[j]].disFromOrigin = sqrt( dx * dx + dz * dz);
		}
	}
	//���߂������ɉ����āA�ݒ肳�ꂽ���������𖞂����悤�ȉ��x���z�����
	float r[11];		//	10���߂̊O�����K�v�Ȃ���
	for(unsigned i=0;i<11;++i){
		r[i] = 0.01 * i;
	}

	//	���S������͈͓��̖ʐϘa�����߂�	face���_�Ŕ��f
	for(unsigned id=0;id<mesh->nSurfaceFace;++id){
		if(faceVars[id].mayIHheated){
			//	face�̏d�S�̌��_����̋��������߂�
			double grvPnt = 1.0 / 3.0 * (vertexVars[mesh->faces[id].vertexIDs[0]].disFromOrigin + vertexVars[mesh->faces[id].vertexIDs[1]].disFromOrigin +vertexVars[mesh->faces[id].vertexIDs[2]].disFromOrigin);
			for(unsigned i=0; i < 10;++i){
				if(i>0){
					if( (r[i-1] + r[i]) / 2.0 < grvPnt && grvPnt <= ( r[i] + r[i+1]) / 2.0 ){
						faceSq[i] += faceVars[id].area;
						faceVars[id].map = i;
						faceVars[id].fluxarea[3] = faceVars[id].area;
						//DSTR << "id:" << id << ", i:" << i << std::endl; 
					} 
				}
				else{
					if(grvPnt <=  (r[i] + r[i+1]) /2.0){
						faceSq[i] += faceVars[id].area;
						faceVars[id].map = i;
						faceVars[id].fluxarea[3] = faceVars[id].area;
						//DSTR << "id:" << id << ", i:" << i << std::endl;
					} 
				}

			}
		}		
	}
	//face�ߓ_�̂����A1�_�ł��͈͓��ɓ�������A�T�����~�߂āA���͈̔͂�face�Ɗm�肷��B�S���̃^�O�t����ɁA���Y�̈�̖ʐϘa�����߂Ȃ��ƃY������
	//double debugfaceSum=0;
	//for(unsigned i=0;i<10;i++){
	//	DSTR <<"faceSq["<< i << "]:"<<  faceSq[i] << std::endl;
	//	debugfaceSum += faceSq[i];
	//}
	double debugW=0;
	for(unsigned id=0;id<mesh->nSurfaceFace;++id){
		if(faceVars[id].mayIHheated){
			for(unsigned i=0;i < 10; ++i){
				if(faceSq[i]>0){
					//	���M�����Ώ�face�i�͈͓�face�j�̂�
					if(faceVars[id].map<10){
						faceVars[id].heatflux[mode][3] = areaQ[faceVars[id].map] / faceSq[i] ;		// J/m^2�ɒ����Batleast�����̃\�[�X�Q��(�M�����̗ʂ�heatflux�̖ʐς���v�Z���AJ/m^2�ɒ����Ȃ���΂����Ȃ����s��Ōv�Z���邽�߂ɁB)
						//debugW += faceSq[i] * faces[id].heatflux[mode][3];
					}
				}
			}
		}		
	}
	//debug
	//for(unsigned i=0;i<10;++i){
	//	if(faceSq[i]>0){
	//		debugW += areaQ[i] * faceSq[i] / faceSq[i];
	//		DSTR << faceSq[i] / faceSq[i] <<std::endl;
	//	}
	//}
	//DSTR << "�ʐϘa[m^2]:"<<debugfaceSum<<std::endl;
	//DSTR << "���M�a[W]:" << debugW <<std::endl;

}		// /*CalcIHdqdt_atleast_hogehoge*/

void PHFemThermo::CalcIHdqdt_atleast_high(double r,double R,double dqdtAll,unsigned mode){
	PHFemMeshNew* mesh = GetPHFemMesh();

	//dqdtAll��P�ʖʐϕӂ�ʂɒ���
	double dqdtdsAll =0.0;

	///	�����a�ƊO���a�̊Ԃ̐ߓ_�ɔM�������E������ݒ�
	//	�����ł��̈�ɂ������Ă���΁AIH���M�Ɋ܂߂�
	int cnt = 0;
	//	������
	for(unsigned i=0;i<mesh->faces.size();i++){
		faceVars[i].fluxarea[0] = 0.0;
		faceVars[i].fluxarea[1] = 0.0;
		faceVars[i].fluxarea[2] = 0.0;
	}
	//> ���M����l�ʑ̖ʂ̖ʐς̑��a�����߂�
	double faceS = 0.0;
	for(unsigned i=0;i < mesh->nSurfaceFace; i++){
		if(faceVars[i].mayIHheated){			// face�̐ߓ_��y���W�����̏ꍇ��IH���M�̑Ώېߓ_
			if(faceVars[i].area==0) faceVars[i].area = CalcTriangleArea(mesh->faces[i].vertexIDs[0],mesh->faces[i].vertexIDs[1],mesh->faces[i].vertexIDs[2]);
			for(unsigned j=0;j<3;j++){
				if( r <= vertexVars[mesh->faces[i].vertexIDs[j]].disFromOrigin && vertexVars[mesh->faces[i].vertexIDs[j]].disFromOrigin <= R){
					//|| vertices[faces[i].vertices[j]].edges[0] ){
					faceVars[i].fluxarea[0] = faceVars[i].area;
					break;
				}
			}
			faceS += faceVars[i].fluxarea[0];
			cnt+=1;
		}
	}
	double debugS=0.0;
	double debugdq=0.0;
	double powerRatio =0.0;
	if(faceS > 0){
		//dqdtdsAll = dqdtAll / faceS;
		//> dqdt ��P�ʖʐς�����ɒ���([1/m^2])
		double dqdtds = dqdtAll / faceS;
		//>	�ȉ��A�M������faces�Ɋi�[����
		//>	�M�����̖ʐόv�Z��fluxarea��p���čs��
		for(unsigned i=0;i < mesh->nSurfaceFace; i++){
			if(faceVars[i].mayIHheated){
				double avgTemp = 0.0;
				double ohm = 0.0;
				avgTemp = 1.0 / 3.0 * ( vertexVars[mesh->faces[i].vertexIDs[0]].temp + vertexVars[mesh->faces[i].vertexIDs[1]].temp + vertexVars[mesh->faces[i].vertexIDs[2]].temp);
				if(avgTemp >= 25.0 && avgTemp < 102.0){
					ohm = (avgTemp - 25) * (9.92e-10) + 5.86e-7;
				}
				else if(102.0 <= avgTemp && avgTemp < 202.0){
					ohm = (avgTemp -102.0) * 9.2e-10 + 6.87e-7;
				}
				else if(202.0<= avgTemp && avgTemp < 302.0){
					ohm = (avgTemp -202.0) * 8.3e-10 + 7.79e-7;
				}
				else if(302.0 <= avgTemp && avgTemp <402.0){
					ohm = (avgTemp -302.0) * 7.5e-10 + 8.62e-7;
				}
				//if(ohm > 5.91e-7){
				//	DSTR << "ohm: " << ohm << std::endl;
				//	DSTR << "avgTemp:" << avgTemp <<std::endl; 
				//	int katton=0;
				//}
				if(avgTemp > 25.0){
					faceVars[i].heatflux[mode][0] = dqdtds * (5.86e-7)/(ohm);		//	�M�����̗ʂ�heatflux�̖ʐς���v�Z���AJ/m^2�ɒ����Ȃ���΂����Ȃ����s��Ōv�Z���邽�߂ɁB
				}else{
					faceVars[i].heatflux[mode][0] = dqdtds;
				}
				//debug
				debugdq += dqdtds * faceVars[i].fluxarea[0];
				debugS += faceVars[i].fluxarea[0];
			}
		} 
	}
	if(debugS != faceS){ DSTR << "diff between debugS:" << debugS << ", faceS:" << faceS << std::endl; }
	if( debugdq <= dqdtAll - 1e-8 &&  dqdtAll + 1e-8 <= debugdq){	DSTR << "�ʐς���̓����ł͂Ȃ�" <<std::endl;} 	//��̓����ł͂Ȃ��Ƃ��ɁA�x��			��̓����Ƃ��̏�������@dqdtAll - 1e-8 <= debugdq && debugdq <= dqdtAll + 1e-8: ��̓��� 
	//�@�ȏ�A�l�͓����Ă���悤��
	//DSTR << "face ���M�ʐ�cnt: " << cnt<<std::endl;
}		// /*CalcIHdqdt_atleast*/

void PHFemThermo::CalcIHdqdt_atleast(double r,double R,double dqdtAll,unsigned mode){
	PHFemMeshNew* mesh = GetPHFemMesh();


	//dqdtAll��P�ʖʐϕӂ�ʂɒ���
	double dqdtdsAll =0.0;

	///	�����a�ƊO���a�̊Ԃ̐ߓ_�ɔM�������E������ݒ�
	//	�����ł��̈�ɂ������Ă���΁AIH���M�Ɋ܂߂�
	int cnt = 0;
	//	������
	for(unsigned i=0;i<mesh->faces.size();i++){
		faceVars[i].fluxarea[0] = 0.0;
		faceVars[i].fluxarea[1] = 0.0;
		faceVars[i].fluxarea[2] = 0.0;
	}
	//> ���M����l�ʑ̖ʂ̖ʐς̑��a�����߂�
	double faceS = 0.0;
	for(unsigned i=0;i < mesh->nSurfaceFace; i++){
		if(faceVars[i].mayIHheated){			// face�̐ߓ_��y���W�����̏ꍇ��IH���M�̑Ώېߓ_
			if(faceVars[i].area==0) faceVars[i].area = CalcTriangleArea(mesh->faces[i].vertexIDs[0],mesh->faces[i].vertexIDs[1],mesh->faces[i].vertexIDs[2]);
			for(unsigned j=0;j<3;j++){
				if( r <= vertexVars[mesh->faces[i].vertexIDs[j]].disFromOrigin && vertexVars[mesh->faces[i].vertexIDs[j]].disFromOrigin <= R){
					//|| vertices[faces[i].vertices[j]].edges[0] ){
					faceVars[i].fluxarea[0] = faceVars[i].area;
					break;
				}
			}
			faceS += faceVars[i].fluxarea[0];
			cnt+=1;
		}
	}
	double debugS=0.0;
	double debugdq=0.0;
	if(faceS > 0){
		//dqdtdsAll = dqdtAll / faceS;
		//> dqdt ��P�ʖʐς�����ɒ���([1/m^2])
		double dqdtds = dqdtAll / faceS;
		//>	�ȉ��A�M������faces�Ɋi�[����
		//>	�M�����̖ʐόv�Z��fluxarea��p���čs��
		for(unsigned i=0;i < mesh->nSurfaceFace; i++){
			if(faceVars[i].mayIHheated){
				faceVars[i].heatflux[mode][0] = dqdtds;		//	�M�����̗ʂ�heatflux�̖ʐς���v�Z���AJ/m^2�ɒ����Ȃ���΂����Ȃ����s��Ōv�Z���邽�߂ɁB
				//debug
				debugdq += dqdtds * faceVars[i].fluxarea[0];
				debugS += faceVars[i].fluxarea[0];
			}
		} 
	}
	if(debugS != faceS){ DSTR << "diff between debugS:" << debugS << ", faceS:" << faceS << std::endl; }
	if( debugdq <= dqdtAll - 1e-8 &&  dqdtAll + 1e-8 <= debugdq){	DSTR << "�ʐς���̓����ł͂Ȃ�" <<std::endl;} 	//��̓����ł͂Ȃ��Ƃ��ɁA�x��			��̓����Ƃ��̏�������@dqdtAll - 1e-8 <= debugdq && debugdq <= dqdtAll + 1e-8: ��̓��� 
	//�@�ȏ�A�l�͓����Ă���悤��
	//DSTR << "face ���M�ʐ�cnt: " << cnt<<std::endl;
}		// /*CalcIHdqdt_atleast*/

void PHFemThermo::CalcIHdqdt(double r,double R,double dqdtAll,unsigned num){
	PHFemMeshNew* mesh = GetPHFemMesh();

	///	�����a�ƊO���a�̊Ԃ̐ߓ_�ɔM�������E������ݒ�

	//> ���M����l�ʑ̖ʂ̖ʐς̑��a�����߂�
	double faceS = 0.0;
	for(unsigned i=0;i < mesh->nSurfaceFace; i++){
		if(faceVars[i].mayIHheated){			// face�̐ߓ_��y���W�����̏ꍇ��IH���M�̑Ώېߓ_
			unsigned nObinnerVtx = 0;
			if(faceVars[i].area==0) faceVars[i].area = CalcTriangleArea(mesh->faces[i].vertexIDs[0],mesh->faces[i].vertexIDs[1],mesh->faces[i].vertexIDs[2]);
			for(unsigned j=0;j<3;j++){
				if( r <= vertexVars[mesh->faces[i].vertexIDs[j]].disFromOrigin && vertexVars[mesh->faces[i].vertexIDs[j]].disFromOrigin <= R){
					nObinnerVtx += 1;
				}
			}
			if( nObinnerVtx == 1)			faceVars[i].fluxarea[0] = 1.5/3.0 * faceVars[i].area;//faces[i].fluxarea = 1.0/3.0 * faces[i].area;
			else if(nObinnerVtx == 2)		faceVars[i].fluxarea[0] = 2.8/3.0 * faceVars[i].area;//faces[i].fluxarea = 2.0/3.0 * faces[i].area;
			else if(nObinnerVtx == 3)		faceVars[i].fluxarea[0] = faceVars[i].area;
			else if(nObinnerVtx == 0)		faceVars[i].fluxarea[0] = 0;

			if(faceVars[i].fluxarea >= 0){	
				faceS += faceVars[i].fluxarea[0];
			}else{		assert(0);	}		//	faces[i].fluxarea��0�����̐����������Ă���̂ɉ��Z���悤�Ƃ��Ă���
			//DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
		}
	}

	//for(unsigned i=0;i < nSurfaceFace; i++){
	//	DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
	//}

	if(faceS > 0){
		//> dqdt ��P�ʖʐς�����ɒ���([1/m^2])
		double dqdtds = dqdtAll / faceS;
//		DSTR << "dqdtds:  " << dqdtds << std::endl;
		//>	�ȉ��A�M������faces�Ɋi�[����
		//>	�M�����̖ʐόv�Z��fluxarea��p���čs��
		for(unsigned i=0;i < mesh->nSurfaceFace; i++){
			if(faceVars[i].mayIHheated){
				faceVars[i].heatflux[num][0] = dqdtds * faceVars[i].fluxarea[0];		//	�M�����̗ʂ�heatflux�̖ʐς���v�Z
//				DSTR << "faces[" << i <<"].heatflux: " << faces[i].heatflux <<std::endl;
			}
		}
	}
	
	//�@�ȏ�A�l�͓����Ă���悤��

	int katoon =0;
	//���������āACreateMatk2t���R�s�[�����֐��ŁAVecf2?�������

	//>	�M�ʂ́Adqdtds��p����

	//> r <= <= R�̒��S������ˏ�ɉ��M

	//	�ߓ_��dqdt�̒l���X�V����

	//�@�ȉ��́A�x�N�g�������֐��̎d��
	//	�ߓ_�̑�����\�ʂ̖ʂŁA�v�Z����
	//  vertices[].heatFluxValue����Ɍv�Z��i�߂�
	//	�K�E�X�U�C�f���v�Z�ł���悤�ɏ����ȂǁA��������

}
/// face���ɍ���Ă��܂��̂��ǂ��̂��Avertices���Ƃɂ��̂������̂��B�ǂ�����������������Ȃ��̂ŁA�ЂƂ܂��Avertices���ɍ���Ă��܂���

//void PHFemThermo::SetVertexHeatFlux(int id,double heatFlux){
//	vertices[id].heatFluxValue = heatFlux;
//}

//void PHFemThermo::SetVtxHeatFluxAll(double heatFlux){
//	for(unsigned i=0; i < vertices.size() ;i++){
//		SetVertexHeatFlux(i,heatFlux);
//	}
//}


void PHFemThermo::CalcHeatTransDirect2(double dt){
	PHFemMeshNew* mesh = GetPHFemMesh();

	//lapack���p
	int n = (int)mesh->vertices.size();

	double eps =0.5;
	//	�W���s��̍쐬
	keisu.resize(mesh->vertices.size(),mesh->vertices.size());
	keisu.clear();

	PTM::VVector<double> uhen;
	uhen.resize(mesh->vertices.size(),1);
	uhen.clear();

	keisu = eps * matKAll + 1 / dt * matCAll;
	uhen = (- (1.0 - eps) * matKAll - 1 / dt * matCAll ) * TVecAll + vecFAllSum;

	//if(deformed) keisuInv = keisu.inv(); deformed = false;
	//TVecAll = keisuInv * uhen;

	//PTM::VMatrixRow<double> matk;		//keisu
	PTM::VVector<double> x;				//TVecAll
	//PTM::VVector<double> b;			//uhen
	PTM::VVector<int> ip;
	//int n= 5000;
	//matk.resize(n, n, 0.0);
	//b.resize(n);
	ip.resize(n);
	//for(int i=0; i<n; ++i){
	//	for(int j=0; j<n; ++j){
	//		matk[i][j] = rand();
	//	}
	//}
	//for(int i=0; i<n; ++i){
	//	b[i] = i*10+15;
	//}


	typedef double element_type;
	typedef bindings::remove_imaginary<element_type>::type real_type ;
	typedef bindings::remove_imaginary<int>::type int_type ;
	typedef ublas::vector< real_type > vector_type;
	typedef ublas::matrix< element_type, ublas::column_major > matrix_type;
	ublas::vector<int_type> ipiv(n);
	matrix_type mm(n, n);
	vector_type bb(n);
	for(int i=0; i<n; ++i){
		bb[i] = uhen[i];		//b
	}
	for(int i=0; i<n; ++i){
		for(int j=0; j<n; ++j){
			mm.at_element(i, j)=keisu[i][j];		//=matk
		}
	}
	double det = lapack::gesv(mm, ipiv, bb);
	x.resize(n);
	for(int i=0; i<n; ++i){
		x[i] = bb[i];
	}
	for(int i=0; i<n; ++i){
		TVecAll[i] = x[i];
	}
//	DSTR << "TVecAll: " << TVecAll <<  std::endl;

}

void PHFemThermo::CalcHeatTransDirect(double dt){
	PHFemMeshNew* mesh = GetPHFemMesh();

	if(strcmp(mesh->GetName(), "femNsteak") == 0){
		for(unsigned i=0; i < mesh->vertices.size(); i++){
			DSTR << vecFAllSum[i] << std::endl;
		}
		DSTR << std::endl;
	}

	//���ږ@���p

	double eps =0.5;
	double eps2 = 0.5;
	double eps3 = 1.0;
	eps = 1.0;
	
//	DSTR << "tets[0].volume: " << tets[0].volume << std::endl;
	
	TVecAll2.resize(mesh->vertices.size());
	TVecAll2 = TVecAll;


	PTM::VVector<double> TVecAll3;	
	//// define @.h
	TVecAll3.resize(mesh->vertices.size());
	TVecAll3 = TVecAll;


	//	�W���s��̍쐬
	keisu.resize(mesh->vertices.size(),mesh->vertices.size());
	keisu.clear();

	//DSTR << "tets.size(): " << tets.size() << ", vertices.size(): " << vertices.size() <<", edges.size(): " << edges.size()<<  std::endl;
	//DSTR << "eps: " << eps << std::endl;

	PTM::VVector<double> uhen;
	uhen.resize(mesh->vertices.size(),1);
	uhen.clear();
	//DSTR <<"TVecAll_before: "<< TVecAll <<std::endl;
	double TEMP =0.0;
	for(unsigned i=0; i<mesh->vertices.size();i++){
		TEMP += vecFAllSum[i];
	}
//	DSTR << "TEMP: " << TEMP << std::endl;

	keisu = eps * matKAll + 1 / dt * matCAll;

	////vecf�̕�����K�p
	//PTM::VVector<double> vecFAllTemp;
	//vecFAllTemp.resize(mesh->vertices.size());
	//vecFAllTemp.clear();
	//for(unsigned i=0; i < mesh->vertices.size(); i++){
	//	for(unsigned j=0; j < mesh->vertices.size(); j++){
	//		vecFAllTemp[j] += vecFAllSum[i] * fpat[j][i];
	//	}
	//}
	//uhen = (- (1.0 - eps) * matKAll + 1 / dt * matCAll ) * TVecAll + vecFAllTemp;

	//createNewVecF();

//	DSTR <<"keisu: " << keisu << std::endl;
	uhen = (- (1.0 - eps) * matKAll + 1 / dt * matCAll ) * TVecAll + vecFAllSum;
//	DSTR << "(- (1.0 - eps) * matKAll + 1 / dt * matCAll ) * TVecAll: " << (- (1.0 - eps) * matKAll + 1 / dt * matCAll ) * TVecAll <<std::endl;
//	DSTR << "vecFAllSum: "  << vecFAllSum <<std::endl;

	if(deformed){ 
		keisuInv = keisu.inv();
		deformed = false;
	}


	//DSTR << "keisuInv: " << keisuInv << std::endl;

	//PTM::VMatrixRow<double> VV;
	//VV.resize(vertices.size(),vertices.size());
	//VV.clear();
	//VV = keisu * keisuInv;

	//DSTR << "keisu * keisuInv" <<std::endl;
	//DSTR << VV << std::endl;
	//DSTR << "VV�Ίp����" <<std::endl;
	//for(unsigned i=0;i<vertices.size();i++){
	//	DSTR << VV[i][i] <<std::endl;
	//}

	//DSTR <<"keisu.det(): " << keisu.det() << std::endl;
	//DSTR << "keisuInv.det(): " << keisuInv.det() << std::endl;

	//int keisucnt =0;
	//for(int i=0;i<vertices.size();++i){
	//	for(int j=0;j<vertices.size();++j){
	//		if(keisu[i][j] <0){
	//			keisucnt +=1;
	//		} 
	//	}
	//}
//	DSTR <<keisucnt << std::endl;

	//DSTR << "before calculate "<< std::endl;
	//DSTR << "TVecAll:  " << TVecAll << std::endl; 
	//DSTR << "TVecAll2: " << TVecAll2 << std::endl; 
	//DSTR << "TVecAll3: " << TVecAll3 << std::endl; 

	
	//DSTR << "uhen: " << uhen << std::endl;
	//PTM::VMatrixRow<double> keisuInv2;
	//keisuInv2.resize(mesh->vertices.size(), mesh->vertices.size());
	//keisuInv2.clear();
	//
	//for(unsigned i=0; i < mesh->vertices.size(); i++){
	//	for(unsigned j=0; j < mesh->vertices.size(); j++){
	//		if(keisuInv[i][j] < 0){
	//			keisuInv2[i][j] = 0;
	//		}else{
	//			keisuInv2[i][j] = keisuInv[i][j]; 
	//		}
	//	}
	//}

	//TVecAll = keisuInv * (- (1.0 - eps) * (matKAll + 1 / dt * matCAll ) * TVecAll) + (1 / dt * matCAll).inv() * vecFAllSum;
	//TVecAll = keisuInv * ((- (1.0 - eps) * matKAll + 1 / dt * matCAll ) * TVecAll) + keisuInv2 * vecFAllSum;
	TVecAll = keisuInv * uhen;
	//TVecAll = preTVecAll + 2*tdt*matCAll.inv()*(vecFAllSum - matKAll*TVecAll);
	//preTVecAll = TVecAll3;
	//if(makeAFlag){
	//	A.resize(mesh->vertices.size(), mesh->vertices.size());
	//	ImA.resize(mesh->vertices.size(), mesh->vertices.size());
	//	IpAinv.resize(mesh->vertices.size(), mesh->vertices.size());
	//	keisuIA.resize(mesh->vertices.size(), mesh->vertices.size());
	//	
	//	A = 0.5 * tdt * matCAll.inv() * matKAll;
	//	ImA = I-A;
	//	IpAinv = (I + A).inv();
	//	keisuIA = IpAinv * ImA;

	//	makeAFlag = false;
	//}

	//TVecAll = keisuIA * TVecAll;

	////��Ίp�悤���̌v�Z��ɑΊp�v�f�𑫂�
	//PTM::VVector<double> pT;
	//pT.resize(mesh->vertices.size());
	//pT.clear();
	//TVecAll.clear();
	//for(unsigned i=0; i < mesh->vertices.size(); i++){
	//	for(unsigned j=0; j < mesh->vertices.size(); j++){
	//		if(i==j){
	//			pT[i] = keisuInv[i][j]*uhen[j];
	//		}else{
	//			TVecAll[i] += keisuInv[i][j]*uhen[j];
	//		}
	//	}
	//}
	//TVecAll += pT;
	double sumtemp=0.0;

	

	//for(unsigned i=0;i<vertices.size();i++){
	//	for(unsigned j=0; j < vertices[i].tets.size(); j++){
	//		sumtemp += tets[vertices[i].tets[j]].volume * RHO * SPECIFICHEAT * 5 / 20 * TVecAll[i]; 
	//	}
	//}
	//DSTR << "sumtemp: "<< sumtemp <<std::endl;


	//DSTR<< "TVecAll: " << TVecAll <<std::endl;

	//PTM::VMatrixRow<double> keisu2;
	//keisu2.resize(vertices.size(),vertices.size());
	//keisu2.clear();

	//keisu2 = keisuInv.inv();
	//int checkint=0;
	//for(unsigned i=0;i<vertices.size();i++){
	//	for(unsigned j=0;j<vertices.size();j++){
	//		if(keisu2[i][j] != keisu[i][j]){
	//			checkint +=1;
	//		}
	//	}
	//}
	//DSTR << "checkint: " << checkint << std::endl;

	//DSTR <<"keisu: ";
	//DSTR << keisu << std::endl;

	//DSTR << "keisuInvInv";
	//DSTR << keisu2 << std::endl;

	// �W���s��̔�0�������t�t�s���0�ɂȂ��Ă��邩�ǂ�����check
	//for(unsigned i=0;i<vertices.size();i++){
	//	for(unsigned j=0;j<vertices.size();j++){
	//		if(keisu[i][j]!=0){
	//			DSTR << keisu[i][j] - keisu2[i][j] << " - " <<  keisu[i][j] << " : "<< keisu2[i][j] << std::endl;
	//		}
	//	}
	//}

	//// �O������0���H
	//for(unsigned i=0;i<vertices.size();i++){
	//	for(unsigned j=0;j<vertices.size();j++){
	//		if(keisu[i][j] ==0){
	//			DSTR << keisu[i][j] - keisu2[i][j] << " - " <<  keisu[i][j] << " : "<< keisu2[i][j] << std::endl;
	//		}
	//	}
	//}


	//PTM::VVector<double> uhen2;
	//uhen2.resize(vertices.size(),1);
	//uhen2.clear();
	//PTM::VMatrixRow<double> keisuInv2;
	//keisuInv2.resize(vertices.size(),vertices.size());
	//keisuInv2.clear();

	//keisu2 = eps2 * matKAll + 1 / dt * matCAll;
	//keisuInv2 = keisu2.inv();
	//uhen2 = (- (1.0 - eps2) * matKAll + 1 / dt * matCAll ) * TVecAll2 + vecFAllSum;
	//TVecAll2 = keisuInv2 * uhen2;

	//
	//DSTR << uhen2 << std::endl;
	//DSTR << keisu2 << std::endl;

	//DSTR << keisuInv2 << std::endl;

	//PTM::VMatrixRow<double> keisu3;			//	���ږ@�Ōv�Z����T(t+dt)�W���s��
	//PTM::VMatrixRow<double> keisuInv3;
	//PTM::VVector<double> uhen3;
	//keisu3.resize(vertices.size(),vertices.size());
	//keisu3.clear();
	//uhen3.resize(vertices.size(),1);
	//uhen3.clear();

	//keisuInv3.resize(vertices.size(),vertices.size());
	//keisuInv3.clear();


	//keisu3 = eps3 * matKAll + 1 / dt * matCAll;
	//keisuInv3 = keisu3.inv();
	//uhen3 = (- (1.0 - eps3) * matKAll + 1 / dt * matCAll ) * TVecAll3 + vecFAllSum;

	//TVecAll3 = keisuInv3 * uhen3;


	//int diff_keisu=0;
	//int diff_uhen=0;
	//int diff_keisuInv=0;
	//int diff_TVecAll=0;
	//int diff_TVecAll13=0;

	//int dk2 = 0;
	//int du2 = 0;
	//int di2 = 0;


	//for(int i=0;i<vertices.size();i++){
	//	for(int j=0;j<vertices.size();j++){
	//		if(keisu[i][j] != keisu2[i][j]){
	//			diff_keisu += 1;
	//		}
	//		if(keisu[i][j] != keisu3[i][j]){
	//			dk2 += 1;
	//		}
	//		if(keisuInv[i][j] != keisuInv2[i][j]){
	//			diff_keisuInv += 1;
	//		}
	//		if(keisuInv[i][j] != keisuInv3[i][j]){
	//			di2 += 1;
	//		}
	//	}
	//	if(uhen[i] != uhen2[i]){
	//		diff_uhen += 1;
	//	}
	//	if(uhen[i] != uhen3[i]){
	//		du2 += 1;
	//	}
	//	if(TVecAll[i] != TVecAll2[i]){
	//		diff_TVecAll += 1;
	//	}
	//	if(TVecAll[i] != TVecAll2[i] || TVecAll[i] != TVecAll3[i] || TVecAll2[i] != TVecAll3[i] ){
	//		diff_TVecAll13 += 1;
	//		if(i==0)DSTR << "eps: " << eps <<", "<< eps2 << ", " << eps3<<std::endl;
	//		DSTR << TVecAll[i] << ", " << TVecAll2[i] << ", " << TVecAll3[i] << std::endl;
	//	}
	//}
	//DSTR << "diff_keisu: " << diff_keisu << ", diff_uhen: " << diff_uhen <<", diff_keisuInv: " << diff_keisuInv << ", diff_TVecAll: " << diff_TVecAll << std::endl; 
	//DSTR << "diff_keisu(1-3): " << dk2 << ", diff_uhen(1-3): " << du2 <<", diff_keisuInv(1-3): " << di2 << "diff_TVecAll1-3" << diff_TVecAll13 <<std::endl;

	//DSTR <<"TVecAll_calculated: "<< TVecAll <<std::endl;
	//DSTR <<"TVecAll2_calculated: "<< TVecAll2 <<std::endl;
	//DSTR <<"TVecAll3_calculated: "<< TVecAll3 <<std::endl;
	//DSTR << "TVecAll[0]" << TVecAll[0] <<std::endl;
	//DSTR << "TVecAll2[0]" << TVecAll2[0] <<std::endl;
	//DSTR << "TVecAll3[0]" << TVecAll3[0] <<std::endl;

	int debughensu=0;
}

void PHFemThermo::CalcHeatTransUsingScilab(double dt, double eps){
	PHFemMeshNew* mesh = GetPHFemMesh();

	//���ږ@���p

	//PTM::TMatrixRow<1,1,double> dtMat;
	//PTM::TMatrixRow<1,1,double> epsMat;
	PTM::VMatrixRow<double> uhenScilab;
	PTM::VMatrixRow<double> TAll;

	uhenScilab.resize(mesh->vertices.size(), 1);
	TAll.resize(mesh->vertices.size(), 1);
	uhenScilab.clear();
	TAll.clear();

	//dtMat.clear();
	//epsMat.clear();
	//dtMat[0][0] = dt;


	TVecAll2.resize(mesh->vertices.size());
	TVecAll2 = TVecAll;


	PTM::VVector<double> TVecAll3;	
	TVecAll3.resize(mesh->vertices.size());
	TVecAll3 = TVecAll;


	//	�W���s��̍쐬
	keisu.resize(mesh->vertices.size(),mesh->vertices.size());
	keisu.clear();

	PTM::VVector<double> uhen;
	uhen.resize(mesh->vertices.size(),1);
	uhen.clear();
	double TEMP =0.0;
	for(unsigned i=0; i<mesh->vertices.size();i++){
		TEMP += vecFAllSum[i];
	}

	keisu = eps * matKAll + 1 / dt * matCAll;

	uhen = (- (1.0 - eps) * matKAll + 1 / dt * matCAll ) * TVecAll + vecFAllSum;

	if(deformed){ 
		//keisuInv = keisu.inv();
		ScilabSetMatrix("keisu", keisu);
		ScilabJob("keisuInv = inv(keisu)");
		ScilabGetMatrix(keisuInv, "keisuInv");
		deformed = false;
	}

	//for(unsigned i=0; i < uhen.size(); i++){
	//	uhenScilab[i][0] = uhen[i];
	//}
	//ScilabSetMatrix("uhen", uhenScilab);
	//ScilabJob("TAll = -linsolve(keisu, uhen)");
	//ScilabGetMatrix(TAll, "TAll");

	//for(unsigned i=0; i < mesh->vertices.size(); i++){
	//	TVecAll[i] = TAll[i][0];
	//}

	TVecAll = keisuInv * uhen;
	
	double sumtemp=0.0;
}

//// �K�E�X�U�C�f���@���g��Ax+b>0������
//	template <class AD, class XD, class BD>
//	void GaussSeidel(MatrixImp<AD>& a, VectorImp<XD>& x, const VectorImp<BD>& b){
//		int nIter = 15;					// �����񐔂̏��
//		double error = 0.0;
//		double errorRange = 10e-8;		// ���e�덷
//		int n = (int)a.height();		// �A���������̐�(�s��a�̍s��)
//		std::vector< double > lastx;
//		for(int i = 0; i < n; i++){
//			lastx.push_back(x[i]);
//			x[i] = 0;
//		}
//
//		for(int k = 0; k < nIter; k++){		
//			for(int i = 0; i < n; i++){
//				double term1 = 0.0;
//				double term2 = 0.0;
//				for(int j = 0; j < i; j++){
//					term1 += a[i][j] * x[j];
//				}
//				for(int j = i+1; j < n; j++){
//					term2 += a[i][j] * lastx[j];
//				}
//				// x�̍X�V(�J��Ԃ��v�Z�̎����g�p)
//				x[i] =  (-b[i] - term1 - term2) / a[i][i];
//				if(x[i] < 0) x[i] = 0.0;
//			}
//
//			// (lastx - x)��2��̑��a�ƌ덷�͈͂��r
//			error = 0.0;
//			for(int i = 0; i < n; i++){
//				error += pow(x[i] - lastx[i], 2);
//				//DSTR << "iterete" << i << "," << x[i] << std::endl;
//			}
//			if(error < errorRange){
//				//DSTR << "Finish the iteration in admissible error. " << std::endl;
//				//DSTR << k << std::endl;
//				return;
//			}
//
//			// �J��Ԃ��v�Z�̂��߂ɍX�V���x��lastx�ɕۑ�
//			for(int i = 0; i < n; i++) lastx[i] = x[i];
//		}
//		//nIter�Ōv�Z���I���Ȃ������̂őł��؂�
//		//static int iterError = 0;
//		//iterError += 1;
//		//DSTR << iterError << "Could not converge in iteration steps. Error = " << error << std::endl;
//		//CSVOUT << error << std::endl;
//	}

//void PHFemThermo::CalcHeatTransUsingGaussSeidel2(unsigned NofCyc,double dt,double eps){
//	double error = 0.0;
//	double errorRange = 1e-7;
//	std::vector<double> lastx;
//	for(unsigned i=0;i<vertices.size();i++){
//		lastx.push_back(TVecAll[i]);
//	}
//	
//	double _eps = 1-eps;			// 1-eps�̌v�Z�ɗ��p
//	bool DoCalc =true;											//���񂾂��萔�x�N�g��b�̌v�Z���s��bool		//NofCyc��0�̎��ɂ���΂����̂���
//	for(unsigned i=0; i < NofCyc; i++){							//�K�E�X�U�C�f���̌v�Z���[�v
//		if(DoCalc){												
//			if(deformed || alphaUpdated ){												//D_ii�̍쐬�@�`�󂪍X�V���ꂽ�ۂ�1�x�����s���΂悢
//				for(unsigned j =0; j < vertices.size() ; j++){
//					_dMatAll.resize(1,vertices.size());
//					_dMatAll[0][j] = 1.0/ ( eps * dMatKAll[0][j] + 1.0/dt * dMatCAll[0][j] );		//1 / D__ii	�����߂�
//					//DSTR << "_dMatAll[0][" << j << "] : " << _dMatAll[0][j]  << std::endl;
//					int debughogeshi =0;
//				}
//				deformed = false;
//			}
//			//	 1      1        1  
//			//	--- ( - - [K] + ---[C] ){T(t)} + {F} 
//			//	D_jj    2       ��t
//			//
//
//			for(unsigned j =0; j < vertices.size() ; j++){		//���񃋁[�v����	�W���x�N�g��bVecAll�̐������v�Z
//				bVecAll[j][0] = 0.0;							//bVecAll[j][0]�̏�����
//				//�ߓ_������edges���Ɂ@�Ίp����(j,j)�Ɣ�Ίp����(j,?)���Ɍv�Z
//				//�Ίp�����́Avertices[j].k or .c �ɓ����Ă���l���A��Ίp������edges[hoge].vertices[0] or vertices[1] .k or .c�ɓ����Ă���l��p����
//				//�@)��Ίp�����ɂ���
//				for(unsigned k =0;k < vertices[j].edges.size() ; k++){
//					unsigned edgeId = vertices[j].edges[k];
//					//���t�@�N�^�����O	�ȉ��̏�������ɂ���j>edges[edgeId].vertices[0] �Ƃ����łȂ����Ƃŕ������ق����R�ꂪ�o��S�z�͂Ȃ��H
//					if( j != edges[edgeId].vertices[0]){					//�ߓ_�ԍ�j��edges.vertices[0]���قȂ�ߓ_�ԍ��̎�:��Ίp����
//						unsigned vtxid0 = edges[edgeId].vertices[0];
//						bVecAll[j][0] += (-_eps * edges[edgeId].k + 1.0/dt * edges[edgeId].c ) * TVecAll[vtxid0];
//					}
//					else if( j != edges[edgeId].vertices[1] ){			//�ߓ_�ԍ�j��edges.vertices[1]���قȂ�ߓ_�ԍ��̎�:��Ίp����
//						unsigned vtxid1 = edges[edgeId].vertices[1];
//						bVecAll[j][0] += (-_eps * edges[edgeId].k + 1.0/dt * edges[edgeId].c ) * TVecAll[vtxid1];
//
//					}
//					else{
//						//��L�̂ǂ���ł��Ȃ��ꍇ�A�G���[
//						DSTR << "edges.vertex has 3 vertexies or any other problem" <<std::endl;
//					}
//				}
//				//�A)�Ίp�����ɂ���
//				bVecAll[j][0] += (-_eps * dMatKAll[0][j] + 1.0/dt * dMatCAll[0][j] ) * TVecAll[j];
//				//  {F}�����Z
//				bVecAll[j][0] += vecFAllSum[j];		//F�����Z
//				//D_ii�Ŋ��� �˂��̏ꏊ�́A�����ŗǂ��́H�ǂ��܂Ŋ|���Z����́H
//				bVecAll[j][0] = bVecAll[j][0] * _dMatAll[0][j];
//			}
//			DoCalc = false;			//����̃��[�v�����ŗ��p
//			int debughogeshi =0;
//		}		//if(DoCalc){...}
//
//#ifdef DEBUG
//		//	�O�̂��߁A�v�Z�O�̏������x��0�ɂ��Ă���B
//		if(i == 0){
//				for(unsigned j=0;j <vertices.size() ;j++){
//					TVecAll[j] = 0.0;
//				}
//		}
//#endif
//		//	 1      
//		//	--- [F]{T(t+dt)}
//		//	D_jj 		
//		//[F] = eps(ilon) [K] +1/dt [C] ����Ίp����������(-1)������������
//		//�G�b�W�ɓ����Ă��鐬����-1��������̂ł͂Ȃ��A�Ō��-1��������B
//		//
//		for(unsigned j =0; j < vertices.size() ; j++){
//			//T(t+dt) = �̎�
//			//	�܂�tempkj�����
//			double tempkj = 0.0;			//�K�E�X�U�C�f���̓r���v�Z�ŏo�Ă���F�̐����v�Z�Ɏg�p����ꎞ�ϐ�
//			for(unsigned k =0;k < vertices[j].edges.size() ; k++){
//				unsigned edgeId = vertices[j].edges[k]; 
//				if( j != edges[edgeId].vertices[0]){					//�ߓ_�ԍ�j��edges.vertices[0]���قȂ�ߓ_�ԍ��̎�:��Ίp����		//OK
//					unsigned vtxid0 = edges[edgeId].vertices[0];
//					//DSTR << "TVecAll["<< vtxid0<<"] : " << TVecAll[vtxid0] <<std::endl;
//					//TVecAll[j] +=_dMatAll[j][0] * -(1.0/2.0 * edges[edgeId].k + 1.0/dt * edges[edgeId].c ) * TVecAll[vtxid0] + bVecAll[j][0]; 
//					//DSTR << "j : " << j << ", vtxid0 : " << vtxid0 <<", edges[edgeId].vertices[0] : " << edges[edgeId].vertices[0] <<  std::endl;
//					tempkj += (eps * edges[edgeId].k + 1.0/dt * edges[edgeId].c ) * TVecAll[vtxid0];
//				}
//				else if( j != edges[edgeId].vertices[1] ){			//�ߓ_�ԍ�j��edges.vertices[1]���قȂ�ߓ_�ԍ��̎�:��Ίp����
//					unsigned vtxid1 = edges[edgeId].vertices[1];
//					//DSTR << "TVecAll["<< vtxid1<<"] : " << TVecAll[vtxid1] <<std::endl;
//					tempkj += (eps * edges[edgeId].k + 1.0/dt * edges[edgeId].c ) * TVecAll[vtxid1];
//				}
//				else{
//					//��L�̂ǂ���ł��Ȃ��ꍇ�A�G���[
//					DSTR << "edges.vertex has 3 vertexies or any other problem" <<std::endl;
//				}
//			}
//			TVecAll[j] =	_dMatAll[0][j] * ( -1.0 * tempkj) + bVecAll[j][0];			//	-b = D^(-1) [ (-1/2 * K + 1/dt * C ){T(t+dt)} + {F} ]�Ȃ̂ŁAbVecAll�͂����̉��Z�ł悢
//		}
//		int piyopiyoyo =0;
//	}
//}


void PHFemThermo::CalcHeatTransUsingGaussSeidel(unsigned NofCyc,double dt){
	PHFemMeshNew* mesh = GetPHFemMesh();

	//dt = 0.0000000000001 * dt;		//�f�o�b�O�p�ɁAdt�����̂��������������Ă��A�ߓ_0���}�C�i�X�ɂȂ�̂��A���ׂ�
	
	//dt��PHFemEngine.cpp�Ŏ擾���铮�͊w�V�~�����[�V�����̃X�e�b�v����
	bool DoCalc =true;											//���񂾂��萔�x�N�g��b�̌v�Z���s��bool		//NofCyc��0�̎��ɂ���΂����̂���
	//std::ofstream ofs("log.txt");
	for(unsigned i=0; i < NofCyc; i++){							//�K�E�X�U�C�f���̌v�Z���[�v
		if(DoCalc){		
			if(deformed || alphaUpdated){												//D_ii�̍쐬�@�`���M�`�B�����X�V���ꂽ�ۂ�1�x�����s���΂悢
				for(unsigned j =0; j < mesh->vertices.size() ; j++){
					//for(unsigned k =0;k < vertices.size(); k++){
					//	DSTR << "dMatCAll "<< k << " : " << dMatCAll[0][k] << std::endl;
					//}
					_dMatAll.resize(1,mesh->vertices.size());
					_dMatAll[0][j] = 1.0/ ( 1.0/2.0 * dMatKAll[0][j] + 1.0/dt * dMatCAll[0][j] );		//1 / D__ii	�����߂�
					//1.0/dt = 500 d
					//DSTR << "dMatKAll : "  << dMatKAll << std::endl;
					//DSTR << "dMatCAll : "  << dMatCAll << std::endl;
					//DSTR << "1.0/dt : " << 1.0/dt <<std::endl;
					//DSTR <<  1.0/dt *dMatCAll[0][j] << std::endl;		//0.001�̃I�[�_�[
					//DSTR << 1.0/2.0 * dMatKAll[0][j] << std::endl;		//0.0003�O��̃I�[�_�[
					//�l�������Ă��邩���`�F�b�N
					//DSTR << "_dMatAll[0][" << j << "] : " << _dMatAll[0][j]  << std::endl;
					int debughogeshi =0;
				}
				deformed = false;
				alphaUpdated =false;
				//	for DEBUG
				//DSTR << "_dMatAll : " << std::endl;
				//for(unsigned j =0; j < vertices.size() ;j++){
				//	DSTR << j << " : " << _dMatAll[0][j] << std::endl;
				//}
				//int hogeshi=0;
			}
			//	 1      1        1  
			//	--- ( - - [K] + ---[C] ){T(t)} + {F} 
			//	D_jj    2       ��t
			//

			for(unsigned j =0; j < mesh->vertices.size() ; j++){		//���񃋁[�v����	�W���x�N�g��bVecAll�̐������v�Z
				bVecAll[j][0] = 0.0;							//bVecAll[j][0]�̏�����
				bVecAll_IH[j][0] = 0.0;
				//�ߓ_������edges���Ɂ@�Ίp����(j,j)�Ɣ�Ίp����(j,?)���Ɍv�Z
				//�Ίp�����́Avertices[j].k or .c �ɓ����Ă���l���A��Ίp������edges[hoge].vertices[0] or vertices[1] .k or .c�ɓ����Ă���l��p����
				//�@)��Ίp�����ɂ���
				for(unsigned k =0;k < mesh->vertices[j].edgeIDs.size() ; k++){
					unsigned edgeId = mesh->vertices[j].edgeIDs[k];
					//���t�@�N�^�����O	�ȉ��̏�������ɂ���j>edges[edgeId].vertices[0] �Ƃ����łȂ����Ƃŕ������ق����R�ꂪ�o��S�z�͂Ȃ��H
					if( j != mesh->edges[edgeId].vertexIDs[0]){					//�ߓ_�ԍ�j��edges.vertices[0]���قȂ�ߓ_�ԍ��̎�:��Ίp����
						unsigned vtxid0 = mesh->edges[edgeId].vertexIDs[0];
						bVecAll[j][0] += (-1.0/2.0 * edgeVars[edgeId].k + 1.0/dt * edgeVars[edgeId].c ) * TVecAll[vtxid0];
					}
					else if( j != mesh->edges[edgeId].vertexIDs[1] ){			//�ߓ_�ԍ�j��edges.vertices[1]���قȂ�ߓ_�ԍ��̎�:��Ίp����
						unsigned vtxid1 = mesh->edges[edgeId].vertexIDs[1];
						bVecAll[j][0] += (-1.0/2.0 * edgeVars[edgeId].k + 1.0/dt * edgeVars[edgeId].c ) * TVecAll[vtxid1];

					}
					else{
						//��L�̂ǂ���ł��Ȃ��ꍇ�A�G���[
						DSTR << "edges.vertex has 3 vertexies or any other problem" <<std::endl;
					}
					//	for Debug
					//DSTR << "edges[" << edgeId << "].vertices[0] : " << edges[edgeId].vertices[0] << std::endl;
					//DSTR << "edges[" << edgeId << "].vertices[1] : " << edges[edgeId].vertices[1] << std::endl;
					//int hogeshi =0;
				}
				//�A)�Ίp�����ɂ���
				bVecAll[j][0] += (-1.0/2.0 * dMatKAll[0][j] + 1.0/dt * dMatCAll[0][j] ) * TVecAll[j];
				//ofs << "bVecAll[" << j <<"][0] : " << bVecAll[j][0] << std::endl;			// DSTR
				//{F}�����Z
				bVecAll[j][0] += vecFAllSum[j];		//F�����Z
				//DSTR << " vecFAllSum[" << j << "] : "  << vecFAllSum[j] << std::endl;
				//DSTR << std::endl;
				//D_ii�Ŋ��� �˂��̏ꏊ�́A�����ŗǂ��́H�ǂ��܂Ŋ|���Z����́H
				bVecAll[j][0] = bVecAll[j][0] * _dMatAll[0][j];
				//ofs << "bVecAll[" << j <<"][0] * _dMatAll : " << bVecAll[j][0] << std::endl;
				//	DSTR <<  "bVecAll[" << j <<"][0] * _dMatAll : " << bVecAll[j][0] << std::endl;
				//ofs << "TVecAll[" << j <<"] : " << TVecAll[j] << std::endl;
				//	DSTR << "TVecAll[" << j <<"] : " << TVecAll[j] << std::endl;
			}
			DoCalc = false;			//����̃��[�v�����ŗ��p
			//�l�������Ă��邩�A���킻�������`�F�b�N
			//DSTR << "bVecAll[j][0] : " << std::endl;
			//for(unsigned j =0;j <vertices.size() ; j++){
			//	DSTR << j << " : "<< bVecAll[j][0] << std::endl;
			//}
			int debughogeshi =0;
		}		//if(DoCalc){...}

#ifdef DEBUG
		//	�O�̂��߁A�v�Z�O�̏������x��0�ɂ��Ă���B
		if(i == 0){
				for(unsigned j=0;j <vertices.size() ;j++){
					TVecAll[j] = 0.0;
				}
		}
#endif
		//	 1      
		//	--- [F]{T(t+dt)}
		//	D_jj 		
		//[F] = 1/2 [K] +1/dt [C] ����Ίp����������(-1)������������
		//�G�b�W�ɓ����Ă��鐬����-1��������̂ł͂Ȃ��A�Ō��-1��������B
		//
		for(unsigned j =0; j < mesh->vertices.size() ; j++){
			//T(t+dt) = �̎�
			//	�܂�tempkj�����
			double tempkj = 0.0;			//�K�E�X�U�C�f���̓r���v�Z�ŏo�Ă���F�̐����v�Z�Ɏg�p����ꎞ�ϐ�
			for(unsigned k =0;k < mesh->vertices[j].edgeIDs.size() ; k++){
				unsigned edgeId = mesh->vertices[j].edgeIDs[k]; 
				if( j != mesh->edges[edgeId].vertexIDs[0]){					//�ߓ_�ԍ�j��edges.vertices[0]���قȂ�ߓ_�ԍ��̎�:��Ίp����		//OK
					unsigned vtxid0 = mesh->edges[edgeId].vertexIDs[0];
					//DSTR << "TVecAll["<< vtxid0<<"] : " << TVecAll[vtxid0] <<std::endl;
					//TVecAll[j] +=_dMatAll[j][0] * -(1.0/2.0 * edges[edgeId].k + 1.0/dt * edges[edgeId].c ) * TVecAll[vtxid0] + bVecAll[j][0]; 
					//DSTR << "j : " << j << ", vtxid0 : " << vtxid0 <<", edges[edgeId].vertices[0] : " << edges[edgeId].vertices[0] <<  std::endl;
					tempkj += (1.0/2.0 * edgeVars[edgeId].k + 1.0/dt * edgeVars[edgeId].c ) * TVecAll[vtxid0];
				}
				else if( j != mesh->edges[edgeId].vertexIDs[1] ){			//�ߓ_�ԍ�j��edges.vertices[1]���قȂ�ߓ_�ԍ��̎�:��Ίp����
					unsigned vtxid1 = mesh->edges[edgeId].vertexIDs[1];
					//DSTR << "TVecAll["<< vtxid1<<"] : " << TVecAll[vtxid1] <<std::endl;
					tempkj += (1.0/2.0 * edgeVars[edgeId].k + 1.0/dt * edgeVars[edgeId].c ) * TVecAll[vtxid1];
				}
				else{
					//��L�̂ǂ���ł��Ȃ��ꍇ�A�G���[
					DSTR << "edges.vertex has 3 vertexies or any other problem" <<std::endl;
				}
				//	for Debug
				//DSTR << "TVecAll:"
				//DSTR << "edges[" << edgeId << "].vertices[0] : " << edges[edgeId].vertices[0] << std::endl;
				//DSTR << "edges[" << edgeId << "].vertices[1] : " << edges[edgeId].vertices[1] << std::endl;
				//int hogeshi =0;
			}
			//	TVecAll�̌v�Z
			TVecAll[j] =	_dMatAll[0][j] * ( -1.0 * tempkj) + bVecAll[j][0];			//	-b = D^(-1) [ (-1/2 * K + 1/dt * C ){T(t+dt)} + {F} ]�Ȃ̂ŁAbVecAll�͂����̉��Z�ł悢

			////	for DEBUG
			//int hofgeshi =0;
			//if(TVecAll[j] != 0.0){
			//	DSTR << "!=0 TVecAll["<< j<<"] : " << TVecAll[j] <<std::endl;
			//}
			//DSTR << i << "��ڂ̌v�Z�A" << j <<"�s�ڂ�tempkj: " << tempkj << std::endl;
			//tempkj =0.0;

			//ofs << j << std::endl;
			//ofs << "tempkj: "<< tempkj << std::endl;
			//ofs << "DMatAll[0][j] * ( -1.0 * tempkj) :" <<_dMatAll[0][j] * ( -1.0 * tempkj) << std::endl;
			//ofs << "bVecAll[j][0] :  " <<  bVecAll[j][0] << std::endl;
			//ofs << "  TVecAll[j] : " << TVecAll[j] << std::endl;
			//ofs << std::endl;
		}
		////	for Debug
		//for(unsigned j=0;j < vertices.size();j++){
		//	//DSTR << "tempk" << j << " : " << tempkj << std::endl;
		//	int hogeshi__ =0;
		//	//TVecAll[j]�̌v�Z���ʂ�������
		//	//�萔�x�N�g��b����Ōv�Z�A���s��bVecAll�����Z����΂悢�B
		//	DSTR << i << "��ڂ̌v�Z�� " << "TVecAll[" << j << "] : " << TVecAll[j] << std::endl;
		//}
		//for(unsigned j=0;j < vertices.size();j++){
		//	//DSTR << "tempk" << j << " : " << tempkj << std::endl;
		//	int hogeshi__ =0;
		//	//TVecAll[j]�̌v�Z���ʂ�������
		//	//�萔�x�N�g��b����Ōv�Z�A���s��bVecAll�����Z����΂悢�B
		//	DSTR << i << "��ڂ̌v�Z�� " << "bVecAll[" << j << "][0] : " << bVecAll[j][0] << std::endl;
		//}

		//DSTR << i <<  "th Cyc" << std::endl; 
		//DSTR << i << "��ڂ̌v�Z�ATVecAll : " <<std::endl;
		//DSTR << TVecAll << std::endl;
		//ofs << i <<  "th Cyc" << std::endl;
		//ofs << i << "��ڂ̌v�Z�ATVecAll : " <<std::endl;
		//ofs << TVecAll << std::endl;
		//ofs << "bVecAll: " <<std::endl;
		//ofs << bVecAll << std::endl;
		//ofs << "_dMatAll: " <<std::endl; 
		//ofs << _dMatAll <<std::endl;
		int piyopiyoyo =0;
		//double tempTemp=0.0;
		//for(unsigned j=0;j <vertices.size() ; j++){
		//	tempTemp += TVecAll[j];
		//}
		//	DSTR
		//ofs << i <<"��ڂ̌v�Z���́@�S�ߓ_�̉��x�̘a : " << tempTemp << std::endl;
		//ofs << std::endl;
	}
//	deformed = true;
}

void PHFemThermo::CalcHeatTransUsingGaussSeidel(unsigned NofCyc,double dt,double eps){
	PHFemMeshNew* mesh = GetPHFemMesh();

	//for(unsigned i=0; i < vertices.size(); i++){
	//	FEMLOG << vertices[i].k <<","<< vertices[i].c << std::endl;
	//}
	//FEMLOG << vecFAllSum << std::endl;
	//dt = 0.0000000000001 * dt;		//�f�o�b�O�p�ɁAdt�����̂��������������Ă��A�ߓ_0���}�C�i�X�ɂȂ�̂��A���ׂ�
	double _eps = 1-eps;			// 1-eps�̌v�Z�ɗ��p
	//dt��PHFemEngine.cpp�Ŏ擾���铮�͊w�V�~�����[�V�����̃X�e�b�v����
	bool DoCalc =true;											//���񂾂��萔�x�N�g��b�̌v�Z���s��bool		//NofCyc��0�̎��ɂ���΂����̂���
	//FEMLOG( std::ofstream ofs("log.txt") ) ;
	for(unsigned i=0; i < NofCyc; i++){							//�K�E�X�U�C�f���̌v�Z���[�v
		convTest = TVecAll;
		if(DoCalc){												
			if(deformed || alphaUpdated ){												//D_ii�̍쐬�@�`�󂪍X�V���ꂽ�ۂ�1�x�����s���΂悢
				for(unsigned j =0; j < mesh->vertices.size() ; j++){
					//for(unsigned k =0;k < vertices.size(); k++){
					//	DSTR << "dMatCAll "<< k << " : " << dMatCAll[0][k] << std::endl;
					//}
					_dMatAll.resize(1,mesh->vertices.size());
					_dMatAll[0][j] = 1.0/ ( eps * dMatKAll[0][j] + 1.0/dt * dMatCAll[0][j] );		//1 / D__ii	�����߂�
					//1.0/dt = 500 d
					//DSTR << "dMatKAll : "  << dMatKAll << std::endl;
					//DSTR << "dMatCAll : "  << dMatCAll << std::endl;
					//DSTR << "1.0/dt : " << 1.0/dt <<std::endl;
					//DSTR <<  1.0/dt *dMatCAll[0][j] << std::endl;		//0.001�̃I�[�_�[
					//DSTR << 1.0/2.0 * dMatKAll[0][j] << std::endl;		//0.0003�O��̃I�[�_�[
					//�l�������Ă��邩���`�F�b�N
					//DSTR << "_dMatAll[0][" << j << "] : " << _dMatAll[0][j]  << std::endl;
					int debughogeshi =0;
				}
				deformed = false;
				//	for DEBUG
				//DSTR << "_dMatAll : " << std::endl;
				//for(unsigned j =0; j < vertices.size() ;j++){
				//	DSTR << j << " : " << _dMatAll[0][j] << std::endl;
				//}
				//int hogeshi=0;
			}
			//	 1      1        1  
			//	--- ( - - [K] + ---[C] ){T(t)} + {F} 
			//	D_jj    2       ��t
			//

			for(unsigned j =0; j < mesh->vertices.size() ; j++){		//���񃋁[�v����	�W���x�N�g��bVecAll�̐������v�Z
				bVecAll[j][0] = 0.0;							//bVecAll[j][0]�̏�����
				//�ߓ_������edges���Ɂ@�Ίp����(j,j)�Ɣ�Ίp����(j,?)���Ɍv�Z
				//�Ίp�����́Avertices[j].k or .c �ɓ����Ă���l���A��Ίp������edges[hoge].vertices[0] or vertices[1] .k or .c�ɓ����Ă���l��p����
				//�@)��Ίp�����ɂ���
				for(unsigned k =0;k < mesh->vertices[j].edgeIDs.size() ; k++){
					unsigned edgeId = mesh->vertices[j].edgeIDs[k];
					//���t�@�N�^�����O	�ȉ��̏�������ɂ���j>edges[edgeId].vertices[0] �Ƃ����łȂ����Ƃŕ������ق����R�ꂪ�o��S�z�͂Ȃ��H
					if( j != mesh->edges[edgeId].vertexIDs[0]){					//�ߓ_�ԍ�j��edges.vertices[0]���قȂ�ߓ_�ԍ��̎�:��Ίp����
						unsigned vtxid0 = mesh->edges[edgeId].vertexIDs[0];
						bVecAll[j][0] += (-_eps * edgeVars[edgeId].k + 1.0/dt * edgeVars[edgeId].c ) * TVecAll[vtxid0];
					}
					else if( j != mesh->edges[edgeId].vertexIDs[1] ){			//�ߓ_�ԍ�j��edges.vertices[1]���قȂ�ߓ_�ԍ��̎�:��Ίp����
						unsigned vtxid1 = mesh->edges[edgeId].vertexIDs[1];
						bVecAll[j][0] += (-_eps * edgeVars[edgeId].k + 1.0/dt * edgeVars[edgeId].c ) * TVecAll[vtxid1];

					}
					else{
						//��L�̂ǂ���ł��Ȃ��ꍇ�A�G���[
						DSTR << "edges.vertex has 3 vertexies or any other problem" <<std::endl;
					}
					//	for Debug
					//DSTR << "edges[" << edgeId << "].vertices[0] : " << edges[edgeId].vertices[0] << std::endl;
					//DSTR << "edges[" << edgeId << "].vertices[1] : " << edges[edgeId].vertices[1] << std::endl;
					//int hogeshi =0;
				}
				//�A)�Ίp�����ɂ���
				bVecAll[j][0] += (-_eps * dMatKAll[0][j] + 1.0/dt * dMatCAll[0][j] ) * TVecAll[j];

				//  {F}�����Z
				bVecAll[j][0] += vecFAllSum[j];		//F�����Z

				//D_ii�Ŋ��� �˂��̏ꏊ�́A�����ŗǂ��́H�ǂ��܂Ŋ|���Z����́H
				bVecAll[j][0] = bVecAll[j][0] * _dMatAll[0][j];
				//FEMLOG(ofs << "bVecAll[" << j <<"][0] * _dMatAll : " << bVecAll[j][0] << std::endl);
				//FEMLOG(ofs << "TVecAll[" << j <<"] : " << TVecAll[j] << std::endl);
			}
			DoCalc = false;			//����̃��[�v�����ŗ��p
			//�l�������Ă��邩�A���킻�������`�F�b�N
			//DSTR << "bVecAll[j][0] : " << std::endl;
			//for(unsigned j =0;j <vertices.size() ; j++){
			//	DSTR << j << " : "<< bVecAll[j][0] << std::endl;
			//}
			int debughogeshi =0;
		}		//if(DoCalc){...}

#ifdef DEBUG
		//	�O�̂��߁A�v�Z�O�̏������x��0�ɂ��Ă���B
		if(i == 0){
				for(unsigned j=0;j <vertices.size() ;j++){
					TVecAll[j] = 0.0;
				}
		}
#endif
		//	 1      
		//	--- [F]{T(t+dt)}
		//	D_jj 		
		//[F] = eps(ilon) [K] +1/dt [C] ����Ίp����������(-1)������������
		//�G�b�W�ɓ����Ă��鐬����-1��������̂ł͂Ȃ��A�Ō��-1��������B
		//
		for(unsigned j =0; j < mesh->vertices.size() ; j++){
			//T(t+dt) = �̎�
			//	�܂�tempkj�����
			double tempkj = 0.0;			//�K�E�X�U�C�f���̓r���v�Z�ŏo�Ă���F�̐����v�Z�Ɏg�p����ꎞ�ϐ�
			for(unsigned k =0;k < mesh->vertices[j].edgeIDs.size() ; k++){
				unsigned edgeId = mesh->vertices[j].edgeIDs[k]; 
				if( j != mesh->edges[edgeId].vertexIDs[0]){					//�ߓ_�ԍ�j��edges.vertices[0]���قȂ�ߓ_�ԍ��̎�:��Ίp����		//OK
					unsigned vtxid0 = mesh->edges[edgeId].vertexIDs[0];
					//DSTR << "TVecAll["<< vtxid0<<"] : " << TVecAll[vtxid0] <<std::endl;
					//TVecAll[j] +=_dMatAll[j][0] * -(1.0/2.0 * edges[edgeId].k + 1.0/dt * edges[edgeId].c ) * TVecAll[vtxid0] + bVecAll[j][0]; 
					//DSTR << "j : " << j << ", vtxid0 : " << vtxid0 <<", edges[edgeId].vertices[0] : " << edges[edgeId].vertices[0] <<  std::endl;
					tempkj += (eps * edgeVars[edgeId].k + 1.0/dt * edgeVars[edgeId].c ) * TVecAll[vtxid0];
				}
				else if( j != mesh->edges[edgeId].vertexIDs[1] ){			//�ߓ_�ԍ�j��edges.vertices[1]���قȂ�ߓ_�ԍ��̎�:��Ίp����
					unsigned vtxid1 = mesh->edges[edgeId].vertexIDs[1];
					//DSTR << "TVecAll["<< vtxid1<<"] : " << TVecAll[vtxid1] <<std::endl;
					tempkj += (eps * edgeVars[edgeId].k + 1.0/dt * edgeVars[edgeId].c ) * TVecAll[vtxid1];
				}
				else{
					//��L�̂ǂ���ł��Ȃ��ꍇ�A�G���[
					DSTR << "edges.vertex has 3 vertexies or any other problem" <<std::endl;
				}
				//	for Debug
				//DSTR << "TVecAll:"
				//DSTR << "edges[" << edgeId << "].vertices[0] : " << edges[edgeId].vertices[0] << std::endl;
				//DSTR << "edges[" << edgeId << "].vertices[1] : " << edges[edgeId].vertices[1] << std::endl;
				//int hogeshi =0;
			}
			//	TVecAll�̌v�Z
			TVecAll[j] =	_dMatAll[0][j] * ( -1.0 * tempkj) + bVecAll[j][0];			//	-b = D^(-1) [ (-1/2 * K + 1/dt * C ){T(t+dt)} + {F} ]�Ȃ̂ŁAbVecAll�͂����̉��Z�ł悢
			//TVecAll[j] =	_dMatAll[0][j] * ( -1.0 * tempkj) + bVecAll[j][0];   // -b = D^(-1) [ (-1/2 * K + 1/dt * C ){T(t+dt)} + {F} ]�Ȃ̂ŁAbVecAll�͂����̉��Z�ł悢
//			TVecAll[j] =	_dMatAll[0][j] * ( -1.0 * tempkj) + bVecAll[j][0];			//���̌v�Z�����ƁA�܂Ƃ������Ȓl���o�邪�E�E�E���_�I�ɂ͂ǂ��Ȃ̂��A������Ȃ��B�B�B
			////	for DEBUG
			//int hofgeshi =0;
			//if(TVecAll[j] != 0.0){
			//	DSTR << "!=0 TVecAll["<< j<<"] : " << TVecAll[j] <<std::endl;
			//}
			//DSTR << i << "��ڂ̌v�Z�A" << j <<"�s�ڂ�tempkj: " << tempkj << std::endl;
			//tempkj =0.0;

			//FEMLOG(ofs << j << std::endl);
			//FEMLOG(ofs << "tempkj: "<< tempkj << std::endl);
			//FEMLOG(ofs << "DMatAll[0][j] * ( -1.0 * tempkj) :" <<_dMatAll[0][j] * ( -1.0 * tempkj) << std::endl;)
			//FEMLOG(ofs << "bVecAll[j][0] :  " <<  bVecAll[j][0] << std::endl;)
			//FEMLOG(ofs << "  TVecAll[j] : " << TVecAll[j] << std::endl;)
			//FEMLOG(ofs << std::endl;)
		}
		////	for Debug
		//for(unsigned j=0;j < vertices.size();j++){
		//	//DSTR << "tempk" << j << " : " << tempkj << std::endl;
		//	int hogeshi__ =0;
		//	//TVecAll[j]�̌v�Z���ʂ�������
		//	//�萔�x�N�g��b����Ōv�Z�A���s��bVecAll�����Z����΂悢�B
		//	DSTR << i << "��ڂ̌v�Z�� " << "TVecAll[" << j << "] : " << TVecAll[j] << std::endl;
		//}
		//for(unsigned j=0;j < vertices.size();j++){
		//	//DSTR << "tempk" << j << " : " << tempkj << std::endl;
		//	int hogeshi__ =0;
		//	//TVecAll[j]�̌v�Z���ʂ�������
		//	//�萔�x�N�g��b����Ōv�Z�A���s��bVecAll�����Z����΂悢�B
		//	DSTR << i << "��ڂ̌v�Z�� " << "bVecAll[" << j << "][0] : " << bVecAll[j][0] << std::endl;
		//}

		//DSTR << i <<  "th Cyc" << std::endl; 
		//DSTR << i << "��ڂ̌v�Z�ATVecAll : " <<std::endl;
		//DSTR << TVecAll << std::endl;

		//FEMLOG(ofs << i <<  "th Cyc" << std::endl;)
		//FEMLOG(ofs << i << "��ڂ̌v�Z�ATVecAll : " <<std::endl;)
		//FEMLOG(ofs << TVecAll << std::endl;)
		//FEMLOG(ofs << "bVecAll: " <<std::endl;)
		//FEMLOG(ofs << bVecAll << std::endl;)
		//FEMLOG(ofs << "_dMatAll: " <<std::endl;) 
		//FEMLOG(ofs << _dMatAll <<std::endl;)
		int piyopiyoyo =0;
		//double tempTemp=0.0;
		//for(unsigned j=0;j <vertices.size() ; j++){
		//	tempTemp += TVecAll[j];
		//}
		//	DSTR
		//FEMLOG(ofs << i <<"��ڂ̌v�Z���́@�S�ߓ_�̉��x�̘a : " << tempTemp << std::endl;)
		//FEMLOG(ofs << std::endl;)
		if(strcmp(mesh->GetName(), "femNsteak") == 0){
			for(unsigned vtxid=0; vtxid < mesh->vertices.size(); vtxid++){
				ggConvOut << pow(TVecAll[vtxid]-convTest[vtxid],2) << ",";
			}
			ggConvOut << std::endl;
		}
	}
	if(strcmp(mesh->GetName(), "femNsteak") == 0){
		ggConvOut.close();
	}
//	deformed = true;
}

void PHFemThermo::UpdateVertexTempAll(){
	for(unsigned i=0;i < GetPHFemMesh()->vertices.size();i++){
		vertexVars[i].temp = TVecAll[i];
	}
}
void PHFemThermo::UpdateVertexTemp(unsigned vtxid){
		vertexVars[vtxid].temp = TVecAll[vtxid];
}

void PHFemThermo::Step(){
	Step(tdt/*10.0*/);
}

void PHFemThermo::Step(double dt){
	PHFemMeshNew* mesh = GetPHFemMesh();


	//// cps�\���p
	//static bool bOneSecond = false;
	//{
	//	static DWORD lastTick = GetTickCount();			//DWORD:unsigned long �̂���
	//	static int cpsCount = 0;
	//	int ellapsed = GetTickCount() - lastTick;
	//	++cpsCount;
	//	bOneSecond = false;
	//	if (ellapsed > 1000) {
	//		std::cout << "cps : " << cpsCount << std::endl;
	//		cpslog << StepCount<< "," << cpsCount << "," ;	
	//		lastTick = GetTickCount();
	//		cpsCount = 0;
	//		bOneSecond = true;
	//	}
	//	if(cpsCount){	cpstime	= 1 / cpsCount;		}
	//}
	//static DWORD stepStart = GetTickCount();
	////�r������
	//if (bOneSecond) {
	//	std::cout << "1: " << GetTickCount() - stepStart << std::endl;
	//	cpslog << GetTickCount() - stepStart << ",";
	//} 
	//stepStart = GetTickCount();
	//�r������
	//if (bOneSecond) { std::cout << "1: " << GetTickCount() - stepStart << std::endl; }
	// %%% CPS�\��

	//ScilabTest();									//	Scilab���g���e�X�g
	//���E������ݒ�:���x�̐ݒ�
	//UsingFixedTempBoundaryCondition(0,200.0);

	//	�����p�̃R�[�h
	//	a) ���ԗ��U�����ɑ�`�������p�A�O�i�E��ލ����ŐU��������M����
	//if(StepCount == 0){
	//	unsigned texid_ =7;
	//	UsingFixedTempBoundaryCondition(texid_,200.0);
	//}

	//	b) �f�M�ߒ��̎���
	//	�M�`�B����0�ɂ���B���x�Œ苫�E�����ŉ��M�B

	//	UsingFixedTempBoundaryCondition(3,50.0);
	//for(unsigned i=0 ;i<1;i++){
	//	UsingFixedTempBoundaryCondition(i,200.0);
	//}

	//%%%%		�M�`�B���E����		%%%%//
	//	�H�ރ��b�V���̕\�ʂ̐ߓ_�ɁA���̗͂��̉��x��^����
	//	���̗͂��̉��x�́A�t���C�p���̕\�ʉ��x��A�H�ނ�UsingFixedTempBoundaryCondition(0,200.0);�t�̓��̉��x�̕��z����A���̏ꏊ�ł̎��͗��̉��x�𔻕ʂ���B
	//	�ʒu���W���画�ʂ���R�[�h�������ɋL�q
	//UsingHeatTransferBoundaryCondition(unsigned id,double temp);
	//�G�l���M�[�ۑ������A���͗��̉��x�̒ቺ��A���̂ւ̋����M�ʂ͐��������ׂ�

	//dt = dt *0.01;		�덷1�x���x�ɂȂ�
	//dt = dt;				�����������́A�v�Z�덷�H�i�}�C�i�X�ɂȂ��Ă���ߓ_���x������Ȃ�ɑ傫���Ȃ�B�j
	
	///>	�K�E�X�U�C�f���@�̐ݒ�
	//	CalcHeatTransUsingGaussSeidel(20,dt);			//�K�E�X�U�C�f���@�ŔM�`���v�Z�������@�N�����N�j�R���\���@�݂̂��g�������ꍇ

//	dNdt = 10.0 * dt;

#if 0
	// �����O�ɂ��Ȃ炸�s��
	UpdateVecFAll_frypan(WEEK);				// �����ɉ��M������^����B(OFF/WEEK/MIDDLE/HIGH)
#endif

	//{F}���g�킸�ɁA�M�`���v�Z���������邽�߂̃e�X�g
#if 1
	//checkTVecAllout << COUNT * dt << ", "; 
	//for(unsigned i=0;i<mesh->vertices.size();i++){
	//	if(i != mesh->vertices.size() -1){
	//		checkTVecAllout << TVecAll[i] <<", ";
	//	}else{
	//		checkTVecAllout <<  TVecAll[i] << std::endl;
	//	}

	//}
	COUNT +=1;

	if(COUNT * dt == 1 * dt){
		if(tempe.size()){
			DSTR << "�������x�\���F" << std::endl;
			for(unsigned i=0;i<tempe.size();i++){
				Vec3d hyoukaPos =Vec3d(i*0.01, 0.0015, -0.005);
				DSTR << i << ":" << GetVtxTempInTets(hyoukaPos) << std::endl;
			}
		}
	}
#ifdef HYOKA
	//	�]�������p
	if(COUNT * dt >= stopTime -0.02  ){
		checkTVecAllout.close();
		DSTR << "STOP:" <<COUNT * dt<<"Sec passed" << std::endl;
		std::cout << "stopTime are set as" << stopTime <<"[sec]" << std::endl;
		//	LMSR�����߂�
		double LMS[10] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
		std::vector<double> hyouka;
		hyouka.clear();
		//%%%		���M
		//200
		//hyouka.push_back(207.7);
		//hyouka.push_back(207.5);
		//hyouka.push_back(206.3);
		//hyouka.push_back(203.4);
		//hyouka.push_back(199.4);
		//hyouka.push_back(193.2);
		//hyouka.push_back(184.7);
		//hyouka.push_back(174.6);
		//hyouka.push_back(166.1);
		//hyouka.push_back(155.5);

		//150
		//hyouka.push_back(156.0);
		//hyouka.push_back(155.7);
		//hyouka.push_back(154.2);
		//hyouka.push_back(151.4);
		//hyouka.push_back(148.1);
		//hyouka.push_back(143.7);
		//hyouka.push_back(138.3);
		//hyouka.push_back(132.9);
		//hyouka.push_back(129.2);
		//hyouka.push_back(124.4);
		
		//100
		//hyouka.push_back(99.1);
		//hyouka.push_back(99.1);
		//hyouka.push_back(98.4);
		//hyouka.push_back(97.2);
		//hyouka.push_back(95.9);
		//hyouka.push_back(94.1);
		//hyouka.push_back(91.9);
		//hyouka.push_back(89.9);
		//hyouka.push_back(88.6);
		//hyouka.push_back(86.8);

		//	50.0 10sec
		//hyouka.push_back(49.9);
		//hyouka.push_back(49.9);
		//hyouka.push_back(49.8);
		//hyouka.push_back(49.7);
		//hyouka.push_back(49.5);

		//hyouka.push_back(49.1);
		//hyouka.push_back(48.6);
		//hyouka.push_back(48.3);
		//hyouka.push_back(48.3);
		//hyouka.push_back(47.9);

		//	50.0	�X�V�^�C�~���O 10sec
		//hyouka.push_back(49.9);
		//hyouka.push_back(49.8);
		//hyouka.push_back(49.7);
		//hyouka.push_back(49.6);
		//hyouka.push_back(49.5);

		//hyouka.push_back(49.1);
		//hyouka.push_back(48.6);
		//hyouka.push_back(48.3);
		//hyouka.push_back(48.2);
		//hyouka.push_back(47.8);

		//	50�� 60sec
		//hyouka.push_back(49.3);
		//hyouka.push_back(49.3);
		//hyouka.push_back(49.3);
		//hyouka.push_back(49.1);
		//hyouka.push_back(48.9);

		//hyouka.push_back(48.6);
		//hyouka.push_back(48.1);
		//hyouka.push_back(47.9);
		//hyouka.push_back(47.8);
		//hyouka.push_back(47.5);

		//%%%	���M����@�ŏ�����10sec
		//hyouka.push_back(31.1);
		//hyouka.push_back(31.6);
		//hyouka.push_back(33.9);
		//hyouka.push_back(37.2);
		//hyouka.push_back(40.6);

		//hyouka.push_back(43.4);
		//hyouka.push_back(44.9);
		//hyouka.push_back(43.9);
		//hyouka.push_back(40.1);
		//hyouka.push_back(34.6);

		//%%%	���M����@�ŏ�����60sec
		hyouka.push_back(59.6);
		hyouka.push_back(61.6);
		hyouka.push_back(68.7);
		hyouka.push_back(79.3);
		hyouka.push_back(90.0);

		hyouka.push_back(97.5);
		hyouka.push_back(99.1);
		hyouka.push_back(92.0);
		hyouka.push_back(79.6);
		hyouka.push_back(62.1);

		//%%%	j�̊O�����Ⴂ���x�ɐݒ肵�Ă���̂ŁAj�̒l�͐M�p�o���Ȃ��Bi���M�p�o���Ȃ��Ƃ��āA-2
		for(unsigned i=0;i<tempe.size() - 9; i++){
			Vec3d hyoukaPos =Vec3d(i*0.01, 0.0015, -0.001);
			LMS[0] += (GetVtxTempInTets(hyoukaPos) - hyouka[i] ) * (GetVtxTempInTets(hyoukaPos) - hyouka[i] ); 
		}
		for(unsigned i=0;i<tempe.size() - 8; i++){
			Vec3d hyoukaPos =Vec3d(i*0.01, 0.0015, -0.001);
			LMS[1] += (GetVtxTempInTets(hyoukaPos) - hyouka[i] ) * (GetVtxTempInTets(hyoukaPos) - hyouka[i] ); 
		}
		for(unsigned i=0;i<tempe.size() - 7; i++){
			Vec3d hyoukaPos =Vec3d(i*0.01, 0.0015, -0.001);
			LMS[2] += (GetVtxTempInTets(hyoukaPos) - hyouka[i] ) * (GetVtxTempInTets(hyoukaPos) - hyouka[i] ); 
		}
		for(unsigned i=0;i<tempe.size() - 6; i++){
			Vec3d hyoukaPos =Vec3d(i*0.01, 0.0015, -0.001);
			LMS[3] += (GetVtxTempInTets(hyoukaPos) - hyouka[i] ) * (GetVtxTempInTets(hyoukaPos) - hyouka[i] ); 
		}
		for(unsigned i=0;i<tempe.size() - 5; i++){
			Vec3d hyoukaPos =Vec3d(i*0.01, 0.0015, -0.001);
			LMS[4] += (GetVtxTempInTets(hyoukaPos) - hyouka[i] ) * (GetVtxTempInTets(hyoukaPos) - hyouka[i] ); 
		}
		for(unsigned i=0;i<tempe.size() - 4; i++){
			Vec3d hyoukaPos =Vec3d(i*0.01, 0.0015, -0.001);
			LMS[5] += (GetVtxTempInTets(hyoukaPos) - hyouka[i] ) * (GetVtxTempInTets(hyoukaPos) - hyouka[i] ); 
		}
		for(unsigned i=0;i<tempe.size() - 3; i++){
			Vec3d hyoukaPos =Vec3d(i*0.01, 0.0015, -0.001);
			LMS[6] += (GetVtxTempInTets(hyoukaPos) - hyouka[i] ) * (GetVtxTempInTets(hyoukaPos) - hyouka[i] ); 
		}
		for(unsigned i=0;i<tempe.size() - 2; i++){
			Vec3d hyoukaPos =Vec3d(i*0.01, 0.0015, -0.001);
			LMS[7] += (GetVtxTempInTets(hyoukaPos) - hyouka[i] ) * (GetVtxTempInTets(hyoukaPos) - hyouka[i] ); 
		}
		for(unsigned i=0;i<tempe.size() - 1; i++){
			Vec3d hyoukaPos =Vec3d(i*0.01, 0.0015, -0.001);
			LMS[8] += (GetVtxTempInTets(hyoukaPos) - hyouka[i] ) * (GetVtxTempInTets(hyoukaPos) - hyouka[i] ); 
		}
		DSTR << "�����ݒ艷�x" << std::endl;
		for(unsigned i=0; i < tempe.size(); ++i){
			DSTR << tempe[i] << ",";
		}
		DSTR << std::endl;
		DSTR << "�ڕW���x" << std::endl;
		for(unsigned i=0; i < hyouka.size(); ++i){
			DSTR << hyouka[i] << ",";
		}
		DSTR << std::endl;
		DSTR << "����_�̉��x," << std::endl;
		for(unsigned i=0;i<tempe.size(); i++){
			Vec3d hyoukaPos =Vec3d(i*0.01, 0.0015, -0.001);
			LMS[9] += (GetVtxTempInTets(hyoukaPos) - hyouka[i] ) * (GetVtxTempInTets(hyoukaPos) - hyouka[i] ); 
			DSTR << GetVtxTempInTets(hyoukaPos) << ", ";
		}
		DSTR << std::endl;
		DSTR << "�M���˗��@���W��,�ؕ�" << "Tout," <<  "j�ȉ����x," << std::endl;
		DSTR << ems <<"," << ems_const <<"," << vertices[0].Tout <<"," << jout << std::endl;
		DSTR << vertices[0].Tc <<"," << vertices[0].Tout <<"," << vertices[0].heatTransRatio << "," << vertices[0].thermalEmissivity << std::endl;
		DSTR << "���M�o��," << "�����a," <<  "�O���a," << std::endl;
		DSTR << weekPow_ <<  "," << inr_ << "," << outR_ << std::endl;
		DSTR << "�ǉ��o��," << "�����a," <<  "�O���a," << std::endl;
		DSTR << weekPow_add <<  "," << inr_add << "," << outR_add << std::endl;
		DSTR << "�o�͑ł�����," << "�����a," <<  "�O���a," << std::endl;
		DSTR << weekPow_decr <<  "," << inr_decr << "," << outR_decr << std::endl;

		for(unsigned i=0;i<10;++i){
			DSTR << "LMS" << i+1 <<":"  << LMS[i] <<std::endl;
		}
		DSTR << "LMS������" <<std::endl;
		for(unsigned i=0;i<10;++i){
			DSTR << LMS[i] <<",";
		}
		DSTR << std::endl;
		DSTR << "�ڕW�l�Ƃ̍���" << std::endl;
		for(unsigned i=0;i<tempe.size(); i++){
			Vec3d hyoukaPos =Vec3d(i*0.01, 0.0015, -0.001);
			DSTR << hyouka[i] - GetVtxTempInTets(hyoukaPos) << ", ";
		}
		DSTR << std::endl;
		int katoonLMS =0;
		DSTR<<"------------------"<<std::endl;

	}
#endif HYOKA
	if(COUNT * dt >= stopTime + 0.1){
		//assert(0);			//	�~�߂���

	}
#endif
	doCalc =true;
	
	if(doCalc){
	//�K�E�X�U�C�f���@�ŉ���
		if(strcmp(mesh->GetName(), "femNsteak") == 0){
			
			//vecFOutToFile();
			//vecFTopOutToFile();
			//vecFBottomOutToFile();

			CalcHeatTransUsingGaussSeidel(NofCyc,dt,epsilonG);			//�K�E�X�U�C�f���@�ŔM�`���v�Z������ ��O�����́A�O�i�E�N�����N�j�R���\���E��ސϕ��̂����ꂩ�𐔒l�őI��
			//CalcHeatTransDirect(dt);
			//CalcHeatTransDirect3(dt);
			//CalcHeatTransDirect4(dt);
			//CalcHeatTransDirect2(dt);
			//CalcHeatTransUsingScilab(dt, epsilonG);

		}else{
			CalcHeatTransUsingGaussSeidel(NofCyc,dt,epsilonG);			//�K�E�X�U�C�f���@�ŔM�`���v�Z������ ��O�����́A�O�i�E�N�����N�j�R���\���E��ސϕ��̂����ꂩ�𐔒l�őI��
			//CalcHeatTransDirect(dt);
			//CalcHeatTransDirect2(dt);
			//CalcHeatTransUsingScilab(dt, epsilonG);
		}
	}
	doCalc = false;

	if(mesh->GetPHFemPorousWOMove()){
		for(unsigned vtxid=0; vtxid < mesh->surfaceVertices.size(); vtxid++){
			VertexWaterBoiling(vtxid);
		}
	}
//	if(strcmp(mesh->GetName(), "femPan") == 0){
//		for(unsigned vtxid=0; vtxid < mesh->vertices.size(); vtxid++){
//			vecFOuPant << vecFAllSum[vtxid] << ",";
//		}
//		vecFOut << std::endl;
//	}
//	if(strcmp(mesh->GetName(), "femNsteak") == 0){
//		vecFOutSteak << COUNT * dt << ",";
//		for(unsigned vtxid=0; vtxid < mesh->vertices.size(); vtxid++){
//			vecFOutSteak << vecFAllSum[vtxid] << ",";
//		}
//		vecFOutSteak << std::endl;
//	}
	//���x�̃x�N�g������ߓ_�։��x�̔��f
	UpdateVertexTempAll();
	StepCount += 1;
	if(StepCount >= 1000*1000*1000){
		StepCount = 0;
		StepCount_ += 1;
	}

	/*���쌴�ǉ���������--------------------------------------------*/
	//beCondVtxs��toofar��False������Ă����B������
	for(unsigned i=0; i<mesh->surfaceVertices.size(); i++){
		vertexVars[mesh->surfaceVertices[i]].beCondVtxs = vertexVars[mesh->surfaceVertices[i]].toofar = false;
	}
	/*���쌴�ǉ������܂�--------------------------------------------*/

//#ifdef UseMatAll
//	if(strcmp(mesh->GetName(), "femNsteak") == 0){
//		//for(unsigned vtxid=0; vtxid < mesh->vertices.size(); vtxid++){
//		//	vecFOutSteak << vecFAllSum[vtxid] << ",";
//		//}
//		//vecFOutSteak << std::endl;
//		std::ofstream Kout;
//		std::ofstream Cout;
//		Kout.open("matKMeatSingleTet2.txt");
//		Cout.open("matCMeatSingleTet2.txt");
//
//		//dtKout << "[";
//		//Cout << "[";
//		for(unsigned i=0; i < mesh->vertices.size(); i++){
//			for(unsigned j=0; j < mesh->vertices.size(); j++){
//				Kout << /*tdt * */matKAll[i][j] << " ";
//				Cout << matCAll[i][j] << " ";
//			}
//			Kout/* << ";" */<< std::endl;
//			Cout/* << ";" */<< std::endl;
//		}
//		//dtKout << "]";
//		//Cout <<"]"; 
//
//		Kout.close();
//		Cout.close();
//	}
//#endif

//#ifdef UseMatAll
//	if(strcmp(mesh->GetName(), "femNsteak") == 0){
//		OutEig();
//	}
//#endif
}



//void PHFemThermo::CreateMatrix(){
//}

void PHFemThermo::InitTcAll(double temp){
	for(unsigned i =0; i < GetPHFemMesh()->vertices.size();i++){
		vertexVars[i].Tc = temp;
	}
}

void PHFemThermo::InitToutAll(double temp){
	for(unsigned i =0; i < GetPHFemMesh()->vertices.size();i++){
		vertexVars[i].Tout = temp;
		//DSTR << vertices[i].Tout << temp <<std::endl;
	}
}

void PHFemThermo::InitCreateMatC(){
	/// MatC�ɂ���	//	�g�p����s��̏�����
	//dMatCAll�F�Ίp�s��̐����̓������s��̃T�C�Y���`:�z��Ƃ��ė��p	��:vertices.size(),����:1
	dMatCAll.resize(1,GetPHFemMesh()->vertices.size()); //(h,w)
	dMatCAll.clear();								///	�l�̏�����
	//matc�̏������́Amatc�����֐��ł���Ă���̂ŁA�ȗ�
	matCAll.resize(GetPHFemMesh()->vertices.size(),GetPHFemMesh()->vertices.size());
	matCAll.clear();
}

void PHFemThermo::InitCreateMatk_(){
	///	MatK�ɂ���
	//matk�̏�����
	for(unsigned i =0; i < 4 ; i++){
		for(unsigned j =0; j < 4 ; j++){
			matk[i][j] = 0.0;
		}
	}
	dMatKAll.clear();		///	������
#ifdef UseMatAll
	//matKAll.resize(vertices.size(),vertices.size());	///	matKAll�̃T�C�Y�̑��
	matKAll.clear();									///	matKAll�̏�����
	//DSTR << "matKAll: " << matKAll <<std::endl;
#endif UseMatAll

#ifdef DumK
	//matKAll�̏�����
	matKAll.resize(vertices.size(),vertices.size());
	matkAll.clear();					///	�������A���̏������R�[�h�͕s�v
#endif


}

void PHFemThermo::InitCreateMatk(){
	PHFemMeshNew* mesh = GetPHFemMesh();

	///	MatK�ɂ���
	//matk�̏�����
	for(unsigned i =0; i < 4 ; i++){
		for(unsigned j =0; j < 4 ; j++){
			matk[i][j] = 0.0;
		}
	}
	//for(unsigned i=0;i<3;i++){
	//tets.matk[i]
	//}
	//DMatAll�F�Ίp�s��̐����̓������s��̃T�C�Y���`:�z��Ƃ��ė��p	��:vertices.size(),����:1
	dMatKAll.resize(1,mesh->vertices.size()); //(h,w)
	dMatKAll.clear();		///	������
	////�l�̏�����
	//for(unsigned i=0;i<vertices.size();i++){
	//	dMatKAll[0][i] = 0.0;
	//}
#ifdef UseMatAll
	
	matKAll.resize(mesh->vertices.size(),mesh->vertices.size());	///	matKAll�̃T�C�Y�̑��
	matKAll.clear();									///	matKAll�̏�����
	//for(unsigned i=0;i<vertices.size();i++){
	//	for(unsigned j=0;j<vertices.size();j++){
	//		matKAll[i][j] = 0.0;
	//	}
	//}
#endif UseMatAll

#ifdef DumK
	//matKAll�̏�����
	matKAll.resize(vertices.size(),vertices.size());
	matkAll.clear();					///	�������A���̏������R�[�h�͕s�v
	//for(unsigned i=0;i<vertices.size();i++){
	//	for(unsigned j=0;j<vertices.size();j++){
	//		matKAll[i][j] = 0.0;
	//	}
	//}
#endif
}

void PHFemThermo::InitCreateVecf_(){
	///	Vecf�ɂ���
	//Vecf�̏�����
	for(unsigned i =0; i < 4 ; i++){
			vecf[i] = 0.0;
	}
	vecFAllSum.clear();						///	������
}

void PHFemThermo::InitVecFAlls(){
	PHFemMeshNew* mesh = GetPHFemMesh();

	for(unsigned i =0; i < 4 ; i++){ vecf[i] = 0.0;}	/// Vecf�̏�����
	vecFAllSum.resize(mesh->vertices.size());					///	�S�̍����x�N�g��F�̃T�C�Y���K��
	vecFAllSum.clear();									///		�V			�̏�����

	for(unsigned i=0;i < HIGH +1 ; i++){				/// IH���M���[�h�̊e�x�N�g����������
		vecFAll_f2IH[i].resize(mesh->vertices.size(),1);
		vecFAll_f2IH[i].clear();
	}
	// tets.vecf[4]�̏�����
	for(unsigned i=0;i<mesh->tets.size();i++){
		for(unsigned j=0; j <4;j++){
			tetVars[i].vecf[j].clear();
		}
	}
	// vecFAll�̏�����
	//initialize
	for(unsigned i =0;i<4;i++){
		vecFAll[i].resize(mesh->vertices.size());
		vecFAll[i].clear();
	}
}

void PHFemThermo::UpdateVecF_frypan(){
	PHFemMeshNew* mesh = GetPHFemMesh();

	// mode�͕K�v���H

	//	Initialize
	//InitVecFAlls();		// ���̒��̏������̂��ׂĂ��K�v���H

//. 1)�ŏ���2��({F2},{F3})�́AF2,F3�̂ǂ��炩�����X�V����Ηǂ��ꍇ�ɗp����
#if 0
	// {F2}
	for(unsigned tetsid = 0; tetsid < mesh->tets.size();tetsid++){
		unsigned id = tetsid;
		CreateVecf2surface(id);			// tets[id].vecf[1];�Ɍ��ʊi�[
		for(unsigned j =0;j < 4; j++){
			int vtxid0 = mesh->tets[id].vertexIDs[j];
			//vecFAll[1][vtxid0] += vecf[j];
			vecFAll[1][vtxid0] += tetVars[id].vecf[1][j];
		}
	}	
#endif

#if 0
	// {F3}
	for(unsigned tetsid = 0; tetsid < mesh->tets.size();tetsid++){
		unsigned id = tetsid;
		CreateVecf3(id);		// tets[id].vecf[2];�Ɍ��ʊi�[
		//vecf = tets[id].vecf[2];
		for(unsigned j =0;j < 4; j++){
			int vtxid0 = mesh->tets[id].vertexIDs[j];
			//vecFAll[1][vtxid0] += vecf[j];
			vecFAll[2][vtxid0] += tetVars[id].vecf[2][j];
		}
	}	
#endif

//. 2) {F2,F3}�̗������X�V���ėǂ��ꍇ
#if 1
	for(unsigned tetsid = 0; tetsid < mesh->tets.size();tetsid++){
		unsigned id = tetsid;
		CreateVecf2surface(id);
		CreateVecf4surface(id);
#if 0
		CreateVecf3surface(id);
#endif
#ifndef NOTUSE_HEATTRANS_HERE
		CreateVecf3(id);		// tets[id].vecf[2];�Ɍ��ʊi�[
#endif
		//vecf = tets[id].vecf[2];
		for(unsigned j =0;j < 4; j++){
			int vtxid0 = mesh->tets[id].vertexIDs[j];
			//vecFAll[1][vtxid0] += vecf[j];
			vecFAll[1][vtxid0] += tetVars[id].vecf[1][j];
			vecFAll[3][vtxid0] -= tetVars[id].vecf[3][j];
			//DSTR << "vtxid0: " << vtxid0 <<", tets[id].vecf[1]["<< j << "]: " << tets[id].vecf[1][j] << std::endl; 
#ifndef NOTUSE_HEATTRANS_HERE
			vecFAll[2][vtxid0] += tets[id].vecf[2][j];
#endif
		}
	}
#endif

	//��{F[i]}_{i=1}^{4}
#ifndef NOTUSE_HEATTRANS_HERE
	vecFAllSum = vecFAll[1] + vecFAll[2];
#else

	vecFAllSum += vecFAll[1] + vecFAll[3];

	// VecFAll ��weekpow�œ��ꂽ���M�ʂƍ��v���邩�`�F�b�N�R�[�h
	//double tempSumVECF=0.0;
	//for(unsigned i=0;i<vertices.size();i++){
	//	tempSumVECF += vecFAllSum[i];
	//}
	//DSTR <<"vecFAllSum �� Sum: " << tempSumVECF << std::endl;

#endif
	//DSTR << "vecFAll[1]: " << vecFAll[1] << std::endl;


//%%%% ���̊֐��͂����܂łłƂ肠�������� 2012.10.09

	//	�����\��
#if 0
//depend on mode, I don't need to use mode state.Because mode state cause different calc result of heatflus.
// I just use the result of IHdqdt Function.
	//vecFAll[1],[2]�ɑ��
	for(unsigned id = 0; id < mesh->vertices.size();id++){
		if(mode == OFF){ 
			// F2�͉��Z���Ȃ�
			vecFAllSum[id] = vecFAllSum[id]; //F3
		}
		else if(mode == WEEK){
			vecFAllSum[id] =  vecFAll_f2IH[mode][id][0] + vecFAll_f3[id][0];//F2+F3		//mode=0 -> F2��WEEK�̋���
		}
		else if(mode == MIDDLE){
			vecFAllSum[id] = vecFAll_f2IH[mode][id][0];//F2+F3		//mode=1 -> F2��middle�̋���
		}
		else if(mode == HIGH){
			vecFAllSum[id] = vecFAll_f2IH[mode][id][0];//F2+F3		//mode=2 -> F2��high�̋���
		}
	}

#endif

#if 0
	for(unsigned i =0; i< 4;i++){
		vecFAllSum += vecFAll[i];				//�S�̍����s��̘a�����
	}
	// F2,F3�����Z����
#endif
}


void PHFemThermo::UpdateVecF(){
	PHFemMeshNew* mesh = GetPHFemMesh();

	// ������enum mode�����ă��[�h�w����s���d�l�ɕς���


//. 1)�ŏ���2��({F2},{F3})�́AF2,F3�̂ǂ��炩�����X�V����Ηǂ��ꍇ�ɗp����
#if 0
	// {F2}
	for(unsigned tetsid = 0; tetsid < mesh->tets.size();tetsid++){
		unsigned id = tetsid;
		CreateVecf2surface(id);			// tets[id].vecf[1];�Ɍ��ʊi�[
		for(unsigned j =0;j < 4; j++){
			int vtxid0 = mesh->tets[id].vertexIDs[j];
			//vecFAll[1][vtxid0] += vecf[j];
			vecFAll[1][vtxid0] += tetVars[id].vecf[1][j];
		}
	}	
#endif

#if 0
	// {F3}
	for(unsigned tetsid = 0; tetsid < mesh->tets.size();tetsid++){
		unsigned id = tetsid;
		CreateVecf3(id);		// tets[id].vecf[2];�Ɍ��ʊi�[
		//vecf = tets[id].vecf[2];
		for(unsigned j =0;j < 4; j++){
			int vtxid0 = mesh->tets[id].vertexIDs[j];
			//vecFAll[1][vtxid0] += vecf[j];
			vecFAll[2][vtxid0] += tetVars[id].vecf[2][j];
		}
	}	
#endif

//. 2) {F2,F3}�̗������X�V���ėǂ��ꍇ
#if 1
	for(unsigned tetsid = 0; tetsid < mesh->tets.size();tetsid++){
		unsigned id = tetsid;
		CreateVecf2surface(id);
		CreateVecf4surface(id);
#if 0
		CreateVecf3surface(id);
#endif
#ifndef NOTUSE_HEATTRANS_HERE
		CreateVecf3(id);		// tets[id].vecf[2];�Ɍ��ʊi�[
#endif
		//vecf = tets[id].vecf[2];
		for(unsigned j =0;j < 4; j++){
			int vtxid0 = mesh->tets[id].vertexIDs[j];
			//vecFAll[1][vtxid0] += vecf[j];
			vecFAll[1][vtxid0] += tetVars[id].vecf[1][j];
			vecFAll[3][vtxid0] -= tetVars[id].vecf[3][j];
			//DSTR << "vtxid0: " << vtxid0 <<", tets[id].vecf[1]["<< j << "]: " << tets[id].vecf[1][j] << std::endl; 
#ifndef NOTUSE_HEATTRANS_HERE
			vecFAll[2][vtxid0] += tetVars[id].vecf[2][j];
#endif
		}
	}
#endif

	//��{F[i]}_{i=1}^{4}
#ifndef NOTUSE_HEATTRANS_HERE
	vecFAllSum = vecFAll[1] + vecFAll[2];
#else

	vecFAllSum += vecFAll[1] + vecFAll[3];

	// VecFAll ��weekpow�œ��ꂽ���M�ʂƍ��v���邩�`�F�b�N�R�[�h
	//double tempSumVECF=0.0;
	//for(unsigned i=0;i<vertices.size();i++){
	//	tempSumVECF += vecFAllSum[i];
	//}
	//DSTR <<"vecFAllSum �� Sum: " << tempSumVECF << std::endl;

#endif
	//DSTR << "vecFAll[1]: " << vecFAll[1] << std::endl;


//%%%% ���̊֐��͂����܂łłƂ肠�������� 2012.10.09

	//	�����\��
#if 0
//depend on mode, I don't need to use mode state.Because mode state cause different calc result of heatflus.
// I just use the result of IHdqdt Function.
	//vecFAll[1],[2]�ɑ��
	for(unsigned id = 0; id < vertices.size();id++){
		if(mode == OFF){ 
			// F2�͉��Z���Ȃ�
			vecFAllSum[id] = vecFAllSum[id]; //F3
		}
		else if(mode == WEEK){
			vecFAllSum[id] =  vecFAll_f2IH[mode][id][0] + vecFAll_f3[id][0];//F2+F3		//mode=0 -> F2��WEEK�̋���
		}
		else if(mode == MIDDLE){
			vecFAllSum[id] = vecFAll_f2IH[mode][id][0];//F2+F3		//mode=1 -> F2��middle�̋���
		}
		else if(mode == HIGH){
			vecFAllSum[id] = vecFAll_f2IH[mode][id][0];//F2+F3		//mode=2 -> F2��high�̋���
		}
	}

#endif

#if 0
	for(unsigned i =0; i< 4;i++){
		vecFAllSum += vecFAll[i];				//�S�̍����s��̘a�����
	}
	// F2,F3�����Z����
#endif
}

//void PHFemThermo::UpdateMatK(){
//
//	//%%%	��������		%%%//
//	//�e�탁���o�ϐ��̏������˃R���X�g���N�^�łł����ق���������������Ȃ��B
//	///	Edges
//	for(unsigned i =0; i < edges.size();i++){
//		edges[i].c = 0.0;	
//		edges[i].k = 0.0;
//	}
//
//	///	faces
//	for(unsigned i=0;i<faces.size();i++){
//		faces[i].alphaUpdated = true;
//		faces[i].area = 0.0;
//		faces[i].heatTransRatio = 0.0;
//		faces[i].deformed = true;				//������Ԃ́A�ό`��Ƃ���
//		faces[i].fluxarea =0.0;
//		faces[i].thermalEmissivity =0.0;
//		//faces[i].heatflux.clear();				// ������
//		//faces[i].heatflux[hum]�̗̈�m�ہF�z��Ƃ��āA���Avector�Ƃ��Ă�push_back���A�ǂ��炩���s���B�z��Ȃ炱���ɋL�q�B
//		for(unsigned mode =0; mode < HIGH +1 ; mode++){			// ���M���[�h�̐������A�x�N�g���𐶐�
//			faces[i].heatflux[mode] = 0.0;
//		}
//	}
//
//	//�s��̐������Ȃǂ�������
//	bVecAll.resize(vertices.size(),1);
//	TVecAll.resize(vertices.size());
//
//	//	���b�V���ߓ_���x ������
//	SetVerticesTempAll(30.0);
//
//	//�]�������p�R�[�h
//	//>	LMS�̂��߂̃R�[�h
//	//	�ߓ_���x�̏����ݒ�(�s������O�ɍs��)
//	//SetVerticesTempAll((158.5 + (30.0 + 158.5) /2.0)/2.0 );
//	//SetVerticesTempAll((158.5+30.0)/2.0);
//	
//	
//	// main.cpp�œ����悤�ɐݒ肷��
//	round.clear();
//	tempe.clear();
//	for(unsigned i=0;i<11;++i){
//		round.push_back(i*0.01);
//	}
//	//200
//	//tempe.push_back(211.3);
//	//tempe.push_back(211.2);
//	//tempe.push_back(210.2);
//	//tempe.push_back(207.6);
//	//tempe.push_back(203.9);
//
//	//tempe.push_back(198.0);
//	//tempe.push_back(189.3);
//	//tempe.push_back(178.7);
//	//tempe.push_back(169.8);
//	//tempe.push_back(158.5);
//	
//	// 150
//	//double tempe2[10] = {158.6, 158.3, 156.8, 153.9, 150.4, 146.0, 140.3, 134.8, 130.8, 125.7};
//	//tempe.push_back(158.6);
//	//tempe.push_back(158.3);
//	//tempe.push_back(156.8);
//	//tempe.push_back(153.9);
//	//tempe.push_back(150.4);
//
//	//tempe.push_back(146);
//	//tempe.push_back(140.3);
//	//tempe.push_back(134.8);
//	//tempe.push_back(130.8);
//	//tempe.push_back(125.7);
//
//	//50
//	tempe.push_back(50.0);
//	tempe.push_back(50.0);
//	tempe.push_back(49.9);
//	tempe.push_back(49.8);
//	tempe.push_back(49.7);
//
//	tempe.push_back(49.3);
//	tempe.push_back(48.7);
//	tempe.push_back(48.5);
//	tempe.push_back(48.4);
//	tempe.push_back(48.0);
//
//	SetConcentricHeatMap(round,tempe,Vec2d(0.0, -0.005));		//	-0.001�ɂ��Ă��A�������x�͕s�ς������B
//	DSTR << "���� check it out" <<std::endl;
//	for(unsigned i=0; i< tempe.size();++i){
//		DSTR << round[i] << "; "<< i*0.01 <<"; " <<  tempe[i] << std::endl;
//	}
//
//	InitTcAll(temp_c);
//	//>	�M���ˊ֌W	LMSS
//	InitToutAll(temp_out);
//	SetThermalEmissivityToVerticesAll(ems);	
//
//	//vertices.temp�����ׂāATVecAll�֑������
//	CreateTempVertex();
//	CalcVtxDisFromVertex(Vec2d(0.0, -0.005));		// �S�̌��_���W�������ɓ���A�Ώ̐��������悤�ɂȂ��Ă���B
//	InitCreateMatk();					///	CreateMatK�̏�����
//	keisuInv.resize(vertices.size(),vertices.size());
//	keisuInv.clear();
//	///	�M�`�B�����e�ߓ_�Ɋi�[
//	SetHeatTransRatioToAllVertex();
//	for(unsigned i=0; i < this->tets.size(); i++){
//
//		/*���쌴�ǉ���������--------------------------------------------*/
//		//�\��face�̖ʐς��v�Z
//		for(unsigned j= 0 ; j < 4; j++){
//			if(tets[i].faces[j] < (int)nSurfaceFace){			///	�O�k�̖�
//				///	�l�ʑ̂̎O�p�`�̖ʐς��v�Z		///	���̊֐��̊O�Ŗʐϕ��̖ʐόv�Z����������B�ړ�����
//				faces[tets[i].faces[j]].area = CalcTriangleArea(faces[tets[i].faces[j]].vertices[0], faces[tets[i].faces[j]].vertices[1], faces[tets[i].faces[j]].vertices[2]);
//			}
//		}
//		/*���쌴�ǉ������܂�--------------------------------------------*/
//
//		tets[i].volume = CalcTetrahedraVolume2(i);
//
//		//�e�s�������āA�K�E�X�U�C�f���Ōv�Z���邽�߂̌W���̊�{�����BTimestep�̓����Ă��鍀�́A���̃\�[�X(SetDesc())�ł́A�����ł��Ȃ����Ƃ���������(NULL���Ԃ��Ă���)
//		CreateMatkLocal(i);				///	Matk1 Matk2(�X�V���K�v�ȏꍇ������)�����	//ifdef�X�C�b�`�őS�̍����s���(�\���p����)�����\
//	}
//	//���_�̕\�ʖʐς��v�Z�i���쌴�ǉ��j
//	calcVerticesArea();
//
//	//�J�E���g�̏�����
//	COUNT = 0;
//
//
//}

void PHFemThermo::IfRadiantHeatTrans(){
	for(int id =0;id<NSurfaceVertices();id++){
		if(vertexVars[id].beRadiantHeat){
			vertexVars[id].thermalEmissivity = ems;
			vertexVars[id].thermalEmissivity_const = ems_const;
		}
		else{
			vertexVars[id].thermalEmissivity = 0.0;
			vertexVars[id].thermalEmissivity_const = 0.0;
		}	
	}
}

void PHFemThermo::UpdateMatk_RadiantHeatToAir(){
		for(unsigned i =0; i < GetPHFemMesh()->edges.size();i++){
			edgeVars[i].k = 0.0;
		}
#ifdef UseMatAll
		matKAll.clear();
#endif
		dMatKAll[0].clear(); 
		for(unsigned i=0;i<GetPHFemMesh()->tets.size();++i){
			CreateMatkLocal_update(i);
		}
#ifdef badMatK
		if(strcmp(GetPHFemMesh()->GetName(), "femNsteak") == 0){
			//if(matkupSwitch){
				CalcMatKup();
				//matkupSwitch = false;
			//}
			for(unsigned i=0; i < GetPHFemMesh()->vertices.size(); i++){
				dMatKAll[0][i] += matkupdate[i];
#ifdef UseMatAll
				matKAll[i][i] += matkupdate[i];
#endif
			}
		}
#endif
#ifdef UseMatAll
#ifdef outMatK
		if(strcmp(GetPHFemMesh()->GetName(), "femNsteak") == 0){
			OutputMatKall();
		//}
		matKAllout.open("matKAllout.csv");
		for(unsigned i=0; i<GetPHFemMesh()->vertices.size(); i++){
			for(unsigned j=0; j<GetPHFemMesh()->vertices.size(); j++){
				matKAllout << matKAll[i][j] << ",";
			}
			matKAllout << std::endl;
		}
		matKAllout.close();
		}//
#endif
#endif
}

void PHFemThermo::UpdateIHheat(unsigned heatingMODE){

	//���j�F�M�����x�N�g���v�Z�݂̂��܂߂�B�i�M�`����M�e�ʂ͋��ʂȂ̂ł��̊֐��ɂ͊܂߂Ȃ��j

	//�M�`�����A���x�A��M�A�M�`�B���@�̃p�����[�^�[��ݒ�E���
		//PHFemThermo�̃����o�ϐ��̒l���� CADThermo���A0.574;//�ʂ˂��̒l//�M�`����[W/(���EK)]�@Cp = 1.96 * (Ndt);//�ʂ˂��̔�M[kJ/(kg�EK) 1.96kJ/(kg K),�i�ʂ˂��̖��x�j�H�i���M�̉Ȋwp64���970kg/m^3
		//�M�`�B���̒P�ʌn�@W/(m^2 K)�˂����SI�P�ʌn�Ȃ̂��H�@25�͘_��(MEAT COOKING SIMULATION BY FINITE ELEMENTS)�̃I�[�u�����M���̎����l
		//SetInitThermoConductionParam(0.574,970,1.96,25);
	//SetInitThermoConductionParam(0.574,970,0.1960,25 * 0.01);		//> thConduct:�M�`���� ,roh:���x,	specificHeat:��M J/ (K�Ekg):1960 ,�@heatTrans:�M�`�B�� W/(m^2�EK)
		//�����A�ϐ��l�͌ォ��v�Z�̓r���ŕύX�ł���悤��SetParam()�֐�������Ă������ق����������ȁH

	//.		�M�����̐ݒ�
	//..	������
	//SetVtxHeatFluxAll(0.0);

	//	CalcIHdqdt�̑O�ɕK�v
	//faces[i].fluxarea = 0.0;
	

	//1.�t���C�p���ʒu������Ă���
		//ih���M�~���S����̓��S�~����M�̈���v�Z���Aihdqdt�ɓ��Ă͂߂郁�b�V�����𐶐�
		//�@if(�t���C�p������������)	�����Ă��Ȃ���΁Avecf���A1step�O�̒l���g����悤�ɂ��Ă��������B

	

	//2...	face�ʂł̔M�����ʂ��v�Z�i�t���C�p���ʒu���̓|�C���^�������ɑ���F����t���C�p���̈ʒu���ω�����̂ŁA�t���C�p���ʒu�̕ω��̓x�ɐ�������j
	if(heatingMODE == OFF){
		CalcIHdqdt_atleast(0.0,0.0,0.0, OFF);		//	IH���M�s��̌W��0�ƂȂ邽�߁A�v�Z����Ȃ�
	}
	else if(heatingMODE == WEEK){	
#ifdef TempDependHeat
		CalcIHdqdt_atleast_high(inr_,outR_,weekPow_, WEEK);
		CalcIHdqdt_add_high(inr_add,outR_add,weekPow_add, WEEK);
		CalcIHdqdt_decrease_high(inr_decr,outR_decr,weekPow_decr, WEEK);
#else
	#ifdef DISABLE_COIL
		//CalcIHdqdt_atleast_map(Vec2d(0.0, -0.005),weekPow_FULL,WEEK);
		CalcIHdqdt_atleast_map(Vec2d(0.0425, -0.0425),weekPow_FULL,WEEK);
	#else
		CalcIHdqdt_atleast(inr_,outR_,weekPow_, WEEK);		//	API���ς�
		CalcIHdqdt_add(inr_add,outR_add,weekPow_add, WEEK);
		CalcIHdqdt_decrease(inr_decr,outR_decr,weekPow_decr, WEEK);
	#endif

	//>�ʂɌĂԎd�l�ɕύX	//UpdateMatk_RadiantHeatToAir();				//	�M�`�B���E�����ŋ�C�ւ̔M�`�B�������X�V����
#endif
	}	
	else if(heatingMODE == MIDDLE){
		//CalcIHdqdt_atleast(inr_,outR_,231.9 * 0.005 * 1e4, MIDDLE);		//
		CalcIHdqdt_atleast(0.11,0.14,231.9 * 0.005 * 1e4, MIDDLE);		//
	}
	else if(heatingMODE == HIGH){
		CalcIHdqdt_atleast(0.11,0.14,231.9 * 0.005 * 1e5, HIGH);		//
	}
	// GaussSeidel�v�Z����
	doCalc = true;

	//3.�e�ʂł̔M�����ʂ���S�̍����x�N�g�������B{F}�ɑ��

#if 1			// switch1
	//UpdateVecF_frypan();		//	�֐��̊O�Ɉړ�2014.3.12
#endif
	//%%	IH���M�̃��[�h�ؑ�
	//	���C����ɉ��M
	//	CalcIHdqdtband_(0.09,0.10,231.9 * 5e3);		//*0.5*1e4	�l��ς��Ď���	//*1e3�@//*1e4 //5e3
	//	�~��ɉ��M

	//	���̌�ŁA�M�����x�N�g�����v�Z����֐����Ăяo��
	///	�M�`�B�����e�ߓ_�Ɋi�[
	//SetHeatTransRatioToAllVertex();
#if 0			//!switch1
	InitVecFAlls();
	for(unsigned i=0; i < this->tets.size(); i++){
		CreateVecFAll(i);
	}
#endif

#if 0
	CreateVecF2surfaceAll();		//	CreateVecFAll(i);�̑���
	CreateVecF3surfaceAll();		//	CreateVecFAll(i);�̑���
#endif
}

//���쌴�ǉ���������==========================================
void PHFemThermo::UpdateIHheatband(double xS,double xE,unsigned heatingMODE){////x���W�FxS~xE�̊Ԃ̐ߓ_�ɔM�������E������ݒ�

	//�M�`�����A���x�A��M�A�M�`�B���@�̃p�����[�^�[��ݒ�E���
		//PHFemThermo�̃����o�ϐ��̒l���� CADThermo���A0.574;//�ʂ˂��̒l//�M�`����[W/(���EK)]�@Cp = 1.96 * (Ndt);//�ʂ˂��̔�M[kJ/(kg�EK) 1.96kJ/(kg K),�i�ʂ˂��̖��x�j�H�i���M�̉Ȋwp64���970kg/m^3
		//�M�`�B���̒P�ʌn�@W/(m^2 K)�˂����SI�P�ʌn�Ȃ̂��H�@25�͘_��(MEAT COOKING SIMULATION BY FINITE ELEMENTS)�̃I�[�u�����M���̎����l
		//SetInitThermoConductionParam(0.574,970,1.96,25);
	//SetInitThermoConductionParam(0.574,970,0.1960,25 * 0.01);		//> thConduct:�M�`���� ,roh:���x,	specificHeat:��M J/ (K�Ekg):1960 ,�@heatTrans:�M�`�B�� W/(m^2�EK)
		//�����A�ϐ��l�͌ォ��v�Z�̓r���ŕύX�ł���悤��SetParam()�֐�������Ă������ق����������ȁH

	//.		�M�����̐ݒ�
	//..	������
	//SetVtxHeatFluxAll(0.0);

	//1.�t���C�p���ʒu������Ă���
		//ih���M�~���S����̓��S�~����M�̈���v�Z���Aihdqdt�ɓ��Ă͂߂郁�b�V�����𐶐�
		//�@if(�t���C�p������������)	�����Ă��Ȃ���΁Avecf���A1step�O�̒l���g����悤�ɂ��Ă��������B

	//2...	face�ʂł̔M�����ʂ��v�Z�i�t���C�p���ʒu���̓|�C���^�������ɑ���F����t���C�p���̈ʒu���ω�����̂ŁA�t���C�p���ʒu�̕ω��̓x�ɐ�������j
	if(heatingMODE == OFF){
		CalcIHdqdtband(xS,xE,0.0, OFF);		//	IH���M�s��̌W��0�ƂȂ邽�߁A�v�Z����Ȃ�
	}
	else if(heatingMODE == WEEK){	
		CalcIHdqdtband(xS,xE,231.9 * 1e1, WEEK);		//
	}
	else if(heatingMODE == MIDDLE){
		CalcIHdqdtband(xS,xE,231.9 * 0.005 * 1e4, MIDDLE);		//
	}
	else if(heatingMODE == HIGH){
		CalcIHdqdtband(xS,xE,231.9 * 0.005 * 1e5, HIGH);		//
	}

	//3.�e�ʂł̔M�����ʂ���S�̍����x�N�g�������B{F}�ɑ��

#if 1			// switch1
	UpdateVecF_frypan();
#endif
	//%%	IH���M�̃��[�h�ؑ�
	//	���C����ɉ��M
	//	CalcIHdqdtband_(0.09,0.10,231.9 * 5e3);		//*0.5*1e4	�l��ς��Ď���	//*1e3�@//*1e4 //5e3
	//	�~��ɉ��M

	//	���̌�ŁA�M�����x�N�g�����v�Z����֐����Ăяo��
	///	�M�`�B�����e�ߓ_�Ɋi�[
	//SetHeatTransRatioToAllVertex();
#if 0			//!switch1
	InitVecFAlls();
	for(unsigned i=0; i < this->tets.size(); i++){
		CreateVecFAll(i);
	}
#endif

#if 0
	CreateVecF2surfaceAll();		//	CreateVecFAll(i);�̑���
	CreateVecF3surfaceAll();		//	CreateVecFAll(i);�̑���
#endif
}
//���쌴�ǉ������܂�==========================================

void PHFemThermo::SetParamAndReCreateMatrix(double thConduct0,double roh0,double specificHeat0){
	// �f�o�b�O�̂��߁@&&�@�g���Ă��Ȃ������̂ŁA�R�����g�A�E�g
	DSTR << "���̊֐��̓f�o�b�O�̂��߁A�R�����g�A�E�g���Ă��܂��B�p����ꍇ�ɂ́A������ǂ����ĕ��������Ă��������B" <<std::endl;

	//for(unsigned i =0; i < edges.size();i++){
	//	edges[i].c = 0.0;
	//	edges[i].k = 0.0;
	//}

	/////	faces
	//for(unsigned i=0;i<faces.size();i++){
	//	faces[i].alphaUpdated = true;
	//	faces[i].area = 0.0;
	//	faces[i].heatTransRatio = 0.0;
	//	faces[i].deformed = true;				//������Ԃ́A�ό`��Ƃ���
	//	faces[i].fluxarea =0.0;
	//	for(unsigned j =0; j < HIGH +1 ; j++){			// ���M���[�h�̐������A�x�N�g���𐶐�
	//		faces[i].heatflux[j] = 0.0;
	//	}
	//}

	////�s��̐������Ȃǂ�������
	//bVecAll.resize(vertices.size(),1);

	////�ߓ_���x�̏����ݒ�(�s������O�ɍs��)
	//SetVerticesTempAll(0.0);			///	�������x�̐ݒ�

	////���͗��̉��x�̏�����(temp�x�ɂ���)
	//InitTcAll(0.0);

	////dmnN �����̉��x�̏c�i��j�x�N�g��
	//CreateTempVertex();

	////�M�`�����A���x�A��M�A�M�`�B���@�̃p�����[�^�[��ݒ�E���
	//	//PHFemThermo�̃����o�ϐ��̒l���� CADThermo���A0.574;//�ʂ˂��̒l//�M�`����[W/(���EK)]�@Cp = 1.96 * (Ndt);//�ʂ˂��̔�M[kJ/(kg�EK) 1.96kJ/(kg K),�i�ʂ˂��̖��x�j�H�i���M�̉Ȋwp64���970kg/m^3
	//	//�M�`�B���̒P�ʌn�@W/(m^2 K)�˂����SI�P�ʌn�Ȃ̂��H�@25�͘_��(MEAT COOKING SIMULATION BY FINITE ELEMENTS)�̃I�[�u�����M���̎����l
	////. �M�`�B���� SetInitThermoConductionParam(0.574,970,0.1960,25 * 0.001 );		//> thConduct:�M�`���� ,roh:���x,	specificHeat:��M J/ (K�Ekg):1960 ,�@heatTrans:�M�`�B�� W/(m^2�EK)
	////. �M�`�B���Ȃ�
	//SetInitThermoConductionParam(thConduct0,roh0,specificHeat0,0);		// �M�`�B��=0;�ɂ��Ă���w
	//
	////> �M�����̏�����
	////SetVtxHeatFluxAll(0.0);			// ���_�̔M�����̏�����

	////>	�M���˗��̐ݒ�
	//SetThermalEmissivityToVerticesAll(0.0);				///	�b��l0.0�ŏ�����	�F�M���˂͂��Ȃ���

	////> IH���M����face��������x(�\��face && �����)�i��A�֌W��������face�ߓ_�̌��_����̋������v�Z���Aface[].mayIHheated�𔻒�
	//CalcVtxDisFromOrigin();
	////CalcVtxDisFromVertex(0.0,-1.2);
	////>	IH����̒P�ʎ��ԓ�����̉��M�M��	//�P�ʎ��ԓ�����̑����M�M��	231.9; //>	J/sec
	//
	////	���̌�ŁA�M�����x�N�g�����v�Z����֐����Ăяo��
	//InitCreateMatC();					///	CreateMatC�̏�����
	//InitVecFAlls();					///	VecFAll�ނ̏�����
	//InitCreateMatk();					///	CreateMatK�̏�����

	/////	�M�`�B�����e�ߓ_�Ɋi�[
	//SetHeatTransRatioToAllVertex();
	//for(unsigned i=0; i < this->tets.size(); i++){
	//	tets[i].volume = CalcTetrahedraVolume2(i);
	//	//�e�s�������āA�K�E�X�U�C�f���Ōv�Z���邽�߂̌W���̊�{�����BTimestep�̓����Ă��鍀�́A���̃\�[�X(SetDesc())�ł́A�����ł��Ȃ����Ƃ���������(NULL���Ԃ��Ă���)
	//	CreateMatkLocal(i);				///	Matk1 Matk2(�X�V���K�v�ȏꍇ������)�����	//ifdef�X�C�b�`�őS�̍����s���(�\���p����)�����\
	//	CreatedMatCAll(i);
	//	CreateVecFAll(i);
	//}
	//
	//// �J�E���g�̏�����
	////Ndt =0;

	////������������̏�����
	//InitMoist();

	//SetVerticesTempAll(0.0);

}

//void PHFemThermo::ReProduceMat_Vec_ThermalRadiation() {	
//
//	//%%%	��������		%%%//
//
//	//�e�탁���o�ϐ��̏������˃R���X�g���N�^�łł����ق���������������Ȃ��B
//	///	Edges
//	for(unsigned i =0; i < edges.size();i++){
//		edges[i].c = 0.0;	
//		edges[i].k = 0.0;
//	}
//
//	///	faces
//	for(unsigned i=0;i<faces.size();i++){
//		faces[i].alphaUpdated = true;
//		faces[i].area = 0.0;
//		faces[i].heatTransRatio = 0.0;
//		faces[i].deformed = true;				//������Ԃ́A�ό`��Ƃ���
//		faces[i].fluxarea =0.0;
//		faces[i].thermalEmissivity =0.0;
//		for(unsigned mode =0; mode < HIGH +1 ; mode++){			// ���M���[�h�̐������A�x�N�g���𐶐�
//			faces[i].heatflux[mode] = 0.0;
//		}
//	}
//
//	//�s��̐������Ȃǂ�������
//	bVecAll.resize(vertices.size(),1);	
//	TVecAll.resize(vertices.size());
//
//	SetVerticesTempAll(jout);
//
//	//���͂ւ̔M�`�B���x�̏�����(temp�x�ɂ���)�@���@�M�t��
//	InitTcAll(temp_c);
//	//>	�M���ˊ֌W	LM SS
//	InitToutAll(temp_out);
//	//SetThermalEmissivityToVerticesAll(radiantHeat);				///	�b��l0.0�ŏ�����	�F�M���˂͂��Ȃ���	
//	SetThermalEmissivityToVerticesAll(ems);	
//
//	//vertices.temp�����ׂāATVecAll�֑������
//	CreateTempVertex();
//
//	DSTR << "thConduct:" << thConduct << std::endl;
//	
//	CalcVtxDisFromVertex(Vec2d(0.0, -0.005));		// �S�̌��_���W�������ɓ���A�Ώ̐��������悤�ɂȂ��Ă���B
//	InitCreateMatC();					///	CreateMatC�̏�����
//	InitVecFAlls();					///	VecFAll�ނ̏�����
//	InitCreateMatk();					///	CreateMatK�̏�����
//
//	keisuInv.resize(vertices.size(),vertices.size());
//	keisuInv.clear();
//
//	///	�M�`�B�����e�ߓ_�Ɋi�[
//	SetHeatTransRatioToAllVertex();
//	for(unsigned i=0; i < this->tets.size(); i++){
//		tets[i].volume = CalcTetrahedraVolume2(i);
//		CreateMatkLocal(i);				///	Matk1 Matk2(�X�V���K�v�ȏꍇ������)�����	//ifdef�X�C�b�`�őS�̍����s���(�\���p����)�����\
//		CreatedMatCAll(i);
//		CreateVecFAll(i);
//	}
//#if 0
//
//	//	���_�P�̒S���̐ςɑ΂��A�M�ʂ������邽�߂ɁA�S���̐ϊ��Z�ŔM�ʂ𒸓_�̉��x�Ƃ��ė^����B{F}���g��Ȃ��̂ŁA�M�������g��Ȃ��B
//	double rcv=0.0;
//	for(unsigned i=0;i<vertices[0].tets.size();i++){
//		rcv += tets[vertices[0].tets[i]].volume * RHO * SPECIFICHEAT * 5 / 20; 
//	}
//	double kuwae =1.58;	//	������M��
//	//vertices[0].temp = kuwae / rcv;
//	SetVertexTemp(0,kuwae / rcv);
//#endif
//	//����temp��TVecAll�ɐݒ�
//	//C,K�����̌v�Z�������ėl�q������
//
//
//	int hogeshidebug =0;
//	//	�ߓ_���x���ڂ̏����o��
////	templog.open("templog.csv");
//
//	//matCAllout.open("matCAllout.txt"); 
//	//matKAllout.open("matKAllout.txt");
//	//
//	//matCAllout << matCAll << std::endl;
//	//matKAllout << matKAll << std::endl;
//	//
//	//matCAllout.close();
//
//	//matCAllout.open("matCAll-1out.txt");
//	//matCAllout << matCAll.inv() << std::endl;
//	//matCAllout.close();
//	////scilab���Ă΂Ȃ��ŁASPR�̋@�\�ŁAC.inv() K �����߂�
//	//matCAllout.open("matCAll.inv()xmatKAll.txt");
//	//matCAllout << matCAll.inv() * matKAll << std::endl; 
//	//matCAllout.close();
//
//	//tempMat.resize(vertices.size(),vertices.size());
//	//tempMat.clear();
//	//tempMat = matCAll.inv() * matKAll;
//
//	//std::vector<double> rowval;
//	//double tempval=0;
//	//for(unsigned i=0;i<vertices.size();i++){
//	//	for(unsigned j=0;j<vertices.size();j++){
//	//	tempval += tempMat[i][j];
//	//	}
//	//	rowval.push_back(tempval);
//	//}
//	//matCAllout.open("matCAll.inv()xmatKAll�̊e��̘a.txt");
//	//for(unsigned i=0;i<vertices.size();i++){
//	//	matCAllout << rowval[i] <<std::endl;
//	//}
//	//matCAllout.close();
//
//	//�J�E���g�̏�����
//	COUNT = 0;
//
//	//���x�ω��o��
//	checkTVecAllout.open("checkTVecAllout.csv");
//	checkTVecAllout <<"����" << COUNT<<", ";
//	for(unsigned i=0; i < vertices.size();i++){
//		if(i != vertices.size() -1){
//			checkTVecAllout << "���_" << i << ", ";	
//		}
//		else{
//			checkTVecAllout << "���_" << i << std::endl;
//		}
//	}
//	FEMLOG.open("femLog.csv");
//
//	//	CPS�̌o���ω��������o��
//	//cpslog.open("cpslog.csv");
//
//	// �J�E���g�̏�����
//	Ndt =0;
//
//	//������������̏�����
//	InitMoist();
//
//}




void PHFemThermo::AfterSetDesc() {	
}

//void PHFemThermo::CreateLocalMatrixAndSet(){
//	//K,C,F�̍s������֐����Ăяo���āA��点��
//	for(unsigned i = 0; i< tets.size() ; i++){
//		//tets�������ɂ����ƁA���̍s��E�x�N�g��������Ă���āA�ł����s��A�x�N�g������ɌW����ݒ肵�����
//		//��������΁A�e�v�f�����s���for�����񂳂Ȃ��Ă��悭�Ȃ�
//		//CreateMatkLocal(tets);
//		//CreateMatcLocal(tets);
//		//CreateVecfLocal(tets);
//
//		//tets�����č�点��
////		SetkcfParam(tets);
//
//	}
//
//}

//void PHFemThermo::SetkcfParam(Tet tets){
//}

void PHFemThermo::OutputMatKall(){
	//matKAllout.open("matKAllout.txt");
	//matKAllout << matKAll << std::endl;
	//matKAllout.close();
#ifdef badMatK
	upmatKAllout.open("upMatKAllout.csv");
	matKAllout.open("matKAllout.csv");
	for(unsigned i=0; i<GetPHFemMesh()->vertices.size(); i++){
		for(unsigned j=0; j<GetPHFemMesh()->vertices.size(); j++){
			if(i==j){
				matKAllout << matKAll[i][j]-matkupdate[i] << ",";
			}else{
				matKAllout << matKAll[i][j] << ",";
			}
			upmatKAllout << matKAll[i][j] << ",";
		}
		matKAllout << std::endl;
		upmatKAllout << std::endl;
	}
	matKAllout.close();
	upmatKAllout.close();
	std::ofstream upVecout;
	upVecout.open("upVecout.csv");
	for(unsigned i=0; i<matkupdate.size(); i++){
		upVecout << matkupdate[i] << std::endl;
	}
	upVecout.close();
#endif
	std::ofstream matkprop;
	matkprop.open("matKprop.txt");
	double maxgap = 0.0;
	//unsigned maxvtx;
	//double maxtotal;
	for(unsigned i=0; i < GetPHFemMesh()->vertices.size(); i++){
		double total = 0.0;
		for(unsigned j=0; j < GetPHFemMesh()->vertices.size(); j++){
			if(i != j){
				total += fabs(matKAll[i][j]);
			}
		}
		if(total >= fabs(matKAll[i][i])){
			matkprop << i << "�s�ځF�@�Ίp�����F" << matKAll[i][i] << "�@��Ίp�����̘a�F" << total << std::endl;
		}else{
			if((total - matKAll[i][i]) > maxgap){
				maxgap = total - matKAll[i][i];
			//	maxvtx = i;
				//maxtotal = total;
			}
		}
	}
	matkprop << "�Ίp�����Ɣ�Ίp�����̘a�̍��̍ő�l�F"<< maxgap << std::endl;
	//matkprop << maxvtx << "�s�ځF�@�Ίp�����F" << matKAll[maxvtx][maxvtx] << "�@��Ίp�����̘a�F" << maxtotal;
	matkprop.close();
}

void PHFemThermo::CreateMatc(unsigned id){
	//�Ō�ɓ����s���������
	for(unsigned i =0; i < 4 ;i++){
		for(unsigned j =0; j < 4 ;j++){
			matc[i][j] = 0.0;
		}
	}
	//matc ��21�łł����s�������
	matc = Create44Mat21();
	//matc = Create44Mat50();
	//	for debug
		//DSTR << "matc " << matc << " �� ";
	
	matc = rho * specificHeat * CalcTetrahedraVolume2(id) / 20.0 * matc;
	
	//	debug	//�W���̐ς��Ƃ�
		//DSTR << matc << std::endl;
		//int hogemat =0 ;
}

void PHFemThermo::CreatedMatCAll(unsigned id){
	PHFemMeshNew* mesh = GetPHFemMesh();

	//���ׂĂ̗v�f�ɂ��ČW���s������
		//c
	CreateMatc(id);
	int mathoge=0;
	//	(�K�E�X�U�C�f�����g�����v�Z��)�v�f���ɍ�����s��̐������A�G�b�W�ɌW�����i�[����
	//	or	(�K�E�X�U�C�f�����g��Ȃ��v�Z��)�v�f���Ƃ̌v�Z���I��邽�тɁA�v�f�����s��̐����������G�b�W��_�ɍ��ϐ��Ɋi�[���Ă���	#ifedef�Ń��[�h����āA�ǂ�����ł���悤�ɂ��Ă����Ă��ǂ�����w
#ifdef UseGaussSeidel
	for(unsigned j=1; j < 4; j++){
		int vtxid0 = mesh->tets[id].vertexIDs[j];
		//	���O�p�s�񕔕��ɂ��Ă̂ݎ��s
		//	j==1:k=0, j==2:k=0,1, j==3:k=0,1,2
		for(unsigned k = 0; k < j; k++){
			int vtxid1 = mesh->tets[id].vertexIDs[k];
				for(unsigned l =0; l < mesh->vertices[vtxid0].edgeIDs.size(); l++){
					for(unsigned m =0; m < mesh->vertices[vtxid1].edgeIDs.size(); m++){
						if(mesh->vertices[vtxid0].edgeIDs[l] == mesh->vertices[vtxid1].edgeIDs[m]){
							edgeVars[mesh->vertices[vtxid0].edgeIDs[l]].c += matc[j][k];		//�������̂������͂������甼���ɂ���B��O�p�����O�p������������ɂ́A�ǂ�����for�����ɂ���Ηǂ��̂��H
							//DSTR << edges[vertices[vtxid0].edges[l]].k << std::endl;
						}
					}
				}
		}
	}
	//�Ίp������Ίp�����̑S�̍����s�񂩂甲���o����1�~n�̍s��ɑ������
	//j=0~4�܂ő��(��̃��[�v�ł́Aj�͑Ίp�����͈̔͂����Ȃ��̂ŁA�l������Ȃ��������o�Ă��܂�)
	for(unsigned j =0;j<4;j++){
		dMatCAll[0][mesh->tets[id].vertexIDs[j]] += matc[j][j];
	}
#endif

#ifdef UseMatAll
	//SciLab�Ŏg�����߂ɁA�S�̍����s������
	//matk������
	for(unsigned j=0; j<4 ; j++){
		for(unsigned k=0; k<4 ;k++){
			matCAll[mesh->tets[id].vertexIDs[j]][mesh->tets[id].vertexIDs[k]] += matc[j][k];
		}
	}
#endif

	////	for debug
	//DSTR << "dMatCAll : " << std::endl;
	//for(unsigned j =0;j < vertices.size();j++){
	//	DSTR << j << "th : " << dMatCAll[0][j] << std::endl;
	//}
	// �l�M�ɂ��Ĕ�0�����ɂȂ����B

	//	���ׂ�
	//dMatKAll�̐����̂����A0�ƂȂ�v�f����������A�G���[�\��������R�[�h������
	// try catch���ɂ���
	//for(unsigned j = 0; j < vertices.size() ; j++){
	//	if(dMatCAll[0][j] ==0.0){
	//		DSTR << "dMatCAll[0][" << j << "] element is blank" << std::endl;
	//	}
	//}
	int piyodebug =0;
}

void PHFemThermo::CreateVecFAll(unsigned id){

	vecf.clear();
	//	����
	//	f3���g�p����ꍇ:���͗��̉��xTc��0�̐ߓ_�̗v�f��0�ɂȂ邽�߁A���x�̐ݒ肪�K�v
	
	//���ׂĂ̗v�f�ɂ��ČW���s������
	//f1�����
	//>	�M�������E����	vecf2�����			
	CreateVecf2surface(id);				//�l�ʑ̂̊e�ʂɂ��Čv�Z�����Z�i�d�˂��킹�j����
	
	//DSTR << "tets[id].vecf[2]: " << tets[id].vecf[2] << std::endl;
	CreateVecf4surface(id);			//produce vecf[3]

	//>	�M�`�B���E����	f3�����
#ifndef NOTUSE_HEATTRANS_HERE
	CreateVecf3(id);			// surface�����ׂ������	//>	tets[id].vecf[2] ��������,���		�M�`�B���͑������ρA���͗��̉��x�͐ߓ_�̌`��֐��H���Ƃɋ��߂�
	
	CreateVecf3surface(id);			//��������
#endif
	//CreateVecf3_(id);			//>	tets[id].vecf[2] ��������,���		�M�`�B���A���͗��̉��x�𑊉����ςŋ��߂�
	//f4�����
	//f1:vecf[0],f2:vecf[1],f3:vecf[2],f4:vecf[3]�����Z����

#ifndef NOTUSE_HEATTRANS_HERE
	vecf = tets[id].vecf[1] + tets[id].vecf[2] + tets[id].vecf[3];		//>	+ tets[id].vecf[0] +  tets[id].vecf[3] �̗\��
#else	//	�M�t�˂�M�`�B�͐؂�ւ�����悤�ɂ���ׂ��B���A�v���P�[�V����
	vecf += tetVars[id].vecf[1] + tetVars[id].vecf[3];
#endif

	//	(�K�E�X�U�C�f�����g�����v�Z��)�v�f���ɍ�����s��̐������A�G�b�W�ɌW�����i�[����
	//	or	(�K�E�X�U�C�f�����g��Ȃ��v�Z��)�v�f���Ƃ̌v�Z���I��邽�тɁA�v�f�����s��̐����������G�b�W��_�ɍ��ϐ��Ɋi�[���Ă���	#ifedef�Ń��[�h����āA�ǂ�����ł���悤�ɂ��Ă����Ă��ǂ�����w

	//�v�f�̐ߓ_�ԍ��̏ꏊ�ɁA���̐ߓ_��f�̒l������
	//j:�v�f�̒��̉��Ԗڂ�
	for(unsigned j =0;j < 4; j++){
		int vtxid0 = GetPHFemMesh()->tets[id].vertexIDs[j];
		vecFAllSum[vtxid0] += vecf[j];
		//vecFAll_f2IH[num][vtxid0][0] += vecf[j];
	}
	//	for debug
	//vecFAllSum�ɒl���������̂��ǂ����𒲂ׂ� 2011.09.21�S���ɒl�������Ă��邱�Ƃ��m�F����
	//DSTR << "vecFAllSum : " << std::endl;
	//for(unsigned j =0; j < vertices.size() ; j++){
	//	DSTR << j << " ele is :  " << vecFAllSum[j] << std::endl;
	//}

	////	���ׂ�
	////vecFAllSum�̐����̂����A0�ƂȂ�v�f����������A�G���[�\��������R�[�h������
	//// try catch���ɂ���
	//for(unsigned j = 0; j < vertices.size() ; j++){
	//	if(vecFAllSum[j] ==0.0){
	//		DSTR << "vecFAllSum[" << j << "] element is blank" << std::endl;
	//	}
	//}

}


void PHFemThermo::CreateMatkLocal_update(unsigned id){
	PHFemMeshNew* mesh = GetPHFemMesh();

	//	�l�ʑ̗v�f�ɂ��ČW���s��[k]�����			�����K��	
	//..	k1�����	k1k�ł��Ak1b�ł��ǂ���ł��\��Ȃ�	�ǂ��炪���������ׂ�
	///....		�ό`������������������
	//if(deformed){	
	//	CreateMatk1k(id);
	//}			//  K��ꍀ�i���b�V�����M�`���j�̍s��쐬	//k���_�������ɁA���M���āA�`��֐��𓱏o	
	//if(deformed){	CreateMatk1b(id);}			//	���Ђ̗��_�������ɁA������p���Č`��֐��𓱏o
	
	//....		�M�`�B���E�����ɕK�v��k2
	//CreateMatk2t(id);					///	K2: �M �`�B ���E����
	//CreateMatk3t(id);					///	K3: �M �t�� ���E����(��C�ւ̔M�`�B��)
	CreateMatk3t_nonRadiantHeat(id);	//>	��LK3�ɑ΂��A�M�`�Bface�ł́A

	//k1,k2,k3�����Z����(�g���Ă��鐔�l����)
	matk = tetVars[id].matk[0] + tetVars[id].matk[1] + tetVars[id].matk[2];	
	//<����>	matk[0]:K1, matk[1]:K2, matk[2]:K3

#ifdef UseGaussSeidel
	//	(�K�E�X�U�C�f�����g�����v�Z��)�v�f���ɍ�����s��̐������A�G�b�W�ɌW�����i�[����
	//	or	(�K�E�X�U�C�f�����g��Ȃ��v�Z��)�v�f���Ƃ̌v�Z���I��邽�тɁA�v�f�����s��̐����������G�b�W��_�ɍ��ϐ��Ɋi�[���Ă���	#ifedef�Ń��[�h����āA�ǂ�����ł���悤�ɂ��Ă����Ă��ǂ�����w
	//	Edges �̂��̗v�f�Ōv�Z����K�s��̐�����k�ɌW���Ƃ��Ċi�[����		
	//matk�̑Ίp�����ȊO�ŁA���O�p�̕����̒l���Aedge��k�ɑ������
	for(unsigned j=1; j < 4; j++){
		int vtxid0 = mesh->tets[id].vertexIDs[j];
		//	���O�p�s�񕔕��ɂ��Ă̂ݎ��s
		//	j==1:k=0, j==2:k=0,1, j==3:k=0,1,2

		for(unsigned k = 0; k < j; k++){
			int vtxid1 = mesh->tets[id].vertexIDs[k];
			for(unsigned l =0; l < mesh->vertices[vtxid0].edgeIDs.size(); l++){
				for(unsigned m =0; m < mesh->vertices[vtxid1].edgeIDs.size(); m++){
					if(mesh->vertices[vtxid0].edgeIDs[l] == mesh->vertices[vtxid1].edgeIDs[m]){
						edgeVars[mesh->vertices[vtxid0].edgeIDs[l]].k += matk[j][k];		//�������̂������͂������甼���ɂ���B��O�p�����O�p������������ɂ́A�ǂ�����for�����ɂ���Ηǂ��̂��H
						//DSTR << edges[vertices[vtxid0].edges[l]].k << std::endl;
#ifdef DumK
						edgeVars[mesh->vertices[vtxid0].edgeIDs[l]].k = 0.0;
#endif DumK
					}
				}
			}
		}
	}
#endif

#ifdef UseMatAll
	//SciLab�Ŏg�����߂ɁA�S�̍����s������
	//matk������
	for(unsigned j=0; j<4 ; j++){
		for(unsigned k=0; k<4 ;k++){
			matKAll[mesh->tets[id].vertexIDs[j]][mesh->tets[id].vertexIDs[k]] += matk[j][k];
		}
	}
#endif UseMatAll

#ifdef DumK
	//SciLab�Ŏg�����߂ɁA�S�̍����s������
	//matk������
	for(unsigned j=0; j<4 ; j++){
		for(unsigned k=0; k<4 ;k++){
			matKAll[tets.vertices[j]][tets.vertices[k]] = 0.0;;
		}
	}
#endif

#ifdef UseGaussSeidel
	//�Ίp������Ίp�����̑S�̍����s�񂩂甲���o����1�~n�̍s��ɑ������
	//j=0~4�܂ő��(��̃��[�v�ł́Aj�͑Ίp�����͈̔͂����Ȃ��̂ŁA�l������Ȃ��������o�Ă��܂�)
	for(unsigned j =0;j<4;j++){
		dMatKAll[0][mesh->tets[id].vertexIDs[j]] += matk[j][j];
		int hoge4 =0;
	}
#endif

#ifdef DumK
	for(unsigned j=0;j<4;j++){
		dMatKAll[0][mesh->tets[id].vertexID[j]] = 0.0;
		int hogeshi =0;
	} 
#endif DumK
	
//#ifdef UseMatAll
//#ifdef UseGaussSeidel
//	for(unsigned j =0;j<mesh->vertices.size();j++){
//		if(matKAll[j][j] != dMatKAll[0][j]){
//			DSTR << j <<" �����̗v�f�͂��������I�������K�v�ł���B matKAll�F" << matKAll[j][j] << " dMatKAll�F" << dMatKAll[0][j] <<std::endl;
//		}
//	}
//#endif
//#endif UseMatAll

}

void PHFemThermo::CreateMatkLocal(unsigned id){
	PHFemMeshNew* mesh = GetPHFemMesh();

	//	�l�ʑ̗v�f�ɂ��ČW���s��[k]�����			�����K��	
	//..	k1�����	k1k�ł��Ak1b�ł��ǂ���ł��\��Ȃ�	�ǂ��炪���������ׂ�
	///....		�ό`������������������
	if(deformed){	CreateMatk1k(id);}			//  K��ꍀ�i���b�V�����M�`���j�̍s��쐬	//k���_�������ɁA���M���āA�`��֐��𓱏o	
	//if(deformed){	CreateMatk1b(id);}			//	���Ђ̗��_�������ɁA������p���Č`��֐��𓱏o
	
	//....		�M�`�B���E�����ɕK�v��k2
	CreateMatk2t(id);					///	K2: �M �`�B ���E����
	CreateMatk3t(id);					///	K3: �M �t�� ���E����(��C�ւ̔M�`�B��)
	//k1,k2,k3�����Z����(�g���Ă��鐔�l����)
	matk = tetVars[id].matk[0] + tetVars[id].matk[1] + tetVars[id].matk[2];	
	//<����>	matk[0]:K1, matk[1]:K2, matk[2]:K3
	matkcheck << id << std::endl;
	matkcheck << tetVars[id].matk[2] << std::endl;
	//if(id == 0){
	//	DSTR << "Initial Create matk" << std::endl;
	//	DSTR << matk << std::endl;
	//}
	//DSTR << "tets[id].matk[0]" << std::endl;
	//DSTR << tets[id].matk[0] << std::endl;
	//DSTR << "tets[id].matk[1]" << std::endl;
	//DSTR << tets[id].matk[1] << std::endl;
	//DSTR << "tets[id].matk[2]" << std::endl;
	//DSTR << tets[id].matk[2] << std::endl;

#ifdef UseGaussSeidel
	//	(�K�E�X�U�C�f�����g�����v�Z��)�v�f���ɍ�����s��̐������A�G�b�W�ɌW�����i�[����
	//	or	(�K�E�X�U�C�f�����g��Ȃ��v�Z��)�v�f���Ƃ̌v�Z���I��邽�тɁA�v�f�����s��̐����������G�b�W��_�ɍ��ϐ��Ɋi�[���Ă���	#ifedef�Ń��[�h����āA�ǂ�����ł���悤�ɂ��Ă����Ă��ǂ�����w
	//	Edges �̂��̗v�f�Ōv�Z����K�s��̐�����k�ɌW���Ƃ��Ċi�[����
		
	//matk�̑Ίp�����ȊO�ŁA���O�p�̕����̒l���Aedge��k�ɑ������
	//
	//DSTR << i <<"th tetrahedra element'edges[vertices[vtxid0].edges[l]].k (All edges.k' value): " << std::endl;
	for(unsigned j=1; j < 4; j++){
		int vtxid0 = mesh->tets[id].vertexIDs[j];
		//	���O�p�s�񕔕��ɂ��Ă̂ݎ��s
		//	j==1:k=0, j==2:k=0,1, j==3:k=0,1,2
		for(unsigned k = 0; k < j; k++){
			int vtxid1 = mesh->tets[id].vertexIDs[k];
			for(unsigned l =0; l < mesh->vertices[vtxid0].edgeIDs.size(); l++){
				for(unsigned m =0; m < mesh->vertices[vtxid1].edgeIDs.size(); m++){
					if(mesh->vertices[vtxid0].edgeIDs[l] == mesh->vertices[vtxid1].edgeIDs[m]){
						edgeVars[mesh->vertices[vtxid0].edgeIDs[l]].k += matk[j][k];		//�������̂������͂������甼���ɂ���B��O�p�����O�p������������ɂ́A�ǂ�����for�����ɂ���Ηǂ��̂��H
						//DSTR << edges[vertices[vtxid0].edges[l]].k << std::endl;
#ifdef DumK
						edgeVars[mesh->vertices[vtxid0].edgeIDs[l]].k = 0.0;
#endif DumK
					}
				}
			}
		}
	}
#endif

#ifdef UseMatAll
	//SciLab�Ŏg�����߂ɁA�S�̍����s������
	//matk������
	for(unsigned j=0; j<4 ; j++){
		for(unsigned k=0; k<4 ;k++){
			matKAll[mesh->tets[id].vertexIDs[j]][mesh->tets[id].vertexIDs[k]] += matk[j][k];
		}
	}

	//if(strcmp(mesh->GetName(), "femNsteak") == 0){
	//	DSTR << "matkall" << std::endl;
	//	for(unsigned j=0; j < mesh->vertices.size(); j++){
	//		for(unsigned k=0; k < mesh->vertices.size(); k++){
	//			DSTR << std::setw(13) << matKAll[j][k] << "\t";
	//		}
	//		DSTR << std::endl;
	//	}
	//	DSTR << std::endl;
	//}

	//matKAll�̔�Ίp�����̂������̂��̂̕����𔽓]���A�s�̒l�̍��v���ω����Ȃ��悤�ɑΊp�����̒l�Œ��߂���
	if(strcmp(mesh->GetName(), "femNsteak") == 0){
		//matKModif();
	}

	////edges�ɓ������W��������
	//for(unsigned j=1; j < 4; j++){
	//	int vtxid0 = tets[i].vertices[j];
	//	//	���O�p�s�񕔕��ɂ��Ă̂ݎ��s
	//	//	j==1:k=0, j==2:k=0,1, j==3:k=0,1,2
	//	for(unsigned k = 0; k < j; k++){
	//		int vtxid1 = tets[i].vertices[k];
	//			for(unsigned l =0; l < vertices[vtxid0].edges.size(); l++){
	//				for(unsigned m =0; m < vertices[vtxid1].edges.size(); m++){
	//					if(vertices[vtxid0].edges[l] == vertices[vtxid1].edges[m]){
	//						edges[vertices[vtxid0].edges[l]].k += matk[j][k];		//�������̂������͂������甼���ɂ���B��O�p�����O�p������������ɂ́A�ǂ�����for�����ɂ���Ηǂ��̂��H
	//						//DSTR << edges[vertices[vtxid0].edges[l]].k << std::endl;
	//					}
	//				}
	//			}
	//	}
	//}

#endif UseMatAll

#ifdef DumK
	//SciLab�Ŏg�����߂ɁA�S�̍����s������
	//matk������
	for(unsigned j=0; j<4 ; j++){
		for(unsigned k=0; k<4 ;k++){
			matKAll[tets.vertices[j]][tets.vertices[k]] = 0.0;;
		}
	}
#endif

#ifdef UseGaussSeidel
	//�Ίp������Ίp�����̑S�̍����s�񂩂甲���o����1�~n�̍s��ɑ������
	//j=0~4�܂ő��(��̃��[�v�ł́Aj�͑Ίp�����͈̔͂����Ȃ��̂ŁA�l������Ȃ��������o�Ă��܂�)
	for(unsigned j =0;j<4;j++){
		dMatKAll[0][mesh->tets[id].vertexIDs[j]] += matk[j][j];
		//DSTR << "matk[" << j << "][" << j << "] : " << matk[j][j] << std::endl;
		//DSTR << "dMatKAll[0][" << tets[i].vertices[j] << "] : " << dMatKAll[0][tets[i].vertices[j]] << std::endl;
		int hoge4 =0;
	}
#endif
	//DSTR << std::endl;	//���s

	//std::ofstream matKAll("matKAll.csv");
	//for(){
	//	matKAll
	//	}


#ifdef DumK
	for(unsigned j=0;j<4;j++){
		dMatKAll[0][tets.vertices[j]] = 0.0;
		int hogeshi =0;
	} 
#endif DumK

	
	//	for debug
	//�v�f25��0~3�Ԗڂ̐ߓ_�������\������
	//if(i == 25){
	//		for(unsigned n=0;n < 4;n++){
	//			DSTR << n << " : " << tets[25].vertices[n] << std::endl;	//�v�f25��0�Ԗڂ̐ߓ_��63�ł���B
	//		}
	//}
	//�ߓ_�ԍ���63�̓_���ǂ̗v�f�ɓ����Ă���̂��𒲂ׂ��25,57������
	//for(unsigned j=0;j < vertices[63].tets.size();j++){
	//	DSTR << vertices[63].tets[j] <<std::endl;
	//}
		
	//	���ׂ�
	//dMatKAll�̐����̂����A0�ƂȂ�v�f����������A�G���[���H�@�����Ă��Ȃ������������Ă��A���Ȃ��C������
	// try catch���ɂ���
//	for(unsigned j = 0; j < vertices.size() ; j++){
//		if(dMatKAll[0][j] ==0.0){
//			DSTR << "Creating dMatKAll error!? : dMatKAll[0][" << j << "] == 0.0 " << std::endl;
////			DSTR << "If " << j <<" 's blank eroors didn't banished until display simulation scene, I recommened Source Code Check!" <<std::endl;  
//		}
//	}

	//DSTR << "matKAll : " << matKAll <<std::endl;
	//for(unsigned i=0;i<vertices.size();i++){
	//	for(unsigned j=0;j <  vertices.size();j++){
	//		if(matKAll[i][j] !=0){
	//			DSTR << "matKAll[" << i <<"][" << j << "]: " << matKAll[i][j] <<std::endl;
	//		}
	//	}
	//}
	//DSTR << "dMatKAll : " <<dMatKAll << std::endl;
//#ifdef UseMatAll
//#ifdef UseGaussSeidel
//	for(unsigned j =0;j<mesh->vertices.size();j++){
//		if(matKAll[j][j] != dMatKAll[0][j]){
//			DSTR << j <<" �����̗v�f�͂��������I�������K�v�ł���B matKAll�F" << matKAll[j][j] << " dMatKAll�F" << dMatKAll[0][j] <<std::endl;
//		}
//	}
//#endif
//#endif UseMatAll
	int hoge5 =0;

}

void PHFemThermo::CreateMatk1b(unsigned id){
	PHFemMeshNew* mesh = GetPHFemMesh();

	//yagawa1983����Ƀm�[�g�Ɏ��W�J�����v�Z��
	unsigned i=0;
	unsigned j=0;
	unsigned k=0;
	unsigned l=0;

	//a_1~a_4, ... , c_4	���쐬
	//�W��(����)�~�s�񎮂̓��ꕨ
	double a[4];
	double b[4];
	double c[4];

	//�v�f���\������ߓ_�̍��W�̓��ꕨ
	double x[4];
	double y[4];
	double z[4];
	//�v�f����4�ߓ_��x,y,z���W���i�[
	for(unsigned m=0; m < 4;m++){
		x[m] = mesh->vertices[mesh->tets[id].vertexIDs[m]].pos.x;
		y[m] = mesh->vertices[mesh->tets[id].vertexIDs[m]].pos.y;
		z[m] = mesh->vertices[mesh->tets[id].vertexIDs[m]].pos.z;
	}

	//mata~matc
	//a[i]
	for(unsigned i =0;i<4;i++){
		double fugou =0.0;				// (-1)^i �̕����̒�`

		//	fugou �̕�������
		if(i == 0 || i == 2){		//0,2�̎��A(-1)^1,3 = -1
			fugou = -1.0;
		}
		else{					//1,3�̎��A(-1)^0,2 = 1
			fugou = 1.0;
		}
		
		//i,j,k,l�̊֌W�Z�b�g�˔z��̗v�f�ɂ��Ă����������Bi[4],if(i[0]=0){i[1](=j)=1, i[2](=k)=2, i[3](=l)=3}	if(i[0]=1){i[1](=j)=2, i[2](=k)=3, i[3](=l)=0}
		if(i==0){		j=1;	k=2;	l=3;	}
		else if(i==1){	j=2;	k=3;	l=0;	}
		else if(i==2){	j=3;	k=0;	l=1;	}
		else if(i==3){	j=0;	k=1;	l=2;	}

		// a_i�̍쐬
		for(unsigned m =0;m<3;m++){						//	1�̐����ւ̑���͂���
			mata[m][0] = 1.0;
		}
		mata[0][1] = y[j];
		mata[1][1] = y[k];
		mata[2][1] = y[l];

		mata[0][2] = z[j];
		mata[1][2] = z[k];
		mata[2][2] = z[l];

		a[i] = fugou * mata.det();

		//DSTR << "mata : " <<std::endl;
		//DSTR << mata << std::endl;

		//DSTR << "mata.det() : " <<std::endl;
		//DSTR << mata.det() << std::endl;

		//DSTR << "a[" << i << "] : " <<std::endl;
		//DSTR << a[i] << std::endl;


		// b_i�̍쐬
		matb[0][0]=x[j];
		matb[1][0]=x[k];
		matb[2][0]=x[l];

		for(unsigned m =0;m<3;m++){						//	1�̐����ւ̑���͂���
			matb[m][1] = 1.0;
		}

		matb[0][2]=z[j];
		matb[1][2]=z[k];
		matb[2][2]=z[l];

		b[i] = fugou * matb.det();

		//DSTR << "matb : " <<std::endl;
		//DSTR << matb << std::endl;
		//DSTR << "matb.det() : " <<std::endl;
		//DSTR << matb.det() << std::endl;

		//DSTR << "b[" << i << "] : " <<std::endl;
		//DSTR << b[i] << std::endl;

		// c_i�̍쐬
		matcc[0][0]=x[j];
		matcc[1][0]=x[k];
		matcc[2][0]=x[l];

		matcc[0][1]=y[j];
		matcc[1][1]=y[k];
		matcc[2][1]=y[l];

		for(unsigned m =0;m<3;m++){						//	1�̐����ւ̑���͂���
			matcc[m][2] = 1.0;
		}

		//DSTR << "matcc : " <<std::endl;
		//DSTR << matcc << std::endl;
		//DSTR << "matcc.det() : " <<std::endl;
		//DSTR << matcc.det() << std::endl;

		c[i] = fugou * matcc.det();
		
		//	for debug�@�v�f���Ƃ�a_i~c_i�̎Z�o
		//DSTR << "a["<< i << "] : " << a[i] << std::endl;
		//DSTR << "b["<< i << "] : " << b[i] << std::endl;
		//DSTR << "c["<< i << "] : " << c[i] << std::endl;
		//DSTR << std::endl;
		int debughogeshi =0;
	}
	
	//	matk1�̐�����a_i ~ c_i�̑���������	���ꂢ�����邪�A���̃R�[�h�ŗǂ��I	�Ίp��������Ίp�������A�S���A���̃R�[�h
	//	���P��		���O�p�ƑΊp���������A�v�Z���A��O�p�͉��O�p�����ł��悢�B
	for(unsigned i =0;i<4;i++){
		for(unsigned j =0;j<4;j++){
			tetVars[id].matk[0][i][j] = a[i] * a[j] +b[i] * b[j] + c[i] * c[j];
		}
	}

	////	��L���R�X�g�̏��Ȃ��R�[�h?
	//matk[0][0] = a[0] * a[0] +b[0] * b[0] + c[0] * c[0];
	//matk[1][1] = a[1] * a[1] +b[1] * b[1] + c[1] * c[1];
	//matk[2][2] = a[2] * a[2] +b[2] * b[2] + c[2] * c[2];
	//matk[3][3] = a[3] * a[3] +b[3] * b[3] + c[3] * c[3];

	//matk[0][1] = a[0] * a[1] + b[0] * b[1] + c[0] * c[1];
	//matk[1][0] = matk[0][1];

	//matk[0][2] = a[0] * a[2] + b[0] * b[2] + c[0] * c[2];
	//matk[2][0] = matk[0][2];

	//matk[0][3] = a[0] * a[3] + b[0] * b[3] + c[0] * c[3];
	//matk[3][0] = matk[0][3];

	//	for DEBUG
	//DSTR << "matk1 : " << std::endl;
	//DSTR << matk1 << std::endl;
	//int debughogeshi2 =0;
	
	//�W���̐�
//	tets[id].matk[0]= thConduct / (36 *  CalcTetrahedraVolume(tets[id])) * tets[id].matk[0];		//���_���Ԉ���Ă����̂ŁA�C��
	tetVars[id].matk[0]= thConduct / (36 *  tetVars[id].volume) * tetVars[id].matk[0];		//���_���Ԉ���Ă����̂ŁA�C��

	//	for DEBUG
	//DSTR << "�W���ό�� matk1 : " << std::endl;
	//DSTR << matk1 << std::endl;
	int debughogeshi3 =0;

	DSTR << "Inner Function matk1b _ tets["<< id << "].matk[0] "<< tetVars[id].matk[0] << std::endl;  
	//a~c�̑�������K1�ɑ��
	//matk1(4x4)�ɑ��

}

void PHFemThermo::CreateMatk1k(unsigned id){
	PHFemMeshNew* mesh = GetPHFemMesh();

	//���̌v�Z���Ăяo���Ƃ��ɁA�e�l�ʑ̂��ƂɌv�Z���邽�߁A�l�ʑ̂�0�Ԃ��珇�ɂ��̌v�Z���s��
	//�l�ʑ̂��\������4�ߓ_��ߓ_�̔z��(Tets�ɂ́A�ߓ_�̔z�񂪍���Ă���)�ɓ����Ă��鏇�Ԃ��g���āA�ʂ̌v�Z���s������A�s��̌v�Z���s�����肷��B
	//���̂��߁A���̊֐��̈����ɁA�l�ʑ̗v�f�̔ԍ������

	//�Ō�ɓ����s���������
	tetVars[id].matk[0].clear();
	//for(unsigned i =0; i < 4 ;i++){
	//	for(unsigned j =0; j < 4 ;j++){
	//		//tets[id].matk1[i][j] = 0.0;
	//		tets[id].matk[0][i][j] = 0.0;
	//	}
	//}

	//	A�s��@=	a11 a12 a13
	//				a21 a22 a23
	//				a31 a32 a33
	//�𐶐�
	PTM::TMatrixRow<4,4,double> matk1A;
	FemVertex p[4];
	for(unsigned i = 0; i< 4 ; i++){
		p[i]= mesh->vertices[mesh->tets[id].vertexIDs[i]];
	}
	
	matk1A[0][0] = (p[2].pos.y - p[0].pos.y) * (p[3].pos.z - p[0].pos.z) - (p[2].pos.z - p[0].pos.z) * (p[3].pos.y - p[0].pos.y);
	matk1A[0][1] = (p[1].pos.z - p[0].pos.z) * (p[3].pos.y - p[0].pos.y) - (p[1].pos.y - p[0].pos.y) * (p[3].pos.z - p[0].pos.z);
	matk1A[0][2] = (p[1].pos.y - p[0].pos.y) * (p[2].pos.z - p[0].pos.z) - (p[1].pos.z - p[0].pos.z) * (p[2].pos.y - p[0].pos.y);

	matk1A[1][0] = (p[2].pos.z - p[0].pos.z) * (p[3].pos.x - p[0].pos.x) - (p[2].pos.x - p[0].pos.x) * (p[3].pos.z - p[0].pos.z);
	matk1A[1][1] = (p[1].pos.x - p[0].pos.x) * (p[3].pos.z - p[0].pos.z) - (p[1].pos.z - p[0].pos.z) * (p[3].pos.x - p[0].pos.x);
	matk1A[1][2] = (p[1].pos.z - p[0].pos.z) * (p[2].pos.x - p[0].pos.x) - (p[1].pos.x - p[0].pos.x) * (p[2].pos.z - p[0].pos.z);

	matk1A[2][0] = (p[2].pos.x - p[0].pos.x) * (p[3].pos.y - p[0].pos.y) - (p[2].pos.y - p[0].pos.y) * (p[3].pos.x - p[0].pos.x);
	matk1A[2][1] = (p[1].pos.y - p[0].pos.y) * (p[3].pos.x - p[0].pos.x) - (p[1].pos.x - p[0].pos.x) * (p[3].pos.y - p[0].pos.y);
	matk1A[2][2] = (p[1].pos.x - p[0].pos.x) * (p[2].pos.y - p[0].pos.y) - (p[1].pos.y - p[0].pos.y) * (p[2].pos.x - p[0].pos.x);

	//	for	debug
	//DSTR << matk1A[0][0] << std::endl;
	//DSTR << "matk1A : " << matk1A << std::endl; 
	//int hogeshi =0;

	//a11 ~ a33 ���s��ɓ���āA[N~T] [N] ���v�Z������
	
	PTM::TMatrixRow<1,4,double> Nx;
	PTM::TMatrixRow<1,4,double> Ny;
	PTM::TMatrixRow<1,4,double> Nz;

	Nx[0][0] = -matk1A[0][0] - matk1A[0][1] -matk1A[0][2];
	Nx[0][1] = matk1A[0][0];
	Nx[0][2] = matk1A[0][1];
	Nx[0][3] = matk1A[0][2];

	Ny[0][0] = -matk1A[1][0] - matk1A[1][1] -matk1A[1][2];
	Ny[0][1] = matk1A[1][0];
	Ny[0][2] = matk1A[1][1];
	Ny[0][3] = matk1A[1][2];

	Nz[0][0] = -matk1A[2][0] - matk1A[2][1] -matk1A[2][2];
	Nz[0][1] = matk1A[2][0];
	Nz[0][2] = matk1A[2][1];
	Nz[0][3] = matk1A[2][2];

	//	Km �̎Z�o
	//tets[id].matk1 = Nx.trans() * Nx + Ny.trans() * Ny + Nz.trans() * Nz;
#if 0
	tets[id].matk[0] = Nx.trans() * Nx + Ny.trans() * Ny + Nz.trans() * Nz;
#else
	//	�ٕ����M�`�����ɑΉ�
	//DSTR << "thConduct_x:" << thConduct_x <<"," << "thConduct_y:" << thConduct_y <<"," << "thConduct_z:" << thConduct_z <<std::endl;
	tetVars[id].matk[0] = thConduct_x * Nx.trans() * Nx + thConduct_y * Ny.trans() * Ny + thConduct_z * Nz.trans() * Nz;
#endif

	////Nx,Ny,Nz���o��
	//if(strcmp(mesh->GetName(), "femNsteak") == 0){
	//	PTM::TMatrixRow<4,4,double> NxtNx;
	//	PTM::TMatrixRow<4,4,double> NytNy;
	//	PTM::TMatrixRow<4,4,double> NztNz;
	//	NxtNx=Nx.trans() * Nx;
	//	NytNy=Ny.trans() * Ny;
	//	NztNz=Nz.trans() * Nz;

	//	DSTR << id << std::endl;
	//	for(unsigned i=0; i < 3; i++){
	//		if(i == 0){
	//			DSTR << "Nx=" << "\t";
	//		}else if(i == 1){
	//			DSTR << "Ny=" << "\t";
	//		}else if(i == 2){
	//			DSTR << "Nz=" << "\t";
	//		}
	//		for(unsigned j=0; j < 4; j++){
	//			if(i == 0){
	//				DSTR << Nx[0][j] << "\t";
	//			}else if(i == 1){
	//				DSTR << Ny[0][j] << "\t";
	//			}else if(i == 2){
	//				DSTR << Nz[0][j] << "\t";
	//			}
	//		}
	//		DSTR << std::endl;
	//		if(i == 0){
	//			DSTR << "Nxt*Nx=" << std::endl;
	//		}else if(i == 1){
	//			DSTR << "Nyt*Ny=" << std::endl;
	//		}else if(i == 2){
	//			DSTR << "Nzt*Nz=" << std::endl;
	//		}
	//		for(unsigned k=0; k < 4; k++){
	//			for(unsigned l=0; l < 4; l++){
	//				if(i == 0){
	//					DSTR << std::setw(13) << NxtNx[k][l] << "\t";
	//				}else if(i == 1){
	//					DSTR << std::setw(13) << NytNy[k][l] << "\t";
	//				}else if(i == 2){
	//					DSTR << std::setw(13) << NztNz[k][l] << "\t";
	//				}
	//			}
	//			DSTR << std::endl;
	//		}
	//		DSTR << std::endl;
	//	}

	//	DSTR << "matklocal" << std::endl;
	//	for(unsigned i=0; i < 4; i++){
	//		for(unsigned j=0; j < 4; j++){
	//			DSTR << std::setw(13) << tetVars[id].matk[0][i][j] << "\t";
	//		}
	//		DSTR << std::endl;
	//	}
	//	DSTR << std::endl;
	//}

	////	for debug
	//DSTR << "Nx : " << Nx << std::endl;
	//DSTR << "Nx^T : " << Nx.trans() << std::endl;
	//DSTR << "Nx^T * Nx : " << Nx.trans() * Nx << std::endl;
	//DSTR << "Ny^T * Ny : " << Ny.trans() * Ny << std::endl;
	//DSTR << "Nz^T * Nz : " << Nz.trans() * Nz << std::endl;
	//DSTR << "matk1 : " << tets[id].matk[0] << std::endl;
	//int hogehoge =0;

	//	for	DEBUG
	//DSTR << "matk1 : " << matk1 << std::endl;

	//K1
//	matk1 = thConduct / (36 * CalcTetrahedraVolume(tets) ) * matk1;
	
	//tets[id].matk1 = thConduct / (36 * CalcTetrahedraVolume(tets[id]) ) * tets[id].matk1;
	//tets[id].matk[0] = thConduct / (36 * CalcTetrahedraVolume(tets[id]) ) * tets[id].matk[0];
#if 0
	tets[id].matk[0] = thConduct / (36 * tets[id].volume ) * tets[id].matk[0];
#else
	//tets[id].matk[0] =  1.0/ ( 36 * tets[id].volume ) * tets[id].matk[0];
	tetVars[id].matk[0] =  1.0 / ( 36.0 * tetVars[id].volume ) * tetVars[id].matk[0];
	//tets[id].matk[0] *=  1.0 / ( 36.0 * tets[id].volume )
#endif
	//DSTR << "Inner Function _tets[id].matk1 : " << tets[id].matk1 << std::endl;

}

PTM::TMatrixRow<4,4,double> PHFemThermo::GetKMatInTet(unsigned id){
	//for(unsigned id=0;id<tets.size();++id){
	return tetVars[id].matk[0];
	//}
}

///*���쌴�ǉ���������--------------------------------------------*/
/////���_�̒S������ʐς̌v�Z��\�߂��Ă����B�i���̂����̂Ȃ̂Ń��b�V�����͕ω����Ȃ��O��j
void PHFemThermo::calcVerticesArea(){
	PHFemMeshNew* mesh = GetPHFemMesh();

	for(unsigned i=0; i<mesh->surfaceVertices.size(); i++){
		vertexVars[mesh->surfaceVertices[i]].area = 0; //��������߂�
		for(unsigned j=0; j<mesh->vertices[mesh->surfaceVertices[i]].faceIDs.size(); ++j){
			if(mesh->vertices[mesh->surfaceVertices[i]].faceIDs[j] < (int) mesh->nSurfaceFace){ //����face���\�ʂ̖ʂł�������
				unsigned faceId = mesh->vertices[mesh->surfaceVertices[i]].faceIDs[j];
				vertexVars[mesh->surfaceVertices[i]].area += faceVars[faceId].area / 3 ;
			}
		}
	}
}
///*���쌴�ǉ������܂�--------------------------------------------*/

void PHFemThermo::CreateVecf2surface(unsigned id,unsigned num){
	PHFemMeshNew* mesh = GetPHFemMesh();

	// ������
	tetVars[id].vecf[1].clear();
	//l=0�̎�f21,1�̎�:f22, 2�̎�:f23, 3�̎�:f24	�𐶐�
	///	..j�Ԗڂ̍s��̐�����0�̃x�N�g�����쐬
	for(unsigned l= 0 ; l < 4; l++){
		vecf2array[l] = Create41Vec1();
		vecf2array[l][l] = 0.0;			//	l�s��0��
	}
	for(unsigned l= 0 ; l < 4; l++){
		///	�l�ʑ̂̊e��(l = 0 �` 3) �ɂ��ă��b�V���\�ʂ��ǂ������`�F�b�N����B�\�ʂȂ�A�s��������vecf2array�ɓ����
		//faces[tets.faces[i]].sorted;		/// 1,24,58�݂����Ȑߓ_�ԍ��������Ă���
		///	..�s��^�̓��ꕨ��p��
		//faces[tets.faces[l]].vertices;
		if(mesh->tets[id].faceIDs[l] < (int)mesh->nSurfaceFace && faceVars[mesh->tets[id].faceIDs[l]].fluxarea > 0 ){			///	�O�k�̖� ���� �M�`�B�����X�V���ꂽ�� matk2���X�V����K�v������
			///	�l�ʑ̂̎O�p�`�̖ʐς��v�Z		///	���̊֐��̊O�Ŗʐϕ��̖ʐόv�Z����������B�ړ�����
			if(faceVars[mesh->tets[id].faceIDs[l]].area ==0 || faceVars[mesh->tets[id].faceIDs[l]].deformed ){		///	�ʐς��v�Z����Ă��Ȃ����i�͂��߁j or deformed(�ό`�������E�������)��true�̎�		///	�����̒ǉ�	�ʐς�0�� ||(OR) �����X�V���ꂽ��
				faceVars[mesh->tets[id].faceIDs[l]].area = CalcTriangleArea(mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[0], mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[1], mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[2]);
				faceVars[mesh->tets[id].faceIDs[l]].deformed = false;
			}
			///	�v�Z���ʂ��s��ɑ��
			///	area�̌v�Z�Ɏg���Ă��Ȃ��_�������Ă���s���������x�N�g���̐ς��Ƃ�
			///	�ϕ��v�Z�����{����l����
			unsigned vtx = mesh->tets[id].vertexIDs[0] + mesh->tets[id].vertexIDs[1] + mesh->tets[id].vertexIDs[2] + mesh->tets[id].vertexIDs[3];			
			///	area�v�Z�Ɏg���Ă��Ȃ��ߓ_ID�FID
			unsigned ID = vtx -( mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[0] + mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[1] + mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[2] );
			for(unsigned j=0;j<4;j++){
				if(mesh->tets[id].vertexIDs[j] == ID){					///	�`��֐����P�A�i���Ȃ킿�j����face�ɑΖʂ��钸�_�@�ƈ�v������@���̎���face�Ŗʐϕ�����
					///	�O�k�ɂȂ����b�V���ʂ̖ʐς�0�ŏ��������Ă���
					///	�ȉ���[]�͏�܂ł�[l]�ƈقȂ�B
					///	ID�����Ԗڂ��ɂ���āA�`��֐��̌W�����قȂ�̂ŁA
					for(unsigned i =0;i<3;++i){
						tetVars[id].vecf[1] += faceVars[mesh->tets[id].faceIDs[l]].heatflux[num][i] * (1.0/3.0) * faceVars[mesh->tets[id].faceIDs[l]].area * vecf2array[j];
					}
//					DSTR << vecf2array[j] << std::endl;
					//DSTR << "tets[id].matk2��faces[tets[id].faces[l]].heatTransRatio * (1.0/12.0) * faces[tets[id].faces[l]].area * matk2array[" << j << "]"<< "�����Z: " <<faces[tets[id].faces[l]].heatTransRatio * (1.0/12.0) * faces[tets[id].faces[l]].area * matk2array[j] << std::endl;
					//DSTR << "tets[id].matk2 +=  " << tets[id].matk2 << std::endl;
				}
				//else{
				//	///	ID�ƈ�v���Ȃ��ꍇ�ɂ́Amatk2array[j]�ɂ͑S����0������
				//	///	�Ƃ������Ƃ��낾���A
				//	//matk2array[j] =0.0 * matk2array[j];
				//	//DSTR << "matk2array[" << j << "]: " << matk2array[j] << std::endl;
				//}
			}
		}
		///	SurfaceFace����Ȃ�������Amatk2array�ɂ�0������
		//else{
		//	//matk2array[l];
		//}
	}
}

void PHFemThermo::CreateVecf2surface(unsigned id){
	PHFemMeshNew* mesh = GetPHFemMesh();

	// ������
	tetVars[id].vecf[1].clear();
	//l=0�̎�f21,1�̎�:f22, 2�̎�:f23, 3�̎�:f24	�𐶐�
	///	..j�Ԗڂ̍s��̐�����0�̃x�N�g�����쐬
	for(unsigned l= 0 ; l < 4; l++){
		vecf2array[l] = Create41Vec1();
		vecf2array[l][l] = 0.0;			//	l�s��0��
	}
	for(unsigned l= 0 ; l < 4; l++){
		///	�l�ʑ̂̊e��(l = 0 �` 3) �ɂ��ă��b�V���\�ʂ��ǂ������`�F�b�N����B�\�ʂȂ�A�s��������vecf2array�ɓ����
		//faces[tets.faces[i]].sorted;		/// 1,24,58�݂����Ȑߓ_�ԍ��������Ă���
		///	..�s��^�̓��ꕨ��p��
		//faces[tets.faces[l]].vertices;
		if(mesh->tets[id].faceIDs[l] < (int)mesh->nSurfaceFace && faceVars[mesh->tets[id].faceIDs[l]].fluxarea > 0 ){			///	�O�k�̖� ���� �M�`�B�����X�V���ꂽ�� matk2���X�V����K�v������
			///	�l�ʑ̂̎O�p�`�̖ʐς��v�Z		///	���̊֐��̊O�Ŗʐϕ��̖ʐόv�Z����������B�ړ�����
			if(faceVars[mesh->tets[id].faceIDs[l]].area ==0 || faceVars[mesh->tets[id].faceIDs[l]].deformed ){		///	�ʐς��v�Z����Ă��Ȃ����i�͂��߁j or deformed(�ό`�������E�������)��true�̎�		///	�����̒ǉ�	�ʐς�0�� ||(OR) �����X�V���ꂽ��
				faceVars[mesh->tets[id].faceIDs[l]].area = CalcTriangleArea(mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[0], mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[1], mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[2]);
				faceVars[mesh->tets[id].faceIDs[l]].deformed = false;
			}
			///	�v�Z���ʂ��s��ɑ��
			///	area�̌v�Z�Ɏg���Ă��Ȃ��_�������Ă���s���������x�N�g���̐ς��Ƃ�
			unsigned vtx = mesh->tets[id].vertexIDs[0] + mesh->tets[id].vertexIDs[1] + mesh->tets[id].vertexIDs[2] + mesh->tets[id].vertexIDs[3];			
			///	area�v�Z�Ɏg���Ă��Ȃ��ߓ_ID�FID
			unsigned ID = vtx -( mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[0] + mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[1] + mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[2] );
			for(unsigned j=0;j<4;j++){
				if(mesh->tets[id].vertexIDs[j] == ID){					///	�`��֐����P�A�i���Ȃ킿�j����face�ɑΖʂ��钸�_�@�ƈ�v������@���̎���face�Ŗʐϕ�����
					///	�ȉ���[]�͏�܂ł�[l]�ƈقȂ�B
					///	ID�����Ԗڂ��ɂ���āA�`��֐��̌W�����قȂ�̂ŁA
					//	�����̔M�����ɑΉ�:i
#ifdef DISABLE_COIL
					tetVars[id].vecf[1] += faceVars[mesh->tets[id].faceIDs[l]].heatflux[1][3] * (1.0/3.0) * faceVars[mesh->tets[id].faceIDs[l]].fluxarea[3] * vecf2array[j];	//+=:j=0~3��vecf2array�����Z										
#else 
					for(unsigned i=0;i<3;++i){
						tets[id].vecf[1] += faces[tets[id].faces[l]].heatflux[1][i] * (1.0/3.0) * faces[tets[id].faces[l]].fluxarea[i] * vecf2array[j];	//+=:j=0~3��vecf2array�����Z
					}
#endif
				}
			}
		}

		//�Ȏq�͈͓��ɓ����Ă��Ȃ���΂��肤�遫����
		//else if(tets[id].faces[l] < (int)nSurfaceFace && faces[tets[id].faces[l]].fluxarea <= 0 ){
		//	DSTR << "faces[tets[id].faces[l]].fluxarea���������v�Z����Ă��܂���" <<std::endl;
		//}

		///	SurfaceFace����Ȃ�������Amatk2array�ɂ�0������
		//else{
		//	//matk2array[l];
		//}
	}

	// ���̌��Z�R�[�h
	//double m1=0.0;
	//double m2=0.0;
	//double m3=0.0;
	//m1 = (vertices[tets[id].vertices[1]].pos.y - vertices[tets[id].vertices[0]].pos.y) * (vertices[tets[id].vertices[3]].pos.z - vertices[tets[id].vertices[0]].pos.z)
	//	- (vertices[tets[id].vertices[1]].pos.z - vertices[tets[id].vertices[0]].pos.z) * (vertices[tets[id].vertices[3]].pos.y - vertices[tets[id].vertices[0]].pos.y);
	//m2 = (vertices[tets[id].vertices[3]].pos.x - vertices[tets[id].vertices[0]].pos.x) * (vertices[tets[id].vertices[1]].pos.z - vertices[tets[id].vertices[0]].pos.z)
	//	- (vertices[tets[id].vertices[1]].pos.x - vertices[tets[id].vertices[0]].pos.x) * (vertices[tets[id].vertices[3]].pos.z - vertices[tets[id].vertices[0]].pos.z);
	//m3 = (vertices[tets[id].vertices[1]].pos.x - vertices[tets[id].vertices[0]].pos.x) * (vertices[tets[id].vertices[3]].pos.y - vertices[tets[id].vertices[0]].pos.y)
	//	- (vertices[tets[id].vertices[3]].pos.x - vertices[tets[id].vertices[0]].pos.x) * (vertices[tets[id].vertices[1]].pos.y - vertices[tets[id].vertices[0]].pos.y);
	//double sankaku = 0.0;
	//sankaku = m1 * m1 + m2 * m2 + m3 * m3;
	//sankaku = sqrt(sankaku) * 1/2;
	//double menseki = CalcTriangleArea(tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[3]);
	//DSTR << "sankaku "<<sankaku << ", menseki " << menseki <<std::endl;
	//int debughensu0=0;
}

void PHFemThermo::CreateVecf3surface(unsigned id){
	PHFemMeshNew* mesh = GetPHFemMesh();

	// ������
	tetVars[id].vecf[2].clear();
	double Tc[4];
	double TcAll =0;
	for(unsigned l=0;l<4;++l){
		TcAll += vertexVars[mesh->tets[id].vertexIDs[l]].Tc;
	}
	//l=0�̎�f21,1�̎�:f22, 2�̎�:f23, 3�̎�:f24	�𐶐�
	///	..j�Ԗڂ̍s��̐�����0�̃x�N�g�����쐬
	for(unsigned l= 0 ; l < 4; l++){
		vecf3array[l] = Create41Vec1();
		vecf3array[l][l] = 0.0;			//	l�s��0��
		Tc[l] =  (TcAll - vertexVars[mesh->tets[id].vertexIDs[l]].Tc) * 1.0/3.0;
	}
	for(unsigned l= 0 ; l < 4; l++){
		///	�l�ʑ̂̊e��(l = 0 �` 3) �ɂ��ă��b�V���\�ʂ��ǂ������`�F�b�N����B�\�ʂȂ�A�s��������vecf2array�ɓ����
		//faces[tets.faces[i]].sorted;		/// 1,24,58�݂����Ȑߓ_�ԍ��������Ă���
		if(mesh->tets[id].faceIDs[l] < (int)mesh->nSurfaceFace && faceVars[mesh->tets[id].faceIDs[l]].heatTransRatios.size() ){		///	�O�k�̖ʂł̂ݎ��s && �i�P�ȏ�́j�M�`�B�����ݒ肳��Ă���Ƃ�
			///	�l�ʑ̂̎O�p�`�̖ʐς��v�Z		///	���̊֐��̊O�Ŗʐϕ��̖ʐόv�Z����������B�ړ�����
			if(faceVars[mesh->tets[id].faceIDs[l]].area ==0 || faceVars[mesh->tets[id].faceIDs[l]].deformed ){		///	�ʐς��v�Z����Ă��Ȃ����i�͂��߁j or deformed(�ό`�������E�������)��true�̎�		///	�����̒ǉ�	�ʐς�0�� ||(OR) �����X�V���ꂽ��
				faceVars[mesh->tets[id].faceIDs[l]].area = CalcTriangleArea(mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[0], mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[1], mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[2]);
				faceVars[mesh->tets[id].faceIDs[l]].deformed = false;
			}
			///	area�̌v�Z�Ɏg���Ă��Ȃ��_�������Ă���s���������x�N�g���̐ς��Ƃ�
			///	�ϕ��v�Z�����{����l����
			unsigned vtx = mesh->tets[id].vertexIDs[0] + mesh->tets[id].vertexIDs[1] + mesh->tets[id].vertexIDs[2] + mesh->tets[id].vertexIDs[3];			
			///	area�v�Z�Ɏg���Ă��Ȃ��ߓ_ID�FID
			unsigned ID = vtx -( mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[0] + mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[1] + mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[2] );
			for(unsigned j=0;j<4;j++){
				if(mesh->tets[id].vertexIDs[j] == ID){					///	�`��֐����P�A�i���Ȃ킿�j����face�ɑΖʂ��钸�_�@�ƈ�v������@���̎���face�Ŗʐϕ�����
					///	�ȉ���[]�͏�܂ł�[l]�ƈقȂ�B
					///	ID�����Ԗڂ��ɂ���āA�`��֐��̌W�����قȂ�̂ŁA
					for(unsigned k =0 ; k < faceVars[mesh->tets[id].faceIDs[l]].heatTransRatios.size(); ++k){
						tetVars[id].vecf[2] +=  faceVars[mesh->tets[id].faceIDs[l]].heatTransRatios[k] * (1.0/3.0) * faceVars[mesh->tets[id].faceIDs[l]].area * vecf3array[j];	//+=:j=0~3��vecf2array�����Z			
					}
				}
			}
		}
	}
}

void PHFemThermo::CreateVecf4surface(unsigned id){
	PHFemMeshNew* mesh = GetPHFemMesh();

	// ������
	tetVars[id].vecf[3].clear();
	//l=0�̎�f21,1�̎�:f22, 2�̎�:f23, 3�̎�:f24	�𐶐�
	///	..j�Ԗڂ̍s��̐�����0�̃x�N�g�����쐬
	double Tout[4];
	double ToutAll =0;
	for(unsigned l=0;l<4;++l){
		ToutAll += vertexVars[mesh->tets[id].vertexIDs[l]].Tout;
	}
	for(unsigned l= 0 ; l < 4; l++){
		vecf4array[l] = Create41Vec1();
		vecf4array[l][l] = 0.0;			//	l�s��0��
		Tout[l] =  (ToutAll - vertexVars[mesh->tets[id].vertexIDs[l]].Tout) * 1.0/3.0;
	}
	for(unsigned l= 0 ; l < 4; l++){
		///	�l�ʑ̂̊e��(l = 0 �` 3) �ɂ��ă��b�V���\�ʂ��ǂ������`�F�b�N����B�\�ʂȂ�A�s��������vecf2array�ɓ����
		//faces[tets.faces[i]].sorted;		/// 1,24,58�݂����Ȑߓ_�ԍ��������Ă���
		if(mesh->tets[id].faceIDs[l] < (int)mesh->nSurfaceFace){	// && faces[tets[id].faces[l]].thermalEmissivity != 0 ){		///	�O�k�̖ʂł̂ݎ��s && �i�P�ȏ�́j�M�`�B�����ݒ肳��Ă���Ƃ�
			///	�l�ʑ̂̎O�p�`�̖ʐς��v�Z		///	���̊֐��̊O�Ŗʐϕ��̖ʐόv�Z����������B�ړ�����
			if(faceVars[mesh->tets[id].faceIDs[l]].area ==0 || faceVars[mesh->tets[id].faceIDs[l]].deformed ){		///	�ʐς��v�Z����Ă��Ȃ����i�͂��߁j or deformed(�ό`�������E�������)��true�̎�		///	�����̒ǉ�	�ʐς�0�� ||(OR) �����X�V���ꂽ��
				faceVars[mesh->tets[id].faceIDs[l]].area = CalcTriangleArea(mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[0], mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[1], mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[2]);
				faceVars[mesh->tets[id].faceIDs[l]].deformed = false;
			}
			///	area�̌v�Z�Ɏg���Ă��Ȃ��_�������Ă���s���������x�N�g���̐ς��Ƃ�
			///	�ϕ��v�Z�����{����l����
			unsigned vtx = mesh->tets[id].vertexIDs[0] + mesh->tets[id].vertexIDs[1] + mesh->tets[id].vertexIDs[2] + mesh->tets[id].vertexIDs[3];			
			///	area�v�Z�Ɏg���Ă��Ȃ��ߓ_ID�FID
			unsigned ID = vtx - ( mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[0] + mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[1] + mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[2] );
			for(unsigned j=0;j<4;j++){
				if(mesh->tets[id].vertexIDs[j] == ID){					///	�`��֐����P�A�i���Ȃ킿�j����face�ɑΖʂ��钸�_�@�ƈ�v������@���̎���face�Ŗʐϕ�����
					double ems_temp = ( 	
						vertexVars[mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[0]].thermalEmissivity * vertexVars[mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[0]].temp + vertexVars[mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[0]].thermalEmissivity_const
						+ vertexVars[mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[1]].thermalEmissivity * vertexVars[mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[1]].temp + vertexVars[mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[1]].thermalEmissivity_const
						+ vertexVars[mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[2]].thermalEmissivity * vertexVars[mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[2]].temp + vertexVars[mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[2]].thermalEmissivity_const
						) / 3.0;
					///	�ȉ���[]�͏�܂ł�[l]�ƈقȂ�B
					///	ID�����Ԗڂ��ɂ���āA�`��֐��̌W�����قȂ�̂ŁA
					tetVars[id].vecf[3] +=  ems_temp * Tout[j] * (1.0/3.0) * faceVars[mesh->tets[id].faceIDs[l]].area * vecf4array[j];	//+=:j=0~3��vecf2array�����Z								
				}
			}
		}
	}
}

#if 0
void PHFemThermo::CreateVecF3surfaceAll(){
	PHFemMeshNew* mesh = GetPHFemMesh();

	//	������
	//	��΁A���΁A���΂ɂ��ď�����(�x�N�g���̍s���ݒ�A������)
	vecFAll_f3.resize(mesh->vertices.size(),1);			//�\�ʂ����łȂ��A�S�ߓ_�ɂ��Čv�Z���Ȃ��ƁA�x�N�g���~�s��̌v�Z���s�����̂��߁B
	vecFAll_f3.clear();
	
	//�l�ʑ̗v�f���Ƃɍs������A�ǂ����ō�������
	//id�����āA�ċA�I�ɍ���Ă���
	for(unsigned id =0; id < mesh->tets.size();id++){ 
		//�s������
		CreateVecf3(id);//;		// f3surface�ł͂Ȃ�����ǁA�����̂��H	//CreateVecf2surface(id,num);	//	���̊֐����A�����Ɏw�肵���x�N�g���ɓ������悤�ɂ���?
		for(unsigned j =0;j < 4; j++){
			int vtxid0 = mesh->tets[id].vertexIDs[j];
			//vecFAllSum[vtxid0] = vecf[j];			//�S�̍����x�N�g�����쐬�F�K�E�X�U�C�f���v�Z���ł���Ă��鏈���E������s���B�����܂ł�Vecf2�ł��B
			vecFAll_f3[vtxid0][0] += tetVars[id].vecf[2][j];		//	+= ����Ȃ��Ă������̂��H���l�ɁA���̃\�[�X�ł� += �̕K�v������̂ł́H
		}
	}
	//�������ɁA�K�E�X�U�C�f���v�Z�ŁAVecFAll�ɃZ�b�g����֐������B
	//vecFAllSum�ɉ��Z�Ƃ�����ƁA�ǂ�ǂ񑝂��Ă��܂����A�t�ɁA���́A�ω����Ȃ��v�f{F_3}�ȂǁA�S�̃x�N�g��������āA�ۑ����Ă����K�v
	//�K�E�X�U�C�f���̌v�Z�̒��ŁA����܂ł̌v�Z��F�x�N�g�����g���̂ɑウ�āA�}�C�X�e�b�v�ŁAVecF��F1,F2������K�v������B
}

void PHFemThermo::CreateVecF2surfaceAll(){
	PHFemMeshNew* mesh = GetPHFemMesh();

	//	������
	//	��΁A���΁A���΂ɂ��ď�����(�x�N�g���̍s���ݒ�A������) initVecFAlls()�Ŏ��s
	//for(unsigned i =0; i < 4 ;i++){
	//	vecFAll_f2IH[i].resize(vertices.size(),1);			//�\�ʂ����łȂ��A�S�ߓ_�ɂ��Čv�Z���Ȃ��ƁA�x�N�g���~�s��̌v�Z���s�����̂��߁B
	//	vecFAll_f2IH[i].clear();
	//}
	
	//�l�ʑ̗v�f���Ƃɍs������A�ǂ����ō�������
	//id�����āA�ċA�I�ɍ���Ă���
	for(unsigned id =0; id < mesh->tets.size();id++){ 
		//�s������
		for(unsigned num =0; num <4 ; num++){	//�S�Η�(OFF/WEEK/MIDDLE/HIGH)�ɂ���
			CreateVecf2surface(id,num);			//	���̊֐����A�����Ɏw�肵���x�N�g���ɓ������悤�ɂ���?
			//num���ɁA���ꕨ�ɓ����B
			for(unsigned j =0;j < 4; j++){
				int vtxid0 = mesh->tets[id].vertexIDs[j];
				//vecFAllSum[vtxid0][0] = vecf[j];			//�S�̍����x�N�g�����쐬�F�K�E�X�U�C�f���v�Z���ł���Ă��鏈���E������s���B�����܂ł�Vecf2�ł��B
				//vecFAll_f2IHw[vtxid0][0] = vecf[j];
				vecFAll_f2IH[num][vtxid0][0] += tetVars[id].vecf[1][j];		//f2��[num(�Η�)]	+= ����Ȃ��Ă������̂��H���l�ɁA���̃\�[�X�ł� += �̕K�v������̂ł́H
			}
		}

		////�v�f�̐ߓ_�ԍ��̏ꏊ�ɁA���̐ߓ_��f�̒l������
		////j:�v�f�̒��̉��Ԗڂ�
		//for(unsigned j =0;j < 4; j++){
		//	int vtxid0 = tets[id].vertices[j]
		//	vecFAll_f2IHw[vtxid0][0] = vecf[j];tets[id].vecf[1]//��j�v�f
		//}

		//
		////���ꕨ�ɓ����
		////j:�v�f�̒��̉��Ԗڂ�
		//for(unsigned j =0;j < 4; j++){
		//	int vtxid0 = tets[id].vertices[j];
		//	vecFAllSum[vtxid0] = vecf[j];			//�S�̍����x�N�g�����쐬�F�K�E�X�U�C�f���v�Z���ł���Ă��鏈���E������s���B�����܂ł�Vecf2�ł��B
		//	vecFAll_f2IHw[vtxid0][0] = vecf[j];
		//	
		//}

	//�������ɁA�K�E�X�U�C�f���v�Z�ŁAvecFAllSum�ɃZ�b�g����֐������B
	//vecFAllSum�ɉ��Z�Ƃ�����ƁA�ǂ�ǂ񑝂��Ă��܂����A�t�ɁA���́A�ω����Ȃ��v�f{F_3}�ȂǁA�S�̃x�N�g��������āA�ۑ����Ă����K�v
	//�K�E�X�U�C�f���̌v�Z�̒��ŁA����܂ł̌v�Z��F�x�N�g�����g���̂ɑウ�āA�}�C�X�e�b�v�ŁAVecF��F1,F2������K�v������B

	}

	//�ȉ��ACreateVecfLocal����R�s�y�@2012.9.25
	//���ׂĂ̗v�f�ɂ��ČW���s������
	//f1�����
	//>	�M�������E����	vecf2�����			
}
#endif

//void PHFemThermo::CreateVecf2(unsigned id){
//	//	������
//	for(unsigned i =0; i < 4 ;i++){
//		//�Ō�ɓ����s���������
//		tets[id].vecf[1][i] =0.0;				//>	f3 = vecf[1] 
//	}	
//	//l=0�̎�f31,1:f32, 2:f33, 3:f34	�𐶐�
//	for(unsigned l= 0 ; l < 4; l++){
//		//matk2array[l] = matk2temp;
//		vecf2array[l] = Create41Vec1();
//		//	l�s��0��
//		vecf2array[l][l] = 0.0;
//
//		//array[n][m][l]	= narray[n],m�sl��
//		//	f_3	(vecf3array[0], vecf3array[1],..)
//		// =	| 0 | + | 1 |+...
//		//		| 1 |   | 0 |
//		//		| 1 |   | 1 |
//		//		| 1 |   | 1 |
//
//		//	for debug
//		//DSTR << "vecf3array[" << l << "] : " << std::endl;
//		//DSTR << vecf3array[l] << std::endl;
//
//		//�W���̐ς��Ƃ�
//		//���̐ߓ_�ō\�������l�ʑ̖̂ʐς̐ς��Ƃ�
//		//�l�ʑ̂̐ߓ_1,2,3(0�ȊO)�ō��O�p�`�̖ʐ�
//		//l==0�Ԗڂ̎��A 123	��������
//		//l==1			0 23
//		//l==2			01 3
//		//l==3			012
//		//��CalcTriangleArea�ɓ���邱�Ƃ��ł���悤�ɃA���S���Y�����l����B
//
//		//>	CreateMatk2t�̂悤�ȃA���S���Y���ɕύX�\��
//		//k21
//		if(l==0){
//			//>	�O�p�`�ʂ��\������3���_�̔M�����̑�������
//			double tempHF = (vertices[tets[id].vertices[1]].heatFluxValue + vertices[tets[id].vertices[2]].heatFluxValue + vertices[tets[id].vertices[3]].heatFluxValue ) / 3.0;		//HTR:HeatTransRatio
//			vecf2array[l] = tempHF * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[1],tets[id].vertices[2],tets[id].vertices[3] ) * vecf2array[l];
//			//DSTR << "vecf2array[" << l << "] : " << vecf2array[l] << std::endl;
//			//Vec�̐ߓ_���ɂ��̐ߓ_�ł̎��͗��̉��xTc�Ƃ̐ς��s��
//			
//			////>	�s�v�H
//			//for(unsigned m=0; m<4; m++){
//			//	vecf2array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf2array[l][m];
//			//}
//		}
//		//	k22
//		else if(l==1){
//			double tempHF = (vertices[tets[id].vertices[0]].heatFluxValue + vertices[tets[id].vertices[2]].heatFluxValue + vertices[tets[id].vertices[3]].heatFluxValue ) / 3.0;		//HTR:HeatTransRatio
//			vecf2array[l] = tempHF * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[2],tets[id].vertices[3] ) * vecf2array[l];
//			//vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[2],tets[id].vertices[3] ) * vecf3array[l];
//			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
//			//Vec�̐ߓ_���ɂ��̐ߓ_�ł̎��͗��̉��xTc�Ƃ̐ς��s��
//			//for(unsigned m=0; m<4; m++){
//			//	vecf2array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf2array[l][m];
//			//}
//		}
//		//	k23
//		else if(l==2){
//			double tempHF = (vertices[tets[id].vertices[0]].heatFluxValue + vertices[tets[id].vertices[1]].heatFluxValue + vertices[tets[id].vertices[3]].heatFluxValue ) / 3.0;		//HTR:HeatTransRatio
//			vecf2array[l] = tempHF * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[3] ) * vecf2array[l];
//			//vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[3] ) * vecf3array[l];
//			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
//			//Vec�̐ߓ_���ɂ��̐ߓ_�ł̎��͗��̉��xTc�Ƃ̐ς��s��
//			//for(unsigned m=0; m<4; m++){
//			//	vecf2array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf2array[l][m];
//			//}
//		}
//		//	k24
//		else if(l==3){
//			double tempHF = (vertices[tets[id].vertices[0]].heatFluxValue + vertices[tets[id].vertices[1]].heatFluxValue + vertices[tets[id].vertices[2]].heatFluxValue ) / 3.0;		//HTR:HeatTransRatio
//			vecf2array[l] = tempHF * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[2] ) * vecf2array[l];
//			//vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[2] ) * vecf3array[l];
//			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
//			//Vec�̐ߓ_���ɂ��̐ߓ_�ł̎��͗��̉��xTc�Ƃ̐ς��s��
//			//for(unsigned m=0; m<4; m++){
//			//	vecf2array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf2array[l][m];
//			//}
//		}
//		//for debug
//		//DSTR << "vecf3array[" << l << "]�̊����ł́�" << std::endl;
//		//DSTR << vecf3array[l] << std::endl;
//		//if(dMatCAll == NULL){
//		//	//DSTR <<"i : "<< i << ", l : " << l << std::endl;
//		//	DSTR << "dMatCAll == NULL" <<std::endl;
//		//	DSTR <<"l : " << l << std::endl;
//		//}
//	}
//
//	//f3 = f31 + f32 + f33 + f34
//	for(unsigned i=0; i < 4; i++){
//		//vecf3 += vecf3array[i];
//		tets[id].vecf[1] += vecf2array[i];
//		//	for debug
//		//DSTR << "vecf3 �� vecf3array = f3" << i+1 <<"�܂ŉ��Z�����s��" << std::endl;
//		//DSTR << vecf3 << std::endl;
//	}
//	
//	//	f1,f2,f3,f4	���v�Z����ۂɁA[0][0]��������[3][0]�����܂ł̔�0�����ɂ��āA���Tc�������Ă��܂�
//
//
//	//for debug
//	//DSTR << "�ߓ_�i";
//	//for(unsigned i =0; i < 4; i++){
//	//	DSTR << tets[id].vertices[i] << "," ;
//	//}
//	//DSTR << ")�ō\�������l�ʑ̂�" << std::endl;
//	//DSTR << "vecf3 : " << std::endl;
//	//DSTR << vecf3 << std::endl;
//	//int hogeshishi =0;
//}

void PHFemThermo::CreateVecf3_(unsigned id){
	PHFemMeshNew* mesh = GetPHFemMesh();

	//	������
	for(unsigned i =0; i < 4 ;i++){
		//vecf3[i] = 0.0;		//�Ō�ɓ����s���������
		tetVars[id].vecf[2][i] =0.0;
	}	
	//l=0�̎�f31,1:f32, 2:f33, 3:f34	�𐶐�
	for(unsigned l= 0 ; l < 4; l++){
		//matk2array[l] = matk2temp;
		vecf3array[l] = Create41Vec1();
		//	l�s��0��
		//for(int i=0;i<4;i++){
		//	vecf3array[l][l][i] = 0.0;
		//}
		vecf3array[l][l] = 0.0;
		//array[n][m][l]	= narray[n],m�sl��
		//	f_3	(vecf3array[0], vecf3array[1],..)
		// =	| 0 | + | 1 |+...
		//		| 1 |   | 0 |
		//		| 1 |   | 1 |
		//		| 1 |   | 1 |
		//	for debug
		//DSTR << "vecf3array[" << l << "] : " << std::endl;
		//DSTR << vecf3array[l] << std::endl;

		//�W���̐ς��Ƃ�
		//���̐ߓ_�ō\�������l�ʑ̖̂ʐς̐ς��Ƃ�
		//�l�ʑ̂̐ߓ_1,2,3(0�ȊO)�ō��O�p�`�̖ʐ�
		//l==0�Ԗڂ̎��A 123	��������
		//l==1			0 23
		//l==2			01 3
		//l==3			012
		//��CalcTriangleArea�ɓ���邱�Ƃ��ł���悤�ɃA���S���Y�����l����B
		//k21
		if(l==0){
			//�O�p�`�ʂ��\������3���_�̔M�`�B���̑�������
			double tempHTR = (vertexVars[mesh->tets[id].vertexIDs[1]].heatTransRatio + vertexVars[mesh->tets[id].vertexIDs[2]].heatTransRatio + vertexVars[mesh->tets[id].vertexIDs[3]].heatTransRatio ) / 3.0;		//HTR:HeatTransRatio
			double avgTc = (vertexVars[mesh->tets[id].vertexIDs[1]].Tc + vertexVars[mesh->tets[id].vertexIDs[2]].Tc + vertexVars[mesh->tets[id].vertexIDs[3]].Tc ) / 3.0;
			vecf3array[l] = tempHTR * avgTc * (1.0/3.0) * CalcTriangleArea( mesh->tets[id].vertexIDs[1],mesh->tets[id].vertexIDs[2],mesh->tets[id].vertexIDs[3] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			
			//>	���͖{���H
			//Vec�̐ߓ_���ɂ��̐ߓ_�ł̎��͗��̉��xTc�Ƃ̐ς��s��
			//for(unsigned m=0; m<4; m++){
			//	vecf3array[l][m] = vertices[tets[id].vertices[m]] * vecf3array[l][m];
			//}
		}
		//	k22
		else if(l==1){
			double tempHTR = (vertexVars[mesh->tets[id].vertexIDs[0]].heatTransRatio + vertexVars[mesh->tets[id].vertexIDs[2]].heatTransRatio + vertexVars[mesh->tets[id].vertexIDs[3]].heatTransRatio ) / 3.0;		//HTR:HeatTransRatio
			double avgTc = (vertexVars[mesh->tets[id].vertexIDs[0]].Tc + vertexVars[mesh->tets[id].vertexIDs[2]].Tc + vertexVars[mesh->tets[id].vertexIDs[3]].Tc ) / 3.0;		//HTR:HeatTransRatio
			vecf3array[l] = tempHTR * avgTc * (1.0/3.0) * CalcTriangleArea( mesh->tets[id].vertexIDs[0],mesh->tets[id].vertexIDs[2],mesh->tets[id].vertexIDs[3] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vec�̐ߓ_���ɂ��̐ߓ_�ł̎��͗��̉��xTc�Ƃ̐ς��s��
			//for(unsigned m=0; m<4; m++){
			//	vecf3array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf3array[l][m];
			//}
		}
		//	k23
		else if(l==2){
			double tempHTR = (vertexVars[mesh->tets[id].vertexIDs[0]].heatTransRatio + vertexVars[mesh->tets[id].vertexIDs[1]].heatTransRatio + vertexVars[mesh->tets[id].vertexIDs[3]].heatTransRatio ) / 3.0;		//HTR:HeatTransRatio
			double avgTc = (vertexVars[mesh->tets[id].vertexIDs[0]].Tc + vertexVars[mesh->tets[id].vertexIDs[1]].Tc + vertexVars[mesh->tets[id].vertexIDs[3]].Tc ) / 3.0;		//HTR:HeatTransRatio
			vecf3array[l] = tempHTR * avgTc * (1.0/3.0) * CalcTriangleArea( mesh->tets[id].vertexIDs[0],mesh->tets[id].vertexIDs[1],mesh->tets[id].vertexIDs[3] ) * vecf3array[l];
			//vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[3] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vec�̐ߓ_���ɂ��̐ߓ_�ł̎��͗��̉��xTc�Ƃ̐ς��s��
			//for(unsigned m=0; m<4; m++){
			//	vecf3array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf3array[l][m];
			//}
		}
		//	k24
		else if(l==3){
			double tempHTR = (vertexVars[mesh->tets[id].vertexIDs[0]].heatTransRatio + vertexVars[mesh->tets[id].vertexIDs[1]].heatTransRatio + vertexVars[mesh->tets[id].vertexIDs[2]].heatTransRatio ) / 3.0;		//HTR:HeatTransRatio
			double avgTc = (vertexVars[mesh->tets[id].vertexIDs[0]].Tc + vertexVars[mesh->tets[id].vertexIDs[1]].Tc + vertexVars[mesh->tets[id].vertexIDs[2]].Tc ) / 3.0;		//HTR:HeatTransRatio
			vecf3array[l] = tempHTR * avgTc * (1.0/3.0) * CalcTriangleArea( mesh->tets[id].vertexIDs[0],mesh->tets[id].vertexIDs[1],mesh->tets[id].vertexIDs[2] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vec�̐ߓ_���ɂ��̐ߓ_�ł̎��͗��̉��xTc�Ƃ̐ς��s��
			//for(unsigned m=0; m<4; m++){
			//	vecf3array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf3array[l][m];
			//}
		}
		//for debug
		//DSTR << "vecf3array[" << l << "]�̊����ł́�" << std::endl;
		//DSTR << vecf3array[l] << std::endl;
		//if(dMatCAll == NULL){
		//	//DSTR <<"i : "<< i << ", l : " << l << std::endl;
		//	DSTR << "dMatCAll == NULL" <<std::endl;
		//	DSTR <<"l : " << l << std::endl;
		//}
	}

	//f3 = f31 + f32 + f33 + f34
	for(unsigned i=0; i < 4; i++){
		//vecf3 += vecf3array[i];
		tetVars[id].vecf[2] += vecf3array[i];
		//	for debug
		//DSTR << "vecf3 �� vecf3array = f3" << i+1 <<"�܂ŉ��Z�����s��" << std::endl;
		//DSTR << vecf3 << std::endl;
	}
	
	//	f1,f2,f3,f4	���v�Z����ۂɁA[0][0]��������[3][0]�����܂ł̔�0�����ɂ��āA���Tc�������Ă��܂�


	//for debug
	//DSTR << "�ߓ_�i";
	//for(unsigned i =0; i < 4; i++){
	//	DSTR << tets[id].vertices[i] << "," ;
	//}
	//DSTR << ")�ō\�������l�ʑ̂�" << std::endl;
	//DSTR << "vecf3 : " << std::endl;
	//DSTR << vecf3 << std::endl;
	//int hogeshishi =0;
}

void PHFemThermo::CreateVecf3(unsigned id){
	PHFemMeshNew* mesh = GetPHFemMesh();

	//	������
	for(unsigned i =0; i < 4 ;i++){
		//vecf3[i] = 0.0;		//�Ō�ɓ����s���������
		tetVars[id].vecf[2][i] =0.0;
	}	
	//l=0�̎�f31,1:f32, 2:f33, 3:f34	�𐶐�
	for(unsigned l= 0 ; l < 4; l++){
		//matk2array[l] = matk2temp;
		vecf3array[l] = Create41Vec1();
		//	l�s��0��
		//for(int i=0;i<4;i++){
		//	vecf3array[l][l][i] = 0.0;
		//}
		vecf3array[l][l] = 0.0;
		//array[n][m][l]	= narray[n],m�sl��
		//	f_3	(vecf3array[0], vecf3array[1],..)
		// =	| 0 | + | 1 |+...
		//		| 1 |   | 0 |
		//		| 1 |   | 1 |
		//		| 1 |   | 1 |
		//	for debug
		//DSTR << "vecf3array[" << l << "] : " << std::endl;
		//DSTR << vecf3array[l] << std::endl;

		//�W���̐ς��Ƃ�
		//���̐ߓ_�ō\�������l�ʑ̖̂ʐς̐ς��Ƃ�
		//�l�ʑ̂̐ߓ_1,2,3(0�ȊO)�ō��O�p�`�̖ʐ�
		//l==0�Ԗڂ̎��A 123	��������
		//l==1			0 23
		//l==2			01 3
		//l==3			012
		//��CalcTriangleArea�ɓ���邱�Ƃ��ł���悤�ɃA���S���Y�����l����B
		//k21
		if(l==0){
			//�O�p�`�ʂ��\������3���_�̔M�`�B���̑�������
			double tempHTR = (vertexVars[mesh->tets[id].vertexIDs[1]].heatTransRatio + vertexVars[mesh->tets[id].vertexIDs[2]].heatTransRatio + vertexVars[mesh->tets[id].vertexIDs[3]].heatTransRatio ) / 3.0;		//HTR:HeatTransRatio
			vecf3array[l] = tempHTR * (1.0/3.0) * CalcTriangleArea( mesh->tets[id].vertexIDs[1],mesh->tets[id].vertexIDs[2],mesh->tets[id].vertexIDs[3] ) * vecf3array[l];
			//vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets.vertices[1],tets.vertices[2],tets.vertices[3] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vec�̐ߓ_���ɂ��̐ߓ_�ł̎��͗��̉��xTc�Ƃ̐ς��s��
			for(unsigned m=0; m<4; m++){
				vecf3array[l][m] = vertexVars[mesh->tets[id].vertexIDs[m]].Tc * vecf3array[l][m];
			}
		}
		//	k22
		else if(l==1){
			double tempHTR = (vertexVars[mesh->tets[id].vertexIDs[0]].heatTransRatio + vertexVars[mesh->tets[id].vertexIDs[2]].heatTransRatio + vertexVars[mesh->tets[id].vertexIDs[3]].heatTransRatio ) / 3.0;		//HTR:HeatTransRatio
			vecf3array[l] = tempHTR * (1.0/3.0) * CalcTriangleArea( mesh->tets[id].vertexIDs[0],mesh->tets[id].vertexIDs[2],mesh->tets[id].vertexIDs[3] ) * vecf3array[l];
			//vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[2],tets[id].vertices[3] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vec�̐ߓ_���ɂ��̐ߓ_�ł̎��͗��̉��xTc�Ƃ̐ς��s��
			for(unsigned m=0; m<4; m++){
				vecf3array[l][m] = vertexVars[mesh->tets[id].vertexIDs[m]].Tc * vecf3array[l][m];
			}
		}
		//	k23
		else if(l==2){
			double tempHTR = (vertexVars[mesh->tets[id].vertexIDs[0]].heatTransRatio + vertexVars[mesh->tets[id].vertexIDs[1]].heatTransRatio + vertexVars[mesh->tets[id].vertexIDs[3]].heatTransRatio ) / 3.0;		//HTR:HeatTransRatio
			vecf3array[l] = tempHTR * (1.0/3.0) * CalcTriangleArea( mesh->tets[id].vertexIDs[0],mesh->tets[id].vertexIDs[1],mesh->tets[id].vertexIDs[3] ) * vecf3array[l];
			//vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[3] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vec�̐ߓ_���ɂ��̐ߓ_�ł̎��͗��̉��xTc�Ƃ̐ς��s��
			for(unsigned m=0; m<4; m++){
				vecf3array[l][m] = vertexVars[mesh->tets[id].vertexIDs[m]].Tc * vecf3array[l][m];
			}
		}
		//	k24
		else if(l==3){
			double tempHTR = (vertexVars[mesh->tets[id].vertexIDs[0]].heatTransRatio + vertexVars[mesh->tets[id].vertexIDs[1]].heatTransRatio + vertexVars[mesh->tets[id].vertexIDs[2]].heatTransRatio ) / 3.0;		//HTR:HeatTransRatio
			vecf3array[l] = tempHTR * (1.0/3.0) * CalcTriangleArea( mesh->tets[id].vertexIDs[0],mesh->tets[id].vertexIDs[1],mesh->tets[id].vertexIDs[2] ) * vecf3array[l];
			//vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[2] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vec�̐ߓ_���ɂ��̐ߓ_�ł̎��͗��̉��xTc�Ƃ̐ς��s��
			for(unsigned m=0; m<4; m++){
				vecf3array[l][m] = vertexVars[mesh->tets[id].vertexIDs[m]].Tc * vecf3array[l][m];
			}
		}
		//for debug
		//DSTR << "vecf3array[" << l << "]�̊����ł́�" << std::endl;
		//DSTR << vecf3array[l] << std::endl;
		//if(dMatCAll == NULL){
		//	//DSTR <<"i : "<< i << ", l : " << l << std::endl;
		//	DSTR << "dMatCAll == NULL" <<std::endl;
		//	DSTR <<"l : " << l << std::endl;
		//}
	}

	//f3 = f31 + f32 + f33 + f34
	for(unsigned i=0; i < 4; i++){
		//vecf3 += vecf3array[i];
		tetVars[id].vecf[2] += vecf3array[i];
		//	for debug
		//DSTR << "vecf3 �� vecf3array = f3" << i+1 <<"�܂ŉ��Z�����s��" << std::endl;
		//DSTR << vecf3 << std::endl;
	}
	
	//	f1,f2,f3,f4	���v�Z����ۂɁA[0][0]��������[3][0]�����܂ł̔�0�����ɂ��āA���Tc�������Ă��܂�


	//for debug
	//DSTR << "�ߓ_�i";
	//for(unsigned i =0; i < 4; i++){
	//	DSTR << tets[id].vertices[i] << "," ;
	//}
	//DSTR << ")�ō\�������l�ʑ̂�" << std::endl;
	//DSTR << "vecf3 : " << std::endl;
	//DSTR << vecf3 << std::endl;
	//int hogeshishi =0;
}

double PHFemThermo::CalcTetrahedraVolume2(unsigned id){
	PHFemMeshNew* mesh = GetPHFemMesh();

	PTM::TMatrixRow<4,4,double> tempMat44;
	tempMat44.clear();
	for(unsigned i =0; i < 4; i++){
		for(unsigned j =0; j < 4; j++){
			if(i == 0){
				tempMat44[i][j] = 1.0;
			}
			else if(i == 1){
					tempMat44[i][j] = mesh->vertices[mesh->tets[id].vertexIDs[j]].pos.x;
			}
			else if(i == 2){
					tempMat44[i][j] = mesh->vertices[mesh->tets[id].vertexIDs[j]].pos.y;
			}
			else if(i == 3){
					tempMat44[i][j] = mesh->vertices[mesh->tets[id].vertexIDs[j]].pos.z;
			}

			
		}
	}
	//	for debug
	//DSTR << tempMat44 << std::endl;
	//for(unsigned i =0; i < 4 ;i++){
	//	DSTR << vertices[tets.vertices[i]].pos.x << " , " << vertices[tets.vertices[i]].pos.y << " , " << vertices[tets.vertices[i]].pos.z << std::endl; 
	//}
	//DSTR << tempMat44.det() << std::endl;
	//int hogever = 0;
	return tempMat44.det() / 6.0;
}

double PHFemThermo::CalcTetrahedraVolume(FemTet tet){
	PHFemMeshNew* mesh = GetPHFemMesh();

	PTM::TMatrixRow<4,4,double> tempMat44;
	for(unsigned i =0; i < 4; i++){
		for(unsigned j =0; j < 4; j++){
			if(i == 0){
				tempMat44[i][j] = 1.0;
			}
			else if(i == 1){
				tempMat44[i][j] = mesh->vertices[tet.vertexIDs[j]].pos.x;
			}
			else if(i == 2){
				tempMat44[i][j] = mesh->vertices[tet.vertexIDs[j]].pos.y;
			}
			else if(i == 3){
				tempMat44[i][j] = mesh->vertices[tet.vertexIDs[j]].pos.z;
			}
			
		}
	}
	//	for debug
	//DSTR << tempMat44 << std::endl;
	//for(unsigned i =0; i < 4 ;i++){
	//	DSTR << vertices[tets.vertices[i]].pos.x << " , " << vertices[tets.vertices[i]].pos.y << " , " << vertices[tets.vertices[i]].pos.z << std::endl; 
	//}
	//DSTR << tempMat44.det() << std::endl;
	//int hogever = 0;
	return tempMat44.det() / 6.0;
}

double PHFemThermo::CalcTriangleArea(int id0, int id1, int id2){
	PHFemMeshNew* mesh = GetPHFemMesh();

	double area=0.0;								///	�v���P	faces[id].area�ɒl������ 

	//�s�񎮂̐�����p���Ėʐς����߂�
	//triarea =
	//|  1     1     1   |
	//|x2-x1 y2-y1 z2-z1 |
	//|x3-x1 y3-y1 z3-z1 |
	//|
	PTM::TMatrixRow<3,3,double> triarea;		//�O�p�`�̖ʐρ@= tri + area
	for(unsigned i =0 ; i < 3 ; i++){
		triarea[0][i] = 1.0;
	}
	for(unsigned i =0 ; i < 3 ; i++){
		//					x2(�ߓ_2��x(pos��i����)��)	-	x1(�V)
		// i==0�̎�	vertices[id1].pos[i]	=>	 pos[0] == pos.x
		triarea[1][i] = mesh->vertices[id1].pos[i] - mesh->vertices[id0].pos[i];
	}
	for(unsigned i =0 ; i < 3 ; i++){
		//					x3(�ߓ_3��x(pos��i����)��)	-	x1(�V)
		triarea[2][i] = mesh->vertices[id2].pos[i] - mesh->vertices[id0].pos[i];
	}
	double m1,m2,m3 = 0.0;
	m1 = triarea[1][1] * triarea[2][2] - triarea[1][2] * triarea[2][1];
	m2 = triarea[2][0] * triarea[1][2] - triarea[1][0] * triarea[2][2];
	m3 = triarea[1][0] * triarea[2][1] - triarea[2][0] * triarea[1][1];

	area = sqrt(m1 * m1 + m2 * m2 + m3 * m3) / 2.0;

	//	for debug
	//DSTR << "�O�p�`�̖ʐς� : " << area << std::endl; 

	//0�Ԗڂ̐ߓ_��40,1�Ԗڂ̐ߓ_��134,2�Ԗڂ̐ߓ_��79 �̍��W�Ōv�Z���Ă݂�
	//�O�p�`�����߂�s�� : 2.75949e-005 * 1 = 2.75949 �~ 10-5(byGoogle�v�Z�@) [m^2] = 2.75949 �~ 10-1 [cm^2]�Ȃ̂ŁA�l�M�̃��b�V���̃X�P�[���Ȃ��̂����Ă���͂�

	return area;
}

PTM::TMatrixCol<4,1,double> PHFemThermo::Create41Vec1(){
	PTM::TMatrixCol<4,1,double> Mat1temp;
	for(int i =0; i <4 ; i++){
				Mat1temp[i][0] = 1.0;
	}
	return Mat1temp;
}

PTM::TMatrixRow<4,4,double> PHFemThermo::Create44Mat21(){
	//|2 1 1 1 |
	//|1 2 1 1 |
	//|1 1 2 1 |
	//|1 1 1 2 |	�����
	PTM::TMatrixRow<4,4,double> MatTemp;
	for(int i =0; i <4 ; i++){
		for(int j=0; j < 4 ; j++){
			if(i==j){
				MatTemp[i][j] = 2.0;
			}else{
				MatTemp[i][j] = 1.0;
			}
		}
	}
	return MatTemp;
}

void PHFemThermo::ActivateVtxbeRadiantHeat(){
	for(int i=0;i<NSurfaceVertices();++i){
		vertexVars[i].beRadiantHeat =true;
	}
}


void PHFemThermo::CreateMatk3t_nonRadiantHeat(unsigned id){
	PHFemMeshNew* mesh = GetPHFemMesh();

	//l=0�̎�k21,1�̎�:k22, 2�̎�:k23, 3�̎�:k24	�𐶐�
	for(unsigned l= 0 ; l < 4; l++){
		//matk2array[l] = matk2temp;
		matk3array[l] = Create44Mat21();
		//	1�si���0��
		for(int i=0;i<4;i++){
			matk3array[l][l][i] = 0.0;
		}
		//	i�s1���0��
		for(int i=0;i<4;i++){
			matk3array[l][i][l] = 0.0;
		}
	}
	///	������
	tetVars[id].matk[2].clear();
	for(unsigned l= 0 ; l < 4; l++){
		///	�l�ʑ̂̊e��(l = 0 �` 3) �ɂ��ă��b�V���\�ʂ��ǂ������`�F�b�N����B�\�ʂȂ�A�s��������matk2array�ɓ����
		//faces[tets.faces[i]].sorted;		/// 1,24,58�݂����Ȑߓ_�ԍ��������Ă���

		///	�O�k�̖� ���� �M�`�B�����X�V���ꂽ�� matk2���X�V����K�v������	//2014.3.12�����ǋL	�M�`�B���_���܂�face�ł́A�s��ɎQ�����Ȃ�
		if(mesh->tets[id].faceIDs[l] < (int)mesh->nSurfaceFace && faceVars[mesh->tets[id].faceIDs[l]].alphaUpdated){
			if(vertexVars[mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[0]].beRadiantHeat && vertexVars[mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[1]].beRadiantHeat 
				&& vertexVars[mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[2]].beRadiantHeat	){

				//�Ō�ɓ����s���������
				for(unsigned i =0; i < 4 ;i++){
					for(unsigned j =0; j < 4 ;j++){
						tetVars[id].matk[2][i][j] = 0.0;
					}
				}
				///	�l�ʑ̂̎O�p�`�̖ʐς��v�Z		///	���̊֐��̊O�Ŗʐϕ��̖ʐόv�Z����������B�ړ�����
				if(faceVars[mesh->tets[id].faceIDs[l]].area ==0 || faceVars[mesh->tets[id].faceIDs[l]].deformed ){		///	�ʐς��v�Z����Ă��Ȃ����i�͂��߁j or deformed(�ό`�������E�������)��true�̎�		///	�����̒ǉ�	�ʐς�0�� ||(OR) �����X�V���ꂽ��
					faceVars[mesh->tets[id].faceIDs[l]].area = CalcTriangleArea(mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[0], mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[1], mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[2]);
					faceVars[mesh->tets[id].faceIDs[l]].deformed = false;
				}
				///	�v�Z���ʂ��s��ɑ��
				///	area�̌v�Z�Ɏg���Ă��Ȃ��_�������Ă���s�Ɨ���������s��̐ς��Ƃ�
				///	�ϕ��v�Z�����{����l����
				unsigned vtx = mesh->tets[id].vertexIDs[0] + mesh->tets[id].vertexIDs[1] + mesh->tets[id].vertexIDs[2] + mesh->tets[id].vertexIDs[3];
				unsigned ID = vtx -( mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[0] + mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[1] + mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[2] );
				for(unsigned j=0;j<4;j++){
					if(mesh->tets[id].vertexIDs[j] == ID){					///	�`��֐����P�A�i���Ȃ킿�j����face�ɑΖʂ��钸�_�@�ƈ�v������@���̎���face�Ŗʐϕ�����
						///	j�Ԗڂ̍s��̐�����0�ɂ���matk2array�Ōv�Z����
						///	�O�k�ɂȂ����b�V���ʂ̖ʐς�0�ŏ��������Ă���
						//faces[tets[id].faces[l]].thermalEmissivity = 
						double ems_temp = ( 	
							vertexVars[mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[0]].thermalEmissivity * vertexVars[mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[0]].temp + vertexVars[mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[0]].thermalEmissivity_const
							+ vertexVars[mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[1]].thermalEmissivity * vertexVars[mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[1]].temp + vertexVars[mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[1]].thermalEmissivity_const
							+ vertexVars[mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[2]].thermalEmissivity * vertexVars[mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[2]].temp + vertexVars[mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[2]].thermalEmissivity_const
							) / 3.0;		///	���Yface�̔M�`�B�����\���ߓ_�ł̒l�̑������ς��Ƃ�
						///	�ȉ���[]�͏�܂ł�[l]�ƈقȂ�B
						///	ID�����Ԗڂ��ɂ���āA�`��֐��̌W�����قȂ�̂ŁA
						tetVars[id].matk[2] += ems_temp * (1.0/12.0) * faceVars[mesh->tets[id].faceIDs[l]].area * matk3array[j];
					}
				}
			}
		}
	}

}

void PHFemThermo::CreateMatk3t(unsigned id){
	PHFemMeshNew* mesh = GetPHFemMesh();

	//l=0�̎�k21,1�̎�:k22, 2�̎�:k23, 3�̎�:k24	�𐶐�
	for(unsigned l= 0 ; l < 4; l++){
		//matk2array[l] = matk2temp;
		matk3array[l] = Create44Mat21();
		//	1�si���0��
		for(int i=0;i<4;i++){
			matk3array[l][l][i] = 0.0;
		}
		//	i�s1���0��
		for(int i=0;i<4;i++){
			matk3array[l][i][l] = 0.0;
		}
	}
	///	������
	tetVars[id].matk[2].clear();
	for(unsigned l= 0 ; l < 4; l++){
		///	�l�ʑ̂̊e��(l = 0 �` 3) �ɂ��ă��b�V���\�ʂ��ǂ������`�F�b�N����B�\�ʂȂ�A�s��������matk2array�ɓ����
		//faces[tets.faces[i]].sorted;		/// 1,24,58�݂����Ȑߓ_�ԍ��������Ă���
		if(mesh->tets[id].faceIDs[l] < (int)mesh->nSurfaceFace && faceVars[mesh->tets[id].faceIDs[l]].alphaUpdated ){			///	�O�k�̖� ���� �M�`�B�����X�V���ꂽ�� matk2���X�V����K�v������
			//�Ō�ɓ����s���������
			for(unsigned i =0; i < 4 ;i++){
				for(unsigned j =0; j < 4 ;j++){
					tetVars[id].matk[2][i][j] = 0.0;
				}
			}
			///	�l�ʑ̂̎O�p�`�̖ʐς��v�Z		///	���̊֐��̊O�Ŗʐϕ��̖ʐόv�Z����������B�ړ�����
			if(faceVars[mesh->tets[id].faceIDs[l]].area ==0 || faceVars[mesh->tets[id].faceIDs[l]].deformed ){		///	�ʐς��v�Z����Ă��Ȃ����i�͂��߁j or deformed(�ό`�������E�������)��true�̎�		///	�����̒ǉ�	�ʐς�0�� ||(OR) �����X�V���ꂽ��
				faceVars[mesh->tets[id].faceIDs[l]].area = CalcTriangleArea(mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[0], mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[1], mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[2]);
				faceVars[mesh->tets[id].faceIDs[l]].deformed = false;
			}
			///	�v�Z���ʂ��s��ɑ��
			///	area�̌v�Z�Ɏg���Ă��Ȃ��_�������Ă���s�Ɨ���������s��̐ς��Ƃ�
			///	�ϕ��v�Z�����{����l����
			unsigned vtx = mesh->tets[id].vertexIDs[0] + mesh->tets[id].vertexIDs[1] + mesh->tets[id].vertexIDs[2] + mesh->tets[id].vertexIDs[3];
			unsigned ID = vtx -( mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[0] + mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[1] + mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[2] );
			for(unsigned j=0;j<4;j++){
				if(mesh->tets[id].vertexIDs[j] == ID){					///	�`��֐����P�A�i���Ȃ킿�j����face�ɑΖʂ��钸�_�@�ƈ�v������@���̎���face�Ŗʐϕ�����
					///	j�Ԗڂ̍s��̐�����0�ɂ���matk2array�Ōv�Z����
					///	�O�k�ɂȂ����b�V���ʂ̖ʐς�0�ŏ��������Ă���
					//faces[tets[id].faces[l]].thermalEmissivity = 
					double ems_temp = ( 	
						vertexVars[mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[0]].thermalEmissivity * vertexVars[mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[0]].temp + vertexVars[mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[0]].thermalEmissivity_const
						+ vertexVars[mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[1]].thermalEmissivity * vertexVars[mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[1]].temp + vertexVars[mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[1]].thermalEmissivity_const
						+ vertexVars[mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[2]].thermalEmissivity * vertexVars[mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[2]].temp + vertexVars[mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[2]].thermalEmissivity_const
						) / 3.0;		///	���Yface�̔M�`�B�����\���ߓ_�ł̒l�̑������ς��Ƃ�
					///	�ȉ���[]�͏�܂ł�[l]�ƈقȂ�B
					///	ID�����Ԗڂ��ɂ���āA�`��֐��̌W�����قȂ�̂ŁA
					tetVars[id].matk[2] += ems_temp * (1.0/12.0) * faceVars[mesh->tets[id].faceIDs[l]].area * matk3array[j];
				}
			}
		}
	}
}

void PHFemThermo::CreateMatk2t(unsigned id){
	PHFemMeshNew* mesh = GetPHFemMesh();

	//l=0�̎�k21,1�̎�:k22, 2�̎�:k23, 3�̎�:k24	�𐶐�
	for(unsigned l= 0 ; l < 4; l++){
		//matk2array[l] = matk2temp;
		matk2array[l] = Create44Mat21();
		//	1�si���0��
		for(int i=0;i<4;i++){
			matk2array[l][l][i] = 0.0;
		}
		//	i�s1���0��
		for(int i=0;i<4;i++){
			matk2array[l][i][l] = 0.0;
		}
	}

	///	������
	tetVars[id].matk[1].clear();
	//for(unsigned i =0; i < 4 ;i++){
	//	for(unsigned j =0; j < 4 ;j++){
	//		//tets[id].matk2[i][j] = 0.0;
	//		tets[id].matk[1][i][j] = 0.0;
	//	}
	//}

	//	Check
	//DSTR << "matk2array:" << std::endl;
	//for(unsigned i=0;i<4;i++){
	//	DSTR <<i <<": " << matk2array[i] << std::endl;
	//}
	//DSTR << "++i" <<std::endl;
	//for(unsigned i=0;i<4;++i){
	//	DSTR <<i <<": " << matk2array[i] << std::endl;
	//}


	for(unsigned l= 0 ; l < 4; l++){
		///	�l�ʑ̂̊e��(l = 0 �` 3) �ɂ��ă��b�V���\�ʂ��ǂ������`�F�b�N����B�\�ʂȂ�A�s��������matk2array�ɓ����
		//faces[tets.faces[i]].sorted;		/// 1,24,58�݂����Ȑߓ_�ԍ��������Ă���
		///	�s��^�̓��ꕨ��p��

		//faces[tets.faces[l]].vertices;
		if(mesh->tets[id].faceIDs[l] < (int)mesh->nSurfaceFace && faceVars[mesh->tets[id].faceIDs[l]].alphaUpdated ){			///	�O�k�̖� ���� �M�`�B�����X�V���ꂽ�� matk2���X�V����K�v������
			///	�l�ʑ̂̎O�p�`�̖ʐς��v�Z		///	���̊֐��̊O�Ŗʐϕ��̖ʐόv�Z����������B�ړ�����
			if(faceVars[mesh->tets[id].faceIDs[l]].area ==0 || faceVars[mesh->tets[id].faceIDs[l]].deformed ){		///	�ʐς��v�Z����Ă��Ȃ����i�͂��߁j or deformed(�ό`�������E�������)��true�̎�		///	�����̒ǉ�	�ʐς�0�� ||(OR) �����X�V���ꂽ��
				faceVars[mesh->tets[id].faceIDs[l]].area = CalcTriangleArea(mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[0], mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[1], mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[2]);
				faceVars[mesh->tets[id].faceIDs[l]].deformed = false;
			}
			///	�v�Z���ʂ��s��ɑ��
			///	area�̌v�Z�Ɏg���Ă��Ȃ��_�������Ă���s�Ɨ���������s��̐ς��Ƃ�
			///	�ϕ��v�Z�����{����l����
			unsigned vtx = mesh->tets[id].vertexIDs[0] + mesh->tets[id].vertexIDs[1] + mesh->tets[id].vertexIDs[2] + mesh->tets[id].vertexIDs[3];
			//DSTR << "vtx: " << vtx <<std::endl;
			
			///	area�v�Z�Ɏg���Ă��Ȃ��ߓ_ID�FID
			unsigned ID = vtx -( mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[0] + mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[1] + mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[2] );
			//DSTR << "���b�V���\�ʂ̖ʂ͎���3���_����Ȃ�B" << std::endl;
			//DSTR << "faces[tets.faces[l]].vertices[0]: " << faces[tets.faces[l]].vertices[0] <<std::endl;
			//DSTR << "faces[tets.faces[l]].vertices[1]: " << faces[tets.faces[l]].vertices[1] <<std::endl;
			//DSTR << "faces[tets.faces[l]].vertices[2]: " << faces[tets.faces[l]].vertices[2] <<std::endl;
			//DSTR << "ID: " << ID <<"�̂Ƃ��̐ߓ_�ƑΖʂ���ʂŖʐϕ����v�Z����"<<std::endl;
			for(unsigned j=0;j<4;j++){
				if(mesh->tets[id].vertexIDs[j] == ID){					///	�`��֐����P�A�i���Ȃ킿�j����face�ɑΖʂ��钸�_�@�ƈ�v������@���̎���face�Ŗʐϕ�����
					///	j�Ԗڂ̍s��̐�����0�ɂ���matk2array�Ōv�Z����
					///	�O�k�ɂȂ����b�V���ʂ̖ʐς�0�ŏ��������Ă���
					faceVars[mesh->tets[id].faceIDs[l]].heatTransRatio = (vertexVars[mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[0]].heatTransRatio + vertexVars[mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[1]].heatTransRatio 
						+ vertexVars[mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[2]].heatTransRatio ) / 3.0;		///	���Yface�̔M�`�B�����\���ߓ_�ł̒l�̑������ς��Ƃ�
					///	�ȉ���[]�͏�܂ł�[l]�ƈقȂ�B
					///	ID�����Ԗڂ��ɂ���āA�`��֐��̌W�����قȂ�̂ŁA
					tetVars[id].matk[1] += faceVars[mesh->tets[id].faceIDs[l]].heatTransRatio * (1.0/12.0) * faceVars[mesh->tets[id].faceIDs[l]].area * matk2array[j];		//����tets[id].matk2 +=
					//DSTR << "tets[id].matk2��faces[tets[id].faces[l]].heatTransRatio * (1.0/12.0) * faces[tets[id].faces[l]].area * matk2array[" << j << "]"<< "�����Z: " <<faces[tets[id].faces[l]].heatTransRatio * (1.0/12.0) * faces[tets[id].faces[l]].area * matk2array[j] << std::endl;
					//DSTR << "tets[id].matk2 +=  " << tets[id].matk2 << std::endl;
				}
				//else{
				//	///	ID�ƈ�v���Ȃ��ꍇ�ɂ́Amatk2array[j]�ɂ͑S����0������
				//	///	�Ƃ������Ƃ��낾���A
				//	//matk2array[j] =0.0 * matk2array[j];
				//	//DSTR << "matk2array[" << j << "]: " << matk2array[j] << std::endl;
				//}
			}
		}
		///	SurfaceFace����Ȃ�������Amatk2array�ɂ�0������
		//else{
		//	//matk2array[l];
		//}
	}

	//DSTR << "matk2array:" << std::endl;
	//for(unsigned i=0;i<4;i++){
	//	DSTR <<i <<": " << matk2array[i] << std::endl;
	//}

	//k2 = k21 + k22 + k23 + k24
	//for(unsigned i=0; i < 4; i++){
	//	matk2 += matk2array[i];
	//	//	for debug
	//	//DSTR << "matk2 �� matk2array = k2" << i+1 <<"�܂ŉ��Z�����s��" << std::endl;
	//	//DSTR << matk2 << std::endl;
	//}
	
	//for debug
	//DSTR << "�ߓ_�i";
	//for(unsigned i =0; i < 4; i++){
	//	DSTR << tets[id].vertices[i] << "," ;
	//}
	//DSTR << ")�ō\�������l�ʑ̂�" << std::endl;
	//DSTR << "matk2 : " << std::endl;
	//DSTR << matk2 << std::endl;
	//int hogeshishi =0;
	
	//DSTR << "Inner Function _ matk2t tets[id].matk2: " <<tets[id].matk2 << std::endl;
}

//void PHFemThermo::CreateMatk2t_(unsigned id){
//
//	//l=0�̎�k21,1�̎�:k22, 2�̎�:k23, 3�̎�:k24	�𐶐�
//	for(unsigned l= 0 ; l < 4; l++){
//		//matk2array[l] = matk2temp;
//		matk2array[l] = Create44Mat21();
//		//	1�si���0��
//		for(int i=0;i<4;i++){
//			matk2array[l][l][i] = 0.0;
//		}
//		//	i�s1���0��
//		for(int i=0;i<4;i++){
//			matk2array[l][i][l] = 0.0;
//		}
//	}
//
//	///	������
//	tets[id].matk[1].clear();
//	//for(unsigned i =0; i < 4 ;i++){
//	//	for(unsigned j =0; j < 4 ;j++){
//	//		//tets[id].matk2[i][j] = 0.0;
//	//		tets[id].matk[1][i][j] = 0.0;
//	//	}
//	//}
//
//	//	Check
//	//DSTR << "matk2array:" << std::endl;
//	//for(unsigned i=0;i<4;i++){
//	//	DSTR <<i <<": " << matk2array[i] << std::endl;
//	//}
//	//DSTR << "++i" <<std::endl;
//	//for(unsigned i=0;i<4;++i){
//	//	DSTR <<i <<": " << matk2array[i] << std::endl;
//	//}
//
//
//	for(unsigned l= 0 ; l < 4; l++){
//		///	�l�ʑ̂̊e��(l = 0 �` 3) �ɂ��ă��b�V���\�ʂ��ǂ������`�F�b�N����B�\�ʂȂ�A�s��������matk2array�ɓ����
//		//faces[tets.faces[i]].sorted;		/// 1,24,58�݂����Ȑߓ_�ԍ��������Ă���
//		///	�s��^�̓��ꕨ��p��
//
//		//faces[tets.faces[l]].vertices;
//		if(tets[id].faces[l] < (int)nSurfaceFace && faces[tets[id].faces[l]].alphaUpdated ){			///	�O�k�̖� ���� �M�`�B�����X�V���ꂽ�� matk2���X�V����K�v������
//			//�Ō�ɓ����s���������
//			for(unsigned i =0; i < 4 ;i++){
//				for(unsigned j =0; j < 4 ;j++){
//					//matk2[i][j] = 0.0;
//					//tets[id].matk2[i][j] = 0.0;
//					tets[id].matk[1][i][j] = 0.0;
//				}
//			}
//			///	�l�ʑ̂̎O�p�`�̖ʐς��v�Z		///	���̊֐��̊O�Ŗʐϕ��̖ʐόv�Z����������B�ړ�����
//			if(faces[tets[id].faces[l]].area ==0 || faces[tets[id].faces[l]].deformed ){		///	�ʐς��v�Z����Ă��Ȃ����i�͂��߁j or deformed(�ό`�������E�������)��true�̎�		///	�����̒ǉ�	�ʐς�0�� ||(OR) �����X�V���ꂽ��
//				faces[tets[id].faces[l]].area = CalcTriangleArea(faces[tets[id].faces[l]].vertices[0], faces[tets[id].faces[l]].vertices[1], faces[tets[id].faces[l]].vertices[2]);
//				faces[tets[id].faces[l]].deformed = false;
//			}
//			///	�v�Z���ʂ��s��ɑ��
//			///	area�̌v�Z�Ɏg���Ă��Ȃ��_�������Ă���s�Ɨ���������s��̐ς��Ƃ�
//			///	�ϕ��v�Z�����{����l����
//			unsigned vtx = tets[id].vertices[0] + tets[id].vertices[1] + tets[id].vertices[2] + tets[id].vertices[3];
//			//DSTR << "vtx: " << vtx <<std::endl;
//			
//			///	area�v�Z�Ɏg���Ă��Ȃ��ߓ_ID�FID
//			unsigned ID = vtx -( faces[tets[id].faces[l]].vertices[0] + faces[tets[id].faces[l]].vertices[1] + faces[tets[id].faces[l]].vertices[2] );
//			//DSTR << "���b�V���\�ʂ̖ʂ͎���3���_����Ȃ�B" << std::endl;
//			//DSTR << "faces[tets.faces[l]].vertices[0]: " << faces[tets.faces[l]].vertices[0] <<std::endl;
//			//DSTR << "faces[tets.faces[l]].vertices[1]: " << faces[tets.faces[l]].vertices[1] <<std::endl;
//			//DSTR << "faces[tets.faces[l]].vertices[2]: " << faces[tets.faces[l]].vertices[2] <<std::endl;
//			//DSTR << "ID: " << ID <<"�̂Ƃ��̐ߓ_�ƑΖʂ���ʂŖʐϕ����v�Z����"<<std::endl;
//			for(unsigned k=0; k < faces[tets[id].faces[l]].heatTransRatios.size();++k){
//				for(unsigned j=0;j<4;j++){
//						if(tets[id].vertices[j] == ID){					///	�`��֐����P�A�i���Ȃ킿�j����face�ɑΖʂ��钸�_�@�ƈ�v������@���̎���face�Ŗʐϕ�����
//							///	j�Ԗڂ̍s��̐�����0�ɂ���matk2array�Ōv�Z����
//							///	�O�k�ɂȂ����b�V���ʂ̖ʐς�0�ŏ��������Ă���
//							faces[tets[id].faces[l]].heatTransRatio = (vertices[faces[tets[id].faces[l]].vertices[0]].heatTransRatio + vertices[faces[tets[id].faces[l]].vertices[1]].heatTransRatio 
//								+ vertices[faces[tets[id].faces[l]].vertices[2]].heatTransRatio ) / 3.0;		///	���Yface�̔M�`�B�����\���ߓ_�ł̒l�̑������ς��Ƃ�
//							///	�ȉ���[]�͏�܂ł�[l]�ƈقȂ�B
//							///	ID�����Ԗڂ��ɂ���āA�`��֐��̌W�����قȂ�̂ŁA
//							tets[id].matk[1] += faces[tets[id].faces[l]].heatTransRatios[k] * (1.0/12.0) * faces[tets[id].faces[l]].area * matk2array[j];		//����tets[id].matk2 +=
//							//DSTR << "tets[id].matk2��faces[tets[id].faces[l]].heatTransRatio * (1.0/12.0) * faces[tets[id].faces[l]].area * matk2array[" << j << "]"<< "�����Z: " <<faces[tets[id].faces[l]].heatTransRatio * (1.0/12.0) * faces[tets[id].faces[l]].area * matk2array[j] << std::endl;
//							//DSTR << "tets[id].matk2 +=  " << tets[id].matk2 << std::endl;
//						}
//					//else{
//					//	///	ID�ƈ�v���Ȃ��ꍇ�ɂ́Amatk2array[j]�ɂ͑S����0������
//					//	///	�Ƃ������Ƃ��낾���A
//					//	//matk2array[j] =0.0 * matk2array[j];
//					//	//DSTR << "matk2array[" << j << "]: " << matk2array[j] << std::endl;
//					//}
//				}
//			}
//		}
//		///	SurfaceFace����Ȃ�������Amatk2array�ɂ�0������
//		//else{
//		//	//matk2array[l];
//		//}
//	}
//
//	//DSTR << "matk2array:" << std::endl;
//	//for(unsigned i=0;i<4;i++){
//	//	DSTR <<i <<": " << matk2array[i] << std::endl;
//	//}
//
//	//k2 = k21 + k22 + k23 + k24
//	//for(unsigned i=0; i < 4; i++){
//	//	matk2 += matk2array[i];
//	//	//	for debug
//	//	//DSTR << "matk2 �� matk2array = k2" << i+1 <<"�܂ŉ��Z�����s��" << std::endl;
//	//	//DSTR << matk2 << std::endl;
//	//}
//	
//	//for debug
//	//DSTR << "�ߓ_�i";
//	//for(unsigned i =0; i < 4; i++){
//	//	DSTR << tets[id].vertices[i] << "," ;
//	//}
//	//DSTR << ")�ō\�������l�ʑ̂�" << std::endl;
//	//DSTR << "matk2 : " << std::endl;
//	//DSTR << matk2 << std::endl;
//	//int hogeshishi =0;
//	
//	//DSTR << "Inner Function _ matk2t tets[id].matk2: " <<tets[id].matk2 << std::endl;
//} 



bool PHFemThermo::SetConcentricHeatMap(std::vector<double> r, std::vector<double> temp, Vec2d origin){
	PHFemMeshNew* mesh = GetPHFemMesh();

	//���S����̋��������߂�
	double dx = 0.0;
	double dz = 0.0;
	for(unsigned i = 0; i < mesh->vertices.size(); ++i){
		if(vertexVars[i].disFromOrigin == 0){ 
			dx = mesh->vertices[i].pos.x - origin[0];
			dz = mesh->vertices[i].pos.z - origin[1];
			vertexVars[i].disFromOrigin = sqrt(dx * dx + dz * dz);
		}
	}
	//	�����܂�
	//> �\��face�̓��A���_����eface�̐ߓ_�̃��[�J��(x,z)���W�n�ł̕��ʏ�̋����̌v�Z���Aface�̑S�ߓ_��y���W�����̂��̂ɑ΂��āAIH���M�̉\���������t���O��ݒ�
	for(unsigned i=0;i<mesh->nSurfaceFace;i++){
		//> �\�ʂ�face�̑S�ߓ_��y���W�����Ȃ�΁A����face��IH���M��face�ʂƔ��肵�A�t���O��^����
		//if(faces[i].mayIHheated = true){
			//	(x,z)���ʂɂ�����mayIHheated��face�S�ߓ_�̌��_����̋������v�Z����
			for(unsigned j=0; j<3; j++){
				double dx = mesh->vertices[mesh->faces[i].vertexIDs[j]].pos.x - origin[0];
				double dz = mesh->vertices[mesh->faces[i].vertexIDs[j]].pos.z - origin[1];	//	�\�L��y�����A����z���W�������Ă���
				vertexVars[mesh->faces[i].vertexIDs[j]].disFromOrigin = sqrt( dx * dx + dz * dz);
			}
		//}
	}
	//���߂������ɉ����āA�ݒ肳�ꂽ���������𖞂����悤�ȉ��x���z�����

	//�C���^�t�F�[�X�����āA�p����B
	for(unsigned k=0; k < mesh->vertices.size(); ++k){
		for(size_t i=0; i < r.size()-1;++i){
			if(i>0 ){
				if( (r[i-1] + r[i]) / 2.0 < vertexVars[k].disFromOrigin && vertexVars[k].disFromOrigin <= ( r[i] + r[i+1]) / 2.0 ){
					vertexVars[k].temp = temp[i];
				}
			}
			//else if(i == r.size()-1){
			//	if(r[i] < vertices[k].disFromOrigin){
			//		if(temp.size() == r.size()){
			//			vertices[k].temp = temp[i];
			//		}
			//		else if(temp.size() < r.size()){
			//			vertices[k].temp = temp[temp.size() -1];
			//		}
			//	}
			//}
			else{
				if(vertexVars[k].disFromOrigin <=  (r[i] + r[i+1]) /2.0){
					vertexVars[k].temp = temp[i];
				} 
			}
		} 
	}
	if(r.size() != temp.size()) return -1;
	return 1;
}

void PHFemThermo::SetInitThermoConductionParam(double thConduct0,double roh0,double specificHeat0,double heatTrans0){
	thConduct = thConduct0;
	rho = roh0;
	specificHeat = specificHeat0;
	heatTrans = heatTrans0;
}
double PHFemThermo::Get_thConduct(){
	return thConduct;
}
void PHFemThermo::SetThermalEmissivityToVtx(unsigned id,double thermalEmissivity,double thermalEmissivity_const){
	vertexVars[id].thermalEmissivity = thermalEmissivity;
	vertexVars[id].thermalEmissivity_const = thermalEmissivity_const;
}
void PHFemThermo::SetThermalEmissivityToVerticesAll(double thermalEmissivity,double thermalEmissivity_const){
	for(unsigned i =0; i < GetPHFemMesh()->vertices.size(); i++){
		vertexVars[i].thermalEmissivity = thermalEmissivity;
		vertexVars[i].thermalEmissivity_const = thermalEmissivity_const;
	}
}
void PHFemThermo::SetHeatTransRatioToAllVertex(double heatTransR_){
	for(unsigned i =0; i < GetPHFemMesh()->vertices.size() ; i++){
		vertexVars[i].heatTransRatio = heatTransR_;
	}
	//�s���vecf�̔M�`�B������蒼��
	std::cout << "�s��̔M�`�B�������Ȃ����I"<<std::endl;
	DSTR << "�s��̔M�`�B�������Ȃ����I"<<std::endl;
}

void PHFemThermo::SetHeatTransRatioToAllVertex(){
	for(unsigned i =0; i < GetPHFemMesh()->vertices.size() ; i++){
		vertexVars[i].heatTransRatio = heatTrans;
	}
}

void PHFemThermo::SetTempAllToTVecAll(unsigned size){
	for(unsigned i =0; i < size;i++){
		TVecAll[i] = vertexVars[i].temp;
	}
}

void PHFemThermo::CreateTempVertex(){		//Resize and Set Temp to TVecAll 
	TVecAll.resize(GetPHFemMesh()->vertices.size());
	SetTempAllToTVecAll((unsigned)GetPHFemMesh()->vertices.size());
}

void PHFemThermo::SetTempToTVecAll(unsigned vtxid){

	TVecAll[vtxid] = vertexVars[vtxid].temp;
}

void PHFemThermo::UpdateheatTransRatio(unsigned id,double heatTransRatio){
	//if(vertices[id].heatTransRatio != heatTransRatio){	//�قȂ��Ă�����
	//	vertices[id].heatTransRatio = heatTransRatio;	
	//	///	�����܂ލs��̍X�V	K2,f3
	//	///	f3
	//	for(unsigned i =0; i < vertices[1].tets.size(); i++){
	//		CreateVecf3(tets[vertices[id].tets[i]]);
	//	}
	//	///	K3

	//}
	///	�����Ȃ牽�����Ȃ�
}

void PHFemThermo::SetLocalFluidTemp(unsigned i,double temp){
	vertexVars[i].Tc = temp;			//�ߓ_�̎��͗��̉��x�̐ݒ�
}

void PHFemThermo::SetVertexTemp(unsigned i,double temp){
	vertexVars[i].temp = temp;
	SetTempToTVecAll(i);
}

void PHFemThermo::SetVerticesTempAll(double temp){
	for(std::vector<unsigned int>::size_type i=0; i < GetPHFemMesh()->vertices.size() ; i++){
		vertexVars[i].temp = temp;
		SetTempToTVecAll((unsigned)i);	// �v�����FAftersetdesk�̒��ŌĂ΂�鎞�ATVecAll�̗v�f�̐����ł܂��Ă��炸�A�A�N�Z�X�ᔽ�̉\��������
	}
}

void PHFemThermo::AddvecFAll(unsigned id,double dqdt){
	vecFAllSum[id] += dqdt;		//	+=�ɕύX
	doCalc =true;
}

void PHFemThermo::SetvecFAll(unsigned id,double dqdt){
	vecFAllSum[id] = dqdt;		//	+=�ɕύX���ׂ��ŁA�폜�\��
}

void PHFemThermo::InitAllVertexTemp(){
	//	���̃��b�V���̑S���_�̉��x��0�ɂ���
	for(unsigned i=0; i < GetPHFemMesh()->vertices.size(); i++){
		vertexVars[i].temp = 0.0;
		//	�ǂ̃��b�V���Ń��Z�b�g���������@GetMe()->
	}
}

void PHFemThermo::InitMoist(){
	for(unsigned id =0; id < GetPHFemMesh()->tets.size(); id++){
		tetVars[id].wratio = 0.917;
		double rho = 970;
		if(tetVars[id].volume){
			tetVars[id].tetsMg = tetVars[id].volume * rho;	//����*���x
			tetVars[id].wmass = tetVars[id].tetsMg * tetVars[id].wratio;
		}else if(tetVars[id].volume < 0.0){
			DSTR << "tets[" << id << "]�̑̐ς��v�Z����Ă��܂���" << std::endl;
			tetVars[id].volume;
		}
	}
}

void PHFemThermo::DecrMoist_velo(double vel){
	PHFemMeshNew* mesh = GetPHFemMesh();

	//�������xver
	for(unsigned id =0; id < mesh->tets.size() ; id++){
		////���_��100�x�ȏ�Ŏc���ʂ��O�ł͂Ȃ��Ƃ�
		double tempAdd = 0.0;	//	���Z���x
		for(unsigned i=0; i < 4; i++){
			tempAdd += vertexVars[mesh->tets[id].vertexIDs[i]].temp;
		}
		//�P�ʊ��Z�͍����Ă��邩�H
		double wlatheat = 2.26 * 1000;		//��1kg������̐��M(latent heat)[W�Es]=[J] ���̐��M��540cal/g���J�ɕϊ����Ďg�p   W=J/s 2.26[kJ/kg]
		tetVars[id].tetsMg = tetVars[id].volume * rho;		//�l�ʑ̂̎���
		//�P�ʎ��Ԃ���������ʂ́Adw/dt = A/W * Rc  A:�������\�ʐρAW:�������̖���������
		double faceS=0.0;
		for(int ii=0;ii<4;ii++){
			int facet = mesh->tets[id].faceIDs[ii];
			if(facet <= (int)mesh->nSurfaceFace){
				faceS += faceVars[facet].area;
			}
		}
		//double surfaceS = tets[id].faces[  //	�l�ʑ̂ɑ�����face��nSurfaceface��菬�����ԍ���face�Ȃ�A�\�ʂ�face
		double sokudo = vel;
		double dwdt = faceS / tetVars[id].tetsMg * sokudo;//�P�ʎ��Ԃ�����Ȃ̂�dt�v�f������		//1step�Ō��鐅�̗�
		//double wlat = (tempAdd / 4.0) - 100.0;	//100�x�𒴂���������������B
		//double dw = dwdt * specificHeat *  tets[id].tetsMg / wlatheat;	//	����������	//	(���x�ƕ��_100�x�Ƃ̍����̔M��)�����̐��M�ŏ������鐅�̗ʂ�������B;		//	����������
		double exwater	= 0;	//���o���鐅�̗ʑS��(���� + ���o + �����ړ�)
		//���ω��x��100�x����

		//100�x�����F���C�����@�ɂ������E����
			//���ʂ�����
			tetVars[id].tetsMg -= dwdt;
			//�M�ʂ�D��	�l�ʑ̂̎��ʂ������Ă���M�ʂ���Adwdt�������		//	4�ߓ_�̕��ω��x�ł����̂��H	//�����ɖ��������肻���B
			double tetsheating = rho * specificHeat * tetVars[id].volume * (vertexVars[mesh->tets[id].vertexIDs[0]].temp + vertexVars[mesh->tets[id].vertexIDs[1]].temp + vertexVars[mesh->tets[id].vertexIDs[2]].temp + vertexVars[mesh->tets[id].vertexIDs[3]].temp)/4.0;

			//


		//100�x�ȏ�F���M�ɂ������E����


		//�ϐ��Ɉ˂闬�o�@�^���p�N���ϐ��E�\���ω��Ɛ������o



		//	�����A�ȉ��̃R�[�h�����B
		//if( tempAdd / 4.0 >= 100.0){
		//	//dw�̕������A���ʂ␅���ʂ������
		//	//double delw = (dt / 0.01 * 1.444*(0.000235/0.29)  / 10000000)*100;
		//	double delw = (1.444*(0.000235/0.29)  / 10000000)*100;
		//	exwater = delw * 500;
		//	tets[id].tetsMg -= dw - exwater;
		//	if(tets[id].wmass > dw - exwater){
		//		tets[id].wmass -= dw - exwater;
		//	}else{
		//		DSTR << "�������o�ʂ��������܂�" << std::endl;
		//	}
		//	//���؂���:�ЂƂ܂��Aexwater���O�łȂ���΁A�����Đ������邱�Ƃɂ��悤���B�����o������A���̃��b�V����exwater�̒l���O�ɂ��悤�B
		//	//wlat�̕������A���x�������
		//	for(unsigned j=0; j < 4; j++){
		//		vertices[j].temp -= dwdt;
		//	}
		//}
		//�Ƃ肠�����A�ȒP�ɁA���������炷�R�[�h
		//if(tets[id].wmass >= tets[id].wmass_start *0.01){
		//	tets[id].wmass -= tets[id].wmass_start * 0.01;
		//}
	}
}


void PHFemThermo::DecrMoist_vel(double dt){
	PHFemMeshNew* mesh = GetPHFemMesh();

	//�������xver
	for(unsigned id =0; id < mesh->tets.size() ; id++){
		////���_��100�x�ȏ�Ŏc���ʂ��O�ł͂Ȃ��Ƃ�
		double tempAdd = 0.0;	//	���Z���x
		for(unsigned i=0; i < 4; i++){
			tempAdd += vertexVars[mesh->tets[id].vertexIDs[i]].temp;
		}
		//�P�ʊ��Z�͍����Ă��邩�H
		double wlatheat = 2.26 * 1000;		//��1kg������̐��M(latent heat)[W�Es]=[J] ���̐��M��540cal/g���J�ɕϊ����Ďg�p   W=J/s 2.26[kJ/kg]
		tetVars[id].tetsMg = tetVars[id].volume * rho;		//�l�ʑ̂̎���
		//�P�ʎ��Ԃ���������ʂ́Adw/dt = A/W * Rc  A:�������\�ʐρAW:�������̖���������
		double faceS=0.0;
		for(int ii=0;ii<4;ii++){
			int facet = mesh->tets[id].faceIDs[ii];
			if(facet <= (int)mesh->nSurfaceFace){
				faceS += faceVars[facet].area;
			}
		}
		//double surfaceS = tets[id].faces[  //	�l�ʑ̂ɑ�����face��nSurfaceface��菬�����ԍ���face�Ȃ�A�\�ʂ�face
		double sokudo = 1.0;
		double dwdt = faceS / tetVars[id].tetsMg * sokudo;//�P�ʎ��Ԃ�����Ȃ̂�dt�v�f������
		//double wlat = (tempAdd / 4.0) - 100.0;	//100�x�𒴂���������������B
		double dw = dwdt * specificHeat *  tetVars[id].tetsMg / wlatheat;	//	����������	//	(���x�ƕ��_100�x�Ƃ̍����̔M��)�����̐��M�ŏ������鐅�̗ʂ�������B;		//	����������
		double exwater	= 0;	//���o���鐅�̗ʑS��(���� + ���o + �����ړ�)
		//���ω��x��100�x����
		if( tempAdd / 4.0 >= 100.0){
			//dw�̕������A���ʂ␅���ʂ������
			//double delw = (dt / 0.01 * 1.444*(0.000235/0.29)  / 10000000)*100;
			double delw = (1.444*(0.000235/0.29)  / 10000000)*100;
			exwater = delw * 500;
			tetVars[id].tetsMg -= dw - exwater;
			if(tetVars[id].wmass > dw - exwater){
				tetVars[id].wmass -= dw - exwater;
			}else{
				DSTR << "�������o�ʂ��������܂�" << std::endl;
			}
			//���؂���:�ЂƂ܂��Aexwater���O�łȂ���΁A�����Đ������邱�Ƃɂ��悤���B�����o������A���̃��b�V����exwater�̒l���O�ɂ��悤�B
			//wlat�̕������A���x�������
			for(unsigned j=0; j < 4; j++){
				vertexVars[j].temp -= dwdt;
			}
		}
		//�Ƃ肠�����A�ȒP�ɁA���������炷�R�[�h
		//if(tets[id].wmass >= tets[id].wmass_start *0.01){
		//	tets[id].wmass -= tets[id].wmass_start * 0.01;
		//}
	}
}


void PHFemThermo::DecrMoist(){
	PHFemMeshNew* mesh = GetPHFemMesh();

	for(unsigned id =0; id < mesh->tets.size() ; id++){
		////���_��100�x�ȏ�Ŏc���ʂ��O�ł͂Ȃ��Ƃ�
		double tempAdd = 0.0;	//	���Z���x
		for(unsigned i=0; i < 4; i++){
			tempAdd += vertexVars[mesh->tets[id].vertexIDs[i]].temp;
		}
		//�P�ʊ��Z�͍����Ă��邩�H
		double wlatheat = 2.26 * 1000;		//��1kg������̐��M(latent heat)[W�Es]=[J] ���̐��M��540cal/g���J�ɕϊ����Ďg�p   W=J/s 2.26[kJ/kg]
		tetVars[id].tetsMg = tetVars[id].volume * rho;		//�l�ʑ̂̎���
		//�P�ʎ��Ԃ���������ʂ́A
		double wlat = (tempAdd / 4.0) - 100.0;	//100�x�𒴂���������������B
		double dw = wlat * specificHeat *  tetVars[id].tetsMg / wlatheat;	//	����������	//	(���x�ƕ��_100�x�Ƃ̍����̔M��)�����̐��M�ŏ������鐅�̗ʂ�������B;		//	����������
		double exwater	= 0;	//���o���鐅�̗ʑS��(���� + ���o + �����ړ�)
		//���ω��x��100�x����
		if( tempAdd / 4.0 >= 100.0){
			//dw�̕������A���ʂ␅���ʂ������
			//double delw = (dt / 0.01 * 1.444*(0.000235/0.29)  / 10000000)*100;
			double delw = (1.444*(0.000235/0.29)  / 10000000)*100;
			exwater = delw * 500;
			tetVars[id].tetsMg -= dw - exwater;
			if(tetVars[id].wmass > dw - exwater){
				tetVars[id].wmass -= dw - exwater;
			}else{
				DSTR << "�������o�ʂ��������܂�" << std::endl;
			}
			//���؂���:�ЂƂ܂��Aexwater���O�łȂ���΁A�����Đ������邱�Ƃɂ��悤���B�����o������A���̃��b�V����exwater�̒l���O�ɂ��悤�B
			//wlat�̕������A���x�������
			for(unsigned j=0; j < 4; j++){
				vertexVars[j].temp -= wlat;
			}
		}
		//�Ƃ肠�����A�ȒP�ɁA���������炷�R�[�h
		//if(tets[id].wmass >= tets[id].wmass_start *0.01){
		//	tets[id].wmass -= tets[id].wmass_start * 0.01;
		//}
	}
}

void PHFemThermo::CalcVertexVolume(unsigned vtxid){
	PHFemMeshNew* mesh = GetPHFemMesh();
	vertexVars[vtxid].vVolume = 0;
	for(unsigned tetid=0; tetid < mesh->vertices[vtxid].tetIDs.size(); tetid++){
		vertexVars[vtxid].vVolume += tetVars[mesh->vertices[vtxid].tetIDs[tetid]].volume / 4;
	}
}

void PHFemThermo::VertexWaterBoiling(unsigned vtxid){
	PHFemMeshNew* mesh = GetPHFemMesh();
	unsigned vtxID = mesh->surfaceVertices[vtxid];
	
	if(TVecAll[vtxID] >= 100){
		double calorieOver100 = specificHeat * rho * vertexVars[vtxID].vVolume * (TVecAll[vtxID] - 100.0);	//100���𒴂��Ă��镪�̔M��
		double maxEvapoWaterMass = calorieOver100 / WaterLatentHeat;	//���������鎖���ł��鐅�̎���(kg)
		if(mesh->GetPHFemPorousWOMove()->GetVertexMw(vtxID) > maxEvapoWaterMass){
			TVecAll[vtxID] = 100.0;
			mesh->GetPHFemPorousWOMove()->SetVertexMw(vtxID, mesh->GetPHFemPorousWOMove()->GetVertexMw(vtxID) - maxEvapoWaterMass);
		}else if(mesh->GetPHFemPorousWOMove()->GetVertexMw(vtxID) > 0){
			double upTemp = (calorieOver100 - mesh->GetPHFemPorousWOMove()->GetVertexMw(vtxID) * WaterLatentHeat) / specificHeat / rho / vertexVars[vtxID].vVolume;
			mesh->GetPHFemPorousWOMove()->SetVertexMw(vtxID, 0);
			TVecAll[vtxID] = 100.0 + upTemp;
		}
	}
}

float PHFemThermo::calcGvtx(std::string fwfood, int pv, unsigned texture_mode){
	float gvtx = 0.0f;
	//�e�N�X�`���̐ݒ�
	//�ł��e�N�X�`���̖���
	unsigned kogetex	= 5;
	//�����e�N�X�`���̖���
	unsigned watex		= 2;
	//�T�[���e�N�X�`���̖���
	unsigned thtex		= 6;
	unsigned thcamtex   = 9;		//�M�J�������ۂ��\���p
	//	���[�h�e�N�X�`���[���ł����������x�̏�	�i�܂��́j�������x���ł�	�ɂ��ύX�\�i�t�@�C�����̃��l�[�����K�v�j

	//�����e�N�X�`���̖���(���̋�Ԗ��x��p�����ꍇ)
	unsigned watex2 = 5;

	// num of texture layers
	if(fwfood == "fwNegi"){		///	�e�N�X�`���Ɖ��x�A�����ʂƂ̑Ή��\�́ASamples/Physics/FEMThermo/�e�N�X�`���̐F�Ɖ��x�̑Ή�.xls	���Q�Ƃ̂���
		kogetex	= 5;
	}
	else if(fwfood == "fwNsteak"){
		kogetex	= 5;		//7�ɂ���
	}
	else if(fwfood == "fwPan"){
		kogetex = 5;
	}

	double dtex =(double) 1.0 / ( kogetex + thtex + watex + thcamtex);		//	�e�N�X�`�����s���W�̑w�Ԋu
	if(fwfood == "fwNsteak"){
		dtex =(double) 1.0 / ( kogetex + thtex + watex + thcamtex + watex2);
	}
	double texstart = dtex /2.0;										//	�e�N�X�`�����W�̏����l = �ł��e�N�X�`���̃X�^�[�g���W
	double wastart = texstart + kogetex * dtex;							//	�����ʕ\���e�N�X�`���̃X�^�[�g���W
	double thstart = texstart + kogetex * dtex + 1.0 * dtex;			//	�T�[���̃e�N�X�`���̃X�^�[�g���W �����e�N�X�`����2���ڂ���X�^�[�g
	double thcamstart = texstart + (thtex + kogetex + watex) * dtex;	//	

	if(texture_mode == 1){
		if(fwfood == "fwPan"){
			gvtx = texstart;// + dtex;		// �˂��ݐF�̒��
		}else if(fwfood == "fwNegi"){
			// ���x�ω��Ɠ����Ł@
			double temp = vertexVars[pv].temp;
			// -50.0~0.0:aqua to blue
			if(temp <= -50.0){
				gvtx = texstart + dtex;
			}
			else if(-50.0 < temp && temp <= 0.0){	
				gvtx = texstart + dtex;//(texstart ) + ((temp + 50.0) * dtex /50.0);
			}
			//	0~50.0:blue to green
			else if(0.0 < temp && temp <= 50.0 ){
				//double green = temp * dtex / 50.0 + thstart;
				gvtx = (temp - 50.0)  * dtex / 50.0 + texstart + dtex; //+     dtex;
			}
			//	50.0~100.0:green to yellow
			else if(50.0 < temp && temp <= 100.0){
				gvtx = (temp - 50.0 ) * dtex / 50.0 + texstart + dtex;// + 2 * dtex;
			}
			//	100.0~150:yellow to orange	
			else if(100.0 < temp && temp <= 150.0){
				gvtx = (temp - 50.0 ) * dtex / 50.0 + texstart + dtex;// + 2 * dtex;
			}
			//	150~200:orange to red
			else if(150.0 < temp && temp <= 200.0){
				double pinkc = (temp - 50.0 ) * dtex / 50.0 + thstart ;
				gvtx = dtex * 4.0 + texstart;//(temp - 50.0 ) * dtex / 50.0 + texstart + dtex;// + 2 * dtex;
			}
			//	200~250:red to purple
			else if(200.0 < temp && temp <= 250.0){
				gvtx = dtex * 4.0 + texstart;//(temp - 50.0 ) * dtex / 50.0 + texstart + dtex;// + 2 * dtex;
			}
			///	250~:only purple
			else if(250.0 < temp){
				gvtx = dtex * 4.0 + texstart;
				//gvtx[stride*gv + tex + 2] = wastart;			//whit
			}
			else{
				DSTR << "vertexVars[" << pv << "].temp = " << vertexVars[pv].temp << std::endl;
			}
		}
		else if(fwfood == "fwNsteak"){
			// ���x�ω��Ɠ����Ł@
			double temp = vertexVars[pv].temp;
			// -50.0~0.0:aqua to blue
			if(temp <= -50.0){
				gvtx = texstart + dtex;
			}
			else if(-50.0 < temp && temp <= 0.0){	
				gvtx = texstart + dtex;//(texstart ) + ((temp + 50.0) * dtex /50.0);
			}
			//	0~50.0:blue to green
			else if(0.0 < temp && temp <= 50.0 ){
				//double green = temp * dtex / 50.0 + thstart;
				gvtx = (temp - 50.0)  * dtex / 50.0 + texstart + dtex; //+     dtex;
			}
			//	50.0~100.0:green to yellow
			else if(50.0 < temp && temp <= 100.0){
				gvtx = (temp - 50.0 ) * dtex / 50.0 + texstart + dtex;// + 2 * dtex;
			}
			//	100.0~150:yellow to orange	
			else if(100.0 < temp && temp <= 150.0){
				gvtx = (temp - 50.0 ) * dtex / 50.0 + texstart + dtex;// + 2 * dtex;
			}
			//	150~200:orange to red
			else if(150.0 < temp && temp <= 200.0){
				double pinkc = (temp - 50.0 ) * dtex / 50.0 + thstart ;
				gvtx = (temp - 50.0 ) * dtex / 50.0 + texstart + dtex;// + 2 * dtex;
			}
			//	200~250:red to purple
			else if(200.0 < temp && temp <= 250.0){
				gvtx = (temp - 50.0 ) * dtex / 50.0 + texstart + dtex;// + 2 * dtex;
			}
			///	250~:only purple
			else if(250.0 < temp){
				gvtx = dtex * 6.0 + texstart;
				//gvtx[stride*gv + tex + 2] = wastart;			//white	 ///	�܂���ɂȂ����Ⴄ
			}
			else{
				DSTR << "vertexVars[" << pv << "].temp = " << vertexVars[pv].temp << std::endl;
			}
		}
		else if(fwfood == "tPan"){
			DSTR << "tPan are there" << std::endl;
		}
		int phmeshdebug =0;
	}else if(texture_mode == 3){
		//	���������\�����[�h
		//	�c�����ɉ������ω�
		gvtx = wastart + 2 * dtex;
		for(unsigned j =0; j < GetPHFemMesh()->tets.size(); j++){
			//	�������ł��ł�����
			if(0.5 < tetVars[j].wratio && tetVars[j].wratio < 1.0){
				gvtx = wastart + 2 * dtex - ( (tetVars[j].wratio -0.5) * (dtex / 0.5) );
			}
			else if(0.0 < tetVars[j].wratio && tetVars[j].wratio < 0.5){
				gvtx = wastart + 1 * dtex - ( (tetVars[j].wratio -0.5) * (dtex / 0.5) );
			}
		}
	}else if(texture_mode == 2){
		double temp = vertexVars[pv].temp;
		// -50.0~0.0:aqua to blue
		if(temp <= -50.0){
			gvtx = thstart;
		}
		else if(-50.0 < temp && temp <= 0.0){	
			gvtx = (thstart ) + ((temp + 50.0) * dtex /50.0);
		}
		//	0~50.0:blue to green
		else if(0.0 < temp && temp <= 50.0 ){
			//double green = temp * dtex / 50.0 + thstart;
			gvtx = temp * dtex / 50.0 + thstart + dtex;
		}
		//	50.0~100.0:green to yellow
		else if(50.0 < temp && temp <= 100.0){
			gvtx = (temp - 50.0 ) * dtex /	 50.0 + thstart + 2 * dtex;
		}
		//	100.0~150:yellow to orange	
		else if(100.0 < temp && temp <= 150.0){
			gvtx = (temp - 50.0 ) * dtex / 50.0 + thstart + 2 * dtex;
		}
		//	150~200:orange to red
		else if(150.0 < temp && temp <= 200.0){
			double pinkc = (temp - 50.0 ) * dtex / 50.0 + thstart ;
			gvtx = (temp - 50.0 ) * dtex / 50.0 + thstart + 2 * dtex;
		}
		//	200~250:red to purple
		else if(200.0 < temp && temp <= 250.0){
			gvtx = (temp - 50.0 ) * dtex / 50.0 + thstart + 2 * dtex;
		}
		///	250~:only purple
		else if(250.0 < temp){
			gvtx = dtex * 6.0 + thstart;
			//gvtx[stride*gv + tex + 2] = wastart;			//white	 ///	�܂���ɂȂ����Ⴄ
		}
		else{
			DSTR << "vertexVars[" << pv << "].temp = " << vertexVars[pv].temp << std::endl;
		}
	}else if(texture_mode == 4){
		double temp = vertexVars[pv].temp;
		// -50.0~0.0:aqua to blue => 20 : purple
		if(temp < 20.0){
			gvtx = thstart + 6.0 * dtex; 
		}
		else if(temp == 20.0){
			gvtx = thcamstart;
		}
		else if(20.0 < temp && temp <= 30.0){	
			gvtx = thcamstart + (temp - 20.0) * dtex / 10.0;
		}
		//	0~50.0:blue to green
		else if(30.0 < temp && temp <= 40.0 ){
			//double green = temp * dtex / 50.0 + thstart;
			gvtx = thcamstart + dtex + (temp - 30.0) * dtex / 10.0;
		}
		//	50.0~100.0:green to yellow
		else if(40.0 < temp && temp <= 50.0){
			gvtx = thcamstart + 2 * dtex + (temp - 40.0) * dtex / 10.0;
		}
		//	100.0~150:yellow to orange	
		else if(50.0 < temp && temp <= 60.0){
			gvtx = thcamstart + 3 * dtex + (temp - 50.0) * dtex / 10.0;
		}
		//	150~200:orange to red
		else if(60.0 < temp && temp <= 70.0){
			gvtx = thcamstart + 4 * dtex + (temp - 60.0) * dtex / 10.0;
		}
		//	200~250:red to purple
		else if(70.0 < temp && temp <= 80.0){
			gvtx = thcamstart + 5 * dtex + (temp - 70.0) * dtex / 10.0;
		}
		///	250~:only purple
		else if(80.0 < temp && temp <= 90.0){
			gvtx = thcamstart + 6 * dtex + (temp - 80.0) * dtex / 10.0;
		}
		else if(90.0 < temp && temp <= 100.0){
			gvtx = thcamstart + 7 * dtex + (temp - 90.0) * dtex / 10.0;
		}
		else if(100.0 < temp){
			gvtx = thcamstart + 8 * dtex;
		}
		else{
			DSTR << "vertexVars[" << pv << "].temp = " << vertexVars[pv].temp << std::endl;
		}
	}
	return gvtx;
}

void PHFemThermo::CreateVecFAll(){
	for(unsigned tetid=0; tetid < GetPHFemMesh()->tets.size(); tetid++){
		CreateVecFAll(tetid);
	}
}

void PHFemThermo::CalcFaceNormalAll(){
	PHFemMeshNew* mesh = GetPHFemMesh();

	//	face�̖@�����v�Z
	//.	�\�ʂ̒��_�ɁA�@���x�N�g����ǉ�
	//.	�ɂ��čċA�I�Ɏ��s
	Vec3d extp;		//	�O�����@��
	Vec3d tempV;	//	�O���������r���_(�Y��face�ʏ�ɂȂ����_����)
	DSTR << "tets.size(): " << mesh->tets.size() << std::endl;
	for(unsigned tid=0; tid < mesh->tets.size(); tid++){
		//	�ǂ̒��_ID��face���\������Ă���̂�
		unsigned idsum = 0;
		for(unsigned i=0;i<4;i++){
			idsum += mesh->tets[tid].vertexIDs[i];
		}
		for(unsigned fid = 0; fid < 4; fid++){
			//DSTR << "fid :" << fid <<std::endl;
			extp = (mesh->vertices[mesh->faces[mesh->tets[tid].faceIDs[fid]].vertexIDs[1]].pos - mesh->vertices[mesh->faces[mesh->tets[tid].faceIDs[fid]].vertexIDs[0]].pos)
				% (mesh->vertices[mesh->faces[mesh->tets[tid].faceIDs[fid]].vertexIDs[2]].pos - mesh->vertices[mesh->faces[mesh->tets[tid].faceIDs[fid]].vertexIDs[0]].pos);
			extp = extp / extp.norm();
			Vec3d chkN[2] = {mesh->vertices[mesh->faces[mesh->tets[tid].faceIDs[fid]].vertexIDs[1]].pos - mesh->vertices[mesh->faces[mesh->tets[tid].faceIDs[fid]].vertexIDs[2]].pos
				, mesh->vertices[mesh->faces[mesh->tets[tid].faceIDs[fid]].vertexIDs[2]].pos - mesh->vertices[mesh->faces[mesh->tets[tid].faceIDs[fid]].vertexIDs[1]].pos};
			if(extp * chkN[0]/(extp.norm() * chkN[0].norm()) > 1e-15 ){		// 1e-17���炢0���傫���A���S�Ȗ@���ɂ͂Ȃ��Ă��Ȃ�����
				DSTR << "this normal is invalid. make sure to check it out. " << "tid: "<< tid << ", fid: " << fid << " ; "<< this->GetName() << std::endl;
				DSTR << "the invalid value is... " << extp * chkN[0]/(extp.norm() * chkN[0].norm()) <<", " << extp * chkN[1]/(extp.norm() * chkN[1].norm()) << std::endl;
				assert(0);
			}
			if(extp == 0){
				DSTR << "ERROR: extp value == 0" << "tid = " << tid << ", fid = " << fid << std::endl;
			}
	
			//unsigned expVtx =0;		//	face�ʏ�ɂȂ��A0~3�Ԗڂ̎l�ʑ̒��_
			unsigned idsumt =idsum;
			for(unsigned j=0;j<3;j++){
				idsumt -= mesh->faces[mesh->tets[tid].faceIDs[fid]].vertexIDs[j];
				//DSTR << "faces[" << fid << "].vertices["<<j <<"]: "<< faces[tets[tid].faces[fid]].vertices[j];
			}
			
			//. face�d�S����face�O���_�ւ̃x�N�g��tempV�v�Z
			Vec3d jushin = mesh->vertices[mesh->faces[mesh->tets[tid].faceIDs[fid]].vertexIDs[0]].pos + mesh->vertices[mesh->faces[mesh->tets[tid].faceIDs[fid]].vertexIDs[1]].pos
				+ mesh->vertices[mesh->faces[mesh->tets[tid].faceIDs[fid]].vertexIDs[2]].pos;
			jushin *= 1.0 / 3.0;
			tempV = mesh->vertices[idsumt].pos - jushin;
			//DSTR << "tempV:" << tempV <<std::endl;
			if(tempV==Vec3d(0.0,0.0,0.0)){
				DSTR <<"ERROR:	for normal calculating, some vertices judging is invalids"<< std::endl;
			}
			if((tempV * extp / (tempV.norm() * extp.norm()) ) < 0.0){
				//extp��tempV���}�X�O�x�ȏ㗣��Ă���Fextp���O�����@��
				faceVars[mesh->tets[tid].faceIDs[fid]].normal = extp / 10.0;		//	�������P0cm��
			}else{
				//extp��tempV���X�O�x�ȓ��Fextp�̌�����180�x�ς��āAfaces[fid].normal�ɑ��
				faceVars[mesh->tets[tid].faceIDs[fid]].normal = - extp / 10.0;		// �t�x�N�g��
			}
			
			int debughogeshi=0;
		}
		//Debug
		//�Sface�ɁA�O�����@���x�N�g����\�������Ă݂āA�l�q������΁A�m�F�ł��邩�ȁH���́A�V���v���ȃ��b�V���ŕ\�����Ă݂邩
	}

	//	���_�̖@�����v�Z
	//	���_�̑�����face�ʂ�蕽�ρH���K���������_�@�������߂�
	std::vector<Vec3d> faceNormal;
	faceNormal.clear();
	for(unsigned vid = 0; vid < mesh->vertices.size(); vid++ ){
		//unsigned fsize = vertices[vid].faces.size();
		for(unsigned fid = 0; fid < mesh->vertices[vid].faceIDs.size(); fid++ ){
			//.	������face�@�����قړ��������������Ă���̂����������ꍇ�́A1�������Z���ĕ��ς��Ƃ�悤�ɕύX����

			//�O���̒��_�̖@���������Z			
			if(mesh->vertices[vid].faceIDs[fid] < (int)mesh->nSurfaceFace){
				vertexVars[vid].normal += faceVars[mesh->vertices[vid].faceIDs[fid]].normal;		// ���̃R�[�h�ɑ����āA��Lvector�R�[�h�ƈȉ��̉��Z�R�[�h�ɒu������
			}

		}
		vertexVars[vid].normal = vertexVars[vid].normal / vertexVars[vid].normal.norm();		//	�P�ʃx�N�g����
	}
}

void PHFemThermo::CalcVertexNormalAll(){
	PHFemMeshNew* mesh = GetPHFemMesh();

	//	���_�̖@�����v�Z
	//	���_�̑�����face�ʂ�蕽�ρH���K���������_�@�������߂�
	std::vector<Vec3d> faceNormal;
	faceNormal.clear();
	for(unsigned vid = 0; vid < mesh->vertices.size(); vid++ ){
		//unsigned fsize = vertices[vid].faces.size();
		for(unsigned fid = 0; fid < mesh->vertices[vid].faceIDs.size(); fid++ ){
			//.	������face�@�����قړ��������������Ă���̂����������ꍇ�́A1�������Z���ĕ��ς��Ƃ�悤�ɕύX����

			//�O���̒��_�̖@���������Z			
			if(mesh->vertices[vid].faceIDs[fid] < (int)mesh->nSurfaceFace){
				vertexVars[vid].normal += faceVars[mesh->vertices[vid].faceIDs[fid]].normal;		// ���̃R�[�h�ɑ����āA��Lvector�R�[�h�ƈȉ��̉��Z�R�[�h�ɒu������
			}

		}
		vertexVars[vid].normal = vertexVars[vid].normal / vertexVars[vid].normal.norm();		//	�P�ʃx�N�g����
	}
}

void PHFemThermo::InitFaceNormalAll(){
	for(unsigned faceid=0; faceid < GetPHFemMesh()->faces.size(); faceid++){
		faceVars[faceid].normal = Vec3d(0.0, 0.0, 0.0);
	}
}
void PHFemThermo::InitVertexNormalAll(){
	for(unsigned vtxid=0; vtxid < GetPHFemMesh()->vertices.size(); vtxid++){
		vertexVars[vtxid].normal = Vec3d(0.0, 0.0, 0.0);
	}
}

void PHFemThermo::RevVertexNormalAll(){
	PTM::TMatrixRow<3,3,double> revMat;
	revMat.clear();
	revMat[0][0] = -1.0;
	revMat[1][1] = -1.0;
	revMat[2][2] = -1.0;
	for(unsigned vtxid=0; vtxid < GetPHFemMesh()->vertices.size(); vtxid++){
		vertexVars[vtxid].normal = revMat * vertexVars[vtxid].normal;
	}
}

void PHFemThermo::CalcMatKup(){
#ifdef badMatK
	PHFemMeshNew* mesh = GetPHFemMesh();
	matkupdate.clear();
	for(unsigned i=0; i < mesh->vertices.size(); i++){
		double total = 0.0;
		for(unsigned j=0; j < mesh->vertices.size(); j++){
			if(i!=j){
				total += fabs(matKAll[i][j]);
			}
		}
		if(matKAll[i][i] <= total){
			matkupdate.push_back((total - matKAll[i][i]) + dMatKinf);
		}else{
			matkupdate.push_back(0.0);
		}
	}
#endif
}

void PHFemThermo::CalcEdgeLengthAll(){
	PHFemMeshNew* mesh = GetPHFemMesh();
	for(unsigned edgeid=0; edgeid < mesh->edges.size(); edgeid++){
		double length = 0.0;
		length += pow((mesh->vertices[mesh->edges[edgeid].vertexIDs[0]].pos[0] - mesh->vertices[mesh->edges[edgeid].vertexIDs[1]].pos[0]),2.0);
		length += pow((mesh->vertices[mesh->edges[edgeid].vertexIDs[0]].pos[1] - mesh->vertices[mesh->edges[edgeid].vertexIDs[1]].pos[1]),2.0);
		length += pow((mesh->vertices[mesh->edges[edgeid].vertexIDs[0]].pos[2] - mesh->vertices[mesh->edges[edgeid].vertexIDs[1]].pos[2]),2.0);
		edgeVars[edgeid].length = sqrt(length);
	}
}

double PHFemThermo::GetMaxMinEdgeRatioInTet(unsigned tetId){
	PHFemMeshNew* mesh = GetPHFemMesh();
	double maxL = 0.0;
	double minL = 0.0;
	for(unsigned edgeid=0; edgeid < 6; edgeid++){
		double currentL = edgeVars[mesh->tets[tetId].edgeIDs[edgeid]].length;
		if(edgeid == 0){
			maxL = currentL;
			minL = currentL;
		}else{
			if(currentL > maxL){
				maxL = currentL;
			}
			if(currentL < minL){
				minL = currentL;
			}
		}
	}
	return maxL / minL;
}

void PHFemThermo::OutEig(){
	PHFemMeshNew* mesh = GetPHFemMesh();
	PTM::VMatrixRow<double> eig;

#ifndef UseScilab
	if (!ScilabStart()) DSTR << "Error : ScilabStart \n";
	else DSTR << "ScilabStart \n";
#endif
	ScilabSetMatrix("c", matCAll);
	ScilabSetMatrix("k", tdt*matKAll);
	ScilabJob("eig = spec(inv(k+c)*c)");
	ScilabGetMatrix(eig, "eig");
#ifndef UseScilab
	ScilabEnd();
#endif

	std::ofstream eigOut;
	eigOut.open("eigOut.csv");
	for(unsigned i=0; i < mesh->vertices.size(); i++){
		eigOut << eig[i][0] << std::endl;
	}
	eigOut.close();
}

void PHFemThermo::VecFNegativeCheck(){
	PHFemMeshNew* mesh = GetPHFemMesh();
	bool flag = false;
	for(unsigned i=0; i < mesh->vertices.size(); i++){
		if(vecFAllSum[i] < 0){
			DSTR << i << ":" << vecFAllSum[i] << std::endl;
			flag = true;
		}
	}
	if(flag)
		DSTR << std::endl;
}

void PHFemThermo::SetVerticesPreTempAll(double temp){
	for(unsigned vtxid=0; vtxid < GetPHFemMesh()->vertices.size(); vtxid++){
		preTVecAll[vtxid] = temp;
	}
}

PTM::TMatrixRow<4,4,double> PHFemThermo::Create44Mat50(){
	//|5 0 0 0 |
	//|0 5 0 0 |
	//|0 0 5 0 |
	//|0 0 0 5 |	�����
	PTM::TMatrixRow<4,4,double> MatTemp;
	for(int i =0; i <4 ; i++){
		for(int j=0; j < 4 ; j++){
			if(i==j){
				MatTemp[i][j] = 5.0;
			}else{
				MatTemp[i][j] = 0.0;
			}
		}
	}
	return MatTemp;
}

void PHFemThermo::matKModif(){
	PHFemMeshNew* mesh = GetPHFemMesh();

	for(unsigned i=0; i < mesh->vertices.size(); i++){
		for(unsigned j=0; j < mesh->vertices.size(); j++){
			if(i!=j){
				if(matKAll[i][j] > 0){
					matKAll[i][i] += 2*matKAll[i][j];
					matKAll[i][j] *= -1;
				}
			}
		}
	}
}

void PHFemThermo::makeFPartition(unsigned num){
	PHFemMeshNew* mesh = GetPHFemMesh();

	fpat.resize(mesh->vertices.size(), mesh->vertices.size());
	fpat.clear();
	double cmin = matCAll[1][1];
	for(unsigned i=0; i < mesh->vertices.size(); i++){
		for(unsigned j=0; j < mesh->vertices.size(); j++){
			if(matCAll[i][j] != 0){
				if(matCAll[i][j] < cmin){
					cmin = matCAll[i][j];
				}
			}
		}
	}
	cratio = 1 / cmin * matCAll;

	for(unsigned i=0; i < mesh->vertices.size(); i++){
		if(num == 0){
			fpat[i][i] = 1.0;
		}else{
			PTM::VVector<double> ftemp;
			ftemp = fPartition(i, 1, num-1);
			for(unsigned j=0; j < mesh->vertices.size(); j++){
				fpat[j][i] = ftemp[j];
			}
		}
	}
}

PTM::VVector<double> PHFemThermo::fPartition(unsigned vid, double f, unsigned num){
	unsigned vsize = (unsigned)GetPHFemMesh()->vertices.size();
	double cratioSum = 0;
	PTM::VVector<double> fsub;
	fsub.resize(vsize);
	fsub.clear();

	for(unsigned i=0; i < vsize; i++){
		cratioSum += cratio[i][vid];
	}

	for(unsigned i=0; i < vsize; i++){
		if(num == 0){
			fsub[i] = f * cratio[i][vid] / cratioSum;
		}else{
			fsub += fPartition(i, f*cratio[i][vid]/cratioSum, num-1);
		}
	}
	return fsub;
}

double PHFemThermo::sumVectorElement(PTM::VVector<double> vec){
	double sum = 0;
	for(unsigned i=0; i < vec.size(); i++){
		sum += vec[i];
	}
	return sum;
}

void PHFemThermo::createNewVecF(){
	if(vecFAllSum.norm() != 0){
		PHFemMeshNew* mesh = GetPHFemMesh();
		double fmin = 0;
		double alpha;
		PTM::VVector<double> vecFratio;
		for(unsigned i=0; i < mesh->vertices.size(); i++){
			if(fmin == 0){
				if(vecFAllSum[i] != 0){
					fmin = vecFAllSum[i];
				}
			}else{
				if(vecFAllSum[i] > 0 && vecFAllSum[i] < fmin){
					fmin = vecFAllSum[i];
				}
			}
		}

		vecFratio = 1 / fmin * vecFAllSum;
		//alpha = (sumVectorElement(vecFAllSum) - sumVectorElement(matKAll*TVecAll)) / sumVectorElement((matKAll+1/tdt*matCAll)*vecFratio);
		alpha = sumVectorElement(vecFAllSum) / sumVectorElement(1/tdt*matCAll*vecFratio);
		//alpha = (sumVectorElement(vecFAllSum) - sumVectorElement(matKAll*TVecAllSub)) / sumVectorElement((matKAll+1/tdt*matCAll)*vecFratio);
		//vecFAllSum = matKAll * TVecAll + alpha * (matKAll + 1 / tdt * matCAll) * vecFratio;
		vecFAllSum = alpha * 1 / tdt * matCAll * vecFratio;
		//vecFAllSum = matKAll * TVecAllSub + alpha * (matKAll + 1 / tdt * matCAll) * vecFratio;

		//for(unsigned i=0; i < mesh->vertices.size(); i++){
		//	DSTR << vecFAllSum[i] << std::endl;
		//}
		//DSTR << std::endl;
	}
}

void PHFemThermo::CalcHeatTransDirect3(double dt){
	PHFemMeshNew* mesh = GetPHFemMesh();
	//if(strcmp(mesh->GetName(), "femNsteak") == 0){
	//	for(unsigned i=0; i < mesh->vertices.size(); i++){
	//		DSTR << vecFAllSum[i] << std::endl;
	//	}
	//	DSTR << std::endl;
	//}

	//���ږ@���p

	double eps =0.5;
	double eps2 = 0.5;
	double eps3 = 1.0;
	eps = 1.0;
	
//	DSTR << "tets[0].volume: " << tets[0].volume << std::endl;
	
	TVecAll2.resize(mesh->vertices.size());
	TVecAll2 = TVecAll;


	PTM::VVector<double> TVecAll3;	
	//// define @.h
	TVecAll3.resize(mesh->vertices.size());
	TVecAll3 = TVecAll;


	//	�W���s��̍쐬
	keisu.resize(mesh->vertices.size(),mesh->vertices.size());
	keisu.clear();

	PTM::VVector<double> uhen;
	uhen.resize(mesh->vertices.size(),1);
	uhen.clear();

	double TEMP =0.0;
	for(unsigned i=0; i<mesh->vertices.size();i++){
		TEMP += vecFAllSum[i];
	}

	double alpha = 0;
	PTM::VVector<double> vecFratio;
	vecFratio.resize(mesh->vertices.size());
	vecFratio.clear();

	if(vecFAllSum.norm() != 0){
		double fmin = 0;
		for(unsigned i=0; i < mesh->vertices.size(); i++){
			if(fmin == 0){
				if(vecFAllSum[i] != 0){
					fmin = vecFAllSum[i];
				}
			}else{
				if(vecFAllSum[i] > 0 && vecFAllSum[i] < fmin){
					fmin = vecFAllSum[i];
				}
			}
		}

		vecFratio = 1 / fmin * vecFAllSum;
		alpha = sumVectorElement(vecFAllSum) / sumVectorElement(1/tdt*matCAll*vecFratio);
		//vecFAllSum = alpha * 1 / tdt * matCAll * vecFratio;

		//for(unsigned i=0; i < mesh->vertices.size(); i++){
		//	DSTR << vecFAllSum[i] << std::endl;
		//}
		//DSTR << std::endl;
	}

	keisu = eps * matKAll + 1 / dt * matCAll; 
	
	if(deformed){ 
		keisuInv = keisu.inv();
		deformed = false;
		rightkeisu = keisuInv*matCAll;
		//for(unsigned i=0; i < mesh->vertices.size(); i++){
		//	for(unsigned j=0; j < mesh->vertices.size(); j++){
		//		if(i != j){
		//			if(rightkeisu[i][j] < 0){
		//				rightkeisu[i][i] += 2 * rightkeisu[i][j];
		//				rightkeisu[i][j] = (-1) * rightkeisu[i][j];
		//			}
		//		}
		//	}
		//}
	}

	TVecAll = 1/dt*rightkeisu*(TVecAll+alpha*vecFratio);
}

void PHFemThermo::CalcHeatTransDirect4(double dt){
	PHFemMeshNew* mesh = GetPHFemMesh();
	//if(strcmp(mesh->GetName(), "femNsteak") == 0){
	//	for(unsigned i=0; i < mesh->vertices.size(); i++){
	//		DSTR << vecFAllSum[i] << std::endl;
	//	}
	//	DSTR << std::endl;
	//}

	//���ږ@���p

	double eps =0.5;
	double eps2 = 0.5;
	double eps3 = 1.0;
	eps = 1.0;
	
//	DSTR << "tets[0].volume: " << tets[0].volume << std::endl;
	
	TVecAll2.resize(mesh->vertices.size());
	TVecAll2 = TVecAll;


	PTM::VVector<double> TVecAll3;	
	//// define @.h
	TVecAll3.resize(mesh->vertices.size());
	TVecAll3 = TVecAll;


	//	�W���s��̍쐬
	keisu.resize(mesh->vertices.size(),mesh->vertices.size());
	keisu.clear();

	PTM::VVector<double> uhen;
	uhen.resize(mesh->vertices.size(),1);
	uhen.clear();

	double TEMP =0.0;
	for(unsigned i=0; i<mesh->vertices.size();i++){
		TEMP += vecFAllSum[i];
	}

	double alpha = 0;
	PTM::VVector<double> vecFratio;
	vecFratio.resize(mesh->vertices.size());
	vecFratio.clear();

	for(unsigned i=0; i < mesh->vertices.size(); i++){
		vecFratio[i] = vecFAllSum[i] / vertexVars[i].vVolume;
	}

	if(vecFratio.norm() != 0){
		double fmin = 0;
		for(unsigned i=0; i < mesh->vertices.size(); i++){
			if(fmin == 0){
				if(vecFratio[i] != 0){
					fmin = vecFratio[i];
				}
			}else{
				if(vecFratio[i] > 0 && vecFratio[i] < fmin){
					fmin = vecFratio[i];
				}
			}
		}

		vecFratio = 1 / fmin * vecFratio;
		alpha = sumVectorElement(vecFAllSum) / sumVectorElement(1/tdt*matCAll*vecFratio);
		//vecFAllSum = alpha * 1 / tdt * matCAll * vecFratio;

		//for(unsigned i=0; i < mesh->vertices.size(); i++){
		//	DSTR << vecFAllSum[i] << std::endl;
		//}
		//DSTR << std::endl;
	}

	keisu = eps * matKAll + 1 / dt * matCAll; 
	
	if(deformed){ 
		keisuInv = keisu.inv();
		deformed = false;
		rightkeisu = keisuInv*matCAll;
		//for(unsigned i=0; i < mesh->vertices.size(); i++){
		//	for(unsigned j=0; j < mesh->vertices.size(); j++){
		//		if(i != j){
		//			if(rightkeisu[i][j] < 0){
		//				rightkeisu[i][i] += 2 * rightkeisu[i][j];
		//				rightkeisu[i][j] = (-1) * rightkeisu[i][j];
		//			}
		//		}
		//	}
		//}
	}

	TVecAll = 1/dt*rightkeisu*(TVecAll+alpha*vecFratio);
}

void PHFemThermo::vecFOutToDSTR(){
	for(unsigned i=0; i < GetPHFemMesh()->vertices.size(); i++){
		DSTR << vecFAllSum[i] << std::endl;
	}
	DSTR << std::endl;
}

void PHFemThermo::vecFOutToFile(){
	std::ofstream vecFThermo;
	vecFThermo.open("vecFThermo.csv",std::ios::app);
	for(unsigned i=0; i < GetPHFemMesh()->vertices.size(); i++){
		vecFThermo << vecFAllSum[i] << ",";
	}
	vecFThermo << std::endl;
	vecFThermo.close();
}

void PHFemThermo::vecFBottomOutToFile(){
	std::ofstream vecFThermo;
	vecFThermo.open("vecFBottomThermo.csv",std::ios::app);
	for(unsigned i=0; i < bottomVertexNums.size(); i++){
		vecFThermo << vecFAllSum[bottomVertexNums[i]] << ",";
	}
	vecFThermo << std::endl;
	vecFThermo.close();
}

void PHFemThermo::vecFTopOutToFile(){
	std::ofstream vecFThermo;
	vecFThermo.open("vecFTopThermo.csv",std::ios::app);
	for(unsigned i=0; i < topVertexNums.size(); i++){
		vecFThermo << vecFAllSum[topVertexNums[i]] << ",";
	}
	vecFThermo << std::endl;
	vecFThermo.close();
}

//PTM::VMatrixRow<double> PHFemThermo::inv(PTM::VMatrixRow<double> mat){
//	PTM::VMatrixRow<double> result;
//
//	result.resize(mat.height(),mat.width());
//	result.clear();
//
//	int m = mat.height();//�s��
//	int n = mat.width();//��
//	int lda = n;
//	double *A = new double[m*n];
//	int info;
//	int *ipiv = new int[std::min(m,n)];
//	int lwork = m;
//	double *work = new double[m];
//	
//	for(int i=0; i < m; i++){
//		for(int j=0; j < n; j++){
//			A[j*m+i] = mat[i][j];
//		}
//	}
//
//	dgetrf_(&n, &m, A, &lda, ipiv, &info);
//	dgetri_(&m, A, &lda, ipiv, work, &lwork, &info);
//
//	for(int i=0; i < m; i++){
//		for(int j=0; j < n; j++){
//			result[i][j] = A[j*m+i];
//		}
//	}
//
//	delete[] A;
//	delete[] ipiv;
//	delete[] work;
//
//	return result;
//}

}


