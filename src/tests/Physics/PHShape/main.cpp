#include <Springhead.h>		//	Springhead�̃C���^�t�F�[�X
#pragma hdrstop
using namespace Spr;

PHSdkIf* sdk;
PHSceneIf* scene;
PHSolidIf* solid1, *solid2;


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
//		std::cout << solid1->GetOrientation() << std::endl;
	}
	
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

	//	SDK�͊J�����Ȃ��Ă��ǂ��D���Ȃ��Ă�main�𔲂��Ă���J�������D
	delete sdk;

	//	���������[�N�̃e�X�g�D
	//	�f�o�b�O����Ŏ��s����ƃ��������[�N��VC�̃f�o�b�O�o�͂ɕ\�������D
	char* memoryLeak = new char[123];
}
