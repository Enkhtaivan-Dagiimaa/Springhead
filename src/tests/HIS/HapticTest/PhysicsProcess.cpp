#include "PhysicsProcess.h"
#include "HapticProcess.h"
#include <vector>
#include <iostream>
#include <sstream>
#include <cmath>
#include <ctime>
#include <string>
#include <GL/glut.h>
#include <windows.h>
#include <Physics/PHConstraintEngine.h>
#include <Base/TMatrix.h>

PhysicsProcess pprocess;

PhysicsProcess::PhysicsProcess(){
	bsync=false;
	calcPhys=true;
	dt = 0.02;//0.05;
	gravity =  Vec3d(0, -9.8f , 0);
	nIter = 15;
	bGravity = true;
	range = 0.7;
	bDebug = false;
	expandedObjects.clear();
	bStep = true;
}

namespace Spr{
void FASTCALL ContFindCommonPointSaveParam(const CDConvex* a, const CDConvex* b,
	const Posed& a2w, const Posed& b2w, const Vec3d& dir, double start, double end,
	Vec3d& normal, Vec3d& pa, Vec3d& pb, double& dist);
}

void PhysicsProcess::Init(int argc, char* argv[]){
	FWAppGLUT::Init(argc, argv);

	GetSdk()->Clear();															// SDK�̍쐬
	GetSdk()->CreateScene(PHSceneDesc(), GRSceneDesc());		// Scene�̍쐬
	PHSceneIf* phscene = GetSdk()->GetScene()->GetPHScene();
	phscene->SetGravity(gravity);				
	phscene->SetTimeStep(dt);
	phscene->SetNumIteration(nIter);

	states = ObjectStatesIf::Create();
	states2 = ObjectStatesIf::Create();

	DesignObject();																// ���̂��쐬

	FWWinDesc windowDesc;
	windowDesc.title = "HapticConnect1.2";
	window = CreateWin(windowDesc);
	window->scene = GetSdk()->GetScene();

	InitCameraView();
}

void PhysicsProcess::InitCameraView(){
	std::istringstream issView(
		"((0.9996 0.0107463 -0.0261432 -0.389004)"
		"(-6.55577e-010 0.924909 0.380188 5.65711)"
		"(0.0282657 -0.380037 0.92454 13.7569)"
		"(     0      0      0      1))"
	);
	issView >> cameraInfo.view;
}

void PhysicsProcess::DesignObject(){
	PHSceneIf* phscene = GetSdk()->GetScene()->GetPHScene();
	phscene->SetStateMode(true);

	PHSolidDesc desc;		// ���̂̃f�B�X�N���v�^
	CDBoxDesc bd;			// �����̌`��̃f�B�X�N���v�^
	CDSphereDesc sd;		// ���̂̃f�B�X�N���v�^

	// ���̍쐬
	{
		// soFloor�p��desc
		desc.mass = 1e20f;
		desc.inertia *= 1e30f;
		PHSolidIf* soFloor = phscene->CreateSolid(desc);		
		soFloor->SetDynamical(false);
		soFloor->SetGravity(false);

		bd.boxsize = Vec3f(60, 40, 40);
		CDBoxIf* shapeFloor = XCAST(GetSdk()->GetPHSdk()->CreateShape(bd));
		shapeFloor->SetName("shapeFloor");
		soFloor->AddShape(shapeFloor);
		soFloor->SetFramePosition(Vec3d(0, -20.7 ,0));
		soFloor->GetShape(0)->SetVibration(-100, 150, 150);
		soFloor->SetName("solidFloor");
	}
	// �͊o�|�C���^�̍쐬
	{
		soPointer = phscene->CreateSolid(desc);
		sd.radius = 0.5;//1.0;
		CDSphereIf* shapePointer = DCAST(CDSphereIf,  GetSdk()->GetPHSdk()->CreateShape(sd));
		soPointer->AddShape(shapePointer);
		soPointer->SetFramePosition(Vec3d(0, 3.0, 0));  
		soPointer->SetDynamical(false);
		// �͊o�|�C���^��springhead2�̐ڐG�v�Z����O��
		// ���̂������邽�т�ContactMode��NONE�ɂ��Ȃ���΂����Ȃ�(���������̂ƐڐG�v�Z�����Ă��܂�����)
		phscene->SetContactMode(soPointer, PHSceneDesc::MODE_NONE);
	}
}

void PhysicsProcess::Idle(){
	PhysicsStep();
}

void PhysicsProcess::Start(){
	instance = this;
	if (!NWin()){
		CreateWin();
		wins.back()->SetScene(GetSdk()->GetScene());
	}
	glutIdleFunc(FWAppGLUT::GlutIdleFunc);
	glutMainLoop();
}

void PhysicsProcess::PhysicsStep(){
	PHSceneIf* phscene = GetSdk()->GetScene()->GetPHScene();
	if (bsync) return;
	if (calcPhys){
		UpdateHapticPointer();
		vector<SpatialVector> lastvel;
		for(unsigned int i = 0; i < expandedObjects.size(); i++){
			if(!expandedObjects[i].flag.blocal) continue;
			lastvel.resize(i + 1);
			lastvel.back().v() = expandedObjects[i].phSolidIf->GetVelocity();
			lastvel.back().w() = expandedObjects[i].phSolidIf->GetAngularVelocity();
		}
		if(bStep) phscene->Step();
		else if (bOneStep){
			phscene->Step();
			bOneStep = false;
		}

		for(unsigned i = 0; i < expandedObjects.size(); i++){
			if(!expandedObjects[i].flag.blocal) continue;
			SpatialVector curvel;
			curvel.v() = expandedObjects[i].phSolidIf->GetVelocity();
			curvel.w() = expandedObjects[i].phSolidIf->GetAngularVelocity();
			expandedObjects[i].syncInfo.motionCoeff.curb = (curvel - lastvel[i]) / dt;
		}
		ExpandSolidInfo();
		FindNearestObject();	// �ߖT���̂̎擾
		PredictSimulation();
		glutPostRedisplay();
		calcPhys = false;
	}
	if (hapticcount < dt/hprocess.dt) return;
	hapticcount -= dt/hprocess.dt;
	bsync = true;
	calcPhys = true;

	//static DWORD last;
	//if (phscene->GetCount() % 10==0){
	//	DWORD t = timeGetTime();
	//	DSTR << phscene->GetCount() << " dt:" << t-last << std::endl;
	//	last = t;
	//}
}

void PhysicsProcess::Display(){
	// �`��̐ݒ�
	GetSdk()->SetDebugMode(true);
	GRDebugRenderIf* render = window->render->Cast();

	// �`�惂�[�h�̐ݒ�
	render->SetRenderMode(true, false);
//	render->EnableRenderAxis(bDebug);
	render->EnableRenderForce(bDebug);
	render->EnableRenderContact(bDebug);

	// �J�������W�̎w��
	GRCameraIf* cam = window->scene->GetGRScene()->GetCamera();
	if (cam && cam->GetFrame()){
		cam->GetFrame()->SetTransform(cameraInfo.view);
	}else{
		window->render->SetViewMatrix(cameraInfo.view.inv());
	}

	// �`��̎��s
	if(!GetCurrentWin()) return;
	GRRenderIf*curRender =  GetCurrentWin()->GetRender();
	FWSceneIf* curScene = GetCurrentWin()->GetScene();

	GetSdk()->SwitchScene(curScene);
	GetSdk()->SwitchRender(curRender);

	if(!curRender) return;
	curRender->ClearBuffer();
	curRender->BeginScene();

	if (curScene) curScene->Draw(curRender, GetSdk()->GetDebugMode());

	//	�����̒ǉ�
	GRLightDesc ld;
	ld.diffuse = Vec4f(1,1,1,1) * 0.8f;
	ld.specular = Vec4f(1,1,1,1) * 0.8f;
	ld.ambient = Vec4f(1,1,1,1) * 0.4f;
	ld.position = Vec4f(1,1,1,0);
	render->PushLight(ld);
	if(bDebug){
		DisplayContactPlane();
		DisplayLineToNearestPoint();			// �͊o�|�C���^�ƍ��̂̋ߖT�_�̊Ԃ��Ȃ�
	}
	render->PopLight();	//	�����̍폜

	curRender->EndScene();
	glutSwapBuffers();
}

void PhysicsProcess::UpdateHapticPointer(){
	soPointer->SetFramePosition(phpointer.GetFramePosition());				
//	soPointer->SetFramePosition(cameraInfo.view.Rot() * phpointer.GetFramePosition());		// (������)cameraInfo.view.Rot()�������ė͊o�|�C���^�̑�����J��������]�ɂ��킹��(*�͊o���l���Ȃ��Ƃ�����)			
	soPointer->SetOrientation(phpointer.GetOrientation());					
	soPointer->SetVelocity(phpointer.GetVelocity());
	soPointer->SetAngularVelocity(phpointer.GetAngularVelocity());	
	soPointer->SetDynamical(false);
}

void PhysicsProcess::ExpandSolidInfo(){
	PHSceneIf* phscene = GetSdk()->GetScene()->GetPHScene(); 
	PHSolidIf** solids = phscene->GetSolids();
	expandedObjects.clear();
	for(unsigned i = (int)expandedObjects.size(); i < phscene->NSolids(); i++){
		expandedObjects.resize(i + 1);
		expandedObjects.back().phSolidIf = solids[i];
	}
}

void PhysicsProcess::FindNearestObject(){	
	// GJK���g���ċߖT���̂ƋߖT���̂̍ŋߓ_���擾
	// ��������ׂĂ�shape�������̂ɂ��Ă��
	// AABB�ŗ͊o�|�C���^�ߖT�̕��̂��i��
	// �����ōi�������̂ɂ���GJK���s���D�����ōi�邱�Ƃ�GJK������񐔂����Ȃ��ł���D
	// Solid��BBox���x���ł̌�������(z���\�[�g)�D�����̂�����̖����g�����O		
	//1. BBox���x���̏Փ˔���
	size_t N = expandedObjects.size();
	Vec3f dir(0,0,1);
	Edges edges;
	edges.resize(2 * N);
	Edges::iterator eit = edges.begin();
	for(int i = 0; i < N; ++i){
		// ���[�J����������ׂ�false�ɂ���
		expandedObjects[i].flag.bneighbor = false;
		DCAST(PHSolid, expandedObjects[i].phSolidIf)->GetBBoxSupport(dir, eit[0].edge, eit[1].edge);
		Vec3d dPos = expandedObjects[i].phSolidIf->GetDeltaPosition();
		float dLen = (float) (dPos * dir);
		if (dLen < 0){
			eit[0].edge += dLen;
		}else{
			eit[1].edge += dLen;
		}
		eit[0].index = i; eit[0].bMin = true;
		eit[1].index = i; eit[1].bMin = false;
		eit += 2;
	}
	sort(edges.begin(), edges.end());
	//�[���猩�Ă����āC�ڐG�̉\��������m�[�h�̔��������D
	typedef set<int> SolidSet;
	SolidSet cur;							//	���݂�Solid�̃Z�b�g
	bool found = false;

	for(Edges::iterator it = edges.begin(); it != edges.end(); ++it){
		if (it->bMin){						//	���[��������C���X�g���̕��̂Ɣ���
			for(SolidSet::iterator itf=cur.begin(); itf != cur.end(); ++itf){
				int f1 = it->index;
				int f2 = *itf;
				if (f1 > f2) std::swap(f1, f2);
				// �ߖT���̂Ƃ��Č���
				if(expandedObjects[f1].phSolidIf == soPointer){
					expandedObjects[f2].flag.bneighbor = true;
				}else if(expandedObjects[f2].phSolidIf == soPointer){
					expandedObjects[f1].flag.bneighbor = true;
				}
			}
			cur.insert(it->index);
		}else{
			cur.erase(it->index);			//	�I�[�Ȃ̂ō폜�D
		}
	}

	for(size_t i = 0; i < expandedObjects.size(); i++){
		if(expandedObjects[i].flag.bneighbor) continue;
		expandedObjects[i].flag.bfirstlocal = false;			//�ߖT���̂łȂ��̂�false�ɂ���
		expandedObjects[i].flag.blocal = false;
	}

	// GJK�ŋߖT�_�����߁C�͊o�|�C���^�ŋߖT�̕��̂����肷��
	// �ŋߖT���̂�������blocal��true�ɂ��CphSolid��phSolidIf���R�s�[����
	// blocal�����ł�true��������R�s�[�ς݂Ȃ̂ŋߖT�_�����R�s�[����
	for(unsigned i = 0; i < (int)expandedObjects.size(); i++){
		if(!expandedObjects[i].flag.bneighbor) continue;															// �ߖT�łȂ���Ύ���
		CDConvex* a = DCAST(CDConvex, expandedObjects[i].phSolidIf->GetShape(0));		// ���̂����ʌ`��
		CDConvex* b = DCAST(CDConvex, soPointer->GetShape(0));									// �͊o�|�C���^�̓ʌ`��
		Posed a2w, b2w;																								// ���̂̃��[���h���W
/*		if(expandedObjects[i].blocal) 	a2w = expandedObjects[i].phSolid.GetPose();				// blocal��true�Ȃ�ŐV�̏��ł��
		else */								a2w = expandedObjects[i].phSolidIf->GetPose();
		b2w = soPointer->GetPose();																			// �͊o�|�C���^�̃��[���h���W
		Vec3d pa ,pb;																									// pa:���̂̋ߖT�_�Cpb:�͊o�|�C���^�̋ߖT�_�i���[�J�����W�j
		pa = pb = Vec3d(0.0, 0.0, 0.0);
		FindClosestPoints(a, b, a2w, b2w, pa, pb);															// GJK�ŋߖT�_�̎Z�o
		Vec3d wa = a2w * pa;																						// ���̋ߖT�_�̃��[���h���W
		Vec3d wb = b2w * pb;																						// �͊o�|�C���^�ߖT�_�̃��[���h���W
		Vec3d a2b = wb - wa;																						// ���̂���͊o�|�C���^�ւ̃x�N�g��
		Vec3d normal = a2b.unit();
		if(a2b.norm() < range){																					// �ߖT�_�܂ł̒�������ߖT���̂��i��
			if(a2b.norm() < 0.01){																				// �͊o�|�C���^�ƍ��̂����łɐڐG���Ă�����CCDGJK�Ŗ@�������߂�		
				pa = pb = Vec3d(0.0, 0.0, 0.0);
				Vec3d dir = -1.0 * expandedObjects[i].syncInfo.neighborPoint.face_normal;
				if(dir == Vec3d(0.0, 0.0, 0.0) ){
					dir = -(soPointer->GetCenterPosition() - wa);
				}
				double dist = 0.0;
				int cp = ContFindCommonPoint(a, b, a2w, b2w, dir, -DBL_MAX, 1, normal, pa, pb, dist);
				if(cp != 1){
					ContFindCommonPointSaveParam(a, b, a2w, b2w, dir, -DBL_MAX, 1, normal, pa, pb, dist);
					DSTR << "ContFindCommonPoint do not find contact point" << endl;
				}
			}
			if(!expandedObjects[i].flag.blocal){																			// ���߂čŋߖT���̂ɂȂ�����
				expandedObjects[i].flag.bfirstlocal = true;
				expandedObjects[i].phSolid = *DCAST(PHSolid, expandedObjects[i].phSolidIf);		// �V�[���������̂̒��g��͊o�v���Z�X�Ŏg�����́i���́j�Ƃ��ăR�s�[����
#ifdef _DEBUG
				if (expandedObjects[i].syncInfo.neighborPoint.face_normal * normal < 0.8){
					DSTR << "Too big change on normal" << normal << std::endl;
				}
#endif
				expandedObjects[i].syncInfo.neighborPoint.face_normal = normal;														// ���߂čŋߖT���̂ɂȂ����̂ŁC�O��̖@���͎g��Ȃ��D										
			}
			expandedObjects[i].flag.blocal = true;																		// �ŋߖT���̂Ȃ̂�blocal��true�ɂ���
			expandedObjects[i].syncInfo.neighborPoint.closestPoint = pa;																	// ���̋ߖT�_�̃��[�J�����W
			expandedObjects[i].syncInfo.neighborPoint.pointerPoint = pb;																// �͊o�|�C���^�ߖT�_�̃��[�J�����W
			expandedObjects[i].syncInfo.neighborPoint.last_face_normal = expandedObjects[i].syncInfo.neighborPoint.face_normal;					// �O��̖@���i�@���̕�ԂɎg���j
			expandedObjects[i].syncInfo.neighborPoint.face_normal = normal;															// ���̂���͊o�|�C���^�ւ̖@��
		}else{
			expandedObjects[i].flag.blocal = false;																		// �ŋߖT���̂ł͂Ȃ��̂�blocal��false�ɂ���
			expandedObjects[i].flag.bfirstlocal = false;
		}
	}
}

#define DIVIDE_STEP

void PhysicsProcess::PredictSimulation(){
	PHSceneIf* phscene = GetSdk()->GetScene()->GetPHScene();
	// neighborObjets��blocal��true�̕��̂ɑ΂��ĒP�ʗ͂������C�ڐG���Ă��邷�ׂĂ̕��̂ɂ��āC�^���W�����v�Z����
#ifdef DIVIDE_STEP
	states2->SaveState(phscene);			// �\���V�~�����[�V�����̂��߂Ɍ��݂̍��̂̏�Ԃ�ۑ�����
	//	LCP�̒��O�܂ŃV�~�����[�V�������Ă��̏�Ԃ�ۑ�
	phscene->ClearForce();
	phscene->GenerateForce();
	phscene->IntegratePart1();
#endif
	states->SaveState(phscene);			// �\���V�~�����[�V�����̂��߂Ɍ��݂̍��̂̏�Ԃ�ۑ�����

	for(unsigned i = 0; i < expandedObjects.size(); i++){
		if(!expandedObjects[i].flag.blocal) continue;
		
		// ���݂̑��x��ۑ�
		SpatialVector currentvel, nextvel; 
		currentvel.v() = expandedObjects[i].phSolidIf->GetVelocity();											// ���݂̑��x
		currentvel.w() = expandedObjects[i].phSolidIf->GetAngularVelocity();									// ���݂̊p���x									
		Vec3d cPoint = expandedObjects[i].phSolidIf->GetPose() * expandedObjects[i].syncInfo.neighborPoint.closestPoint;	// �͂�������_
		const float minTestForce = 0.5;
		if(expandedObjects[i].syncInfo.neighborPoint.test_force_norm < minTestForce){
			expandedObjects[i].syncInfo.neighborPoint.test_force_norm = minTestForce;		// �e�X�g�͂�0�Ȃ�1�ɂ��� 
		}

		// �S�����W�n����邽�߂̏���
		Vec3d rpjabs, vpjabs;
		rpjabs = cPoint - soPointer->GetCenterPosition();																							//�͊o�|�C���^�̒��S����ڐG�_�܂ł̃x�N�g��
		vpjabs = soPointer->GetVelocity() + soPointer->GetAngularVelocity() % rpjabs;													//�ڐG�_�ł̑��x
		Vec3d rjabs, vjabs;
		rjabs = cPoint - expandedObjects[i].phSolidIf->GetCenterPosition();																	//���̂̒��S����ڐG�_�܂ł̃x�N�g��
		vjabs = expandedObjects[i].phSolidIf->GetVelocity() + expandedObjects[i].phSolidIf->GetAngularVelocity() % rjabs;	//�ڐG�_�ł̑��x

		//�ڐ��x�N�g��t[0], t[1] (t[0]�͑��Α��x�x�N�g���ɕ��s�ɂȂ�悤�ɂ���)
		Vec3d n, t[2], vjrel, vjrelproj;
		n = -expandedObjects[i].syncInfo.neighborPoint.face_normal;
		vjrel = vjabs - vpjabs;										// ���Α��x
		vjrelproj = vjrel - (n * vjrel) * n;						// ���Α��x�x�N�g����@���ɒ������镽�ʂɎˉe�����x�N�g��
		double vjrelproj_norm = vjrelproj.norm();
		if(vjrelproj_norm < 1.0e-10){							// �ˉe�x�N�g���̃m�������������Ƃ�
			t[0] = n % Vec3d(1.0, 0.0, 0.0);					// t[0]��@����Vec3d(1.0, 0.0, 0.0)�̊O�ςƂ���
			if(t[0].norm() < 1.0e-10)								// ����ł��m������������������
				t[0] = n % Vec3d(0.0, 1.0, 0.0);				// t[0]��@����Vec3d(0.0, 1.0, 0.0)�̊O�ςƂ���
			t[0].unitize();												// t[0]��P�ʃx�N�g���ɂ���
		}
		else{
			t[0] = vjrelproj / vjrelproj_norm;					// �m�������������Ȃ�������C�ˉe�x�N�g���̂܂�
		}
		t[1] = n % t[0];												// t[1]�͖@����t[0]�̊O�ςł��܂�

		// �����͂������Ȃ��ŃV�~�����[�V������1�X�e�b�v�i�߂�
#ifdef DIVIDE_STEP
		phscene->IntegratePart2();
#else
		phscene->Step();
#endif
		nextvel.v() = expandedObjects[i].phSolidIf->GetVelocity();
		nextvel.w() = expandedObjects[i].phSolidIf->GetAngularVelocity();
		expandedObjects[i].syncInfo.motionCoeff.lastb = expandedObjects[i].syncInfo.motionCoeff.b;
		expandedObjects[i].syncInfo.motionCoeff.b = (nextvel - currentvel) / dt;

		TMatrixRow<6, 3, double> u;
		TMatrixRow<3, 3, double> force;
		// �@�������ɗ͂�������
		states->LoadState(phscene);
		force.col(0) = expandedObjects[i].syncInfo.neighborPoint.test_force_norm * n;
		expandedObjects[i].phSolidIf->AddForce(force.col(0), cPoint);
#ifdef DIVIDE_STEP
		phscene->IntegratePart2();
#else
		phscene->Step();
#endif
		nextvel.v() = expandedObjects[i].phSolidIf->GetVelocity();
		nextvel.w() = expandedObjects[i].phSolidIf->GetAngularVelocity();
		u.col(0) = (nextvel - currentvel) /dt - expandedObjects[i].syncInfo.motionCoeff.b;

		// n + t[0]�����ɗ͂�������
		states->LoadState(phscene);
		force.col(1) = expandedObjects[i].syncInfo.neighborPoint.test_force_norm * (n + t[0]);
		expandedObjects[i].phSolidIf->AddForce(force.col(1), cPoint);
		phscene->IntegratePart2();
		nextvel.v() = expandedObjects[i].phSolidIf->GetVelocity();
		nextvel.w() = expandedObjects[i].phSolidIf->GetAngularVelocity();
		u.col(1) = (nextvel - currentvel) /dt - expandedObjects[i].syncInfo.motionCoeff.b;

		// n+t[1]�����͂�������
		states->LoadState(phscene);
		force.col(2) = expandedObjects[i].syncInfo.neighborPoint.test_force_norm * (n + t[1]);
		expandedObjects[i].phSolidIf->AddForce(force.col(2), cPoint);
#ifdef DIVIDE_STEP
		phscene->IntegratePart2();
#else
		phscene->Step();
#endif
		nextvel.v() = expandedObjects[i].phSolidIf->GetVelocity();
		nextvel.w() = expandedObjects[i].phSolidIf->GetAngularVelocity();
		u.col(2) = (nextvel - currentvel) /dt - expandedObjects[i].syncInfo.motionCoeff.b;
		
		expandedObjects[i].syncInfo.motionCoeff.A = u  * force.inv();				// �^���W��A�̌v�Z
		states->LoadState(phscene);								// ����state�ɖ߂��V�~�����[�V������i�߂�
	}
#ifdef DIVIDE_STEP
		states2->LoadState(phscene);								// ����state�ɖ߂��V�~�����[�V������i�߂�
#endif
}

void PhysicsProcess::DisplayContactPlane(){
	for(unsigned int i = 0; i <  expandedObjects.size(); i++){
		if(!expandedObjects[i].flag.blocal) continue;
		Vec3d pPoint = soPointer->GetPose() * expandedObjects[i].syncInfo.neighborPoint.pointerPoint;
		Vec3d cPoint = expandedObjects[i].phSolidIf->GetPose() * expandedObjects[i].syncInfo.neighborPoint.closestPoint;
		Vec3d normal = expandedObjects[i].syncInfo.neighborPoint.face_normal;
		Vec3d v1(0,1,0);

		v1 +=  Vec3d(0, 0, 0.5) - Vec3d(0, 0, 0.5)*normal*normal;
		v1 -= v1*normal * normal;
		v1.unitize();
		Vec3d v2 = normal ^ v1;

		Vec4f moon(1.0, 1.0, 0.8, 0.3);
		GRDebugRenderIf* render = window->render->Cast();
		render->SetMaterial( GRMaterialDesc(moon) );
		render->PushModelMatrix();
		Vec3d offset = 0.02 * normal;
		render->SetLighting( false );
		render->SetAlphaTest(true);
		render->SetAlphaMode(render->BF_SRCALPHA, render->BF_ONE);
		cPoint += offset/2;
		glBegin(GL_QUADS);
			// �ڐG�ʒ��	
			glVertex3d(cPoint[0] + v1[0] + v2[0], cPoint[1] + v1[1] + v2[1], cPoint[2] + v1[2] + v2[2]);
			glVertex3d(cPoint[0] - v1[0] + v2[0], cPoint[1] - v1[1] + v2[1], cPoint[2] - v1[2] + v2[2]);
			glVertex3d(cPoint[0] - v1[0] - v2[0], cPoint[1] - v1[1] - v2[1], cPoint[2] - v1[2] - v2[2]);
			glVertex3d(cPoint[0] + v1[0] - v2[0], cPoint[1] + v1[1] - v2[1], cPoint[2] + v1[2] - v2[2]);
			// ����1
			glVertex3d(cPoint[0] + v1[0] + v2[0] + offset[0], 
							cPoint[1] + v1[1] + v2[1] + offset[1], 
							cPoint[2] + v1[2] + v2[2] + offset[2]);
			glVertex3d(cPoint[0] + v1[0] + v2[0], 
							cPoint[1] + v1[1] + v2[1], 
							cPoint[2] + v1[2] + v2[2]);
			glVertex3d(cPoint[0] - v1[0] + v2[0], 
							cPoint[1] - v1[1] + v2[1], 
							cPoint[2] - v1[2] + v2[2]);
			glVertex3d(cPoint[0] - v1[0] + v2[0] + offset[0], 
							cPoint[1] - v1[1] + v2[1] + offset[1], 
							cPoint[2] - v1[2] + v2[2] + offset[2]);
			// ����2
			glVertex3d(cPoint[0] - v1[0] + v2[0] + offset[0], 
							cPoint[1] - v1[1] + v2[1] + offset[1], 
							cPoint[2] - v1[2] + v2[2] + offset[2]);
			glVertex3d(cPoint[0] - v1[0] + v2[0], 
							cPoint[1] - v1[1] + v2[1], 
							cPoint[2] - v1[2] + v2[2]);
			glVertex3d(cPoint[0] - v1[0] - v2[0], 
							cPoint[1] - v1[1] - v2[1], 
							cPoint[2] - v1[2] - v2[2]);
			glVertex3d(cPoint[0] - v1[0] - v2[0] + offset[0], 
							cPoint[1] - v1[1] - v2[1] + offset[1], 
							cPoint[2] - v1[2] - v2[2] + offset[2]);
			// ����3
			glVertex3d(cPoint[0] - v1[0] - v2[0] + offset[0], 
							cPoint[1] - v1[1] - v2[1] + offset[1], 
							cPoint[2] - v1[2] - v2[2] + offset[2]);
			glVertex3d(cPoint[0] - v1[0] - v2[0], 
							cPoint[1] - v1[1] - v2[1], 
							cPoint[2] - v1[2] - v2[2]);
			glVertex3d(cPoint[0] + v1[0] - v2[0], 
							cPoint[1] + v1[1] - v2[1], 
							cPoint[2] + v1[2] - v2[2]);
			glVertex3d(cPoint[0] + v1[0] - v2[0] + offset[0], 
							cPoint[1] + v1[1] - v2[1] + offset[1], 
							cPoint[2] + v1[2] - v2[2] + offset[2]);
			// ����4
			glVertex3d(cPoint[0] + v1[0] - v2[0] + offset[0], 
							cPoint[1] + v1[1] - v2[1] + offset[1], 
							cPoint[2] + v1[2] - v2[2] + offset[2]);
			glVertex3d(cPoint[0] + v1[0] - v2[0], 
							cPoint[1] + v1[1] - v2[1], 
							cPoint[2] + v1[2] - v2[2]);
			glVertex3d(cPoint[0] + v1[0] + v2[0], 
							cPoint[1] + v1[1] + v2[1],
							cPoint[2] + v1[2] + v2[2]);
			glVertex3d(cPoint[0] + v1[0] + v2[0] + offset[0], 
							cPoint[1] + v1[1] + v2[1] + offset[1], 
							cPoint[2] + v1[2] + v2[2] + offset[2]);
			// �ڐG�ʏ��
			glVertex3d(cPoint[0] - v1[0] + v2[0] + offset[0], 
							cPoint[1] - v1[1] + v2[1] + offset[1], 
							cPoint[2] - v1[2] + v2[2] + offset[2]);
			glVertex3d(cPoint[0] + v1[0] + v2[0] + offset[0], 
							cPoint[1] + v1[1] + v2[1] + offset[1], 
							cPoint[2] + v1[2] + v2[2] + offset[2]);
			glVertex3d(cPoint[0] + v1[0] - v2[0] + offset[0], 
							cPoint[1] + v1[1] - v2[1] + offset[1], 
							cPoint[2] + v1[2] - v2[2] + offset[2]);
			glVertex3d(cPoint[0] - v1[0] - v2[0] + offset[0], 
							cPoint[1] - v1[1] - v2[1] + offset[1], 
							cPoint[2] - v1[2] - v2[2] + offset[2]);
		glEnd();
		render->SetLighting( true);
		render->SetAlphaTest(false);
		render->PopModelMatrix();
		glEnable(GL_DEPTH_TEST);
	}
}

void PhysicsProcess::DisplayLineToNearestPoint(){
	GLfloat moon[]={0.8,0.8,0.8};
	for(unsigned int i = 0; i <  expandedObjects.size(); i++){
		if(!expandedObjects[i].flag.blocal) continue;
		Vec3d pPoint = soPointer->GetPose() * expandedObjects[i].syncInfo.neighborPoint.pointerPoint;
		Vec3d cPoint = expandedObjects[i].phSolidIf->GetPose() * expandedObjects[i].syncInfo.neighborPoint.closestPoint;
		Vec3d normal = expandedObjects[i].syncInfo.neighborPoint.face_normal;
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, moon);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, moon);
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, moon);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, moon);
		glDisable(GL_DEPTH_TEST);
		glBegin(GL_LINES);
		glVertex3f(pPoint.X() + normal[0], pPoint.Y() + normal[1], pPoint.Z() + normal[2]);
		glVertex3f(cPoint.X(), cPoint.Y(), cPoint.Z());
		glEnd();
		glEnable(GL_DEPTH_TEST);
	}
}

void PhysicsProcess::Keyboard(unsigned char key){
	PHSceneIf* phscene = GetSdk()->GetScene()->GetPHScene();
	states->ReleaseState(phscene);
	states2->ReleaseState(phscene);
	switch (key) {
		case 'w':
			InitCameraView();
			DSTR << "InitCameraView" << endl;
			break;
		case 'd':
			if(bDebug){
				bDebug = false;
				DSTR << "Debug Mode OFF" << endl;
			}else{
				bDebug = true;
				DSTR << "Debug Mode ON" << endl;
			}
			break;
		case 'g':
			if(bGravity){
				bGravity = false;
				Vec3d zeroG = Vec3d(0.0, 0.0, 0.0);
				phscene->SetGravity(zeroG);
				DSTR << "Gravity OFF" << endl;
			}else{
				bGravity = true;
				phscene->SetGravity(gravity);
				DSTR << "Gravity ON" << endl;
			}
			break;
		case 's':
			bStep = false;
			bOneStep = true;
			break;
		case 'a':
			bStep = true;
			break;
		case ' ':
			{
				// ������
				PHSolidDesc desc;
				CDBoxDesc bd;

				desc.mass = 0.05;
				desc.inertia = 0.0333 * Matrix3d::Unit();

				PHSolidIf* soBox = phscene->CreateSolid(desc);
				bd.boxsize = Vec3f(2, 2, 2);
				CDBoxIf* shapeBox = XCAST(GetSdk()->GetPHSdk()->CreateShape(bd));
				shapeBox->SetName("shapeBox");

				soBox->AddShape(shapeBox);
				soBox->SetFramePosition(Vec3d(-1, 5, 4));
				//soBox.back()->SetOrientation(
				//	Quaternionf::Rot(Rad(30), 'y') * 
				//	Quaternionf::Rot(Rad(10), 'x'));  
				soBox->GetShape(0)->SetVibration(-200,120,300);
				soBox->GetShape(0)->SetStaticFriction(1);//0.8);
				soBox->GetShape(0)->SetDynamicFriction(1);//0.6);
				phscene->SetContactMode(soPointer, PHSceneDesc::MODE_NONE);
			}
			break;
		default:
			break;
	}
}