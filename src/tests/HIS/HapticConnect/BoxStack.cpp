#include "BoxStack.h"
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

BoxStack bstack;

BoxStack::BoxStack(){
	bsync=false;
	calcPhys=true;
	dt = 0.02;//0.05;
	gravity =  Vec3d(0, -9.8f , 0);
	nIter = 15;
	bGravity = true;
	phscene = NULL;
	render = NULL;
	range = 1.5;
	bDebug = true;
	neighborObjects.clear();
}
namespace Spr{
void FASTCALL ContFindCommonPointSaveParam(const CDConvex* a, const CDConvex* b,
	const Posed& a2w, const Posed& b2w, const Vec3d& dir, double start, double end,
	Vec3d& normal, Vec3d& pa, Vec3d& pb, double& dist);
}

void BoxStack::Init(int argc, char* argv[]){
	FWAppGLUT::Init(argc, argv);

	GetSdk()->Clear();															// SDK�̍쐬
	GetSdk()->CreateScene(PHSceneDesc(), GRSceneDesc());		// Scene�̍쐬
	phscene = GetSdk()->GetScene()->GetPHScene();
	states = ObjectStatesIf::Create();
	states2 = ObjectStatesIf::Create();

	DesignObject();																// ���̂��쐬

	phscene->SetGravity(gravity);				
	phscene->SetTimeStep(dt);
	phscene->SetNumIteration(nIter);

	FWWinDesc windowDesc;
	windowDesc.title = "HapticConnect1.2";
	window = CreateWin(windowDesc);
	window->scene = GetSdk()->GetScene();

	InitCameraView();
}

void BoxStack::InitCameraView(){
	cameraInfo.view[0][0] = 0.9996;
	cameraInfo.view[0][1] = 0.0107463;
	cameraInfo.view[0][2] = -0.0261432;
	cameraInfo.view[0][3] = -0.389004;
	cameraInfo.view[1][0] = -6.55577e-010;
	cameraInfo.view[1][1] = 0.924909;
	cameraInfo.view[1][2] = 0.380188;
	cameraInfo.view[1][3] = 5.65711;
	cameraInfo.view[2][0] = 0.0282657;
	cameraInfo.view[2][1] = -0.380037;
	cameraInfo.view[2][2] = 0.92454;
	cameraInfo.view[2][3] = 13.7569;
	cameraInfo.view[3][0] = 0;
	cameraInfo.view[3][1] = 0;
	cameraInfo.view[3][2] = 0;
	cameraInfo.view[3][3] = 1;
}

void BoxStack::DesignObject(){
	// soFloor�p��desc
	phscene->SetStateMode(true);
	desc.mass = 1e20f;
	desc.inertia *= 1e30f;
	soFloor = phscene->CreateSolid(desc);		// ���̂�desc�Ɋ�Â��č쐬
	soFloor->SetDynamical(false);
	soFloor->SetGravity(false);
	
	// soBox�p��desc
	desc.mass = 0.05;
	desc.inertia = 0.033 * Matrix3d::Unit();
	{
		CDBoxDesc bd;
		bd.boxsize = Vec3f(2,2,2)*2.5  ;
		meshBox = XCAST(GetSdk()->GetPHSdk()->CreateShape(bd));
		meshBox->SetName("meshBox");
		CDSphereDesc sd;
		sd.radius = 1.2;
		meshSphere = XCAST(GetSdk()->GetPHSdk()->CreateShape(sd));
		meshSphere->SetName("meshSphere");
		CDCapsuleDesc cd;
		cd.radius = 1;
		cd.length = 1;
		meshCapsule = XCAST(GetSdk()->GetPHSdk()->CreateShape(cd));
		meshCapsule->SetName("meshCapsule");
	}

	{
		// meshConvex(soBox)�̃��b�V���`��
		CDConvexMeshDesc md;
		md.vertices.push_back(Vec3d(-1,-1,-1));
		md.vertices.push_back(Vec3d(-1,-1, 1));	
		md.vertices.push_back(Vec3d(-1, 1,-1));	
		md.vertices.push_back(Vec3d(-1, 1, 1));
		md.vertices.push_back(Vec3d( 1,-1,-1));	
		md.vertices.push_back(Vec3d( 1,-1, 1));
		md.vertices.push_back(Vec3d( 1, 1,-1));
		md.vertices.push_back(Vec3d( 1, 1, 1));
		meshConvex = DCAST(CDConvexMeshIf, GetSdk()->GetPHSdk()->CreateShape(md));
		meshConvex->SetName("meshConvex");

		// meshFloor(soFloor)�̃��b�V���`��
		for(unsigned i=0; i<md.vertices.size(); ++i){
			md.vertices[i].x *= 30;
			md.vertices[i].z *= 20;
			md.vertices[i].y *= 20;
		}
		meshFloor = DCAST(CDConvexMeshIf, GetSdk()->GetPHSdk()->CreateShape(md));
		meshFloor->SetName("meshFloor");
	}
	soFloor->AddShape(meshFloor);
	soFloor->SetFramePosition(Vec3d(0,-20 - 0.7,0));
	soFloor->SetName("solidFloor");

	// �͊o�|�C���^
	CDSphereDesc sd;
	sd.radius = 0.5;//1.0;
	CDSphereIf* sphere = DCAST(CDSphereIf,  GetSdk()->GetPHSdk()->CreateShape(sd));
	soPointer = phscene->CreateSolid(desc);
	soPointer->AddShape(sphere);//meshConvex);
	soPointer->SetFramePosition(Vec3d(0, 3.0, 0));  
	soPointer->SetDynamical(false);
	// �͊o�|�C���^��springhead2�̐ڐG�v�Z����O��
	// ���̂������邽�т�ContactMode��NONE�ɂ��Ȃ���΂����Ȃ�(���������̂ƐڐG�v�Z�����Ă��܂�����)
	phscene->SetContactMode(soPointer, PHSceneDesc::MODE_NONE);
}

void BoxStack::Idle(){
	PhysicsStep();
}
void BoxStack::Step(){
}
void BoxStack::PhysicsStep(){
	if (bsync) return;
	if (calcPhys){
		UpdateHapticPointer();
		for(unsigned i = 0; i < neighborObjects.size(); i++){
			if(!neighborObjects[i].blocal) continue;
			neighborObjects[i].lastvel.v() = neighborObjects[i].phSolidIf->GetVelocity();
			neighborObjects[i].lastvel.w() = neighborObjects[i].phSolidIf->GetAngularVelocity();
		}
		phscene->Step();

		for(unsigned i = 0; i < neighborObjects.size(); i++){
			if(!neighborObjects[i].blocal) continue;
			SpatialVector curvel;
			curvel.v() = neighborObjects[i].phSolidIf->GetVelocity();
			curvel.w() = neighborObjects[i].phSolidIf->GetAngularVelocity();
			neighborObjects[i].curb = (curvel - neighborObjects[i].lastvel) / dt;
		}
		FindNearestObject();	// �ߖT���̂̎擾
		PredictSimulation();
		glutPostRedisplay();
		calcPhys = false;
	}
	if (hapticcount < dt/hprocess.dt) return;
	hapticcount -= dt/hprocess.dt;
	bsync = true;
	calcPhys = true;

	static DWORD last;
	//if (phscene->GetCount() % 10==0){
	//	DWORD t = timeGetTime();
	//	DSTR << phscene->GetCount() << " dt:" << t-last << std::endl;
	//	last = t;
	//}
}

void BoxStack::Display(){
	// �`��̐ݒ�
	GetSdk()->SetDebugMode(true);
	render = window->render->Cast();

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
	//	DrawHapticSolids();
	}
	render->PopLight();	//	�����̍폜

	curRender->EndScene();
	glutSwapBuffers();
}

void BoxStack::UpdateHapticPointer(){
	// cameraInfo.view.Rot()�������ė͊o�|�C���^�̑�����J��������]�ɂ��킹��
	soPointer->SetFramePosition(phpointer.GetFramePosition());//cameraInfo.view.Rot() * phpointer.GetFramePosition());				
	soPointer->SetOrientation(phpointer.GetOrientation());					
	//soPointer->SetVelocity(phpointer.GetVelocity());
	//soPointer->SetAngularVelocity(phpointer.GetAngularVelocity());	
	soPointer->SetDynamical(false);
};

void BoxStack::FindNearestObject(){
	// GJK���g���ċߖT���̂ƋߖT���̂̍ŋߓ_���擾
	// ��������ׂĂ�shape�������̂ɂ��Ă��

	// �V�[���ŐV���ɍ��̂��������ꂽ�烍�[�J���ŃV�~�����[�V�������Ă��邩�ǂ����̏�������sceneSolids�Ɋi�[����
	// sceneSolids�͗͊o�|�C���^�������Ă���
	sceneSolids.clear();
	PHSolidIf** solids = phscene->GetSolids();
	for(int i = 0; i < phscene->NSolids(); i++){
		sceneSolids.resize(sceneSolids.size() + 1);
		sceneSolids.back().phSolidIf = solids[i];
		sceneSolids.back().bneighbor = false;
		sceneSolids.back().blocal = false;
	}

	// sceneSolids�ŐV��������������neighborObjects�ɒǉ�����
	//neighborObjects����������clear���Ă�ƁC�̂�neighborObjects��ۑ�����K�v������̂� 
	//����neighborObjects��scene�̂��ׂĂ�solid���i�[���Ă���D
	//neighborObjects�͗͊o�|�C���^������blocal�����false�ɂ��Ă����K�v������D
	for(unsigned i = (int)neighborObjects.size(); i < sceneSolids.size(); i++){
		neighborObjects.resize(i + 1);
		neighborObjects.back().phSolidIf = sceneSolids[i].phSolidIf;
		neighborObjects.back().bneighbor = false;
		neighborObjects.back().blocal = false;
	}

#if 0		
	// AABB�ŗ͊o�|�C���^�ߖT�̕��̂��i��i�������j
	// �����ōi�������̂ɂ���GJK���s���D�����ōi�邱�Ƃ�GJK������񐔂����Ȃ��ł���D
	// Solid��BBox���x���ł̌�������(z���\�[�g)�D�����̂�����̖����g�����O		
	//1. BBox���x���̏Փ˔���
	int N = neighborObjects.size();
	Vec3f dir(0,0,1);
	Edges edges;
	edges.resize(2 * N);
	Edges::iterator eit = edges.begin();
	for(int i = 0; i < N; ++i){
		// ���[�J����������ׂ�false�ɂ���
		neighborObjects[i].bneighbor = false;
		DCAST(PHSolid, neighborObjects[i].phSolidIf)->GetBBoxSupport(dir, eit[0].edge, eit[1].edge);
		Vec3d dPos = neighborObjects[i].phSolidIf->GetDeltaPosition();
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
				if(neighborObjects[f1].phSolidIf == soPointer){
					neighborObjects[f2].bneighbor = true;
				}else if(neighborObjects[f2].phSolidIf == soPointer){
					neighborObjects[f1].bneighbor = true;
				}
			}
			cur.insert(it->index);
		}else{
			cur.erase(it->index);			//	�I�[�Ȃ̂ō폜�D
		}
	}

	for(int i = 0; i < neighborObjects.size(); i++){
		if(neighborObjects[i].bneighbor) continue;
		neighborObjects[i].bfirstlocal = false;			//�ߖT���̂łȂ��̂�false�ɂ���
		neighborObjects[i].blocal = false;
	}
#else
	// ���ׂĂ̕��̂ɂ���GJK���s��
	for(int i = 0; i < neighborObjects.size(); i++){
		neighborObjects[i].bneighbor = true;
		if(neighborObjects[i].phSolidIf == soPointer) neighborObjects[i].bneighbor = false;
	}
#endif

	// GJK�ŋߖT�_�����߁C�͊o�|�C���^�ŋߖT�̕��̂����肷��
	// �ŋߖT���̂�������blocal��true�ɂ��CphSolid��phSolidIf���R�s�[����
	// blocal�����ł�true��������R�s�[�ς݂Ȃ̂ŋߖT�_�����R�s�[����
	for(unsigned i = 0; i < (int)neighborObjects.size(); i++){
		if(!neighborObjects[i].bneighbor) continue;															// �ߖT�łȂ���Ύ���
		CDConvex* a = DCAST(CDConvex, neighborObjects[i].phSolidIf->GetShape(0));		// ���̂����ʌ`��
		CDConvex* b = DCAST(CDConvex, soPointer->GetShape(0));									// �͊o�|�C���^�̓ʌ`��
		Posed a2w, b2w;																								// ���̂̃��[���h���W
/*		if(neighborObjects[i].blocal) 	a2w = neighborObjects[i].phSolid.GetPose();				// blocal��true�Ȃ�ŐV�̏��ł��
		else */								a2w = neighborObjects[i].phSolidIf->GetPose();
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
				Vec3d dir = -1.0 * neighborObjects[i].face_normal;
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
			if(!neighborObjects[i].blocal){																			// ���߂čŋߖT���̂ɂȂ�����
				neighborObjects[i].bfirstlocal = true;
				neighborObjects[i].phSolid = *DCAST(PHSolid, neighborObjects[i].phSolidIf);		// �V�[���������̂̒��g��͊o�v���Z�X�Ŏg�����́i���́j�Ƃ��ăR�s�[����
				neighborObjects[i].face_normal = normal;														// ���߂čŋߖT���̂ɂȂ����̂ŁC�O��̖@���͎g��Ȃ��D										
			}
			neighborObjects[i].blocal = true;																		// �ŋߖT���̂Ȃ̂�blocal��true�ɂ���
			neighborObjects[i].closestPoint = pa;																	// ���̋ߖT�_�̃��[�J�����W
			neighborObjects[i].pointerPoint = pb;																// �͊o�|�C���^�ߖT�_�̃��[�J�����W
			neighborObjects[i].last_face_normal = neighborObjects[i].face_normal;					// �O��̖@���i�@���̕�ԂɎg���j
			neighborObjects[i].face_normal = normal;															// ���̂���͊o�|�C���^�ւ̖@��
		}else{
			neighborObjects[i].blocal = false;																		// �ŋߖT���̂ł͂Ȃ��̂�blocal��false�ɂ���
			neighborObjects[i].bfirstlocal = false;
		}
	}
}

//#define DIVIDE_STEP

void BoxStack::PredictSimulation(){
	// neighborObjets��blocal��true�̕��̂ɑ΂��ĒP�ʗ͂������C�ڐG���Ă��邷�ׂĂ̕��̂ɂ��āC�^���W�����v�Z����
#ifdef DIVIDE_STEP
	states2->SaveState(phscene);			// �\���V�~�����[�V�����̂��߂Ɍ��݂̍��̂̏�Ԃ�ۑ�����
	//	LCP�̒��O�܂ŃV�~�����[�V�������Ă��̏�Ԃ�ۑ�
	phscene->ClearForce();
	phscene->GenerateForce();
	phscene->IntegratePart1();
#endif
	states->SaveState(phscene);			// �\���V�~�����[�V�����̂��߂Ɍ��݂̍��̂̏�Ԃ�ۑ�����

	for(unsigned i = 0; i < neighborObjects.size(); i++){
		if(!neighborObjects[i].blocal) continue;
		
		// ���݂̑��x��ۑ�
		SpatialVector currentvel, nextvel; 
		currentvel.v() = neighborObjects[i].phSolidIf->GetVelocity();											// ���݂̑��x
		currentvel.w() = neighborObjects[i].phSolidIf->GetAngularVelocity();									// ���݂̊p���x									
		Vec3d cPoint = neighborObjects[i].phSolidIf->GetPose() * neighborObjects[i].closestPoint;	// �͂�������_
		if(neighborObjects[i].test_force_norm == 0)	neighborObjects[i].test_force_norm = 1;		// �e�X�g�͂�0�Ȃ�1�ɂ���  

		// �S�����W�n����邽�߂̏���
		Vec3d rpjabs, vpjabs;
		rpjabs = cPoint - soPointer->GetCenterPosition();																							//�͊o�|�C���^�̒��S����ڐG�_�܂ł̃x�N�g��
		vpjabs = soPointer->GetVelocity() + soPointer->GetAngularVelocity() % rpjabs;													//�ڐG�_�ł̑��x
		Vec3d rjabs, vjabs;
		rjabs = cPoint - neighborObjects[i].phSolidIf->GetCenterPosition();																	//���̂̒��S����ڐG�_�܂ł̃x�N�g��
		vjabs = neighborObjects[i].phSolidIf->GetVelocity() + neighborObjects[i].phSolidIf->GetAngularVelocity() % rjabs;	//�ڐG�_�ł̑��x

		//�ڐ��x�N�g��t[0], t[1] (t[0]�͑��Α��x�x�N�g���ɕ��s�ɂȂ�悤�ɂ���)
		Vec3d n, t[2], vjrel, vjrelproj;
		n = -neighborObjects[i].face_normal;
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
		nextvel.v() = neighborObjects[i].phSolidIf->GetVelocity();
		nextvel.w() = neighborObjects[i].phSolidIf->GetAngularVelocity();
		neighborObjects[i].lastb = neighborObjects[i].b;
		neighborObjects[i].b = (nextvel - currentvel) / dt;

		TMatrixRow<6, 3, double> u;
		TMatrixRow<3, 3, double> force;
		// �@�������ɗ͂�������
		states->LoadState(phscene);
		force.col(0) = neighborObjects[i].test_force_norm * n;
		neighborObjects[i].phSolidIf->AddForce(force.col(0), cPoint);
#ifdef DIVIDE_STEP
		phscene->IntegratePart2();
#else
		phscene->Step();
#endif
		nextvel.v() = neighborObjects[i].phSolidIf->GetVelocity();
		nextvel.w() = neighborObjects[i].phSolidIf->GetAngularVelocity();
		u.col(0) = (nextvel - currentvel) /dt - neighborObjects[i].b;

		// n + t[0]�����ɗ͂�������
		states->LoadState(phscene);
		force.col(1) = neighborObjects[i].test_force_norm * (n + t[0]);
		neighborObjects[i].phSolidIf->AddForce(force.col(1), cPoint);
		phscene->IntegratePart2();
		nextvel.v() = neighborObjects[i].phSolidIf->GetVelocity();
		nextvel.w() = neighborObjects[i].phSolidIf->GetAngularVelocity();
		u.col(1) = (nextvel - currentvel) /dt - neighborObjects[i].b;

		// n+t[1]�����͂�������
		states->LoadState(phscene);
		force.col(2) = neighborObjects[i].test_force_norm * (n + t[1]);
		neighborObjects[i].phSolidIf->AddForce(force.col(2), cPoint);
#ifdef DIVIDE_STEP
		phscene->IntegratePart2();
#else
		phscene->Step();
#endif
		nextvel.v() = neighborObjects[i].phSolidIf->GetVelocity();
		nextvel.w() = neighborObjects[i].phSolidIf->GetAngularVelocity();
		u.col(2) = (nextvel - currentvel) /dt - neighborObjects[i].b;
		
		neighborObjects[i].A = u  * force.inv();				// �^���W��A�̌v�Z
#ifdef DIVIDE_STEP
		states2->LoadState(phscene);								// ����state�ɖ߂��V�~�����[�V������i�߂�
#else
		states->LoadState(phscene);								// ����state�ɖ߂��V�~�����[�V������i�߂�
#endif
	}
}

void BoxStack::DisplayContactPlane(){
	GLfloat moon[] = {0.8,0.8,0.8};
	for(unsigned int i = 0; i <  neighborObjects.size(); i++){
		if(!neighborObjects[i].blocal) continue;
		Vec3d pPoint = soPointer->GetPose() * neighborObjects[i].pointerPoint;
		Vec3d cPoint = neighborObjects[i].phSolidIf->GetPose() * neighborObjects[i].closestPoint;
		Vec3d normal = neighborObjects[i].face_normal;
		Vec3d v1;
		v1[0] = 1 / normal[0];
		v1[1] = -1 / normal[1];
		v1[2] = 1 / normal[2];
		Vec3d v2;
		// �@���x�N�g���̌����ɂ���ĊO�ς̏��Ԃ��l���Ȃ��Ɩʂ����܂�����Ȃ��D
		if(normal[2]>0) v2 = normal % v1;
		else v2 = v1 % normal;		
		v1 = v1.unit();
		v2 = v2.unit();
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, moon);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, moon);
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, moon);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, moon);
		render->PushModelMatrix();
		Vec3d offset = 0.02 * normal;
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
		render->PopModelMatrix();
		glEnable(GL_DEPTH_TEST);
	}
}

void BoxStack::DisplayLineToNearestPoint(){
	GLfloat moon[]={0.8,0.8,0.8};
	for(unsigned int i = 0; i <  neighborObjects.size(); i++){
		if(!neighborObjects[i].blocal) continue;
		Vec3d pPoint = soPointer->GetPose() * neighborObjects[i].pointerPoint;
		Vec3d cPoint = neighborObjects[i].phSolidIf->GetPose() * neighborObjects[i].closestPoint;
		Vec3d normal = neighborObjects[i].face_normal;
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

void BoxStack::DrawHapticSolids(){
	GLfloat purple[] = {1.0, 0.0, 1.0, 0.0};
	GRDebugRenderIf* render = GetCurrentWin()->GetRender()->Cast();
	render->SetMaterialSample(GRDebugRenderIf::GRAY);
	for(unsigned int i = 0; i < hapticsolids.size(); i++){
		PHSolid* solid = &hapticsolids[i];		
		PHSolidIf* solidIf = solid->Cast(); 
		render->DrawSolid(solidIf);
	}
};


void BoxStack::Keyboard(unsigned char key){
	states->ReleaseState(phscene);
	states2->ReleaseState(phscene);
	switch (key) {
		case 'a':
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
		case ' ':
			{
				// ConvexBox
				desc.mass = 0.08;
				desc.inertia = 0.033 * Matrix3d::Unit() * 10;
				//desc.dynamical = false;
				soBox.push_back(phscene->CreateSolid(desc));
				soBox.back()->AddShape(meshBox);
				soBox.back()->SetFramePosition(Vec3d(-1, 5, 4));
				//soBox.back()->SetOrientation(
				//	Quaternionf::Rot(Rad(30), 'y') * 
				//	Quaternionf::Rot(Rad(10), 'x'));  
				ostringstream os;
				os << "box" << (unsigned int)soBox.size();
				soBox.back()->SetName(os.str().c_str());
			}
			phscene->SetContactMode(soPointer, PHSceneDesc::MODE_NONE);
			DSTR << "Create Box" << endl;
			DSTR << "NSolids		" <<  phscene->NSolids() << endl;
			break;
		case 'v':
			{
				// MeshCapsule
				desc.mass = 0.05;
				desc.inertia = 0.001 * Matrix3d::Unit();
				soBox.push_back(phscene->CreateSolid(desc));
				soBox.back()->SetAngularVelocity(Vec3f(0,0,0.2));
				soBox.back()->AddShape(meshCapsule);
				soBox.back()->SetFramePosition(Vec3f(0.5, 20,0));
	//				soBox.back()->SetFramePosition(Vec3f(0.5, 10+3*soBox.size(),0));
				soBox.back()->SetOrientation(Quaternionf::Rot(Rad(30), 'y'));  
				ostringstream os;
				os << "capsule" << (unsigned int)soBox.size();
				soBox.back()->SetName(os.str().c_str());
				phscene->SetContactMode(soPointer, PHSceneDesc::MODE_NONE);
			}
			break;
		case 'b':
			{
				// MeshSphere
				desc.mass = 0.05;
				desc.inertia = 0.001 * Matrix3d::Unit();
				soBox.push_back(phscene->CreateSolid(desc));
				soBox.back()->AddShape(meshSphere);
//				soBox.back()->SetFramePosition(Vec3f(0.5, 10+3*soBox.size(),0));
				soBox.back()->SetFramePosition(Vec3f(0.5, 20,0));
				soBox.back()->SetOrientation(Quaternionf::Rot(Rad(30), 'y'));  
				ostringstream os;
				os << "sphere" << (unsigned int)soBox.size();
				soBox.back()->SetName(os.str().c_str());
				phscene->SetContactMode(soPointer, PHSceneDesc::MODE_NONE);
			}
			break;
		case 'n':
			{
				// ConvexMesh
				desc.mass = 0.1;
				desc.inertia = 0.1 * Matrix3d::Unit();
				soBox.push_back(phscene->CreateSolid(desc));
				CDConvexMeshDesc md;
				int nv = rand() % 100 + 50;
				for(int i=0; i < nv; ++i){
					Vec3d v;
					for(int c=0; c<3; ++c){
						v[c] = (rand() % 100 / 100.0 - 0.5) * 5 * 1.3;
					}
					md.vertices.push_back(v);
				}
				CDShapeIf* s = GetSdk()->GetPHSdk()->CreateShape(md);
				soBox.back()->AddShape(s);
				soBox.back()->SetFramePosition(Vec3f(0.5, 20,0));
//				soBox.back()->SetFramePosition(Vec3f(0.5, 10+3*soBox.size(),0));
				soBox.back()->SetOrientation(Quaternionf::Rot(Rad(30), 'y'));  
				ostringstream os;
				os << "sphere" << (unsigned int)soBox.size();
				soBox.back()->SetName(os.str().c_str());
				phscene->SetContactMode(soPointer, PHSceneDesc::MODE_NONE);
			}
			break;
		case 'm':
			{
				// Lump of Box
				soBox.push_back(phscene->CreateSolid(desc));
				soBox.back()->AddShape(meshBox);
				soBox.back()->AddShape(meshBox);
				soBox.back()->AddShape(meshBox);
				soBox.back()->AddShape(meshBox);
				soBox.back()->AddShape(meshBox);
				soBox.back()->AddShape(meshBox);
				soBox.back()->AddShape(meshBox);
				Posed pose;
				pose.Pos() = Vec3d(3, 0, 0);
				soBox.back()->SetShapePose(1, pose);
				pose.Pos() = Vec3d(-3, 0, 0);
				soBox.back()->SetShapePose(2, pose);
				pose.Pos() = Vec3d(0, 3, 0);
				soBox.back()->SetShapePose(3, pose);
				pose.Pos() = Vec3d(0, -3, 0);
				soBox.back()->SetShapePose(4, pose);
				pose.Pos() = Vec3d(0, 0, 3);
				soBox.back()->SetShapePose(5, pose);
				pose.Pos() = Vec3d(0, 0, -3);
				soBox.back()->SetShapePose(6, pose);
				
				soBox.back()->SetFramePosition(Vec3f(0.5, 20,0));
	//			soBox.back()->SetFramePosition(Vec3f(0.5, 10+3*soBox.size(),0));
				soBox.back()->SetOrientation(Quaternionf::Rot(Rad(30), 'y'));  
				ostringstream os;
				os << "box" << (unsigned int)soBox.size();
				soBox.back()->SetName(os.str().c_str());
			    phscene->SetContactMode(soPointer, PHSceneDesc::MODE_NONE);				
			}
			break;
		case 'x':
			{
				// Wall
				PHSolidDesc soliddesc;
				soliddesc.mass = 0.05;
				soliddesc.inertia = 0.033 * Matrix3d::Unit();
				int wall_height = 4;
				int numbox = 5;
				for(int i = 0; i < wall_height; i++){
					for(int j = 0; j < numbox; j++){
						soBox.push_back(phscene->CreateSolid(soliddesc));
						soBox.back()->AddShape(meshBox);
						soBox.back()->SetFramePosition(Vec3d(-4.0 + (2.0 + 0.1) * j , (2.0 + 0.1) * (double)i, -2.0));  
					}
				}
			}
			break;
		case 'z':
			{
				// Tower
				PHSolidDesc soliddesc;
				double tower_radius = 5;
				int tower_height = 5;
				int numbox = 5;
				double theta;
				for(int i = 0; i < tower_height; i++){
					for(int j = 0; j < numbox; j++){
						soBox.push_back(phscene->CreateSolid(soliddesc));
						soBox.back()->AddShape(meshBox);
						theta = ((double)j + (i % 2 ? 0.0 : 0.5)) * Rad(360) / (double)numbox;
//						soBox.back()->SetFramePosition(Vec3f(0.5, 20, 0));
						soBox.back()->SetFramePosition(Vec3d(tower_radius * cos(theta), 2.0 * ((double)i), tower_radius * sin(theta)));
						soBox.back()->SetOrientation(Quaterniond::Rot(-theta, 'y'));  
					}
				}
			}
			break;
		default:
			break;
	}
}