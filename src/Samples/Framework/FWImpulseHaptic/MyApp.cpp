//#include "MyApp.h"
//#include <iostream>
//#include <sstream>
//#include <Framework/FWInteractScene.h>
//#include "PrimitiveMeshShape.h"
//
//#include <windows.h>
//
//using namespace std;
//
//MyApp::MyApp(){
//	bStep = true;
//	bDrawInfo = false;
//}
//
//void MyApp::Init(int argc, char* argv[]){
//	/// �`�惂�[�h�̐ݒ�
//	SetGRAdaptee(TypeGLUT);									// GLUT���[�h�ɐݒ�
//	GRInit(argc, argv);						// Sdk�̍쐬
//
//	/// Sdk�̏������C�V�[���̍쐬
//	CreateSdk();
//	GetSdk()->Clear();										// SDK�̏�����
//	GetSdk()->CreateScene(PHSceneDesc(), GRSceneDesc());	// Scene�̍쐬
//	GetSdk()->GetScene()->GetPHScene()->SetTimeStep(0.02);	// ���݂̐ݒ�
//	GetSdk()->GetScene()->GetPHScene()->GetConstraintEngine()->SetPosCorrectionRate(0);
//
//	/// �`��Window�̍쐬�C������
//	FWWinDesc windowDesc;									
//	windowDesc.title = "MyApp";
//	AssignScene(CreateWin(windowDesc));									
//	InitWindow();											
//	InitCameraView();										
//
//	/// HumanInterface�̏�����
//	InitHumanInterface();
//
//	/// InteractScene�̍쐬
//	FWInteractSceneDesc desc;
//	desc.fwScene = GetSdk()->GetScene();					// fwScene�ɑ΂���interactscene�����
//	desc.iaMode = IMPULSE;						// humaninterface�̃����_�����O���[�h�̐ݒ�
//	desc.hMode = PENALTY3D;
//	desc.hdt = 0.001;										// �}���`���[�g�̏ꍇ�̍X�V[s]
//	CreateIAScene(desc);									// interactScene�̍쐬
//
//	/// �����V�~�����[�V�������鍄�̂��쐬
//	BuildScene();
//
//	/// �^�C�}�̍쐬�C�ݒ�
//	UTTimerIf* timer = CreateTimer(UTTimerIf::MULTIMEDIA);
//	timer->SetInterval(1);
//	timer->SetResolution(1);
//}
//
//void MyApp::InitCameraView(){
//	std::istringstream issView(
//		"((0.9996 0.0107463 -0.0261432 -0.389004)"
//		"(-6.55577e-010 0.924909 0.380188 5.65711)"
//		"(0.0282657 -0.380037 0.92454 13.7569)"
//		"(     0      0      0      1))"
//	);
//	issView >> cameraInfo.view;
//}
//
//void MyApp::InitHumanInterface(){
//	/// HISdk�̍쐬
//	CreateHISdk();
//
//	DRUsb20SimpleDesc usbSimpleDesc;
//	GetHISdk()->AddRealDevice(DRUsb20SimpleIf::GetIfInfoStatic(), &usbSimpleDesc);
//	DRUsb20Sh4Desc usb20Sh4Desc;
//	for(int i=0; i<10; ++i){
//		usb20Sh4Desc.number = i;
//		GetHISdk()->AddRealDevice(DRUsb20Sh4If::GetIfInfoStatic(), &usb20Sh4Desc);
//	}
//	GetHISdk()->AddRealDevice(DRKeyMouseWin32If::GetIfInfoStatic());
//	GetHISdk()->Init();
//	GetHISdk()->Print(DSTR);
//#if 1
//	/// SPIDARG6��2��g���ꍇ
//	UTRef<HISpidarGIf> spg[2];
//	for(size_t i = 0; i < 2; i++){
//		spg[i] = GetHISdk()->CreateHumanInterface(HISpidarGIf::GetIfInfoStatic())->Cast();
//		if(i == 0) spg[i]->Init(&HISpidarGDesc("SpidarG6X3R"));
//		if(i == 1) spg[i]->Init(&HISpidarGDesc("SpidarG6X3L"));
//		AddHI(spg[i]);
//	}
//#else
//	/// SPIDAR4D���g���ꍇ
//	UTRef<HISpidar4If> spg[2];
//	for(size_t i = 0; i < 2; i++){
//		spg[i] = GetHISdk()->CreateHumanInterface(HISpidar4If::GetIfInfoStatic())->Cast();
//		if(i == 0) spg[i]->Init(&HISpidar4Desc("SpidarR",Vec4i(1,2,3,4)));
//		if(i == 1) spg[i]->Init(&HISpidar4Desc("SpidarL",Vec4i(5,6,7,8)));
//		AddHI(spg[i]);
//	}
//#endif
//}
//
//void MyApp::Reset(){
//	GetSdk()->Clear();
//	ClearIAScenes();
//	GetSdk()->CreateScene(PHSceneDesc(), GRSceneDesc());	// Scene�̍쐬
//	GetSdk()->GetScene()->GetPHScene()->SetTimeStep(0.02);	// ���݂̐ݒ�
//	FWInteractSceneDesc desc;
//	desc.fwScene = GetSdk()->GetScene();					// fwScene�ɑ΂���interactscene�����
//	desc.iaMode = IMPULSE;							// humaninterface�̃����_�����O���[�h�̐ݒ�
//	desc.hMode = PENALTY3D;
//	desc.hdt = 0.001;										// �}���`���[�g�̏ꍇ�̍X�V[s]
//	CreateIAScene(desc);									// interactScene�̍쐬
//	BuildScene();
//	GetCurrentWin()->SetScene(GetSdk()->GetScene());
//	InitCameraView();
//}
//
//void MyApp::IdleFunc(){
//	/// �V�~�����[�V������i�߂�(interactscene������ꍇ�͂��������Ă�)
//	if(bStep) GetIAScene()->Step();
//	else if(bOneStep){
//		GetIAScene()->Step();
//		bOneStep = false;
//	}
//	PostRedisplay();
//}
//
//void MyApp::TimerFunc(int id){	
//	/// HapticLoop���R�[���o�b�N����
//	GetIAScene()->CallBackHapticLoop();
//}
//
//void MyApp::Display(){
//	/// �`�惂�[�h�̐ݒ�
//	GetSdk()->SetDebugMode(true);
//	FWSceneIf* scene = GetSdk()->GetScene();
//	scene->SetRenderMode(true, false);
//	scene->EnableRenderAxis(bDrawInfo);
//	scene->EnableRenderForce(bDrawInfo);
//	scene->EnableRenderContact(bDrawInfo);
//
//	/// �J�������W�̎w��
//	GRCameraIf* cam = GetCurrentWin()->scene->GetGRScene()->GetCamera();
//	if (cam && cam->GetFrame()){
//		cam->GetFrame()->SetTransform(cameraInfo.view);
//	}else{
//		GetCurrentWin()->render->SetViewMatrix(cameraInfo.view.inv());
//	}
//	//DSTR << cameraInfo.view << std::endl;
//
//	/// �`��̎��s
//	if(!GetCurrentWin()) return;
//	GetSdk()->SwitchScene(GetCurrentWin()->GetScene());
//	GetSdk()->SwitchRender(GetCurrentWin()->GetRender());
//	GetSdk()->Draw();
//	if(bDrawInfo){	
//		DisplayContactPlane();
//		DisplayLineToNearestPoint();
//		//DisplayCutRing();
//		DisplayContactPoints();
//	}
//	glutSwapBuffers();
//}
//
//
//void MyApp::BuildScene(){
//	PHSceneIf* phscene = GetSdk()->GetScene()->GetPHScene();
//	PHSolidDesc desc;
//	CDBoxDesc bd;
//
//	/// ��(�����@���ɏ]��Ȃ��C�^�����ω����Ȃ�)
//	{
//		/// ����(soFloor)�̍쐬
//		desc.mass = 1e20f;
//		desc.inertia *= 1e30f;
//		PHSolidIf* soFloor = phscene->CreateSolid(desc);		// ���̂�desc�Ɋ�Â��č쐬
//		soFloor->SetDynamical(false);
//		soFloor->SetGravity(false);
//		/// �`��(shapeFloor)�̍쐬
//		bd.boxsize = Vec3f(50, 15, 50);
//		CDShapeIf* shapeFloor = GetSdk()->GetPHSdk()->CreateShape(bd);
//		/// ���̂Ɍ`���t������
//		soFloor->AddShape(shapeFloor);
//		soFloor->SetFramePosition(Vec3d(0, -10, 0));
//	}
//
//	/// ��(�����@���ɏ]���C�^�����ω�)
//	{
//		/// ����(soBox)�̍쐬
//		desc.mass = 0.05;
//		desc.inertia = 0.333 * Matrix3d::Unit();
//		PHSolidIf* soBox = phscene->CreateSolid(desc);
//		/// �`��(shapeBox)�̍쐬
//		bd.boxsize = Vec3f(4,4,4);
//		CDShapeIf* shapeBox = GetSdk()->GetPHSdk()->CreateShape(bd);
//		/// ���̂Ɍ`���t��
//		soBox->AddShape(shapeBox);
//		soBox->GetShape(0)->SetStaticFriction(2.0);
//		soBox->GetShape(0)->SetDynamicFriction(1.0);
//		soBox->SetFramePosition(Vec3d(0, 10, 0));
//	}
//
//	/// �|�C���^
//	{	
//		for(int i= 0; i < 1; i++){
//			PHSolidIf* soPointer = phscene->CreateSolid(desc);
//			CDSphereDesc sd;
//			sd.radius = 0.5;//1.0;
//			CDSphereIf* shapePointer = DCAST(CDSphereIf,  GetSdk()->GetPHSdk()->CreateShape(sd));
//			soPointer->AddShape(shapePointer);
//			soPointer->GetShape(0)->SetStaticFriction(1.0);
//			soPointer->GetShape(0)->SetDynamicFriction(1.0);
//			FWInteractPointerDesc idesc;			// interactpointer�̃f�B�X�N���v�^
//			idesc.pointerSolid = soPointer;			// soPointer��ݒ�
//			idesc.humanInterface = GetHI(i);		// humaninterface��ݒ�
//			idesc.springK = 3000;					// haptic rendering�̃o�l�W��
//			idesc.damperD = 0;						// haptic rendering�̃_���p�W��
//			idesc.posScale = 300;					// soPointer�̉���̐ݒ�(�`�{)
//			idesc.forceScale = 1.0;
//			idesc.localRange = 5.0;					// LocalDynamics���g���ꍇ�̋ߖT�͈�
//			if(i==0) idesc.defaultPosition =Posed(1,0,0,0,5,0,0);	// �����ʒu�̐ݒ�
//			if(i==1) idesc.defaultPosition =Posed(1,0,0,0,-5,0,0);
//			GetIAScene()->CreateIAPointer(idesc);	// interactpointer�̍쐬
//		}
//	}
//}
//
//
//void MyApp::Keyboard(int key, int x, int y){
//	PHSceneIf* phscene = GetSdk()->GetScene()->GetPHScene();
//	switch (key) {
//	case  27: //ESC
//	case 'q':
//		exit(0);
//		break;
//	case 'r':
//		Reset();
//		DSTR << "Reset Scene" << endl;
//		break;
//	case 'd':
//		bDrawInfo = !bDrawInfo;
//		break;
//	case 's':
//		bStep = false;
//		bOneStep = true;
//		DSTR << "Stepwise Execution" << endl;
//		break;
//	case 'a':
//		bStep = true;
//		DSTR << "Play" << endl;
//		break;
//	case 'c':
//		{
//			for(int i = 0; i < GetIAScene()->NIAPointers(); i++){
//				GetIAScene()->GetIAPointer(i)->Calibration();
//			}
//		}
//		break;
//	case 'f':
//		{
//			static bool bf = false;
//			bf = !bf;
//			for(int i = 0; i < GetIAScene()->NIAPointers(); i++){
//				GetIAScene()->GetIAPointer(i)->EnableForce(bf);
//			}
//			if(bf){
//				DSTR << "Enable Force Feedback" << std::endl;
//			}else{
//				DSTR << "Disable Force Feedback" << std::endl;
//			}
//		}
//		break;
//	case'n':
//		{
//			double k = GetIAScene()->GetIAPointer(0)->GetSpringCoeff();
//			k += 100;
//			DSTR << "Spring" << k << std::endl;
//			GetIAScene()->GetIAPointer(0)->SetSpringCoeff(k);
//			FWInteractPointer* ip = GetIAScene()->GetIAPointer(0)->Cast();
//			ip->CalcCorrectionSpringDamper();
//		}
//		break;
//	case'm':
//		{
//			double k = GetIAScene()->GetIAPointer(0)->GetSpringCoeff();
//			k -= 100;
//			DSTR << "Spring" << k << std::endl;
//			GetIAScene()->GetIAPointer(0)->SetSpringCoeff(k);
//			FWInteractPointer* ip = GetIAScene()->GetIAPointer(0)->Cast();
//			ip->CalcCorrectionSpringDamper();
//		}
//		break;
//	case' ':
//		break;
//	default:
//		break;
//	}
//}
//
//
///// �f�o�b�N�\���p
////////////////////////////////////////////////////////////////////////////////////////
//void MyApp::DisplayContactPlane(){
//	FWInteractScene* inScene = GetIAScene()->Cast();
//	FWSceneIf* scene = GetSdk()->GetScene();
//	int N = inScene->NIASolids();
//	for(int i = 0; i <  N; i++){
//		FWInteractSolid* inSolid = inScene->GetIASolid(i);
//		if(!inSolid->bSim) continue;
//		for(int j = 0; j < inScene->NIAPointers(); j++){
//			FWInteractPointer* inPointer = inScene->GetIAPointer(j)->Cast();
//			FWInteractInfo* inInfo = &inPointer->interactInfo[i];
//			if(!inInfo->flag.blocal) continue;
//			Vec3d pPoint = inPointer->pointerSolid->GetPose() * inInfo->toHaptic.pointer_point;
//			Vec3d cPoint = inSolid->sceneSolid->GetPose() * inInfo->toHaptic.closest_point;
//			Vec3d normal = inInfo->toHaptic.face_normal;
//			Vec3d v1(0, 1, 0);
//
//			v1 +=  Vec3d(0, 0, 1) - Vec3d(0, 0, 1)*normal*normal;
//			v1 -= v1*normal * normal;
//			v1.unitize();
//			v1 = v1 * 0.1;
//			Vec3d v2 = normal ^ v1;
//
//			Vec4f moon(1.0, 1.0, 0.8, 0.3);
//			GRRenderIf* render = GetCurrentWin()->render;
//			render->SetMaterial( GRMaterialDesc(moon) );
//			render->PushModelMatrix();
//			Vec3d offset = 0.001 * normal;
//			render->SetLighting( false );
//			render->SetAlphaTest(true);
//			render->SetAlphaMode(render->BF_SRCALPHA, render->BF_ONE);
//			cPoint += offset/2;
//			glBegin(GL_QUADS);
//				/// �ڐG�ʒ��	
//				glVertex3d(cPoint[0] + v1[0] + v2[0], cPoint[1] + v1[1] + v2[1], cPoint[2] + v1[2] + v2[2]);
//				glVertex3d(cPoint[0] - v1[0] + v2[0], cPoint[1] - v1[1] + v2[1], cPoint[2] - v1[2] + v2[2]);
//				glVertex3d(cPoint[0] - v1[0] - v2[0], cPoint[1] - v1[1] - v2[1], cPoint[2] - v1[2] - v2[2]);
//				glVertex3d(cPoint[0] + v1[0] - v2[0], cPoint[1] + v1[1] - v2[1], cPoint[2] + v1[2] - v2[2]);
//				/// ����1
//				glVertex3d(cPoint[0] + v1[0] + v2[0] + offset[0], 
//								cPoint[1] + v1[1] + v2[1] + offset[1], 
//								cPoint[2] + v1[2] + v2[2] + offset[2]);
//				glVertex3d(cPoint[0] + v1[0] + v2[0], 
//								cPoint[1] + v1[1] + v2[1], 
//								cPoint[2] + v1[2] + v2[2]);
//				glVertex3d(cPoint[0] - v1[0] + v2[0], 
//								cPoint[1] - v1[1] + v2[1], 
//								cPoint[2] - v1[2] + v2[2]);
//				glVertex3d(cPoint[0] - v1[0] + v2[0] + offset[0], 
//								cPoint[1] - v1[1] + v2[1] + offset[1], 
//								cPoint[2] - v1[2] + v2[2] + offset[2]);
//				/// ����2
//				glVertex3d(cPoint[0] - v1[0] + v2[0] + offset[0], 
//								cPoint[1] - v1[1] + v2[1] + offset[1], 
//								cPoint[2] - v1[2] + v2[2] + offset[2]);
//				glVertex3d(cPoint[0] - v1[0] + v2[0], 
//								cPoint[1] - v1[1] + v2[1], 
//								cPoint[2] - v1[2] + v2[2]);
//				glVertex3d(cPoint[0] - v1[0] - v2[0], 
//								cPoint[1] - v1[1] - v2[1], 
//								cPoint[2] - v1[2] - v2[2]);
//				glVertex3d(cPoint[0] - v1[0] - v2[0] + offset[0], 
//								cPoint[1] - v1[1] - v2[1] + offset[1], 
//								cPoint[2] - v1[2] - v2[2] + offset[2]);
//				/// ����3
//				glVertex3d(cPoint[0] - v1[0] - v2[0] + offset[0], 
//								cPoint[1] - v1[1] - v2[1] + offset[1], 
//								cPoint[2] - v1[2] - v2[2] + offset[2]);
//				glVertex3d(cPoint[0] - v1[0] - v2[0], 
//								cPoint[1] - v1[1] - v2[1], 
//								cPoint[2] - v1[2] - v2[2]);
//				glVertex3d(cPoint[0] + v1[0] - v2[0], 
//								cPoint[1] + v1[1] - v2[1], 
//								cPoint[2] + v1[2] - v2[2]);
//				glVertex3d(cPoint[0] + v1[0] - v2[0] + offset[0], 
//								cPoint[1] + v1[1] - v2[1] + offset[1], 
//								cPoint[2] + v1[2] - v2[2] + offset[2]);
//				/// ����4
//				glVertex3d(cPoint[0] + v1[0] - v2[0] + offset[0], 
//								cPoint[1] + v1[1] - v2[1] + offset[1], 
//								cPoint[2] + v1[2] - v2[2] + offset[2]);
//				glVertex3d(cPoint[0] + v1[0] - v2[0], 
//								cPoint[1] + v1[1] - v2[1], 
//								cPoint[2] + v1[2] - v2[2]);
//				glVertex3d(cPoint[0] + v1[0] + v2[0], 
//								cPoint[1] + v1[1] + v2[1],
//								cPoint[2] + v1[2] + v2[2]);
//				glVertex3d(cPoint[0] + v1[0] + v2[0] + offset[0], 
//								cPoint[1] + v1[1] + v2[1] + offset[1], 
//								cPoint[2] + v1[2] + v2[2] + offset[2]);
//				/// �ڐG�ʏ��
//				glVertex3d(cPoint[0] - v1[0] + v2[0] + offset[0], 
//								cPoint[1] - v1[1] + v2[1] + offset[1], 
//								cPoint[2] - v1[2] + v2[2] + offset[2]);
//				glVertex3d(cPoint[0] + v1[0] + v2[0] + offset[0], 
//								cPoint[1] + v1[1] + v2[1] + offset[1], 
//								cPoint[2] + v1[2] + v2[2] + offset[2]);
//				glVertex3d(cPoint[0] + v1[0] - v2[0] + offset[0], 
//								cPoint[1] + v1[1] - v2[1] + offset[1], 
//								cPoint[2] + v1[2] - v2[2] + offset[2]);
//				glVertex3d(cPoint[0] - v1[0] - v2[0] + offset[0], 
//								cPoint[1] - v1[1] - v2[1] + offset[1], 
//								cPoint[2] - v1[2] - v2[2] + offset[2]);
//			glEnd();
//			render->SetLighting( true);
//			render->SetAlphaTest(false);
//			render->PopModelMatrix();
//			glEnable(GL_DEPTH_TEST);
//		}
//	}
//}
//
//void MyApp::DisplayLineToNearestPoint(){
//	FWInteractScene* inScene = DCAST(FWInteractScene, GetIAScene());
//	int N = inScene->NIASolids();
//	GLfloat moon[]={0.8,0.8,0.8};
//	for(int i = 0; i <  N; i++){
//		FWInteractSolid* inSolid = inScene->GetIASolid(i);
//		if(!inSolid->bSim) continue;
//		for(int j = 0; j < inScene->NIAPointers(); j++){
//			FWInteractPointer* inPointer = inScene->GetIAPointer(j)->Cast();
//			FWInteractInfo* inInfo = &inPointer->interactInfo[i];
//			if(!inInfo->flag.blocal) continue;
//			Vec3d pPoint = inPointer->pointerSolid->GetPose() * inInfo->toHaptic.pointer_point;
//			Vec3d cPoint = inSolid->sceneSolid->GetPose() * inInfo->toHaptic.closest_point;
//			Vec3d normal = inInfo->toHaptic.face_normal * 0;
//			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, moon);
//			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, moon);
//			glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, moon);
//			glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, moon);
//			glDisable(GL_DEPTH_TEST);
//			glBegin(GL_LINES);
//			glVertex3f(pPoint.X() + normal[0], pPoint.Y() + normal[1], pPoint.Z() + normal[2]);
//			glVertex3f(cPoint.X(), cPoint.Y(), cPoint.Z());
//			glEnd();
//			glEnable(GL_DEPTH_TEST);
//		}
//	}
//}
//
//void MyApp::DisplayCutRing(){
//	FWInteractScene* inScene = DCAST(FWInteractScene, GetIAScene());
//	int N = inScene->NIASolids();
//	GLfloat moon[]={0.8,0.8,0.8};
//	for(int i = 0; i <  N; i++){
//		FWInteractSolid* inSolid = inScene->GetIASolid(i);
//		if(!inSolid->bSim) continue;
//		for(int j = 0; j < inScene->NIAPointers(); j++){
//			FWInteractPointer* inPointer = inScene->GetIAPointer(j)->Cast();
//			FWInteractInfo* inInfo = &inPointer->interactInfo[i];
//			if(!inInfo->flag.blocal) continue;
//			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, moon);
//			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, moon);
//			glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, moon);
//			glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, moon);
//			glDisable(GL_DEPTH_TEST);
//			glBegin(GL_LINE_LOOP);
//			for(int k = 0; k < (int)inInfo->toHaptic.solid_section.size(); k++){
//				Vec3f r = inSolid->sceneSolid->GetPose() * inInfo->toHaptic.solid_section[k];
//				glVertex3f(r.x, r.y, r.z);
//			}
//			glEnd();
//			glEnable(GL_DEPTH_TEST);
//		}
//	}
//}
//
//
//void MyApp::DisplayContactPoints(){
//	FWInteractScene* inScene = DCAST(FWInteractScene, GetIAScene());
//	int N = inScene->NIASolids();
//	for(int i = 0; i <  N; i++){
//		FWInteractSolid* inSolid = inScene->GetIASolid(i);
//		if(!inSolid->bSim) continue;
//		for(int j = 0; j < inScene->NIAPointers(); j++){
//			FWInteractPointer* inPointer = inScene->GetIAPointer(j)->Cast();
//			FWInteractInfo* inInfo = &inPointer->interactInfo[i];
//			if(!inInfo->flag.blocal) continue;
//			glEnable(GL_COLOR_MATERIAL);
//			glDisable(GL_DEPTH_TEST);
//
//			for(int k = 0; k < (int)inInfo->toHaptic.intersection_vertices.size(); k++){
//				Vec3f r = inPointer->GetPointerSolid()->GetPose() * inInfo->toHaptic.intersection_vertices[k];
//				glPushMatrix();
//				glTranslated(r.x, r.y, r.z);
//				glColor3d(1.0f, 0.0f, 0.0f);
//				glutSolidSphere(0.01, 10, 10);
//				glPopMatrix();
//			}
//
//			glEnable(GL_DEPTH_TEST);
//			glDisable(GL_COLOR_MATERIAL);
//		}
//	}
//}