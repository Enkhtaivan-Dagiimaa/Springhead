#include "Graphics.h"
#pragma hdrstop
#include <gl/glut.h>

namespace Spr {;
//----------------------------------------------------------------------------
//	GRDeviceGL
OBJECTIMP(GRDeviceGL, GRDevice);
IF_IMP(GRDeviceGL, GRDevice);
/// �����ݒ�
void GRDeviceGL::Init(){
	glDrawBuffer(GL_BACK);
	glEnable(GL_LIGHTING);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_BLEND);
	// ���_�s��̐ݒ�
	viewMatrix.Pos() = Vec3f(0.0, 5.0, 10.0);	                        // eye
	viewMatrix.LookAtGL(Vec3f(0.0, 0.0, 0.0), Vec3f(0.0, 1.0, 0.0));	// center, up 
	viewMatrix = viewMatrix.inv();			
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(viewMatrix);
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
	glPushMatrix();
}
/// ���f���s��̍s��X�^�b�N��Pop
void GRDeviceGL::PopModelMatrix(){
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
void GRDeviceGL::DrawDirect(TPrimitiveType ty, Vec3f* begin, Vec3f* end){
	GLenum mode;
	switch(ty) {
		case POINTS: 
			mode = GL_POINTS;
			break;
		case LINES:
			mode = GL_LINES;
			break;
		case LINE_STRIP:
			mode = GL_LINE_STRIP;
			break;
		case TRIANGLES:
			mode = GL_TRIANGLES;
			break;
		case TRIANGLE_STRIP:
			mode = GL_TRIANGLE_STRIP;
			break;
		case TRIANGLE_FAN:
			mode = GL_TRIANGLE_FAN;
			break;
		default:
			/* DO NOTHING */
			break;
	}
	glBegin(mode);
	for(;begin!=end; ++begin)
		glVertex3fv(*begin);
	glEnd();
}
/// ���_���W�ƃC���f�b�N�X���w�肵�ăv���~�e�B�u��`��
void GRDeviceGL::DrawIndexed(TPrimitiveType ty, size_t* begin, size_t* end, Vec3f* vtx){
	GLenum mode;
	switch(ty) {
		case POINTS: 
			mode = GL_POINTS;
			break;
		case LINES:
			mode = GL_LINES;
			break;
		case LINE_STRIP:
			mode = GL_LINE_STRIP;
			break;
		case TRIANGLES:
			mode = GL_TRIANGLES;
			break;
		case TRIANGLE_STRIP:
			mode = GL_TRIANGLE_STRIP;
			break;
		case TRIANGLE_FAN:
			mode = GL_TRIANGLE_FAN;
			break;
		default:
			/* DO NOTHING */
			break;
	}
	glBegin(mode);
	for(;begin!=end; ++begin) glVertex3fv(vtx[*begin]);
	glEnd();
}


void GRDeviceGL::DrawText(Vec2f pos, std::string str, const GRFont& font){}
void GRDeviceGL::SetMaterial(const GRMaterial& mat){}
void GRDeviceGL::SetLineWidth(float w){}
void GRDeviceGL::PushLight(const GRLight& m){}
void GRDeviceGL::PopLight(){}
void GRDeviceGL::SetDepthWrite(bool b){}
void GRDeviceGL::SetDepthTest(bool b){}
void GRDeviceGL::SetDepthFunc(TDepthFunc f){}
void GRDeviceGL::SetAlphaMode(TBlendFunc src, TBlendFunc dest){}

}	//	Spr

