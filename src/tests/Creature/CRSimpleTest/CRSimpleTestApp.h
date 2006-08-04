#ifndef CR_SIMPLE_TEST_APP
#define CR_SIMPLE_TEST_APP

#include "stdafx.h"
#include <Springhead.h>
#include <SprCreature.h>  // ������x�ł�����Springhead.h�ɑg�ݍ���

namespace Spr{
	// ���C�u�������̎g�p����֐�
	void CRRegisterFactories();
	void PHRegisterTypeDescs();
	void CDRegisterTypeDescs();
	void GRRegisterTypeDescs();
	void FIRegisterTypeDescs();
	void CRRegisterTypeDescs();

	// �A�v���P�[�V�����N���X
	class CRSimpleTestApp {
	public:
		FISdkIf*          fiSdk;
		FIFileXIf*        fiFileX;

		ObjectIfs         objs;

		PHSdkIf*          phSdk;
		PHSceneIf*        phScene;

		GRSdkIf*          grSdk;
		GRSceneIf*        grScene;
		GRDebugRenderIf*  grRender;
		GRDeviceGLIf*     grDevice;

		int               stepCount;
		bool              isLoadComplete;

		CRSimpleTestApp(int window, std::string filename);

		void Register();
		void LoadFile(std::string filename);
		void CreateScene();
		void CreateRender(int window);
		void Step();
		void Display();
		void Reshape(int w, int h);
	};
}

#endif