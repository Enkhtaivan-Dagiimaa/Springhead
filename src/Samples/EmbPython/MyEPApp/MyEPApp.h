#include "../../SampleApp.h"

#include <EmbPython/EmbPython.h>

#include <sstream>

#include "windows.h"

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
// �A�v���P�[�V�����N���X
class MyEPApp : public SampleApp{
public:
	/// �y�[�WID
	enum {
		MENU_MAIN = MENU_SCENE,
	};
	/// �A�N�V����ID
	enum {
		ID_TOGGLE_RENDER_BACKSCENE,
		ID_CALIB_SPIDAR,
		ID_RESET,
	};

	UTRef<HISdkIf>             hiSdk;
	UTRef<HISpaceNavigatorIf>  spaceNavigator0;
	UTRef<HISpaceNavigatorIf>  spaceNavigator1;

	int argc;
	char** argv;

	MyEPApp(){
		appName = "Python with Springhead";
		numScenes   = 1;
	}
	~MyEPApp(){}

	// ������
	virtual void Init(int argc, char* argv[]);

	virtual void OnStep(){
		UTAutoLock critical(EPCriticalSection);
		spaceNavigator0->Update(fwScene->GetPHScene()->GetTimeStep());
		spaceNavigator1->Update(fwScene->GetPHScene()->GetTimeStep());
		fwScene->Step();
	}

	// �`��֐��D�`��v���������Ƃ��ɌĂ΂��
	virtual void OnDraw(GRRenderIf* render) {
		UTAutoLock critical(EPCriticalSection);
		SampleApp::OnDraw(render);
	}

	virtual void OnAction(int menu, int id){
		UTAutoLock critical(EPCriticalSection);
		SampleApp::OnAction(menu, id);
	}

	// ----- ----- ----- ----- -----

	void Drop(int shape, int mat, Vec3d v, Vec3d w, Vec3d p, Quaterniond q){
		SampleApp::Drop(shape, mat, v, w, p, q);
	}

};
