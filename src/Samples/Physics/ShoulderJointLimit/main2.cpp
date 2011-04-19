#include <Springhead.h>
//
#include <stdlib.h>
//#include "TMatrix.h"//�s�񃉃C�u�������C���N���[�h����B
#include <iostream>
//
#include <GL/glut.h>

using namespace PTM;//  �s��N���X��PTM���O��Ԃ̒��Ő錾����Ă���B

using namespace Spr;
UTRef< GRDebugRenderIf > grRender   = NULL;
UTRef< PHSceneIf >       phScene    = NULL;
UTRef< PHSolidIf >		 solid[5]	= {0,0,0,0,0};
UTRef< PHJoint1DIf > joint[2];

// �J�����ʒu
double CameraZoom = 30.0;
double CameraRotX =  0.0;
double CameraRotY =  4.6;
//�֐߂̊p�x
double q1 = 90;
double q2 = 10;
//�N���b�N�����ʒu
double point[2][2];				// ���W���L������z�� 
int w=800;//�E�B���h�E�̃T�C�Y
int h=600;


void Display() {
	// ���_�̐ݒ�
	Affinef view;
	double yoffset = 0.0;
	view.Pos() = CameraZoom * Vec3f( cos(CameraRotX) * cos(CameraRotY),
									 sin(CameraRotX),
									 cos(CameraRotX) * sin(CameraRotY) );
	view.PosY() += yoffset;
	view.LookAtGL(Vec3f(0.0f, yoffset, 0.0f), Vec3f(0.0f, 100.0f, 0.0f));
	grRender->SetViewMatrix(view.inv());
//	glViewport(0, 0, w, h);

	grRender->ClearBuffer();
	grRender->DrawScene(phScene);
	grRender->EndScene();
	glutSwapBuffers();
}

void Timer(int id){
	double x,y,fai;
	glutTimerFunc(20, Timer,0);
	//�V�~�����[�V�����̃X�e�b�v
	phScene->ClearForce();
	//����@�\
	//���S�ʒu�A400,300// 400,255
	//point[1][0] = CameraZoom*(8*cos(Rad(-q1))+7*cos(Rad(-q2)))+400;
	//point[1][1] = -CameraZoom*(8*sin(Rad(-q1))+7*sin(Rad(-q2)))-255;
	//std::cout << point[0][0];
//
	point[1][0] = 8*cos(Rad(q1))+7*cos(Rad(q1+q2));//���݂̈ʒu
	point[1][1] = 8*sin(Rad(q1))+7*sin(Rad(q1+q2));
	
	fai= acos(point[0][0]/sqrt(point[0][0]*point[0][0]+point[0][1]*point[0][1]));
	TVector<2,double> dX;

	x = (point[0][0]-point[1][0])/200;//���ݒl�ƖڕW�l�̍�
	y = (point[0][1]-point[1][1])/200;

	point[0][0] = point[1][0];
	point[0][1] = point[1][1];

	TVector<2,double> dQ; 
	TMatrixRow<2,2,double> jacobien;//2�~2�̍s���錾


	for(int count=0;count<200;count++){
		point[1][0] = 8*cos(Rad(q1))+7*cos(Rad(q1+q2));//���݂̈ʒu�̌v�Z
		point[1][1] = 8*sin(Rad(q1))+7*sin(Rad(q1+q2));

		point[0][0] += x;
	    point[0][1] += y;

		dX[0] = point[0][0]-point[1][0];//���ݒl�ƖڕW�l�̍�
		dX[1] = point[0][1]-point[1][1];

		jacobien[0][0] = -8*sin(Rad(q1))-7*sin(Rad(q1+q2)); jacobien[0][1] = -7*sin(Rad(q1+q2));
		jacobien[1][0] = 8*cos(Rad(q1))+7*cos(Rad(q1+q2));  jacobien[1][1] = 7*cos(Rad(q1+q2));

		dQ = jacobien.inv()*dX; //�t�^���w���f���ɂ��A�p�x�̕ω������߂�
		//std::cout << jacobien;
		if(dQ[0]<-0&&-90<q1){ q1 = q1-1;}
		else if(dQ[0]>0&&q1<270){ q1 = q1+1;}
		if(dQ[1]<-0&&q2>-180){ q2 = q2-1;}
		else if(dQ[1]>0&&q2<180){ q2 = q2+1;}
	}
		printf("%f,%f\n",point[0][0],point[0][1]);
	    printf("%f,%f\n",q1,q2);
	
	printf("%f,%f\n",q1,q2);
	joint[0]->SetSpringOrigin(Rad(90.0-q1));
	joint[1]->SetSpringOrigin(Rad(-q2));
	// �V�~�����[�V�����̃X�e�b�v�̑���
	phScene->GenerateForce();
	phScene->Integrate();
	glutPostRedisplay();
}
void Reshape(int w, int h){
	grRender->Reshape(Vec2f(), Vec2f(w,h));

}


void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
		case 'x': 
			CameraRotX+=0.2;
			break;
		case 'y': 
			CameraRotY+=0.2;
			break;
		case '1':
			point[0][0]=14.0;
			point[0][1]=1.0;
			printf("%f,\n",point[0][0]);
			break;
		case '2':
			point[0][0]=1.0;
			point[0][1]=12.0;
			printf("%f,\n",point[0][0]);
			
			break;
		case '3':
			point[0][0]=-10.0;
			point[0][1]=5.0;
			printf("%f,\n",point[0][0]);
			
			break;
		case 'q':
		case 'Q':
		case '\033':  //\033��ESC��ASCII �R�[�h
		exit(0);
		default:
		break;
	}
}
void mouse(int button, int state, int x, int y){

	switch (button) {
		case GLUT_LEFT_BUTTON:
			printf("Left");
			//�N���b�N�������Ƃ�
			if(state==GLUT_DOWN){
				point[0][0] = (x-w/2)/30.0;
				point[0][1] = (h/2-y)/30.0;
				printf("%f,%f\n",point[0][0],point[0][1]);
			//�N���b�N�������Ƃ�
			}
			else{
				point[0][0] = (x-w/2)/20.0;
				point[0][1] = (h/2-y)/20.0;

			}
			break;
		case GLUT_MIDDLE_BUTTON:
			printf("Middle");
			break;
		case GLUT_RIGHT_BUTTON:
			printf("Right");
			break;
		default:
			break;
	}

	printf(" button is ");

	switch (state) {
		case GLUT_UP:
			printf("up");
			break;
		case GLUT_DOWN:
			printf("down");
			break;
		default:
			break;
	}
	solid[3]->SetCenterPosition(Vec3d(-point[0][0],point[0][1]+1.5,0.0));
	//printf(" at (%d, %d)\n", x, y);
}

int main(int argc,char* argv[]){
	point[0][0] = 8*cos(Rad(q1))+7*cos(Rad(q1+q2));//400;//
	point[0][1] = 8*sin(Rad(q1))+7*sin(Rad(q1+q2));//195;//
	//printf("%f,%f\n",point[0][0],point[0][1]);

	//SDK�̐���
	UTRef< PHSdkIf > phSdk = PHSdkIf::CreateSdk();
	UTRef< GRSdkIf > grSdk = GRSdkIf::CreateSdk();
	
	//�V�[���O���t�̐���
	PHSceneDesc dScene;
	dScene.timeStep = 0.05;		//�ϕ����ԕ�
	dScene.numIteration = 20;	//�J��Ԃ��v�Z��
	phScene = phSdk->CreateScene(dScene);

	//�f�X�N���v�^��p���č��̃I�u�W�F�N�g�𐶐�
	PHSolidDesc sDesc;
	PHSolidDesc dSolid0;
	//���̂��󒆂ɌŒ肷��
	dSolid0.dynamical = false;

	//UTRef< PHSolidIf > solid[3];
	solid[0] = phScene->CreateSolid(dSolid0);
	solid[1] = phScene->CreateSolid(sDesc);
	solid[2] = phScene->CreateSolid(sDesc);
	solid[3] = phScene->CreateSolid(sDesc);

	//�f�X�N���v�^��p���Ĕ��`��I�u�W�F�N�g�𐶐�
	CDBoxDesc bDesc;
	CDBoxDesc sBase;
	CDSphereDesc sPoint;
	bDesc.boxsize = Vec3f(1.0f,6.0f,1.0f);		//Vec3f(�c�C���C����)�����������Ă���
	sBase.boxsize = Vec3f(1.0f,1.0f,1.0f);
	sPoint.radius = 0.5;

	UTRef< CDShapeIf > shapeBox = phSdk->CreateShape(bDesc);
	UTRef< CDShapeIf > BaseBox  = phSdk->CreateShape(sBase);
	UTRef< CDShapeIf > shapePoint  = phSdk->CreateShape(sPoint);
	//���̃I�u�W�F�N�g�ɔ��I�u�W�F�N�g�����蓖�Ă�
	solid[0]->AddShape(BaseBox);
	solid[1]->AddShape(shapeBox);
	solid[2]->AddShape(shapeBox);
	solid[3]->AddShape(shapePoint);
	phScene->SetContactMode(solid[1],solid[2],PHSceneDesc::MODE_NONE);
	phScene->SetContactMode(solid[3],PHSceneDesc::MODE_NONE);

	solid[3]->SetGravity(false);
	solid[3]->SetCenterPosition(Vec3d(-point[0][0],point[0][1]+1.5,0.0));
	//solid[3]->SetPose(Vec3f(point[0][0],point[0][1],0.0));
	//�f�X�N���v�^��2�̍��̂�p���Ċ֐߂𐶐�
	PHHingeJointDesc hDesc;
	hDesc.poseSocket.Pos() = Vec3d(0.0f, 2.0f, 0.0f);
	//hDesc.poseSocket.Ori() = Quaterniond::Rot(Rad(-90.0f),'x');
	hDesc.posePlug.Pos()   = Vec3d(0.0f, -4.0f, 0.0f);
	//hDesc.posePlug.Ori()   = Quaterniond::Rot(Rad(-q1),'x');
	hDesc.spring		   = 1000;
	hDesc.damper		   = 100;
	joint[0] = (PHJoint1DIf*) phScene->CreateJoint(solid[0], solid[1], hDesc);

	//�f�X�N���v�^��2�̍��̂�p���Ċ֐߂𐶐�
	hDesc.poseSocket.Pos() = Vec3d(0.0f, 4.0f, 0.0f);
	//hDesc.poseSocket.Ori() = Quaterniond::Rot(Rad(-90.0f),'x');
	hDesc.posePlug.Pos()   = Vec3d(0.0f, -4.0f, 0.0f);
	//hDesc.posePlug.Ori()   = Quaterniond::Rot(Rad(-180+q1-q2),'x');
	hDesc.spring		   = 1000;
	hDesc.damper		   = 100;
	joint[1] = (PHJoint1DIf*) phScene->CreateJoint(solid[1], solid[2], hDesc);

	//GL�֌W
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(w,h);
	int window = glutCreateWindow("Springhead Application");

	grRender = grSdk->CreateDebugRender();
	UTRef< GRDeviceGLIf > grDevice = grSdk->CreateDeviceGL();
	grDevice -> Init();
	glutKeyboardFunc(Keyboard);
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutTimerFunc(20,Timer,0);
	glutMouseFunc(mouse);

	grRender->SetDevice(grDevice);//Springhead2�̕`��@�\��OpenGL�̎g�p�o�^������B

	// �Ɩ��̐ݒ�
	GRLightDesc light0, light1;
	light0.position = Vec4f(10.0, 20.0, 20.0, 1.0);
	light1.position = Vec4f(-10.0, 10.0, 10.0, 1.0);
	grRender->PushLight(light0);
	grRender->PushLight(light1);

	glutMainLoop();

	return 0;
}