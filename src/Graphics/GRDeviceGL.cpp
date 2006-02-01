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

	size = Vec2f(300, 300);
	center = Vec2f(0.0, 0.0);
	front = 1.0;
	back = 500.0;

	// ���_�s��̐ݒ�
	viewMatrix.Pos() = Vec3f(0.0, 5.0, 10.0);	                        // eye
	viewMatrix.LookAtGL(Vec3f(0.0, 0.0, 0.0), Vec3f(0.0, 1.0, 0.0));	// center, up 
	// �r���[�s��̋t�s������[���h�s��Ƃ���
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
/// Viewport�Ǝˉe�s���ݒ�
void GRDeviceGL::Resize(Vec2f screen){
	glViewport(0, 0, static_cast<GLsizei>(screen.x), static_cast<GLsizei>(screen.y));
	/*Affinef afProj = Affinef::ProjectionGL(Vec3f(center.x, center.y, screen.x/2.0),	// ����p90�x
											Vec2f(size.x, size.x*screen.y/screen.x),
											front, 
											back);*/

	Affinef afProj = Affinef::ProjectionGL(Vec3f(center.x, center.y, screen.x/2.0),	// ����p90�x
											Vec2f(screen.x, screen.x*screen.y/screen.x),
											front, 
											back);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(afProj);
	glMatrixMode(GL_MODELVIEW);
}
/// ���f���s���������
void GRDeviceGL::MultModelMatrix(const Affinef& afw){	
	glMatrixMode(GL_MODELVIEW);
	modelMatrix = modelMatrix * afw;
	glLoadMatrixf(viewMatrix * modelMatrix);
	//glMultMatrixf(afw);
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
void GRDeviceGL::SetViewMatrix(const Affinef& afv){     // ����ł����́H�Ⴄ�C������B�B
	viewMatrix  = afv;		// ���_�s��̕ۑ�
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(viewMatrix * modelMatrix);
}
/// ���e�s���ݒ�
void GRDeviceGL::SetProjectionMatrix(const Affinef& afp){    // <-----------�g���Ċm�F�܂�
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(afp);
	glMatrixMode(GL_MODELVIEW);
}
void GRDeviceGL::DrawDirect(TPrimitiveType ty, Vec3f* begin, Vec3f* end){}
void GRDeviceGL::DrawIndexed(TPrimitiveType ty, size_t* begin, size_t* end, Vec3f* vtx){}
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

