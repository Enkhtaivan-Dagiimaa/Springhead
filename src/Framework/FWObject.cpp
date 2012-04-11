/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include <Framework/FWObject.h>
#include <Framework/FWScene.h>
#include <Framework/FWSdk.h>
#include <Physics/PHConstraint.h>
#include <Physics/PHSdk.h>
#include <Graphics/GRFrame.h>
#include <Graphics/GRMesh.h>
#include <FileIO/FISdk.h>
#include <FileIO/FIFileX.h>
#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

namespace Spr{;

FWObject::FWObject(const FWObjectDesc& d/*=FWObjectDesc()*/)
: phSolid(NULL), grFrame(NULL), FWObjectDesc(d){
	solidLength=0;
}
SceneObjectIf* FWObject::CloneObject(){
	FWObjectIf* origin = DCAST(FWObjectIf,this);
	FWSceneIf* s = DCAST(FWSceneIf, GetScene());
	FWObjectIf* clone = s->CreateFWObject();

	if(origin->GetPHSolid())
		clone->SetPHSolid(origin->GetPHSolid()->CloneObject()->Cast());
	if(origin->GetGRFrame())
		clone->SetGRFrame(origin->GetGRFrame()->CloneObject()->Cast());

	return clone;
}

void FWObject::Sync(bool ph_to_gr){
	if(!phSolid || !grFrame){
		//DSTR << "Warning: No solid or frame for " << GetName() << ":FWObject." << std::endl;
		return;
	}
		
	// �{�[���t��X�t�@�C�����g�p����ꍇ
	if(solidLength){
		if(ph_to_gr){
			Affinef affSolid, affParFrame, afd, afl, AF;
			// ����
			phSolid->GetPose().ToAffine(affSolid);
			// �e�t���[��
			affParFrame = grFrame->GetParent()->GetWorldTransform();	

			afd = affParFrame.inv() * affSolid;								//�K�w�\������Affin�s��ɕϊ�����
			afl.PosZ() += (float)solidLength/2.0f;				//���̒��S�̈ʒu���獄�̂̔������������炵�C�W���C���g�����̈ʒu�ɂ���
			AF = afd * afl;
			grFrame->SetTransform(AF);
		}
		else{
			// undefined
		}
	}
	// �ʏ펞
	else{
		if(ph_to_gr){
			Affinef aff;
			phSolid->GetPose().ToAffine(aff);
			grFrame->SetTransform(aff);
		}
		else{
			Posed pose;
			pose.FromAffine(grFrame->GetTransform());
			phSolid->SetPose(pose);
		}
	}
}


bool FWObject::AddChildObject(ObjectIf* o){
	PHSolidIf* so = DCAST(PHSolidIf, o);
	if(so){
		phSolid = so;
		return true;
	}
	GRFrameIf* fr = DCAST(GRFrameIf, o);
	if(fr){
		grFrame = fr;
		return true;
	}
	GRMeshIf* m = DCAST(GRMeshIf, o);
	if(m){
		FWSceneIf* s=GetScene()->Cast();
		GRSceneIf* gs = s->GetGRScene();
		grFrame = gs->CreateVisual(GRFrameIf::GetIfInfoStatic(), GRFrameDesc())->Cast();
		grFrame->SetName("newFrameForMesh");
		grFrame->AddChildObject(m);
		return true;
	}
	return false;
}

ObjectIf* FWObject::GetChildObject(size_t pos){
	if (pos==0) if (phSolid) return phSolid; else return grFrame;
	if (pos==1) if (phSolid) return grFrame; else return NULL;
	return NULL;
}

size_t FWObject::NChildObject() const {
	return phSolid ? (grFrame ? 2 : 1) : (grFrame ? 1 : 0);
}

bool FWObject::LoadMesh(const char* filename, const IfInfo* ii, GRFrameIf* frame){
	FWScene* scene = DCAST(FWScene, GetScene());
	FISdkIf* fiSdk = scene->sdk->GetFISdk();

	ObjectIfs objs;
	objs.Push(scene->GetGRScene());					///< GRScene���쐬���C
	objs.Push(frame ? frame : GetGRFrame());		///< GRFrame������

	FIFileIf* file = (ii ? fiSdk->CreateFile(ii) : fiSdk->CreateFileFromExt(filename));
	if(!file)
		file = fiSdk->CreateFileX();

	return file->Load(objs, filename);
}

void FWObject::GenerateCDMesh(GRFrameIf* frame, const PHMaterial& mat){
	// �t���[�����X�L����
	for(int i = 0; i < frame->NChildren(); i++){
		GRMeshIf* mesh	 = DCAST(GRMeshIf,  frame->GetChildren()[i]);
		GRFrameIf* child = DCAST(GRFrameIf, frame->GetChildren()[i]);
		
		// �q���b�V��
		if(mesh){
			// desc���x���Œ��_���W���R�s�[
			GRMeshDesc		 grdesc;
			CDConvexMeshDesc cddesc;

			mesh->GetDesc(&grdesc);
			for(int i = 0; i < (int)grdesc.vertices.size(); i++)
				cddesc.vertices.push_back(grdesc.vertices[i]);

			// CDConvexMesh���쐬
			FWScene* scene = DCAST(FWScene, GetScene());
			CDShapeIf* shape = DCAST(CDConvexMeshIf, scene->sdk->GetPHSdk()->CreateShape(cddesc));

			// ���b�V����ۗL����t���[����pose�𔽉f
			Posed pose;
			pose.FromAffine(frame->GetWorldTransform());

			PHSolidIf* solid = GetPHSolid();
			solid->AddShape(shape);
			solid->SetShapePose(solid->NShape()-1, pose);
		}

		// �q�t���[���ɑ΂��čċA�Ăяo��
		if(child)
			GenerateCDMesh(child, mat);
	}

}

/// --- --- --- --- --- --- --- --- --- ---

FWBoneObject::FWBoneObject(const FWBoneObjectDesc& d/*=FWBoneObjectDesc()*/)
: desc(d), phJoint(NULL), endFrame(NULL), FWObject((const FWObjectDesc&)d)
{
	AdaptType = GRFRAME_TO_PHSOLID;
}

void FWBoneObject::Sync(bool ph_to_gr){
	if(AdaptType==GRFRAME_TO_PHSOLID){
		if (phSolid && grFrame && phJoint){
			//PHSolid�̈ʒu��GRFrame�����킹��
			Posed jointPosition;
			jointPosition.Ori() = phJoint->GetRelativePoseQ() * sockOffset.Ori().Inv();
			Posed poseSocket; phJoint->GetSocketPose(poseSocket);
			Posed pose = poseSocket * jointPosition;

			Affinef af; pose.ToAffine(af);
			DCAST(GRFrame, grFrame)->SetTransform(af);

			PHSolidIf *so1 = phJoint->GetSocketSolid(), *so2 = phJoint->GetPlugSolid();
			if (so1 && so2) {
				DCAST(FWSceneIf,GetScene())->GetPHScene()->SetContactMode(so1, so2, PHSceneDesc::MODE_NONE);
			}
		}
		if(phSolid && grFrame && (!phJoint)){
			//�ŏ���Frame�Ɋւ��鏈��
			Affinef af;
			phSolid->GetPose().ToAffine(af);
			DCAST(GRFrame, grFrame)->SetTransform(af);
		}
	}
	if(AdaptType==PHSOLID_TO_GRFRAME){
		//GRFrame�̈ʒu��PHSolid�����킹��
		if (phSolid && grFrame && phJoint){
			Affinef af = grFrame->GetWorldTransform();
			af.Orthonormalization(); //���K������
			//Matrix3d afr = af.Rot();
			//if(afr.det()<1.00){
			//	DSTR<<"�s�� : "<<afr.det()<<std::endl;
			//	DSTR<<af<<std::endl;
			//}
			Posed pose; pose.FromAffine(af);
			//�A�t�B���s�񁨃N�H�[�^�j�I���̕ϊ��덷���傫���ꍇ�̃G���[�\��
			Affinef af2; pose.ToAffine(af2);
			Matrix3d mat,mat2;
			mat = af.Rot();
			mat2 = af2.Rot();
			bool flag = false;
			double filter =0.1;
			for(int i=0;i<2;i++){
				for(int j=0;j<2;j++){
					if(fabs(mat[i][j]-mat2[i][j])>filter){
						flag = true;
					}
				}
			}
			if(flag){
				DSTR<<mat<<std::endl;
				std::cout<<mat<<std::endl;
				std::cout<<mat2<<std::endl;
				std::cout<<"���~"<<filter<<"�ȏ�̌덷�~�~�~�~"<<std::endl;
			}
			//
			phSolid->SetPose(pose);
		}
		if(phSolid && grFrame && (!phJoint)){
			//�ŏ���Frame�Ɋւ��鏈��
			Affinef af = grFrame->GetWorldTransform();
			af.Orthonormalization();
			Posed pose; pose.FromAffine(af);
			phSolid->SetPose(pose);
		}
	}
}


bool FWBoneObject::AddChildObject(ObjectIf* o){
	bool rv = false;

	if (!rv) {
		PHSolidIf* obj = DCAST(PHSolidIf, o);
		if (obj) {
			phSolid = obj;
			rv = true;
		}
	}
	if (!rv) {
		GRFrameIf* obj = DCAST(GRFrameIf, o);
		if (obj) {
			if (!grFrame) {
				grFrame = obj;
				rv = true;
			} else {
				endFrame = obj;
				rv = true;
			}
		}
	}
	if (!rv) {
		PHJointIf* obj = DCAST(PHJointIf, o);
		if (obj) {
			phJoint = obj;
			rv = true;
		}
	}
	/* // ����v��Ȃ��C������D���̖ړI�Őݒu��������ōl���������� <!!>(mitake)
	if ((phJoint==NULL&&endFrame==NULL)){
		//�ŏ���Frame�Ɋւ��鏈��(Center)
		if(phSolid && grFrame){
			Affinef af = grFrame->GetTransform();
			Posed absPose; absPose.FromAffine(af);
			phSolid->SetPose(absPose);
		}
	}
	*/
	if (grFrame && endFrame && phSolid && phJoint) {
		Modify();
	}
	return rv;
}

ObjectIf* FWBoneObject::GetChildObject(size_t pos){
	bool objs[] = {phSolid!=NULL, grFrame!=NULL, phJoint!=NULL, endFrame!=NULL};
	int cnt = -1;
	int i=0;
	for (; i<4; ++i) {
		if (objs[i]) { cnt++; }
		if (cnt==pos) { break; }
	}
	if (i == 0) { return phSolid;  }
	if (i == 1) { return grFrame;  }
	if (i == 2) { return phJoint;  }
	if (i == 3) { return endFrame; }
	return NULL;
}

size_t FWBoneObject::NChildObject() const {
	bool objs[] = {phSolid!=NULL, grFrame!=NULL, phJoint!=NULL, endFrame!=NULL};
	int cnt = 0;
	for (int i=0; i<4; ++i) {
		if (objs[i]) { cnt++; }
	}
	return cnt;
}

void FWBoneObject::Modify() {
	Posed poseSock, posePlug;
	poseSock.FromAffine( grFrame->GetTransform() );
	//DSTR<<"---------------------------------"<<std::endl;
	//DSTR<<grFrame->GetName()<<std::endl;
	//DSTR<<grFrame->GetTransform()<<std::endl;


	posePlug.FromAffine( Affinef() );
	GRFrameIf* fr = grFrame;
	Affinef af = Affinef();
	while (fr->GetParent()) {
		af = fr->GetTransform() * af;
		fr = fr->GetParent();
	}
	Posed absPose; absPose.FromAffine(af);

	PHBallJointIf *bj = phJoint->Cast();
	if (bj) {
		PHBallJointDesc d; bj->GetDesc(&d);
		sockOffset = d.poseSocket;
		d.poseSocket = poseSock * d.poseSocket; d.posePlug = posePlug * d.posePlug;
		d.poseSocket.Ori().unitize();
		d.posePlug.Ori().unitize();
		bj->SetDesc(&d);
	}
	PHHingeJointIf *hj = phJoint->Cast();
	if (hj) {
		PHHingeJointDesc d; hj->GetDesc(&d);
		sockOffset = d.poseSocket;
		d.poseSocket = poseSock * d.poseSocket; d.posePlug = posePlug * d.posePlug;
		d.poseSocket.Ori().unitize();
		d.posePlug.Ori().unitize();
		hj->SetDesc(&d);
	}

	double boneLength = endFrame->GetTransform().Trn().norm();

	for (int i=0; i<phSolid->NShape(); ++i) {
		CDRoundConeIf* rc = phSolid->GetShape(i)->Cast();
		if (rc) {
			CDRoundConeDesc rd;
			rc->GetDesc(&rd);
			rd.length = (float)boneLength;
			rc->SetDesc(&rd);

			Posed pose;
			pose.Pos() = Vec3d(0,0,-boneLength/2.0);
			phSolid->SetShapePose(i, pose);
			phSolid->SetCenterOfMass(Vec3d(0,0,-boneLength/2.0));
			phSolid->SetPose(absPose);
		}
	}
}

/// --- --- --- --- --- --- --- --- --- ---
//FWStructure(Bone�̏W��)

FWStructure::FWStructure(const FWStructureDesc& d/*=FWStructureDesc()*/)
: desc(d)
{
}

FWBoneObjectIf* FWStructure::GetBone(int n){
	if(n < (int)fwBones.size()){
		return fwBones[n];
	}
	DSTR<<"EROOR: FWBone don't exit"<<std::endl;
	return NULL;
}

FWBoneObjectIf* FWStructure::GetBone(const char* n){
	std::string name = n;
	for(int i=0; i < (int)fwBones.size(); i++){
		std::string getName = fwBones[i]->GetName();
		if(getName==n){
			return fwBones[i];
		}		
	}
	DSTR<<"EROOR: FWBone don't exit"<<std::endl;
	return NULL;
}

int FWStructure::GetBoneSize(){
	return (int)fwBones.size();
}

void FWStructure::AddBone(FWBoneObjectIf* o){
	fwBones.push_back(o);
}

bool FWStructure::AddChildObject(ObjectIf* o){
	bool rv = false;
	if (!rv) {
		FWBoneObjectIf* obj = DCAST(FWBoneObjectIf, o);
		if (obj) {
			if(fwBones.size()==0){
				DCAST(FWSceneIf,GetScene())->CreateFWStructure();
			}
			AddBone(obj);
			//FWScene��FWStructure�ɕۑ�
			DCAST(FWSceneIf,GetScene())->GetFWStructure()->AddBone(obj);
			rv = true;
		}
	}
	return rv;
}

void FWStructure::SetPose(Posed p){
	for(int i =0; i < GetBoneSize(); i++){
		if(!(GetBone(i)->GetPHSolid())) continue;
		Posed pose = GetBone(i)->GetPHSolid()->GetPose();
		GetBone(i)->GetPHSolid()->SetPose(p * pose);
	}
}




}
