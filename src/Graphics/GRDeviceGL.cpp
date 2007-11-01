/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#define USE_GREW
//#define GLEW_MX

#if defined(USE_GREW)
#ifndef _MSC_VER
# define GLEW_STATIC		// 定義するとAPI宣言にdeclspecキーワードが付かない
#endif
# include <GL/glew.h>
#endif

#include "Graphics.h"

#ifdef USE_HDRSTOP
#pragma hdrstop
#endif
#ifdef _MSC_VER
# include <windows.h>
#endif
#include <stack>


#include <GL/glut.h>
#include "GRLoadBmp.h"




namespace Spr {;
//----------------------------------------------------------------------------
//	GRDeviceGL
IF_OBJECT_IMP(GRDeviceGL, GRDevice);
/// 初期設定
void GRDeviceGL::Init(){
	nLights = 0;
	glDrawBuffer(GL_BACK);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
	SetVertexFormat(GRVertexElement::vfP3f);
	
	// 視点行列の設定
	viewMatrix.Pos() = Vec3f(0.0, 0.0, 1.0);	                        // eye
	viewMatrix.LookAtGL(Vec3f(0.0, 0.0, 0.0), Vec3f(0.0, 1.0, 0.0));	// center, up 
	viewMatrix = viewMatrix.inv();			
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(viewMatrix);
}
///	Viewport設定
void GRDeviceGL::SetViewport(Vec2f pos, Vec2f sz){
	glViewport((int)pos.x , (int)pos.y, (int)sz.x, (int)sz.y);
}
/// バッファクリア
void GRDeviceGL::ClearBuffer(){
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClearDepth(1.0); 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
/// レンダリングの開始前に呼ぶ関数
void GRDeviceGL::BeginScene(){
  /// GLでレンダリングする場合、特に処理する必要なし
}
/// レンダリングの終了後に呼ぶ関数
void GRDeviceGL::EndScene(){

}
///	カレントの視点行列をafvで置き換える
void GRDeviceGL::SetViewMatrix(const Affinef& afv){   
	viewMatrix  = afv;		// 視点行列の保存
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(viewMatrix);
	//glLoadMatrixf(viewMatrix * modelMatrix);
}
/// カレントの投影行列をafpで置き換える
void GRDeviceGL::SetProjectionMatrix(const Affinef& afp){  
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(afp);
	glMatrixMode(GL_MODELVIEW);
}
/// カレントの投影行列を取得する
void GRDeviceGL::GetProjectionMatrix(const Affinef& afp){  
	glGetFloatv(GL_PROJECTION_MATRIX, (GLfloat *)&afp);
}	
/// カレントのモデル行列をafwで置き換える
void GRDeviceGL::SetModelMatrix(const Affinef& afw){
	modelMatrix = afw;		// モデル行列の保存
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(viewMatrix * modelMatrix);
}
/// カレントのモデル行列に対してafwを掛ける
void GRDeviceGL::MultModelMatrix(const Affinef& afw){	
	modelMatrix = modelMatrix * afw;		
	glMatrixMode(GL_MODELVIEW);
	glMultMatrixf(afw);
}
/// カレントのモデル行列をモデル行列スタックへ保存する
void GRDeviceGL::PushModelMatrix(){
	modelMatrixStack.push(modelMatrix);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
}
/// モデル行列スタックから取り出し、カレントのモデル行列とする
void GRDeviceGL::PopModelMatrix(){
	modelMatrix = modelMatrixStack.top();
	modelMatrixStack.pop();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}
/// ブレンド変換行列の全要素を削除する
void GRDeviceGL::ClearBlendMatrix(){
	blendMatrix.clear();
}	
/// ブレンド変換行列を設定する
bool GRDeviceGL::SetBlendMatrix(const Affinef& afb){
	blendMatrix.push_back(afb);
	return true;
}
/// ブレンド変換行列を設定する
bool GRDeviceGL::SetBlendMatrix(const Affinef& afb, unsigned int id){
	if (id == 0){
		SetBlendMatrix(afb);
		return true;
	}
	if (blendMatrix.size() > id) {
		blendMatrix[id] = afb;
		return true;
	}
	return false;
}		
/// 頂点フォーマットの指定
void GRDeviceGL::SetVertexFormat(const GRVertexElement* e){	
	if (e == GRVertexElement::vfP3f) {
		vertexFormatGl = GL_V3F; 
		vertexSize = sizeof(float)*3;
		vertexColor = false;
	}else if (e == GRVertexElement::vfC4bP3f){
		vertexFormatGl = GL_C4UB_V3F;
		vertexSize = sizeof(float)*3+sizeof(char)*4;
		vertexColor = true;
	}else if (e == GRVertexElement::vfN3fP3f){
		vertexFormatGl = GL_N3F_V3F;
		vertexSize = sizeof(float)*6;
		vertexColor = false;
	}else if (e == GRVertexElement::vfC4fN3fP3f){
		vertexFormatGl = GL_C4F_N3F_V3F;
		vertexSize = sizeof(float)*10;
		vertexColor = true;
	}else if (e == GRVertexElement::vfT2fP3f){
		vertexFormatGl = GL_T2F_V3F;
		vertexSize = sizeof(float)*5;
		vertexColor = false;
	}else if (e == GRVertexElement::vfT2fC4bP3f){
		vertexFormatGl = GL_T2F_C4UB_V3F;
		vertexSize = sizeof(float)*5+sizeof(char)*4;
		vertexColor = true;
	}else if (e == GRVertexElement::vfT2fN3fP3f){
		vertexFormatGl = GL_T2F_N3F_V3F;
		vertexSize = sizeof(float)*8;
		vertexColor = false;
	}else if (e == GRVertexElement::vfT2fC4fN3fP3f){
		vertexFormatGl = GL_T2F_C4F_N3F_V3F;
		vertexSize = sizeof(float)*12;
		vertexColor = true;

	}else if (e == GRVertexElement::vfP3fB4f) {
		vertexFormatGl = GL_V3F; 
		vertexSize = sizeof(float)*15;
		vertexColor = false;
	}else if (e == GRVertexElement::vfC4bP3fB4f){
		vertexFormatGl = GL_C4UB_V3F;
		vertexSize = sizeof(float)*15+sizeof(char)*4;
		vertexColor = true;
	}else if (e == GRVertexElement::vfC3fP3fB4f){
		vertexFormatGl = GL_C3F_V3F;
		vertexSize = sizeof(float)*18;
		vertexColor = true;
	}else if (e == GRVertexElement::vfN3fP3fB4f){
		vertexFormatGl = GL_N3F_V3F;
		vertexSize = sizeof(float)*18;
		vertexColor = false;
	}else if (e == GRVertexElement::vfC4fN3fP3fB4f){
		vertexFormatGl = GL_C4F_N3F_V3F;
		vertexSize = sizeof(float)*22;
		vertexColor = true;
	}else if (e == GRVertexElement::vfT2fP3fB4f){
		vertexFormatGl = GL_T2F_V3F;
		vertexSize = sizeof(float)*17;
		vertexColor = false;
	}else if (e == GRVertexElement::vfT2fC4bP3fB4f){
		vertexFormatGl = GL_T2F_C4UB_V3F;
		vertexSize = sizeof(float)*17+sizeof(char)*4;
		vertexColor = true;
	}else if (e == GRVertexElement::vfT2fN3fP3fB4f){
		vertexFormatGl = GL_T2F_N3F_V3F;
		vertexSize = sizeof(float)*20;
		vertexColor = false;
	}else if (e == GRVertexElement::vfT2fC4fN3fP3fB4f){
		vertexFormatGl = GL_T2F_C4F_N3F_V3F;
		vertexSize = sizeof(float)*24;
		vertexColor = true;
	}else {
		for(int i=0; GRVertexElement::typicalFormats[i]; ++i){
			if (CompareVertexFormat(e, GRVertexElement::typicalFormats[i])){
				SetVertexFormat(GRVertexElement::typicalFormats[i]);
				return;
			}
		}
		for(int i=0; GRVertexElement::typicalBlendFormats[i]; ++i){
			if (CompareVertexFormat(e, GRVertexElement::typicalBlendFormats[i])){
				SetVertexFormat(GRVertexElement::typicalBlendFormats[i]);
				return;
			}
		}
		vertexFormatGl = 0;
		vertexSize = 0;
		vertexColor = false;
		assert(0);
	}
}
/// 頂点座標を指定してプリミティブを描画
void GRDeviceGL::SetVertexShader(void* s){
	assert(0);	//	To Be Implemented
}
/// 頂点座標を指定してプリミティブを描画
void GRDeviceGL::DrawDirect(GRRenderBaseIf::TPrimitiveType ty, void* vtx, size_t count, size_t stride){
	GLenum mode = GL_TRIANGLES;
	switch(ty) {
		case GRRenderBaseIf::POINTS:			mode = GL_POINTS;			break;
		case GRRenderBaseIf::LINES:				mode = GL_LINES;			break;
		case GRRenderBaseIf::LINE_STRIP:		mode = GL_LINE_STRIP;		break;
		case GRRenderBaseIf::TRIANGLES:			mode = GL_TRIANGLES;		break;
		case GRRenderBaseIf::TRIANGLE_STRIP:	mode = GL_TRIANGLE_STRIP;	break;
		case GRRenderBaseIf::TRIANGLE_FAN:		mode = GL_TRIANGLE_FAN;		break;
		case GRRenderBaseIf::QUADS:				mode = GL_QUADS;			break;
		default:				/* DO NOTHING */			break;
	}
	if (!stride) stride = vertexSize;
	if (vertexColor) glEnable(GL_COLOR_MATERIAL);
	else glDisable(GL_COLOR_MATERIAL);
	SetMaterial(currentMaterial);
	glInterleavedArrays(vertexFormatGl, stride, vtx);
	glDrawArrays(mode, 0, count);
}
/// 頂点座標とインデックスを指定してプリミティブを描画
void GRDeviceGL::DrawIndexed(GRRenderBaseIf::TPrimitiveType ty, size_t* idx, void* vtx, size_t count, size_t stride){
	GLenum mode = GL_TRIANGLES;
	switch(ty) {
		case GRRenderBaseIf::POINTS:			mode = GL_POINTS;			break;
		case GRRenderBaseIf::LINES:				mode = GL_LINES;			break;
		case GRRenderBaseIf::LINE_STRIP:		mode = GL_LINE_STRIP;		break;
		case GRRenderBaseIf::TRIANGLES:			mode = GL_TRIANGLES;		break;
		case GRRenderBaseIf::TRIANGLE_STRIP:	mode = GL_TRIANGLE_STRIP;	break;
		case GRRenderBaseIf::TRIANGLE_FAN:		mode = GL_TRIANGLE_FAN;		break;
		case GRRenderBaseIf::QUADS:				mode = GL_QUADS;			break;
		default:				/* DO NOTHING */			break;
	}
	if (!stride) stride = vertexSize;
	if (vertexColor) glEnable(GL_COLOR_MATERIAL);	
	else glDisable(GL_COLOR_MATERIAL);
	SetMaterial(currentMaterial);
	glInterleavedArrays(vertexFormatGl, stride, vtx);
	glDrawElements(mode, count, GL_UNSIGNED_INT, idx);
}
void GRDeviceGL::DrawSphere(float radius, int slices, int stacks){
	glutSolidSphere(radius, slices, stacks);
}

///	DiplayList の作成
int GRDeviceGL::StartList(){
	int list = glGenLists(1);
	glNewList(list, GL_COMPILE);
	return list;
}
void GRDeviceGL::EndList(){
	glEndList();
}
///	DisplayListの表示	
void GRDeviceGL::DrawList(int list){
	glCallList(list);
}
///	DisplayListの解放	
void GRDeviceGL::ReleaseList(int list){
	glDeleteLists(list, 1);
}
/// 3次元テキストの描画（GLオンリー版でfontは指定なし） .. Vec2f pos
void GRDeviceGL::DrawFont(Vec2f pos, const std::string str){
	glDisable(GL_LIGHTING);
	glRasterPos2fv(pos);
	std::string::const_iterator iter;
	for (iter = str.begin(); iter != str.end();	++iter) {
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *iter);
	}
	glEnable(GL_LIGHTING);
}
/// 3次元テキストの描画（GLオンリー版でfontは指定なし）.. Vec3f pos
void GRDeviceGL::DrawFont(Vec3f pos, const std::string str){
	glDisable(GL_LIGHTING);
	glRasterPos3fv(pos);
	std::string::const_iterator iter;
	for (iter = str.begin(); iter != str.end();	++iter) {
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *iter);
	}
	glEnable(GL_LIGHTING);
}
/// 3次元テキストの描画 ... Vec2f pos
void GRDeviceGL::DrawFont(Vec2f pos, const std::string str, const GRFont& font){
/// VC版のみfontをサポートする。
#ifdef _MSC_VER	
	GLuint	base = 0;						// DisplayList開始指標番号
	bool	findFont = false;				// fontListの検索結果
	GLsizei	range	 = 256;					//	生成するディスプレイリストの数
	unsigned long color=font.color;			//	フォントの色

	if (fontList.size() > 0) {		// fontListが空でない場合は、fontListの検索が必要
		// ストアサイズ10以上の場合、先頭を削除	
		if (fontList.size() > 10) {		
			fontList.erase(fontList.begin());	
		}
		// fontListを検索
		std::map<unsigned int, GRFont>::iterator itr = fontList.begin();
		while((itr != fontList.end()) && (findFont == false)) {
			if (((*itr).second.height == font.height)
				&& ((*itr).second.width == font.width)
				&& ((*itr).second.weight == font.weight)
				&& ((*itr).second.bItalic == font.bItalic)
				&& ((strcmp((*itr).second.face.c_str(),font.face.c_str()))==0)
				&& ((*itr).second.color == font.color)){
					findFont = true;
					base = (*itr).first;
					color = (*itr).second.color;
			} 
			++itr;
		}
	}
	if (findFont == false ) {		// 新規登録の場合
		// 引数で指定された特性を持つ論理フォントを作成する
		HFONT		hFont;			// フォントハンドル
		HFONT		hOldFont;	
		fontBase = glGenLists(range);
		// フォントの作成
		hFont = CreateFont(font.height,						//	フォントの高さ
							font.width,						//	平均文字幅
							0,								//	文字送り方向のX軸の角度
							0,								//	各文字とX軸の角度
							font.weight,					//	フォントの太さ
							font.bItalic,					//	イタリック体
							FALSE,							//	アンダーライン
							FALSE,							//	打ち消し線付
							ANSI_CHARSET,					//	フォント文字セット
							OUT_DEFAULT_PRECIS,				//	出力精度
							CLIP_DEFAULT_PRECIS,			//	クリッピング精度
							ANTIALIASED_QUALITY,			//	出力品質
							FF_DONTCARE | DEFAULT_PITCH,	//	ピッチとファミリ(文字間隔と書体)
							font.face.c_str());				//	タイプフェイス
		
		assert(hFont);
		HDC hDC = GetDC(0);
		// 0から256のコードの文字を、DisplayListのbase番目から登録.
		// wglUseFontBitmaps()関数、使用して、生成したフォントをディスプレイリストに割当てる.
		hOldFont = (HFONT)SelectObject(hDC, hFont);			
		wglUseFontBitmaps(hDC, 0, range, fontBase);		
		SelectObject(hDC, hOldFont);
		DeleteObject(hFont);		
		// fontListへ登録
		fontList.insert(std::map<unsigned int, GRFont>::value_type(fontBase, font));	
		base = fontBase;	
	}

	glDisable(GL_LIGHTING);
	glColor3f(((font.color >> 16) & 0xFF) / 255.0,
			  ((font.color >>  8) & 0xFF) / 255.0,
			  ((font.color >>  0) & 0xFF) / 255.0);
	glRasterPos2fv(pos);
	glPushAttrib(GL_LIST_BIT);
	glListBase(base);											// 	ディスプレイリストを渡す.	
	glCallLists(str.size(), GL_UNSIGNED_BYTE, str.c_str());		// 文字列を渡す.
	glPopAttrib();
	glEnable(GL_LIGHTING);
#else	// _MSC_VER	
	// VC以外の環境では、fontをサポートしない。
	DrawFont(pos, str);
#endif	
}
/// 3次元テキストの描画  ... Vec3f pos
void GRDeviceGL::DrawFont(Vec3f pos, const std::string str, const GRFont& font){
/// VC版のみfontをサポートする。
#ifdef _MSC_VER		
	GLuint	base = 0;						// DisplayList開始指標番号
	bool	findFont = false;				// fontListの検索結果
	GLsizei	range	 = 256;					//	生成するディスプレイリストの数
	unsigned long color=font.color;			//	フォントの色

	if (fontList.size() > 0) {		// fontListが空でない場合は、fontListの検索が必要
		// ストアサイズ10以上の場合、先頭を削除	
		if (fontList.size() > 10) {		
			fontList.erase(fontList.begin());	
		}
		// fontListを検索
		std::map<unsigned int, GRFont>::iterator itr = fontList.begin();
		while((itr != fontList.end()) && (findFont == false)) {
			if (((*itr).second.height == font.height)
				&& ((*itr).second.width == font.width)
				&& ((*itr).second.weight == font.weight)
				&& ((*itr).second.bItalic == font.bItalic)
				&& ((strcmp((*itr).second.face.c_str(),font.face.c_str()))==0)
				&& ((*itr).second.color == font.color)){
					findFont = true;
					base = (*itr).first;
					color = (*itr).second.color;
			} 
			++itr;
		}
	}
	if (findFont == false ) {		// 新規登録の場合
		// 引数で指定された特性を持つ論理フォントを作成する
		HFONT		hFont;			// フォントハンドル
		HFONT		hOldFont;	
		fontBase = glGenLists(range);
		// フォントの作成
		hFont = CreateFont(font.height,						//	フォントの高さ
							font.width,						//	平均文字幅
							0,								//	文字送り方向のX軸の角度
							0,								//	各文字とX軸の角度
							font.weight,					//	フォントの太さ
							font.bItalic,					//	イタリック体
							FALSE,							//	アンダーライン
							FALSE,							//	打ち消し線付
							ANSI_CHARSET,					//	フォント文字セット
							OUT_DEFAULT_PRECIS,				//	出力精度
							CLIP_DEFAULT_PRECIS,			//	クリッピング精度
							ANTIALIASED_QUALITY,			//	出力品質
							FF_DONTCARE | DEFAULT_PITCH,	//	ピッチとファミリ(文字間隔と書体)
							font.face.c_str());				//	タイプフェイス
		
		assert(hFont);
		HDC hDC = GetDC(0);
		// 0から256のコードの文字を、DisplayListのbase番目から登録.
		// wglUseFontBitmaps()関数、使用して、生成したフォントをディスプレイリストに割当てる.
		hOldFont = (HFONT)SelectObject(hDC, hFont);			
		wglUseFontBitmaps(hDC, 0, range, fontBase);		
		SelectObject(hDC, hOldFont);
		DeleteObject(hFont);		
		// fontListへ登録
		fontList.insert(std::map<unsigned int, GRFont>::value_type(fontBase, font));	
		base = fontBase;	
	}

	glDisable(GL_LIGHTING);
	glColor3f(((font.color >> 16) & 0xFF) / 255.0,
			  ((font.color >>  8) & 0xFF) / 255.0,
			  ((font.color >>  0) & 0xFF) / 255.0);
	glRasterPos3fv(pos);
	glPushAttrib(GL_LIST_BIT);
	glListBase(base);											// 	ディスプレイリストを渡す.	
	glCallLists(str.size(), GL_UNSIGNED_BYTE, str.c_str());		// 文字列を渡す.
	glPopAttrib();
	glEnable(GL_LIGHTING);
#else	// _MSC_VER	
	// VC以外の環境では、fontをサポートしない。
	DrawFont(pos, str);
#endif	
}
/// 描画の材質の設定
void GRDeviceGL::SetMaterial(const GRMaterialDesc& mat){
	glMaterialfv(GL_FRONT, GL_AMBIENT,   mat.ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat.diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR,  mat.specular);
	glMaterialfv(GL_FRONT, GL_EMISSION,  mat.emissive);
	glMaterialf (GL_FRONT, GL_SHININESS, mat.power);
	if (mat.texname.length()){
		int texId = LoadTexture(mat.texname);
		glBindTexture(GL_TEXTURE_2D, texId);
	}else{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	currentMaterial = mat;
}
/// 描画する点・線の太さの設定
void GRDeviceGL::SetLineWidth(float w){
	glLineWidth(w);
}
/// 光源スタックをPush
void GRDeviceGL::PushLight(const GRLightDesc& light){
	if (nLights < GL_MAX_LIGHTS) {
		glLightfv(GL_LIGHT0+nLights, GL_AMBIENT, light.ambient);
		glLightfv(GL_LIGHT0+nLights, GL_DIFFUSE, light.diffuse);
		glLightfv(GL_LIGHT0+nLights, GL_SPECULAR, light.specular);
		glLightfv(GL_LIGHT0+nLights, GL_POSITION, light.position);
		if (light.position.W()){
			// attenuation factor = 1/(GL_CONSTANT_ATTENUATION + GL_LINEAR_ATTENUATION * d + GL_QUADRATIC_ATTENUATION * d^2)
			glLightf(GL_LIGHT0+nLights, GL_CONSTANT_ATTENUATION, light.attenuation0);		// 	一定減衰率
			glLightf(GL_LIGHT0+nLights, GL_LINEAR_ATTENUATION, light.attenuation1);			//	線形減衰率
			glLightf(GL_LIGHT0+nLights, GL_QUADRATIC_ATTENUATION, light.attenuation2);		//	2次減衰率
			glLightfv(GL_LIGHT0+nLights, GL_SPOT_DIRECTION, light.spotDirection);
			glLightf(GL_LIGHT0+nLights, GL_SPOT_EXPONENT, light.spotFalloff);
			glLightf(GL_LIGHT0+nLights, GL_SPOT_CUTOFF, light.spotCutoff);
		}
		glEnable(GL_LIGHT0+nLights);
	}
	nLights ++;
}
/// 光源スタックをPop
void GRDeviceGL::PopLight(){
	nLights--;
	if (nLights < GL_MAX_LIGHTS) glDisable(GL_LIGHT0+nLights);
}
/// デプスバッファへの書き込みを許可/禁止する
void GRDeviceGL::SetDepthWrite(bool b){
	glDepthMask(b);
}
/// デプステストを有効/無効にする
void GRDeviceGL::SetDepthTest(bool b){
if (b) glEnable(GL_DEPTH_TEST);
	else glDisable(GL_DEPTH_TEST);
}
/// デプスバッファ法に用いる判定条件を指定する
void GRDeviceGL::SetDepthFunc(GRRenderBaseIf::TDepthFunc f){
	GLenum dfunc = GL_LESS;
	switch(f){
		case GRRenderBaseIf::DF_NEVER:		dfunc = GL_NEVER;		break;
		case GRRenderBaseIf::DF_LESS:		dfunc = GL_LESS;		break;
		case GRRenderBaseIf::DF_EQUAL:		dfunc = GL_EQUAL;		break;
		case GRRenderBaseIf::DF_LEQUAL:		dfunc = GL_LEQUAL;		break;
		case GRRenderBaseIf::DF_GREATER:	dfunc = GL_GREATER;		break;
		case GRRenderBaseIf::DF_NOTEQUAL:	dfunc = GL_NOTEQUAL;	break;
		case GRRenderBaseIf::DF_GEQUAL:		dfunc = GL_GEQUAL;		break;
		case GRRenderBaseIf::DF_ALWAYS:		dfunc = GL_ALWAYS;		break;
		default:			/* DO NOTHING */		break;

	}
	glDepthFunc(dfunc);
}
/// アルファブレンディングを有効/無効にする
void GRDeviceGL::SetAlphaTest(bool b){
if (b) glEnable(GL_BLEND);
	else glDisable(GL_BLEND);
}
/// アルファブレンディングのモード設定(SRCの混合係数, DEST混合係数)
void GRDeviceGL::SetAlphaMode(GRRenderBaseIf::TBlendFunc src, GRRenderBaseIf::TBlendFunc dest){
	GRRenderBaseIf::TBlendFunc  bffac[2] = { src, dest };
	GLenum glfac[2];

	for (int iCnt=0; iCnt<2; ++iCnt){
		switch(bffac[iCnt]) {
			case GRRenderBaseIf::BF_ZERO:				glfac[iCnt] = GL_ZERO;					break;
			case GRRenderBaseIf::BF_ONE:				glfac[iCnt] = GL_ONE;					break;
			case GRRenderBaseIf::BF_SRCCOLOR:			glfac[iCnt] = GL_SRC_COLOR;				break;
			case GRRenderBaseIf::BF_INVSRCCOLOR:		glfac[iCnt] = GL_ONE_MINUS_SRC_COLOR;	break;
			case GRRenderBaseIf::BF_SRCALPHA:			glfac[iCnt] = GL_SRC_ALPHA;				break;
			case GRRenderBaseIf::BF_INVSRCALPHA:		glfac[iCnt] = GL_ONE_MINUS_SRC_ALPHA;	break;
			case GRRenderBaseIf::BF_DESTALPHA:			glfac[iCnt] = GL_DST_ALPHA;				break;
			case GRRenderBaseIf::BF_INVDESTALPHA:		glfac[iCnt] = GL_ONE_MINUS_DST_ALPHA;	break;
			case GRRenderBaseIf::BF_DESTCOLOR:			glfac[iCnt] = GL_DST_COLOR;				break;
			case GRRenderBaseIf::BF_INVDESTCOLOR:		glfac[iCnt] = GL_ONE_MINUS_DST_COLOR;	break;
			case GRRenderBaseIf::BF_SRCALPHASAT:		glfac[iCnt] = GL_SRC_ALPHA_SATURATE;	break;
			case GRRenderBaseIf::BF_BOTHINVSRCALPHA:	glfac[iCnt] = 0;						break;
			case GRRenderBaseIf::BF_BLENDFACTOR:		glfac[iCnt] = 0;						break;
			case GRRenderBaseIf::BF_INVBLENDFACTOR:		glfac[iCnt] = 0;						break;
			default:					/* DO NOTHING */						break;
		}
	}
	glBlendFunc(glfac[0], glfac[1]);
}
/// テクスチャのロード（戻り値：テクスチャID）	
static const GLenum	pxfm[] = {GL_LUMINANCE, GL_LUMINANCE_ALPHA, GL_BGR_EXT, GL_BGRA_EXT};
unsigned int GRDeviceGL::LoadTexture(const std::string filename){
	GRTexnameMap::iterator it = texnameMap.find(filename);
	if (it != texnameMap.end()) return it->second;


	char *texbuf = NULL;
	int tx=0, ty=0, nc=0;
	unsigned int texId=0;

	// ファイル名が空なら return 0;
	if (filename.empty()) return 0;

	// paintLib でファイルをロード．
	try{
		int h = LoadBmpCreate(filename.c_str());
		tx = LoadBmpGetWidth(h);
		ty = LoadBmpGetHeight(h);
		nc = LoadBmpGetBytePerPixel(h);
		texbuf = new char[tx*ty*nc];
		LoadBmpGetBmp(h, texbuf);
		LoadBmpRelease(h);
	}
	catch(.../*PLTextException e*/){
		//DSTR << e << endl;
		return 0;
	}

	// テクスチャの生成．
	glGenTextures(1, (GLuint *)&texId);
	glBindTexture(GL_TEXTURE_2D, texId);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

	int rv = gluBuild2DMipmaps(GL_TEXTURE_2D, nc, tx, ty, pxfm[nc - 1], GL_UNSIGNED_BYTE, texbuf);
	glBindTexture(GL_TEXTURE_2D, 0);
	if (rv){
		DSTR << gluErrorString(rv) << std::endl;
	}
	delete texbuf;
	texnameMap[filename] = texId;
	return texId;
}













#if 0		//	頂点シェーダーを使って頂点ブレンドをする時の参考用
/// インデックス形式によるシェーダを適用した DisplayList の作成（SetVertexFormat() および SetShaderFormat() の後に呼ぶ）
int GRDeviceGL::CreateShaderIndexedList(GRHandler shader, void* location, 
										GRRenderBaseIf::TPrimitiveType ty, size_t* idx, void* vtx, size_t count, size_t stride){
	int list = glGenLists(1);
	glNewList(list, GL_COMPILE);

	if (!stride) stride = vertexSize;	
	GLenum mode = GL_TRIANGLES;
	switch(ty) {
		case GRRenderBaseIf::POINTS:			mode = GL_POINTS;			break;
		case GRRenderBaseIf::LINES:				mode = GL_LINES;			break;
		case GRRenderBaseIf::LINE_STRIP:		mode = GL_LINE_STRIP;		break;
		case GRRenderBaseIf::TRIANGLES:			mode = GL_TRIANGLES;		break;
		case GRRenderBaseIf::TRIANGLE_STRIP:	mode = GL_TRIANGLE_STRIP;	break;
		case GRRenderBaseIf::TRIANGLE_FAN:		mode = GL_TRIANGLE_FAN;		break;
		case GRRenderBaseIf::QUADS:				mode = GL_QUADS;			break;
		default:				/* DO NOTHING */			break;
	}											
	
	if ((shaderType == GRShaderFormat::shP3fB4f)		// 他のGRShaderFormatは未対応	
			|| (shaderType == GRShaderFormat::shC4bP3fB4f)
			|| (shaderType == GRShaderFormat::shC3fP3fB4f))			
	{
		glUseProgram(shader);

		// ロケーション型へのキャスト
		GRShaderFormat::SFBlendLocation *loc = (GRShaderFormat::SFBlendLocation *)location;
		if (loc) {
			glUniformMatrix4fv(loc->uBlendMatrix, 4, false, (GLfloat *)&*blendMatrix.begin());
			
			glEnableVertexAttribArray(loc->aWeight); 
			glEnableVertexAttribArray(loc->aMatrixIndices); 
			glEnableVertexAttribArray(loc->aNumMatrix); 
			
			// vtxを頂点フォーマット型へキャスト
			if (shaderType == GRShaderFormat::shP3fB4f) {
				GRVertexElement::VFP3fB4f* basePointer = (GRVertexElement::VFP3fB4f *)vtx;
				glVertexAttribPointer(loc->aWeight, 4, GL_FLOAT, GL_FALSE, vertexSize, &(basePointer->b.data[0]));		
				glVertexAttribPointer(loc->aMatrixIndices, 4, GL_FLOAT, GL_FALSE, vertexSize, &(basePointer->mi.data[0]));
				glVertexAttribPointer(loc->aNumMatrix, 4, GL_FLOAT, GL_FALSE, vertexSize, &(basePointer->nb.data[0]));
			} else if(shaderType == GRShaderFormat::shC4bP3fB4f){
				GRVertexElement::VFC4bP3fB4f* basePointer = (GRVertexElement::VFC4bP3fB4f *)vtx;
				glVertexAttribPointer(loc->aWeight, 4, GL_FLOAT, GL_FALSE, vertexSize, &(basePointer->b.data[0]));		
				glVertexAttribPointer(loc->aMatrixIndices, 4, GL_FLOAT, GL_FALSE, vertexSize, &(basePointer->mi.data[0]));
				glVertexAttribPointer(loc->aNumMatrix, 4, GL_FLOAT, GL_FALSE, vertexSize, &(basePointer->nb.data[0]));
			} else if(shaderType == GRShaderFormat::shC3fP3fB4f){
				GRVertexElement::VFC3fP3fB4f* basePointer = (GRVertexElement::VFC3fP3fB4f *)vtx;
				glVertexAttribPointer(loc->aWeight, 4, GL_FLOAT, GL_FALSE, vertexSize, &(basePointer->b.data[0]));		
				glVertexAttribPointer(loc->aMatrixIndices, 4, GL_FLOAT, GL_FALSE, vertexSize, &(basePointer->mi.data[0]));
				glVertexAttribPointer(loc->aNumMatrix, 4, GL_FLOAT, GL_FALSE, vertexSize, &(basePointer->nb.data[0]));
			}

			glInterleavedArrays(vertexFormatGl, stride, vtx);
			glDrawElements(mode, count, GL_UNSIGNED_INT, idx);	
	
			glDisableVertexAttribArray(loc->aWeight);
			glDisableVertexAttribArray(loc->aMatrixIndices);
			glDisableVertexAttribArray(loc->aNumMatrix);

		} else {
			DSTR << "To Be Implemented. " << std::endl;		
			assert(0);
		}
	} else {
		DSTR << "To Be Implemented. " << std::endl;
		assert(0);
	}									
										
	glEndList();


	return list;
}	


#endif




/// シェーダの初期化	
void GRDeviceGL::InitShader(){
#if defined(USE_GREW)
    glewInit();
	if ( GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader ) {
		DSTR << "GLSL ready" << std::endl;
	} else {
		DSTR << "GLSL not ready" << std::endl;
		assert(0);
	}
#elif defined(GL_VERSION_2_0)    
	if(glslInit())	assert(0);
 	DSTR << "Ready for OpenGL 2.0" << std::endl;
#else
	DSTR << "No GLSL support." << std::endl;
	assert(0);
#endif	
}	
/// シェーダフォーマットの設定
void GRDeviceGL::SetShaderFormat(GRShaderFormat::ShaderType type){
	shaderType = type;
	 if (type == GRShaderFormat::shP3fB4f) {
		vertexShaderFile = "shP3fB4f.vert";
		fragmentShaderFile = "PassThrough.frag";
	}else if (type == GRShaderFormat::shC4bP3fB4f){
		vertexShaderFile = "shC4bP3fB4f.vert";
		fragmentShaderFile = "PassThrough.frag";
	}else if (type == GRShaderFormat::shC3fP3fB4f){
		vertexShaderFile = "shC3fP3fB4f.vert";
		fragmentShaderFile = "PassThrough.frag";
	}else if (type == GRShaderFormat::shN3fP3fB4f){
		vertexShaderFile = "shN3fP3fB4f.vert";
		fragmentShaderFile = "PassThrough.frag";
	}else if (type == GRShaderFormat::shC4fN3fP3fB4f){
		vertexShaderFile = "shC4fN3fP3fB4f.vert";
		fragmentShaderFile = "PassThrough.frag";
	}else if (type == GRShaderFormat::shT2fP3fB4f){
		vertexShaderFile = "shT2fP3fB4f.vert";
		fragmentShaderFile = "PassThrough.frag";
	}else if (type == GRShaderFormat::shT2fC4bP3fB4f){
		vertexShaderFile = "shT2fC4bP3fB4f.vert";
		fragmentShaderFile = "PassThrough.frag";
	}else if (type == GRShaderFormat::shT2fN3fP3fB4f){
		vertexShaderFile = "shT2fN3fP3fB4f.vert";
		fragmentShaderFile = "PassThrough.frag";
	}else if (type == GRShaderFormat::shT2fC4fN3fP3fB4f){
		vertexShaderFile = "shT2fC4fN3fP3fB4f.vert";
		fragmentShaderFile = "PassThrough.frag";
	}else {
		vertexShaderFile = "";
		fragmentShaderFile = "";
		assert(0);
	}
}	
/// シェーダオブジェクトの作成	
bool GRDeviceGL::CreateShader(std::string vShaderFile, std::string fShaderFile, GRHandler& shader){
	GRHandler vertexShader;
	GRHandler fragmentShader;
#if defined(USE_GREW)
    vertexShader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB); 
    fragmentShader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
    if (ReadShaderSource(vertexShader, vShaderFile)==false)   return false;
	if (ReadShaderSource(fragmentShader, fShaderFile)==false) return false;
    glCompileShaderARB(vertexShader);
    glCompileShaderARB(fragmentShader);
    shader = glCreateProgramObjectARB();
    glAttachObjectARB(shader, vertexShader);
    glAttachObjectARB(shader, fragmentShader);
	glDeleteObjectARB(vertexShader);
	glDeleteObjectARB(fragmentShader);	
    glLinkProgramARB(shader);
	glUseProgramObjectARB(shader);
#elif defined(GL_VERSION_2_0)    
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);	
    if (ReadShaderSource(vertexShader, vShaderFile)==false)   return false;
    if (ReadShaderSource(fragmentShader, fShaderFile)==false) return false;
    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);    
    shader = glCreateProgram();    
    glAttachObject(shader, vertexShader);
    glAttachObject(shader, fragmentShader);	
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);	
	glLinkProgram(shader);	
	glUseProgram(shader);	    
#endif
	return true;
}		
/// シェーダオブジェクトの作成、GRDeviceGL::shaderへの登録（あらかじめShaderFile名を登録しておく必要がある）	
GRHandler GRDeviceGL::CreateShader(){
	GRHandler vertexShader;
	GRHandler fragmentShader;
	GRHandler shaderProgram;
#if defined(USE_GREW)
//	GLEWContext* ctx = NULL;
//	ctx = glewGetContext();
	vertexShader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB); 
    fragmentShader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
    if (ReadShaderSource(vertexShader, vertexShaderFile)==false)   return 0;
	if (ReadShaderSource(fragmentShader, fragmentShaderFile)==false) return 0;
    glCompileShaderARB(vertexShader);
    glCompileShaderARB(fragmentShader);
    shaderProgram = glCreateProgramObjectARB();
    glAttachObjectARB(shaderProgram, vertexShader);
    glAttachObjectARB(shaderProgram, fragmentShader);
	glDeleteObjectARB(vertexShader);
	glDeleteObjectARB(fragmentShader);	
    glLinkProgramARB(shaderProgram);
	glUseProgramObjectARB(shaderProgram);
#elif defined(GL_VERSION_2_0)    
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);	
    if (ReadShaderSource(vertexShader, vertexShaderFile)==false)   return 0;
    if (ReadShaderSource(fragmentShader, fragmentShaderFile)==false) return 0;
    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);    
    shaderProgram = glCreateProgram();    
    glAttachObject(shaderProgram, vertexShader);
    glAttachObject(shaderProgram, fragmentShader);	
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);	
	glLinkProgram(shaderProgram);	
	glUseProgram(shaderProgram);	    
#endif
	return shaderProgram;
}	
/// シェーダのソースプログラムをメモリに読み込み、シェーダオブジェクトと関連付ける	
bool GRDeviceGL::ReadShaderSource(GRHandler shader, std::string file){
	FILE *fp;
	char *source = NULL;
	int length=0;

	if (file.empty()) 	
		return false;

	fp = fopen(file.c_str(),"rb");
	if (fp != NULL) {      
      	fseek(fp, 0, SEEK_END);
      	length = ftell(fp);
      	rewind(fp);					
		if (length > 0) {
			source = (char *)malloc(sizeof(char) * (length+1));
			if (source == NULL) {
				DSTR << "Could not allocate read buffer." << std::endl;
				return false;
			}
			length = fread((void *)source,sizeof(char),length,fp);
			source[length] = '\0';
		}
		fclose(fp);
	}	
	const char* sourceStrings =  (char*)source;
	glShaderSource(shader, 1, &sourceStrings, (const GLint *)&length);

	free((void*)source);
	return true;
}			
/// ロケーション情報の取得（SetShaderFormat()でシェーダフォーマットを設定しておく必要あり）
void GRDeviceGL::GetShaderLocation(GRHandler shader, void* location){
	if ((shaderType == GRShaderFormat::shP3fB4f)
			|| (shaderType == GRShaderFormat::shC4bP3fB4f)
			|| (shaderType == GRShaderFormat::shC3fP3fB4f))
	{
		// shaderType が、shP3fB4f または shC4bP3fB4f または shC3fP3fB4f で、
		// ロケーション情報 に SFBlendLocation の型が指定された場合
		GRShaderFormat::SFBlendLocation *loc = (GRShaderFormat::SFBlendLocation *)location;
		if (loc) {
			loc->uBlendMatrix	= glGetUniformLocationARB(shader, "blendMatrix" );
			loc->aWeight		= glGetAttribLocationARB(shader, "weights");;
			loc->aMatrixIndices	= glGetAttribLocationARB(shader, "matrixIndices");
			loc->aNumMatrix		= glGetAttribLocationARB(shader, "numMatrix");
		} else {
			DSTR << "To Be Implemented. " << std::endl;
			assert(0);
		}
	} else {
		DSTR << "To Be Implemented. " << std::endl;
		assert(0);
	}
}	
	
	
}	//	Spr

