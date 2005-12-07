/** 
 @file  Springhead2/src/tests/Physics/PHShape/main.cpp

@brief ����Solid�Ɍ`������������e�X�g�v���O�����i�ʒu���o�́A�ʂ̒��_���W���o�́j

 <PRE>
 <B>�T�v�F</B>
  �E�y�i���e�B�@�ɂ��ʑ��ʑ̓��m�̐ڐG����ƐڐG�͂��m�F����B
  �E���̂����R���������A���̏��2�̃u���b�N��ςݏグ�邱�Ƃ�z�肷��B 
  �E���_���W���f�o�b�O�o�͂����A�����_�����O�͍s��Ȃ��B
  
 <B>�I����F</B>
  �E�v���O����������I��������0��Ԃ��B
 
 <B>�����̗���F</B>
  �E�V�~�����[�V�����ɕK�v�ȏ��(���̂̌`��E���ʁE�����e���\���Ȃ�)��ݒ肷��B
  �@���̂̌`���OpenGL�Ŏw�肷��̂ł͂Ȃ��ASolid���̂Ŏ�������B  
  �E�^����ꂽ�����ɂ�臙t�b��̈ʒu�̕ω���2�X�e�b�v�ϕ����A�ʒu���o�͂���B
�@�E�����_�����O�͍s�킸�A�f�o�b�O�o�͂Ƃ��đ��ʑ̖̂�(�O�p�`)�̒��_���W���o�͂���B
 </PRE>
     
 */
#include <Springhead.h>		//	Springhead�̃C���^�t�F�[�X
#pragma hdrstop
using namespace Spr;

PHSdkIf* sdk;
PHSceneIf* scene;
PHSolidIf* solid1, *solid2;

/**
 @brief		���C���֐�
 @param		<in/--> argc�@�@�R�}���h���C�����͂̌�
 @param		<in/--> argv�@�@�R�}���h���C������
 @return	0 (����I��)
 */
int main(int argc, char* argv[]){
	sdk = CreatePHSdk();				//	SDK�̍쐬
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
	CDConvexMeshIf* mesh2 = ICAST(CDConvexMeshIf, scene->CreateShape(md));

	for(unsigned i=0; i<md.vertices.size(); ++i){
		md.vertices[i].x *= 10;
	}
	CDConvexMeshIf* mesh1 = ICAST(CDConvexMeshIf, scene->CreateShape(md));

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
	for(int i=0; i<solid1->GetNShapes();++i){
		CDShapeIf** shapes = solid1->GetShapes();
		CDConvexMeshIf* mesh = ICAST(CDConvexMeshIf, shapes[i]);
		Vec3f* base = mesh->GetVertices();
		for(size_t f=0; f<mesh->GetNFaces();++f){
			CDFaceIf* face = mesh->GetFace(f);
			for(int v=0; v<face->GetNIndices(); ++v){
				DSTR << base[face->GetIndices()[v]];
			}
			DSTR << std::endl;
		}
	}
	DSTR << "***  solid2  ***\n";
	for(int i=0; i<solid2->GetNShapes();++i){
		CDShapeIf** shapes = solid2->GetShapes();
		CDConvexMeshIf* mesh = ICAST(CDConvexMeshIf, shapes[i]);
		Vec3f* base = mesh->GetVertices();
		for(size_t f=0; f<mesh->GetNFaces();++f){
			CDFaceIf* face = mesh->GetFace(f);
			for(int v=0; v<face->GetNIndices(); ++v){
				DSTR << base[face->GetIndices()[v]];
			}
			DSTR << std::endl;
		}
	}


	//	SDK�͊J�����Ȃ��Ă��ǂ��D���Ȃ��Ă�main�𔲂��Ă���J�������D
	delete sdk;

}
