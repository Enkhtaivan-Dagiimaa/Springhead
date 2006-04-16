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
#include <Springhead.h>
#include <GL/glut.h>
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
GRMaterial mat_red(Vec4f(0.0, 0.0, 0.0, 1.0),		// ambient
					Vec4f(0.7, 0.0, 0.0, 1.0),		// diffuse
					Vec4f(1.0, 1.0, 1.0, 1.0),		// specular
					Vec4f(0.0, 0.0, 0.0, 1.0),		// emissive
					100.0);							// power

GRMaterial mat_green(Vec4f(0.0, 0.0, 0.0, 1.0),		// ambient
					Vec4f(0.0, 0.7, 0.0, 1.0),		// diffuse
					Vec4f(1.0, 1.0, 1.0, 1.0),		// specular
					Vec4f(0.0, 0.0, 0.0, 1.0),		// emissive
					20.0);							// power

GRMaterial mat_blue(Vec4f(0.0, 0.0, 0.0, 1.0),		// ambient
					Vec4f(0.0, 0.0, 0.7, 1.0),		// diffuse
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
	render->SetMaterial(mat_red);	

	PHSceneIf* scene = NULL;
	if (phSdk->NScene()){
		PHSceneIf** s = phSdk->GetScenes();
		scene = *s;
	}
	if (!scene){
		std::cout << "scene == NULL. File may not found." << std::endl;
		exit(-1);
	}
	PHSolidIf **solids = scene->GetSolids();
	for (int num=0; num < scene->NSolids(); ++num){
		if (num == 1){
			render->SetMaterial(mat_green);	
		} else if (num == 2) {
			render->SetMaterial(mat_blue);	
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
			if (mesh){
				Vec3f* base = mesh->GetVertices();
				for (size_t f=0; f<mesh->NFace(); ++f) {	
					CDFaceIf* face = mesh->GetFace(f);
					render->DrawFace(face, base);
				}
			}
			CDSphereIf* sphere = ICAST(CDSphereIf, shapes[s]);
			if (sphere){
				float r = sphere->GetRadius();
				GLUquadricObj* quad = gluNewQuadric();
				gluSphere(quad, r, 16, 8);
				gluDeleteQuadric(quad);
			}
			render->PopModelMatrix();
		}
		render->PopModelMatrix();
	}

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
	light0.position = Vec4f(0.0, 10.0, -5.0, 1.0);
	render->PushLight(light0);
	
	GRLight light1;
	light1.ambient	= Vec4f(0.0, 0.0, 0.0, 1.0);
	light1.diffuse	= Vec4f(0.9, 0.9, 0.9, 1.0);
	light1.specular	= Vec4f(1.0, 1.0, 1.0, 1.0);
	light1.position = Vec4f(0.0, 5.0, 10.0, 1.0);
	render->PushLight(light1);

	GRLight light2;
	light2.ambient	= Vec4f(0.0, 0.0, 0.0, 1.0);
	light2.diffuse	= Vec4f(0.9, 0.9, 0.9, 1.0);
	light2.specular	= Vec4f(1.0, 1.0, 1.0, 1.0);
	light2.position = Vec4f(-5.0, 5.0, -10.0, 1.0);
	render->PushLight(light2);

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
	if (count > 1200){
		std::cout << "1200 count passed." << std::endl;
		exit(0);
	}
}

/**
 brief		���C���֐�
 param		<in/--> argc�@�@�R�}���h���C�����͂̌�
 param		<in/--> argv�@�@�R�}���h���C������
 return		0 (����I��)
 */

int main(int argc, char* argv[]){
	FISdkIf* fiSdk = CreateFISdk();
	FIFileXIf* fileX = fiSdk->CreateFileX();
	phSdk = CreatePHSdk();
	ObjectIfs objs;
	objs.push_back(phSdk);
	if (argc>=2){
		fileX->Load(objs, argv[1]);
	}else{
		fileX->Load(objs, "test.x");
	}
	fiSdk->Clear();
	objs.clear();
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
	view.Pos() = Vec3f(-2.0, 3.0, 5.0);									// eye
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

