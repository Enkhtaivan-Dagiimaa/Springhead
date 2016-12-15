/*
 *  Copyright (c) 2003 - 2011, Fumihiro Kato, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include <SciLab/SprSciLab.h>
#include <Physics/PHFemPorousWOMove.h>
#include <Base/Affine.h>

#define NOMINMAX
#include "windows.h"

#include <Foundation/UTClapack.h>
#include <iomanip>
#include <time.h>
#include <direct.h>
#include <sstream>

//#define Scilab

//#define shrink

//#define calcWOMotion	//�����ړ��v�Z���s���ꍇ�̓R�����g�A�E�g���O��
//#define UseBoundWater	//�������Ǝ��R���ɕ�����ꍇ�̓R�����g�A�E�g���O��
#define usingG
//#define aboutG
#define saturationLimit	//�O�a���̍ő�l��1�ɐݒ�

using namespace PTM;

namespace Spr{;

PHFemPorousWOMoveDesc::PHFemPorousWOMoveDesc(){
	Init();
}

void PHFemPorousWOMoveDesc::Init(){
	wDiffAir = 0.0000288;
	K = 3.0 / pow(10.0,4.0);
	kc = 15000.0;
	kp = 5.0 * exp(4.0);
	alpha = 0.1;
	gamma = 1.0;
	rhoWater = 1000.0; //���̖��x
	rhoOil = 900.0; //���̖��x
	evapoRate = 10;
	boundWaterRatio = 0.2;	//�������̊���
	denatTemp = 80.0;
	equilWaterCont = 0.0;
	limitWaterCont = 1.0;
	boundaryThick = 0.001;
	initMassAll = 95;
	initWaterRatio = 0.583;
	initOilRatio = 0.193;
	shrinkageRatio = 0.8;
	top = Vec3d(0.0, 0.0149, 0.0);		//�����̃��b�V��
	center = Vec3d(0.0, 0.0, 0.0);			//�����̃��b�V��
	bottom = Vec3d(0.0, -0.0149, 0.0);	//�����̃��b�V��
	//top = Vec3d(0.0, 0.00749, 0.0);			//�~�����b�V��
	//center = Vec3d(0.0, 0.0, 0.0);			//�~�����b�V��
	//bottom = Vec3d(0.0, -0.00749, 0.0);		//�~�����b�V��
}

PHFemPorousWOMove::PHFemPorousWOMove(const PHFemPorousWOMoveDesc& desc, SceneIf* s){
	SetDesc(&desc);
	if(s){SetScene(s);}
	StepCount = 0;
	StepCount_ = 0;
}

void PHFemPorousWOMove::Init(){

	eps = 1.0;

	time_t startTime = time(NULL);
	std::string dataDir;
	struct tm *pnow = localtime(&startTime);
	std::stringstream ss;
	ss << "Data/" << pnow->tm_year+1900;
	
	if((pnow->tm_mon+1) < 10){
		ss << "0" << pnow->tm_mon+1;
	}else{
		ss << pnow->tm_mon+1;
	}

	if(pnow->tm_mday < 10){
		ss << "0" << pnow->tm_mday;
	}else{
		ss << pnow->tm_mday;
	}

	if(pnow->tm_hour < 10){
		ss << "0" << pnow->tm_hour;
	}else{
		ss << pnow->tm_hour;
	}

	if(pnow->tm_min < 10){
		ss << "0" << pnow->tm_min;
	}else{
		ss << pnow->tm_min;
	}

	if(pnow->tm_sec < 10){
		ss << "0" << pnow->tm_sec;
	}else{
		ss << pnow->tm_sec;
	}

	ss >> dataDir;
	_mkdir(dataDir.c_str());


	PHFemMeshNew* mesh = GetPHFemMesh();
	
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

	///	face
	for(unsigned faceid=0;faceid<mesh->faces.size();faceid++){
		CalcFaceArea(faceid);
		faceVars[faceid].deformed = true;				//������Ԃ́A�ό`��Ƃ���
		faceVars[faceid].surroundFlux = 1.0;
		CalcFaceMaxVaporPress(faceid);
		CalcFaceMaxVaporCont(faceid);
		faceVars[faceid].vaporCont = faceVars[faceid].maxVaporCont * 0.6;	//�������x�Ŏ��x60%
		if(faceid < mesh->nSurfaceFace){
			faceVars[faceid].dryingStep = constRateDrying;
		}
	}
	
	rhoWVecAll.resize(mesh->vertices.size(), 1);
	rhoOVecAll.resize(mesh->vertices.size(), 1);
	
	/// vertex
	//�S�ړ_�̊ܐ����Ɗܖ����̏����l���Z�b�g���A���R�����݂̂��ܐ����x�N�g���ɃZ�b�g
	for(unsigned vtxid=0; vtxid < mesh->vertices.size(); vtxid++){
		if(!mesh->GetPHFemThermo()){
			vertexVars[vtxid].T = 20.0;
			vertexVars[vtxid].preT = vertexVars[vtxid].T;
		}else{
			vertexVars[vtxid].preT = mesh->GetPHFemThermo()->GetVertexTemp(vtxid);
		}
		vertexVars[vtxid].Tc = 20.0;
		CalcVertexWOMu(vtxid);
		vertexVars[vtxid].denaturated = false;
		vertexVars[vtxid].rhoS = 1080;
		vertexVars[vtxid].rhoS0 = vertexVars[vtxid].rhoS;
		vertexVars[vtxid].outflowWater = 0;
		vertexVars[vtxid].outflowOil = 0;
	}

	double allVolume = 0.0;

	for(unsigned i=0; i < mesh->surfaceVertices.size(); i++){
		unsigned vtxid = mesh->surfaceVertices[i];
		Vec3d vtxPose = mesh->vertices[vtxid].pos;
		if(vtxPose.y > 0.00749){
			topVertices.push_back(vtxid);
		}else if(vtxPose.y < -0.00749){
			bottomVertices.push_back(vtxid);
		}else{
			sideVertices.push_back(vtxid);
		}
	}

	for(unsigned i=0; i < mesh->vertices.size(); i++){
		std::vector<unsigned>::iterator topIt = std::find(topVertices.begin(), topVertices.end(),i);
		std::vector<unsigned>::iterator sideIt = std::find(sideVertices.begin(), sideVertices.end(),i);
		std::vector<unsigned>::iterator bottomIt = std::find(bottomVertices.begin(), bottomVertices.end(),i);
		
		if((topIt == topVertices.end()) && (sideIt == sideVertices.end()) && (bottomIt == bottomVertices.end())){
			internalVertices.push_back(i);
		}
	}

	/// tet
	for(unsigned tetid=0; tetid < mesh->tets.size(); tetid++){
		CalcTetVolume(tetid);
		tetVars[tetid].preTetPorosity = 0.0;
		CalcTetWOMu(tetid);
		tetVars[tetid].preTetMuW = 1.0;
		tetVars[tetid].preTetMuO = 1.0;
		CalcTetRhoS(tetid);
		tetVars[tetid].rhoS0 = tetVars[tetid].rhoS;
		tetVars[tetid].preRhoS = 0.0;
		tetVars[tetid].sDensChanged = true;

		allVolume += tetVars[tetid].volume;
	}
	DSTR << "allVolume�F" << allVolume << std::endl;

	rhowInit = initMassAll * initWaterRatio / allVolume;
	rhooInit = initMassAll * initOilRatio / allVolume;

	for(unsigned vtxid=0; vtxid < mesh->vertices.size(); vtxid++){
		vertexVars[vtxid].rhoWInit = rhowInit;
		vertexVars[vtxid].rhoOInit = rhooInit;
#ifdef UseBoundWater
		vertexVars[vtxid].rhoW = vertexVars[vtxid].rhoWInit * (1-boundWaterRatio);
#else
		vertexVars[vtxid].rhoW = vertexVars[vtxid].rhoWInit;
#endif
		vertexVars[vtxid].rhoO = vertexVars[vtxid].rhoOInit;
		CalcVertexVolume(vtxid);
		CalcVertexMwo(vtxid);
		vertexVars[vtxid].porosity = ((vertexVars[vtxid].mw / rhoWater) + (vertexVars[vtxid].mo / rhoOil)) / vertexVars[vtxid].vVolume;
#ifdef UseBoundWater
		vertexVars[vtxid].boundWaterMass = vertexVars[vtxid].rhoWInit*boundWaterRatio*vertexVars[vtxid].vVolume;
#endif
	}

	//tet
	for(unsigned tetid=0; tetid < mesh->tets.size(); tetid++){
		CalcTetPorosity(tetid);
	}

	///edge
	for(unsigned edgeid=0; edgeid < mesh->edges.size(); edgeid++){
		edgeVars[edgeid].ww = 0.0;
		edgeVars[edgeid].ow = 0.0;
		edgeVars[edgeid].wo = 0.0;
		edgeVars[edgeid].oo = 0.0;
		edgeVars[edgeid].c= 0.0;
		edgeVars[edgeid].b = 0.0;
	}

	//vertices.Ww�����ׂāAWwVecAll�֑������
	CreateRhoWVecAll();
	//vertices.Wo�����ׂāAWoVecAll�֑������
	CreateRhoOVecAll();

	InitPcVecAll();

	InitMatWO();	//MatWO�̏�����
	InitMatC();		//MatC�̏�����
	InitMatPc();	//MatPc�̏�����
	InitVecF();		//VecF�̏�����

	keisuWInv.resize(mesh->vertices.size(),mesh->vertices.size());
	keisuOInv.resize(mesh->vertices.size(),mesh->vertices.size());
	keisuWInv.clear();
	keisuOInv.clear();

	bwVecAll.resize(mesh->vertices.size(), 1);
	boVecAll.resize(mesh->vertices.size(), 1);
	bwVecAll.clear();
	boVecAll.clear();

	gravity = Vec3d(0.0, -9.8, 0.0);
	initSolidMass = initMassAll * (1 - initWaterRatio - initOilRatio);

	//matk2w�p��l=0�̎�k21,1�̎�:k22, 2�̎�:k23, 3�̎�:k24	�𐶐�
	for(unsigned l= 0 ; l < 4; l++){
		//matk2array[l] = matk2temp;
		matk2array[l] = Create44Mat21();
		//	l�si���0��
		for(int i=0;i<4;i++){
			matk2array[l][l][i] = 0.0;
		}
		//	i�sl���0��
		for(int i=0;i<4;i++){
			matk2array[l][i][l] = 0.0;
		}
	}

	//vecf�p��l=0�̎�f31,1:f32, 2:f33, 3:f34�𐶐�
	for(unsigned l= 0 ; l < 4; l++){
		vecFarray[l] = Create41Vec1();
		//	l�s��0��
		vecFarray[l][l] = 0.0;
		//array[n][m][l]	= narray[n],m�sl��
		//	f_3	(vecf3array[0], vecf3array[1],..)
		// =	| 0 | + | 1 |+...
		//		| 1 |   | 0 |
		//		| 1 |   | 1 |
		//		| 1 |   | 1 |
	}

	//�P�ʍs������
	idMat.resize(mesh->vertices.size(),mesh->vertices.size());
	idMat.clear();
	for(size_t i=0; i < mesh->vertices.size(); i++){
		idMat[i][i] = 1.0;
	}

	matCTemp = Create44Mat21();

#ifdef Scilab
	ScilabStart();
	ScilabSetMatrix("rhowvecall", rhoWVecAll);
	ScilabSetMatrix("rhoovecall", rhoOVecAll);
#endif

	for(unsigned tetid=0; tetid < mesh->tets.size(); tetid++){
		tetVars[tetid].sDensChanged = false;
	}
	
	//�J�E���g�̏�����
	COUNT = 0;

	//�ܐ����E�ܖ����ω��o��
	checkRhoWVecAllout.open(dataDir+"/checkRhoWVecAllout.csv");
	checkRhoOVecAllout.open(dataDir+"/checkRhoOVecAllout.csv");
	tempAll.open(dataDir+"/tempAll.csv");
	tempAndWater.open(dataDir+"/tempAndWater.csv");

	checkRhoWVecAllout <<"����" << COUNT<<", ";
	checkRhoOVecAllout <<"����" << COUNT<<", ";
	tempAll << "����" << COUNT << ", ";
	tempAndWater << "����" << COUNT << ",";
	for(unsigned i=0; i < mesh->vertices.size();i++){
		if(i != mesh->vertices.size() -1){
			checkRhoWVecAllout << "���_" << i << ", ";
			checkRhoOVecAllout << "���_" << i << ", ";
			tempAll << "���_" << i << ", ";
		}
		else{
			checkRhoWVecAllout << "���_" << i << std::endl;
			checkRhoOVecAllout << "���_" << i << std::endl;
			tempAll << "���_" << i << std::endl;
		}
	}

	tempAndWater << "top,center,bottom,topW,centerW,bottomW,topO,centerO,bottomO,�ܐ���[g-waterw/g-all],�ܖ���[g-oil/g-all],�ܐ���[g],�ܖ���[g]" << std::endl;
	tempAndWater << 0 << ", " 
					<< mesh->GetPHFemThermo()->GetVtxTempInTets(top) << "," 
					<< mesh->GetPHFemThermo()->GetVtxTempInTets(center) << "," 
					<< mesh->GetPHFemThermo()->GetVtxTempInTets(bottom) << ","
					<< GetVtxWaterInTets(top) << ","
					<< GetVtxWaterInTets(center) << ","
					<< GetVtxWaterInTets(bottom) << ","
					<< GetVtxOilInTets(top) << ","
					<< GetVtxOilInTets(center) << ","
					<< GetVtxOilInTets(bottom) << ",";

	invCheck.open(dataDir+"/invCheck.csv");

	FEMLOG.open(dataDir+"/femLogNew.csv");
	//	CPS�̌o���ω��������o��
	//cpslog.open(dataDir+"/cpslog.csv");
	//keisuWLog.open(dataDir+"/keisuWLog.csv");
	//keisuOLog.open(dataDir+"/keisuOLog.csv");
	//matWwAllout.open(dataDir+"/matWw.csv");
	//matOwAllout.open(dataDir+"/matOw.csv");
	//matWoAllout.open(dataDir+"/matWo.csv");
	//matOoAllout.open(dataDir+"/matOo.csv");
	//matCAllout.open(dataDir+"/matC.csv");
	//matPcwAllout.open(dataDir+"/matPcw.csv");
	//matPcoAllout.open(dataDir+"/matPco.csv");
	vecFwAllout.open(dataDir+"/vecFw.csv");
	vecFoAllout.open(dataDir+"/vecFo.csv");

	wAll.open(dataDir+"/wAll.csv");
	oAll.open(dataDir+"/oAll.csv");
	SAll.open(dataDir+"/SAll.csv");
	topS.open(dataDir+"/topS.csv");
	sideS.open(dataDir+"/sideS.csv");
	bottomS.open(dataDir+"/bottomS.csv");
	internalS.open(dataDir+"/internalS.csv");
	PcAll.open(dataDir+"/PcAll.csv");

	topOutflowWater.open(dataDir+"/topOutflowWater.csv");
	topOutflowOil.open(dataDir+"/topOutflowOil.csv");
	sideOutflowWater.open(dataDir+"/sideOutflowWater.csv");
	sideOutflowOil.open(dataDir+"/sideOutflowOil.csv");
	bottomOutflowWater.open(dataDir+"/bottomOutflowWater.csv");
	bottomOutflowOil.open(dataDir+"/bottomOutflowOil.csv");

	paramout.open(dataDir+"/parameter.txt");

	paramout << "�Z���W�� K�F" << K << std::endl;
	paramout << "�ъǃ|�e���V�����̌W�� kc�F" << kc  << std::endl;
	paramout << "�ъǈ��͂̌W�� kp�F" << kp << std::endl;
	paramout << "�ъǃ|�e���V�����̒萔 ���F" << alpha << std::endl;
	paramout << "�ъǈ��͂̒萔 ���F" << gamma << std::endl;
	paramout << "�H�ނ̏������ʁF" << initMassAll << "g" << std::endl;
	paramout << "�����̐����̊����F" << initWaterRatio*100 << "%" << std::endl;
	paramout << "�����̖����̊����F" << initOilRatio*100 << "%" << std::endl;
	paramout << "�S���b�V���̑e���F" << "pq2.1a1.0e-8" << std::endl;
	paramout << "�S�̏������x�F" << "120��" << std::endl;
	paramout << "�������b�V���̑e���F" << "pq2.1a1.0e-8" << std::endl;
	paramout << "�����̏������x�F" << "17��" << std::endl;
	paramout << "weekPow_FULL�F" << mesh->GetPHFemThermo()->GetWeekPowFULL() << std::endl;
	
	wAll <<"����" << COUNT<<", ";
	oAll <<"����" << COUNT<<", ";
	for(unsigned i=0; i < mesh->vertices.size();i++){
		//if(i != mesh->vertices.size() -1){
			wAll << "���_" << i << ", ";
			oAll << "���_" << i << ", ";
		//}
		//else{
		//	wAll << "���_" << i << std::endl;
		//	oAll << "���_" << i << std::endl;
		//}
	}
	wAll << ",���̑S����,�ܐ���" << std::endl;
	oAll << ",���̑S����,�ܖ���" << std::endl;

	topS << "����,";
	sideS << "����,";
	bottomS << "����,";
	internalS << "����,";
	topOutflowWater << "����,";
	topOutflowOil << "����,";
	sideOutflowWater << "����,";
	sideOutflowOil << "����,";
	bottomOutflowWater << "����,";
	bottomOutflowOil << "����,";
	for(unsigned i=0; i < topVertices.size(); i++){
		topS << "���_" << topVertices[i] << ",";
		topOutflowWater << "���_" << topVertices[i] << ",";
		topOutflowOil << "���_" << topVertices[i] << ",";
	}
	topS << std::endl;
	topOutflowWater << std::endl;
	topOutflowOil << std::endl;

	for(unsigned i=0; i < sideVertices.size(); i++){
		sideS << "���_" << sideVertices[i] << ",";
		sideOutflowWater << "���_" << sideVertices[i] << ",";
		sideOutflowOil << "���_" << sideVertices[i] << ",";
	}
	sideS << std::endl;
	sideOutflowWater << std::endl;
	sideOutflowOil << std::endl;

	for(unsigned i=0; i < bottomVertices.size(); i++){
		bottomS << "���_" << bottomVertices[i] << ",";
		bottomOutflowWater << "���_" << bottomVertices[i] << ",";
		bottomOutflowOil << "���_" << bottomVertices[i] << ",";
	}
	bottomS << std::endl;
	bottomOutflowWater << std::endl;
	bottomOutflowOil << std::endl;

	for(unsigned i=0; i < internalVertices.size(); i++){
		internalS << "���_" << internalVertices[i] << ",";
	}
	internalS << std::endl;

	double totalW = 0.0;
	double totalO = 0.0;

	wAll << 0 << ", ";
	oAll << 0 << ", ";
	for(unsigned i=0; i < mesh->vertices.size(); i++){
		//for(unsigned j=0; j < mesh->vertices.size(); j++){
		//	matWwAllout << matWwAll[i][j] << ",";
		//	matOwAllout << matOwAll[i][j] << ",";
		//	matWoAllout << matWoAll[i][j] << ",";
		//	matOoAllout << matOoAll[i][j] << ",";
		//	matCAllout << matCAll[i][j] << ",";
		//	matPcwAllout << matPcwAll[i][j] << ",";
		//	matPcoAllout << matPcoAll[i][j] << ",";
		//}
		//matWwAllout << std::endl;
		//matOwAllout << std::endl;
		//matWoAllout << std::endl;
		//matOoAllout << std::endl;
		//matCAllout << std::endl;
		//matPcwAllout << std::endl;
		//matPcoAllout << std::endl;
		vecFwAllout << vecFwAll[i][0] << ",";
		vecFoAllout << vecFoAll[i][0] << ",";
#ifdef UseBoundWater
		wAll << vertexVars[i].mw + vertexVars[i].boundWaterMass << ",";
		totalW += vertexVars[i].mw + vertexVars[i].boundWaterMass;
#else
		wAll << vertexVars[i].mw<< ",";
		totalW += vertexVars[i].mw;
#endif
		totalO += vertexVars[i].mo;
		oAll << vertexVars[i].mo << ",";
	}
	wAll << "," << totalW << "," << totalW / (totalW + totalO + initSolidMass) << std::endl;
	oAll << "," << totalO << "," << totalO / (totalW + totalO + initSolidMass) << std::endl;
	tempAndWater << totalW / (totalW + totalO + initSolidMass) << "," << totalO / (totalW + totalO + initSolidMass) << "," << totalW*1000 << "," << totalO*1000 << std::endl;

	//matWwAllout << std::endl;
	//matOwAllout << std::endl;
	//matWoAllout << std::endl;
	//matOoAllout << std::endl;
	//matCAllout << std::endl;
	//matPcwAllout << std::endl;
	//matPcoAllout << std::endl;
	vecFwAllout << std::endl;
	vecFoAllout << std::endl;
	
	// �J�E���g�̏�����
	Ndt =0;

	//	�Sface�̖@�����v�Z
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
		}
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

void PHFemPorousWOMove::Step(){
	Step(tdt);
}

void PHFemPorousWOMove::Step(double dt){
	
	//decrhoW(0.2);

	PHFemMeshNew* mesh = GetPHFemMesh();

	////�\�ʂɂ���ʂ��\������3���_�̊ܐ����̑������ς�ʂ̊ܐ����Ƃ��A���̖ʂɂ����銣���̒i�K�����肷��, 
	//for(unsigned fid = 0; fid < mesh->nSurfaceFace; fid++){
	//	double faceWaterContent = (vertexVars[mesh->faces[fid].vertexIDs[0]].Ww + vertexVars[mesh->faces[fid].vertexIDs[1]].Ww + vertexVars[mesh->faces[fid].vertexIDs[2]].Ww) / 3;
	//	if(faceVars[fid].dryingStep == constRateDrying && faceWaterContent <= limitWaterCont){
	//		faceVars[fid].dryingStep = fallRateDrying1st;
	//		faceVars[fid].K = wDiffAir * (faceVars[fid].maxVaporCont - faceVars[fid].vaporCont) / boundaryThick / (limitWaterCont - equilWaterCont);
	//	}else if(faceVars[fid].dryingStep == fallRateDrying1st && faceWaterContent == 0.0){
	//		faceVars[fid].dryingStep = fallRateDrying2nd;
	//	}

	//	CalcFaceMaxVaporPress(fid);
	//	CalcFaceMaxVaporCont(fid);
	//}

#ifdef calcWOMotion

	//�ϐ����N�����Ă��Ȃ����_�ɂ��āA���_���x���Q�Ƃ��ĕϐ��������ǂ��������߂�+�ъǃ|�e���V�������v�Z����
	for(unsigned vtxid=0; vtxid < mesh->vertices.size(); vtxid++){
#ifdef UseBoundWater
		if(!vertexVars[vtxid].denaturated){
			if(mesh->femThermo){
				if(mesh->GetPHFemThermo()->GetVertexTemp(vtxid) > denatTemp){
					vertexVars[vtxid].denaturated = true;
					rhoWVecAll[vtxid][0] += vertexVars[vtxid].rhoWInit * boundWaterRatio;
					vertexVars[vtxid].rhoW += vertexVars[vtxid].rhoWInit * boundWaterRatio;
				}
			}else{
				if(vertexVars[vtxid].T > denatTemp){
					vertexVars[vtxid].denaturated = true;
					rhoWVecAll[vtxid][0] += vertexVars[vtxid].rhoWInit * boundWaterRatio;
					vertexVars[vtxid].rhoW += vertexVars[vtxid].rhoWInit * boundWaterRatio;
				}
			}
		}
#endif
		
		if(!vertexVars[vtxid].denaturated && mesh->GetPHFemThermo()->GetVertexTemp(vtxid) >= 60.0){
			vertexDenatProcess(vtxid);
			vertexVars[vtxid].denaturated = true;
		}

		CalcVertexSaturation(vtxid);
		CalcVertexPc(vtxid);
		PcVecAll[vtxid][0] = vertexVars[vtxid].Pc;
		SAll << vertexVars[vtxid].saturation << ",";
		PcAll << PcVecAll[vtxid][0] << ",";
		CalcVertexWOMu(vtxid);
		CalcRhoWVecFromVertexMw(vtxid);
	}
	SAll << std::endl;
	PcAll << std::endl;

	//�O�a���̏o�͂͂������������̂��H
	topS << COUNT * dt << ",";
	for(unsigned id=0; id < topVertices.size(); id++){
		topS << vertexVars[topVertices[id]].saturation << ",";
	}
	topS << std::endl;

	sideS << COUNT * dt << ",";
	for(unsigned id=0; id < sideVertices.size(); id++){
		sideS << vertexVars[sideVertices[id]].saturation << ",";
	}
	sideS << std::endl;

	bottomS << COUNT * dt << ",";
	for(unsigned id=0; id < bottomVertices.size(); id++){
		bottomS << vertexVars[bottomVertices[id]].saturation << ",";
	}
	bottomS << std::endl;

	internalS << COUNT * dt << ",";
	for(unsigned id=0; id < internalVertices.size(); id++){
		internalS << vertexVars[internalVertices[id]].saturation << ",";
	}
	internalS << std::endl;

	//�e�l�ʑ̂̐��Ɩ��̔S�x,�Ԍ���,�ő̖��x���v�Z
	for(unsigned tetid=0; tetid < mesh->tets.size(); tetid++){
		CalcTetWOMu(tetid);
		CalcTetPorosity(tetid);
		CalcTetRhoS(tetid);
		//CalcTetVolume(tetid);
	}
#endif

	checkRhoWVecAllout << COUNT * dt << ", ";
	checkRhoOVecAllout << COUNT * dt << ", ";
	tempAll << COUNT * dt << ", ";
	for(unsigned i=0;i<mesh->vertices.size();i++){
		if(i != mesh->vertices.size() -1){
			checkRhoWVecAllout << rhoWVecAll[i][0] <<", ";
			checkRhoOVecAllout << rhoOVecAll[i][0] <<", ";
			if(mesh->femThermo){
				tempAll << mesh->GetPHFemThermo()->GetVertexTemp(i) << ", ";
			}else{
				tempAll << vertexVars[i].T << ", ";
			}
		}else{
			checkRhoWVecAllout <<  rhoWVecAll[i][0] << std::endl;
			checkRhoOVecAllout <<  rhoOVecAll[i][0] << std::endl;
			if(mesh->femThermo){
				tempAll << mesh->GetPHFemThermo()->GetVertexTemp(i) << std::endl;
			}else{
				tempAll << vertexVars[i].T << std::endl;
			}
		}	
	}
	
	COUNT +=1;

#ifdef calcWOMotion

	CreateMatVecAll();
	//matWwOut();
	//matWoOut();
	//matOwOut();
	//matOoOut();
	//matCOut();
	//matPcwOut();
	//matPcoOut();
	//vecFwAllOut();
	//vecFoAllOut();
	//vecPcAllOut();

	//for(unsigned i=0; i < mesh->vertices.size(); i++){
	//	for(unsigned j=0; j < mesh->vertices.size(); j++){
	//		matWwAllout << matWwAll[i][j] << ",";
	//		matOwAllout << matOwAll[i][j] << ",";
	//		matWoAllout << matWoAll[i][j] << ",";
	//		matOoAllout << matOoAll[i][j] << ",";
	//		matCAllout << matCAll[i][j] << ",";
	//		matPcwAllout << matPcwAll[i][j] << ",";
	//		matPcoAllout << matPcoAll[i][j] << ",";
	//	}
	//	matWwAllout << std::endl;
	//	matOwAllout << std::endl;
	//	matWoAllout << std::endl;
	//	matOoAllout << std::endl;
	//	matCAllout << std::endl;
	//	matPcwAllout << std::endl;
	//	matPcoAllout << std::endl;
	//	vecFwAllout << vecFwAll[i][0] << ",";
	//	vecFoAllout << vecFoAll[i][0] << ",";
	//}
	//matWwAllout << std::endl;
	//matOwAllout << std::endl;
	//matWoAllout << std::endl;
	//matOoAllout << std::endl;
	//matCAllout << std::endl;
	//matPcwAllout << std::endl;
	//matPcoAllout << std::endl;
	//vecFwAllout << std::endl;
	//vecFoAllout << std::endl;

#ifdef Scilab
	CalcWOContentUsingScilab(dt);
#endif

#ifndef Scilab
	
	//���ږ@�ɂ��ܐ����E�ܖ������v�Z
#ifndef usingG
	//CalcWOContentDirect(dt, eps);
	CalcWOContentDirect2(dt, eps);
#endif
#ifdef usingG
	CalcWOContentUsingGaussSeidel(100, dt, eps);
#endif
	//for(unsigned vtxid=0; vtxid < mesh->vertices.size(); vtxid++){
	//	if(rhoWVecAll[vtxid][0] < 0.0){
	//		rhoWVecAll[vtxid][0] = 0.0;
	//	}
	//}
	for(unsigned i=0; i < mesh->surfaceVertices.size(); i++){
		outflowOverSaturation(mesh->surfaceVertices[i]);
	}

	UpdateVertexRhoWAll();
	UpdateVertexRhoOAll();


	topOutflowWater << COUNT * dt << ",";
	topOutflowOil << COUNT * dt << ",";
	for(unsigned id=0; id < topVertices.size(); id++){
		topOutflowWater << vertexVars[topVertices[id]].outflowWater << ",";
		topOutflowOil << vertexVars[topVertices[id]].outflowOil << ",";
	}
	topOutflowWater << std::endl;
	topOutflowOil << std::endl;

	sideOutflowWater << COUNT * dt << ",";
	sideOutflowOil << COUNT * dt << ",";
	for(unsigned id=0; id < sideVertices.size(); id++){
		sideOutflowWater << vertexVars[sideVertices[id]].outflowWater << ",";
		sideOutflowOil << vertexVars[sideVertices[id]].outflowOil << ",";
	}
	sideOutflowWater << std::endl;
	sideOutflowOil << std::endl;

	bottomOutflowWater << COUNT * dt << ",";
	bottomOutflowOil << COUNT * dt << ",";
	for(unsigned id=0; id < bottomVertices.size(); id++){
		bottomOutflowWater << vertexVars[bottomVertices[id]].outflowWater << ",";
		bottomOutflowOil << vertexVars[bottomVertices[id]].outflowOil << ",";
	}
	bottomOutflowWater << std::endl;
	bottomOutflowOil << std::endl;

	//for(unsigned i=0; i < mesh->vertices.size(); i++){
	//	for(unsigned j=0; j < mesh->vertices.size(); j++){
	//		keisuWLog << keisuW[i][j] << ",";
	//		keisuOLog << keisuO[i][j] << ",";
	//	}
	//	keisuWLog << std::endl;
	//	keisuOLog << std::endl;
	//}
	//keisuWLog << std::endl;
	//keisuOLog << std::endl;
#endif

	StepCount += 1;
	if(StepCount >= 1000*1000*1000){
		StepCount = 0;
		StepCount_ += 1;
	}

	for(unsigned vtxid=0; vtxid < mesh->vertices.size(); vtxid++){
		CalcVertexMwo(vtxid);
	}
#endif
	double totalW = 0.0;
	double totalO = 0.0;

	//wAll << COUNT * dt << ", ";
	//oAll << COUNT * dt << ", ";
	for(unsigned vtxid=0; vtxid < mesh->vertices.size(); vtxid++){
		//CalcVertexMwo(vtxid);
		if(vertexVars[vtxid].denaturated){
			//wAll << vertexVars[vtxid].mw << ",";
			totalW += vertexVars[vtxid].mw;
		}else{
#ifdef UseBoundWater
			wAll << vertexVars[vtxid].mw + vertexVars[vtxid].boundWaterMass << ",";
			totalW += vertexVars[vtxid].mw + vertexVars[vtxid].boundWaterMass;
#else
			//wAll << vertexVars[vtxid].mw << ",";
			totalW += vertexVars[vtxid].mw;
#endif
		}
		//oAll << vertexVars[vtxid].mo << ",";
		totalO += vertexVars[vtxid].mo;
	}
	//wAll << "," << totalW << "," << totalW / (totalW + totalO + initSolidMass) << std::endl;
	//oAll << "," << totalO << "," << totalO / (totalW + totalO + initSolidMass) << std::endl;

	tempAndWater << COUNT * dt << ", " 
				<< mesh->GetPHFemThermo()->GetVtxTempInTets(top) << "," 
				<< mesh->GetPHFemThermo()->GetVtxTempInTets(center) << "," 
				<< mesh->GetPHFemThermo()->/*GetVertexTemp(19)*/GetVtxTempInTets(bottom) << "," 
				<< GetVtxWaterInTets(top) << ","
				<< GetVtxWaterInTets(center) << ","
				<< GetVtxWaterInTets(bottom) << ","
				<< GetVtxOilInTets(top) << ","
				<< GetVtxOilInTets(center) << ","
				<< GetVtxOilInTets(bottom) << ","
				<< totalW / (totalW + totalO + initSolidMass) << "," << totalO / (totalW + totalO + initSolidMass) << "," << totalW*1000 << "," << totalO*1000 << std::endl;
	
	for(unsigned tetid=0; tetid < mesh->tets.size(); tetid++){
		tetVars[tetid].preTetPorosity = tetVars[tetid].tetPorosity;
		tetVars[tetid].preRhoS = tetVars[tetid].rhoS;
		tetVars[tetid].preTetMuW = tetVars[tetid].tetMuW;
		tetVars[tetid].preTetMuO = tetVars[tetid].tetMuO;
	}

	//DSTR << GetVtxWaterInTets(top) << "\t"	<< GetVtxWaterInTets(center) << "\t"	<< GetVtxWaterInTets(bottom) << std::endl;
}

void PHFemPorousWOMove::InitMatWO(){
	PHFemMeshNew* mesh = GetPHFemMesh();
	int vtxSize = (int)mesh->vertices.size();

	matWwAll.resize(vtxSize, vtxSize);
	matOwAll.resize(vtxSize, vtxSize);
	matWoAll.resize(vtxSize, vtxSize);
	matOoAll.resize(vtxSize, vtxSize);

	matWwAll.clear();
	matOwAll.clear();
	matWoAll.clear();
	matOoAll.clear();

	for(unsigned tetid=0; tetid < mesh->tets.size(); tetid++){
		tetVars[tetid].matWw.clear();
		tetVars[tetid].matOw.clear();
		tetVars[tetid].matWo.clear();
		tetVars[tetid].matOo.clear();
	}

	dMatWwAll.resize(1, mesh->vertices.size());
	dMatOwAll.resize(1, mesh->vertices.size());
	dMatWoAll.resize(1, mesh->vertices.size());
	dMatOoAll.resize(1, mesh->vertices.size());

	dMatWwAll.clear();
	dMatOwAll.clear();
	dMatWoAll.clear();
	dMatOoAll.clear();
}

void PHFemPorousWOMove::InitMatC(){
	PHFemMeshNew* mesh = GetPHFemMesh();
	int vtxSize = (int)mesh->vertices.size();
	matCAll.resize(vtxSize, vtxSize);
	matCAll.clear();
	for(unsigned tetid=0; tetid < mesh->tets.size(); tetid++){
		tetVars[tetid].matC.clear();
	}

	dMatCAll.resize(1, mesh->vertices.size());
	dMatCAll.clear();
}

void PHFemPorousWOMove::InitMatPc(){
	PHFemMeshNew* mesh = GetPHFemMesh();
	int vtxSize = (int)mesh->vertices.size();
	matPcwAll.resize(vtxSize, vtxSize);
	matPcoAll.resize(vtxSize, vtxSize);

	matPcwAll.clear();
	matPcoAll.clear();
	for(unsigned tetid=0; tetid < mesh->tets.size(); tetid++){
		tetVars[tetid].matPcw.clear();
		tetVars[tetid].matPco.clear();
	}
}

void PHFemPorousWOMove::InitVecF(){
	PHFemMeshNew* mesh = GetPHFemMesh();
	int vtxSize = (int)mesh->vertices.size();

	vecFwAll.resize(vtxSize, 1);
	vecFoAll.resize(vtxSize, 1);

	vecFwFinal.resize(vtxSize, 1);
	vecFoFinal.resize(vtxSize, 1);

	vecFwAll.clear();
	vecFoAll.clear();
	vecFwFinal.clear();
	vecFoFinal.clear();

	for(unsigned tetid=0; tetid < mesh->tets.size(); tetid++){
		tetVars[tetid].vecFw[0].clear();
		tetVars[tetid].vecFw[1].clear();
		tetVars[tetid].vecFo[0].clear();
		tetVars[tetid].vecFo[1].clear();
	}
}

void PHFemPorousWOMove::CreateMatWOPcVecF2Local(unsigned tetid){

	PHFemMeshNew* mesh = GetPHFemMesh();

	//���̌v�Z���Ăяo���Ƃ��ɁA�e�l�ʑ̂��ƂɌv�Z���邽�߁A�l�ʑ̂�0�Ԃ��珇�ɂ��̌v�Z���s��
	//�l�ʑ̂��\������4�ߓ_��ߓ_�̔z��(Tets�ɂ́A�ߓ_�̔z�񂪍���Ă���)�ɓ����Ă��鏇�Ԃ��g���āA�ʂ̌v�Z���s������A�s��̌v�Z���s�����肷��B
	//���̂��߁A���̊֐��̈����ɁA�l�ʑ̗v�f�̔ԍ������
	if((tetVars[tetid].tetPorosity != tetVars[tetid].preTetPorosity) || (tetVars[tetid].rhoS != tetVars[tetid].preRhoS)){

		//�Ō�ɓ����s���������
		tetVars[tetid].matWw.clear();
		tetVars[tetid].matOw.clear();
		tetVars[tetid].matWo.clear();
		tetVars[tetid].matOo.clear();
		tetVars[tetid].matPcw.clear();
		tetVars[tetid].matPco.clear();
		tetVars[tetid].vecFw[1].clear();
		tetVars[tetid].vecFo[1].clear();
	
		//	A�s��@=	a11 a12 a13
		//				a21 a22 a23
		//				a31 a32 a33
		//�𐶐�


		PTM::TMatrixRow<4,4,double> matk1A;
		PTM::TMatrixRow<4,4,double> matkm;
		FemVertex p[4];
		for(unsigned i = 0; i< 4 ; i++){
			p[i]= mesh->vertices[mesh->tets[tetid].vertexIDs[i]];
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
		matkm = Nx.trans() * Nx + Ny.trans() * Ny + Nz.trans() * Nz;
	

		//tetVars[tetid].matWw = K * kp * (pow((tetVars[tetid].rhoS)/tetVars[tetid].rhoS0, gamma) - 1) / pow(tetVars[tetid].tetPorosity, 2) / tetVars[tetid].tetMuW / rhoWater / (36 * tetVars[tetid].volume ) * matkm;
		//tetVars[tetid].matOw = K * kp * (pow((tetVars[tetid].rhoS)/tetVars[tetid].rhoS0, gamma) - 1) / pow(tetVars[tetid].tetPorosity, 2) / tetVars[tetid].tetMuW / rhoOil / (36 * tetVars[tetid].volume ) * matkm;
		//tetVars[tetid].matWo = K * kp * (pow((tetVars[tetid].rhoS)/tetVars[tetid].rhoS0, gamma) - 1) / pow(tetVars[tetid].tetPorosity, 2) / tetVars[tetid].tetMuO / rhoWater / (36 * tetVars[tetid].volume ) * matkm;
		//tetVars[tetid].matOo = K * kp * (pow((tetVars[tetid].rhoS)/tetVars[tetid].rhoS0, gamma) - 1) / pow(tetVars[tetid].tetPorosity, 2) / tetVars[tetid].tetMuO / rhoOil / (36 * tetVars[tetid].volume ) * matkm;
		//tetVars[tetid].matPcw = K / tetVars[tetid].tetPorosity / tetVars[tetid].tetMuW / (36 * tetVars[tetid].volume ) * matkm;
		//tetVars[tetid].matPco = K / tetVars[tetid].tetPorosity / tetVars[tetid].tetMuO / (36 * tetVars[tetid].volume ) * matkm;
		//tetVars[tetid].vecFw[1] = K / tetVars[tetid].tetPorosity / tetVars[tetid].tetMuW * tetVars[tetid].rhoS / 6 * (gravity[0] * Nx.trans() + gravity[1] * Ny.trans() + gravity[2] * Nz.trans());
		//tetVars[tetid].vecFo[1] = K / tetVars[tetid].tetPorosity / tetVars[tetid].tetMuO * tetVars[tetid].rhoS / 6 * (gravity[0] * Nx.trans() + gravity[1] * Ny.trans() + gravity[2] * Nz.trans());
		tetVars[tetid].matWw = K * kp * (pow((tetVars[tetid].rhoS)/tetVars[tetid].rhoS0, gamma) - 1) / tetVars[tetid].tetPorosity / tetVars[tetid].tetMuW / rhoWater / (36 * tetVars[tetid].volume ) * matkm;
		tetVars[tetid].matOw = K * kp * (pow((tetVars[tetid].rhoS)/tetVars[tetid].rhoS0, gamma) - 1) / tetVars[tetid].tetPorosity / tetVars[tetid].tetMuW / rhoOil / (36 * tetVars[tetid].volume ) * matkm;
		tetVars[tetid].matWo = K * kp * (pow((tetVars[tetid].rhoS)/tetVars[tetid].rhoS0, gamma) - 1) / tetVars[tetid].tetPorosity / tetVars[tetid].tetMuO / rhoWater / (36 * tetVars[tetid].volume ) * matkm;
		tetVars[tetid].matOo = K * kp * (pow((tetVars[tetid].rhoS)/tetVars[tetid].rhoS0, gamma) - 1) / tetVars[tetid].tetPorosity / tetVars[tetid].tetMuO / rhoOil / (36 * tetVars[tetid].volume ) * matkm;
		tetVars[tetid].matPcw = K / tetVars[tetid].tetMuW / (36 * tetVars[tetid].volume ) * matkm;
		tetVars[tetid].matPco = K / tetVars[tetid].tetMuO / (36 * tetVars[tetid].volume ) * matkm;
		tetVars[tetid].vecFw[1] = K / tetVars[tetid].tetMuW * tetVars[tetid].rhoS / 6 * (gravity[0] * Nx.trans() + gravity[1] * Ny.trans() + gravity[2] * Nz.trans());
		tetVars[tetid].vecFo[1] = K / tetVars[tetid].tetMuO * tetVars[tetid].rhoS / 6 * (gravity[0] * Nx.trans() + gravity[1] * Ny.trans() + gravity[2] * Nz.trans());
		
		matWOPcVecF2Changed = true;
	}else {
		if(tetVars[tetid].tetMuW != tetVars[tetid].preTetMuW){
			tetVars[tetid].matWw *= tetVars[tetid].preTetMuW / tetVars[tetid].tetMuW;
			tetVars[tetid].matOw *= tetVars[tetid].preTetMuW / tetVars[tetid].tetMuW;
			tetVars[tetid].matPcw *= tetVars[tetid].preTetMuW / tetVars[tetid].tetMuW;
			tetVars[tetid].vecFw[1] *= tetVars[tetid].preTetMuW / tetVars[tetid].tetMuW;

			matWOPcVecF2Changed = true;
		}
		if(tetVars[tetid].tetMuO != tetVars[tetid].preTetMuO){
			tetVars[tetid].matWo *= tetVars[tetid].preTetMuO / tetVars[tetid].tetMuO;
			tetVars[tetid].matOo *= tetVars[tetid].preTetMuO / tetVars[tetid].tetMuO;
			tetVars[tetid].matPco *= tetVars[tetid].preTetMuO / tetVars[tetid].tetMuO;
			tetVars[tetid].vecFo[1] *= tetVars[tetid].preTetMuO / tetVars[tetid].tetMuO;

			matWOPcVecF2Changed = true;
		}
	}

	for(unsigned j=1; j < 4; j++){
		int vtxid0 = mesh->tets[tetid].vertexIDs[j];
		//	���O�p�s�񕔕��ɂ��Ă̂ݎ��s
		//	j==1:k=0, j==2:k=0,1, j==3:k=0,1,2
		for(unsigned k = 0; k < j; k++){
			int vtxid1 = mesh->tets[tetid].vertexIDs[k];
			for(unsigned l =0; l < mesh->vertices[vtxid0].edgeIDs.size(); l++){
				for(unsigned m =0; m < mesh->vertices[vtxid1].edgeIDs.size(); m++){
					if(mesh->vertices[vtxid0].edgeIDs[l] == mesh->vertices[vtxid1].edgeIDs[m]){
						edgeVars[mesh->vertices[vtxid0].edgeIDs[l]].ww += tetVars[tetid].matWw[j][k];		//�������̂������͂������甼���ɂ���B��O�p�����O�p������������ɂ́A�ǂ�����for�����ɂ���Ηǂ��̂��H
						edgeVars[mesh->vertices[vtxid0].edgeIDs[l]].ow += tetVars[tetid].matOw[j][k];
						edgeVars[mesh->vertices[vtxid0].edgeIDs[l]].wo += tetVars[tetid].matWo[j][k];
						edgeVars[mesh->vertices[vtxid0].edgeIDs[l]].oo += tetVars[tetid].matOo[j][k];
					}
				}
			}
		}
	}

	for(unsigned j=0; j < 4; j++){
		dMatWwAll[0][mesh->tets[tetid].vertexIDs[j]] += tetVars[tetid].matWw[j][j];
		dMatOwAll[0][mesh->tets[tetid].vertexIDs[j]] += tetVars[tetid].matOw[j][j];
		dMatWoAll[0][mesh->tets[tetid].vertexIDs[j]] += tetVars[tetid].matWo[j][j];
		dMatOoAll[0][mesh->tets[tetid].vertexIDs[j]] += tetVars[tetid].matOo[j][j];
	}
}

//void PHFemPorousWOMove::CreateMatk2(unsigned id){
//
//	PHFemMeshNew* mesh = GetPHFemMesh();
//
//	///	������
//	tetVars[id].matk1W[1].clear();
//
//	for(unsigned l= 0 ; l < 4; l++){
//		///	�l�ʑ̂̊e��(l = 0 �` 3) �ɂ��ă��b�V���\�ʂ��ǂ������`�F�b�N����B�\�ʂȂ�A�s��������matk2array�ɓ����
//		//faces[tets.faces[i]].sorted;		/// 1,24,58�݂����Ȑߓ_�ԍ��������Ă���
//		///	�s��^�̓��ꕨ��p��
//
//		//�ʂ̉��x 3���_�̑�������
//		double surfaceTemp;
//		if(mesh->GetPHFemThermo()){
//			surfaceTemp = (mesh->GetPHFemThermo()->GetVertexTemp(mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[0]) + mesh->GetPHFemThermo()->GetVertexTemp(mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[1]) + mesh->GetPHFemThermo()->GetVertexTemp(mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[2])) / 3.0;
//		}else{
//			surfaceTemp = (vertexVars[mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[0]].T + vertexVars[mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[1]].T + vertexVars[mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[2]].T) / 3.0;
//		}
//		
//		if(mesh->tets[id].faceIDs[l] < (int)mesh->nSurfaceFace && surfaceTemp >= 100.0){			///	�O�k�̖�&�\�ʂ̉��x��100���ȏ�
//			if(faceVars[mesh->tets[id].faceIDs[l]].area ==0 || faceVars[mesh->tets[id].faceIDs[l]].deformed ){		///	�ʐς��v�Z����Ă��Ȃ����i�͂��߁j or deformed(�ό`�������E�������)��true�̎�		///	�����̒ǉ�	�ʐς�0�� ||(OR) �����X�V���ꂽ��
//				faceVars[mesh->tets[id].faceIDs[l]].deformed = false;
//			}
//			///	�v�Z���ʂ��s��ɑ��
//			///	area�̌v�Z�Ɏg���Ă��Ȃ��_�������Ă���s�Ɨ���������s��̐ς��Ƃ�
//			///	�ϕ��v�Z�����{����l����
//			unsigned vtx = mesh->tets[id].vertexIDs[0] + mesh->tets[id].vertexIDs[1] + mesh->tets[id].vertexIDs[2] + mesh->tets[id].vertexIDs[3];
//			//DSTR << "vtx: " << vtx <<std::endl;
//		
//			///	area�v�Z�Ɏg���Ă��Ȃ��ߓ_ID�FID
//			unsigned ID = vtx -( mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[0] + mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[1] + mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[2] );
//			for(unsigned j=0;j<4;j++){
//				if(mesh->tets[id].vertexIDs[j] == ID){					///	�`��֐����P�A�i���Ȃ킿�j����face�ɑΖʂ��钸�_�@�ƈ�v������@���̎���face�Ŗʐϕ�����
//					///	j�Ԗڂ̍s��̐�����0�ɂ���matk2array�Ōv�Z����
//					///	�O�k�ɂȂ����b�V���ʂ̖ʐς�0�ŏ��������Ă���
//						///	�ȉ���[]�͏�܂ł�[l]�ƈقȂ�B
//					///	ID�����Ԗڂ��ɂ���āA�`��֐��̌W�����قȂ�̂ŁA
//					double faceWwInit;	//�ʂ̊ܐ����̏����l
//					faceWwInit = (vertexVars[mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[0]].WwInit + vertexVars[mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[1]].WwInit + vertexVars[mesh->faces[mesh->tets[id].faceIDs[l]].vertexIDs[2]].WwInit) / 3;
//					tetVars[id].matk1W[1] += evapoRate * (surfaceTemp-100) / faceWwInit * (1.0/12.0) * faceVars[mesh->tets[id].faceIDs[l]].area * matk2array[j];
//				}
//			}
//		}
//	}
//}

//void PHFemPorousWOMove::CreateMatWOLocal(unsigned id){
//	PHFemMeshNew* mesh = GetPHFemMesh();
//
//	//���ׂĂ̎l�ʑ̗v�f�ɂ��ČW���s������
//	//[K1W1]�A[K2W]�A[K1O]�A[K2O]�����
//	if(tetVars[id].sDensChanged){
//		CreateMatk1k(id);
//		matkChanged = true;
//		matVecChanged = true;
//	}
//
//	//[K1W2]����� �\�ʂɑ�����ʂ�����A���̖ʂ̒��_�̂������x���ς�������̂������
//	for(unsigned i=0; i < 4; i++){
//		unsigned faceid = mesh->tets[id].faceIDs[i];
//		if(faceid < mesh->nSurfaceFace){
//			if(mesh->femThermo){
//				if((vertexVars[mesh->faces[faceid].vertexIDs[0]].preT != mesh->GetPHFemThermo()->GetVertexTemp(mesh->faces[faceid].vertexIDs[0]))
//					|| (vertexVars[mesh->faces[faceid].vertexIDs[1]].preT != mesh->GetPHFemThermo()->GetVertexTemp(mesh->faces[faceid].vertexIDs[1]))
//					|| (vertexVars[mesh->faces[faceid].vertexIDs[2]].preT != mesh->GetPHFemThermo()->GetVertexTemp(mesh->faces[faceid].vertexIDs[2]))){
//					CreateMatk2(id);
//					matkChanged = true;
//					matVecChanged = true;
//				}
//			}else{
//				if((vertexVars[mesh->faces[faceid].vertexIDs[0]].preT != vertexVars[mesh->faces[faceid].vertexIDs[0]].T)
//					|| (vertexVars[mesh->faces[faceid].vertexIDs[1]].preT != vertexVars[mesh->faces[faceid].vertexIDs[1]].T)
//					|| (vertexVars[mesh->faces[faceid].vertexIDs[2]].preT != vertexVars[mesh->faces[faceid].vertexIDs[2]].T)){
//					CreateMatk2(id);
//					matkChanged = true;
//					matVecChanged = true;
//				}
//			}
//		}
//	}
//}

//void PHFemPorousWOMove::CreateMatWOPcVecF2All(){
//	PHFemMeshNew* mesh = GetPHFemMesh();
//
//	for(unsigned tetid=0; tetid < mesh->tets.size(); tetid++){
//		CreateMatWOPcVecF2Local(tetid);
//	}
//
//	//�S�̍����s������
//	if(matWOPcVecF2Changed){
//		matWwAll.clear();
//		matOwAll.clear();
//		matWoAll.clear();
//		matOoAll.clear();
//		matPcwAll.clear();
//		matPcoAll.clear();
//		for(unsigned tetid=0; tetid < mesh->tets.size(); tetid++){
//			for(unsigned j=0; j<4 ; j++){
//				for(unsigned k=0; k<4 ;k++){
//					matWwAll[mesh->tets[tetid].vertexIDs[j]][mesh->tets[tetid].vertexIDs[k]] += tetVars[tetid].matWw[j][k];
//					matOwAll[mesh->tets[tetid].vertexIDs[j]][mesh->tets[tetid].vertexIDs[k]] += tetVars[tetid].matOw[j][k];
//					matWoAll[mesh->tets[tetid].vertexIDs[j]][mesh->tets[tetid].vertexIDs[k]] += tetVars[tetid].matWo[j][k];
//					matOoAll[mesh->tets[tetid].vertexIDs[j]][mesh->tets[tetid].vertexIDs[k]] += tetVars[tetid].matOo[j][k];
//					matPcwAll[mesh->tets[tetid].vertexIDs[j]][mesh->tets[tetid].vertexIDs[k]] += tetVars[tetid].matPcw[j][k];
//					matPcoAll[mesh->tets[tetid].vertexIDs[j]][mesh->tets[tetid].vertexIDs[k]] += tetVars[tetid].matPco[j][k];
//				}
//			}
//		}
//		keisuChanged = true;
//
//#ifdef Scilab
//		ScilabSetMatrix("matww", matWwAll);
//		ScilabSetMatrix("matow", matOwAll);
//		ScilabSetMatrix("matwo", matWoAll);
//		ScilabSetMatrix("matoo", matOoAll);
//		ScilabSetMatrix("matpcw", matPcwAll);
//		ScilabSetMatrix("matpco", matPcoAll);
//#endif
//
//		matWOPcVecF2Changed = false;
//		keisuChanged = true;
//	}
//}

void PHFemPorousWOMove::CreateMatCLocal(unsigned tetid){
	PHFemMeshNew* mesh = GetPHFemMesh();

	//�Ō�ɓ����s���������
	if((tetVars[tetid].tetPorosity != tetVars[tetid].preTetPorosity) || (tetVars[tetid].rhoS != tetVars[tetid].preRhoS)){
		tetVars[tetid].matC.clear();
	
		tetVars[tetid].matC = tetVars[tetid].volume / 20.0 * matCTemp;

		matCChanged = true;
	}

	for(unsigned j=1; j < 4; j++){
		int vtxid0 = mesh->tets[tetid].vertexIDs[j];
		//	���O�p�s�񕔕��ɂ��Ă̂ݎ��s
		//	j==1:k=0, j==2:k=0,1, j==3:k=0,1,2
		for(unsigned k = 0; k < j; k++){
			int vtxid1 = mesh->tets[tetid].vertexIDs[k];
				for(unsigned l =0; l < mesh->vertices[vtxid0].edgeIDs.size(); l++){
					for(unsigned m =0; m < mesh->vertices[vtxid1].edgeIDs.size(); m++){
						if(mesh->vertices[vtxid0].edgeIDs[l] == mesh->vertices[vtxid1].edgeIDs[m]){
							edgeVars[mesh->vertices[vtxid0].edgeIDs[l]].c += tetVars[tetid].matC[j][k];		//�������̂������͂������甼���ɂ���B��O�p�����O�p������������ɂ́A�ǂ�����for�����ɂ���Ηǂ��̂��H
							//DSTR << edges[vertices[vtxid0].edges[l]].k << std::endl;
						}
					}
				}
		}
	}

	for(unsigned j=0; j < 4; j++){
		dMatCAll[0][mesh->tets[tetid].vertexIDs[j]] += tetVars[tetid].matC[j][j];
	}
}

//void PHFemPorousWOMove::CreateMatCAll(){
//
//	PHFemMeshNew* mesh = GetPHFemMesh();
//
//	//�S�̍����s������
//	if(matCChanged){
//		matCAll.clear();
//		for(unsigned tetid=0; tetid < mesh->tets.size(); tetid++){
//			for(unsigned j=0; j<4 ; j++){
//				for(unsigned k=0; k<4 ;k++){
//					matCAll[mesh->tets[tetid].vertexIDs[j]][mesh->tets[tetid].vertexIDs[k]] += tetVars[tetid].matC[j][k];
//				}
//			}
//		}
//
//#ifdef Scilab
//		ScilabSetMatrix("matc", matCAll);
//#endif
//	
//		matCChanged = false;
//	}
//}

void PHFemPorousWOMove::CreateVecF1Local(unsigned tetid){

	PHFemMeshNew* mesh = GetPHFemMesh();

	// ������
	tetVars[tetid].vecFw[0].clear();
	tetVars[tetid].vecFo[0].clear();

	for(unsigned localfaceid= 0 ; localfaceid < 4; localfaceid++){
		///	�l�ʑ̂̊e��(l = 0 �` 3) �ɂ��ă��b�V���\�ʂ��ǂ������`�F�b�N����B�\�ʂȂ�A�s��������vecf2array�ɓ����
		
		//�ʂ̉��x:3���_�̑�������
		int faceid = mesh->tets[tetid].faceIDs[localfaceid];
		double surfaceTemp;
		if(mesh->GetPHFemThermo()){
			surfaceTemp = (mesh->GetPHFemThermo()->GetVertexTemp(mesh->faces[faceid].vertexIDs[0]) +mesh->GetPHFemThermo()->GetVertexTemp(mesh->faces[faceid].vertexIDs[1]) + mesh->GetPHFemThermo()->GetVertexTemp(mesh->faces[faceid].vertexIDs[2])) / 3.0;
		}else{
			surfaceTemp = (vertexVars[mesh->faces[faceid].vertexIDs[0]].T + vertexVars[mesh->faces[faceid].vertexIDs[1]].T + vertexVars[mesh->faces[faceid].vertexIDs[2]].T) / 3.0;
		}
		
		if(faceid < (int)mesh->nSurfaceFace && surfaceTemp < 100.0){			///	�\�ʂ���100�������̂Ƃ�
			///	�l�ʑ̂̎O�p�`�̖ʐς��v�Z
			if(faceVars[faceid].area ==0 || faceVars[faceid].deformed ){		///	�ʐς��v�Z����Ă��Ȃ����i�͂��߁j or deformed(�ό`�������E�������)��true�̎�
				faceVars[faceid].area = CalcTriangleArea(mesh->faces[faceid].vertexIDs[0], mesh->faces[faceid].vertexIDs[1], mesh->faces[faceid].vertexIDs[2]);
				faceVars[faceid].deformed = false;
			}
			///	�v�Z���ʂ��s��ɑ��
			///	area�̌v�Z�Ɏg���Ă��Ȃ��_�������Ă���s���������x�N�g���̐ς��Ƃ�
			///	�ϕ��v�Z�����{����l����
			unsigned vtx = mesh->tets[tetid].vertexIDs[0] + mesh->tets[tetid].vertexIDs[1] + mesh->tets[tetid].vertexIDs[2] + mesh->tets[tetid].vertexIDs[3];
			///	area�v�Z�Ɏg���Ă��Ȃ��ߓ_ID�FID
			unsigned ID = vtx -( mesh->faces[mesh->tets[tetid].faceIDs[localfaceid]].vertexIDs[0] + mesh->faces[mesh->tets[tetid].faceIDs[localfaceid]].vertexIDs[1] + mesh->faces[mesh->tets[tetid].faceIDs[localfaceid]].vertexIDs[2] );
			for(unsigned localvtxid=0; localvtxid < 4; localvtxid++){
				if(mesh->tets[tetid].vertexIDs[localvtxid] == ID){					///	�`��֐����P�A�i���Ȃ킿�j����face�ɑΖʂ��钸�_�@�ƈ�v������@���̎���face�Ŗʐϕ�����
					///	�O�k�ɂȂ����b�V���ʂ̖ʐς�0�ŏ��������Ă���
					///	�ȉ���[]�͏�܂ł�[l]�ƈقȂ�B
					///	ID�����Ԗڂ��ɂ���āA�`��֐��̌W�����قȂ�̂ŁA
					if(faceVars[faceid].dryingStep == constRateDrying){
						tetVars[tetid].vecFw[0] -= 0.001 * wDiffAir * (faceVars[faceid].maxVaporCont - faceVars[faceid].vaporCont) / boundaryThick * (1.0/3.0) * faceVars[faceid].area * vecFarray[localvtxid];
						/*
						tetVars[id].vecfW[1] -= faceVars[currentFaceID].surroundFlux * (faceVars[currentFaceID].maxVaporPress - faceVars[currentFaceID].vaporPress) * (1.0/3.0) * faceVars[currentFaceID].area * vecfarray[j];
						*/
					//}else if(faceVars[faceid].dryingStep == fallRateDrying1st){
					//	tetVars[tetid].vecFw[0] -= faceVars[faceid].K * ((vertexVars[mesh->faces[faceid].vertexIDs[0]].Ww + vertexVars[mesh->faces[faceid].vertexIDs[1]].Ww + vertexVars[mesh->faces[currentFaceID].vertexIDs[2]].Ww) / 3 - equilWaterCont) * (1.0/3.0) * faceVars[currentFaceID].area * vecfarray[j];
					//}else if(faceVars[currentFaceID].dryingStep == fallRateDrying2nd){
					//	tetVars[id].vecfW[1] -= faceVars[currentFaceID].surroundFlux * (faceVars[currentFaceID].maxVaporPress - faceVars[currentFaceID].vaporPress) * (1.0/3.0) * faceVars[currentFaceID].area * vecfarray[j];
					}
				}
			}
		}
	}
}

//void PHFemPorousWOMove::CreateVecF2(unsigned tetid){
//	PHFemMeshNew* mesh = GetPHFemMesh();
//
//	if((tetVars[tetid].tetPorosity != tetVars[tetid].preTetPorosity) || (tetVars[tetid].rhoS != tetVars[tetid].preRhoS)){
//		//������
//		tetVars[tetid].vecFw[0].clear();
//		tetVars[tetid].vecFo[0].clear();
//
//		//l=0�̎�f31,1:f32, 2:f33, 3:f34�𐶐�
//		for(unsigned l= 0 ; l < 4; l++){
//
//			//�W���̐ς��Ƃ�
//			//���̐ߓ_�ō\�������l�ʑ̖̂ʐς̐ς��Ƃ�
//			//�l�ʑ̂̐ߓ_1,2,3(0�ȊO)�ō��O�p�`�̖ʐ�
//			//l==0�Ԗڂ̎��A 123	��������
//			//l==1			0 23
//			//l==2			01 3
//			//l==3			012
//			//��CalcTriangleArea�ɓ���邱�Ƃ��ł���悤�ɃA���S���Y�����l����B
//
//			//fw11,fo1
//			if(l==0){
//				tetVars[tetid].vecFw[0] += tetVars[id].wFlux[l] * (1.0/3.0) * CalcTriangleArea( mesh->tets[id].vertexIDs[1],mesh->tets[id].vertexIDs[2],mesh->tets[id].vertexIDs[3] ) * vecfarray[l];
//				tetVars[tetid].vecFo[] += tetVars[id].oFlux[l] * (1.0/3.0) * CalcTriangleArea( mesh->tets[id].vertexIDs[1],mesh->tets[id].vertexIDs[2],mesh->tets[id].vertexIDs[3] ) * vecfarray[l];
//			}
//			//fw12,fo2
//			else if(l==1){
//				tetVars[id].vecfW[0] += tetVars[id].wFlux[l] * (1.0/3.0) * CalcTriangleArea( mesh->tets[id].vertexIDs[0],mesh->tets[id].vertexIDs[2],mesh->tets[id].vertexIDs[3] ) * vecfarray[l];
//				tetVars[id].vecfO += tetVars[id].oFlux[l] * (1.0/3.0) * CalcTriangleArea( mesh->tets[id].vertexIDs[0],mesh->tets[id].vertexIDs[2],mesh->tets[id].vertexIDs[3] ) * vecfarray[l];
//			}
//			//fw13,fo3
//			else if(l==2){
//				tetVars[id].vecfW[0] += tetVars[id].wFlux[l] * (1.0/3.0) * CalcTriangleArea( mesh->tets[id].vertexIDs[0],mesh->tets[id].vertexIDs[1],mesh->tets[id].vertexIDs[3] ) * vecfarray[l];
//				tetVars[id].vecfO += tetVars[id].oFlux[l] * (1.0/3.0) * CalcTriangleArea( mesh->tets[id].vertexIDs[0],mesh->tets[id].vertexIDs[1],mesh->tets[id].vertexIDs[3] ) * vecfarray[l];
//			}
//			//fw14,fo4
//			else if(l==3){
//				tetVars[id].vecfW[0] += tetVars[id].wFlux[l] * (1.0/3.0) * CalcTriangleArea( mesh->tets[id].vertexIDs[0],mesh->tets[id].vertexIDs[1],mesh->tets[id].vertexIDs[2] ) * vecfarray[l];
//				tetVars[id].vecfO += tetVars[id].oFlux[l] * (1.0/3.0) * CalcTriangleArea( mesh->tets[id].vertexIDs[0],mesh->tets[id].vertexIDs[1],mesh->tets[id].vertexIDs[2] ) * vecfarray[l];
//			}
//		}
//	}
//}

//void PHFemPorousWOMove::CreateVecFLocal(unsigned id){
//	PHFemMeshNew* mesh = GetPHFemMesh();
//
//	//CreateVecf1(id);
//
//	//[K1W2]����� �\�ʂɑ�����ʂ�����A���̖ʂ̒��_�̂������x���ς�������̂������
//	for(unsigned i=0; i < 4; i++){
//		unsigned faceid = mesh->tets[id].faceIDs[i];
//		if(faceid < mesh->nSurfaceFace){
//			if(mesh->femThermo){
//				if((vertexVars[mesh->faces[faceid].vertexIDs[0]].preT != mesh->GetPHFemThermo()->GetVertexTemp(mesh->faces[faceid].vertexIDs[0]))
//					|| (vertexVars[mesh->faces[faceid].vertexIDs[1]].preT != mesh->GetPHFemThermo()->GetVertexTemp(mesh->faces[faceid].vertexIDs[1]))
//					|| (vertexVars[mesh->faces[faceid].vertexIDs[2]].preT != mesh->GetPHFemThermo()->GetVertexTemp(mesh->faces[faceid].vertexIDs[2]))){
//					CreateVecf2(id);
//					vecfChanged = true;
//					matVecChanged = true;
//				}
//			}else{
//				if((vertexVars[mesh->faces[faceid].vertexIDs[0]].preT != vertexVars[mesh->faces[faceid].vertexIDs[0]].T)
//					|| (vertexVars[mesh->faces[faceid].vertexIDs[1]].preT != vertexVars[mesh->faces[faceid].vertexIDs[1]].T)
//					|| (vertexVars[mesh->faces[faceid].vertexIDs[2]].preT != vertexVars[mesh->faces[faceid].vertexIDs[2]].T)){
//					CreateVecf2(id);
//					vecfChanged = true;
//					matVecChanged = true;
//				}
//			}
//		}
//	}
//}

//void PHFemPorousWOMove::CreateVecFAll(){
//	PHFemMeshNew* mesh = GetPHFemMesh();
//
//	//�v�f�̐ߓ_�ԍ��̏ꏊ�ɁA���̐ߓ_��f�̒l������
//	//j:�v�f�̒��̉��Ԗڂ�
//	if(vecfChanged){
//		vecFWAllSum.clear();
//		vecFOAllSum.clear();
//		for(unsigned tetid=0; tetid < mesh->tets.size(); tetid++){
//			for(unsigned j =0;j < 4; j++){
//				int vtxid0 = GetPHFemMesh()->tets[tetid].vertexIDs[j];
//				vecFWAllSum[vtxid0][0] += tetVars[tetid].vecfW[0][j] + tetVars[tetid].vecfW[1][j];
//				vecFOAllSum[vtxid0][0] += tetVars[tetid].vecfO[j];
//			}
//		}
//
//#ifdef Scilab
//		ScilabSetMatrix("vecfw", vecFWAllSum);
//		ScilabSetMatrix("vecfo", vecFOAllSum);
//#endif
//	}
//	vecfChanged = false;
//}

void PHFemPorousWOMove::CreateMatVecAll(){
	PHFemMeshNew* mesh = GetPHFemMesh();

	for(unsigned tetid=0; tetid < mesh->tets.size(); tetid++){
		CreateMatWOPcVecF2Local(tetid);
		CreateMatCLocal(tetid);
		CreateVecF1Local(tetid);
	}

	//�S�̍����s������
	if(matWOPcVecF2Changed){
#ifndef usingG
		matWwAll.clear();
		matOwAll.clear();
		matWoAll.clear();
		matOoAll.clear();
#endif
		matPcwAll.clear();
		matPcoAll.clear();
		for(unsigned tetid=0; tetid < mesh->tets.size(); tetid++){
			for(unsigned j=0; j<4 ; j++){
				for(unsigned k=0; k<4 ;k++){
#ifndef usingG
					matWwAll[mesh->tets[tetid].vertexIDs[j]][mesh->tets[tetid].vertexIDs[k]] += tetVars[tetid].matWw[j][k];
					matOwAll[mesh->tets[tetid].vertexIDs[j]][mesh->tets[tetid].vertexIDs[k]] += tetVars[tetid].matOw[j][k];
					matWoAll[mesh->tets[tetid].vertexIDs[j]][mesh->tets[tetid].vertexIDs[k]] += tetVars[tetid].matWo[j][k];
					matOoAll[mesh->tets[tetid].vertexIDs[j]][mesh->tets[tetid].vertexIDs[k]] += tetVars[tetid].matOo[j][k];
#endif
					matPcwAll[mesh->tets[tetid].vertexIDs[j]][mesh->tets[tetid].vertexIDs[k]] += tetVars[tetid].matPcw[j][k];
					matPcoAll[mesh->tets[tetid].vertexIDs[j]][mesh->tets[tetid].vertexIDs[k]] += tetVars[tetid].matPco[j][k];
				}
			}
		}

#ifdef Scilab
		ScilabSetMatrix("matww", matWwAll);
		ScilabSetMatrix("matow", matOwAll);
		ScilabSetMatrix("matwo", matWoAll);
		ScilabSetMatrix("matoo", matOoAll);
		ScilabSetMatrix("matpcw", matPcwAll);
		ScilabSetMatrix("matpco", matPcoAll);
#endif
		matWOPcVecF2Changed = false;
		keisuChanged = true;
	}
#ifndef usingG
	if(matCChanged){
		matCAll.clear();
		for(unsigned tetid=0; tetid < mesh->tets.size(); tetid++){
			for(unsigned j=0; j<4 ; j++){
				for(unsigned k=0; k<4 ;k++){
					matCAll[mesh->tets[tetid].vertexIDs[j]][mesh->tets[tetid].vertexIDs[k]] += tetVars[tetid].matC[j][k];
				}
			}
		}

#ifdef Scilab
		ScilabSetMatrix("matc", matCAll);
#endif
	
		matCChanged = false;
		keisuChanged = true;
	}
#endif

	vecFwAll.clear();
	vecFoAll.clear();
	for(unsigned tetid=0; tetid < mesh->tets.size(); tetid++){
		for(unsigned j=0; j < 4; j++){
			vecFwAll[mesh->tets[tetid].vertexIDs[j]][0] += tetVars[tetid].vecFw[0][j] + tetVars[tetid].vecFw[1][j];
			vecFoAll[mesh->tets[tetid].vertexIDs[j]][0] += tetVars[tetid].vecFo[0][j] + tetVars[tetid].vecFo[1][j];
		}
	}
	vecFwFinal.clear();
	vecFoFinal.clear();
	
	for(unsigned vtxid0=0; vtxid0 < mesh->vertices.size(); vtxid0++){
		vecFwFinal[vtxid0][0] += vecFwAll[vtxid0][0];
		vecFoFinal[vtxid0][0] += vecFoAll[vtxid0][0];
		for(unsigned vtxid1=0; vtxid1 < mesh->vertices.size(); vtxid1++){
			vecFwFinal[vtxid0][0] += matPcwAll[vtxid0][vtxid1] * PcVecAll[vtxid1][0];
			vecFoFinal[vtxid0][0] += matPcoAll[vtxid0][vtxid1] * PcVecAll[vtxid1][0];
		}
	}

#ifdef Scilab
		ScilabSetMatrix("vecfw", vecFwAll);
		ScilabSetMatrix("vecfo", vecFoAll);
#endif

}

void PHFemPorousWOMove::CreateRhoWVecAll(){
	rhoWVecAll.resize(GetPHFemMesh()->vertices.size(), 1);
	SetRhoWAllToRhoWVecAll();
}

void PHFemPorousWOMove::CreateRhoOVecAll(){
	rhoOVecAll.resize(GetPHFemMesh()->vertices.size(), 1);
	SetRhoOAllToRhoOVecAll();
}

void PHFemPorousWOMove::InitPcVecAll(){
	PcVecAll.resize(GetPHFemMesh()->vertices.size(), 1);
}

void PHFemPorousWOMove::InitTcAll(double temp){
	for(unsigned i =0; i <GetPHFemMesh()->vertices.size();i++){
		vertexVars[i].Tc = temp;
	}
}

double PHFemPorousWOMove::CalcTriangleArea(int id0, int id1, int id2){

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

double PHFemPorousWOMove::CalcTetrahedraVolume(FemTet tet){

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
	
	return tempMat44.det() / 6.0;
}

PTM::TMatrixRow<4,4,double> PHFemPorousWOMove::Create44Mat21(){
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

PTM::TMatrixCol<4,1,double> PHFemPorousWOMove::Create41Vec1(){
	PTM::TMatrixCol<4,1,double> Mat1temp;
	for(int i =0; i <4 ; i++){
				Mat1temp[i][0] = 1.0;
	}
	return Mat1temp;
}

void PHFemPorousWOMove::CalcWOContentDirect(double dt, double eps){
	PHFemMeshNew* mesh = GetPHFemMesh();
	//lapack���p
	int n = (int)mesh->vertices.size();

	if(keisuChanged){
		//	�W���s��̍쐬
		keisuW.resize(mesh->vertices.size(),mesh->vertices.size());
		keisuO.resize(mesh->vertices.size(),mesh->vertices.size());
		keisuW.clear();
		keisuO.clear();
		uhenW.resize(mesh->vertices.size(),1);
		uhenO.resize(mesh->vertices.size(),1);
		uhenW.clear();
		uhenO.clear();

		//��=0�̂Ƃ�
		if(eps == 0.0){
			keisuW = 1 / dt * matCAll;
			keisuO = 1 / dt * matCAll;
			uhenW = (-matWwAll+1/dt*matCAll)*rhoWVecAll - matOwAll*rhoOVecAll + vecFwAll + matPcwAll*PcVecAll;
			uhenO = -matWoAll*rhoWVecAll + (-matOoAll+1/dt*matCAll)*rhoOVecAll + vecFoAll + matPcoAll*PcVecAll;
		}
		////��=1/2�̂Ƃ�
		//else if(eps == 0.5){
		//	keisuW = 2 * matK2WAll.inv() * (0.5 * matK1WAll + 1/dt * matCWAll) - 0.5 * (0.5*matK2OAll + 1/dt*matCOAll).inv() *matK1OAll;
		//	keisuO = 2 * matK1OAll.inv() * (0.5 * matK2OAll + 1/dt * matCOAll) - 0.5 * (0.5*matK1WAll + 1/dt*matCWAll).inv() *matK2WAll;
		//	uhenW = (2*matK2WAll.inv()*(-0.5*matK1WAll+1/dt*matCWAll)+0.5*(0.5*matK2OAll+1/dt*matCOAll).inv()*matK1OAll)*WwVecAll
		//										- (idMat+(0.5*matK2OAll+1/dt*matCOAll).inv()*(-0.5*matK2OAll+1/dt*matCOAll)) * WoVecAll
		//													+ 2*matK2WAll.inv()*vecFWAllSum - (0.5*matK2OAll+1/dt*matCOAll).inv()*vecFOAllSum;
		//	uhenO = -(idMat+(0.5*matK1WAll+1/dt*matCWAll).inv()*(-0.5*matK1WAll+1/dt*matCWAll)) * WwVecAll
		//			+ (2*matK1OAll.inv()*(-0.5*matK2OAll+1/dt*matCOAll)+0.5*(0.5*matK1WAll+1/dt*matCWAll).inv()*matK2WAll)*WoVecAll
		//													- (0.5*matK1WAll+1/dt*matCWAll).inv()*vecFWAllSum + 2*matK1OAll.inv()*vecFOAllSum;
		//}
		////��=1�̂Ƃ�
		else{
			keisuW = matWwAll + 1 / dt * matCAll;
			keisuO = matOoAll + 1 / dt * matCAll;
			uhenW = 1/dt*matCAll*rhoWVecAll - matOwAll * rhoOVecAll  + 0*vecFwAll + matPcwAll*PcVecAll;
			uhenO = -matWoAll*rhoWVecAll + 1/dt*matCAll*rhoOVecAll + 0*vecFoAll + matPcoAll*PcVecAll;
		}
		keisuChanged = false;
	}

	PTM::VVector<double> xW;
	PTM::VVector<double> xO;

	PTM::VVector<int> ip;
	ip.resize(n);
	
	typedef double element_type;
	typedef bindings::remove_imaginary<element_type>::type real_type ;
	typedef bindings::remove_imaginary<int>::type int_type ;
	typedef ublas::vector< real_type > vector_type;
	typedef ublas::matrix< element_type, ublas::column_major > matrix_type;
	ublas::vector<int_type> ipivW(n);
	ublas::vector<int_type> ipivO(n);
	matrix_type mmW(n, n);
	matrix_type mmO(n, n);
	vector_type bbW(n);
	vector_type bbO(n);
	for(int i=0; i<n; ++i){
		bbW[i] = uhenW[i];
		bbO[i] = uhenO[i];
	}
	for(int i=0; i<n; ++i){
		for(int j=0; j<n; ++j){
			mmW.at_element(i, j)=keisuW[i][j];
			mmO.at_element(i, j)=keisuO[i][j];
		}
	}
	double detW = lapack::gesv(mmW, ipivW, bbW);
	double detO = lapack::gesv(mmO, ipivO, bbO);
	xW.resize(n);
	xO.resize(n);
	for(int i=0; i<n; ++i){
		xW[i] = bbW[i];
		xO[i] = bbO[i];
	}
	for(int i=0; i<n; ++i){
		rhoWVecAll[i][0] = xW[i];
		rhoOVecAll[i][0] = xO[i];
	}

	for(unsigned tetid=0; tetid < mesh->tets.size(); tetid++){
		tetVars[tetid].sDensChanged = false;
	}
}

void PHFemPorousWOMove::CalcWOContentDirect2(double dt, double eps){
	PHFemMeshNew* mesh = GetPHFemMesh();

	int n = (int)mesh->vertices.size();

	if(keisuChanged){
		//	�W���s��̍쐬
		keisuW.resize(mesh->vertices.size(),mesh->vertices.size());
		keisuO.resize(mesh->vertices.size(),mesh->vertices.size());
		keisuW.clear();
		keisuO.clear();
		uhenW.resize(mesh->vertices.size(),1);
		uhenO.resize(mesh->vertices.size(),1);
		uhenW.clear();
		uhenO.clear();

		//��=0�̂Ƃ�
		if(eps == 0.0){
			keisuW = 1 / dt * matCAll;
			keisuO = 1 / dt * matCAll;
			uhenW = (-matWwAll+1/dt*matCAll)*rhoWVecAll - matOwAll*rhoOVecAll + vecFwAll + matPcwAll*PcVecAll;
			uhenO = -matWoAll*rhoWVecAll + (-matOoAll+1/dt*matCAll)*rhoOVecAll + vecFoAll + matPcoAll*PcVecAll;
		}
		////��=1/2�̂Ƃ�
		//else if(eps == 0.5){
		//	keisuW = 2 * matK2WAll.inv() * (0.5 * matK1WAll + 1/dt * matCWAll) - 0.5 * (0.5*matK2OAll + 1/dt*matCOAll).inv() *matK1OAll;
		//	keisuO = 2 * matK1OAll.inv() * (0.5 * matK2OAll + 1/dt * matCOAll) - 0.5 * (0.5*matK1WAll + 1/dt*matCWAll).inv() *matK2WAll;
		//	uhenW = (2*matK2WAll.inv()*(-0.5*matK1WAll+1/dt*matCWAll)+0.5*(0.5*matK2OAll+1/dt*matCOAll).inv()*matK1OAll)*WwVecAll
		//										- (idMat+(0.5*matK2OAll+1/dt*matCOAll).inv()*(-0.5*matK2OAll+1/dt*matCOAll)) * WoVecAll
		//													+ 2*matK2WAll.inv()*vecFWAllSum - (0.5*matK2OAll+1/dt*matCOAll).inv()*vecFOAllSum;
		//	uhenO = -(idMat+(0.5*matK1WAll+1/dt*matCWAll).inv()*(-0.5*matK1WAll+1/dt*matCWAll)) * WwVecAll
		//			+ (2*matK1OAll.inv()*(-0.5*matK2OAll+1/dt*matCOAll)+0.5*(0.5*matK1WAll+1/dt*matCWAll).inv()*matK2WAll)*WoVecAll
		//													- (0.5*matK1WAll+1/dt*matCWAll).inv()*vecFWAllSum + 2*matK1OAll.inv()*vecFOAllSum;
		//}
		////��=1�̂Ƃ�
		else{
			keisuW = matWwAll + 1 / dt * matCAll;
			keisuO = matOoAll + 1 / dt * matCAll;
			uhenW = 1/dt*matCAll*rhoWVecAll - matOwAll * rhoOVecAll  + vecFwAll + matPcwAll*PcVecAll;
			uhenO = -matWoAll*rhoWVecAll + 1/dt*matCAll*rhoOVecAll + vecFoAll + matPcoAll*PcVecAll;
		}
		keisuWInv = inv(keisuW);
		keisuOInv = inv(keisuO);
		keisuChanged = false;
	}

	PTM::VVector<double> rhowTemp = keisuWInv * uhenW;
	PTM::VVector<double> rhooTemp = keisuOInv * uhenO;
	for(unsigned i=0; i < mesh->vertices.size(); i++){
		rhoWVecAll[i][0] = rhowTemp[i];
		rhoOVecAll[i][0] = rhooTemp[i];
	}

	for(unsigned tetid=0; tetid < mesh->tets.size(); tetid++){
		tetVars[tetid].sDensChanged = false;
	}
}

void PHFemPorousWOMove::CalcWOContentUsingGaussSeidel(unsigned NofCyc, double dt, double eps){
	PHFemMeshNew* mesh = GetPHFemMesh();
	
	double _eps = 1-eps;			// 1-eps�̌v�Z�ɗ��p
	bool DoCalc =true;											//���񂾂��萔�x�N�g��b�̌v�Z���s��bool		//NofCyc��0�̎��ɂ���΂����̂���
	for(unsigned i=0; i < NofCyc; i++){							//�K�E�X�U�C�f���̌v�Z���[�v
		if(DoCalc){												
			if(keisuChanged){												//D_ii�̍쐬�@�`�󂪍X�V���ꂽ�ۂ�1�x�����s���΂悢
				for(unsigned j =0; j < mesh->vertices.size() ; j++){	
					//for(unsigned k =0;k < vertices.size(); k++){
					//	DSTR << "dMatCAll "<< k << " : " << dMatCAll[0][k] << std::endl;
					//}
					_dMatWAll.resize(1, mesh->vertices.size());
					_dMatOAll.resize(1, mesh->vertices.size());
					_dMatWAll[0][j] = 1.0/ (eps * dMatWwAll[0][j] + 1.0/dt * dMatCAll[0][j]);		//1 / D__ii	�����߂�
					_dMatOAll[0][j] = 1.0/ (eps * dMatOoAll[0][j] + 1.0/dt * dMatCAll[0][j]);
					int debughogeshi =0;
				}
				keisuChanged = false;
			}
			//	 1      1        1  
			//	--- ( - - [K] + ---[C] ){T(t)} + {F} 
			//	D_jj    2       ��t
			//

			for(unsigned j =0; j < mesh->vertices.size() ; j++){		//���񃋁[�v����	�W���x�N�g��bVecAll�̐������v�Z
				bwVecAll[j][0] = 0.0;							//bVecAll[j][0]�̏�����
				boVecAll[j][0] = 0.0;
				//�ߓ_������edges���Ɂ@�Ίp����(j,j)�Ɣ�Ίp����(j,?)���Ɍv�Z
				//�Ίp�����́Avertices[j].k or .c �ɓ����Ă���l���A��Ίp������edges[hoge].vertices[0] or vertices[1] .k or .c�ɓ����Ă���l��p����
				//�@)��Ίp�����ɂ���
				for(unsigned k =0;k < mesh->vertices[j].edgeIDs.size() ; k++){
					unsigned edgeId = mesh->vertices[j].edgeIDs[k];
					//���t�@�N�^�����O	�ȉ��̏�������ɂ���j>edges[edgeId].vertices[0] �Ƃ����łȂ����Ƃŕ������ق����R�ꂪ�o��S�z�͂Ȃ��H
					if( j != mesh->edges[edgeId].vertexIDs[0]){					//�ߓ_�ԍ�j��edges.vertices[0]���قȂ�ߓ_�ԍ��̎�:��Ίp����
						unsigned vtxid0 = mesh->edges[edgeId].vertexIDs[0];
#ifndef aboutG
						bwVecAll[j][0] += (-_eps * edgeVars[edgeId].ww + 1.0/dt * edgeVars[edgeId].c ) * rhoWVecAll[vtxid0][0] -_eps * edgeVars[edgeId].ow * rhoOVecAll[vtxid0][0];
						boVecAll[j][0] += -_eps * edgeVars[edgeId].wo * rhoWVecAll[vtxid0][0] + (-_eps * edgeVars[edgeId].oo + 1.0/dt * edgeVars[edgeId].c) * rhoOVecAll[vtxid0][0];
#endif
#ifdef aboutG
						bwVecAll[j][0] += (-_eps * edgeVars[edgeId].ww + 1.0/dt * edgeVars[edgeId].c ) * rhoWVecAll[vtxid0][0] -1 * edgeVars[edgeId].ow * rhoOVecAll[vtxid0][0];
						boVecAll[j][0] += -1 * edgeVars[edgeId].wo * rhoWVecAll[vtxid0][0] + (-_eps * edgeVars[edgeId].oo + 1.0/dt * edgeVars[edgeId].c) * rhoOVecAll[vtxid0][0];
#endif
					}
					else if( j != mesh->edges[edgeId].vertexIDs[1] ){			//�ߓ_�ԍ�j��edges.vertices[1]���قȂ�ߓ_�ԍ��̎�:��Ίp����
						unsigned vtxid1 = mesh->edges[edgeId].vertexIDs[1];
#ifndef aboutG 
						bwVecAll[j][0] += (-_eps * edgeVars[edgeId].ww + 1.0/dt * edgeVars[edgeId].c ) * rhoWVecAll[vtxid1][0] -_eps * edgeVars[edgeId].ow * rhoOVecAll[vtxid1][0];
						boVecAll[j][0] += -_eps * edgeVars[edgeId].wo * rhoWVecAll[vtxid1][0] + (-_eps * edgeVars[edgeId].oo + 1.0/dt * edgeVars[edgeId].c) * rhoOVecAll[vtxid1][0];
#endif
#ifdef aboutG
						bwVecAll[j][0] += (-_eps * edgeVars[edgeId].ww + 1.0/dt * edgeVars[edgeId].c ) * rhoWVecAll[vtxid1][0] -1 * edgeVars[edgeId].ow * rhoOVecAll[vtxid1][0];
						boVecAll[j][0] += -1 * edgeVars[edgeId].wo * rhoWVecAll[vtxid1][0] + (-_eps * edgeVars[edgeId].oo + 1.0/dt * edgeVars[edgeId].c) * rhoOVecAll[vtxid1][0];
#endif
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
#ifndef aboutG
				bwVecAll[j][0] += (-_eps * dMatWwAll[0][j] + 1.0/dt * dMatCAll[0][j] ) * rhoWVecAll[j][0] -_eps * dMatOwAll[0][j] * rhoOVecAll[j][0];
				boVecAll[j][0] += -_eps * dMatWoAll[0][j] * rhoWVecAll[j][0] + (-_eps * dMatOoAll[0][j] + 1.0/dt * dMatCAll[0][j]) * rhoOVecAll[j][0];
#endif
#ifdef aboutG
				bwVecAll[j][0] += (-_eps * dMatWwAll[0][j] + 1.0/dt * dMatCAll[0][j] ) * rhoWVecAll[j][0] -1 * dMatOwAll[0][j] * rhoOVecAll[j][0];
				boVecAll[j][0] += -1 * dMatWoAll[0][j] * rhoWVecAll[j][0] + (-_eps * dMatOoAll[0][j] + 1.0/dt * dMatCAll[0][j]) * rhoOVecAll[j][0];
#endif
				//FEMLOG << "bVecAll[" << j <<"][0] : " << bVecAll[j][0] << std::endl;		// DSTR
				//FEMLOG << "dMatKAll[0][" << j <<"] : " << dMatKAll[0][j] << std::endl;			// DSTR
				//FEMLOG << "dMatCAll[0][" << j <<"] : " << dMatCAll[0][j] << std::endl;			// DSTR
				//  {F}�����Z
				bwVecAll[j][0] += vecFwFinal[j][0];		//F�����Z
				boVecAll[j][0] += vecFoFinal[j][0];
				//DSTR << " vecFAllSum[" << j << "] : "  << vecFAllSum[j] << std::endl;
				//DSTR << std::endl;
				//D_ii�Ŋ��� �˂��̏ꏊ�́A�����ŗǂ��́H�ǂ��܂Ŋ|���Z����́H
				bwVecAll[j][0] = bwVecAll[j][0] * _dMatWAll[0][j];
				boVecAll[j][0] = boVecAll[j][0] * _dMatOAll[0][j];
				//FEMLOG(ofs << "bVecAll[" << j <<"][0] * _dMatAll : " << bVecAll[j][0] << std::endl);
				//FEMLOG(ofs << "TVecAll[" << j <<"] : " << TVecAll[j] << std::endl);
			}
			DoCalc = false;			//����̃��[�v�����ŗ��p
			//�l�������Ă��邩�A���킻�������`�F�b�N
			//DSTR << "bVecAll[j][0] : " << std::endl;
			//for(unsigned j =0;j <mesh->vertices.size() ; j++){
			//	DSTR << j << " : "<< bVecAll[j][0] << std::endl;
			//}
			int debughogeshi =0;
		}		//if(DoCalc){...}

		//	 1      
		//	--- [F]{T(t+dt)}
		//	D_jj 		
		//[F] = eps(ilon) [K] +1/dt [C] ����Ίp����������(-1)������������
		//�G�b�W�ɓ����Ă��鐬����-1��������̂ł͂Ȃ��A�Ō��-1��������B
		//
		for(unsigned j =0; j < mesh->vertices.size() ; j++){
			//T(t+dt) = �̎�
			//	�܂�tempkj�����
			double tempkjw = 0.0;			//�K�E�X�U�C�f���̓r���v�Z�ŏo�Ă���F�̐����v�Z�Ɏg�p����ꎞ�ϐ�
			double tempkjo = 0.0;
			for(unsigned k =0;k < mesh->vertices[j].edgeIDs.size() ; k++){
				unsigned edgeId = mesh->vertices[j].edgeIDs[k]; 
				if( j != mesh->edges[edgeId].vertexIDs[0]){					//�ߓ_�ԍ�j��edges.vertices[0]���قȂ�ߓ_�ԍ��̎�:��Ίp����		//OK
					unsigned vtxid0 = mesh->edges[edgeId].vertexIDs[0];
					//DSTR << "TVecAll["<< vtxid0<<"] : " << TVecAll[vtxid0] <<std::endl;
					//TVecAll[j] +=_dMatAll[j][0] * -(1.0/2.0 * edges[edgeId].k + 1.0/dt * edges[edgeId].c ) * TVecAll[vtxid0] + bVecAll[j][0]; 
					//DSTR << "j : " << j << ", vtxid0 : " << vtxid0 <<", edges[edgeId].vertices[0] : " << mesh->edges[edgeId].vertexIDs[0] <<  std::endl;
					tempkjw += (eps * edgeVars[edgeId].ww + 1.0/dt * edgeVars[edgeId].c) * rhoWVecAll[vtxid0][0];
					tempkjo += (eps * edgeVars[edgeId].oo + 1.0/dt * edgeVars[edgeId].c) * rhoOVecAll[vtxid0][0]; 
				}
				else if( j != mesh->edges[edgeId].vertexIDs[1] ){			//�ߓ_�ԍ�j��edges.vertices[1]���قȂ�ߓ_�ԍ��̎�:��Ίp����
					unsigned vtxid1 = mesh->edges[edgeId].vertexIDs[1];
					//DSTR << "TVecAll["<< vtxid1<<"] : " << TVecAll[vtxid1] <<std::endl;
					tempkjw += (eps * edgeVars[edgeId].ww + 1.0/dt * edgeVars[edgeId].c) * rhoWVecAll[vtxid1][0];
					tempkjo += (eps * edgeVars[edgeId].oo + 1.0/dt * edgeVars[edgeId].c) * rhoOVecAll[vtxid1][0];
				}
				else{
					//��L�̂ǂ���ł��Ȃ��ꍇ�A�G���[
					DSTR << "edges.vertex has 3 vertexies or any other problem" <<std::endl;
				}
				//	for Debug
				//DSTR << "TVecAll:";
				//DSTR << "edges[" << edgeId << "].vertices[0] : " << mesh->edges[edgeId].vertexIDs[0] << std::endl;
				//DSTR << "edges[" << edgeId << "].vertices[1] : " << mesh->edges[edgeId].vertexIDs[1] << std::endl;
				//int hogeshi =0;
			}
			//	TVecAll�̌v�Z
			rhoWVecAll[j][0] = _dMatWAll[0][j] * (-1.0 * tempkjw) + bwVecAll[j][0];			//	-b = D^(-1) [ (-1/2 * K + 1/dt * C ){T(t+dt)} + {F} ]�Ȃ̂ŁAbVecAll�͂����̉��Z�ł悢
			rhoOVecAll[j][0] = _dMatOAll[0][j] * (-1.0 * tempkjo) + boVecAll[j][0];
		}
	}

	for(unsigned tetid=0; tetid < mesh->tets.size(); tetid++){
		tetVars[tetid].sDensChanged = false;
	}
}

//void PHFemPorousWOMove::CalcWOContentUsingScilab(double dt){
//	PHFemMeshNew* mesh = GetPHFemMesh();
//	double eps = 0.5; 
//
//	dtMat[0][0] = dt;
//	//ScilabSetMatrix("dt", dtMat);
//
//	ScilabJob("wwveccp = wwvecall");
//	ScilabJob("woveccp = wovecall");
//
//	int error;
//
//	error = ScilabSetScalarDouble("dt", dt);
//	error = ScilabSetScalarDouble("eps", eps);
//
//	if(keisuChanged){
//
//		//��=0�̂Ƃ�
//		if(eps == 0.0){
//			leftKeisuW = 1 / dt * matCWAll;
//			rightKeisuWWw = -matK1WAll+1/dt*matCWAll;
//			rightKeisuWWo = -matK2WAll;
//			leftKeisuO = 1 / dt * matCOAll;
//			rightKeisuOWw = -matK1OAll;
//			rightKeisuOWo = -matK2OAll+1/dt*matCOAll;
//			for(unsigned i=0; i < mesh->vertices.size(); i++){
//				for(unsigned j=0; j < mesh->vertices.size(); j++){
//					if(i==j){
//						rightKeisuWFw[i][j] = 1.0;
//						rightKeisuOFo[i][j] = 1.0;
//					}
//				}
//			}
//		}
//		//��=1/2�̂Ƃ�
//		else if(eps == 0.5){
//			PTM::VMatrixRow<double> tmat;
//
///*
//			ScilabGetMatrix(tmat, "matk1w");
//			out1 << "matk1w" << std::endl;
//			for(unsigned i=0; i<tmat.height(); i++){
//				for(unsigned j=0; j<tmat.width(); j++){
//					out1 << tmat[i][j] << ",";
//				}
//				out1 << std::endl;
//			}
//			out1 << std::endl;
//
//			ScilabGetMatrix(tmat, "matk2w");
//			out1 << "matk2w" << std::endl;
//			for(unsigned i=0; i<tmat.height(); i++){
//				for(unsigned j=0; j<tmat.width(); j++){
//					out1 << tmat[i][j] << ",";
//				}
//				out1 << std::endl;
//			}
//			out1 << std::endl;
//
//			ScilabGetMatrix(tmat, "matk1o");
//			out1 << "matk1o" << std::endl;
//			for(unsigned i=0; i<tmat.height(); i++){
//				for(unsigned j=0; j<tmat.width(); j++){
//					out1 << tmat[i][j] << ",";
//				}
//				out1 << std::endl;
//			}
//			out1 << std::endl;
//
//			ScilabGetMatrix(tmat, "matk2o");
//			out1 << "matk2o" << std::endl;
//			for(unsigned i=0; i<tmat.height(); i++){
//				for(unsigned j=0; j<tmat.width(); j++){
//					out1 << tmat[i][j] << ",";
//				}
//				out1 << std::endl;
//			}
//			out1 << std::endl;
//
//			ScilabGetMatrix(tmat, "matcw");
//			out1 << "matcw" << std::endl;
//			for(unsigned i=0; i<tmat.height(); i++){
//				for(unsigned j=0; j<tmat.width(); j++){
//					out1 << tmat[i][j] << ",";
//				}
//				out1 << std::endl;
//			}
//			out1 << std::endl;
//
//			ScilabGetMatrix(tmat, "matco");
//			out1 << "matco" << std::endl;
//			for(unsigned i=0; i<tmat.height(); i++){
//				for(unsigned j=0; j<tmat.width(); j++){
//					out1 << tmat[i][j] << ",";
//				}
//				out1 << std::endl;
//			}
//			out1 << std::endl;
//
//			ScilabGetMatrix(tmat, "vecfw");
//			out1 << "vecfw" << std::endl;
//			for(unsigned i=0; i<tmat.height(); i++){
//				for(unsigned j=0; j<tmat.width(); j++){
//					out1 << tmat[i][j] << ",";
//				}
//				out1 << std::endl;
//			}
//			out1 << std::endl;
//
//			ScilabGetMatrix(tmat, "vecfo");
//			out1 << "vecfo" << std::endl;
//			for(unsigned i=0; i<tmat.height(); i++){
//				for(unsigned j=0; j<tmat.width(); j++){
//					out1 << tmat[i][j] << ",";
//				}
//				out1 << std::endl;
//			}
//			out1 << std::endl;
//*/
//#ifdef shrink
//			ScilabJob("leftkeisuw = 2*inv(matk2w) * (eps * matk1w + 1/dt*matcw) - eps*inv(eps * matk2o + 1/dt*matco) * matk1o");
//			/*
//			ScilabGetMatrix(tmat, "leftkeisuw");
//			out1 << "leftkeisuw" << std::endl;
//			for(unsigned i=0; i<tmat.height(); i++){
//				for(unsigned j=0; j<tmat.width(); j++){
//					out1 << tmat[i][j] << ",";
//				}
//				out1 << std::endl;
//			}
//			out1 << std::endl;
//			*/
//			ScilabJob("rightkeisuwWw = 2*inv(matk2w) * (-eps * matk1w + 1/dt*matcw) + eps*inv(eps * matk2o + 1/dt*matco) * matk1o");
//			/*
//			ScilabGetMatrix(tmat, "rightkeisuwWw");
//			out1 << "rightkeisuwWw" << std::endl;
//			for(unsigned i=0; i<tmat.height(); i++){
//				for(unsigned j=0; j<tmat.width(); j++){
//					out1 << tmat[i][j] << ",";
//				}
//				out1 << std::endl;
//			}
//			out1 << std::endl;
//			*/
//			ScilabJob("rightkeisuwWo = -(eye(matk1w) + inv(eps*matk2o + 1/dt*matco) * (-eps*matk2o+1/dt*matco))");
//			/*
//			ScilabGetMatrix(tmat, "rightkeisuwWo");
//			out1 << "rightkeisuwWo" << std::endl;
//			for(unsigned i=0; i<tmat.height(); i++){
//				for(unsigned j=0; j<tmat.width(); j++){
//					out1 << tmat[i][j] << ",";
//				}
//				out1 << std::endl;
//			}
//			out1 << std::endl;
//			*/
//			ScilabJob("rightkeisuwFw = 2*inv(matk2w)");
//			/*
//			ScilabGetMatrix(tmat, "rightkeisuwFw");
//			out1 << "rightkeisuwFw" << std::endl;
//			for(unsigned i=0; i<tmat.height(); i++){
//				for(unsigned j=0; j<tmat.width(); j++){
//					out1 << tmat[i][j] << ",";
//				}
//				out1 << std::endl;
//			}
//			out1 << std::endl;
//			*/
//			ScilabJob("rightkeisuwFo = -inv(eps*matk2o+1/dt*matco)");
//			/*
//			ScilabGetMatrix(tmat, "rightkeisuwFo");
//			out1 << "rightkeisuwFo" << std::endl;
//			for(unsigned i=0; i<tmat.height(); i++){
//				for(unsigned j=0; j<tmat.width(); j++){
//					out1 << tmat[i][j] << ",";
//				}
//				out1 << std::endl;
//			}
//			out1 << std::endl;
//			*/
//			ScilabJob("leftkeisuo = 1/2*matk2o + 1/dt*matco");
//			/*
//			ScilabGetMatrix(tmat, "leftkeisuo");
//			out1 << "leftkeisuo" << std::endl;
//			for(unsigned i=0; i<tmat.height(); i++){
//				for(unsigned j=0; j<tmat.width(); j++){
//					out1 << tmat[i][j] << ",";
//				}
//				out1 << std::endl;
//			}
//			out1 << std::endl;
//			*/
//			ScilabJob("rightkeisuoWw = -1/2*matk1o");
//			/*
//			ScilabGetMatrix(tmat, "rightkeisuoWw");
//			out1 << "rightkeisuoWw" << std::endl;
//			for(unsigned i=0; i<tmat.height(); i++){
//				for(unsigned j=0; j<tmat.width(); j++){
//					out1 << tmat[i][j] << ",";
//				}
//				out1 << std::endl;
//			}
//			out1 << std::endl;
//			*/
//			ScilabJob("rightkeisuoWo = -1/2*matk2o + 1/dt*matco");
//			/*
//			ScilabGetMatrix(tmat, "rightkeisuoWo");
//			out1 << "rightkeisuoWo" << std::endl;
//			for(unsigned i=0; i<tmat.height(); i++){
//				for(unsigned j=0; j<tmat.width(); j++){
//					out1 << tmat[i][j] << ",";
//				}
//				out1 << std::endl;
//			}
//			out1 << std::endl;
//*/
//			ScilabJob("rightkeisuoFw = 0");
//			/*
//			ScilabGetMatrix(tmat, "rightkeisuoFw");
//			out1 << "rightkeisuoFw" << std::endl;
//			for(unsigned i=0; i<tmat.height(); i++){
//				for(unsigned j=0; j<tmat.width(); j++){
//					out1 << tmat[i][j] << ",";
//				}
//				out1 << std::endl;
//			}
//			out1 << std::endl;
//			*/
//			ScilabJob("rightkeisuoFo = 1");
//			/*
//			ScilabGetMatrix(tmat, "rightkeisuoFo");
//			out1 << "rightkeisuoFo" << std::endl;
//			for(unsigned i=0; i<tmat.height(); i++){
//				for(unsigned j=0; j<tmat.width(); j++){
//					out1 << tmat[i][j] << ",";
//				}
//				out1 << std::endl;
//			}
//			out1 << std::endl;
//			*/
//			ScilabJob("rightkeisuoWwnext = -1/2*matk1o");
//			/*
//			ScilabGetMatrix(tmat, "rightkeisuoWwnext");
//			out1 << "rightkeisuoWwnext" << std::endl;
//			for(unsigned i=0; i<tmat.height(); i++){
//				for(unsigned j=0; j<tmat.width(); j++){
//					out1 << tmat[i][j] << ",";
//				}
//				out1 << std::endl;
//			}
//			out1 << std::endl;
//		*/
//#endif
//
//#ifndef shrink
//			ScilabJob("leftkeisuw = 1/2*matk1w + 1/dt*matcw");
//			ScilabJob("rightkeisuwWw = -1/2*matk1w + 1/dt*matcw");
//			ScilabJob("rightkeisuwWo = 0");
//			ScilabJob("rightkeisuwFw = 1");
//			ScilabJob("rightkeisuwFo = 0");
//			ScilabJob("leftkeisuo = 1");
//			ScilabJob("rightkeisuoWw = 0");
//			ScilabJob("rightkeisuoWo = 1");
//			ScilabJob("rightkeisuoFw = 0");
//			ScilabJob("rightkeisuoFo = inv(1/dt*matco)");
//			ScilabJob("rightkeisuoWwnext = 0");
//#endif
//
//		}
//		//��=1�̂Ƃ�
//		else{
//			leftKeisuW = matK2WAll.inv()*(matK1WAll+1/dt*matCWAll)-(matK2OAll+1/dt*matCOAll).inv()*matK1OAll;
//			rightKeisuWWw = 1/dt*matK2WAll.inv()*matCWAll;
//			rightKeisuWWo = -1/dt*(matK2OAll+1/dt*matCOAll).inv()*matCOAll;
//			rightKeisuWFw = matK2WAll.inv();
//			rightKeisuWFo = - (matK2OAll+1/dt*matCOAll).inv();
//			leftKeisuO = matK1OAll.inv()*(matK2OAll+1/dt*matCOAll)-(matK1WAll+1/dt*matCWAll).inv()*matK2WAll;
//			rightKeisuWWw = -1/dt*(matK1WAll+1/dt*matCWAll).inv()*matCWAll;
//			rightKeisuWWo = 1/dt*matK1OAll.inv()*matCOAll;
//			rightKeisuWFw = -(matK1WAll+1/dt*matCWAll).inv();
//			rightKeisuWFo = matK1OAll.inv();
//		}
//		ScilabJob("keisuwInv = inv(leftkeisuw)");
//		ScilabJob("keisuoInv = inv(leftkeisuo)");
//	}
//	keisuChanged = false;
//
//	ScilabJob("wwvecall = keisuwInv * (rightkeisuwWw*wwveccp + rightkeisuwWo*woveccp + rightkeisuwFw*vecfw + rightkeisuwFo*vecfo)");
//	ScilabJob("wovecall = keisuoInv * (rightkeisuoWw*wwveccp + rightkeisuoWo*woveccp + rightkeisuoFw*vecfw + rightkeisuoFo*vecfo + rightkeisuoWwnext*wwvecall)");
//
//	ScilabGetMatrix(WwVecAll, "wwvecall");
//	ScilabGetMatrix(WoVecAll, "wovecall");
//	/*for(unsigned vtxid=0; vtxid < mesh->vertices.size(); vtxid++){
//		if(WwVecAll[vtxid][0] < 0.0){
//			WwVecAll[vtxid][0] = 0.0;
//		}
//		if(WoVecAll[vtxid][0] < 0.0){
//			WoVecAll[vtxid][0] = 0.0;
//		}
//	}
//	*/
//}

void PHFemPorousWOMove::SetRhoWAllToRhoWVecAll(){
	unsigned vtxSize = (unsigned)GetPHFemMesh()->vertices.size();
	for(unsigned i=0; i < vtxSize; i++){
		rhoWVecAll[i][0] = vertexVars[i].rhoW;
	}
}

void PHFemPorousWOMove::SetRhoOAllToRhoOVecAll(){
	unsigned vtxSize = (unsigned)GetPHFemMesh()->vertices.size();
	for(unsigned i=0; i < vtxSize; i++){
		rhoOVecAll[i][0] = vertexVars[i].rhoO;
	}
}

void PHFemPorousWOMove::UpdateVertexRhoWAll(){
	for(unsigned vtxid=0; vtxid < GetPHFemMesh()->vertices.size(); vtxid++){
		vertexVars[vtxid].rhoW = rhoWVecAll[vtxid][0];
	}
}

void PHFemPorousWOMove::UpdateVertexRhoOAll(){
	for(unsigned vtxid=0; vtxid < GetPHFemMesh()->vertices.size(); vtxid++){
		vertexVars[vtxid].rhoO = rhoOVecAll[vtxid][0];
	}
}

void PHFemPorousWOMove::InitAllVertexRhoW(){
	for(unsigned i=0; i < GetPHFemMesh()->vertices.size(); i++){
		vertexVars[i].rhoW = 0.0;
	}
}

void PHFemPorousWOMove::InitAllVertexRhoO(){
	for(unsigned i=0; i < GetPHFemMesh()->vertices.size(); i++){
		vertexVars[i].rhoO = 0.0;
	}
}

void PHFemPorousWOMove::SetVertexTemp(unsigned vtxid, double temp){
	vertexVars[vtxid].T = temp;
}

void PHFemPorousWOMove::SetVertexTc(unsigned vtxid, double tc){
	vertexVars[vtxid].Tc = tc;
}

void PHFemPorousWOMove::SetVertexRhoW(unsigned vtxid, double rhow){
	vertexVars[vtxid].rhoW = rhow;
}

void PHFemPorousWOMove::SetVertexRhoWInit(unsigned vtxid, double rhowInit){
	vertexVars[vtxid].rhoWInit = rhowInit;
}

void PHFemPorousWOMove::SetVertexRhoO(unsigned vtxid, double rhoo){
	vertexVars[vtxid].rhoO = rhoo;
}

void PHFemPorousWOMove::SetVertexRhoOInit(unsigned vtxid, double rhooInit){
	vertexVars[vtxid].rhoOInit = rhooInit;
}

void PHFemPorousWOMove::SetVertexMw(unsigned vtxid, double mw){
	vertexVars[vtxid].mw = mw;
}

void PHFemPorousWOMove::SetVertexMo(unsigned vtxid, double mo){
	vertexVars[vtxid].mo = mo;
}

void PHFemPorousWOMove::CalcVertexWOMu(unsigned vtxid){
	PHFemMeshNew* mesh = GetPHFemMesh();
	double vtxTemp;
	if(mesh->femThermo){
		vtxTemp = mesh->GetPHFemThermo()->GetVertexTemp(vtxid);
	}else{
		vtxTemp = vertexVars[vtxid].T;
	}
	vtxTemp += 273.15;
	vertexVars[vtxid].muW = 0.01257187 * exp((1-0.005806436*vtxTemp)/(0.001130911*vtxTemp-0.000005723952*vtxTemp*vtxTemp)) * 0.001;
	vertexVars[vtxid].muO = 0.0000021 * exp(3062.7792 / vtxTemp);
}

void PHFemPorousWOMove::CalcVertexVolume(unsigned vtxid){
	PHFemMeshNew* mesh = GetPHFemMesh();
	vertexVars[vtxid].vVolume = 0;
	for(unsigned tetid=0; tetid < mesh->vertices[vtxid].tetIDs.size(); tetid++){
		vertexVars[vtxid].vVolume += tetVars[mesh->vertices[vtxid].tetIDs[tetid]].volume / 4;
	}
}

void PHFemPorousWOMove::CalcVertexMwo(unsigned vtxid){
	vertexVars[vtxid].mw = vertexVars[vtxid].rhoW * vertexVars[vtxid].vVolume;
	vertexVars[vtxid].mo = vertexVars[vtxid].rhoO * vertexVars[vtxid].vVolume;
}

void PHFemPorousWOMove::CalcVertexSaturation(unsigned vtxid){
	vertexVars[vtxid].saturation = (vertexVars[vtxid].rhoW / rhoWater / vertexVars[vtxid].porosity) + (vertexVars[vtxid].rhoO / rhoOil / vertexVars[vtxid].porosity);
}

void PHFemPorousWOMove::CalcVertexPc(unsigned vtxid){
	double saturation = vertexVars[vtxid].saturation;

#ifdef saturationLimit
	if(saturation > 1.0){
		saturation = 1.0;
	}
#endif

	vertexVars[vtxid].Pc = kc * pow(1-saturation, alpha);
}

void PHFemPorousWOMove::CalcRhoWVecFromVertexMw(unsigned vtxid){
	rhoWVecAll[vtxid][0] = vertexVars[vtxid].mw / vertexVars[vtxid].vVolume;
}

double PHFemPorousWOMove::GetVertexTemp(unsigned vtxid){
	return vertexVars[vtxid].T;
}

double PHFemPorousWOMove::GetVertexTc(unsigned vtxid){
	return vertexVars[vtxid].Tc;
}

double PHFemPorousWOMove::GetVertexRhoW(unsigned vtxid){
	return vertexVars[vtxid].rhoW;
}

double PHFemPorousWOMove::GetVertexRhoWInit(unsigned vtxid){
	return vertexVars[vtxid].rhoWInit;
}

double PHFemPorousWOMove::GetVertexRhoO(unsigned vtxid){
	return vertexVars[vtxid].rhoO;
}

double PHFemPorousWOMove::GetVertexRhoOInit(unsigned vtxid){
	return vertexVars[vtxid].rhoOInit;
}

double PHFemPorousWOMove::GetVertexMw(unsigned vtxid){
	return vertexVars[vtxid].mw;
}

double PHFemPorousWOMove::GetVertexMo(unsigned vtxid){
	return vertexVars[vtxid].mo;
}

void PHFemPorousWOMove::CalcFaceArea(unsigned faceid){
	PHFemMeshNew* mesh = GetPHFemMesh();
	faceVars[faceid].area = CalcTriangleArea(mesh->faces[faceid].vertexIDs[0], mesh->faces[faceid].vertexIDs[1], mesh->faces[faceid].vertexIDs[2]);
}

void PHFemPorousWOMove::SetFaceSurroundFlux(unsigned faceid, double surroundFlux){
	faceVars[faceid].surroundFlux = surroundFlux;
}

void PHFemPorousWOMove::SetFaceVaporPress(unsigned faceid, double vaporPress){
	faceVars[faceid].vaporPress = vaporPress;
}

void PHFemPorousWOMove::CalcFaceMaxVaporPress(unsigned faceid){
	PHFemMeshNew* mesh = GetPHFemMesh();
	double faceTemp;
	if(mesh->GetPHFemThermo()){
		faceTemp = (mesh->GetPHFemThermo()->GetVertexTemp(mesh->faces[faceid].vertexIDs[0]) + mesh->GetPHFemThermo()->GetVertexTemp(mesh->faces[faceid].vertexIDs[1]) + mesh->GetPHFemThermo()->GetVertexTemp(mesh->faces[faceid].vertexIDs[2])) / 3;
	}else{
		faceTemp = (vertexVars[mesh->faces[faceid].vertexIDs[0]].T + vertexVars[mesh->faces[faceid].vertexIDs[1]].T + vertexVars[mesh->faces[faceid].vertexIDs[2]].T) / 3;
	}
	faceVars[faceid].maxVaporPress = 6.1078 * pow(10.0, 7.5*faceTemp/(faceTemp+237.3));
}

void PHFemPorousWOMove::CalcFaceMaxVaporCont(unsigned faceid){
	PHFemMeshNew* mesh = GetPHFemMesh();
	double faceTemp;
	if(mesh->GetPHFemThermo()){
		faceTemp = (mesh->GetPHFemThermo()->GetVertexTemp(mesh->faces[faceid].vertexIDs[0]) + mesh->GetPHFemThermo()->GetVertexTemp(mesh->faces[faceid].vertexIDs[1]) + mesh->GetPHFemThermo()->GetVertexTemp(mesh->faces[faceid].vertexIDs[2])) / 3;
	}else{
		faceTemp = (vertexVars[mesh->faces[faceid].vertexIDs[0]].T + vertexVars[mesh->faces[faceid].vertexIDs[1]].T + vertexVars[mesh->faces[faceid].vertexIDs[2]].T) / 3;
	}
	faceVars[faceid].maxVaporCont = 217 * 6.1078 * pow(10.0, 7.5*faceTemp/(faceTemp+237.3)) / (faceTemp + 273.15);
}

double PHFemPorousWOMove::GetFaceArea(unsigned faceid){
	return faceVars[faceid].area;
}

double PHFemPorousWOMove::GetFaceSurroundFlux(unsigned faceid){
	return faceVars[faceid].surroundFlux;
}

double PHFemPorousWOMove::GetFaceVaporPress(unsigned faceid){
	return faceVars[faceid].vaporPress;
}

double PHFemPorousWOMove::GetFaceMaxVaporPress(unsigned faceid){
	return faceVars[faceid].maxVaporPress;
}

void PHFemPorousWOMove::CalcTetVolume(unsigned tetid){
	tetVars[tetid].volume = CalcTetrahedraVolume(GetPHFemMesh()->tets[tetid]);
}

void PHFemPorousWOMove::CalcTetPorosity(unsigned tetid){
	PHFemMeshNew* mesh = GetPHFemMesh();
	tetVars[tetid].tetPorosity = (vertexVars[mesh->tets[tetid].vertexIDs[0]].porosity + vertexVars[mesh->tets[tetid].vertexIDs[1]].porosity + vertexVars[mesh->tets[tetid].vertexIDs[2]].porosity + vertexVars[mesh->tets[tetid].vertexIDs[3]].porosity) / 4;
}

void PHFemPorousWOMove::CalcTetWOMu(unsigned tetid){
	PHFemMeshNew* mesh = GetPHFemMesh();
	tetVars[tetid].tetMuW = (vertexVars[mesh->tets[tetid].vertexIDs[0]].muW + vertexVars[mesh->tets[tetid].vertexIDs[1]].muW + vertexVars[mesh->tets[tetid].vertexIDs[2]].muW + vertexVars[mesh->tets[tetid].vertexIDs[3]].muW) / 4;
	tetVars[tetid].tetMuO = (vertexVars[mesh->tets[tetid].vertexIDs[0]].muO + vertexVars[mesh->tets[tetid].vertexIDs[1]].muO + vertexVars[mesh->tets[tetid].vertexIDs[2]].muO + vertexVars[mesh->tets[tetid].vertexIDs[3]].muO) / 4;
}

void PHFemPorousWOMove::CalcTetRhoS(unsigned tetid){
	PHFemMeshNew* mesh = GetPHFemMesh();
	tetVars[tetid].rhoS = (vertexVars[mesh->tets[tetid].vertexIDs[0]].rhoS + vertexVars[mesh->tets[tetid].vertexIDs[1]].rhoS + vertexVars[mesh->tets[tetid].vertexIDs[2]].rhoS + vertexVars[mesh->tets[tetid].vertexIDs[3]].rhoS) / 4;
}

void PHFemPorousWOMove::SetTetSolidDensity(unsigned tetid, double rhos){
	tetVars[tetid].rhoS = rhos;
}

double PHFemPorousWOMove::GetTetVolume(unsigned tetid){
	return tetVars[tetid].volume;
}

double PHFemPorousWOMove::GetTetSolidDensity(unsigned tetid){
	return tetVars[tetid].rhoS;
}

PTM::VMatrixRow<double> PHFemPorousWOMove::inv(PTM::VMatrixRow<double> mat){
	PTM::VMatrixRow<double> result;

	result.resize(mat.height(),mat.width());
	result.clear();

	int m = (int)mat.height();//�s��
	int n = (int)mat.width();//��
	int lda = n;
	double *A = new double[m*n];
	int info;
	int *ipiv = new int[std::min(m,n)];
	int lwork = m;
	double *work = new double[m];
	
	for(int i=0; i < m; i++){
		for(int j=0; j < n; j++){
			A[j*m+i] = mat[i][j];
		}
	}

	dgetrf_(&n, &m, A, &lda, ipiv, &info);
	dgetri_(&m, A, &lda, ipiv, work, &lwork, &info);

	for(int i=0; i < m; i++){
		for(int j=0; j < n; j++){
			result[i][j] = A[j*m+i];
		}
	}

	delete[] A;
	delete[] ipiv;
	delete[] work;

	return result;
}

PTM::VMatrixRow<double> PHFemPorousWOMove::inv2(PTM::VMatrixRow<double> mat){

	PTM::VMatrixRow<double> matInv;

#ifndef Scilab
	if(!ScilabStart()) DSTR << "Error : ScilabStart \n" << std::endl;
#endif

	ScilabSetMatrix("mat", mat);
	ScilabJob("matInv = inv(mat);");
	ScilabGetMatrix(matInv, "matInv");

#ifndef Scilab
	ScilabEnd();
#endif

	return matInv;
}

double PHFemPorousWOMove::GetVtxWaterInTets(Vec3d temppos){
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
			return CalcWaterInnerTets( id , coeffk);		
		}
		coeffk.clear();
	}
	return DBL_MAX;		//	������Ȃ������T�C��
}

double PHFemPorousWOMove::GetVtxOilInTets(Vec3d temppos){
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
			return CalcOilInnerTets( id , coeffk);		
		}
		coeffk.clear();
	}
	return DBL_MAX;		//	������Ȃ������T�C��
}

double PHFemPorousWOMove::CalcWaterInnerTets(unsigned id,PTM::TVector<4,double> N){
	PHFemMeshNew* mesh = GetPHFemMesh();

	double temp = 0.0;
	for(unsigned i=0;i<4;i++){
		 temp += N[i] * vertexVars[mesh->tets[id].vertexIDs[i]].rhoW;
	}
	return temp;
};

double PHFemPorousWOMove::CalcOilInnerTets(unsigned id,PTM::TVector<4,double> N){
	PHFemMeshNew* mesh = GetPHFemMesh();

	double temp = 0.0;
	for(unsigned i=0;i<4;i++){
		 temp += N[i] * vertexVars[mesh->tets[id].vertexIDs[i]].rhoO;
	}
	return temp;
};

float PHFemPorousWOMove::calcGvtx(std::string fwfood, int pv, unsigned texture_mode){
	float gvtx;
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

	double dtex =(double) 1.0 / ( kogetex + thtex + watex + thcamtex + watex2);		//	�e�N�X�`�����s���W�̑w�Ԋu
	double texstart = dtex /2.0;										//	�e�N�X�`�����W�̏����l = �ł��e�N�X�`���̃X�^�[�g���W
	double wastart = texstart + kogetex * dtex;							//	�����ʕ\���e�N�X�`���̃X�^�[�g���W
	double thstart = texstart + kogetex * dtex + 1.0 * dtex;			//	�T�[���̃e�N�X�`���̃X�^�[�g���W �����e�N�X�`����2���ڂ���X�^�[�g
	double thcamstart = texstart + (thtex + kogetex + watex) * dtex;	//	
	double wastart2 = texstart + (thtex + kogetex + watex + thcamtex) * dtex;

	if(texture_mode == 5){
		double remainWaterPercentage;
		if(vertexVars[pv].rhoWInit == 0){
			remainWaterPercentage = 0;
		}else{
			remainWaterPercentage = vertexVars[pv].rhoW / vertexVars[pv].rhoWInit * 100;
		}
		// -50.0~0.0:aqua to blue
		if(/*0 <= remainWaterPercentage &&*/ remainWaterPercentage <= 25.0){
			gvtx = wastart2 + remainWaterPercentage * dtex / 25.0 - dtex;
		}
		else if(25.0 < remainWaterPercentage && remainWaterPercentage <= 50.0){	
			gvtx = wastart2 + remainWaterPercentage * dtex / 25.0 - dtex;
		}
		//	0~50.0:blue to green
		else if(50.0 < remainWaterPercentage && remainWaterPercentage <= 75.0 ){
			gvtx = wastart2 + remainWaterPercentage * dtex / 25.0 - dtex;
		}
		//	50.0~100.0:green to yellow
		else if(75.0 < remainWaterPercentage /*&& remainWaterPercentage <= 100.0*/){
			gvtx = wastart2 + remainWaterPercentage * dtex / 25.0 - dtex;
		}
		else{
			DSTR << "vertexVars[" << pv << "].temp = " << vertexVars[pv].rhoW / vertexVars[pv].rhoWInit * 100 << std::endl;
		}
	}
	return gvtx;
};

void PHFemPorousWOMove::matWwOut(){
	PHFemMeshNew* mesh = GetPHFemMesh();

	std::ofstream wwOut;
	wwOut.open("matWw.csv",std::ios::app);
	wwOut << COUNT * tdt << std::endl;
	for(unsigned i=0; i < mesh->vertices.size(); i++){
		for(unsigned j=0; j < mesh->vertices.size(); j++){
			wwOut << matWwAll[i][j] << ",";
		}
		wwOut << std::endl;
	}
	wwOut << std::endl;
	wwOut.close();
};

void PHFemPorousWOMove::matWoOut(){
	PHFemMeshNew* mesh = GetPHFemMesh();

	std::ofstream woOut;
	woOut.open("matWo.csv",std::ios::app);
	woOut << COUNT * tdt << std::endl;
	for(unsigned i=0; i < mesh->vertices.size(); i++){
		for(unsigned j=0; j < mesh->vertices.size(); j++){
			woOut << matWoAll[i][j] << ",";
		}
		woOut << std::endl;
	}
	woOut << std::endl;
	woOut.close();
};

void PHFemPorousWOMove::matOwOut(){
	PHFemMeshNew* mesh = GetPHFemMesh();

	std::ofstream owOut;
	owOut.open("matOw.csv",std::ios::app);
	owOut << COUNT * tdt << std::endl;
	for(unsigned i=0; i < mesh->vertices.size(); i++){
		for(unsigned j=0; j < mesh->vertices.size(); j++){
			owOut << matOwAll[i][j] << ",";
		}
		owOut << std::endl;
	}
	owOut << std::endl;
	owOut.close();
};

void PHFemPorousWOMove::matOoOut(){
	PHFemMeshNew* mesh = GetPHFemMesh();

	std::ofstream ooOut;
	ooOut.open("matOo.csv",std::ios::app);
	ooOut << COUNT * tdt << std::endl;
	for(unsigned i=0; i < mesh->vertices.size(); i++){
		for(unsigned j=0; j < mesh->vertices.size(); j++){
			ooOut << matOoAll[i][j] << ",";
		}
		ooOut << std::endl;
	}
	ooOut << std::endl;
	ooOut.close();
};

void PHFemPorousWOMove::matPcwOut(){
	PHFemMeshNew* mesh = GetPHFemMesh();

	std::ofstream pcwOut;
	pcwOut.open("matPcw.csv",std::ios::app);
	pcwOut << COUNT * tdt << std::endl;
	for(unsigned i=0; i < mesh->vertices.size(); i++){
		for(unsigned j=0; j < mesh->vertices.size(); j++){
			pcwOut << matPcwAll[i][j] << ",";
		}
		pcwOut << std::endl;
	}
	pcwOut << std::endl;
	pcwOut.close();
};

void PHFemPorousWOMove::matPcoOut(){
	PHFemMeshNew* mesh = GetPHFemMesh();

	std::ofstream pcoOut;
	pcoOut.open("matPco.csv",std::ios::app);
	pcoOut << COUNT * tdt << std::endl;
	for(unsigned i=0; i < mesh->vertices.size(); i++){
		for(unsigned j=0; j < mesh->vertices.size(); j++){
			pcoOut << matPcoAll[i][j] << ",";
		}
		pcoOut << std::endl;
	}
	pcoOut << std::endl;
	pcoOut.close();
};

void PHFemPorousWOMove::matCOut(){
	PHFemMeshNew* mesh = GetPHFemMesh();

	std::ofstream cOut;
	cOut.open("matCpwom.csv",std::ios::app);
	cOut << COUNT * tdt << std::endl;
	for(unsigned i=0; i < mesh->vertices.size(); i++){
		for(unsigned j=0; j < mesh->vertices.size(); j++){
			cOut << matCAll[i][j] << ",";
		}
		cOut << std::endl;
	}
	cOut << std::endl;
	cOut.close();
};

void PHFemPorousWOMove::vecFwAllOut(){
	PHFemMeshNew* mesh = GetPHFemMesh();

	std::ofstream vecFwOut;
	vecFwOut.open("vecFwAll.csv",std::ios::app);
	vecFwOut << COUNT * tdt << std::endl;
	for(unsigned i=0; i < mesh->vertices.size(); i++){
		vecFwOut << vecFwAll[i][0] << ",";
	}
	vecFwOut << std::endl;
	vecFwOut.close();

};

void PHFemPorousWOMove::vecFoAllOut(){
	PHFemMeshNew* mesh = GetPHFemMesh();

	std::ofstream vecFoOut;
	vecFoOut.open("vecFoAll.csv",std::ios::app);
	vecFoOut << COUNT * tdt << std::endl;
	for(unsigned i=0; i < mesh->vertices.size(); i++){
		vecFoOut << vecFoAll[i][0] << ",";
	}
	vecFoOut << std::endl;
	vecFoOut.close();
};

void PHFemPorousWOMove::vecFwFinalOut(){
	PHFemMeshNew* mesh = GetPHFemMesh();

	std::ofstream vecFwOut;
	vecFwOut.open("vecFwFinal.csv",std::ios::app);
	vecFwOut << COUNT * tdt << std::endl;
	for(unsigned i=0; i < mesh->vertices.size(); i++){
		vecFwOut << vecFwFinal[i][0] << ",";
	}
	vecFwOut << std::endl;
	vecFwOut.close();

};

void PHFemPorousWOMove::vecFoFinalOut(){
	PHFemMeshNew* mesh = GetPHFemMesh();

	std::ofstream vecFoOut;
	vecFoOut.open("vecFoFinal.csv",std::ios::app);
	vecFoOut << COUNT * tdt << std::endl;
	for(unsigned i=0; i < mesh->vertices.size(); i++){
		vecFoOut << vecFoFinal[i][0] << ",";
	}
	vecFoOut << std::endl;
	vecFoOut.close();
};

void PHFemPorousWOMove::vecPcAllOut(){
	PHFemMeshNew* mesh = GetPHFemMesh();

	std::ofstream vecPcAllOut;
	vecPcAllOut.open("vecPcAll.csv",std::ios::app);
	vecPcAllOut << COUNT * tdt << std::endl;
	for(unsigned i=0; i < mesh->vertices.size(); i++){
		vecPcAllOut << PcVecAll[i][0] << ",";
	}
	vecPcAllOut << std::endl;
	vecPcAllOut.close();
};

void PHFemPorousWOMove::decrhoW(double dec){
	PHFemMeshNew* mesh = GetPHFemMesh();

	for(unsigned i=0; i < mesh->vertices.size() / 2; i++){
		vertexVars[2*i+1].rhoW -= dec;
	}
};

void PHFemPorousWOMove::vertexDenatProcess(unsigned vtxid){
	vertexVars[vtxid].porosity *= shrinkageRatio;
	vertexVars[vtxid].rhoS /= shrinkageRatio;
};

void PHFemPorousWOMove::outflowOverSaturation(unsigned vtxid){
	double saturation = rhoWVecAll[vtxid][0]/rhoWater/vertexVars[vtxid].porosity + rhoOVecAll[vtxid][0]/rhoOil/vertexVars[vtxid].porosity;
	if(saturation > 1.0){
		double decRhoW = (rhoOil*rhoWVecAll[vtxid][0] + rhoWater*rhoOVecAll[vtxid][0] - vertexVars[vtxid].saturation*rhoWater*rhoOil) / (rhoOil + vertexVars[vtxid].muW/vertexVars[vtxid].muO*rhoWater);
		double decRhoO = vertexVars[vtxid].muW / vertexVars[vtxid].muO * decRhoW;
		//if(rhoWVecAll[vtxid][0] > decRhoW){
		//	rhoWVecAll[vtxid][0] -= decRhoW;
		//}else{
		//	decRhoO += (decRhoW - rhoWVecAll[vtxid][0]) / rhoWater * rhoOil;
		//	rhoWVecAll[vtxid][0] = 0;
		//}
		//rhoOVecAll[vtxid][0] -= decRhoO;
		//vertexVars[vtxid].outflowWater += decRhoW * vertexVars[vtxid].vVolume;
		//vertexVars[vtxid].outflowOil += decRhoO * vertexVars[vtxid].vVolume;
		vertexVars[vtxid].outflowWater = decRhoW * vertexVars[vtxid].vVolume;
		vertexVars[vtxid].outflowOil = decRhoO * vertexVars[vtxid].vVolume;
		//double vSaturation = rhoWVecAll[vtxid][0]/rhoWater/vertexVars[vtxid].porosity + rhoOVecAll[vtxid][0]/rhoOil/vertexVars[vtxid].porosity;
		//vertexVars[vtxid].saturation = vSaturation;
	}else{
		vertexVars[vtxid].outflowWater = 0.0;
		vertexVars[vtxid].outflowOil = 0.0;
	}
};

double PHFemPorousWOMove::decideWetValue(unsigned vtxid){
	PHFemMeshNew* mesh = GetPHFemMesh();
	
	double outflowLiquid = vertexVars[vtxid].outflowOil + vertexVars[vtxid].outflowWater;
	
	return outflowLiquid;
}

}
