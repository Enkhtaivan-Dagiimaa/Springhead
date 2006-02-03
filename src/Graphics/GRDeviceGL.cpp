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
	size = Vec2f(480, 360);
	center = Vec2f(0.0, 0.0);
	near = 1.0;
	far = 500.0;

	//std::count << mocomoco << std::endl;

	//camera.mocomoco = 10;

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
#if 0
   GLdouble m[16];
//static void
void  
frustum(GLdouble left, GLdouble right,
        GLdouble bottom, GLdouble top, 
        GLdouble nearval, GLdouble farval)
{
   GLdouble x, y, a, b, c, d;


   x = (2.0 * nearval) / (right - left);
   y = (2.0 * nearval) / (top - bottom);
   a = (right + left) / (right - left);
   b = (top + bottom) / (top - bottom);
   c = -(farval + nearval) / ( farval - nearval);
   d = -(2.0 * farval * nearval) / (farval - nearval);

#define M(row,col)  m[col*4+row]
   M(0,0) = x;     M(0,1) = 0.0F;  M(0,2) = a;      M(0,3) = 0.0F;
   M(1,0) = 0.0F;  M(1,1) = y;     M(1,2) = b;      M(1,3) = 0.0F;
   M(2,0) = 0.0F;  M(2,1) = 0.0F;  M(2,2) = c;      M(2,3) = d;
   M(3,0) = 0.0F;  M(3,1) = 0.0F;  M(3,2) = -1.0F;  M(3,3) = 0.0F;
#undef M

   return m;
}

void Perspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
   GLdouble xmin, xmax, ymin, ymax;

   ymax = zNear * tan(fovy * M_PI / 360.0);
   ymin = -ymax;
   xmin = ymin * aspect;
   xmax = ymax * aspect;

   /* don't call glFrustum() because of error semantics (covglu) */
   frustum(xmin, xmax, ymin, ymax, zNear, zFar);
}
#endif
/// Viewport�Ǝˉe�s���ݒ�
void GRDeviceGL::Resize(Vec2f screen){
	glViewport(0, 0, static_cast<GLsizei>(screen.x), static_cast<GLsizei>(screen.y));
#if 1
			// �v�C���FResize()�́CGRRender�����������Ă���֐��ɂ��āC
			//device��SetProjectionMatrix���Ăяo���悤�ɂ���Ηǂ��Ǝv���܂��D
	projectionMatrix = Affinef::ProjectionGL(Vec3f(center.x, center.y, screen.x/2.0),	// ����p90�x
											//Vec2f(size.x, size.x*screen.y/screen.x),
											Vec2f(size.x, size.y),
											near, 
											far);

	/*projectionMatrix = Affinef::ProjectionGL(Vec3f(center.x, center.y, screen.x/2.0),	// ����p90�x
											Vec2f(screen.x, screen.x*screen.y/screen.x),
											near, 
											far);*/

	GLdouble xmin, xmax, ymin, ymax;
	GLdouble fovy = 30.0;
	GLdouble aspect = static_cast<GLfloat>(screen.x)/static_cast<GLfloat>(screen.y);
	GLdouble zNear = 1.0;
	GLdouble zFar = 500.0;

   ymax = zNear * tan(fovy * M_PI / 360.0);
   ymin = -ymax;
   xmin = ymin * aspect;
   xmax = ymax * aspect;

	
   GLdouble left = xmin;
   GLdouble right = xmax;
   GLdouble bottom = ymin;
	GLdouble top = ymax;
    GLdouble nearval = zNear;
	GLdouble farval = zFar;

   GLdouble x, y, a, b, c, d;


   x = (2.0 * nearval) / (right - left);
   y = (2.0 * nearval) / (top - bottom);
   a = (right + left) / (right - left);
   b = (top + bottom) / (top - bottom);
   c = -(farval + nearval) / ( farval - nearval);
   d = -(2.0 * farval * nearval) / (farval - nearval);

  

   projectionMatrix.xx = x;     projectionMatrix.xy = 0.0F;  projectionMatrix.xw = a;      projectionMatrix.xw = 0.0F;
   projectionMatrix.yx = 0.0F;  projectionMatrix.yy = y;     projectionMatrix.yw = b;      projectionMatrix.yw = 0.0F;
   projectionMatrix.zx = 0.0F;  projectionMatrix.zy = 0.0F;  projectionMatrix.zw = c;      projectionMatrix.zw = d;
   projectionMatrix.px = 0.0F;  projectionMatrix.py = 0.0F;  projectionMatrix.pw = -1.0F;  projectionMatrix.pw = 0.0F;


 



	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(projectionMatrix);
	glMatrixMode(GL_MODELVIEW);

#else	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, static_cast<GLfloat>(screen.x)/static_cast<GLfloat>(screen.y), 1.0, 500.0);
	glMatrixMode(GL_MODELVIEW);
#endif	
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

