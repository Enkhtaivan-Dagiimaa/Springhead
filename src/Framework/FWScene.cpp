/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include <Framework/FWScene.h>
#include <Framework/FWObject.h>
#include <Framework/FWSdk.h>
#include <Physics/PHScene.h>
#include <Physics/PHSdk.h>
#include <Physics/PHContactPoint.h>
#include <Physics/PHConstraintEngine.h>
#include <Physics/PHHapticEngine.h>
#include <Physics/PHBallJoint.h>
#include <Physics/PHHingeJoint.h>
#include <Physics/PHSliderJoint.h>
#include <Graphics/GRScene.h>
#include <Graphics/GRSdk.h>
#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

namespace Spr{;

FWScene::FWScene(const FWSceneDesc& d) : phScene(NULL), grScene(NULL){
	// �f�t�H���g�`��ݒ�
	renderPHScene = true;
	// �\���b�h�`��̂�
	renderSolid = true;
	renderWire  = false;
	// �t�����̕`��͂Ȃ�
	renderAxisWorld = renderAxisSolid = renderAxisConst = false;
	renderForceSolid = renderForceConst	= false;
	renderGridX = renderGridY = renderGridZ = false;
	renderLimit     = false;
	renderContact	= false;
	renderIK		= false;
	// �{���͓��{
	scaleAxisWorld = scaleAxisSolid = scaleAxisConst = 1.0f;
	scaleForce = 1.0f;
	scaleMoment = 1.0f;
	scaleIK = 1.0f;
	// �}�e���A��
	matAxis.x	= GRRenderIf::RED;
	matAxis.y	= GRRenderIf::GREEN;
	matAxis.z	= GRRenderIf::BLUE;
	matContact	= GRRenderIf::YELLOW;
	matForce	= GRRenderIf::ORANGE;
	matMoment	= GRRenderIf::CYAN;
	matGrid.x = matGrid.y = matGrid.z = GRRenderIf::GRAY;
	// ���W��
	axisStyle = FWSceneIf::AXIS_LINES;
	// �O���b�h
	gridOffset.clear();
	gridSize = Vec3f(100.0f, 100.0f, 100.0f);
	gridSlice = Vec3i(100, 100, 100);
}

NamedObjectIf* FWScene::FindObject(UTString name, UTString cls){
	//	�]����phScene,grScene��2�̃V�[������������̂ŁCNameManager::FindObject�Ƃ�����ƈႤ�D
	//	2�̃V�[���́CSdk�ɏ��L����Ă���̂ŁC���ʂ�NameManager�Ƃ��āCFWScene���w�肷�邱�Ƃ͂ł��Ȃ��D

	//	�܂������Ǝq��������
	NamedObjectIf* rv = FindObjectFromDescendant(name, cls);
	if (rv) return rv;
	//	��c������
	rv = FindObjectFromAncestor(name, cls);
	if (rv) return rv;

	//	�Ȃ���΁CphScene��grScene�ɂ��Ď����Ǝq����T���B
	if (phScene){
		rv = DCAST(PHScene, phScene)->FindObjectFromDescendant(name, cls);
		if (rv) return rv;
	}
	if (grScene){
		rv = DCAST(GRScene, grScene)->FindObjectFromDescendant(name, cls);
		if (rv) return rv;
	}

	//	����ł��Ȃ��Ȃ�΁Anamespace������āA������x����
	size_t pos = name.find('/');
	if (pos != UTString::npos){	//	 ���O��Ԃ̎w�肪����ꍇ
		UTString n = name.substr(pos+1);
		rv = FindObject(n, cls);
	}
	return rv;
}

ObjectIf* FWScene::CreateObject(const IfInfo* info, const void* desc){
	ObjectIf* rv = Scene::CreateObject(info, desc);
	if (!rv && phScene) rv = phScene->CreateObject(info, desc);
	if (!rv && grScene) rv = grScene->CreateObject(info, desc);
	return rv;
}

FWObjectIf* FWScene::CreateFWObject(){
	FWObjectDesc desc;
	FWObjectIf* obj = DCAST(FWObjectIf, CreateObject(FWObjectIf::GetIfInfoStatic(), &desc));
	obj->SetPHSolid(NULL);
	obj->SetGRFrame(NULL);
	AddChildObject(obj);
	return obj;
}

bool FWScene::AddChildObject(ObjectIf* o){
	FWSdkIf* sdk = DCAST(FWSdkIf, GetNameManager());
	bool ok = false;
	if (!ok) {
		FWObject* obj = DCAST(FWObject, o);
		if (obj) {
			fwObjects.push_back(obj->Cast());
			obj->SetScene(Cast());
			// �f�t�H���g�l�[��
			if(strcmp(obj->GetName(), "") == 0){
				char name[256];
				sprintf(name, "object%d", NObject()-1);
				obj->SetName(name);
			}
			ok = true;
		}
	}
	if (!ok) {
		PHScene* obj = DCAST(PHScene, o);
		if (obj) {
			phScene = obj->Cast();
			sdk->GetPHSdk()->AddChildObject(obj->Cast());
			ok = true;
		}
	}
	if (!ok) {
		GRScene* obj = DCAST(GRScene, o);
		if (obj) {
			grScene = obj->Cast();
			sdk->GetGRSdk()->AddChildObject(obj->Cast());
			ok = true;
		}
	}
	if (!ok && phScene) {
		ok = phScene->AddChildObject(o);
	}
	if (!ok && grScene) {
		ok = grScene->AddChildObject(o);
	}
	return ok;
}

bool FWScene::DelChildObject(ObjectIf* o){
	FWObjectIf* obj = DCAST(FWObjectIf, o);
	if(obj){
		FWObjects::iterator it = find(fwObjects.begin(), fwObjects.end(), obj);
		if(it != fwObjects.end()){
			fwObjects.erase(it);
			return true;
		}
	}
	return false;
}

HIForceDevice6D* FWScene::GetHumanInterface(size_t pos){
/*	hase:	TBW
	if (pos < humanInterfaces.size()) return humanInterfaces[pos];
*/	return NULL;
}

size_t FWScene::NChildObject() const{
	return fwObjects.size() + (grScene?1:0) + (phScene?1:0);
}

ObjectIf* FWScene::GetChildObject(size_t pos){
	if (phScene){
		if (pos == 0) return phScene;
		pos--;
	}
	if (grScene){
		if (pos == 0) return grScene;
		pos--;
	}	
	if (pos < fwObjects.size()) return fwObjects[pos];
	return NULL;
}

FWSceneIf* SPR_CDECL CreateFWScene(){
	FWScene* rv = DBG_NEW FWScene;
	return rv->Cast();
}

FWSceneIf* SPR_CDECL CreateFWScene(const void* desc){
	FWScene* rv = DBG_NEW FWScene(*(FWSceneDesc*)desc);
	return rv->Cast();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void FWScene::Sync(bool ph_to_gr){
	//	�I�u�W�F�N�g�ʒu�E�p���̓���
	for(FWObjects::iterator it = fwObjects.begin(); it!=fwObjects.end(); ++it){
		DCAST(FWObject, *it)->Sync(ph_to_gr);
	}
	
	//	�J�����̓��� (�������H)
	if(ph_to_gr && grScene){
		HIForceDevice6D* device = GetHumanInterface(HI_CAMERACONTROLLER);
		GRCameraIf* camera = grScene->GetCamera();
		if(camera && device){
			Posed pose;
	/*		hase:	TBW	
			pose.Pos() = device->GetPos();	
			pose.Ori() = device->GetOri();
	*/		if (!camera->GetFrame()){
				GRSceneIf* scene = DCAST(GRSceneIf, camera->GetNameManager());
				if (scene) camera->SetFrame(DCAST(GRFrameIf, 
					scene->CreateVisual(GRFrameIf::GetIfInfoStatic(), GRFrameDesc())));
			}
			if (camera->GetFrame()){
				Affinef af;
				pose.ToAffine(af);
				camera->GetFrame()->SetTransform(af);
			}
		}
	}

}

void FWScene::Step(){
	if (phScene) phScene->Step();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// �`��n

void FWScene::Draw(GRRenderIf* render, bool ph_or_gr){
	render->ClearBuffer();
	render->BeginScene();
	
	if(ph_or_gr)
		DrawPHScene(render);
	else if(GetGRScene()){
		Sync();
		GetGRScene()->Render(render);
	}

	render->EndScene();
}

/// �V�[�����̑S�ẴI�u�W�F�N�g�������_�����O����
void FWScene::DrawPHScene(GRRenderIf* render){
	if (!phScene) return;

	// GRScene�̃J�����̎��_�𔽉f
	GRCameraIf* cam = NULL;
	if (grScene) cam = grScene->GetCamera();
	if (cam) cam->Render(render);
			
	// ������1���Z�b�g����Ȃ��ꍇ
	bool defLight = false;
	if(render->NLights() == 0){
		defLight = true;
		GRLightDesc ld;
		ld.diffuse = Vec4f(1,1,1,1) * 0.8f;
		ld.specular = Vec4f(1,1,1,1) * 0.8f;
		ld.ambient = Vec4f(1,1,1,1) * 0.4f;
		ld.position = Vec4f(1,1,1,0);
		render->PushLight(ld);
	}

	// ���[���h���W��
	if(renderAxisWorld){
		if(renderSolid)
			DrawCoordinateAxis(render, scaleAxisWorld, true);
		if(renderWire){
			render->SetLighting(false);
			DrawCoordinateAxis(render, scaleAxisWorld, false);
			render->SetLighting(true);
		}
	}

	// �O���b�h
	render->SetLighting(false);
	if(renderGridX){
		render->PushModelMatrix();
		render->MultModelMatrix(Affinef::Trn(gridOffset.x, 0.0f, 0.0f));
		render->MultModelMatrix(Affinef::Rot(Rad(90.0f), 'y'));
		render->SetMaterial(matGrid.x);
		render->DrawGrid(gridSize.x, gridSlice.x);
		render->PopModelMatrix();
	}
	if(renderGridY){
		render->PushModelMatrix();
		render->MultModelMatrix(Affinef::Trn(0.0f, gridOffset.y, 0.0f));
		render->MultModelMatrix(Affinef::Rot(Rad(90.0f), 'x'));
		render->SetMaterial(matGrid.y);
		render->DrawGrid(gridSize.y, gridSlice.y);
		render->PopModelMatrix();
	}
	if(renderGridZ){
		render->PushModelMatrix();
		render->MultModelMatrix(Affinef::Trn(0.0f, 0.0f, gridOffset.z));
		render->SetMaterial(matGrid.z);
		render->DrawGrid(gridSize.z, gridSlice.z);
		render->PopModelMatrix();
	}
	render->SetLighting(true);

	// ����
	PHSolidIf **solids = phScene->GetSolids();
	for(int i = 0; i < phScene->NSolids(); ++i){
		if(!IsRenderEnabled(solids[i]))
			continue;

		// �`���`��
		if(renderSolid){
			int matSolid = GetSolidMaterial(solids[i]);
			if(matSolid == -1)
				matSolid = i % GRRenderIf::MATERIAL_SAMPLE_END;
			render->SetMaterial(matSolid);
			DrawSolid(render, solids[i], true);
		}
		if(renderWire){
			int matWire  = GetWireMaterial(solids[i]);
			if(matWire == -1)
				matWire = i % GRRenderIf::MATERIAL_SAMPLE_END;
			render->SetMaterial(matWire);
			render->SetLighting(false);
			DrawSolid(render, solids[i], false);
			render->SetLighting(true);
		}
	}

	// �S��
	for(int i = 0; i < phScene->NJoints(); ++i){
		PHConstraintIf* con = phScene->GetJoint(i);
		if(IsRenderEnabled(con))
			DrawConstraint(render, con);
	}
	
	// �ڐG
	if(renderContact){
		for(int i = 0; i < phScene->NContacts(); ++i){
			PHContactPointIf* con = phScene->GetContact(i);
			if(IsRenderEnabled(con)){
				DrawConstraint(render, con);
				DrawContact(render, con);
			}
		}
	}

	// �֐߉���
	if(renderLimit){
		for(int i = 0; i < phScene->NJoints(); ++i){
			PHConstraintIf* con = phScene->GetJoint(i);
			if(IsRenderEnabled(con))
				DrawLimit(render, con);
		}
	}
	
	// Inverse Kinematics
	if(renderIK){
		//SetMaterialSample((GRRenderIf::TMaterialSample)2);
		PHIKEngineIf* ikEngine = phScene->GetIKEngine();
		if (ikEngine) {
			DrawIK(render, ikEngine);
		}
	}

	if(renderHaptic){
		PHHapticEngineIf* hapticEngine = phScene->GetHapticEngine();
		if(hapticEngine){
			DrawHaptic(render, hapticEngine);
		}
	}

	if(defLight) render->PopLight();
	if (cam) cam->Rendered(render);

}

/// ���̂������_�����O����
void FWScene::DrawSolid(GRRenderIf* render, PHSolidIf* solid, bool solid_or_wire){
	Affinef aff;
	solid->GetPose().ToAffine(aff);
	render->PushModelMatrix();
	render->MultModelMatrix(aff);
	
	for(int s = 0; s < solid->NShape(); ++s){
		CDShapeIf* shape = solid->GetShape(s);
		if(IsRenderEnabled(shape)){
			solid->GetShapePose(s).ToAffine(aff);
			render->PushModelMatrix();
			render->MultModelMatrix(aff);
			DrawShape(render, shape, solid_or_wire);
			render->PopModelMatrix();
		}
	}

	// ���W��
	if(renderAxisSolid)
		DrawCoordinateAxis(render, scaleAxisSolid, solid_or_wire);

	// ��
	if(renderForceSolid){
		DrawForce(render, solid->GetForce(), solid->GetTorque());
	}


	render->PopModelMatrix();
}

void FWScene::DrawShape(GRRenderIf* render, CDShapeIf* shape, bool solid_or_wire){
	CDBoxIf*		box		= DCAST(CDBoxIf, shape);
	CDSphereIf*		sphere	= DCAST(CDSphereIf, shape);
	CDCapsuleIf*	cap		= DCAST(CDCapsuleIf, shape);
	CDRoundConeIf*	rc		= DCAST(CDRoundConeIf, shape);
	CDConvexMeshIf* mesh	= DCAST(CDConvexMeshIf, shape);

	// solid, wireframe�̏��ɕ`��
	const int slice = 16;
	if(box){
		Vec3f sz = box->GetBoxSize();
		render->DrawBox(sz.x, sz.y, sz.z, solid_or_wire);
	}
	if(sphere)	render->DrawSphere(sphere->GetRadius(), slice, slice, solid_or_wire);
	if(cap)		render->DrawCapsule(cap->GetRadius(), cap->GetLength(), slice, solid_or_wire);
	if(rc){
		Vec2f r = rc->GetRadius();
		render->DrawRoundCone(r[0], r[1], rc->GetLength(), slice, solid_or_wire);
	}
	if(mesh)	DrawMesh(render, mesh, solid_or_wire);
}

void FWScene::DrawConstraint(GRRenderIf* render, PHConstraintIf* con){
	Affinef aff;
	Vec3d f, t;

	Posed sock, plug;
	con->GetSocketPose(sock);
	con->GetPlugPose(plug);

	if(renderAxisConst || renderForceConst){
		// socket
		(con->GetSocketSolid()->GetPose() * sock).ToAffine(aff);
		render->PushModelMatrix();
		render->MultModelMatrix(aff);

		if(renderAxisConst){
			if(renderSolid)
				DrawCoordinateAxis(render, scaleAxisConst, true);
			if(renderWire){
				render->SetLighting(false);
				DrawCoordinateAxis(render, scaleAxisConst, false);
				render->SetLighting(true);
			}
		}
	
		if(renderForceConst){
			con->GetConstraintForce(f, t);
			DrawForce(render, f, t);
		}
		render->PopModelMatrix();
	}
	
	// plug
	if(renderAxisConst){
		(con->GetPlugSolid()->GetPose() * plug).ToAffine(aff);
		render->PushModelMatrix();
		render->MultModelMatrix(aff);
		if(renderSolid)
			DrawCoordinateAxis(render, scaleAxisConst, true);
		if(renderWire){
			render->SetLighting(false);
			DrawCoordinateAxis(render, scaleAxisConst, false);
			render->SetLighting(true);
		}
		render->PopModelMatrix();
	}

	// �ǉ��S�����W�n�i�ꕔ��JointLimit���֐ߖ{�̂Ƃ͕ʂɎ����Ă���j
	if(renderAxisConst){
		PHBallJoint* bj = con->Cast();
		if (bj) {
			PHBallJointLimit* limit = bj->GetLimit()->Cast();
			if (limit) {
				con->GetSocketSolid()->GetPose().ToAffine(aff);
				Matrix3d Jcinv = bj->limit->J[0] * bj->J[0].ww().inv();
				aff.Rot() = Jcinv.trans() * aff.Rot();
				aff.Trn() = con->GetPlugSolid()->GetPose().Pos();

				render->PushModelMatrix();
				render->MultModelMatrix(aff);
				render->SetLighting(false);
				DrawCoordinateAxis(render, scaleAxisConst*0.5, false);
				render->SetLighting(true);
				render->PopModelMatrix();
			}
		}
	}
}

void FWScene::DrawLimit(GRRenderIf* render, PHConstraintIf* con){
	Affinef aff;

	Posed sock, plug;
	con->GetSocketPose(sock);
	con->GetPlugPose(plug);

	if (renderLimit) {

		// �{�[���W���C���g��Limit
		PHBallJoint* bj = con->Cast();
		if (bj) {
			(con->GetPlugSolid()->GetPose() * plug).ToAffine(aff);
			render->PushModelMatrix();
			render->MultModelMatrix(aff);
			render->SetLighting(false);
			render->SetMaterial(matAxis.z);
			float vtx[2][3] = {{0,0,0}, {0,0,1}};
			size_t idx[2] = {0, 1};
			render->DrawIndexed(GRRenderBaseIf::LINES, idx, (void*)vtx, 2);
			render->SetLighting(true);
			render->PopModelMatrix();

			(con->GetSocketSolid()->GetPose() * sock).ToAffine(aff);
			render->PushModelMatrix();
			render->MultModelMatrix(aff);
			render->SetLighting(false);

			// Spline����Ȑ��̕\��
			PHBallJointSplineLimit* spL = bj->GetLimit()->Cast();
			if (spL) {
				for (int i=0; i<spL->limitCurve.NEdges(); ++i) {
					SplinePoint pt = spL->limitCurve.GetPointOnEdge(i, 0);
					if (i==0) {
						render->SetMaterial(matAxis.x);
					} else if (i%2==0) {
						render->SetMaterial(matAxis.y);
					} else {
						render->SetMaterial(matAxis.z);
					}
					for (double t=0; t<1.0; t+=0.1) {
						pt.t = t; pt.Update();
						Vec3d p0 = Vec3d(sin(pt.pos[0])*cos(pt.pos[1]), sin(pt.pos[0])*sin(pt.pos[1]), cos(pt.pos[0]));

						pt.t = (t+0.1 > 1.0) ? 1.0 : (t+0.1); pt.Update();
						Vec3d p1 = Vec3d(sin(pt.pos[0])*cos(pt.pos[1]), sin(pt.pos[0])*sin(pt.pos[1]), cos(pt.pos[0]));

						float vtx[2][3] = {{p0[0], p0[1], p0[2]}, {p1[0], p1[1], p1[2]}};
						size_t idx[2] = {0, 1};
						render->DrawIndexed(GRRenderBaseIf::LINES, idx, (void*)vtx, 2);
					}
				}
			}

			// �~�`����Ȑ��̕\��
			PHBallJointConeLimit* coL = bj->GetLimit()->Cast();
			if (coL) {
				// �i���j����limitDir�ɂ͔�Ή�<!!>
				Vec2d lim; coL->GetSwingRange(lim);
				for (double t=0; t<2*M_PI; t+=Rad(10)) {
					double z = cos(lim[1]);
					double r = z * tan(lim[1]);
					float vtx[2][3] = {{r*cos(t), r*sin(t), z}, {r*cos(t+Rad(10)), r*sin(t+Rad(10)), z}};
					size_t idx[2] = {0, 1};
					render->SetMaterial(matAxis.z);
					render->DrawIndexed(GRRenderBaseIf::LINES, idx, (void*)vtx, 2);
				}
			}

			render->SetLighting(true);
			render->PopModelMatrix();
		}

		// �q���W�W���C���g��Limit
		PHHingeJoint* hj = con->Cast();
		if (hj) {
			PH1DJointLimit* limit = hj->GetLimit()->Cast();
			if (limit) {
				con->GetSocketSolid()->GetPose().ToAffine(aff);
				render->PushModelMatrix();
				render->MultModelMatrix(aff);
				render->SetLighting(false);
				Vec2d lim; limit->GetRange(lim);
				if (lim[0] < lim[1]) {
					for (double t=lim[0]; t<lim[1]; t+=Rad(5)) {
						double r = 1.0;
						float vtx[2][3] = {{r*cos(t), r*sin(t), 0.0}, {r*cos(t+Rad(10)), r*sin(t+Rad(10)), 0.0}};
						size_t idx[2] = {0, 1};
						render->SetMaterial(matAxis.z);
						render->DrawIndexed(GRRenderBaseIf::LINES, idx, (void*)vtx, 2);
					}
				}
				{
					render->SetMaterial(matAxis.z);
					float vtx[2][3] = {{0,0,0}, {1*cos(hj->GetPosition()),1*sin(hj->GetPosition()),0}};
					size_t idx[2] = {0, 1};
					render->DrawIndexed(GRRenderBaseIf::LINES, idx, (void*)vtx, 2);
				}
				render->SetLighting(true);
				render->PopModelMatrix();
			}
		}

		// �X���C�_�W���C���g�̂�Limit
		PHSliderJoint* sj = con->Cast();
		if (sj) {
			PH1DJointLimit* limit = sj->GetLimit()->Cast();
			if (limit) {
				con->GetSocketSolid()->GetPose().ToAffine(aff);
				render->PushModelMatrix();
				render->MultModelMatrix(aff);
				render->SetLighting(false);
				Vec2d lim; limit->GetRange(lim);
				if (lim[0] < lim[1]) {
					float vtx[2][3] = {{lim[0], 0, 0}, {lim[1], 0, 0}};
					size_t idx[2] = {0, 1};
					render->SetMaterial(matAxis.z);
					render->DrawIndexed(GRRenderBaseIf::LINES, idx, (void*)vtx, 2);
				}
				render->SetLighting(true);
				render->PopModelMatrix();
			}
		}
	}
}

void FWScene::DrawContact(GRRenderIf* render, PHContactPointIf* con){
	render->SetMaterial(matContact);

	PHContactPoint* c = con->Cast();
	if(c->shapePair->section.size() < 3)
		return;
	std::vector<Vec3f> vtx;
	vtx.resize(c->shapePair->section.size());
	copy(c->shapePair->section.begin(), c->shapePair->section.end(), vtx.begin());
	
	render->SetLighting(false);
	render->SetDepthTest(false);
	
	render->SetVertexFormat(GRVertexElement::vfP3f);
	render->DrawDirect(GRRenderBaseIf::LINE_LOOP, &vtx[0], vtx.size());
	
	render->SetDepthTest(true);
	render->SetLighting(true);
}

/// IK�̌v�Z���ʂ������_�����O����
void FWScene::DrawIK(GRRenderIf* render, PHIKEngineIf* ikEngine) {
	render->SetLighting(false);
	render->SetDepthTest(false);

	render->PushModelMatrix();
	render->SetModelMatrix(Affinef());

	// IK�����API�ύX�ɑΉ�����悤�ɂ����ꏑ��������@�i10/01/09, mitake�j
	for (size_t i=0; i < DCAST(PHIKEngine,ikEngine)->actuators.size(); ++i) {
		PHIKActuator* ikAct = DCAST(PHIKEngine,ikEngine)->actuators[i];
		if(!ikAct) continue;
		
		PHIKBallActuator* ikBJ = ikAct->Cast();
		if (ikBJ) {
			Vec3d w;
			for (int j=0; j < (int)ikBJ->omega.size(); ++j) {
				w += (ikBJ->omega[j]/ikBJ->GetBias()) * ikBJ->e[j];
			}
			PHBallJointIf* jt = ikBJ->joint;
			PHBallJointDesc d;
			jt->GetDesc(&d);
			Vec3d Pj = jt->GetSocketSolid()->GetPose() * d.poseSocket.Pos();

			{
				GRMaterialDesc mat;
				mat.ambient = mat.diffuse = Vec4f(1,0,0.7,1);
				render->SetMaterial(mat);
				render->DrawLine(Pj, Pj + (w * scaleIK));
			}

			{
				GRMaterialDesc mat;
				mat.ambient = mat.diffuse = Vec4f(1.0,0,0,1);
				render->SetMaterial(mat);
				for (int j=0; j < ikBJ->ndof; ++j) {
					render->DrawLine(Pj, Pj + (ikBJ->e[j]));
				}
			}
		}

		PHIKHingeActuator* ikHJ = ikAct->Cast();
		if (ikHJ) {
			PHHingeJointIf* jt = ikHJ->joint;
			PHHingeJointDesc d; jt->GetDesc(&d);

			Vec3d Pj = jt->GetSocketSolid()->GetPose() * d.poseSocket.Pos();
			Vec3d wD = jt->GetSocketSolid()->GetPose().Ori() * d.poseSocket.Ori() * Vec3d(0,0,1);

			double w = 0;
			if (ikHJ->omega.size() != 0) {
				w = (ikHJ->omega[0]/ikHJ->GetBias());
			}

			{
				GRMaterialDesc mat;
				mat.ambient = mat.diffuse = Vec4f(1.0,0,0,1);
				render->SetMaterial(mat);
				render->DrawLine(Pj, Pj + (w * scaleIK * wD));
			}
		}
	}

	/*
	for (size_t i=0; i < DCAST(PHIKEngine,ikEngine)->endeffectors.size(); ++i) {
		PHIKEndEffector* ikEE = DCAST(PHIKEngine,ikEngine)->endeffectors[i];
		if (ikEE && DCAST(PHIKEndEffector,ikEE)->bEnabled) {
			Vec3d sp = ikEE->solid->GetPose() * ikEE->targetLocalPosition;
			Vec3d tg = ikEE->GetTempTarget();
			{
				GRMaterialDesc mat;
				mat.ambient = mat.diffuse = Vec4f(0,1.0,0,1);
				SetMaterial(mat);
				DrawLine(sp, sp + tg);
			}
		}
	}
	*/

	render->PopModelMatrix();
	render->SetLighting(true);
	render->SetDepthTest(true);
}

void FWScene::DrawHaptic(GRRenderIf* render, PHHapticEngineIf* hapticEngine) {
	// �A�N�Z�X�ᔽ�����\�N����̂łȂ�Ƃ����Ȃ��Ƃ����Ȃ�
	PHHapticEngine* he = DCAST(PHHapticEngine, hapticEngine);
	int Npointers = he->NHapticPointers();
	if(Npointers == 0) return;

	// �v���L�V�̕`��
	for(int i = 0; i< he->NHapticPointers(); i++){
		PHHapticPointer* pointer = he->GetHapticPointer(i);
		Posed proxyPose = pointer->proxyPose;
		Affinef aff;
		proxyPose.ToAffine(aff);
		render->PushModelMatrix();
		render->MultModelMatrix(aff);
	
		for(int j = 0; j < pointer->NShape(); ++j){
			CDShapeIf* shape = pointer->GetShape(j);
			if(IsRenderEnabled(shape)){
				pointer->GetShapePose(j).ToAffine(aff);
				render->PushModelMatrix();
				render->MultModelMatrix(aff);
				DrawShape(render, shape, false);
				render->PopModelMatrix();
			}
		}
		render->PopModelMatrix();
	}

	render->SetLighting(false);
	render->SetDepthTest(false);

	render->PushModelMatrix();
	render->SetModelMatrix(Affinef());
	for (int i = 0; i < Npointers; i++){
		PHHapticPointer* pointer = he->GetHapticPointer(i);
		int nNeighbors = (int)pointer->neighborSolidIDs.size();
		for(int j = 0; j < nNeighbors; j++){
			int solidID = pointer->neighborSolidIDs[j];
			PHSolidPairForHaptic* solidPair = he->solidPairs.item(solidID, i);
			for(int k = 0; k < solidPair->solid[0]->NShape(); k++){
				for(int l = 0; l < solidPair->solid[1]->NShape(); l++){
					PHShapePairForHaptic* sp = solidPair->shapePairs.item(k, l);
					for(int m = 0; m < 2; m++){
						// �ߖT�_��
						Posed p;
						p.Pos() = sp->shapePoseW[m] * sp->closestPoint[m];
						Affinef aff;
						p.ToAffine(aff);
						render->PushModelMatrix();
						render->MultModelMatrix(aff);
						render->SetMaterial(GRRenderIf::WHITE);
						render->DrawSphere(0.01f, 10, 10, true);
						render->PopModelMatrix();
					}
					// �ڐG�_
					for(int m = 0; m < (int)sp->intersectionVertices.size(); m++){
						Posed p;
						p.Pos() = sp->shapePoseW[1] * sp->intersectionVertices[m];
						Affinef aff;
						p.ToAffine(aff);
						render->PushModelMatrix();
						render->MultModelMatrix(aff);
						render->SetMaterial(GRRenderIf::YELLOW);
						render->DrawSphere(0.01f, 10, 10, true);
						render->PopModelMatrix();
					}

					// ��
					Posed p;	// �ʂ̈ʒu�p��
					p.Pos() = sp->shapePoseW[0] * sp->closestPoint[0];
					Vec3d vec = Vec3d(0.0, 1.0, 0.0);
					double angle = acos(vec * sp->normal);
					Vec3d axis = vec % sp->normal;
					if(axis.norm() < 1e-5) axis = vec;
					p.Ori() = Quaterniond::Rot(angle, axis);
					
					Affinef aff;
					p.ToAffine(aff);
					Vec4f moon(1.0, 1.0, 0.8, 0.3);
					render->PushModelMatrix();
					render->MultModelMatrix(aff);
					render->SetMaterial( GRMaterialDesc(moon) );
					render->SetAlphaTest(true);
					render->SetAlphaMode(render->BF_SRCALPHA, render->BF_ONE);
					render->DrawBox(0.2f, 0.005f, 0.2f, true);
					render->PopModelMatrix();
					render->SetAlphaTest(false);
				}
			}
		}
	}

	render->PopModelMatrix();
	render->SetLighting(true);
	render->SetDepthTest(true);
}

void FWScene::DrawMesh(GRRenderIf* render, CDConvexMeshIf* mesh, bool solid){
	Vec3f* base = mesh->GetVertices();
	if(solid){
		for (int f=0; f<mesh->NFace(); ++f) {	
			CDFaceIf* face = mesh->GetFace(f);
			this->DrawFaceSolid(render, face, base);
		}
	}
	else{
		for (int f=0; f<mesh->NFace(); ++f) {	
			CDFaceIf* face = mesh->GetFace(f);
			this->DrawFaceWire(render, face, base);
		}
	}
}

void FWScene::DrawFaceSolid(GRRenderIf* render, CDFaceIf* face, Vec3f * base){
	int numIndices = face->NIndex();			// (=3 :�O�p�`�Ȃ̂�3���_)
	struct Vtx{
		Vec3f n;
		Vec3f p;
	} vtxs[10];
	assert((size_t)numIndices <= sizeof(vtxs)/sizeof(vtxs[0]));
	for(int v = 0; v < numIndices; ++v)
		vtxs[v].p = base[face->GetIndices()[v]].data;
	Vec3f edge0, edge1;
	edge0 = vtxs[1].p - vtxs[0].p;
	edge1 = vtxs[2].p - vtxs[0].p;
	Vec3f n = (edge0^edge1).unit();
	for(int v = 0; v < numIndices; ++v)
		vtxs[v].n = n;
	render->SetVertexFormat(GRVertexElement::vfN3fP3f);
	render->DrawDirect(GRRenderBaseIf::TRIANGLE_FAN, vtxs, numIndices);
}

void FWScene::DrawFaceWire(GRRenderIf* render, CDFaceIf* face, Vec3f * base){
	int numIndices = face->NIndex();
	Vec3f vtxs[10];
	assert((size_t)numIndices+1 <= sizeof(vtxs)/sizeof(vtxs[0]));
	int v;
	for(v = 0; v < numIndices; ++v)
		vtxs[v] = base[face->GetIndices()[v]].data;
	vtxs[v] = base[face->GetIndices()[0]].data;
	render->SetVertexFormat(GRVertexElement::vfP3f);
	render->DrawDirect(GRRenderBaseIf::LINES, vtxs, numIndices+1);
}

void FWScene::DrawCoordinateAxis(GRRenderIf* render, float scale, bool solid_or_wire){
	if(axisStyle == FWSceneIf::AXIS_LINES){
		render->SetVertexFormat(GRVertexElement::vfP3f);
		render->PushModelMatrix();
		render->MultModelMatrix(Affinef::Scale(scale, scale, scale));

		float vtx[4][3] = {{0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}};
		size_t idx_x[2] = {0, 1};
		size_t idx_y[2] = {0, 2};
		size_t idx_z[2] = {0, 3};
		render->SetMaterial(matAxis.x);
		render->DrawIndexed(GRRenderBaseIf::LINES, idx_x, (void*)vtx, 2);
		render->SetMaterial(matAxis.y);
		render->DrawIndexed(GRRenderBaseIf::LINES, idx_y, (void*)vtx, 2);
		render->SetMaterial(matAxis.z);
		render->DrawIndexed(GRRenderBaseIf::LINES, idx_z, (void*)vtx, 2);
	
		render->PopModelMatrix();
	}
	if(axisStyle == FWSceneIf::AXIS_ARROWS){
		float rbar  = 0.05f * scale;
		float rhead = 0.10f * scale;
		float lhead = 0.20f * scale;
		// x
		render->SetMaterial(matAxis.x);
		render->DrawArrow(Vec3f(), Vec3f(1.0f, 0.0f, 0.0f), rbar, rhead, lhead, 16, solid_or_wire);
		// y
		render->SetMaterial(matAxis.y);
		render->DrawArrow(Vec3f(), Vec3f(0.0f, 1.0f, 0.0f), rbar, rhead, lhead, 16, solid_or_wire);
		// z
		render->SetMaterial(matAxis.z);
		render->DrawArrow(Vec3f(), Vec3f(0.0f, 0.0f, 1.0f), rbar, rhead, lhead, 16, solid_or_wire);
	}
}

void FWScene::DrawForce(GRRenderIf* render, const Vec3d& f, const Vec3d& t){
	render->SetLighting(false);
	render->SetDepthTest(false);
	// constraint force
	render->SetMaterial(matForce);
	render->DrawLine(Vec3f(), scaleForce * f);
			
	// constraint moment
	render->SetMaterial(matMoment);
	render->DrawLine(Vec3f(), scaleMoment * t);
			
	render->SetDepthTest(true);
	render->SetLighting(true);
}

void FWScene::SetRenderMode(bool solid, bool wire){
	renderSolid = solid;
	renderWire  = wire;
}
void FWScene::EnableRender(ObjectIf* obj, bool enable){
	renderObject[obj] = enable;
}
void FWScene::SetSolidMaterial(int mat, PHSolidIf* solid){
	matSolid[solid] = mat;
}
void FWScene::SetWireMaterial(int mat, PHSolidIf* solid){
	matWire[solid] = mat;
}	
void FWScene::EnableRenderAxis(bool world, bool solid, bool con){
	renderAxisWorld = world;
	renderAxisSolid = solid;
	renderAxisConst = con;
}
void FWScene::SetAxisMaterial(int matX, int matY, int matZ){
	matAxis = Vec3i(matX, matY, matZ);
}
void FWScene::SetAxisScale(float scaleWorld, float scaleSolid, float scaleConst){
	scaleAxisWorld = scaleWorld;
	scaleAxisSolid = scaleSolid;
	scaleAxisConst = scaleConst;
}
void FWScene::SetAxisStyle(int style){
	axisStyle = style;
}
void FWScene::EnableRenderForce(bool solid, bool con){
	renderForceSolid = solid;
	renderForceConst = con;
}
void FWScene::SetForceMaterial(int matf, int matm){
	matForce	= matf;
	matMoment	= matm;	
}
void FWScene::SetForceScale(float scalef, float scalem){
	scaleForce	= scalef;
	scaleMoment = scalem;
}
void FWScene::EnableRenderContact(bool enable){
	renderContact = enable;
}
void FWScene::SetContactMaterial(int mat){
	matContact = mat;
}
void FWScene::EnableRenderGrid(bool x, bool y, bool z){
	renderGridX = x;
	renderGridY = y;
	renderGridZ = z;
}
void FWScene::SetGridOption(char axis, float offset, float size, int slice){
	int i = tolower(axis) - 'x';
	gridOffset[i] = offset;
	gridSize[i]   = size;
	gridSlice[i]  = slice;
}
void FWScene::SetGridMaterial(int matX, int matY, int matZ){
	matGrid = Vec3i(matX, matY, matZ);
}
void FWScene::EnableRenderIK(bool enable){
	renderIK = enable;
}
void FWScene::SetIKMaterial(int mat){
	matIK = mat;
}
void FWScene::SetIKScale(float scale){
	scaleIK = scale;
}
void FWScene::EnableRenderLimit(bool enable){
	renderLimit = enable;
}
void FWScene::EnableRenderHaptic(bool enable){
	renderHaptic = enable;
}
bool FWScene::IsRenderEnabled(ObjectIf* obj){
	std::map<ObjectIf*, bool>::iterator it = renderObject.find(obj);
	if(it != renderObject.end())
		return it->second;
	return true;
}
int FWScene::GetSolidMaterial(PHSolidIf* solid){
	// �ŏ��ɓ����solid�ɂ��Ă�ꂽ�}�e���A�������邩���ׁC
	// �Ȃ���Ύ���0 (�S����)�̃}�e���A���𒲂ׁC
	// �ǂ�����Ȃ����-1��Ԃ�
	std::map<PHSolidIf*, int>::iterator it;
	it = matSolid.find(solid);
	if(it != matSolid.end())
		return it->second;
	it = matSolid.find(0);
	if(it != matSolid.end())
		return it->second;
	return -1;
}
int FWScene::GetWireMaterial(PHSolidIf* solid){
	std::map<PHSolidIf*, int>::iterator it;
	it = matWire.find(solid);
	if(it != matWire.end())
		return it->second;
	it = matWire.find(0);
	if(it != matWire.end())
		return it->second;
	return -1;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
// HumanInterface�n

void FWScene::AddHumanInterface(HIForceDevice6D* d){
//	hase	TBW
	//	humanInterfaces.push_back(d);
}

void FWScene::SetFWBones(FWBoneIf* b){
	fwBones.push_back(b);
}

std::vector< UTRef<FWBoneIf> > FWScene::GetFWBones(){
	return fwBones;
}

void FWScene::CreateFWStructure(){
	FWStructureIf *fwStructure =DCAST(FWStructureIf ,new FWStructure);
	fwStructures.push_back(fwStructure);
}
void FWScene::AddFWStructure(FWStructureIf* o){
	return fwStructures.push_back(o);
}

FWStructureIf* FWScene::GetFWStructure(){
	return fwStructures.back();
}

FWStructureIf* FWScene::GetFWStructure(int n){
	if(n < (int)fwStructures.size()){
		return fwStructures[n];
	}
	return NULL;
}
size_t FWScene::NFWStructure(){
	return fwStructures.size();
}
}
