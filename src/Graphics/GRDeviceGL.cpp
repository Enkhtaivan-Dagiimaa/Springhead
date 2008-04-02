/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#define USE_GREW
//#define GLEW_MX

#if defined(USE_GREW)
#ifndef _MSC_VER
# define GLEW_STATIC		// ��`�����API�錾��declspec�L�[���[�h���t���Ȃ�
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
#include <iomanip>
#include <sstream>
#include <io.h>

#include <boost/regex.hpp>

static bool enableDebugMessage = false;

namespace Spr {;
//----------------------------------------------------------------------------
//	GRDeviceGL
/// �����ݒ�
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
	
	// ���_�s��̐ݒ�
	viewMatrix.Pos() = Vec3f(0.0, 0.0, 1.0);	                        // eye
	viewMatrix.LookAtGL(Vec3f(0.0, 0.0, 0.0), Vec3f(0.0, 1.0, 0.0));	// center, up 
	viewMatrix = viewMatrix.inv();			
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(viewMatrix);
}
///	Viewport�ݒ�
void GRDeviceGL::SetViewport(Vec2f pos, Vec2f sz){
	glViewport((int)pos.x , (int)pos.y, (int)sz.x, (int)sz.y);
}
/// �o�b�t�@�N���A
void GRDeviceGL::ClearBuffer(){
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClearDepth(1.0); 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
/// �����_�����O�̊J�n�O�ɌĂԊ֐�
void GRDeviceGL::BeginScene(){
  /// GL�Ń����_�����O����ꍇ�A���ɏ�������K�v�Ȃ�
}
/// �����_�����O�̏I����ɌĂԊ֐�
void GRDeviceGL::EndScene(){

}
///	�J�����g�̎��_�s���afv�Œu��������
void GRDeviceGL::SetViewMatrix(const Affinef& afv){   
	viewMatrix  = afv;		// ���_�s��̕ۑ�
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(viewMatrix);
	//glLoadMatrixf(viewMatrix * modelMatrix);
}
/// �J�����g�̓��e�s���afp�Œu��������
void GRDeviceGL::SetProjectionMatrix(const Affinef& afp){  
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(afp);
	glMatrixMode(GL_MODELVIEW);
}
/// �J�����g�̓��e�s����擾����
void GRDeviceGL::GetProjectionMatrix(const Affinef& afp){  
	glGetFloatv(GL_PROJECTION_MATRIX, (GLfloat *)&afp);
}	
/// �J�����g�̃��f���s���afw�Œu��������
void GRDeviceGL::SetModelMatrix(const Affinef& afw){
	modelMatrix = afw;		// ���f���s��̕ۑ�
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(viewMatrix * modelMatrix);
}
/// �J�����g�̃��f���s��ɑ΂���afw���|����
void GRDeviceGL::MultModelMatrix(const Affinef& afw){	
	modelMatrix = modelMatrix * afw;		
	glMatrixMode(GL_MODELVIEW);
	glMultMatrixf(afw);
}
/// �J�����g�̃��f���s������f���s��X�^�b�N�֕ۑ�����
void GRDeviceGL::PushModelMatrix(){
	modelMatrixStack.push(modelMatrix);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
}
/// ���f���s��X�^�b�N������o���A�J�����g�̃��f���s��Ƃ���
void GRDeviceGL::PopModelMatrix(){
	modelMatrix = modelMatrixStack.top();
	modelMatrixStack.pop();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}
/// �u�����h�ϊ��s��̑S�v�f���폜����
void GRDeviceGL::ClearBlendMatrix(){
	blendMatrix.clear();
}	
/// �u�����h�ϊ��s���ݒ肷��
bool GRDeviceGL::SetBlendMatrix(const Affinef& afb){
	blendMatrix.push_back(afb);
	return true;
}
/// �u�����h�ϊ��s���ݒ肷��
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
/// ���_�t�H�[�}�b�g�̎w��
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
	}else if (e == GRVertexElement::vfT4fP4f){
		vertexFormatGl = GL_T4F_V4F;
		vertexSize = sizeof(float)*8;
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
	}else if (e == GRVertexElement::vfT4fC4fN3fP4f){
		vertexFormatGl = GL_T4F_C4F_N3F_V4F;
		vertexSize = sizeof(float)*15;
		vertexColor = false;
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
/// ���_���W���w�肵�ăv���~�e�B�u��`��
void GRDeviceGL::SetVertexShader(void* s){
	assert(0);	//	To Be Implemented
}
/// ���_���W���w�肵�ăv���~�e�B�u��`��
void GRDeviceGL::DrawDirect(GRRenderBaseIf::TPrimitiveType ty, void* vtx, size_t count, size_t stride){
	GLenum mode = GL_TRIANGLES;
	switch(ty) {
		case GRRenderBaseIf::POINTS:			mode = GL_POINTS;			break;
		case GRRenderBaseIf::LINES:				mode = GL_LINES;			break;
		case GRRenderBaseIf::LINE_STRIP:		mode = GL_LINE_STRIP;		break;
		case GRRenderBaseIf::LINE_LOOP:			mode = GL_LINE_LOOP;		break;
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
/// ���_���W�ƃC���f�b�N�X���w�肵�ăv���~�e�B�u��`��
void GRDeviceGL::DrawIndexed(GRRenderBaseIf::TPrimitiveType ty, size_t* idx, void* vtx, size_t count, size_t stride){
	GLenum mode = GL_TRIANGLES;
	switch(ty) {
		case GRRenderBaseIf::POINTS:			mode = GL_POINTS;			break;
		case GRRenderBaseIf::LINES:				mode = GL_LINES;			break;
		case GRRenderBaseIf::LINE_STRIP:		mode = GL_LINE_STRIP;		break;
		case GRRenderBaseIf::LINE_LOOP:			mode = GL_LINE_LOOP;		break;
		case GRRenderBaseIf::TRIANGLES:			mode = GL_TRIANGLES;		break;
		case GRRenderBaseIf::TRIANGLE_STRIP:	mode = GL_TRIANGLE_STRIP;	break;
		case GRRenderBaseIf::TRIANGLE_FAN:		mode = GL_TRIANGLE_FAN;		break;
		case GRRenderBaseIf::QUADS:				mode = GL_QUADS;			break;
		default:				/* DO NOTHING */			break;
	}
	if (!stride) stride = vertexSize;
	if (vertexColor) glEnable(GL_COLOR_MATERIAL);
	else glDisable(GL_COLOR_MATERIAL);
	glInterleavedArrays(vertexFormatGl, stride, vtx);
	glDrawElements(mode, count, GL_UNSIGNED_INT, idx);
}
void GRDeviceGL::DrawSphere(float radius, int slices, int stacks, bool solid){
	if(solid)
		 glutSolidSphere(radius, slices, stacks);
	else glutWireSphere(radius, slices, stacks);
}
void GRDeviceGL::DrawCone(float radius, float height, int slice, bool solid){
	if(solid)
		 glutSolidCone(radius, height, slice, 1);
	else glutWireCone(radius, height, slice, 1);
}
void GRDeviceGL::DrawCylinder(float radius, float height, int slice, bool solid){
	// ����ł͑��ʂ̂�
	glBegin(solid ? GL_QUAD_STRIP : GL_LINES);
	float step = (float)(M_PI * 2.0f) / (float)slice;
	float t = 0.0;
	float x,y;
	for (int i=0; i<=slice; i++) {
		x=sin(t);
		y=cos(t);
		glNormal3f(x, y, 0.0);
		glVertex3f(radius * x, radius * y,  height/2);
		glVertex3f(radius * x, radius * y, -height/2);
		t += step;
	}
	glEnd();
}
	

///	DiplayList �̍쐬
int GRDeviceGL::StartList(){
	int list = glGenLists(1);
	glNewList(list, GL_COMPILE);
	return list;
}
void GRDeviceGL::EndList(){
	glEndList();
}
///	DisplayList�̕\��	
void GRDeviceGL::DrawList(int list){
	glCallList(list);
}
///	DisplayList�̉��	
void GRDeviceGL::ReleaseList(int list){
	glDeleteLists(list, 1);
}
/// 3�����e�L�X�g�̕`��iGL�I�����[�ł�font�͎w��Ȃ��j .. Vec2f pos
void GRDeviceGL::DrawFont(Vec2f pos, const std::string str){
	glDisable(GL_LIGHTING);
	glRasterPos2fv(pos);
	std::string::const_iterator iter;
	for (iter = str.begin(); iter != str.end();	++iter) {
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *iter);
	}
	glEnable(GL_LIGHTING);
}
/// 3�����e�L�X�g�̕`��iGL�I�����[�ł�font�͎w��Ȃ��j.. Vec3f pos
void GRDeviceGL::DrawFont(Vec3f pos, const std::string str){
	glDisable(GL_LIGHTING);
	glRasterPos3fv(pos);
	std::string::const_iterator iter;
	for (iter = str.begin(); iter != str.end();	++iter) {
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *iter);
	}
	glEnable(GL_LIGHTING);
}
/// 3�����e�L�X�g�̕`�� ... Vec2f pos
void GRDeviceGL::DrawFont(Vec2f pos, const std::string str, const GRFont& font){
/// VC�ł̂�font���T�|�[�g����B
#ifdef _MSC_VER	
	GLuint	base = 0;						// DisplayList�J�n�w�W�ԍ�
	bool	findFont = false;				// fontList�̌�������
	GLsizei	range	 = 256;					//	��������f�B�X�v���C���X�g�̐�
	unsigned long color=font.color;			//	�t�H���g�̐F

	if (fontList.size() > 0) {		// fontList����łȂ��ꍇ�́AfontList�̌������K�v
		// �X�g�A�T�C�Y10�ȏ�̏ꍇ�A�擪���폜	
		if (fontList.size() > 10) {		
			fontList.erase(fontList.begin());	
		}
		// fontList������
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
	if (findFont == false ) {		// �V�K�o�^�̏ꍇ
		// �����Ŏw�肳�ꂽ���������_���t�H���g���쐬����
		HFONT		hFont;			// �t�H���g�n���h��
		HFONT		hOldFont;	
		fontBase = glGenLists(range);
		// �t�H���g�̍쐬
		hFont = CreateFont(font.height,						//	�t�H���g�̍���
							font.width,						//	���ϕ�����
							0,								//	�������������X���̊p�x
							0,								//	�e������X���̊p�x
							font.weight,					//	�t�H���g�̑���
							font.bItalic,					//	�C�^���b�N��
							FALSE,							//	�A���_�[���C��
							FALSE,							//	�ł��������t
							ANSI_CHARSET,					//	�t�H���g�����Z�b�g
							OUT_DEFAULT_PRECIS,				//	�o�͐��x
							CLIP_DEFAULT_PRECIS,			//	�N���b�s���O���x
							ANTIALIASED_QUALITY,			//	�o�͕i��
							FF_DONTCARE | DEFAULT_PITCH,	//	�s�b�`�ƃt�@�~��(�����Ԋu�Ə���)
							font.face.c_str());				//	�^�C�v�t�F�C�X
		
		assert(hFont);
		HDC hDC = GetDC(0);
		// 0����256�̃R�[�h�̕������ADisplayList��base�Ԗڂ���o�^.
		// wglUseFontBitmaps()�֐��A�g�p���āA���������t�H���g���f�B�X�v���C���X�g�Ɋ����Ă�.
		hOldFont = (HFONT)SelectObject(hDC, hFont);			
		wglUseFontBitmaps(hDC, 0, range, fontBase);		
		SelectObject(hDC, hOldFont);
		DeleteObject(hFont);		
		// fontList�֓o�^
		fontList.insert(std::map<unsigned int, GRFont>::value_type(fontBase, font));	
		base = fontBase;	
	}

	glDisable(GL_LIGHTING);
	glColor3f(((font.color >> 16) & 0xFF) / 255.0,
			  ((font.color >>  8) & 0xFF) / 255.0,
			  ((font.color >>  0) & 0xFF) / 255.0);
	glRasterPos2fv(pos);
	glPushAttrib(GL_LIST_BIT);
	glListBase(base);											// 	�f�B�X�v���C���X�g��n��.	
	glCallLists(str.size(), GL_UNSIGNED_BYTE, str.c_str());		// �������n��.
	glPopAttrib();
	glEnable(GL_LIGHTING);
#else	// _MSC_VER	
	// VC�ȊO�̊��ł́Afont���T�|�[�g���Ȃ��B
	DrawFont(pos, str);
#endif	
}
/// 3�����e�L�X�g�̕`��  ... Vec3f pos
void GRDeviceGL::DrawFont(Vec3f pos, const std::string str, const GRFont& font){
/// VC�ł̂�font���T�|�[�g����B
#ifdef _MSC_VER		
	GLuint	base = 0;						// DisplayList�J�n�w�W�ԍ�
	bool	findFont = false;				// fontList�̌�������
	GLsizei	range	 = 256;					//	��������f�B�X�v���C���X�g�̐�
	unsigned long color=font.color;			//	�t�H���g�̐F

	if (fontList.size() > 0) {		// fontList����łȂ��ꍇ�́AfontList�̌������K�v
		// �X�g�A�T�C�Y10�ȏ�̏ꍇ�A�擪���폜	
		if (fontList.size() > 10) {		
			fontList.erase(fontList.begin());	
		}
		// fontList������
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
	if (findFont == false ) {		// �V�K�o�^�̏ꍇ
		// �����Ŏw�肳�ꂽ���������_���t�H���g���쐬����
		HFONT		hFont;			// �t�H���g�n���h��
		HFONT		hOldFont;	
		fontBase = glGenLists(range);
		// �t�H���g�̍쐬
		hFont = CreateFont(font.height,						//	�t�H���g�̍���
							font.width,						//	���ϕ�����
							0,								//	�������������X���̊p�x
							0,								//	�e������X���̊p�x
							font.weight,					//	�t�H���g�̑���
							font.bItalic,					//	�C�^���b�N��
							FALSE,							//	�A���_�[���C��
							FALSE,							//	�ł��������t
							ANSI_CHARSET,					//	�t�H���g�����Z�b�g
							OUT_DEFAULT_PRECIS,				//	�o�͐��x
							CLIP_DEFAULT_PRECIS,			//	�N���b�s���O���x
							ANTIALIASED_QUALITY,			//	�o�͕i��
							FF_DONTCARE | DEFAULT_PITCH,	//	�s�b�`�ƃt�@�~��(�����Ԋu�Ə���)
							font.face.c_str());				//	�^�C�v�t�F�C�X
		
		assert(hFont);
		HDC hDC = GetDC(0);
		// 0����256�̃R�[�h�̕������ADisplayList��base�Ԗڂ���o�^.
		// wglUseFontBitmaps()�֐��A�g�p���āA���������t�H���g���f�B�X�v���C���X�g�Ɋ����Ă�.
		hOldFont = (HFONT)SelectObject(hDC, hFont);			
		wglUseFontBitmaps(hDC, 0, range, fontBase);		
		SelectObject(hDC, hOldFont);
		DeleteObject(hFont);		
		// fontList�֓o�^
		fontList.insert(std::map<unsigned int, GRFont>::value_type(fontBase, font));	
		base = fontBase;	
	}

	glDisable(GL_LIGHTING);
	glColor3f(((font.color >> 16) & 0xFF) / 255.0,
			  ((font.color >>  8) & 0xFF) / 255.0,
			  ((font.color >>  0) & 0xFF) / 255.0);
	glRasterPos3fv(pos);
	glPushAttrib(GL_LIST_BIT);
	glListBase(base);											// 	�f�B�X�v���C���X�g��n��.	
	glCallLists(str.size(), GL_UNSIGNED_BYTE, str.c_str());		// �������n��.
	glPopAttrib();
	glEnable(GL_LIGHTING);
#else	// _MSC_VER	
	// VC�ȊO�̊��ł́Afont���T�|�[�g���Ȃ��B
	DrawFont(pos, str);
#endif	
}
/// �`��̍ގ��̐ݒ�
void GRDeviceGL::SetMaterial(const GRMaterialDesc& mat){
	glMaterialfv(GL_FRONT, GL_AMBIENT,   mat.ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat.diffuse);
	glColor3fv(mat.diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR,  mat.specular);
	glMaterialfv(GL_FRONT, GL_EMISSION,  mat.emissive);
	glMaterialf (GL_FRONT, GL_SHININESS, mat.power);
	if (mat.texname.length()){
		if (mat.Is3D()){
			int texId = LoadTexture(mat.texname);
			glDisable(GL_TEXTURE_2D);
			glEnable(GL_TEXTURE_3D);
			glBindTexture(GL_TEXTURE_3D, texId);
			if (enableDebugMessage){ std::cout << "1: glBindTexture(GL_TEXTURE_3D, " << texId << ");" << std::endl; }
		}else{
			int texId = LoadTexture(mat.texname);
			// std::cout << " SetMaterial : id : " << texId << std::endl;
			glDisable(GL_TEXTURE_3D);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texId);
			if (enableDebugMessage){ std::cout << "2: glBindTexture(GL_TEXTURE_2D, " << texId << ");" << std::endl; }
		}
	}else{
		glBindTexture(GL_TEXTURE_3D, 0);
		if (enableDebugMessage){ std::cout << "3: glBindTexture(GL_TEXTURE_3D, " << 0 << ");" << std::endl; }
		glBindTexture(GL_TEXTURE_2D, 0);
		if (enableDebugMessage){ std::cout << "4: glBindTexture(GL_TEXTURE_2D, " << 0 << ");" << std::endl; }
	}
	currentMaterial = mat;
}
/// �`�悷��_�E���̑����̐ݒ�
void GRDeviceGL::SetLineWidth(float w){
	glLineWidth(w);
}
/// �����X�^�b�N��Push
void GRDeviceGL::PushLight(const GRLightDesc& light){
	if (nLights < GL_MAX_LIGHTS) {
		glLightfv(GL_LIGHT0+nLights, GL_AMBIENT, light.ambient);
		glLightfv(GL_LIGHT0+nLights, GL_DIFFUSE, light.diffuse);
		glLightfv(GL_LIGHT0+nLights, GL_SPECULAR, light.specular);
		glLightfv(GL_LIGHT0+nLights, GL_POSITION, light.position);
		if (light.position.W()){
			// attenuation factor = 1/(GL_CONSTANT_ATTENUATION + GL_LINEAR_ATTENUATION * d + GL_QUADRATIC_ATTENUATION * d^2)
			glLightf(GL_LIGHT0+nLights, GL_CONSTANT_ATTENUATION, light.attenuation0);		// 	��茸����
			glLightf(GL_LIGHT0+nLights, GL_LINEAR_ATTENUATION, light.attenuation1);			//	���`������
			glLightf(GL_LIGHT0+nLights, GL_QUADRATIC_ATTENUATION, light.attenuation2);		//	2��������
			glLightfv(GL_LIGHT0+nLights, GL_SPOT_DIRECTION, light.spotDirection);
			glLightf(GL_LIGHT0+nLights, GL_SPOT_EXPONENT, light.spotFalloff);
			glLightf(GL_LIGHT0+nLights, GL_SPOT_CUTOFF, light.spotCutoff);
		}
		glEnable(GL_LIGHT0+nLights);
	}
	nLights ++;
}
/// �����X�^�b�N��Pop
void GRDeviceGL::PopLight(){
	nLights--;
	if (nLights < GL_MAX_LIGHTS) glDisable(GL_LIGHT0+nLights);
}
/// �f�v�X�o�b�t�@�ւ̏������݂�����/�֎~����
void GRDeviceGL::SetDepthWrite(bool b){
	glDepthMask(b);
}
/// �f�v�X�e�X�g��L��/�����ɂ���
void GRDeviceGL::SetDepthTest(bool b){
if (b) glEnable(GL_DEPTH_TEST);
	else glDisable(GL_DEPTH_TEST);
}
/// �f�v�X�o�b�t�@�@�ɗp���锻��������w�肷��
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
/// �A���t�@�u�����f�B���O��L��/�����ɂ���
void GRDeviceGL::SetAlphaTest(bool b){
if (b) glEnable(GL_BLEND);
	else glDisable(GL_BLEND);
}
/// �A���t�@�u�����f�B���O�̃��[�h�ݒ�(SRC�̍����W��, DEST�����W��)
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
void GRDeviceGL::SetLighting(bool on){
	if (on) glEnable(GL_LIGHTING);
	else glDisable(GL_LIGHTING);
}

void GRDeviceGL::SetTextureImage(const std::string id, int components, int xsize, int ysize, int format, char* tb){
	unsigned int texId=0;

	char *texbuf = NULL;
	char *texbuf2 = NULL;
	int tx=0, ty=0, tz=0, nc=0;

	GRTexnameMap::iterator it = texnameMap.find(id);
	if (it != texnameMap.end()) {
		texId = it->second;

		float bx = log((float)xsize)/log(2.0f);
		bx = (int)bx + ((bx - (int)bx) > 0.5);
		float by = log((float)ysize)/log(2.0f);
		by = (int)by + ((by - (int)by) > 0.5);

		tx = (int)(pow(2.0f, bx)) ; ty = (int)(pow(2.0f, by)); nc = 4;
		texbuf  = DBG_NEW char[tx*ty*nc];

		glDisable(GL_TEXTURE_3D);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texId);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		gluScaleImage((GLenum)format, xsize, ysize, GL_UNSIGNED_BYTE, tb, tx, ty, GL_UNSIGNED_BYTE, texbuf);

		int rv = gluBuild2DMipmaps(GL_TEXTURE_2D, nc, tx, ty, (GLenum)format, GL_UNSIGNED_BYTE, texbuf);
		if (rv){DSTR << "SetTextureImage :" << gluErrorString(rv) << std::endl;}
		glBindTexture(GL_TEXTURE_2D, 0);

		delete texbuf;
	}
}

/// �e�N�X�`���̃��[�h�i�߂�l�F�e�N�X�`��ID�j	
static const GLenum	pxfm[] = {GL_LUMINANCE, GL_LUMINANCE_ALPHA, GL_BGR_EXT, GL_BGRA_EXT};
static boost::regex Tex3DRegex("^(.*_tex3d_)([0-9]+)(\\Q.\\E[^\\Q.\\E]+)$");
unsigned int GRDeviceGL::LoadTexture(const std::string filename){
	if (enableDebugMessage) { std::cout << "GRDeviceGL::LoadTexture(" << filename.c_str() << ");" << std::endl; }
	GRTexnameMap::iterator it = texnameMap.find(filename);
	if (it != texnameMap.end()) return it->second;

	// �t�@�C��������Ȃ� return 0;
	if (filename.empty()) return 0;

	char *texbuf = NULL;
	int tx=0, ty=0, tz=0, nc=0;
	unsigned int texId=0;

	boost::smatch results;
	if (boost::regex_search(filename, results, Tex3DRegex)) {
		//	3D texture�̏ꍇ
		//	�t�@�C���̐��𒲂ׂ�
		for(tz=0; ; ++tz){
			std::ostringstream fnStr;
			fnStr << results.str(1)
				<< std::setfill('0') << std::setw(results.str(2).length()) << tz
				<< results.str(3);
			if (_access(fnStr.str().c_str(), 0) != 0) break;
		}
		//	�摜�T�C�Y�𒲂ׂ�
		int h = LoadBmpCreate(filename.c_str());
		tx = LoadBmpGetWidth(h);
		ty = LoadBmpGetHeight(h);
		nc = LoadBmpGetBytePerPixel(h);
		int pictureSize = tx*ty*nc;
		texbuf = DBG_NEW char[pictureSize * tz];
		LoadBmpRelease(h);
		//	�t�@�C���̃��[�h
		for(int i=0; i<tz; ++i){
			std::ostringstream fnStr;
			fnStr << results.str(1)
				<< std::setfill('0') << std::setw(results.str(2).length()) << tz
				<< results.str(3);
			int h = LoadBmpCreate(fnStr.str().c_str());
			int x = LoadBmpGetWidth(h);
			int y = LoadBmpGetHeight(h);
			int c = LoadBmpGetBytePerPixel(h);
			if (x!=tx || y!=ty || c!=nc){
				DSTR << "Error: Texture file '" << fnStr.str() << "' has an illegal format." << std::endl;
				return 0;
			}
			LoadBmpGetBmp(h, texbuf+pictureSize*i);
			LoadBmpRelease(h);	
		}
		// �e�N�X�`���̐����D
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_TEXTURE_3D);
		glGenTextures(1, (GLuint *)&texId);
		glBindTexture(GL_TEXTURE_3D, texId);
		if (enableDebugMessage){ std::cout << "7: glBindTexture(GL_TEXTURE_3D, " << texId << ");" << std::endl; }
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage3D(GL_TEXTURE_3D, 0, nc, tx, ty, tz, 0, pxfm[nc - 1], GL_UNSIGNED_BYTE, texbuf);

		glBindTexture(GL_TEXTURE_3D, 0);
		if (enableDebugMessage){ std::cout << "8: glBindTexture(GL_TEXTURE_3D, " << 0 << ");" << std::endl; }

		delete texbuf;
	}else{	
		//	2D texture�̏ꍇ
		
		bool loadFromFile = (filename.c_str()[0]!=':');
		
		if (loadFromFile) { 
			// paintLib �Ńt�@�C�������[�h�D
			int h = LoadBmpCreate(filename.c_str());
			tx = LoadBmpGetWidth(h);
			ty = LoadBmpGetHeight(h);
			nc = LoadBmpGetBytePerPixel(h);
			texbuf = DBG_NEW char[tx*ty*nc];
			LoadBmpGetBmp(h, texbuf);
			LoadBmpRelease(h);
		}

		// �e�N�X�`���̐����D
		glDisable(GL_TEXTURE_3D);
		glEnable(GL_TEXTURE_2D);
		glGenTextures(1, (GLuint *)&texId);
		glBindTexture(GL_TEXTURE_2D, texId);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

		if (loadFromFile) {
			int rv = gluBuild2DMipmaps(GL_TEXTURE_2D, nc, tx, ty, pxfm[nc - 1], GL_UNSIGNED_BYTE, texbuf);
			delete texbuf;
			if (rv){
				DSTR << gluErrorString(rv) << std::endl;
				return 0;
			}
		}
		glBindTexture(GL_TEXTURE_2D, 0);

	}
	texnameMap[filename] = texId;
	return texId;
}













#if 0		//	���_�V�F�[�_�[���g���Ē��_�u�����h�����鎞�̎Q�l�p
/// �C���f�b�N�X�`���ɂ��V�F�[�_��K�p���� DisplayList �̍쐬�iSetVertexFormat() ����� SetShaderFormat() �̌�ɌĂԁj
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
	
	if ((shaderType == GRShaderFormat::shP3fB4f)		// ����GRShaderFormat�͖��Ή�	
			|| (shaderType == GRShaderFormat::shC4bP3fB4f)
			|| (shaderType == GRShaderFormat::shC3fP3fB4f))			
	{
		glUseProgram(shader);

		// ���P�[�V�����^�ւ̃L���X�g
		GRShaderFormat::SFBlendLocation *loc = (GRShaderFormat::SFBlendLocation *)location;
		if (loc) {
			glUniformMatrix4fv(loc->uBlendMatrix, 4, false, (GLfloat *)&*blendMatrix.begin());
			
			glEnableVertexAttribArray(loc->aWeight); 
			glEnableVertexAttribArray(loc->aMatrixIndices); 
			glEnableVertexAttribArray(loc->aNumMatrix); 
			
			// vtx�𒸓_�t�H�[�}�b�g�^�փL���X�g
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




/// �V�F�[�_�̏�����	
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
/// �V�F�[�_�t�H�[�}�b�g�̐ݒ�
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
/// �V�F�[�_�I�u�W�F�N�g�̍쐬	
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
/// �V�F�[�_�I�u�W�F�N�g�̍쐬�AGRDeviceGL::shader�ւ̓o�^�i���炩����ShaderFile����o�^���Ă����K�v������j	
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
/// �V�F�[�_�̃\�[�X�v���O�������������ɓǂݍ��݁A�V�F�[�_�I�u�W�F�N�g�Ɗ֘A�t����	
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
/// ���P�[�V�������̎擾�iSetShaderFormat()�ŃV�F�[�_�t�H�[�}�b�g��ݒ肵�Ă����K�v����j
void GRDeviceGL::GetShaderLocation(GRHandler shader, void* location){
	if ((shaderType == GRShaderFormat::shP3fB4f)
			|| (shaderType == GRShaderFormat::shC4bP3fB4f)
			|| (shaderType == GRShaderFormat::shC3fP3fB4f))
	{
		// shaderType ���AshP3fB4f �܂��� shC4bP3fB4f �܂��� shC3fP3fB4f �ŁA
		// ���P�[�V������� �� SFBlendLocation �̌^���w�肳�ꂽ�ꍇ
		SFBlendLocation *loc = (SFBlendLocation *)location;
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

