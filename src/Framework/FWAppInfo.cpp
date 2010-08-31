#include <Framework/SprFWAppInfo.h>
#include <Framework/SprFWApp.h>
#include <Springhead.h>
#include <GL/glut.h>

namespace Spr{;

enum TimerType;

//-------------------------------------------------------------------------------------------------
// FWMouseInfo

FWMouseInfo::FWMouseInfo(){
	left = middle = right = false;
	shift = ctrl = alt = false;
	first = false;
}

//-------------------------------------------------------------------------------------------------
// FWUICamera

FWUICamera::FWUICamera(){
	rot = Vec2f(Rad(0.0f), Rad(80.0f));
	zoom = 1.0f;

	rotRangeX = Vec2f(Rad(-80.0f), Rad(80.0f));
	rotRangeY = Vec2f(Rad(-180.0f), Rad(180.0f));
	zoomRange = Vec2f(0.01f, 100.0f);

	rotGain	 = 0.01f;
	posGain  = 0.001f;
	zoomGain = 0.1f;

	UpdateView();
}

void FWUICamera::UpdateView(){
	view  = Affinef();
	view.Pos() = target + zoom * Vec3f(
		cos(rot.x) * cos(rot.y),
		sin(rot.x),
		cos(rot.x) * sin(rot.y));
	view.LookAtGL(target);
}

void FWUICamera::Fit(const GRCameraDesc& cam, float radius){
	float sz = std::min(cam.size.x, cam.size.y);
	if(sz == 0.0f)
		sz = cam.size.x;
	zoom = cam.front * (radius / (sz/2.0f));
	UpdateView();
}

void FWUICamera::Rotate(int xrel, int yrel, Vec2f px){
	rot.y += (float)xrel * rotGain;
	rot.y =	std::max(rotRangeY[0], std::min(rot.y, rotRangeY[1]));
	rot.x += (float)yrel * rotGain;
	rot.x =	std::max(rotRangeX[0], std::min(rot.x, rotRangeX[1]));
	UpdateView();
}

void FWUICamera::Translate(int xrel, int yrel, Vec2f px){
	Vec3f rel(-(float)xrel * zoom * posGain, (float)yrel * zoom * posGain, 0.0f);
	target += view.Rot() * rel;
	UpdateView();
}

void FWUICamera::Zoom(int yrel){
	zoom *= (float)exp((double)yrel * zoomGain);
	zoom = std::max(zoomRange[0], std::min(zoom, zoomRange[1]));
	UpdateView();
}

//-------------------------------------------------------------------------------------------------
// FWDragger

void FWDragger::Init(PHSceneIf* _scene, const Affinef& _view, GRRenderIf* _render){
	scene	= _scene;
	view	= _view;
	render	= _render;

	if(!ray)
		ray = scene->CreateRay();
	if(!cursor){
		cursor = scene->CreateSolid();
		cursor->SetDynamical(false);
		cursor->SetName("PHCursor");
		scene->SetContactMode(cursor, PHSceneDesc::MODE_NONE);
		//CDSphereDesc sd;
		//sd.radius = 0.002f;
		//cursor->AddShape(fwSdk->GetPHSdk()->CreateShape(sd));
	}
}

void FWDragger::Grab(int x, int y){
	ray->SetOrigin(view.Pos());
	ray->SetDirection(view.Rot() * render->ScreenToCamera(x, y, 1.0f));
	ray->Apply();

	if(ray->NHits()){
		// �Ȃ��K�v�H tazz
		//ReleaseAllTimer();
		
		PHRaycastHit* hit = ray->GetNearest();

		// �J�[�\�����̂��q�b�g�ʒu�Ɉړ�
		cursor->SetCenterPosition(hit->point);

		// �q�b�g�ʒu�̃J�������猩�������iZ���W�j���L��
		Vec3f pointCamera = view.inv() * hit->point;
		depth = abs(pointCamera.z);
		
		// �q�b�g�������̂ƃJ�[�\�����̂��Ȃ��o�l
		PHSpringDesc desc;
		desc.fMax =100;
		desc.type = PHJointDesc::ELASTIC;
		Posed pose;
		// �q�b�g�����_�Ƀ\�P�b�g��z�u
		pose.Pos() = hit->point;
		desc.poseSocket = hit->solid->GetPose().Inv() * pose;

		spring = DCAST(PHSpringIf, scene->CreateJoint(hit->solid, cursor, desc));
		double mass = hit->solid->GetMass();	//mass�ɂ���čœK��K,D��ݒ肷��K�v������
		const double K = 50000.0, D = 1000;		//K = 10000.0, D = 100.0;
		spring->SetSpring(Vec3d(K, K, K)*mass);
		spring->SetDamper(Vec3d(D, D, D)*mass);
		
		//CreateAllTimer();
	}
}

void FWDragger::Drag(int x, int y){
	if(!spring)
		return;

	cursor->SetCenterPosition(view * render->ScreenToCamera(x, y, depth));
}

void FWDragger::Release(){
	// �h���b�O�o�l�̍폜
	if(spring){
		scene->DelChildObject(spring);
		spring = NULL;
	}
}

//-------------------------------------------------------------------------------------------------
// FWTimer

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