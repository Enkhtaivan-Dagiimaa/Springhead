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
		ID_TEMPUP,
		ID_TEMPDN
	};
	bool debugRender;

public:
	MyApp():debugRender(false){
		appName = "FEMThermo";

		AddAction(MENU_MAIN, ID_TEMPUP, "Tc UP 20degree");
		AddHotKey(MENU_MAIN, ID_TEMPUP, 'o');
		AddAction(MENU_MAIN, ID_TEMPDN, "Tc DOWN 20degree");
		AddHotKey(MENU_MAIN, ID_TEMPDN, 'O');
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
		GetSdk()->LoadScene("sceneTHtest.spr", import);			// �t�@�C���̃��[�h			// scene.spr:negi�����[�h, scene2.spr:�f�o�b�O�p�̒�����, scene3.spr:������cheese�����[�h, sceneTHtest.spr:�t���C�p���ȂǃC���|�[�g�̃e�X�g
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
			}else if(id=ID_TEMPUP){
				FWObjectIf** fwobject = fwScene->GetObjects();	
				//fwScene
			}else if(id=ID_TEMPDN){

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

		//	�t���C�p��������Ă���
		FWObjectIf* pan		=	DCAST(FWObjectIf, GetSdk()->GetScene()->FindObject("fwPan"));
		//	�H�ނ�����Ă���
		FWFemMeshIf* tmesh	= GetSdk()->GetScene()->FindObject("fwNegi")->Cast();
		//	���[���h���W�ɕϊ�����

		//DSTR <<"pan: " << pan << std::endl;
		//DSTR <<"tmesh: " << tmesh << std::endl;

		//curScene = id;
		//// id�Ԗڂ̃V�[����I��
		//fwScene = GetSdk()->GetScene(id);
		//phScene = fwScene->GetPHScene();
		//GetCurrentWin()->SetScene(fwScene);
		//editor.SetObject(phScene);

		//tmesh->GetScene();
		
		if(pan && tmesh){
			Affinef afPan = pan->GetGRFrame()->GetWorldTransform();
			Affinef afMesh = tmesh->GetGRFrame()->GetWorldTransform();
			Affinef afMeshToPan = afPan.inv() * afMesh;
			//DSTR << afPan <<std::endl;
			//DSTR << afMesh <<std::endl;
			//DSTR << afMeshToPan <<std::endl;

	
			//size_t nchObj = tmesh->NChildObject();
			PHFemMeshThermoIf* pfem = NULL;
			for(int i=0; i<tmesh->NChildObject() && !pfem; ++i){
				pfem = tmesh->GetChildObject(i)->Cast();
				int kattoon =0;
				if(pfem){
					DSTR <<"i : " << i << "th pfem->GetName(): " << pfem->GetName() << std::endl;
					//pfem->GetPHMesh();
					int katton1 =1;
				}
			}
			DSTR << "tmesh->GetPHMesh(): " << tmesh->GetPHMesh()<< std::endl;;
			PHFemMeshIf* phm = tmesh->GetPHMesh();
			int nOfSurfaceVtx = phm->NSurfaceVertices();
			for(unsigned i =0; i <nOfSurfaceVtx;i++){
				DSTR << i <<  "th phm->GetSurfaceVertex(i): " << phm->GetSurfaceVertex(i) << std::endl;
			} 
			int kattton =0;


			//////	�ŊO�k�̐ߓ_�̃t���C�p������̍��W
			//for(unsigned i=-0; i < surfaceVertices.size();i++){
			//	if(vertices[surfaceVertices[i]].pos){};
			//	//vertices[surfaceVertices[i]].Tc;
			//}

		//	for
		//	Vec3f posOnPan = afMeshToPan * tmesh.tvtxs[j].GetPos();
		//	int katoon =1;
		}

		//	�t���C�p���ɂƂ��Ă̌��_����̋����ɉ����āA���M����

		//char grName = "";
		//char phName = "";

		//GRFrameIf* findFrame = DCAST(GRFrameIf, GetSdk()->GetScene()->GetGRScene()->FindObject(grName));
		//PHSolidIf* findSolid = DCAST(PHSolidIf, GetSdk()->GetScene()->GetPHScene()->FindObject(phName));

		//if(pan){
		//	Affinef afPan = pan->GetGRFrame()->GetWorldTransform();
		//	Affinef afMesh = tmesh.obj->GetGRFrame()->GetWorldTransform();
		//	Affinef afMeshToPan = afPan.inv() * afMesh;	
		//}
		//PHFemMeshThermo�I�u�W�F�N�g�����Amain�֐��Ŏg����悤�ɂ���֐�
			//CreatePHFemMeshThermo();
		//���ߓ_���A���v�f���A�ߓ_���W�A�v�f�̍\���Ȃǂ�o�^
			//PHFemMeshThermo�̃I�u�W�F�N�g�����ۂɁA�f�B�X�N���v�^�ɒl��ݒ肵�č��
			
		//�ߓ_�̏������x��ݒ肷���{T}�c�x�N�g���ɑ��
			//{T}�c�x�N�g�����쐬����B�ȍ~��K,C,F�s��E�x�N�g���̐ߓ_�ԍ��́A���̏c�x�N�g���̐ߓ_�̕��я��ɍ��킹��B
			//CreateMeshTempVec();
		//�M�`�����A�M�`�B���A���x�A��M�Ȃǂ̃p�����[�^�[��ݒ�E���
			//PHFemMEshThermo�̃����o�ϐ��̒l����
			//SetThermoConductionParam();
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
		//CreatePHFemMeshThermo();
		//[K]�s������
		//CreateKele();
		//[C]�s������
		//{F}�x�N�g�������
		//�ʐς����߂�
		//�̐ς����߂�
		
		//�s��v�Z�̃e�X�g���s���֐�
		//TestMatrixFunc();
	}
	void CreateMeshTempVec(){

	}
	void SetThermoConductionParam(){

	}
	void CreatePHFemMeshThermo(){
		PHFemMeshThermoIf* phmesht;
		PHFemMeshThermoDesc desc;
	}
	void CreateKele(){

	}

	void CopyTheFood(){

	}

//	bool CopyTheFood(const char* phName, const char* grName){
//	if(GetSdk()->GetScene()->GetPHScene() && GetSdk()->GetScene()->GetGRScene()){
//		std::cout << phName << " : " << GetSdk()->GetScene()->GetPHScene()->FindObject(phName) << std::endl;
//		if(GetSdk()->GetScene()->GetPHScene()->FindObject(phName)){
//			PHSolidIf* findSolid = DCAST(PHSolidIf, GetSdk()->GetScene()->GetPHScene()->FindObject(phName));
//			PHSolidDesc sDesc;
//			if (findSolid) {
//				findSolid->GetDesc(&sDesc);
//				std::cout << "dupl : solid found : " << findSolid->GetName() << std::endl;
//			} else {
//				std::cout << "dupl : solid NOT found : " << phName << std::endl;
//			}
//
//			GRFrameIf* findFrame = DCAST(GRFrameIf, GetSdk()->GetScene()->GetGRScene()->FindObject(grName));
//			GRFrameDesc fDesc;
//			
//			cout << "GRFrame Name : " << findFrame->GetName() 
//				 << " GRMesh Name  : " << DCAST(NamedObjectIf, findFrame->GetChildObject(0))->GetName() << std::endl;
//			GRMesh* origMesh = findFrame->GetChildObject(0)->Cast();
//			GRMeshDesc& md = *(GRMeshDesc*)origMesh->GetDescAddress();		//GRMeshDesc�^�̒l(�|�C���^�̎w���l)md�ɁA��ŃA�h���X���i�[�����|�C���^ mesh��GetDescAddress�Ƃ����֐��̕Ԃ�l���i�[����B
//			GRMesh* cm = GetSdk()->GetScene()->GetGRScene()->CreateObject(md)->Cast();	//cm:create mesh? md�Ɋi�[���ꂽ�|�C���^�̂����I�u�W�F�N�g��create(����)���āAGRMesh*�^�ɃL���X�g����B
//			cm->tex3d = true;												//3�����e�N�X�`�����g�p�ɂ���B
//			cm->material = origMesh->material;									//cm��material�Ɂ@mesh��materialList�̒l���i�[����B
//			cm->materialList = origMesh->materialList;							//cm��materialList��mesh��materialList���i�[����B
//			cm->faceNormals  = origMesh->faceNormals;							//cm��mesh�̖ʂ̖@���x�N�g���ɁAmesh�̂�����i�[����B
//			cm->originalFaceIds = origMesh->originalFaceIds;					//mesh��originalFaceIds(�R�������b�V���̌��̒��_�ԍ�)��cm�̂���Ɋi�[����B
//			cm->originalFaces  = origMesh->originalFaces;						//mesh��originalFaces��cm�̂���Ɋi�[����B
//
//			// FWObjectIf* copied = GetSdk()->GetScene()->CreateObject(sDesc, fDesc);
//			FWObjectIf* copied = GetSdk()->GetScene()->CreateFWObject();
//			copied->SetPHSolid(GetSdk()->GetScene()->GetPHScene()->CreateSolid(sDesc));
//			copied->SetGRFrame(GetSdk()->GetScene()->GetGRScene()->CreateVisual(fDesc.GetIfInfo(), fDesc)->Cast());
//
//			for (unsigned int i=0; i<findSolid->NChildObject(); ++i) {
//				copied->GetPHSolid()->AddChildObject(findSolid->GetChildObject(i));
//			}
//			copied->GetGRFrame()->AddChildObject(cm->Cast());
//
//			std::stringstream sout;
//			sout << "soCopiedFood" << copiedNum++;
//			copied->GetPHSolid()->SetName(sout.str().c_str());
//			Posed p = pan->GetPHSolid()->GetPose();					//�t���C�p���̏�ɓ����
//			//�H�ނ͏�Ƀt���C�p���ɓ����
//			copied->GetPHSolid()->SetFramePosition(Vec3d((double)rand() / (double)RAND_MAX * 0.03 * cos((double)rand()) + p.px,
//														(double)rand() / (double)RAND_MAX * 0.03 + 0.07 + p.py,
//														(double)rand() / (double)RAND_MAX * 0.03 * sin((double)rand()) +p.pz)
//														);
//			//copied->GetPHSolid()->SetAngularVelocity(Vec3d( (double)rand() / (double)RAND_MAX * 2.9,  (double)rand() / (double)RAND_MAX * 0.7,	(double)rand() / (double)RAND_MAX * 1.7 ));
//			copied->GetPHSolid()->SetAngularVelocity(Vec3d(0.0,0.0,0.0));
//			copied->GetPHSolid()->SetOrientation(Quaternionf::Rot(Rad((double)rand() / (double)RAND_MAX) * cos((double)rand()) + Rad(15) , 'x') 
//															  * Quaternionf::Rot(Rad((double)rand()), 'y')
//															  * Quaternionf::Rot(Rad((double)rand() / (double)RAND_MAX) * sin((double)rand()), 'z'));
//			copied->GetPHSolid()->SetVelocity(Vec3d(0.0,0.0,0.0));
//			sout.str("");
//			sout << "frCopiedFood" << copiedNum;
//			copied->GetGRFrame()->SetName(sout.str().c_str());
//			
//			foodObjs.push_back(copied);
//			GRMesh* mesh = foodObjs.back()->GetGRFrame()->GetChildObject(0)->Cast();
//			mesh->Render(GetCurrentWin()->render);
//
//			if(mesh && mesh->GetNVertex()){
//			
//				GRThermoMesh tmesh;
//				tmesh.Init(mesh, foodObjs.back());
//
//				std::stringstream foodName;
//				foodName << phName;
//				//copythings = 1;					//�v����
//				if (foodName.str().find("negi")!= string::npos || foodName.str().find("Negi")!= string::npos){
//					tmesh.copyInit =true;
//					tmesh.fwobj = copied;			//foodObjs�̍Ō�̗v�f��FWObjectIf�Ȃ�A������i�[
//					tmeshnegis.push_back(tmesh);
//					MyApp::InitNegi();
//				}
//				else if (foodName.str().find("carrot")!= string::npos || foodName.str().find("Carrot")!= string::npos){
//					tmesh.copyInit =true;
//					tmesh.fwobj = copied;			//foodObjs�̍Ō�̗v�f��FWObjectIf�Ȃ�A������i�[
//					tmeshcarrots.push_back(tmesh);
//					MyApp::InitCarrot();
//				}
//				else if (foodName.str().find("shrimp")!= string::npos || foodName.str().find("Shrimp")!= string::npos){
//					tmesh.copyInit =true;
//					tmesh.fwobj = copied;			//foodObjs�̍Ō�̗v�f��FWObjectIf�Ȃ�A������i�[
//					tmeshshrimps.push_back(tmesh);
//					MyApp::InitShrimp();
//				}
//				else if (foodName.str().find("asteakc")!= string::npos || foodName.str().find("Asteakc")!= string::npos){
//					tmesh.copyInit =true;
//					tmesh.fwobj = copied;			//foodObjs�̍Ō�̗v�f��FWObjectIf�Ȃ�A������i�[
//					tmesh.swtovtx = true;			//�f�ʂƖ{�̂̓����ʒu���W�������_�̃y�A����鏈���̃t���O
//					tmeshasteaks_c.push_back(tmesh);
//					MyApp::InitAsteak_c();
//				}
//				else if (foodName.str().find("asteak")!= string::npos || foodName.str().find("Asteak")!= string::npos){
//					tmesh.copyInit =true;
//					tmesh.fwobj = copied;			//foodObjs�̍Ō�̗v�f��FWObjectIf�Ȃ�A������i�[
//					tmesh.swtovtx = true;
//					tmeshasteaks.push_back(tmesh);
//					MyApp::InitAsteak();
//				}
//				else if (foodName.str().find("nsteakc")!= string::npos || foodName.str().find("Nsteakc")!= string::npos){
//					tmesh.copyInit =true;
//					tmesh.fwobj = copied;			//foodObjs�̍Ō�̗v�f��FWObjectIf�Ȃ�A������i�[
//					tmesh.swtovtx = true;
//					tmeshnsteaks_c.push_back(tmesh);
//					MyApp::InitNsteak_c();
//				}
//				else if (foodName.str().find("nsteak")!= string::npos || foodName.str().find("Nsteak")!= string::npos){
//					tmesh.copyInit =true;
//					tmesh.fwobj = copied;			//foodObjs�̍Ō�̗v�f��FWObjectIf�Ȃ�A������i�[
//					tmesh.swtovtx = true;
//					tmeshnsteaks.push_back(tmesh);
//					MyApp::InitNsteak();
//				}
//				
//				tmeshes.push_back(tmesh);
//				DSTR		<< "Copy number of the food : " << copiedNum << std::endl;
//				std::cout	<< "Copy number of the food : " << copiedNum << std::endl;
//			}
//		}
//	}
//	//�R�s�[�����s�����Ƃ��̃G���[�\��
//	//DSTR		<< "Failed to copy." << std::endl;
//	//std::cout	<< "Failed to copy." << std::endl;
//	return false;
//}

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

