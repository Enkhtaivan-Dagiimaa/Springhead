/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/** 
 Springhead2/src/tests/Physics/PHShape/main.cpp

�y�T�v�z
  ����Solid�Ɍ`������������e�X�g�v���O�����i�ʒu���o�́A�ʂ̒��_���W���o�́j
  �E�y�i���e�B�@�ɂ��ʑ��ʑ̓��m�̐ڐG����ƐڐG�͂��m�F����B
  �E���̂����R���������A���̏��2�̃u���b�N��ςݏグ�邱�Ƃ�z�肷��B 
  �E���_���W���f�o�b�O�o�͂����A�����_�����O�͍s��Ȃ��B
  
�y�I����z
  �E�v���O����������I��������0��Ԃ��B
 
�y�����̗���z
  �E�V�~�����[�V�����ɕK�v�ȏ��(���̂̌`��E���ʁE�����e���\���Ȃ�)��ݒ肷��B
  �@���̂̌`���OpenGL�Ŏw�肷��̂ł͂Ȃ��ASolid���̂Ŏ�������B  
  �E�^����ꂽ�����ɂ�臙t�b��̈ʒu�̕ω���2�X�e�b�v�ϕ����A�ʒu���o�͂���B
�@�E�����_�����O�͍s�킸�A�f�o�b�O�o�͂Ƃ��đ��ʑ̖̂�(�O�p�`)�̒��_���W���o�͂���B
     
 */
#include <Springhead.h>		//	Springhead�̃C���^�t�F�[�X
#ifdef USE_HDRSTOP
#pragma hdrstop
#endif
using namespace Spr;

UTRef<PHSdkIf> sdk;
PHSceneIf* scene;
PHSolidIf* solid1, *solid2;

/**
 brief		���C���֐�
 param		<in/--> argc�@�@�R�}���h���C�����͂̌�
 param		<in/--> argv�@�@�R�}���h���C������
 return	0 (����I��)
 */
int main(int argc, char* argv[]){
	sdk = PHSdkIf::CreateSdk();				//	SDK�̍쐬
	scene = sdk->CreateScene();			//	�V�[���̍쐬
	PHSolidDesc desc;
	desc.mass = 2.0;
	desc.inertia *= 2.0;
	solid2 = scene->CreateSolid(desc);	//	���̂�desc�Ɋ�Â��č쐬

	desc.mass = 1e20f;
	desc.inertia *= 1e20f;
	solid1 = scene->CreateSolid(desc);	//	���̂�desc�Ɋ�Â��č쐬
	solid1->SetGravity(false);
	
	//	�`��̍쐬
	CDConvexMeshDesc md;
	md.vertices.push_back(Vec3f(-1,-1,-1));
	md.vertices.push_back(Vec3f(-1,-1, 1));
	md.vertices.push_back(Vec3f(-1, 1,-1));
	md.vertices.push_back(Vec3f(-1, 1, 1));
	md.vertices.push_back(Vec3f( 1,-1,-1));
	md.vertices.push_back(Vec3f( 1,-1, 1));
	md.vertices.push_back(Vec3f( 1, 1,-1));
	md.vertices.push_back(Vec3f( 1, 1, 1));
	CDConvexMeshIf* mesh2 = DCAST(CDConvexMeshIf, sdk->CreateShape(md));

	for(unsigned i=0; i<md.vertices.size(); ++i){
		md.vertices[i].x *= 10;
	}
	CDConvexMeshIf* mesh1 = DCAST(CDConvexMeshIf, sdk->CreateShape(md));

	solid1->AddShape(mesh1);
	solid2->AddShape(mesh2);
	solid1->SetFramePosition(Vec3f(0,-1,0));
	solid2->SetFramePosition(Vec3f(0,2,0));

	scene->SetGravity(Vec3f(0,-9.8f, 0));	// �d�͂�ݒ�

	for(int i=0; i<2; ++i){
		scene->Step();
		std::cout << solid1->GetFramePosition();
		std::cout << solid2->GetFramePosition() << std::endl;
		// std::cout << solid1->GetOrientation() << std::endl;
	}
	
	// �f�o�b�O�o��
	DSTR << "***  solid1  ***\n";
	for(int i=0; i<solid1->NShape();++i){
		CDShapeIf* shape = solid1->GetShape(i);
		CDConvexMeshIf* mesh = DCAST(CDConvexMeshIf, shape);
		Vec3f* base = mesh->GetVertices();
		for(size_t f=0; f<mesh->NFace();++f){
			CDFaceIf* face = mesh->GetFace(f);
			for(int v=0; v<face->NIndex(); ++v){
				DSTR << base[face->GetIndices()[v]];
			}
			DSTR << std::endl;
		}
	}
	DSTR << "***  solid2  ***\n";
	for(int i=0; i<solid2->NShape();++i){
		CDShapeIf* shape = solid2->GetShape(i);
		CDConvexMeshIf* mesh = DCAST(CDConvexMeshIf, shape);
		Vec3f* base = mesh->GetVertices();
		for(size_t f=0; f<mesh->NFace();++f){
			CDFaceIf* face = mesh->GetFace(f);
			for(int v=0; v<face->NIndex(); ++v){
				DSTR << base[face->GetIndices()[v]];
			}
			DSTR << std::endl;
		}
	}
}
