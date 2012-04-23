#include <conio.h>
#include <Springhead.h>
#include <HumanInterface/SprHIKeyMouse.h>
#include <iomanip>
#include <gl/glut.h>
#include <Windows.h>

Spr::DRKeyMouseWin32If* win32KeyMouse;
WNDPROC OldWndProc;
LRESULT CALLBACK NewWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	MSG m;
	m.hwnd		= hWnd;
	m.message	= msg;
	m.wParam	= wParam;
	m.lParam	= lParam;
	if (win32KeyMouse->PreviewMessage(&m)) {
		return 0L;
	}
	return CallWindowProc(OldWndProc, hWnd, msg, wParam, lParam);
}

using namespace Spr;
UTRef<HISdkIf> sdk;
UTRef<HITrackballIf> trackball;
void _cdecl Disp(){}


//	���C���̏���
void _cdecl Idle(){
	//	����������
	if (!OldWndProc){
		//	PreviewMesssage()���Ăяo�����߂̃v���V�[�W���̓���ւ�
		//	hWnd�̎擾
		HWND hWnd = FindWindow(NULL, "KeyMouse Test");
		// �E�B���h�E�v���V�[�W����u������
		OldWndProc = (WNDPROC)(GetWindowLongPtr(hWnd, GWLP_WNDPROC));
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG)(NewWndProc));

		//	KeyMouse��Trackball�̍쐬
		HISdkIf::RegisterSdk();
		sdk = HISdkIf::CreateSdk();
		sdk->AddRealDevice(DRKeyMouseWin32If::GetIfInfoStatic());

		//	���f�o�C�X�̎擾
		win32KeyMouse = sdk->FindRealDevice(DRKeyMouseWin32If::GetIfInfoStatic())->Cast();
		sdk->Print(DSTR);
		trackball = sdk->CreateHumanInterface("HITrackball")->Cast();
		trackball->SetKeyMouse(sdk->RentVirtualDevice("DVKeyMouse", NULL, (int)hWnd)->Cast());
	}

	//	�L�[�̓ǂݏo���Ȃ�
	if (trackball->GetKeyMouse()->GetKeyState('Q') & DVKeySt::PRESSED)
		exit(0);
	for(int i=0; i<200; ++i){
		if (trackball->GetKeyMouse()->GetKeyState(i) & DVKeySt::PRESSED){
			std::cout << i << " '"<< (char)i << "' " << std::endl;
		}
	}
	std::cout << "pose:" << trackball->GetPose() << std::endl;
}

int __cdecl main(int argc, char* argv[]){
	glutInit(&argc, argv);
	glutCreateWindow("KeyMouse Test");
	glutIdleFunc(Idle);
	glutDisplayFunc(Disp);
	glutMainLoop();
}
