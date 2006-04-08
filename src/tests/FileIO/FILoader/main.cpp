/**
 Springhead2/src/tests/FileIO/FILoader/main.cpp

�y�T�v�z
  X�t�@�C�������[�h���APhysics�G���W���Ɛڑ����ăV�~�����[�V��������B
  
�y�I����z
  �E�v���O����������I��������0��Ԃ��B  
 
�y�����̗���z
  �Etest.x�t�@�C�������[�h����B
  �E���[�h���������o�͂���B
  �EPhysics�G���W���Ɛڑ����A�V�~�����[�V����������B
  
 */
#include <FileIO/FITypeDesc.h>
#include <FileIO/FIFileContext.h>
#include <FileIO/FIFileX.h>
#include <Springhead.h>
#ifdef __APPLE__
# include <GLUT/glut.h>
#else
# include <GL/glut.h>
#endif
#define	ESC		27
namespace Spr{
	PHSdkIf* phSdk;
	PHSceneIf** scene;
	CDShapeIf** shape;
	GRSdkIf* grSdk;
	GRDeviceGLIf* grDevice;
	GRDebugRenderIf* render;
}
using namespace Spr;


// �ގ��̐ݒ�
GRMaterial matFloor(Vec4f(0.0, 0.0, 0.0, 1.0),		// ambient
					Vec4f(0.9, 0.0, 0.0, 1.0),		// diffuse
					Vec4f(1.0, 1.0, 1.0, 1.0),		// specular
					Vec4f(0.0, 0.0, 0.0, 1.0),		// emissive
					20.0);							// power

GRMaterial matBox(Vec4f(0.0, 0.0, 0.0, 1.0),		// ambient
					Vec4f(0.0, 0.2, 0.8, 1.0),		// diffuse
					Vec4f(1.0, 1.0, 1.0, 1.0),		// specular
					Vec4f(0.0, 0.0, 0.0, 1.0),		// emissive
					20.0);							// power

/**
 brief     	glutDisplayFunc�Ŏw�肵���R�[���o�b�N�֐�
 param	 	�Ȃ�
 return 	�Ȃ�
 */
void display(){
	//	�o�b�t�@�N���A
	render->ClearBuffer();
	render->SetMaterial(matFloor);	

	PHSceneIf* scene = NULL;
	if (phSdk->NScene()){
		PHSceneIf** s = phSdk->GetScenes();
		scene = *s;
	}
	PHSolidIf **solids = scene->GetSolids();
	for (int num=0; num < scene->NSolids(); ++num){
		if (num == 1){
			render->SetMaterial(matBox);	
		}
		Affinef af;
		solids[num]->GetPose().ToAffine(af);
		render->PushModelMatrix();
		render->MultModelMatrix(af);
		CDShapeIf** shapes = solids[num]->GetShapes();
		for(int s=0; s<solids[num]->NShape(); ++s){
			Affinef af;
			shapes[s]->GetPose().ToAffine(af);
			render->PushModelMatrix();
			render->MultModelMatrix(af);
			CDConvexMeshIf* mesh = ICAST(CDConvexMeshIf, shapes[s]);
			Vec3f* base = mesh->GetVertices();
			for (size_t f=0; f<mesh->NFace(); ++f) {	
				CDFaceIf* face = mesh->GetFace(f);
				render->DrawFace(face, base);
			}
			render->PopModelMatrix();
		}
		render->PopModelMatrix();
	}
/*

	render->SetMaterial(material);	
	for (int num=0; num < phSdk->NShape(); ++num){
		CDShapeIf **shapes = phSdk->GetShapes();
		CDConvexMeshIf* mesh = ICAST(CDConvexMeshIf, shapes[num]);
		Vec3f* base = mesh->GetVertices();
		for (size_t f=0; f<mesh->NFace(); ++f) {	
			CDFaceIf* face = mesh->GetFace(f);
			render->DrawFace(face, base);
		}
	}
*/
	render->EndScene();
}
/**
 brief     	�����̐ݒ�
 param	 	�Ȃ�
 return 	�Ȃ�
 */
void setLight() {
	GRLight light0;
	light0.ambient	= Vec4f(0.0, 0.0, 0.0, 1.0);
	light0.diffuse	= Vec4f(0.9, 0.9, 0.9, 1.0);
	light0.specular	= Vec4f(1.0, 1.0, 1.0, 1.0);
	light0.position = Vec4f(2.0, 0.0, 3.0, 1.0);
	render->PushLight(light0);

}

/**
 brief  	glutReshapeFunc�Ŏw�肵���R�[���o�b�N�֐�
 param	 	<in/--> w�@�@��
 param  	<in/--> h�@�@����
 return 	�Ȃ�
 */
void reshape(int w, int h){
	render->Reshape(Vec2f(w,h));
}

/**
 brief 		glutKeyboardFunc�Ŏw�肵���R�[���o�b�N�֐� 
 param		<in/--> key�@�@ ASCII�R�[�h
 param 		<in/--> x�@�@�@ �L�[�������ꂽ���̃}�E�X���W
 param 		<in/--> y�@�@�@ �L�[�������ꂽ���̃}�E�X���W
 return 	�Ȃ�
 */
void keyboard(unsigned char key, int x, int y){
	if (key == ESC) exit(0);
}

/**
 brief  	glutIdleFunc�Ŏw�肵���R�[���o�b�N�֐�
 param	 	�Ȃ�
 return 	�Ȃ�
 */
void idle(){
	if(scene && *scene) (*(scene))->Step();
	glutPostRedisplay();
	static int count;
	count ++;
	if (count > 5000) exit(0);
}

/**
 brief		���C���֐�
 param		<in/--> argc�@�@�R�}���h���C�����͂̌�
 param		<in/--> argv�@�@�R�}���h���C������
 return		0 (����I��)
 */

int main(int argc, char* argv[]){
	static FIFileContext fc;
	fc.errorStream=&DSTR;
	fc.fileInfo.Push();
	fc.fileInfo.back().Map("test.x");
	phSdk = CreatePHSdk();
	fc.objects.Push(phSdk);
	static FIFileX fileX;	//<	glut�� exit�ŏI��邽�߁Cstatic�ɂ��Ȃ��Ƃ��̕ϐ���Cleanup���s���Ȃ��Ȃ�D
	fileX.Init();
	fileX.Load(&fc);
	phSdk->Print(DSTR);

	scene = phSdk->GetScenes();		// Scene�̎擾
	shape = phSdk->GetShapes();		// Shape�̎擾
	DSTR << "Loaded : " << "NScene=" << phSdk->NScene() << ", NSphape=" << phSdk->NShape() << std::endl;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	int window = glutCreateWindow("FILoad");

	// Graphics Sdk
	grSdk = CreateGRSdk();
	render = grSdk->CreateDebugRender();
	grDevice = grSdk->CreateDeviceGL(window);
	grDevice->Init();
	render->SetDevice(grDevice);

	// ���_�ݒ�
	Affinef view;
	view.Pos() = Vec3f(2.0, 2.0, 3.0);								// eye
		view.LookAtGL(Vec3f(0.0, 0.0, 0.0), Vec3f(0.0, 1.0, 0.0));		// center, up 
	view = view.inv();	
	render->SetViewMatrix(view);

	// �����ݒ�
	setLight();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutMainLoop();		// ����ǉ�����Ƒ�ʂ�memory leak����������

	return 0;
}

