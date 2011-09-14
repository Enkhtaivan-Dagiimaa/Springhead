/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
\page pageFEMThermo FEM�ɂ�鉷�x�V�~�����[�V�����̃T���v��
Springhead2/src/Samples/FEMThermo

\section secQuitBoxStack �I���
- Esc��'q'�ŋ����I���B
	
*/
//
#include "../../SampleApp.h"

#pragma hdrstop

using namespace PTM;

using namespace Spr;
using namespace std;



class MyApp : public SampleApp{
public:
	/// �y�[�WID
	enum {
		MENU_MAIN = MENU_SCENE,
	};
	/// �A�N�V����ID
	enum {
		ID_BOX,
		ID_SWITCHRENDER,
	};
	bool debugRender;

public:
	MyApp():debugRender(false){
		appName = "FEMThermo";

		AddAction(MENU_MAIN, ID_BOX, "drop box");
		AddHotKey(MENU_MAIN, ID_BOX, 'b');
		AddAction(MENU_MAIN, ID_SWITCHRENDER, "switch physics(debug)/graphics rendering");
		AddHotKey(MENU_MAIN, ID_SWITCHRENDER, 'd');
	}
	~MyApp(){}

	virtual void Init(int argc, char* argv[]){
		CreateSdk();
		GetSdk()->CreateScene();
		SetGRAdaptee(TypeGLUT);
		GRInit(argc, argv);
		//CreateSdk();

		FWWinDesc windowDesc;
		windowDesc.width = 1024;
		windowDesc.height = 768;
		windowDesc.title = appName;
		CreateWin(windowDesc);
		InitWindow();


		// �`��̍쐬
		CDBoxDesc bd;
		bd.boxsize = Vec3f(2,2,2);
		shapeBox = GetSdk()->GetPHSdk()->CreateShape(bd)->Cast();
		
		CDSphereDesc sd;
		sd.radius = 1;
		shapeSphere = GetSdk()->GetPHSdk()->CreateShape(sd)->Cast();
		
		CDCapsuleDesc cd;
		cd.radius = 1;
		cd.length = 1;
		shapeCapsule = GetSdk()->GetPHSdk()->CreateShape(cd)->Cast();
		
		CDRoundConeDesc rcd;
		rcd.length = 3;
		shapeRoundCone= GetSdk()->GetPHSdk()->CreateShape(rcd)->Cast();
		
		//	�t�@�C���̃��[�h
		UTRef<ImportIf> import = GetSdk()->GetFISdk()->CreateImport();
		GetSdk()->LoadScene("scene.spr", import);			// �t�@�C���̃��[�h
		numScenes = GetSdk()->NScene();
		if (numScenes) SwitchScene(GetSdk()->NScene()-1);

		/// �`��ݒ�
		if (fwScene){
			fwScene->SetWireMaterial(GRRenderIf::WHITE);
			fwScene->SetRenderMode(true, true);				///< �\���b�h�`��C���C���t���[���`��
			fwScene->EnableRenderAxis(false, true, true);		///< ���W��
			fwScene->SetAxisStyle(FWSceneIf::AXIS_LINES);	///< ���W���̃X�^�C��
			fwScene->EnableRenderForce(false, true);			///< ��
			fwScene->EnableRenderContact(true);				///< �ڐG�f��
			phScene = fwScene->GetPHScene();
		}

		//	�Z�[�u�p�̃X�e�[�g�̗p��
		states = ObjectStatesIf::Create();		
		// �^�C�}
		timer = CreateTimer(UTTimerIf::FRAMEWORK);
		timer->SetInterval(10);
	}

	// �`��֐��D�`��v���������Ƃ��ɌĂ΂��
	virtual void OnDraw(GRRenderIf* render) {
		if (debugRender){
			fwScene->DrawPHScene(render);
		}else{
			fwScene->Sync();
			fwScene->GetGRScene()->Render(render);
		}

		std::ostringstream sstr;
		sstr << "NObj = " << phScene->NSolids();
		render->DrawFont(Vec2f(-21, 23), sstr.str());
	}

	//�A�N�V��������
	virtual void OnAction(int menu, int id){
		if(menu == MENU_MAIN){
			Vec3d v, w(0.0, 0.0, 0.2), p(0.5, 20.0, 0.0);
			Quaterniond q = Quaterniond::Rot(Rad(30.0), 'y');

			if(id == ID_BOX){
				Drop(SHAPE_BOX, GRRenderIf::RED, v, w, p, q);
				message = "box dropped.";
			}else if(id==ID_SWITCHRENDER){
				debugRender = !debugRender; 
			}
		}
		SampleApp::OnAction(menu, id);
	}
	//�X�e�b�v���s
	virtual void OnStep() {
		SampleApp::OnStep();
		PHSceneIf* phscene = GetSdk()->GetScene()->GetPHScene();
		//if(phscene)		cout << phscene << endl;
		SceneObjectIf* phSceneObject;
		PHFemMeshIf* phFemMesh;
		size_t Nobject = GetSdk()->GetPHSdk()->GetObjectIf()->NChildObject();
		//�Ȃ񂩂���A�h���X�����Ă���͕̂�������
		//for(int i=0; i< Nobject ;i++){
		//	ObjectIf* object = GetSdk()->GetPHSdk()->GetChildObject(i);
		//	cout << "Object" << i << " : " << object << endl;
		//}
		//NamedObjectIf* nobject = GetSdk()->FindObject("negi");
		//phFemMesh������āAphFemMesh�̏����������s�����Ă݂����B
		//
		//�M�V�~�����[�V�����X�e�b�v
		HeatConductionStep();
	}
	void HeatConductionStep(){
		//PHFemMeshThermo�I�u�W�F�N�g�����Amain�֐��Ŏg����悤�ɂ���֐�
			CreatePHFemMeshThermo();
		//���ߓ_���A���v�f���A�ߓ_���W�A�v�f�̍\���Ȃǂ�o�^
			//PHFemMeshThermo�̃I�u�W�F�N�g�����ۂɁA�f�B�X�N���v�^�ɒl��ݒ肵�č��
			
		//�ߓ_�̏������x��ݒ肷���{T}�c�x�N�g���ɑ��
			//{T}�c�x�N�g�����쐬����B�ȍ~��K,C,F�s��E�x�N�g���̐ߓ_�ԍ��́A���̏c�x�N�g���̐ߓ_�̕��я��ɍ��킹��B
			CreateMeshTempVec();
		//�M�`�����A�M�`�B���A���x�A��M�Ȃǂ̃p�����[�^�[��ݒ�E���
			//PHFemMEshThermo�̃����o�ϐ��̒l����
			SetThermoConductionParam();
			//�����A�ϐ��l�͌ォ��v�Z�̓r���ŕύX�ł���悤��SetParam()�֐�������Ă������ق����������ȁH

		//�v�Z�ɗp����}�g���N�X�A�x�N�g�����쐬�i���b�V�����Ƃ̗v�f�����s��/�x�N�g���ˑS�̍����s��/�x�N�g���j
			//{T}�c�x�N�g���̐ߓ_�̕��я��ɕ��Ԃ悤�ɁA�W���s������Z����B�W���s��ɂ́A�ʐς�̐ρA�M�`�B���Ȃǂ̃p�����[�^�̐ς����Ă��܂������̂�����B
		//CreateK1();
		//CreateK2();
		//CreateC();
		//CreateF();
		//PrepareGaussSeidel();
			//�K�E�X�U�C�f���Ōv�Z���邽�߂ɁA�N�����N�j�R���\���̍������̌`�ōs������B�s��D��F�A-b�Ȃǂ����A�K�E�X�U�C�f���Ōv�Z�X�e�b�v�����s���O�܂�
		//�K�E�X�U�C�f���̌v�Z��P�ʃX�e�b�v���Ԃ��Ƃɍs��
			//�K�E�X�U�C�f���̌v�Z
			//CalcGaussSeidel();
		//�i�`�󂪕ς������A�}�g���N�X��x�N�g������蒼���j
		//���x�ω��E�ŐV�̎��Ԃ�{T}�c�x�N�g���ɋL�ڂ���Ă���ߓ_���x����ɉ��w�ω��V�~�����[�V�������s��
			//SetChemicalSimulation();
			//���w�ω��V�~�����[�V�����ɕK�v�ȉ��x�Ȃǂ̃p�����[�^��n��
		//���x�ω��≻�w�V�~�����[�V�����̌��ʂ̓O���t�B�N�X�\�����s��
			
		//�ȉ��A�폜�\��
		CreatePHFemMeshThermo();
		//[K]�s������
		CreateKele();
		//[C]�s������
		//{F}�x�N�g�������
		//�ʐς����߂�
		//�̐ς����߂�
		
		//�s��v�Z�̃e�X�g���s���֐�
		TestMatrixFunc();
	}
	void CreateMeshTempVec(){
		//TetGen()�ō쐬�����f�[�^����A�ߓ_�̕��я����擾���āA���x���������c�x�N�g�������
		//�x�N�g���̐���������
		//int n = 
		
		//TVector<n,double> vec; 
		//�s��̊e
	}
	void SetThermoConductionParam(){

	}
	void CreatePHFemMeshThermo(){
		PHFemMeshThermoIf* phmesht;
		PHFemMeshThermoDesc desc;
		//FWFemMesh::CreatePHFromGR()��Tetgen�ɑ������ PHFemMeshThermoDesc pmd; ������Ă���B
		////�l�ʑ̂̐������������
		////int Ntets = //Tetgen�œ����H�v�f�̐�
		//for(int i=0 ; i < Ntets ; i++){
		//	desc.tets[i] = ;
		//}
		////�ߓ_�̐������������
		//int NVertices = //Tetgen�œ����H�ߓ_�̐�
		//for(int i=0 ; i < Nvertics ; i++){
		//	desc.vertices[i] = ;
		//}



	}
	void CreateKele(){

	}

	void TestMatrixFunc(){

		////�f�B�X�N���v�^�̎��̂�����āA�I�u�W�F�N�g����鎞�ɁA�f�B�X�N���v�^������BSPIDAR�̃f�B�X�N���v�^�g�����Ɠ��������B
		//PHFemMeshThermoDesc desc;
		//desc.tets = ;
		//desc.vertice = ;
		////�V�[��������Ă��� phscene sceneobject �ɑ����Ă���I�u�W�F�N�g �V�[���I�u�W�F�N�g�́A��鎞�ɁA�ǂ̃V�[���ɑ����Ă���̂���`����K�v������B
		//PHFemMeshThermo phftmesh(desc,s);//����������

		PTM::VMatrixCol<double> Tvec;
		TMatrixRow<4,4,double> mat;              // 2�s2��̍s���l�����s���錾
												   // TMatrixCol���Ɨ񂪋l�����s��ɂȂ�
		//�l�ʑ�vector�Ȃǂ���ċA���߂Ȃǂœ���
		mat[0][0] = 1;  mat[0][1] = 1;	mat[0][2] = 1;	mat[0][3] = 1;          // 0�s0�� = 1;  0�s1�� = 2;
		mat[1][0] = 3;  mat[1][1] = 4;	mat[1][2] = 2;	mat[1][3] = 2;          // 1�s0�� = 3;  1�s1�� = 4;
		mat[2][0] = 2;	mat[2][1] = 2;	mat[2][2] = 2;	mat[2][3] = 2;
		mat[3][0] = 2;	mat[3][1] = 2;	mat[3][2] = 2;	mat[3][3] = 2;

		TVector<4,float> vec;                   // 4�����̃x�N�g����錾
		vec[0] = 1; vec[1] = 0; vec[2] = 1; vec[3] = 0; 
		std::cout << mat;
		std::cout << vec << std::endl;
		std::cout << mat * vec << std::endl;    // �|���Z
		std::cout << mat + mat << std::endl;    // �����Z
		std::cout << mat - mat << std::endl;    // �����Z
		std::cout << mat.trans() << std::endl;  // �]�u
		std::cout << mat.inv() << std::endl;    // �t�s��
		//return 0;

		//double elemK2[4][4];
		//double elemK3[4][4];


	}



};


////////////////////////////////////////////////////////////////////////////////////////////////////////////

//FWMyApptest app;
MyApp app;

/**
 brief		���C���֐�
 param		<in/--> argc�@�@�R�}���h���C�����͂̌�
 param		<in/--> argv�@�@�R�}���h���C������
 return		0 (����I��)
 */
int main(int argc, char *argv[]) {
	app.Init(argc, argv);
	app.StartMainLoop();
	return 0;
}

