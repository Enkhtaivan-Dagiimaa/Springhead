/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include "robot.h"
#include <vector>

using namespace std;

void SetMass(PHSolidIf* solid, double m, const Vec3d& sz){
	solid->SetMass(m);
	solid->SetInertia(0.33 * m * Matrix3d::Diag(sz.y*sz.y + sz.z*sz.z, sz.x*sz.x + sz.z*sz.z, sz.x*sz.x + sz.y*sz.y));
}

//���[�g���W���[���̋r���\�z
void Robot::Leg::Build_root(PHSolidIf* body, PHRootNodeIf* root, const Posed& base, PHSceneIf* scene, PHSdkIf* sdk){

	//���i�̃T�C�Y���w�肷��
	CDBoxDesc bd;
	bd.boxsize = Vec3f(0.005, 0.12, 0.038);//ASHIGARU����A���~����
	boxFoot = sdk->CreateShape(bd)->Cast();
	bd.boxsize = Vec3f(0.026, 0.067, 0.032);//�_�C�i�~�N�Z���P
	boxDX1 = sdk->CreateShape(bd)->Cast();
	bd.boxsize = Vec3f(0.026, 0.067, 0.032);//�_�C�i�~�N�Z���Q
	boxDX2 = sdk->CreateShape(bd)->Cast();

	CDSphereDesc sp;						//����̋�
	sp.radius = 0.008f;
	Sphere = sdk->CreateShape(sp)->Cast();

	//���i�̃\���b�h���f�����쐬����
	PHSolidDesc sd;
	sd.mass = 0.04;//�P��Kg??
	sd.inertia = Matrix3d::Unit() * sd.mass * 0.1;//Unit()�͒P�ʍs��
	soFoot = scene->CreateSolid(sd);
	//SetMass(soFoot, 0.4, Vec3d(0.005, 0.12, 0.038));
	soFoot->AddShape(boxFoot);
	sd.mass = 0.09;
	sd.inertia = Matrix3d::Unit() * sd.mass * 0.1;
	soDX1 = scene->CreateSolid(sd);
	//SetMass(soDX1, 0.9, Vec3d(0.026, 0.067, 0.032));
	soDX1->AddShape(boxDX1);
	soDX2 = scene->CreateSolid(sd);
	soDX2->AddShape(boxDX2);
	sd.mass = 0.001;
	sd.inertia = Matrix3d::Unit() * sd.mass * 0.1;
	soSphere = scene->CreateSolid(sd);
	//SetMass(soSphere, 0.01, Vec3d(0.008, 0.008, 0.008));
	soSphere->AddShape(Sphere);

	PHHingeJointDesc jd;
	jd.poseSocket = base;
	jd.posePlug.Pos() = Vec3d(0.0, 0.0, 0.0);//����͉��̈Ӗ��H�H�ς��Ă������ω��������B
	
	const double K = 100.0, D = 15.0; //����Ŋ֐߃g���N�����܂�
	
	Posed pose;
	PHTreeNodeIf* node;

	//���i�t�����j�֐�
		pose.Ori() = Quaterniond::Rot(Rad(90.0), 'x');//���[���֐�
		pose.Pos() = Vec3d(0.09, 0.04, 0.0);//�r�̕t�����̈ʒu��ݒ�
		jd.poseSocket = base * pose;		//���̈ʒu���w��???
		jd.posePlug.Pos() = Vec3d(0.0, 0.04, 0.0);
		jntDX1 = scene->CreateJoint(body, soDX1, jd)->Cast();//�֐߂��쐬
		jntDX1->SetSpring(K);
		jntDX1->SetDamper(D);
		jntDX1->SetSpringOrigin(Rad(-90.0));
		node = scene->CreateTreeNode(root, soDX1);
		
	//���֐�
		jd.poseSocket.Ori() = Quaterniond::Rot(Rad(-90.0), 'y');//�s�b�`���֐�
		jd.poseSocket.Pos() = Vec3d(0.0, -0.033, 0.0);
		jd.posePlug.Pos() = Vec3d(0.0, 0.033, 0.0);
		jntDX2 = scene->CreateJoint(soDX1, soDX2, jd)->Cast();
		jntDX2->SetSpring(K);
		jntDX2->SetDamper(D);
		jntDX2->SetSpringOrigin(Rad(60.0));
		node = scene->CreateTreeNode(node, soDX2);

	//��O�֐߁i����A���~�{���j
		jd.poseSocket.Ori() = Quaterniond::Rot(Rad(0.0), 'y');
		jd.poseSocket.Pos() = Vec3d(0.0, -0.04, 0.0);
		jd.posePlug.Pos() = Vec3d(0.0, 0.05, 0.0);
		jntFoot = scene->CreateJoint(soDX2, soFoot, jd)->Cast();
		jntFoot->SetSpring(K);
		jntFoot->SetDamper(D);
		jntFoot->SetSpringOrigin(Rad(30.0));
		node = scene->CreateTreeNode(node, soFoot);

		jd.poseSocket.Ori() = Quaterniond();
		jd.poseSocket.Pos() = Vec3d(0.0, 0.0, 0.0);
		jd.posePlug.Pos() = Vec3d(0.0, 0.07, 0.0);
		jntSphere = scene->CreateJoint(soFoot, soSphere, jd)->Cast();
		node = scene->CreateTreeNode(node, soSphere);
		jntSphere->SetSpring(K);
		jntSphere->SetDamper(D);
		jntSphere->SetSpringOrigin(Rad(0.0));

	// ���΂����҂�����
	/*double dt = scene->GetTimeStep();
	double T = 0.3;
	for(double t = 0.0; t < T; t+=dt)
		scene->Step();*/

	soFoot->SetDynamical(true);

	scene->SetContactMode(soDX1, soDX2, PHSceneDesc::MODE_NONE);
	scene->SetContactMode(soDX2, soFoot, PHSceneDesc::MODE_NONE);
											
}

//�c���[���W���[���̋r
void Robot::Leg::Build_Tree(PHSolidIf* body, PHTreeNodeIf* node_body, const Posed& base, PHSceneIf* scene, PHSdkIf* sdk){

	//���i�̃T�C�Y���w�肷��
	CDBoxDesc bd;
	bd.boxsize = Vec3f(0.005, 0.12, 0.038);  boxFoot = sdk->CreateShape(bd)->Cast();//ASHIGARU����A���~����
	bd.boxsize = Vec3f(0.026, 0.067, 0.032); boxDX1 = sdk->CreateShape(bd)->Cast(); //�_�C�i�~�N�Z���P
	bd.boxsize = Vec3f(0.026, 0.067, 0.032); boxDX2 = sdk->CreateShape(bd)->Cast(); //�_�C�i�~�N�Z���Q

	CDSphereDesc sp; sp.radius = 0.008f; Sphere = sdk->CreateShape(sp)->Cast();//����̋�

	//���i�̃\���b�h���f�����쐬����
	PHSolidDesc sd;
	sd.mass = 0.04;//�P��Kg??
	sd.inertia = Matrix3d::Unit() * sd.mass * 0.1;//Unit()�͒P�ʍs��
	soFoot = scene->CreateSolid(sd);
	//SetMass(soFoot, 0.4, Vec3d(0.005, 0.12, 0.038));
	soFoot->AddShape(boxFoot);
	sd.mass = 0.09;
	sd.inertia = Matrix3d::Unit() * sd.mass * 0.1;
	soDX1 = scene->CreateSolid(sd);
	//SetMass(soDX1, 0.9, Vec3d(0.026, 0.067, 0.032));
	soDX1->AddShape(boxDX1);
	soDX2 = scene->CreateSolid(sd);
	soDX2->AddShape(boxDX2);
	sd.mass = 0.001;
	sd.inertia = Matrix3d::Unit() * sd.mass * 0.1;
	soSphere = scene->CreateSolid(sd);
	//SetMass(soSphere, 0.01, Vec3d(0.008, 0.008, 0.008));
	soSphere->AddShape(Sphere);

	PHHingeJointDesc jd;
	jd.poseSocket = base;
	jd.posePlug.Pos() = Vec3d(0.0, 0.0, 0.0);//����͉��̈Ӗ��H�H�ς��Ă������ω��������B
	
	const double K = 1000.0, D = 100.0;
	
	Posed pose;
	PHTreeNodeIf* node;

	//���i�t�����j�֐�
		pose.Ori() = Quaterniond::Rot(Rad(90.0), 'x');//���[���֐�
		pose.Pos() = Vec3d(0.09, 0.04, 0.0);//�r�̕t�����̈ʒu��ݒ�
		jd.poseSocket = base * pose;		//���̈ʒu���w��???
		jd.posePlug.Pos() = Vec3d(0.0, 0.04, 0.0);
		jntDX1 = scene->CreateJoint(body, soDX1, jd)->Cast();//�֐߂��쐬
		jntDX1->SetDamper(D);
		jntDX1->SetSpring(K);
		jntDX1->SetSpringOrigin(Rad(-90.0));
		node = scene->CreateTreeNode(node_body, soDX1);
		
	//���֐�
		jd.poseSocket.Ori() = Quaterniond::Rot(Rad(-90.0), 'y');//�s�b�`���֐�
		jd.poseSocket.Pos() = Vec3d(0.0, -0.033, 0.0);
		jd.posePlug.Pos() = Vec3d(0.0, 0.033, 0.0);
		jntDX2 = scene->CreateJoint(soDX1, soDX2, jd)->Cast();
		jntDX2->SetSpring(K);
		jntDX2->SetDamper(D);
		jntDX2->SetSpringOrigin(Rad(60.0));
		node = scene->CreateTreeNode(node, soDX2);

	//��O�֐߁i����A���~�{���j
		jd.poseSocket.Ori() = Quaterniond::Rot(Rad(0.0), 'y');
		jd.poseSocket.Pos() = Vec3d(0.0, -0.04, 0.0);
		jd.posePlug.Pos() = Vec3d(0.0, 0.05, 0.0);
		jntFoot = scene->CreateJoint(soDX2, soFoot, jd)->Cast();
		jntFoot->SetSpring(K);
		jntFoot->SetDamper(D);
		jntFoot->SetSpringOrigin(Rad(30.0));
		node = scene->CreateTreeNode(node, soFoot);

		jd.poseSocket.Ori() = Quaterniond();
		jd.poseSocket.Pos() = Vec3d(0.0, 0.0, 0.0);
		jd.posePlug.Pos() = Vec3d(0.0, 0.07, 0.0);
		jntSphere = scene->CreateJoint(soFoot, soSphere, jd)->Cast();
		node = scene->CreateTreeNode(node, soSphere);
		jntSphere->SetSpring(K);
		jntSphere->SetDamper(D);
		jntSphere->SetSpringOrigin(Rad(0.0));

	soFoot->SetDynamical(true);

	scene->SetContactMode(soDX1, soDX2, PHSceneDesc::MODE_NONE);
	scene->SetContactMode(soDX2, soFoot, PHSceneDesc::MODE_NONE);
											
}

//���[�g���W���[�����\�z
void Robot::Build_root(const Posed& pose, PHSceneIf* scene, PHSdkIf* sdk){

	CDConvexMeshDesc md;                                                    // ���b�V���f�B�X�N���v�^(md)
    md.vertices.push_back(Vec3f( 0.0,   0.02, 0.06));							// �Z�p���̒��_���W��o�^
    md.vertices.push_back(Vec3f( 0.052, 0.02, 0.03));	
    md.vertices.push_back(Vec3f( 0.052, 0.02,-0.03));	
    md.vertices.push_back(Vec3f( 0.0,   0.02,-0.06));
    md.vertices.push_back(Vec3f(-0.052, 0.02,-0.03));	
    md.vertices.push_back(Vec3f(-0.052, 0.02, 0.03));
    md.vertices.push_back(Vec3f( 0.0,  -0.02, 0.06));
    md.vertices.push_back(Vec3f( 0.052,-0.02, 0.03));	
    md.vertices.push_back(Vec3f( 0.052,-0.02,-0.03));	
    md.vertices.push_back(Vec3f( 0.0,  -0.02,-0.06));
    md.vertices.push_back(Vec3f(-0.052,-0.02,-0.03));	
    md.vertices.push_back(Vec3f(-0.052,-0.02, 0.03));

	boxBody = sdk->CreateShape(md)->Cast();
	
	PHSolidDesc sd;
	sd.mass = 0.1;
	sd.inertia *= 0.01;
	soBody = scene->CreateSolid(sd);
	//SetMass(soBody, 1, Vec3d(0.1, 0.04, 0.1));
	soBody->AddShape(boxBody);
	soBody->SetPose(pose);
	soBody->SetDynamical(false);
	PHRootNodeIf* root = scene->CreateRootNode(soBody);

	Posed poseLeg;
	poseLeg.Ori() = Quaterniond::Rot(Rad(60.0), 'y');
	poseLeg.Pos() = Vec3d(-0.075, -0.04, 0.13);//���_�ɑ΂���h�N�����N�i�A�N�`���G�[�^�j�́h�ʒu?
	leg[0].Build_root(soBody, root, poseLeg, scene, sdk);
	poseLeg.Ori() = Quaterniond::Rot(Rad(180.0), 'y');//�r�̐���������𔽓]
	poseLeg.Pos() = Vec3d(0.145, -0.04, 0.0);
	leg[1].Build_root(soBody, root, poseLeg, scene, sdk);
	poseLeg.Ori() = Quaterniond::Rot(Rad(-60.0), 'y');
	poseLeg.Pos() = Vec3d(-0.075, -0.04, -0.13);
	leg[2].Build_root(soBody, root, poseLeg, scene, sdk);
	
	vector<PHSolidIf*> group;
	group.push_back(soBody);
	for(int i = 0; i < 3; i++){
		group.push_back(leg[i].soDX1);
		group.push_back(leg[i].soDX2);
		group.push_back(leg[i].soFoot);
		group.push_back(leg[i].soSphere);
	}
	scene->SetContactMode(&group[0], group.size(), PHSceneDesc::MODE_NONE);
}

//�c���[���W���[�����\�z
void Robot::Build_Tree(PHSolidIf* pre_body, PHRootNodeIf* root_node, const Posed& pose, PHSceneIf* scene, PHSdkIf* sdk){

	CDConvexMeshDesc md;                                                    // ���b�V���f�B�X�N���v�^(md)
    md.vertices.push_back(Vec3f( 0.0,   0.02, 0.06));							// �Z�p���̒��_���W��o�^
    md.vertices.push_back(Vec3f( 0.052, 0.02, 0.03));	
    md.vertices.push_back(Vec3f( 0.052, 0.02,-0.03));	
    md.vertices.push_back(Vec3f( 0.0,   0.02,-0.06));
    md.vertices.push_back(Vec3f(-0.052, 0.02,-0.03));	
    md.vertices.push_back(Vec3f(-0.052, 0.02, 0.03));
    md.vertices.push_back(Vec3f( 0.0,  -0.02, 0.06));
    md.vertices.push_back(Vec3f( 0.052,-0.02, 0.03));	
    md.vertices.push_back(Vec3f( 0.052,-0.02,-0.03));	
    md.vertices.push_back(Vec3f( 0.0,  -0.02,-0.06));
    md.vertices.push_back(Vec3f(-0.052,-0.02,-0.03));	
    md.vertices.push_back(Vec3f(-0.052,-0.02, 0.03));

	boxBody = sdk->CreateShape(md)->Cast();
	
	PHSolidDesc sd;
	sd.mass = 0.1;
	sd.inertia *= 0.01;
	soBody = scene->CreateSolid(sd);
	//SetMass(soBody, 1, Vec3d(0.1, 0.04, 0.1));
	soBody->AddShape(boxBody);
	soBody->SetPose(pose);
	soBody->SetDynamical(false);

	PHHingeJointDesc jd;
	PHTreeNodeIf* node;

	jd.poseSocket.Ori() = Quaterniond();
	jd.poseSocket.Pos() = pose;
	jd.posePlug.Pos() = pose;
	jntConnect = scene->CreateJoint(pre_body, soBody, jd)->Cast();
	jntConnect->SetSpring(10000);
	jntConnect->SetDamper(100);
	jntConnect->SetSpringOrigin(Rad(0.0));
	//node = scene->CreateTreeNode(root_node, soBody);		�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@//�����Ŏ~�܂��Ă��܂�

	//Posed poseLeg;
	//poseLeg.Ori() = Quaterniond::Rot(Rad(60.0), 'y');
	//poseLeg.Pos() = Vec3d(-0.075, -0.04, 0.13);//���_�ɑ΂���A�N�`���G�[�^�̈ʒu
	//leg[0].Build_Tree(soBody, node, poseLeg, scene, sdk);
	//poseLeg.Ori() = Quaterniond::Rot(Rad(180.0), 'y');//�r�̐���������𔽓]
	/*poseLeg.Pos() = Vec3d(0.145, -0.04, 0.0);
	leg[1].Build_Tree(soBody, node, poseLeg, scene, sdk);
	poseLeg.Ori() = Quaterniond::Rot(Rad(-60.0), 'y');
	poseLeg.Pos() = Vec3d(-0.075, -0.04, -0.13);
	leg[2].Build_Tree(soBody, node, poseLeg, scene, sdk);
	
	vector<PHSolidIf*> group;
	group.push_back(soBody);
	for(int i = 0; i < 3; i++){
		group.push_back(leg[i].soDX1);
		group.push_back(leg[i].soDX2);
		group.push_back(leg[i].soFoot);
		group.push_back(leg[i].soSphere);
	}
	scene->SetContactMode(&group[0], group.size(), PHSceneDesc::MODE_NONE);*/
}

//const double speed = 5.0;

void Robot::Stop(){
	/*leg[0].jntDX1->SetMotorTorque(0);
	leg[1].jntDX1->SetMotorTorque(0);
	leg[2].jntDX1->SetMotorTorque(0);
	leg[0].jntDX2->SetMotorTorque(0);
	leg[1].jntDX2->SetMotorTorque(0);
	leg[2].jntDX2->SetMotorTorque(0);
	leg[0].jntFoot->SetMotorTorque(0);
	leg[1].jntFoot->SetMotorTorque(0);
	leg[2].jntFoot->SetMotorTorque(0);*/
}
void Robot::Forward(){

	//leg[0].jntDX2->SetSpringOrigin(Rad(80.0));
	leg[1].jntDX2->SetSpringOrigin(Rad(80.0));
	leg[2].jntDX2->SetSpringOrigin(Rad(80.0));
	//leg[0].jntFoot->SetSpringOrigin(Rad(10.0));
	leg[1].jntFoot->SetSpringOrigin(Rad(10.0));
	leg[2].jntFoot->SetSpringOrigin(Rad(10.0));

	//leg[0].jntDX1->SetMotorTorque(speed);
	//leg[1].jntDX1->SetMotorTorque(speed);
	//leg[2].jntDX1->SetMotorTorque(speed);
	//leg[0].jntDX2->SetMotorTorque(speed);
	//leg[1].jntDX2->SetMotorTorque(speed);
	//leg[2].jntDX2->SetMotorTorque(speed);
	//leg[0].jntFoot->SetMotorTorque(speed);
	//leg[1].jntFoot->SetMotorTorque(speed);
	//leg[2].jntFoot->SetMotorTorque(speed);
}

void Robot::Backward(){

	//leg[0].jntDX2->SetSpringOrigin(Rad(-60.0));
	leg[1].jntDX2->SetSpringOrigin(Rad(-60.0));
	leg[2].jntDX2->SetSpringOrigin(Rad(-60.0));
	//leg[0].jntFoot->SetSpringOrigin(Rad(150.0));
	leg[1].jntFoot->SetSpringOrigin(Rad(150.0));
	leg[2].jntFoot->SetSpringOrigin(Rad(150.0));

	//leg[0].jntDX1->SetMotorTorque(-speed);
	//leg[1].jntDX1->SetMotorTorque(-speed);
	//leg[2].jntDX1->SetMotorTorque(-speed);
	//leg[0].jntDX2->SetMotorTorque(-speed);
	//leg[1].jntDX2->SetMotorTorque(-speed);
	//leg[2].jntDX2->SetMotorTorque(-speed);
	//leg[0].jntFoot->SetMotorTorque(-speed);
	//leg[1].jntFoot->SetMotorTorque(-speed);
	//leg[2].jntFoot->SetMotorTorque(-speed);
}