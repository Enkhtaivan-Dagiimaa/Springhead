/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include "Graphics.h"
#ifdef USE_HDRSTOP
#pragma hdrstop
#endif
#ifdef _MSC_VER
# include <WinBasis/WinBasis.h>
# include <windows.h>
#endif
#include <GL/glut.h>

namespace Spr {;
//----------------------------------------------------------------------------
//	GRDeviceGL
IF_OBJECT_IMP(GRDeviceGL, GRDevice);
/// �����ݒ�
void GRDeviceGL::Init(){
	nLights = 0;
	glDrawBuffer(GL_BACK);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_BLEND);
//	glEnable(GL_CULL_FACE);
	glDisable(GL_CULL_FACE);
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
  /// �_�u���o�b�t�@���[�h���A�J�����g�E�B���h�E�̃o�b�t�@�������s��
	glutSwapBuffers();
}
/// ���f���s���������
void GRDeviceGL::MultModelMatrix(const Affinef& afw){	
	glMatrixMode(GL_MODELVIEW);
	glMultMatrixf(afw);
}
/// ���f���s��̍s��X�^�b�N��Push
void GRDeviceGL::PushModelMatrix(){
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
}
/// ���f���s��̍s��X�^�b�N��Pop
void GRDeviceGL::PopModelMatrix(){
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}
/// ���f���s���ݒ�
void GRDeviceGL::SetModelMatrix(const Affinef& afw){
	modelMatrix = afw;		// ���f���s��̕ۑ�
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(viewMatrix * modelMatrix);
}
///	���_�s���ݒ�
void GRDeviceGL::SetViewMatrix(const Affinef& afv){   
	viewMatrix  = afv;		// ���_�s��̕ۑ�
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(viewMatrix);
}
/// ���e�s���ݒ�
void GRDeviceGL::SetProjectionMatrix(const Affinef& afp){  
	projectionMatrix = afp;
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(projectionMatrix);
	glMatrixMode(GL_MODELVIEW);
}

/// ���_���W���w�肵�ăv���~�e�B�u��`��
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
		vertexSize = sizeof(float)*5 + sizeof(char)*4;
		vertexColor = true;
	}else if (e == GRVertexElement::vfT2fN3fP3f){
		vertexFormatGl = GL_T2F_N3F_V3F;
		vertexSize = sizeof(float)*8;
		vertexColor = false;
	}else if (e == GRVertexElement::vfT2fC4fN3fP3f){
		vertexFormatGl = GL_T2F_C4F_N3F_V3F;
		vertexSize = sizeof(float)*12;
		vertexColor = true;
	}else {
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
void GRDeviceGL::DrawDirect(TPrimitiveType ty, void* vtx, size_t count, size_t stride){
	GLenum mode = GL_TRIANGLES;
	switch(ty) {
		case POINTS:			mode = GL_POINTS;			break;
		case LINES:				mode = GL_LINES;			break;
		case LINE_STRIP:		mode = GL_LINE_STRIP;		break;
		case TRIANGLES:			mode = GL_TRIANGLES;		break;
		case TRIANGLE_STRIP:	mode = GL_TRIANGLE_STRIP;	break;
		case TRIANGLE_FAN:		mode = GL_TRIANGLE_FAN;		break;
		case QUADS:				mode = GL_QUADS;			break;
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
void GRDeviceGL::DrawIndexed(TPrimitiveType ty, size_t* idx, void* vtx, size_t count, size_t stride){
	GLenum mode = GL_TRIANGLES;
	switch(ty) {
		case POINTS:			mode = GL_POINTS;			break;
		case LINES:				mode = GL_LINES;			break;
		case LINE_STRIP:		mode = GL_LINE_STRIP;		break;
		case TRIANGLES:			mode = GL_TRIANGLES;		break;
		case TRIANGLE_STRIP:	mode = GL_TRIANGLE_STRIP;	break;
		case TRIANGLE_FAN:		mode = GL_TRIANGLE_FAN;		break;
		case QUADS:				mode = GL_QUADS;			break;
		default:				/* DO NOTHING */			break;
	}
	if (!stride) stride = vertexSize;
	if (vertexColor) glEnable(GL_COLOR_MATERIAL);	
	else glDisable(GL_COLOR_MATERIAL);
	SetMaterial(currentMaterial);
	glInterleavedArrays(vertexFormatGl, stride, vtx);
	glDrawElements(mode, count, GL_UNSIGNED_INT, idx);
}
int GRDeviceGL::CreateList(TPrimitiveType ty, void* vtx, size_t count, size_t stride){
	int list = glGenLists(1);
	glNewList(list, GL_COMPILE);
	DrawDirect(ty, vtx, count, stride);
	glEndList();
	return list;
}
int GRDeviceGL::CreateList(GRMaterialIf* mat, unsigned int texid, 
						   TPrimitiveType ty, void* vtx, size_t count, size_t stride){
	int list = glGenLists(1);
	glNewList(list, GL_COMPILE);
	SetMaterial(*DCAST(GRMaterial, mat));
	if (texid){
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texid);
	}else{
		glDisable(GL_TEXTURE_2D);
	}
	DrawDirect(ty, vtx, count, stride);
	glEndList();
	return list;
}
int GRDeviceGL::CreateList(float radius, int slices, int stacks){
	int list = glGenLists(1);
	glNewList(list, GL_COMPILE);
	glutSolidSphere(radius, slices, stacks);
	glEndList();
	return list;
}
int GRDeviceGL::CreateList(GRMaterialIf* mat, float radius, int slices, int stacks){
	int list = glGenLists(1);
	glNewList(list, GL_COMPILE);
	GRMaterialDesc* desc = DCAST(GRMaterial, mat);
	SetMaterial(*desc);
	unsigned int texid = LoadTexture(desc->texname);
	if (texid){
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texid);
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
	}else{
		glDisable(GL_TEXTURE_2D);
	}		
	glutSolidSphere(radius, slices, stacks);
	glEndList();
	return list;	
}		
int GRDeviceGL::CreateIndexedList(TPrimitiveType ty, size_t* idx, void* vtx, size_t count, size_t stride){
	int list = glGenLists(1);
	glNewList(list, GL_COMPILE);
	DrawIndexed(ty, idx, vtx, count, stride);
	glEndList();
	return list;
}
int GRDeviceGL::CreateIndexedList(GRMaterialIf* mat,  
								  TPrimitiveType ty, size_t* idx, void* vtx, size_t count, size_t stride){
	int list = glGenLists(1);						  
	glNewList(list, GL_COMPILE);
	GRMaterialDesc* desc = DCAST(GRMaterial, mat);
	SetMaterial(*desc);								  	
	unsigned int texid = LoadTexture(desc->texname);
	if (texid){
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texid);
	}else{
		glDisable(GL_TEXTURE_2D);
	}
	DrawIndexed(ty, idx, vtx, count, stride);
	glEndList();
	return list;
}
void GRDeviceGL::DrawList(int list){
	glCallList(list);
}
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
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   mat.ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   mat.diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  mat.specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,  mat.emissive);
	glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, mat.power);
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
void GRDeviceGL::SetDepthFunc(TDepthFunc f){
	GLenum dfunc = GL_LESS;
	switch(f){
		case DF_NEVER:		dfunc = GL_NEVER;		break;
		case DF_LESS:		dfunc = GL_LESS;		break;
		case DF_EQUAL:		dfunc = GL_EQUAL;		break;
		case DF_LEQUAL:		dfunc = GL_LEQUAL;		break;
		case DF_GREATER:	dfunc = GL_GREATER;		break;
		case DF_NOTEQUAL:	dfunc = GL_NOTEQUAL;	break;
		case DF_GEQUAL:		dfunc = GL_GEQUAL;		break;
		case DF_ALWAYS:		dfunc = GL_ALWAYS;		break;
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
void GRDeviceGL::SetAlphaMode(TBlendFunc src, TBlendFunc dest){
	TBlendFunc  bffac[2] = { src, dest };
	GLenum glfac[2];

	for (int iCnt=0; iCnt<2; ++iCnt){
		switch(bffac[iCnt]) {
			case BF_ZERO:				glfac[iCnt] = GL_ZERO;					break;
			case BF_ONE:				glfac[iCnt] = GL_ONE;					break;
			case BF_SRCCOLOR:			glfac[iCnt] = GL_SRC_COLOR;				break;
			case BF_INVSRCCOLOR:		glfac[iCnt] = GL_ONE_MINUS_SRC_COLOR;	break;
			case BF_SRCALPHA:			glfac[iCnt] = GL_SRC_ALPHA;				break;
			case BF_INVSRCALPHA:		glfac[iCnt] = GL_ONE_MINUS_SRC_ALPHA;	break;
			case BF_DESTALPHA:			glfac[iCnt] = GL_DST_ALPHA;				break;
			case BF_INVDESTALPHA:		glfac[iCnt] = GL_ONE_MINUS_DST_ALPHA;	break;
			case BF_DESTCOLOR:			glfac[iCnt] = GL_DST_COLOR;				break;
			case BF_INVDESTCOLOR:		glfac[iCnt] = GL_ONE_MINUS_DST_COLOR;	break;
			case BF_SRCALPHASAT:		glfac[iCnt] = GL_SRC_ALPHA_SATURATE;	break;
			case BF_BOTHINVSRCALPHA:	glfac[iCnt] = 0;						break;
			case BF_BLENDFACTOR:		glfac[iCnt] = 0;						break;
			case BF_INVBLENDFACTOR:		glfac[iCnt] = 0;						break;
			default:					/* DO NOTHING */						break;
		}
	}
	glBlendFunc(glfac[0], glfac[1]);
}
unsigned int GRDeviceGL::LoadTexture(const std::string filename){
	FILE *fp;
	bool iscomment;
	char type[3];
	char str[128];
	char c;
	int r, g, b;
	int i, j, t;
	unsigned char ur, ug, ub;
	int				width  = 0;			///< �e�N�X�`���T�C�Y
	int				height = 0;			///< �e�N�X�`���T�C�Y
	unsigned int	id     = 0;			///< �e�N�X�`��ID
	unsigned char*	data   = NULL;		///< �e�N�X�`���f�[�^(RGB)
	
	// �t�@�C��������Ȃ� return 0;
	if (filename.empty()){
		return 0;
	}

	// open file
	fp = fopen(filename.c_str(), "rb");
	if (!fp){ DSTR << "Error : Can not Open File. " << filename.c_str() << std::endl; }
	else{
		fscanf(fp, "%s\n", &type);
		do {
			iscomment = false;
			c = fgetc(fp);
			ungetc(c, fp);
			if (c == '#'){
				iscomment = true;
				fgets(str, 127, fp);
			}
		} while(iscomment);

		fscanf(fp, "%i %i\n255\n", &width, &height);
		try {
			data = DBG_NEW unsigned char[width * height * 3];	
			if (data == NULL) throw data;
		} catch (...){
			DSTR << "Cannot allocate memory for texture data : " << filename.c_str() << std::endl;
			assert(0);
		}

		if (!strcmp(type, "P3")){								// ascii			
			for (j=0, t=0; j<height; j++){
				for (i=0; i<width; i++, t+=3){
					fscanf(fp, "%i %i %i ", &r, &g, &b);
					data[t]   = (unsigned char)r;
					data[t+1] = (unsigned char)g;
					data[t+2] = (unsigned char)b;
				}
				fscanf(fp, "\n");
			}
		} else if (!strcmp(type, "P6")){						// binary
			for (j=0, t=0; j<height; j++){
				for (i=0; i<width; i++, t+=3){
					fscanf(fp, "%c%c%c", &ur, &ug, &ub);
					data[t]   = (unsigned char)ur;
					data[t+1] = (unsigned char)ug;
					data[t+2] = (unsigned char)ub;
				}
			}
		} else {												// unknown
			DSTR << "unrecognized ppm file type." << std::endl;
			assert(0);
		}
		fclose(fp);

		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
		delete [] data;
	}
	return id;
}
			
}	//	Spr

