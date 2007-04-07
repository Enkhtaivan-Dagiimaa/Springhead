/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include "robot2.h"
#include <vector>

using namespace std;

void Robot2::Leg::Build(PHSolidIf* body, PHRootNodeIf* root, const Posed& base, PHSceneIf* scene, PHSdkIf* sdk){
	CDBoxDesc bd;
	bd.boxsize = Vec3f(0.1, 0.4, 0.1);
	boxCrank = DCAST(CDBoxIf, sdk->CreateShape(bd));
	bd.boxsize = Vec3f(0.1, 1.5, 0.1);
	boxFoot = DCAST(CDBoxIf, sdk->CreateShape(bd));
	bd.boxsize = Vec3f(0.1, 0.8, 0.1);
	boxGuide = DCAST(CDBoxIf, sdk->CreateShape(bd));

	PHSolidDesc sd;
	sd.mass = 0.1;
	sd.inertia = Matrix3d::Unit() * 0.1;
	soCrank = scene->CreateSolid(sd);
	soCrank->AddShape(boxCrank);
	soFoot[0] = scene->CreateSolid(sd);
	soFoot[0]->AddShape(boxFoot);
	soFoot[1] = scene->CreateSolid(sd);
	soFoot[1]->AddShape(boxFoot);
	soGuide[0] = scene->CreateSolid(sd);
	soGuide[0]->AddShape(boxGuide);
	soGuide[1] = scene->CreateSolid(sd);
	soGuide[1]->AddShape(boxGuide);

	// �N�����N
	PHHingeJointDesc jd;
	jd.poseSocket = base;
	jd.posePlug.Pos() = Vec3d(0.0, 0.0, 0.0);
	jntCrank = DCAST(PHHingeJointIf, scene->CreateJoint(body, soCrank, jd));
	jntCrank->SetDamper(1.0);
	scene->CreateTreeNode(root, soCrank);
	
	const double K = 100.0, D = 10.0;
	
	Posed pose;
	PHTreeNodeIf* node;
	for(int i = 0; i < 2; i++){
		pose.Pos() = Vec3d(0.9, 0.4, (i == 0 ? 0.06 : -0.06));
		jd.poseSocket = base * pose;
		jd.posePlug.Pos() = Vec3d(0.0, 0.4, (i == 0 ? -0.06 : 0.06));
		jntGuideBody[i] = DCAST(PHHingeJointIf, scene->CreateJoint(body, soGuide[i], jd));
		jntGuideBody[i]->SetDamper(D);
		jntGuideBody[i]->SetSpring(K);
		jntGuideBody[i]->SetSpringOrigin(Rad(-90.0));
		node = scene->CreateTreeNode(root, soGuide[i]);
			
		jd.poseSocket.Ori() = Quaterniond();
		jd.poseSocket.Pos() = Vec3d(0.0, -0.4, 0.0);
		jd.posePlug.Pos() = Vec3d(0.0, 0.7, 0.0);
		jntFootGuide[i] = DCAST(PHHingeJointIf, scene->CreateJoint(soGuide[i], soFoot[i], jd));
		scene->CreateTreeNode(node, soFoot[i]);
		jntFootGuide[i]->SetSpring(K);
		jntFootGuide[i]->SetDamper(D);
		jntFootGuide[i]->SetSpringOrigin(Rad(90.0));
	}
	
	// ���΂��҂�
	double dt = scene->GetTimeStep();
	double T = 1.0;
	for(double t = 0.0; t < T; t+=dt)
		scene->Step();

	// �o�l����
	jntGuideBody[0]->SetSpring(0.0);
	jntGuideBody[1]->SetSpring(0.0);
	jntFootGuide[0]->SetSpring(0.0);
	jntFootGuide[1]->SetSpring(0.0);

	// �����N�̍\��
	for(int i = 0; i < 2; i++){
		jd.poseSocket = Posed();
		jd.poseSocket.Pos() = Vec3d(0.0, (i == 0 ? 0.1 : -0.1), (i == 0 ? 0.06: -0.06));
		jd.posePlug.Pos() = Vec3d(0.0, -0.1+0.25, (i == 0 ? -0.06 : 0.06));
		jntFoot[i] = DCAST(PHHingeJointIf, scene->CreateJoint(soCrank, soFoot[i], jd));
	}
	
	scene->SetContactMode(soCrank, soFoot[0], PHSceneDesc::MODE_NONE);
	scene->SetContactMode(soCrank, soFoot[1], PHSceneDesc::MODE_NONE);
	scene->SetContactMode(soCrank, soGuide[0], PHSceneDesc::MODE_NONE);
	scene->SetContactMode(soCrank, soGuide[1], PHSceneDesc::MODE_NONE);
	scene->SetContactMode(soGuide[0], soFoot[0], PHSceneDesc::MODE_NONE);
	scene->SetContactMode(soGuide[1], soFoot[1], PHSceneDesc::MODE_NONE);	
}

void Robot2::Build(const Posed& pose, PHSceneIf* scene, PHSdkIf* sdk){
	CDBoxDesc bd;
	bd.boxsize = Vec3f(0.8, 0.6, 1.0);
	boxBody = DCAST(CDBoxIf, sdk->CreateShape(bd));
	
	PHSolidDesc sd;
	soBody = scene->CreateSolid(sd);
	soBody->AddShape(boxBody);
	soBody->SetPose(pose);
	soBody->SetDynamical(false);
	PHRootNodeIf* root = scene->CreateRootNode(soBody);

	Posed poseLeg;
	poseLeg.Pos() = Vec3d(-1.3, -0.3, 1.0);
	leg[0].Build(soBody, root, poseLeg, scene, sdk);
	poseLeg.Pos() = Vec3d(-1.3, -0.3, -1.0);
	leg[2].Build(soBody, root, poseLeg, scene, sdk);
	
	poseLeg.Ori() = Quaterniond::Rot(Rad(180.0), 'y');
	
	poseLeg.Pos() = Vec3d(1.3, -0.3, 1.0);
	leg[1].Build(soBody, root, poseLeg, scene, sdk);
	poseLeg.Pos() = Vec3d(1.3, -0.3, -1.0);
	leg[3].Build(soBody, root, poseLeg, scene, sdk);

	vector<PHSolidIf*> group;
	group.push_back(soBody);
	for(int i = 0; i < 4; i++){
		group.push_back(leg[i].soCrank);
		group.push_back(leg[i].soFoot[0]);
		group.push_back(leg[i].soFoot[1]);
		group.push_back(leg[i].soGuide[0]);
		group.push_back(leg[i].soGuide[1]);
	}
	scene->SetContactMode(&group[0], group.size(), PHSceneDesc::MODE_NONE);

	soBody->SetDynamical(true);
}


const double speed = 3.0;
void Robot2::Stop(){
	leg[0].jntCrank->SetMotorTorque(0);
	leg[1].jntCrank->SetMotorTorque(0);
	leg[2].jntCrank->SetMotorTorque(0);
	leg[3].jntCrank->SetMotorTorque(0);
}

void Robot2::Forward(){
	leg[0].jntCrank->SetMotorTorque(speed);
	leg[1].jntCrank->SetMotorTorque(-speed);
	leg[2].jntCrank->SetMotorTorque(speed);
	leg[3].jntCrank->SetMotorTorque(-speed);
}

void Robot2::Backward(){
	leg[0].jntCrank->SetMotorTorque(-speed);
	leg[1].jntCrank->SetMotorTorque(speed);
	leg[2].jntCrank->SetMotorTorque(-speed);
	leg[3].jntCrank->SetMotorTorque(speed);
}

void Robot2::TurnLeft(){
	leg[0].jntCrank->SetMotorTorque(speed);
	leg[1].jntCrank->SetMotorTorque(-speed);
	leg[2].jntCrank->SetMotorTorque(-speed);
	leg[3].jntCrank->SetMotorTorque(speed);
}

void Robot2::TurnRight(){
	leg[0].jntCrank->SetMotorTorque(-speed);
	leg[1].jntCrank->SetMotorTorque(speed);
	leg[2].jntCrank->SetMotorTorque(speed);
	leg[3].jntCrank->SetMotorTorque(-speed);
}
