/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_FWAPPGLUT_H
#define SPR_FWAPPGLUT_H

#include <Framework/SprFWAppGL.h>

namespace Spr{;

class FWWindowDesc{	//	hase	TypeDescができないようにクラスにしてある。TypeDesc側での対応が望ましい。
public:
	int width;			///<	幅
	int height;			///<	高さ
	int left;			///<	左端の座標
	int top;			///<	上端の座標
	int parentWindow;	///<	子ウィンドウを作る場合は、親のID、そうでなければ0
	UTString title;		///<	ウィンドウのタイトル文字列(トップレベルウィンドウのみ)
	bool fullscreen;
	FWWindowDesc(int w=640, int h=480, int l=-1, int t=-1, int p=0, bool f=false):
		width(w), height(h), left(l), top(t), parentWindow(p), fullscreen(f){
	}
};

/** @brief GLUTを用いるアプリケーションクラス
 */
class FWAppGLUT : public FWAppGL{
protected:
	static FWAppGLUT* instance;
	static void SPR_CDECL GlutDisplayFunc();
	static void SPR_CDECL GlutReshapeFunc(int w, int h);
	static void SPR_CDECL GlutTimerFunc(int id);
	static void SPR_CDECL GlutIdleFunc();
	static void SPR_CDECL GlutKeyboardFunc(unsigned char key, int x, int y);
	static void SPR_CDECL AtExit();
	std::vector<int> windows;
public:
	~FWAppGLUT();
	///	描画コールバック
	virtual void Display();

	///	GLUTの初期化を行う。最初にこれを呼ぶ必要がある。
	virtual void Init(int argc, char* argv[]);
	///	作成済みのウィンドウの数
	virtual int NWindow(){ return (int)windows.size(); }
	///	ウィンドウIDの取得
	virtual int GetWindow(int i){ return windows[i]; }
	///	ウィンドウを作成し、ウィンドウ IDを返す
	virtual int CreateWindow(const FWWindowDesc d=FWWindowDesc());
	///	ウィンドウを破棄する
	virtual void DestroyWindow(int wid);
	///	カレントウィンドウを設定する。設定後のIDを返す。
	virtual int SetWindow(int wid);
	///	カレントウィンドウのIDを返す。
	virtual int GetWindow();

	///	フレームワークをスタートする。
	virtual void Start();
};


}

#endif
