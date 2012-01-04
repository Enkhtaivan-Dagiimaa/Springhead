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
#include <Framework/FWFemMesh.h>
#include <Collision/CDConvexMesh.h>
#include <Physics/PHConstraintEngine.h>
#include <Physics/PHFemMeshThermo.h>
#include <Graphics/GRMesh.h>
#include <list>

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
		GetSdk()->LoadScene("sceneTHTest5.spr", import);			// �t�@�C���̃��[�h			// scene.spr:negi�����[�h, scene2.spr:�f�o�b�O�p�̒�����, scene3.spr:������cheese�����[�h, sceneTHtest.spr:�t���C�p���ȂǃC���|�[�g�̃e�X�g
		numScenes = GetSdk()->NScene();
		if (numScenes) SwitchScene(GetSdk()->NScene()-1);

		//	FEMMesh��ۑ����Ă݂�
		FWFemMesh* fm[2] = {GetSdk()->GetScene()->FindObject("fwNegi")->Cast(), GetSdk()->GetScene()->FindObject("fwPan")->Cast()};
		//FWFemMeshIf* fm[1] = GetSdk()->GetScene()->FindObject("fwNegi")->Cast();
		//FWFemMeshIf* fm[1] = GetSdk()->GetScene()->FindObject("fwPan")->Cast();
		
		PHFemMeshThermoDesc d;		//	���ꕨ�����
		fm[1]->GetPHMesh()->GetDesc(&d);			//	�A�h���X��n��
		for(unsigned i=0; i < d.vertices.size(); i++){
			d.vertices[i].y = 0.5 *d.vertices[i].y;
		}
		fm[1]->GetPHMesh()->SetDesc(&d);
		
		//	PHMesh����GRMesh�����B
		fm[1]->grFrame->DelChildObject(fm[1]->grMesh->Cast());
		fm[1]->CreateGRFromPH();
		fm[1]->grFrame->AddChildObject(fm[1]->grMesh->Cast());


		//DSTR << "fm[0]: " << fm[0] <<std::endl;
		//DSTR << "fm[1]: " << fm[1] <<std::endl;
		int hasek =0;

		ObjectIfs objs;
		FIFileSprIf* spr = GetSdk()->GetFISdk()->CreateFileSpr();
		for(unsigned i= 0; i < 2; i++){
			if (fm[i] && fm[i]->GetPHMesh()){
				objs.Push(fm[i]->GetPHMesh());
			}
		}
		spr->Save(objs, "femmesh.spr");

		//> ���̃R�[�h
		//FWFemMeshIf* fm[1] = GetSdk()->GetScene()->FindObject("fwNegi")->Cast();
		//FWFemMeshIf* fm = GetSdk()->GetScene()->FindObject("fwNegi")->Cast();
		//if (fm && fm->GetPHMesh()){
		//	FIFileSprIf* spr = GetSdk()->GetFISdk()->CreateFileSpr();
		//	ObjectIfs objs;
		//	objs.Push(fm->GetPHMesh());
		//	spr->Save(objs, "femmeshPan.spr");
		//}

//		fm[1]->GetPHSolid()->GetPose()

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

		//> ���_���W�̊i�[	
		//FWObjectIf* pan_		=	DCAST(FWObjectIf, GetSdk()->GetScene()->FindObject("Pan"));
		//Affinef afPan_ = pan_->GetGRFrame()->GetWorldTransform();
		//Affinef afPan_ = pan_->GetGRFrame()->GetWorldTransform();
		///	���_�����������[�J�����W�n��(0,0,0)�̂͂�
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
	inline static double dist2D2(const Vec3d& a, const Vec3d& b){
		return Square(a.x-b.x) + Square(a.y-b.y);
	}
	struct CondVtx;
	struct CondVtxs:public std::vector<CondVtx>{
		//PHFemMesh* pmesh;
		//
		PHFemMeshThermo* pmesh;
		std::vector<int> vtx2Cond;
	};
	struct CondVtx{
		int vid;
		Vec3d pos;	//	x,y�͐ڐG�ʏ�ł̈ʒu�Az�͐ڐG�ʂ���̋���
		double area;
		double assign;
		CondVtx():vid(-1), area(0), assign(0){}
		struct Companion{
			int id;
			double area;
			Companion(int i, double a):id(i), area(a){}
		};
		std::vector<Companion> companions;
		struct Less{
			int axis;
			Less(int a):axis(a){}
			bool operator() (const CondVtx& a, const CondVtx& b) const { return a.pos[axis] < b.pos[axis]; }
		};
	};
	void AddCompanion(CondVtxs& v1, int id1, CondVtxs& v2, int id2, double a){
		assert(a > 0);
		v1[id1].assign += a;
		v2[id2].assign += a;
		assert(v1[id1].area - v1[id1].assign > -1e8 );
		assert(v2[id2].area - v2[id2].assign > -1e8 );
		unsigned i;
		for(i=0; i<v1[id1].companions.size(); ++i){
			if (v1[id1].companions[i].id == id2){
				v1[id1].companions[i].area += a;
				break;
			}
		}
		if (i == v1[id1].companions.size()){
			v1[id1].companions.push_back(CondVtx::Companion(id2, a));
		}
		for(i=0; i<v2[id2].companions.size(); ++i){
			if (v2[id2].companions[i].id == id1){
				v2[id2].companions[i].area += a;
				break;
			}
		}
		if (i == v2[id2].companions.size()){
			v2[id2].companions.push_back(CondVtx::Companion(id1, a));
		}
	}

	//	cur�ׂ̗ŁAused�Ɋ܂܂�Ȃ����_���
	void FindNext(std::vector<int>& next, const std::vector<int>& cur, const std::vector<int>& used, CondVtxs& condVtxs){
		for(unsigned i=0; i<cur.size(); ++i){
			for(unsigned e=0; e < condVtxs.pmesh->vertices[condVtxs[cur[i]].vid].edges.size(); ++e){
				int vid = condVtxs[cur[i]].vid;
				PHFemMesh::Edge& edge = condVtxs.pmesh->edges[condVtxs.pmesh->vertices[vid].edges[e]];
				int f = edge.vertices[0] == vid ? edge.vertices[1] : edge.vertices[0]; 
				int cf = condVtxs.vtx2Cond[f];
				if (cf>=0){
					if (std::find(used.begin(), used.end(), cf) == used.end()
						&& std::find(cur.begin(), cur.end(), cf) == cur.end()
						&& std::find(next.begin(), next.end(), cf) == next.end())
						next.push_back(cf);
				}
			}
		}
	}
	//	condVtxs�̂Ȃ�����Afroms�ׂ̗̊��蓖�Ăɋ󂫂����钸�_��񋓂��Apos�ɋ߂����Ƀ\�[�g���ĕԂ��B
	struct Dist2Less{
		Vec3d pos;
		CondVtxs& condVtxs;
		Dist2Less(Vec3d p, CondVtxs& c):pos(p),condVtxs(c){}
		bool operator () (int a, int b)const{
			return dist2D2(condVtxs[a].pos, pos) < dist2D2(condVtxs[b].pos, pos); 
		}
	};
	void FindNearests(std::vector<int>& nears, const Vec3d& pos, CondVtxs& condVtxs, const std::vector<int>& froms){
		FindNext(nears, froms, std::vector<int>(), condVtxs);
		std::sort(nears.begin(), nears.end(), Dist2Less(pos, condVtxs));
	}
	static void CalcWeight3(double* weights, Vec3d pos, Vec3d p0, Vec3d p1, Vec3d p2){
		Vec3d a = p1-p0;
		Vec3d b = p2-p0;
		Vec2d v(pos.x-p0.x, pos.y-p0.y);
		Matrix2d mat(a.x, b.x, a.y, b.y);
		Vec2d w = mat.inv() * v;
		weights[0] = 1- w[0] - w[1];
		weights[1] = w[0];
		weights[2] = w[1];
	}
	double FindNearest(const Vec3d& pos, CondVtxs& condVtxs, int& found){
		if (condVtxs.size() == 0){
			found = -1;
			return DBL_MAX;
		}
		int minId = found;
		double minDist2;
		int cid;
		do{
			cid = minId;
			minDist2 = dist2D2(condVtxs[cid].pos, pos);
			int vid = condVtxs[cid].vid;
			for(unsigned e=0; e < condVtxs.pmesh->vertices[vid].edges.size(); ++e){
				PHFemMesh::Edge& edge = condVtxs.pmesh->edges[condVtxs.pmesh->vertices[vid].edges[e]];
				int next = edge.vertices[0] == vid ? edge.vertices[1] : edge.vertices[0]; 
				int cnext = condVtxs.vtx2Cond[next];
				if (cnext>=0){
					double d2 = dist2D2(condVtxs[cnext].pos, pos);
					if (d2 < minDist2){
						minDist2 = d2;
						minId = cnext;
					}
				}
			}
		}while(cid != minId);
		found = cid;
		return sqrt(minDist2);
	}
	//	condVtxs �̒�����Apos �ɋ߂�3�_�� from ���N�_�ɒT������ ids�ɕԂ��Bpos���Ԃ���d�݂�wegihts�ɕԂ��B
	void FindNearest3(int *ids, double* weights, const Vec3d& pos, CondVtxs& condVtxs, int from){
		int minId = from;
		double minDist2;
		int cid;
		do{
			cid = minId;
			minDist2 = dist2D2(condVtxs[cid].pos, pos);
			int vid = condVtxs[cid].vid;
			for(unsigned e=0; e < condVtxs.pmesh->vertices[vid].edges.size(); ++e){
				PHFemMesh::Edge& edge = condVtxs.pmesh->edges[condVtxs.pmesh->vertices[vid].edges[e]];
				int next = edge.vertices[0] == vid ? edge.vertices[1] : edge.vertices[0]; 
				int cnext = condVtxs.vtx2Cond[next];
				if (cnext>=0){
					double d2 = dist2D2(condVtxs[cnext].pos, pos);
					if (d2 < minDist2){
						minDist2 = d2;
						minId = cnext;
					}
				}
			}
		}while(cid != minId);
		//	�ׂ̒��_�ŋ߂����̂��Q������
		double min1Dist2 = DBL_MAX;
		double min2Dist2 = DBL_MAX;
		int min1Id = -1;
		int min2Id = -1;
		int vid = condVtxs[cid].vid;
		for(unsigned e=0; e < condVtxs.pmesh->vertices[vid].edges.size(); ++e){
			PHFemMesh::Edge& edge = condVtxs.pmesh->edges[condVtxs.pmesh->vertices[vid].edges[e]];
			int next = edge.vertices[0] == vid ? edge.vertices[1] : edge.vertices[0];
			int cnext = condVtxs.vtx2Cond[next];
			if (cnext>=0){
				double d2 = dist2D2(condVtxs[cnext].pos, pos);
				if (d2 < min1Dist2){
					min2Dist2 = min1Dist2;
					min1Dist2 = d2;
					min2Id = min1Id;
					min1Id = cnext;
				}
			}
		}
		ids[0] = cid;
		ids[1] = min1Id;
		ids[2] = min2Id;
		if (min1Id == -1){
			weights[0] = 1; weights[1] = weights[2] = 0;
		}else if (min2Id == -1){
			double mind1 = sqrt(min1Dist2);
			double mind = sqrt(minDist2);
			weights[0] = mind1 / (mind+ mind1);
			weights[1] = mind / (mind+ mind1);
			weights[2] = 0;
		}else{
			double mind2 = sqrt(min2Dist2);
			double mind1 = sqrt(min1Dist2);
			double mind = sqrt(minDist2);
			double sum2 = (mind + mind1 + mind2)*2;
			weights[0] = (mind1 + mind2) / sum2;
			weights[1] = (mind + mind2) / sum2;
			weights[2] = (mind + mind1) / sum2;
		}
	}
	void HeatConductionStep(){
#if 1
		//	2���̂����o��
		FWFemMesh* fmesh[2];
		fmesh[0] = GetSdk()->GetScene()->FindObject("fwNegi")->Cast();
		fmesh[1] = GetSdk()->GetScene()->FindObject("fwPan")->Cast();
		if (!(fmesh[0] && fmesh[1])){
			DSTR << "�Ώۂ�FWFemMesh��������܂���" << std::endl;
			return;
		}
		PHSolid* solids[2];
		solids[0] = fmesh[0]->GetPHSolid()->Cast();
		solids[1] = fmesh[1]->GetPHSolid()->Cast();
		//	FEMMesh�͐ڐG���肪�Ȃ�CDConvexMesh�ɂ���BCDConvexMesh�̈ʒu������Ă���
		if (solids[0]->NShape() != 1 || solids[0]->NShape() !=1){
			DSTR << "�����`��ɂ͖��Ή�" << std::endl;
			return;
		}
		PHScene* scene = solids[0]->GetScene()->Cast();
		//	�ڐG�y�A�������āA������Ȃ画�肷��B
		bool bSwap;
		PHSolidPairForLCP* pair = scene->GetSolidPair(solids[0]->Cast(), solids[1]->Cast(), bSwap)->Cast();
		PHShapePairForLCP* sp = pair->GetShapePair(0,0)->Cast();
		if (sp->state == CDShapePair::NONE){
			//	���ڐG�Ȃ̂ŁAGJK���Ă�
			sp->shapePoseW[0] = solids[0]->GetPose() * solids[0]->GetShapePose(0);
			sp->shapePoseW[1] = solids[1]->GetPose() * solids[1]->GetShapePose(0);
			if (bSwap) std::swap(sp->shapePoseW[0], sp->shapePoseW[1]);	
			Vec3d sep;
			double dist = FindClosestPoints(sp->shape[0], sp->shape[1], sp->shapePoseW[0], sp->shapePoseW[1], sep, sp->closestPoint[0], sp->closestPoint[1]);
			if (dist < 1e-10){
				//	���Ȃ�߂��̂ŁA�@�����������̂ŁA�x���������Ă����B
				DSTR << "�Q���̂����ɋ߂����A�ڐG���͂��Ă��Ȃ������ȏ��"  << std::endl;
			}
			sp->depth = -dist;
			sp->normal = sp->shapePoseW[1]*sp->closestPoint[1] - sp->shapePoseW[0]*sp->closestPoint[0];
			sp->normal.unitize();
		}

		//	����:dist

		//	�������߂���Γ`�M�̏���
		/*	�M�`�B�� �� [W/(m^2 K)] ��p����ƁA���E��� q = ��(T-Tc) (T:�ړ_���x Tc:���̗͂��̓��̉��x)
			�Q���̂̐ڐG���ƁAT1-��1->Tc-��2->T2 �ƂȂ�ƍl������B
			q = ��1(T1-Tc) = ��2(Tc-T2) ��� (��1+��2)Tc = ��1T1 + ��2T2
			Tc = (��1T1 + ��2T2)/(��1+��2)
			q = ��1(T1-(��1T1 + ��2T2)/(��1+��2)) = ��1T1 - ��1(��1T1 + ��2T2)/(��1+��2)
				= (��1��2T1 - ��1��2T2)/(��1+��2) = ��'(T1-T2)  ��' = ��1��2/(��1+��2)	*/
		/*	q��Q�ɂ��Ă̍l�@
			q�͒P�ʖʐς�����Ȃ̂ŁA���_�Ԃ̔M�̈ړ���Q�ɒ����ɂ́A���_����\����ʐς��|����K�v������B
			�{���́A�O�p�`�̏d�Ȃ�ƌ`��֐����狁�߂�ׂ����́B
			�������A�d�Ȃ��͖��X�e�b�v�ς��̂ŁA���̌v�Z�͑�ρB�ȗ������l����B
			���_�͒��_���܂ގO�p�`�ɐ��͂����B�O�p�`�̏d�Ȃ���A���_�̋����̈Ӗ����傫���B
			�������߂����̂����蓖�ĂĂ������A��Ŕ�ђn���o�Ă͂����Ȃ��B
			�����ŁA�P�_���珉�߂ď��X�Ɋ��蓖�ė̈��傫�����čs���B����ł���͋N���Ă���ђn�͂łȂ��B
				
			���_�̖ʐς́A���_���܂ގO�p�`�B�̖ʐς̘a��1/3�B				
		*/
		const double isoLen = 0.005;	//	����ȏ㗣���Ɠ`�����Ȃ�����
		if (sp->depth > -isoLen){
			CondVtxs condVtxs[2];
			condVtxs[0].pmesh = fmesh[bSwap? 1:0]->GetPHMesh()->Cast();
			condVtxs[1].pmesh = fmesh[bSwap? 0:1]->GetPHMesh()->Cast();
			//	�������߂����_��񋓁B���łɖ@���ɐ����ȕ��ʏ�ł̍��W�����߂Ă����B
			Matrix3d coords;
			if (std::abs(sp->normal.x) < std::abs(sp->normal.y)) coords = Matrix3d::Rot(sp->normal, Vec3d(1,0,0), 'z');
			else coords = Matrix3d::Rot(sp->normal, Vec3d(0,1,0), 'z');
			Matrix3d coords_inv = coords.inv();
			Vec3d normalL[2];
			for(int i=0; i<2; ++i){
				normalL[i] = sp->shapePoseW[i].Ori().Inv() * sp->normal * (i==0 ? 1 : -1);
				for(unsigned v=0; v < condVtxs[i].pmesh->surfaceVertices.size(); ++v){
					double vd = (sp->closestPoint[i] - condVtxs[i].pmesh->vertices[condVtxs[i].pmesh->surfaceVertices[v]].pos) * normalL[i];
					vd -= sp->depth;
					if (vd < isoLen) {
						CondVtx c;
						c.vid = condVtxs[i].pmesh->surfaceVertices[v];
						c.pos = coords_inv * (sp->shapePoseW[i] * condVtxs[i].pmesh->vertices[c.vid].pos);
						c.pos.z = vd + (sp->depth/2);	//	���ʂ���̋����ɂ��Ă����B
						condVtxs[i].push_back(c);
					}
				}
			}
			//	�@���ɐ����ȕ��ʏ�Ń\�[�g���āABBox���d�Ȃ�Ȃ������͎̂Ă�B
			//	�{���́ACDConvex::FindCutRing()���g���ėǂ������ɂ�肽���Ƃ���B�ʕ������ł������肽���B
			Vec2d bboxMin, bboxMax;
			for(int axis = 0; axis<2; ++axis){
				for(int i=0; i<2; ++i){
					std::sort(condVtxs[i].begin(), condVtxs[i].end(), CondVtx::Less(axis));
				}
				bboxMin[axis] = condVtxs[0].size() ? condVtxs[0].front().pos[axis] : DBL_MIN;
				bboxMin[axis] = std::max(bboxMin[axis], condVtxs[1].size() ? condVtxs[1].front().pos[axis] : DBL_MIN);
				bboxMin[axis] -= isoLen;
				for(int i=0; i<2; ++i){
					CondVtx tmp;
					tmp.pos[axis] = bboxMin[axis];
					CondVtxs::iterator it = std::lower_bound(condVtxs[i].begin(), condVtxs[i].end(), tmp, CondVtx::Less(axis));
					condVtxs[i].erase(condVtxs[i].begin(), it);
				}
				bboxMax[axis] = condVtxs[0].size() ? condVtxs[0].back().pos[axis] : DBL_MAX;
				bboxMax[axis] = std::min(bboxMax[axis], condVtxs[1].size() ? condVtxs[1].back().pos[axis] : DBL_MAX);
				bboxMax[axis] += isoLen;
				for(int i=0; i<2; ++i){
					CondVtx tmp;
					tmp.pos[axis] = bboxMax[axis];
					CondVtxs::iterator it = std::upper_bound(condVtxs[i].begin(), condVtxs[i].end(), tmp, CondVtx::Less(axis));
					condVtxs[i].erase(it, condVtxs[i].end());
				}
			}
			//	�Ή����钸�_�̂Ȃ����_���폜����B
			//		�܂�vertices����condVtx�ւ̑Ή��\�����
			for(int i=0; i<2; ++i){
				condVtxs[i].vtx2Cond.resize(condVtxs[i].pmesh->vertices.size(), -1);
				for(unsigned j=0; j<condVtxs[i].size(); ++j){
					condVtxs[i].vtx2Cond[condVtxs[i][j].vid] = j;
				}
			}
			//	�Ή����钸�_��������Ȃ����_���A�폜�̂��߂Ƀ}�[�N�B
			std::vector<int> eraseVtxs[2];
			for(int i=0; i<2; ++i){
				int from = 0;
				for(unsigned j=0; j<condVtxs[i].size(); ++j){
					int found = from;
					double dist = FindNearest(condVtxs[i][j].pos, condVtxs[1-i], found);
					if (dist > isoLen){	//	���ʋ�����isoLen�ȏ�̒��_�͍폜
						eraseVtxs[i].push_back(i);
					}else{
						from = found;
					}
				}
			}
			//	�}�[�N�������_���폜
			for(int i=0; i<2; ++i){
				for(int j=eraseVtxs[i].size()-1; j>0; --j){
					condVtxs[i].erase(condVtxs[i].begin() + eraseVtxs[i][j]);
				}
				if (condVtxs[i].size() == 0){
					DSTR << "Can not find companion vertex on object " << i << std::endl;
					return;
				}
			}
			//	vertices����condVtx�ւ̑Ή��\�����
			for(int i=0; i<2; ++i){
				condVtxs[i].vtx2Cond.clear();
				condVtxs[i].vtx2Cond.resize(condVtxs[i].pmesh->vertices.size(), -1);
				for(unsigned j=0; j<condVtxs[i].size(); ++j){
					condVtxs[i].vtx2Cond[condVtxs[i][j].vid] = j;
				}
			}
			//	���_�̒S������ʐς��v�Z����
			for(int i=0; i<2; ++i){
				for(unsigned j=0; j<condVtxs[i].size(); ++j){
					condVtxs[i][j].area = 0;
					int vid = condVtxs[i][j].vid;
					for(int k=0; k<condVtxs[i].pmesh->vertices[vid].faces.size(); ++k){
						if (condVtxs[i].pmesh->vertices[vid].faces[k] < condVtxs[i].pmesh->nSurfaceFace){
							PHFemMesh::Face& face = condVtxs[i].pmesh->faces[condVtxs[i].pmesh->vertices[vid].faces[k]];
							Vec3d a = condVtxs[i].pmesh->vertices[face.vertices[2]].pos - condVtxs[i].pmesh->vertices[face.vertices[0]].pos;
							Vec3d b = condVtxs[i].pmesh->vertices[face.vertices[1]].pos - condVtxs[i].pmesh->vertices[face.vertices[0]].pos;
							double ip = normalL[i] * (a^b).unit();
							if (ip > 0){
								condVtxs[i][j].area += ip * face.area / 3;
							}
						}
					}
				}
			}
			//	bbox�̒��S�߂��̒��_��������
			double xCenter = 0.5*(bboxMin.x + bboxMax.x);
			int centerVtx[2] = {-1, -1};
			for(int i=0; i<2; ++i){
				CondVtx tmp;
				tmp.pos[1] = 0.5*(bboxMin.y+bboxMax.y);
				CondVtxs::iterator it = std::lower_bound(condVtxs[i].begin(), condVtxs[i].end(), tmp, CondVtx::Less(1));
				int cit = it - condVtxs[i].begin();
				double minDistX = DBL_MAX;
				for(int y=0; y<5; ++y){
					if (cit-y >= 0 && cit-y < (int)condVtxs[i].size()){
						double dist = std::abs(condVtxs[i][cit-y].pos.x - xCenter);
						if (dist < minDistX){
							centerVtx[i] = cit-y;
							minDistX = dist;
						}
					}
					if (cit+y < (int)condVtxs[i].size()){
						double dist = std::abs(condVtxs[i][cit+y].pos.x - xCenter);
						if (dist < minDistX){
							centerVtx[i] = cit+y;
							minDistX = dist;
						}
					}
				}
				assert(centerVtx[i]>=0);
			}
			//	centerVtx[i]�ƈ�ԋ߂����_��T��
			int closestVtx[2];
			double minDist2[2];
			for(int i=0; i<2; ++i){
				Vec3d pos = condVtxs[1-i][centerVtx[1-i]].pos;
				int minId = centerVtx[i];
				int cid;
				do {
					cid = minId;
					minDist2[i] = dist2D2(condVtxs[i][cid].pos, pos);
					int vid = condVtxs[i][cid].vid;
					for(unsigned e=0; e < condVtxs[i].pmesh->vertices[vid].edges.size(); ++e){
						PHFemMesh::Edge& edge = condVtxs[i].pmesh->edges[condVtxs[i].pmesh->vertices[vid].edges[e]];
						int next = edge.vertices[0] == vid ? edge.vertices[1] : edge.vertices[0]; 
						int cnext = condVtxs[i].vtx2Cond[next];
						if (cnext>=0){
							double d2 = dist2D2(condVtxs[i][cnext].pos, pos);
							if (d2 < minDist2[i]){
								minDist2[i] = d2;
								minId = cnext;
							}
						}
					}
				} while(minId != cid);
				closestVtx[i] = cid;
			}
			//	���߂��ق��� closestVtx[2] �ɓ����B
			if (minDist2[0] < minDist2[1]) closestVtx[1] = centerVtx[1];
			else closestVtx[0] = centerVtx[0];
			//	��������Ή����L���Ă���
			std::vector<int> cur[2];
			cur[0].push_back(closestVtx[0]);
			cur[1].push_back(closestVtx[1]);
			//	���蓖�čς݂̒��_�͂�����Ɉڂ�
			std::vector<int> used[2];
			while(cur[0].size() || cur[1].size()){
				//	cur�ɂ��ĔM�����̌W���i�M�`�B���j�����߁A�Ή����钸�_��������
				for(int i=0; i<2; ++i){
					for(unsigned j=0; j<cur[i].size();++j){
						double a = condVtxs[i][cur[i][j]].area - condVtxs[i][cur[i][j]].assign;
						if (a <= 1e-11) continue;
						std::vector<int> ids(3, -1);
						double weights[3];
						FindNearest3(&ids[0], weights, condVtxs[i][cur[i][j]].pos, condVtxs[1-i], cur[1-i].size() ? cur[1-i].back() : used[1-i].back());
						double rest = a;
						while(rest > 1e-11){
							for(int k=0; k<3; ++k){
								if (ids[k] < 0) continue;
								if (condVtxs[1-i][ids[k]].area - condVtxs[1-i][ids[k]].assign > a * weights[k]){
									double delta = a * weights[k];
									if (delta > 0){
										AddCompanion(condVtxs[1-i], ids[k], condVtxs[i], cur[i][j], delta);
										rest -= delta;
									}
								}else{
									double delta = condVtxs[1-i][ids[k]].area - condVtxs[1-i][ids[k]].assign;
									if (delta > 0){
										AddCompanion(condVtxs[1-i], ids[k], condVtxs[i], cur[i][j], delta);
										rest -= delta;
									}
									//	�����ς��Ȃ̂ŁAweight��0�ɂ��Ă���
									weights[k] = 0;
								}
							}
							double wsum = weights[0] + weights[1] + weights[2];
							if (wsum == 0) goto filled;
							for(int k=0; k<3; ++k) weights[k] /= wsum;
							a = rest;
						}
filled:;
						//	�R�_�ł͋z��������Ȃ������̂ŁA���΂̒��_���ǂ�ǂ�T���Ċ��蓖�ĂĂ���
						while (rest > 0){
							std::vector<int> nears;
							if (ids[2] == -1) ids.resize(2);
							if (ids[1] == -1) ids.resize(1);
							FindNearests(nears, condVtxs[i][cur[i][j]].pos, condVtxs[1-i], ids);	//	�ׂ̒��_���߂����ɕԂ��B
							if (nears.size() == 0) break;
							for(unsigned k=0; rest > 0 && k<nears.size(); ++k){
								if (condVtxs[1-i][nears[k]].area - condVtxs[1-i][nears[k]].assign > rest){
									AddCompanion(condVtxs[1-i], nears[k], condVtxs[i], cur[i][j], rest);
									rest = 0;
								}else{
									double delta = condVtxs[1-i][nears[k]].area - condVtxs[1-i][nears[k]].assign;
									if (delta > 0){
										AddCompanion(condVtxs[1-i], nears[k], condVtxs[i], cur[i][j], delta);
										rest -= delta;
									}
								}
							}
							if (rest > 0){
								ids.insert(ids.end(), nears.begin(), nears.end());
								nears.clear();
							}
						}
					}
				}
				for(int i=0; i<2; ++i){
					std::vector<int> next;
					FindNext(next, cur[i], used[i], condVtxs[i]);
					used[i].insert(used[i].end(), cur[i].begin(), cur[i].end());
					cur[i] = next;
				}
			}


			//	�ʒu�̊m�F
/*			for(unsigned i=0; i<2; ++i){
				DSTR << "mesh" << i << " " << condVtxs[i].size() << "/" 
					<< condVtxs[i].pmesh->vertices.size() << "vtxs" << std::endl;
				for(unsigned j=0; j<condVtxs[i].size(); ++j){
					Vec3d pos = sp->shapePoseW[i] * condVtxs[i].pmesh->vertices[condVtxs[i][j].id].pos;
					//Vec3d pos = condVtxs[i][j].pos;
					for(int ax=0; ax<3; ++ax){
						DSTR << "\t" << pos[ax];
					}
					DSTR << std::endl;
				}
				DSTR << "Outside:" << std::endl;
				for(unsigned j=0; j<condVtxs[i].vtx2Cond.size(); ++j){
					if (condVtxs[i].vtx2Cond[j] < 0){
						Vec3d pos = sp->shapePoseW[i] * condVtxs[i].pmesh->vertices[j].pos;
						for(int ax=0; ax<3; ++ax){
							DSTR << "\t" << pos[ax];
						}
						DSTR << std::endl;
					}
				}
			}
*/

			/// 2���̂̊ԂŔM�`�B���E�������p�ɂ��M�`�B�v�Z���s��
			for(unsigned i =0;i < condVtxs[0].size(); i++){
				//	�Ή�����ߓ_(companions[j])�̉��x���g���ĔM�`�B�̌v�Z���s��
				for(unsigned j =0;j < condVtxs[0][i].companions.size(); j++){
					double dqdt = 
					condVtxs[0].pmesh->vertices[condVtxs[0][i].vid].heatTransRatio * ( condVtxs[0].pmesh->vertices[condVtxs[0][i].vid].temp 
					- condVtxs[1].pmesh->vertices[condVtxs[0][i].companions[j].id].temp ) * condVtxs[0][i].companions[j].area ;//
					
					//>	dqdt *= 1e4;

					// condvtx[0]��Vecf ��dqdt �𑫂�
					condVtxs[0].pmesh->SetvecFAll(condVtxs[0][i].vid,-dqdt);

					// condVtx[1]��companion.id�Ԗڂ�Vecf�������
					condVtxs[1].pmesh->SetvecFAll(condVtxs[1][condVtxs[0][i].companions[j].id].vid, +dqdt);
				}
			}
		}

		//�ȉ��̌v�Z���������A�d���I�I�I�I�㉺�̃R�[�h�͏d���Ȃ��B
		//	�ȉ��̌v�Z�́A��̃R�[�h�������Ă���̂��m�肽���āA�}�����炦�̃R�s�y�R�[�h�Ȃ̂ŁA������Ƃ��������������āA�M�`�B������
		//	�ȉ��A���S�~�ƂȂ�������

		//>	���S�~�̕����́A�M�������E������������
		
		//	Pan��phmesh������Ă���

		//�@SUrfaceMEsh��face�ɂ��āAface�̐ߓ_���A


		//	distFromOrigin������͈͓̔��ɂ��鎞�A
		//	�M������������

		//	


		//	���x�A��M��f�ނ��Ƃɐݒ�
		//	�M������������͈͂�ݒ�H
			//�@�A�v���P�[�V�����ŕς���K�v�͂Ȃ��H��PHFemMeshThermo()�Őݒ�A�����������M���[�h��I�Ԃ̂��A�v���P�[�V����


		
		//FWFemMeshIf* tmesh	= GetSdk()->GetScene()->FindObject("fwPan")->Cast();
		//PHFemMeshIf* phpanmesh = tmesh->GetPHMesh();
		//PHFemMeshThermoIf* pfem = NULL;
		//double tempTc =10.0;
		//for(unsigned int i=0; i<tmesh->NChildObject() && !pfem; ++i){
		//	pfem = tmesh->GetChildObject(i)->Cast();
		//	if(pfem){
		//			���M���x�̏オ����𐧌�
		//		if(tempTc <= 250.0){ tempTc += tempTc * pfem->GetStepCount() * 0.02;}		//negi test 0.02 // cheese 0.01
		//		else{
		//			tempTc = 250.0;
		//		}
		//		for(unsigned j =0; j < pfem->NSurfaceVertices(); j++){
		//			Vec3d pfemPose = pfem->GetPose(pfem->GetSurfaceVertex(j));
		//			//DSTR << j << " th pfemPose.y: " << pfemPose.y << std::endl;		// -0.0016 �� 0.0016 �������Ă���B�}�C�i�X�̕��������M
		//			//	���S�~��ɉ��M
		//			//	test code �����S���ɉ��M
		//			if( pfemPose.y < 0.0){
		//				//pfem->SetVertexTc(j,tempTc,25.0);			//>	���̊֐��̌Ăяo�������d
		//				//���͂�A���͎g��Ȃ��B�M�`�B���E�����́A#if�ȉ��̃R�[�h�𗘗p
		//			}
		//		}
		//		pfem->SetVertexTc(0,200.0,25.0);
		//	}
		//}


		//FWFemMeshIf* tmeshN	= GetSdk()->GetScene()->FindObject("fwNegi")->Cast();
		//PHFemMeshIf* phpanmeshN = tmeshN->GetPHMesh();
		//PHFemMeshThermoIf* pfemN = NULL;
		//double tempTcN =10.0;
		//for(unsigned int i=0; i<tmeshN->NChildObject() && !pfemN; ++i){
		//	pfemN = tmeshN->GetChildObject(i)->Cast();
		//	if(pfemN){
		//		///	���M���x�̏オ����𐧌�
		//		//if(tempTcN <= 250.0){ tempTcN += tempTcN * pfemN->GetStepCount() * 0.02;}		//negi test 0.02 // cheese 0.01
		//		//else{
		//		//	tempTcN = 250.0;
		//		//}
		//		//for(unsigned j =0; j < pfem->NSurfaceVertices(); j++){
		//		//	Vec3d pfemPose = pfem->GetPose(pfem->GetSurfaceVertex(j));
		//		//	//DSTR << j << " th pfemPose.y: " << pfemPose.y << std::endl;		// -0.0016 �� 0.0016 �������Ă���B�}�C�i�X�̕��������M
		//		//	//	���S�~��ɉ��M
		//		//	//	test code �����S���ɉ��M
		//		//	if( pfemPose.y < 0.0){
		//		//		//pfem->SetVertexTc(j,tempTc,25.0);			//>	���̊֐��̌Ăяo�������d
		//		//		//���͂�A���͎g��Ȃ��B�M�`�B���E�����́A#if�ȉ��̃R�[�h�𗘗p
		//		//	}
		//		//}
		//		for(unsigned j=0;j<5;j++){
		//			pfemN->SetVertexTc(j,200.0,25.0);
		//		}
		//	}
		//}

#else

//#endif
//#if 0
		//	�t���C�p��������Ă���
		FWObjectIf* pan		=	DCAST(FWObjectIf, GetSdk()->GetScene()->FindObject("fwPan"));
		//	�H�ނ�����Ă���
		FWFemMeshIf* tmesh	= GetSdk()->GetScene()->FindObject("fwNegi")->Cast();
//		FWFemMeshIf* tmesh	= GetSdk()->GetScene()->FindObject("fwCheese")->Cast();
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
			//	�t���C�p���̐��E���W�����
			Affinef afPan = pan->GetGRFrame()->GetWorldTransform();
			//	���M���̂̐��E���W�����
			Affinef afMesh = tmesh->GetGRFrame()->GetWorldTransform();
			//	afPan���猩���AafMesh�̍��W�n�ւ̕ϊ��s������
			Affinef afMeshToPan = afPan.inv() * afMesh;
			//DSTR << afPan <<std::endl;
			//DSTR << afMesh <<std::endl;
			//DSTR << afMeshToPan <<std::endl;

			PHFemMeshIf* phm = tmesh->GetPHMesh();
//			int nOfSurfaceVtx = phm->NSurfaceVertices();

			///	�t���C�p���ɂƂ��Ă̌��_����̋����ɉ����āA���M����
			////	�ŊO�k�̐ߓ_�̃t���C�p������̍��W
			double tempTc =10.0;			//negitest 10.0		///	cheese 100.0

			
			//	pfem�̒�`
			//	PHFemMEshTherm�̐ߓ_������Ă��āA���M����
			PHFemMeshThermoIf* pfem = NULL;

			///	���b�V����PosOnPan���W�����āA���W�����������ɕ��ׂ�
			std::list<double> posOnPany;

			for(unsigned int i=0; i<tmesh->NChildObject() && !pfem; ++i){
				pfem = tmesh->GetChildObject(i)->Cast();
				//	pfem�����Ă��邱�Ƃ��m�F
				if(pfem){
					///	���M���x�̏オ����𐧌�
					if(tempTc <= 250.0){ tempTc += tempTc * pfem->GetStepCount() * 0.02;}		//negi test 0.02 // cheese 0.01
					else{
						tempTc = 250.0;
					}

					//DSTR << pfem->NSurfaceVertices() <<std::endl;
					for(unsigned j =0; j < pfem->NSurfaceVertices(); j++){
						Vec3d pfemPose = pfem->GetPose(pfem->GetSurfaceVertex(j));
						Vec3d posOnPan = afMeshToPan * pfemPose;
						//DSTR << j <<"th pfemPose: " << pfemPose << std::endl;
						//Vec3d pfemPose_ = pfem->GetSufVtxPose(pfem->GetSurfaceVertex(j));
						//DSTR << j <<"th pfemPose_: " << pfemPose_ << std::endl;

						//> ���b�V���̕\�ʂ̐ߓ_vertex���W������t���C�p�����W�n�̂Ƃ���͈͂ɂ��鎞�A�M�`�B���E�����ŉ��M����
						//if(posOnPan.y >= -0.03 && posOnPan.y <= -0.02 ){				///	pfemPose.y >= -0.0076 && pfemPose.y <= -0.0074 /// cube_test.x�p	//	 -0.01 <= pfemPose.y <= 0.0
						//if(posOnPan.y >= -0.03 && posOnPan.y <= -0.020 ){		///	cheese
						if(posOnPan.y >= -0.0076 && posOnPan.y <= -0.0070 ){
//							pfem->SetVertexTc(j,tempTc);
							pfem->SetVertexTc(j,tempTc,25.0);
							//pfem->SetVertexTemp(j,25.0);
							//DSTR << "pfem->GetStepCount(): " << pfem->GetStepCount() << std::endl;
							//DSTR << "afPan: " << afPan <<std::endl;
							//DSTR << "afMesh: " << afMesh <<std::endl;
							posOnPany.push_back(posOnPan.y);
							//DSTR << "j: "<<j<< ", posOnPan.y: " << posOnPan.y << std::endl;
							//UsingHeatTransferBoundaryCondition		���Ăяo��
							//DSTR << j << "th vertex.Tc��" << tempTc << "��ݒ�" <<std::endl;
							//Tc�̍X�V�H
						}
					}
				}
			}
			//posOnPany.sort();
			//DSTR << "posOnPany.front(): " <<posOnPany.front() << std::endl;
			//for(unsigned i=0;i<50;i++){
			//	DSTR << posOnPany[i] << std::endl;
			//}

			int kattton =0;
		
			//for(unsigned i=-0; i < nOfSurfaceVtx; i++){
			//	DSTR <<i <<"th: phm->GetPose(i): " << phm->GetPose(i) << std::endl;
			//	if(posOnPan.y >= -1.0 && posOnPan.y <= 0.3){
			//		DSTR << i << "th posOnPan.y: " << posOnPan.y << std::endl; 
			//	}
			//}
			
		}
		int nSolidsPair	=	GetSdk()->GetPHSdk()->GetScene(0)->NSolidPairs();
		int nSolid		=	GetSdk()->GetPHSdk()->GetScene(0)->NSolids();

//		PHSolidPairForLCPIf* solidPair = GetSdk()->GetPHSdk()->GetScene(0)->GetSolidPair(

		//char grName = "";
		//char phName = "";

		//GRFrameIf* findFrame = DCAST(GRFrameIf, GetSdk()->GetScene()->GetGRScene()->FindObject(grName));
		//PHSolidIf* findSolid = DCAST(PHSolidIf, GetSdk()->GetScene()->GetPHScene()->FindObject(phName));
		//PHFemMeshThermo�I�u�W�F�N�g�����Amain�֐��Ŏg����悤�ɂ���֐�
			//CreatePHFemMeshThermo();
		//���ߓ_���A���v�f���A�ߓ_���W�A�v�f�̍\���Ȃǂ�o�^
			//PHFemMeshThermo�̃I�u�W�F�N�g�����ۂɁA�f�B�X�N���v�^�ɒl��ݒ肵�č��
		
		//�i�`�󂪕ς������A�}�g���N�X��x�N�g������蒼���j
		//���x�ω��E�ŐV�̎��Ԃ�{T}�c�x�N�g���ɋL�ڂ���Ă���ߓ_���x����ɉ��w�ω��V�~�����[�V�������s��
			//SetChemicalSimulation();
			//���w�ω��V�~�����[�V�����ɕK�v�ȉ��x�Ȃǂ̃p�����[�^��n��
		//���x�ω��≻�w�V�~�����[�V�����̌��ʂ̓O���t�B�N�X�\�����s��
			
#endif
	}
	//>	�Η��E���˓`�M�̋����ɔ��E����Ⴕ�ĉ�����M�ʂ��ς��֐��̎���
	void hogehoge(){
		///	��̈ʒu
		double topOfPan=0.0;
		///	�H�ނ̈ʒu
		double altitudeOfFood=0.0;

		///	�Η��A�t�˂ɂ����H�W��
		double a_0 = 0;
			///	��ϐ��ߎ����ǂ����A���ϐ��ߎ����ǂ���
				/// ���W���Aa * x^3 +  b * x^2  + c * x^1
		///	���̐H�ނ�x,y�ʒu�̎ˉe�̃t���C�p���\�ʂ̉��x
		double tempP=0.0;
				///	�H�ו��Ɠ���xy�ʒu(���E���W)�̃t���C�p���̉��x������Ă���H
		 a_0 * (topOfPan - altitudeOfFood) * tempP; ///	�P�ʂ̎���[K/m]

	}
	void CreatePHFemMeshThermo(){
		PHFemMeshThermoIf* phmesht;
		PHFemMeshThermoDesc desc;
	}
	void SetInductionHeating(double radius,double width,double heatFlux){						//>	�������M�����@heatFlux
		double inRadius = radius - width /2;		//> �����a
		double outRadius = radius + width /2; 		//>	�O���a

		//>�@�����a�ƊO���a�̊Ԃ�temp�ŉ��M����




	}
	void SetInductionHeating2(double radius,double width,double temp,double ){			//>	���a�̓������͊O���ɍs���ɂ�āA���M��������������
		double inRadius = radius - width /2;		//> �����a
		double outRadius = radius + width /2; 		//>	�O���a

		//>�@�����a�ƊO���a�̊Ԃ�temp�ŉ��M����




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

