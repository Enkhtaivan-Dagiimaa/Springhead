/** \page pageFramework �t���[�����[�NSDK 


\contents

\section sec_FWSdkAbst �t���[�����[�NSDK�̊T�v

�t���[�����[�N\c SDK �́C\ref pagePhysics \ref pageGraphics \ref pageFileIO
�𓝍����C�A�v���P�[�V�����Ƃ��ē��삳���邽�߂̃t���[�����[�N��񋟂���SDK�ł��D

����SDK�Ƃ̊֌W��}�ɂ���Ǝ��̂悤�ɂȂ�܂��D
\image html FWScene.png

FWObject �́C�O���t�B�N�X�𕨗��ɓ����C�܂� GRFrame�̈ʒu�p����PHSolid�̈ʒu�p���ɓ��������邽�߂̃I�u�W�F�N�g�ł��D
FWScene �́C�O���t�B�N�X�ƕ����̃V�[���������Ă܂Ƃ߂����������܂��D

FWSdk�́CFileIOSDK��p���ăt�@�C�������[�h������C�Z�[�u����������܂��D
�܂��C�����_���̊Ǘ���GLUT�Ȃǂ̃R�[���o�b�N�̏����Ȃǂ��s�����Ƃ��ł��܂��D

\section sec_FWSdkSample �t���[�����[�NSDK���g�����A�v���P�[�V����
FWSdk�̓t�@�C�������[�h���ăV�~�����[�V���������Ȃ���O���t�B�N�X�ŕ\������Ƃ����悤�ȁC
�T�^�I�ȃA�v���P�[�V���������Ƃ��ɖ𗧂��܂��D

�ȉ��̃\�[�X�́C�t�@�C�� scene.x �����[�h���ăV�~�����[�V��������T���v���v���O�����ł��D
\verbatim
#include <Springhead.h>
#include <GL/glut.h>
using namespace Spr;

UTRef<FWSdkIf> fwSdk;

void Display() {
	fwSdk->Draw();
	glutSwapBuffers();
}

void Timer(int id) {
	fwSdk->Step();	// �V�~�����[�V�����̃X�e�b�v
	glutPostRedisplay();
	glutTimerFunc(20, Timer, 0);
}
void Reshape(int w, int h){
	fwSdk->Reshape(w,h);
}
void Keyboard(unsigned char key, int x, int y){
	if (key == 27) exit(0);
}

int main(int argc, char* argv[]) {
	fwSdk = FWSdkIf::CreateSdk();
	fwSdk->LoadScene("scene.x");	//	�t�@�C�������[�h

	// OpenGL�̏�����
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Springhead with framework");
	fwSdk->CreateRender();

	// GLUT�̃R�[���o�b�N�̓o�^
	glutKeyboardFunc(Keyboard);
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutTimerFunc(20, Timer, 0);

	// ���C�����[�v�J�n
	glutMainLoop();
	return 0;
}
\endverbatim

\section sec_FWApp �A�v���P�[�V�����N���X
�t���[�����[�NSDK�́C���[�U�A�v���P�[�V�������������߂̊�{�N���X�Ƃ��Ďg���ƕ֗���
FWApp �N���X�Ƃ��̔h���N���X���񋟂��Ă��܂��D

�����́CSpringhead��API�N���X�ł͂Ȃ��C�ʏ��C++�̃N���X��`�ɂȂ��Ă��܂��D
���[�U�͂������p�����āC���z�֐���K�v�ɉ����ăI�[�o�[���C�h���邱�ƂŁC
�Ǝ��̃A�v���P�[�V��������邱�Ƃ��ł��܂��D

�y�Q�l�z
\ref pageFWAppGLUT
���g�����A�v���P�[�V�����쐬
\ref pageFWAppGLUI
���g�����A�v���P�[�V�����쐬

*/
