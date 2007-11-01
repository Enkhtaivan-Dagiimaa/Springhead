/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 Springhead2/src/tests/Graphics/GRVertexBlending/main.cpp

�y�T�v�z
  �EVertexBlending �̃e�X�g�v���O����

�y���e�z
  �E�e���_���W�ɑ΂��A�ϊ��s���^���A�E�F�C�g�ō�������B
  �E���ꂼ��̃I�u�W�F�N�g�͂R��Quad����\������A
	Blue Quad �ɂ͉E�ɂP���炵���ϊ��s���^���AYellow Quad �ɂ͍��ɂP���炵���ϊ��s���^����B
  �E�܂��A�����珇�ɁAGRVertexElement::VFP3fB4f�AGRVertexElement::VFC3fP3fB4f�AGRVertexElement::VFC4bP3fB4f �̃e�X�g�ł���A
  �@�����_�����O���ʂ́A
  �@	GRVertexElement::VFP3fB4f		�@	�F������Ȃ�R�̃u���b�N
		GRVertexElement::VFC3fP3fB4f		�F�Ή�����Ȃ�R�̃u���b�N
		GRVertexElement::VFC4bP3fB4f		�F�Ή�����Ȃ�R�̃u���b�N
	�ƂȂ�B

 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <GL/glew.h>
#include <Springhead.h>
#include <GL/glut.h>
using namespace Spr;

#define EXIT_TIMER	4000
const int MAX_BLEND_MATRIX = 2;												

UTRef<GRSdkIf> grSdk;
GRDebugRenderIf* render;
GRDeviceGLIf* grDevice;

std::vector<GRVertexElement::VFP3fB4f> vtxP3fB4f;							// data
std::vector<GRVertexElement::VFC3fP3fB4f> vtxC3fP3fB4f;
std::vector<GRVertexElement::VFC4bP3fB4f> vtxC4bP3fB4f;

unsigned int listP3fB4f, listC3fP3fB4f, listC4bP3fB4f;						// display list
GRHandler shaderP3fB4f, shaderC3fP3fB4f, shaderC4bP3fB4f;					// shader program
GRShaderFormat::SFBlendLocation locP3fB4f, locC3fP3fB4f, locC4bP3fB4f;		// shader location

std::vector<size_t> faces;													// data indices

struct VertexC3f
{
	float r, g, b;
	float x, y, z;
	float blends[MAX_BLEND_MATRIX];
	float matrixIndices[MAX_BLEND_MATRIX];
	float numMatrix;				
};
VertexC3f verticesC3f[] =
{
	// r    g    b      x    y    z      w0   w1    mi0   mi1    nb
	{ 0.0f,0.0f,1.0f,  1.0f,6.0f,0.0f,  1.0f,0.0f,  0.0f,0.0f,  1.0f },
	{ 0.0f,0.0f,1.0f, -1.0f,6.0f,0.0f,  1.0f,0.0f,  0.0f,0.0f,  1.0f },
	{ 0.0f,0.0f,1.0f, -1.0f,4.0f,0.0f,  0.5f,0.5f,  0.0f,1.0f,  2.0f }, // Blue Quad �i��j
	{ 0.0f,0.0f,1.0f,  1.0f,4.0f,0.0f,  0.5f,0.5f,  0.0f,1.0f,  2.0f },

	{ 0.0f,1.0f,0.0f,  1.0f,4.0f,0.0f,  0.5f,0.5f,  0.0f,1.0f,  2.0f },
	{ 0.0f,1.0f,0.0f, -1.0f,4.0f,0.0f,  0.5f,0.5f,  0.0f,1.0f,  2.0f },
	{ 0.0f,1.0f,0.0f, -1.0f,2.0f,0.0f,  0.5f,0.5f,  0.0f,1.0f,  2.0f }, // Green Quad�i���j
	{ 0.0f,1.0f,0.0f,  1.0f,2.0f,0.0f,  0.5f,0.5f,  0.0f,1.0f,  2.0f },

	{ 1.0f,1.0f,0.0f,  1.0f,2.0f,0.0f,  0.5f,0.5f,  0.0f,1.0f,  2.0f },
	{ 1.0f,1.0f,0.0f, -1.0f,2.0f,0.0f,  0.5f,0.5f,  0.0f,1.0f,  2.0f },
	{ 1.0f,1.0f,0.0f, -1.0f,0.0f,0.0f,  1.0f,0.0f,  1.0f,0.0f,  1.0f }, // Yellow Quad�i���j
	{ 1.0f,1.0f,0.0f,  1.0f,0.0f,0.0f,  1.0f,0.0f,  1.0f,0.0f,  1.0f }
};

struct VertexC4f
{
	float r, g, b, a;
	float x, y, z;
	float blends[MAX_BLEND_MATRIX];
	float matrixIndices[MAX_BLEND_MATRIX];
	float numMatrix;				
};
VertexC4f verticesC4f[] =
{
	// r    g    b    a        x    y    z     w0   w1    mi0   mi1    nb
	{ 0.0f,0.0f,1.0f,1.0f,   1.0f,6.0f,0.0f,  1.0f,0.0f,  0.0f,0.0f,  1.0f },
	{ 0.0f,0.0f,1.0f,1.0f,  -1.0f,6.0f,0.0f,  1.0f,0.0f,  0.0f,0.0f,  1.0f },
	{ 0.0f,0.0f,1.0f,1.0f,  -1.0f,4.0f,0.0f,  0.5f,0.5f,  0.0f,1.0f,  2.0f }, // Blue Quad �i��j
	{ 0.0f,0.0f,1.0f,1.0f,   1.0f,4.0f,0.0f,  0.5f,0.5f,  0.0f,1.0f,  2.0f },

	{ 0.0f,1.0f,0.0f,1.0f,   1.0f,4.0f,0.0f,  0.5f,0.5f,  0.0f,1.0f,  2.0f },
	{ 0.0f,1.0f,0.0f,1.0f,  -1.0f,4.0f,0.0f,  0.5f,0.5f,  0.0f,1.0f,  2.0f },
	{ 0.0f,1.0f,0.0f,1.0f,  -1.0f,2.0f,0.0f,  0.5f,0.5f,  0.0f,1.0f,  2.0f }, // Green Quad�i���j
	{ 0.0f,1.0f,0.0f,1.0f,   1.0f,2.0f,0.0f,  0.5f,0.5f,  0.0f,1.0f,  2.0f },

	{ 1.0f,1.0f,0.0f,1.0f,   1.0f,2.0f,0.0f,  0.5f,0.5f,  0.0f,1.0f,  2.0f },
	{ 1.0f,1.0f,0.0f,1.0f,  -1.0f,2.0f,0.0f,  0.5f,0.5f,  0.0f,1.0f,  2.0f },
	{ 1.0f,1.0f,0.0f,1.0f,  -1.0f,0.0f,0.0f,  1.0f,0.0f,  1.0f,0.0f,  1.0f }, // Yellow Quad�i���j
	{ 1.0f,1.0f,0.0f,1.0f,   1.0f,0.0f,0.0f,  1.0f,0.0f,  1.0f,0.0f,  1.0f }
};


//------------------------------------------------------------------------------------------------------


/**
 brief  	glutReshapeFunc�Ŏw�肵���R�[���o�b�N�֐�
 param	 	<in/--> w�@�@��
 param  	<in/--> h�@�@����
 return 	�Ȃ�
 */
void reshape(int w, int h){
	render->Reshape(Vec2f(0,0), Vec2f(w,h));
}
/**
 brief     	glutDisplayFunc�Ŏw�肵���R�[���o�b�N�֐�
 param	 	�Ȃ�
 return 	�Ȃ�
 */
void display(void) {
	render->ClearBuffer();
	glLoadIdentity();

	gluLookAt(0.0,3.0,5.0, 
		      0.0,3.0,-1.0,
			  0.0f,1.0f,0.0f);
	
	glPushMatrix();
	glTranslatef(-3.0, 0.0, 0.0);
	render->DrawList(listP3fB4f);
	glPopMatrix();

	render->DrawList(listC3fP3fB4f);

	glPushMatrix();
	glTranslatef(3.0, 0.0, 0.0);
	render->DrawList(listC4bP3fB4f);
	glPopMatrix();

	render->EndScene();
	glutSwapBuffers();
}
/**
 brief 		glutKeyboardFunc�Ŏw�肵���R�[���o�b�N�֐� 
 param		<in/--> key�@�@ ASCII�R�[�h
 param 		<in/--> x�@�@�@ �L�[�������ꂽ���̃}�E�X���W
 param 		<in/--> y�@�@�@ �L�[�������ꂽ���̃}�E�X���W
 return 	�Ȃ�
 */
void keyboard(unsigned char key, int x, int y) {
	if (key == 27) 
		exit(0);
}
/**
 brief  	glutIdleFunc�Ŏw�肵���R�[���o�b�N�֐�
 param	 	�Ȃ�
 return 	�Ȃ�
 */
void idle(){
	glutPostRedisplay();
	static int count=0;
	count++;
	if (count > EXIT_TIMER){
		DSTR << EXIT_TIMER << " count passed." << std::endl;
		exit(0);
	}
}
/**
 brief  	���_�f�[�^�ݒ�֐�
 param	 	�Ȃ�
 return 	�Ȃ�
 */
void SetVertex(){
	int nIndex=0;
	int nIndexOffset=0;	
	for( int j = 0; j < 3; ++j )
	{
		nIndexOffset = j * 4;
		for( int i = 0; i < 4; ++i )
		{
			nIndex = i + nIndexOffset;

			GRVertexElement::VFP3fB4f tmpP3fB4f;
			tmpP3fB4f.p.x = verticesC3f[nIndex].x;
			tmpP3fB4f.p.y = verticesC3f[nIndex].y;
			tmpP3fB4f.p.z = verticesC3f[nIndex].z;
			tmpP3fB4f.b.data[0]  = verticesC3f[nIndex].blends[0];
			tmpP3fB4f.b.data[1]  = verticesC3f[nIndex].blends[1];
			tmpP3fB4f.mi.data[0] = verticesC3f[nIndex].matrixIndices[0];
			tmpP3fB4f.mi.data[1] = verticesC3f[nIndex].matrixIndices[1];
			tmpP3fB4f.nb.data[0] = verticesC3f[nIndex].numMatrix;
			vtxP3fB4f.push_back(tmpP3fB4f);

			GRVertexElement::VFC3fP3fB4f tmpC3fP3fB4f;			
			tmpC3fP3fB4f.c.x = verticesC3f[nIndex].r;
			tmpC3fP3fB4f.c.y = verticesC3f[nIndex].g;
			tmpC3fP3fB4f.c.z = verticesC3f[nIndex].b;
			tmpC3fP3fB4f.p.x = verticesC3f[nIndex].x;
			tmpC3fP3fB4f.p.y = verticesC3f[nIndex].y;
			tmpC3fP3fB4f.p.z = verticesC3f[nIndex].z;
			tmpC3fP3fB4f.b.data[0]  = verticesC3f[nIndex].blends[0];
			tmpC3fP3fB4f.b.data[1]  = verticesC3f[nIndex].blends[1];
			tmpC3fP3fB4f.mi.data[0] = verticesC3f[nIndex].matrixIndices[0];
			tmpC3fP3fB4f.mi.data[1] = verticesC3f[nIndex].matrixIndices[1];
			tmpC3fP3fB4f.nb.data[0] = verticesC3f[nIndex].numMatrix;
			vtxC3fP3fB4f.push_back(tmpC3fP3fB4f);

			GRVertexElement::VFC4bP3fB4f tmpC4bP3fB4f;			
			tmpC4bP3fB4f.c = ((unsigned char)(verticesC4f[nIndex].r*255)) |
			 		   ((unsigned char)(verticesC4f[nIndex].g*255) << 8) |
					   ((unsigned char)(verticesC4f[nIndex].b*255) << 16) |
					   ((unsigned char)(verticesC4f[nIndex].a*255) << 24);
			tmpC4bP3fB4f.p.x = verticesC4f[nIndex].x;
			tmpC4bP3fB4f.p.y = verticesC4f[nIndex].y;
			tmpC4bP3fB4f.p.z = verticesC4f[nIndex].z;
			tmpC4bP3fB4f.b.data[0]  = verticesC4f[nIndex].blends[0];
			tmpC4bP3fB4f.b.data[1]  = verticesC4f[nIndex].blends[1];
			tmpC4bP3fB4f.mi.data[0] = verticesC4f[nIndex].matrixIndices[0];
			tmpC4bP3fB4f.mi.data[1] = verticesC4f[nIndex].matrixIndices[1];
			tmpC4bP3fB4f.nb.data[0] = verticesC4f[nIndex].numMatrix;
			vtxC4bP3fB4f.push_back(tmpC4bP3fB4f);

		}
	}
	
	// ���_�t�H�[�}�b�g�̃f�[�^�^�ɑ΂���C���f�b�N�X�ݒ�
	for (size_t index=0; index<12; ++index){
		faces.push_back(index);
	}
}

/**
 brief  	�V�F�[�_�ݒ�֐�
 param	 	�Ȃ�
 return 	�Ȃ�
 */
void SetShader(){
	// �V�F�[�_�̏�����
	render->InitShader();	

	// �V�F�[�_�I�u�W�F�N�g�쐬�A���P�[�V�������̎擾
	render->SetShaderFormat(GRShaderFormat::shP3fB4f);
	shaderP3fB4f = render->CreateShader();
	render->GetShaderLocation(shaderP3fB4f, &locP3fB4f);

	render->SetShaderFormat(GRShaderFormat::shC3fP3fB4f);
	shaderC3fP3fB4f = render->CreateShader();
	render->GetShaderLocation(shaderC3fP3fB4f, &locC3fP3fB4f);

	render->SetShaderFormat(GRShaderFormat::shC4bP3fB4f);
	shaderC4bP3fB4f = render->CreateShader();
	render->GetShaderLocation(shaderC4bP3fB4f, &locC4bP3fB4f);

	// �u�����h�ϊ��s��̐ݒ�
	Affinef vMatrix0, vMatrix1;
	vMatrix0.data[3][0] = 1.0;		// �E��1�������s�ړ�
	vMatrix1.data[3][0] = -1.0;		// ����1�������s�ړ�	
	render->ClearBlendMatrix();
	render->SetBlendMatrix(vMatrix0);
	render->SetBlendMatrix(vMatrix1);
}
/**
 brief		���C���֐�
 param		<in/--> argc�@�@�R�}���h���C�����͂̌�
 param		<in/--> argv�@�@�R�}���h���C������
 return		0 (����I��)
 */
int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(320,320);
	int window = glutCreateWindow("GRVertexBlending");

	grSdk = GRSdkIf::CreateSdk();
	render = grSdk->CreateDebugRender();
	grDevice = grSdk->CreateDeviceGL();
	render->SetDevice(grDevice);
	grDevice->Init();

	// �f�[�^�ݒ�
	SetVertex();
	// �V�F�[�_�ݒ�
	SetShader();

	// �V�F�[�_��K�p�����f�B�X�v���C���X�g���쐬����
	listP3fB4f = render->StartList();
	render->SetVertexFormat(GRVertexElement::vfP3fB4f);
	render->SetShaderFormat(GRShaderFormat::shP3fB4f);
	render->DrawIndexed(GRRenderIf::QUADS, &*faces.begin(), &*vtxP3fB4f.begin(), faces.size());
	render->EndList();

	listC3fP3fB4f = render->StartList();
	render->SetVertexFormat(GRVertexElement::vfC3fP3fB4f);
	render->SetShaderFormat(GRShaderFormat::shC3fP3fB4f);
	
	listC3fP3fB4f = render->CreateShaderIndexedList(shaderC3fP3fB4f, &locC3fP3fB4f, 
												GRRenderIf::QUADS, &*faces.begin(), &*vtxC3fP3fB4f.begin(), faces.size());	
	render->EndList();


	listC4bP3fB4f = render->StartList();
	render->SetVertexFormat(GRVertexElement::vfC4bP3fB4f);
	render->SetShaderFormat(GRShaderFormat::shC4bP3fB4f);	
	render->DrawIndexed(GRRenderIf::QUADS, &*faces.begin(), &*vtxC4bP3fB4f.begin(), faces.size());
	render->EndList();

 	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);

	glutMainLoop();
	return 0;
}

