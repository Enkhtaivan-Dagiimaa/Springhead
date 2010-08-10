#include <Framework/SprFWAppInfo.h>
#include <Framework/SprFWApp.h>
#include <Springhead.h>
#include <GL/glut.h>

namespace Spr{;

enum TimerType;

CameraInfo::CameraInfo():
	rot(Rad(0.0), Rad(80.0)), zoom(1.0f),
	rotRangeY(Rad(-180.0), Rad(180.0)), rotRangeX(Rad(-80.0), Rad(80.0)), zoomRange(0.01f, 100.0f){
	UpdateView();
}

void CameraInfo::UpdateView(){
	view  = Affinef();
	view.Pos() = target + zoom * Vec3f(
		cos(rot.x) * cos(rot.y),
		sin(rot.x),
		cos(rot.x) * sin(rot.y));
	view.LookAtGL(target);
}

void CameraInfo::Fit(const GRCameraDesc& cam, float radius){
	float sz = std::min(cam.size.x, cam.size.y);
	if(sz == 0.0f)
		sz = cam.size.x;
	zoom = cam.front * (radius / (sz/2.0f));
	UpdateView();
}

void FWTimer::GLUTTimerFunc(int id){
	FWApp* app = FWApp::instance;
	if(!app)return;
	

	
	// �^�C�}�[�̍Ďn��
	// �������w�肳��Ă���΂��̒l���̗p
	int ms = app->GetTimer(id)->GetInterval();
	// ����ȊO�̓A�N�e�B�u�V�[����time step���猈��
	if(!ms){
		FWSceneIf* scene = app->GetSdk()->GetScene();
		if(scene && scene->GetPHScene()){
			ms = (int)(scene->GetPHScene()->GetTimeStep() * 1000.0);
			if(ms < 1) ms = 1;
		}
	}
	glutTimerFunc(ms, GLUTTimerFunc, id);

	app->ReleaseAllTimer();		//�}���`���f�B�A�^�C�}�[�����
	app->TimerFunc(id);
	app->CreateAllTimer();		//�}���`���f�B�A�^�C�}�[���č\��
}

void FWTimer::MultiMediaTimerFunc(int id){
	FWApp* app = FWApp::instance;
	if(!app)return;
	app->TimerFunc(id);
}

FWTimer::FWTimer(int _id ,TimerType t){
	id			=	_id;
	interval	=	0;
	resolution	=	1;
	timerType	=   t;
	mtimer		=	NULL;
}
void FWTimer::SetInterval(unsigned ms){
	interval	=	ms;
	if(timerType==MM && mtimer!=NULL) {
		mtimer->Interval(interval);
	}
}
void FWTimer::SetResolution(unsigned r){
	resolution	=	r;
	if(timerType==MM && mtimer!=NULL) {
		mtimer->Resolution(resolution);
	}
}
void FWTimer::Create(){
	switch(timerType){
		case GLUT:
			glutTimerFunc(interval, GLUTTimerFunc, id);
			break;
		case MM:
			mtimer = new UTMMTimer;
			mtimer->Resolution(resolution);							// ����\[ms]
			if(interval!=0) mtimer->Interval(interval);				// �Ăт������o[ms]
			mtimer->Set(MultiMediaTimerFunc, id);					// �R�[���o�b�N����֐�
			mtimer->Create();										// �R�[���o�b�N�J�n
			break;
	}
}

void FWTimer::Recreate(){
	//GLUTTimer�Ɋւ��Ă͖�����
	if(timerType==MM){
		if(mtimer!=NULL){
			mtimer->Create();
		}
	}
}

void FWTimer::Release(){
	//GLUTTimer�Ɋւ��Ă͖�����
	if(timerType==MM){
		if(mtimer!=NULL){
			mtimer->Release();
		}
	}
}

void FWTimer::Clear(){
	if(mtimer){
		delete mtimer;
	}
}

}