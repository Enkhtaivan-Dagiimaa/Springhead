#ifndef MYAPP_H
#define MYAPP_H
#include <windows.h>
#include <d3d9.h>

class MyApp{
	
private:
	HWND				hWnd;			// window�ւ̃n���h��
	MSG					msg;
	LPCSTR				winClassName;	// winClass�̎��ʎq
	LPCTSTR				windowName;		// �\�������Window�̖��O
	LPDIRECT3D9         pD3D;			// Used to create the D3DDevice
	LPDIRECT3DDEVICE9   pD3DDevice;		// Our rendering device
	WNDCLASSEX			winClass;		// �F�X�֐���o�^����\����
	int					top;			// �E�B���h�E�̃��j�^��[����̈ʒu
	int					left;			// �E�B���h�E�̃��j�^���[����̈ʒu
	int					width;			// �E�B���h�E�̕�
	int					height;			// �E�B���h�E�̍���

public:
	static MyApp*		instance;
	//�R���X�g���N�^
	MyApp();
	//�f�X�g���N�^
	~MyApp();
	//���̃N���X�̊֐�
	bool					Init(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow);
	HRESULT					InitD3D(const HWND hWnd);
	VOID					Render();
	VOID					CleanUp();
	static LRESULT CALLBACK	CallMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT WINAPI			MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void					dx9MainLoop();
};

#endif