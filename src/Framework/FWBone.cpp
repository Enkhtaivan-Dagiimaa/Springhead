/*  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include "FWBone.h"
#include <Graphics/GRFrame.h>

#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

using namespace std;
namespace Spr{;

BoneJoint::BoneJoint()
{
	K				= 0.01;
	D1				= 0.1;
	D2				= 10;
	yieldStress		= 0.1;
	hardnessRate	= 1e3;
	SocketPos		=Vec3f(0.0,0.0,0.0);
	PlugPos			=Vec3f(0.0,0.0,0.0);
}

FWBone::FWBone()
{

}

void FWBone::Sync(){
	if (phSolid && grFrame){
		if(solidLength){
			//�{�[���t��X�t�@�C�����g�p����ꍇ
			if(bonePositionFlag==1){
				//�{�[���P�@�i���s�s��݂̂̃A�t�B���s��X�V�j
				Affinef af;
				Matrix3d matrix;
				phSolid->GetPose().ToAffine(af);
				af.Rot()=matrix;									//��]�����̏�����
				DCAST(GRFrame, grFrame)->SetTransform(af);
			}else if(bonePositionFlag==2){
				//�{�[���Q�@�i��]�s��݂̂̃A�t�B���s��X�V�j
				Affinef af,afParent,afd,afl,AF;
				phSolid->GetPose().ToAffine(af);
				afParent=grFrame->GetParent()->GetWorldTransform();
				afd=afParent.inv()*af;
				afl.PosZ()+=solidLength/2;							//���̒��S�̈ʒu���獄�̂̔������������炵�C�W���C���g�����̈ʒu�ɂ���
				AF=afd*afl;
				DCAST(GRFrame, grFrame)->SetTransform(AF);
			}else if(bonePositionFlag==3){
				//�{�[���R
				Affinef af,afParent,afd,afl,AF;
				phSolid->GetPose().ToAffine(af);
				afParent=grFrame->GetParent()->GetWorldTransform();
				afd=afParent.inv()*af;
				afl.PosZ()+=solidLength/2;							//���̒��S�̈ʒu���獄�̂̔������������炵�C�W���C���g�����̈ʒu�ɂ���
				AF=afd*afl;
				DCAST(GRFrame, grFrame)->SetTransform(AF);
			}else if(bonePositionFlag==0){
				//�{�[���P�C�Q�ȊO�̃{�[���i�A�t�B���s��̍X�V�j
				Affinef af,afParent,afd,afl,AF;
				phSolid->GetPose().ToAffine(af);	
				afParent=grFrame->GetParent()->GetWorldTransform();	//�e��World���W����݂�Frame���擾
				afd=afParent.inv()*af;								//�K�w�\������Affin�s��ɕϊ�����
				afl.PosZ()+=solidLength/2;							//���̒��S�̈ʒu���獄�̂̔������������炵�C�W���C���g�����̈ʒu�ɂ���
				AF=afd*afl;
				DCAST(GRFrame, grFrame)->SetTransform(AF);
			}else{
				DSTR << "Warning: No solid or frame for " << GetName() << ":FWObject." << std::endl;
			}
		}
	}
}

//Boot���Ăׂ΂��ׂĂ̏����������ōs����-------------------------------------------------------
/*NodeHandler��BoneCreate���Ă�Ń{�[���쐬���K��������*/
void FWBoneCreate::Boot(GRMesh* mesh, PHScene* phScene){
	/*Mesh��PHScene���ŏ��ɐݒ�,�ȍ~�̊֐��͑������Ă��鎖��O��Ƃ���*/
	SetMesh(mesh);
	SetPHScene(phScene);
	//FWSdk�̎擾
	fwSdk = phScene->GetSdk()->GetNameManager()->Cast();
	//phSceneIf�̎擾
	phSceneIf =phScene->Cast();

	SetFWBone();		//X�t�@�C���̏���FWBone�f�[�^�\���ɕϊ�
	SetBoneJoint();		//FWBone�̃f�[�^����e�ɑ΂���W���C���g�̃v���O�C�\�P�b�g�̈ʒu����FWBone�ɑ��
	GenerateBone();		//�V�[���ɍ��̂��쐬
	FWJointCreate();	//�V�[���ɃW���C���g���쐬
	ContactCanceler();	//�A�Ȃ鍄�̂̐ڐG��؂�
}

//X�t�@�C���̏���FWBone�f�[�^�\���ɕϊ�--------------------------------------------------------
/*FWBone���쐬�����l����*/
void FWBoneCreate::SetFWBone(){
	if (mesh){
		for(unsigned int i=0 ;i<mesh->skinWeights.size()-1; ++i){
			GRFrameIf* frame1 =mesh->skinWeights[i].frame->Cast();
			GRFrameIf* frame2 =mesh->skinWeights[i+1].frame->Cast();
			if(BoneDetector(frame1,frame2)==true){
				bone_= new FWBone;
				bone.push_back(bone_);
				bone.back()->centerPoint			= BonePosition(frame1,frame2);
				bone.back()->length					= BoneLength(frame1,frame2);
				bone.back()->shapeBone				= BoneShape(frame1,frame2);
				bone.back()->parentBone				= ParentBone(frame1);
				bone.back()->grFrame				= frame1;
				bone.back()->worldTransformAffine	= frame1->GetWorldTransform();
			}
		}
	}
}

/*2��grFrame���Ȃ����Ă��邩�ǂ��������o���CFWBone���쐬����ꏊ��T��*/
bool FWBoneCreate::BoneDetector(GRFrameIf* frame1,GRFrameIf* frame2){
	bool Flag=false;
	std::string name1		= frame1->GetName();
	std::string name2Parent	= frame2->GetParent()->GetName();
	if(!(name1=="Root")){//Root�͍쐬���Ȃ�
		if(name1==name2Parent){//frame1��frame2���Ȃ����Ă��鎞true
			Flag=true;
		}
	}
	return Flag;
}
/*2��grFrame���璆�Ԓn�_���Z�o*/
Vec3d FWBoneCreate::BonePosition(GRFrameIf* frame1,GRFrameIf* frame2){
	Vec3d point1	= frame1->GetWorldTransform()*Vec3d(0.0,0.0,0.0);
	Vec3d point2	= frame2->GetWorldTransform()*Vec3d(0.0,0.0,0.0);
	return (point1+point2)*0.5;
}
/*2��grFrame���狗�����쐬*/
double FWBoneCreate::BoneLength(GRFrameIf* frame1,GRFrameIf* frame2){
	Vec3d point1= frame1->GetWorldTransform()*Vec3d(0.0,0.0,0.0);
	Vec3d point2= frame2->GetWorldTransform()*Vec3d(0.0,0.0,0.0);
	Vec3d length= point2-point1;
	return length.norm();
}
/*2��grFrame����shape���쐬*/
CDBoxIf* FWBoneCreate::BoneShape(GRFrameIf* frame1,GRFrameIf* frame2){
	double wide=0.5;
	double length=BoneLength(frame1,frame2);
	CDBoxDesc dBox;
	dBox.boxsize=Vec3d(wide,wide,length);
	shapeBone.push_back(XCAST(fwSdk->GetPHSdk()->CreateShape(dBox)));
	return shapeBone[shapeBone.size()-1];
}
/*�ebone��ݒ�*/
FWBone* FWBoneCreate::ParentBone(GRFrameIf* frame1){
	FWBone* parentBone=NULL;
	std::string parentName=frame1->GetParent()->GetName();
	for(unsigned int i=0 ;i<bone.size()-1; ++i){
		DSTR<<bone[bone.size()-2-i]->grFrame->GetName()<<endl;
		if(parentName==bone[bone.size()-2-i]->grFrame->GetName()){
			parentBone=bone[bone.size()-2-i];
			break;
		}
	}
	return parentBone;
}
/*�ebone�ɑ΂���BoneJoint�f�[�^��ݒ�*/
void FWBoneCreate::SetBoneJoint(){
	if (bone.size()){
		for(unsigned int i=0 ;i<bone.size(); ++i){
			if(!(bone[i]->parentBone==NULL)){
				bone[i]->jointData.SocketPos=Vec3f(0.0,0.0, -bone[i]->parentBone->length/2);
				bone[i]->jointData.PlugPos=Vec3f(0.0,0.0, bone[i]->length/2);
			}
		}
	}
}
//�{�[���̍쐬-----------------------------------------------------------------------------------
/*FWBone�̏����g���ăV�[�����쐬*/
void FWBoneCreate::GenerateBone(){
	for(unsigned int i=0; i<bone.size(); ++i){
		//soBone�̍쐬
		PHSolidDesc	desc;
		desc.mass = 0.05;
		desc.inertia = 0.033 * Matrix3d::Unit();
		soBone.push_back(phScene->CreateSolid(desc));
		bone[i]->phSolid=soBone[i];
		{
			if(i==0){
				bone[i]->phSolid->SetDynamical(false);
			}else{
				bone[i]->phSolid->SetDynamical(true);
			}
			//�{�[���̏����ʒu��ݒ�
			Posed pose,pose2;
			pose.PosZ()=-bone[i]->length/2;
			pose2.FromAffine(bone[i]->worldTransformAffine);
			pose=pose2*pose;
			bone[i]->phSolid->SetPose(pose);
		}
		//shapeBone�̍쐬
		if(i>0){
			bone[i]->phSolid->AddShape(bone[i]->shapeBone);
		}
	}
}
/*�W���C���g���쐬*/
void FWBoneCreate::FWJointCreate(){
	if (bone.size()){
		for(int i=0 ;i<bone.size(); ++i){
			if(!(bone[i]->parentBone==NULL)){
				PH3ElementBallJointDesc d3Ball;
				{
					d3Ball.poseSocket.Pos()	= bone[i]->jointData.SocketPos;
					d3Ball.posePlug.Pos()	= bone[i]->jointData.PlugPos;
					d3Ball.spring			= bone[i]->jointData.K;
					d3Ball.damper			= bone[i]->jointData.D1;
					d3Ball.secondDamper		= bone[i]->jointData.D2;
					d3Ball.yieldStress		= bone[i]->jointData.yieldStress;
					d3Ball.hardnessRate		= bone[i]->jointData.hardnessRate;
				}
				phSceneIf->SetContactMode(bone[i]->parentBone->phSolid, bone[i]->phSolid, PHSceneDesc::MODE_NONE);
				joint.push_back( phSceneIf->CreateJoint(bone[i]->parentBone->phSolid, bone[i]->phSolid, d3Ball));
				bone[i]->joint=joint.back();
			}
		}
	}
}
/*�ڐG����̐ݒ�(�׍������̂̐ڐG��؂�)*/
void FWBoneCreate::ContactCanceler(){
	if (bone.size()){
		for(int i=0 ;i<bone.size(); ++i){
			if(!(bone[i]->parentBone==NULL)){
				//�e�q�̐ڐG��؂�
				phSceneIf->SetContactMode(bone[i]->parentBone->phSolid, bone[i]->phSolid, PHSceneDesc::MODE_NONE);
				//�����e�������̂̐ڐG��؂�
				for(int j=0 ;j<bone.size(); ++j){
					if(!(bone[j]->parentBone==NULL)){
						if(!(j==i)){
							if(bone[i]->parentBone->grFrame->GetName()==bone[j]->parentBone->grFrame->GetName()){
								phSceneIf->SetContactMode(bone[i]->phSolid, bone[j]->phSolid, PHSceneDesc::MODE_NONE);
							}
						}
					}
				}
			}
		}
	}
}


void FWBoneCreate::FWSkinMeshAdapt(){

	//for(int i=0; i<bone.size(); ++i){
	//	//�{�[���P�̐ݒ������i���s�s��݂̂̃A�t�B���s��X�V�j
	//	if(i==0){
	//		fwoAncestorBone.push_back(fwSdk->GetScene()->CreateFWObject());
	//		ancestorBone[0].fwObject	= fwoAncestorBone[0];
	//		ancestorBone[0].grFrame		= grfBone[0];
	//		ancestorBone[0].length		= bone[0].length;

	//		ancestorBone[0].fwObject->SetPHSolid(ancestorBone[0].solid);
	//		ancestorBone[0].fwObject->SetbonePositionFlag(1);
	//		ancestorBone[0].fwObject->SetSolidLength(ancestorBone[0].length);
	//		ancestorBone[0].fwObject->SetGRFrame(ancestorBone[0].grFrame->Cast());
	//	}
	//	fwoBone.push_back(fwSdk->GetScene()->CreateFWObject());

	//	bone[i].fwObject=fwoBone[i];
	//	bone[i].grFrame=grfBone[i+1];
	//	//PH��GR��A��������
	//	bone[i].fwObject->SetPHSolid(bone[i].solid);
	//	if(i==0){
	//		//�{�[���Q�̐ݒ������i��]�s��݂̂̃A�t�B���s��X�V�j
	//		bone[i].fwObject->SetbonePositionFlag(2);
	//		bone[i].fwObject->SetSolidLength(bone[i].length);
	//	}else if(i==1){
	//		//�{�[���R�̐ݒ�i�A�t�B���s��j
	//		bone[i].fwObject->SetbonePositionFlag(3);
	//		bone[i].fwObject->SetSolidLength(bone[i].length);
	//	}else{
	//		//�{�[���P�C�Q�ȊO�̃{�[���̐ݒ�i�A�t�B���s��j
	//		bone[i].fwObject->SetbonePositionFlag(0);
	//		bone[i].fwObject->SetSolidLength(bone[i].length);
	//	}
	//	bone[i].fwObject->SetGRFrame(bone[i].grFrame->Cast());

	//	Affinef afs;
	//	bone[i].solid->GetPose().ToAffine(afs);
	//	bone[i].grFrame->GetObjectIf()->Print(std::cout);
	//}
}

void FWBoneCreate::DisplayBonePoint(){
	//CDSphereDesc dSphere;
	//CDSphereIf* shapeSphere;
	//std::vector<PHSolidIf*> soSphere;

	//for(int i=0; i<bonePoint.size(); ++i){
	//	//soSphere�̍쐬
	//	soSphere.push_back(phScene->CreateSolid(desc));
	//	{
	//		soSphere[i]->SetDynamical(false);
	//		soSphere[i]->SetFramePosition(bonePoint[i]);
	//	}
	//	//shapeBone�̍쐬	
	//	{
	//		dSphere.radius=0.1;
	//		shapeSphere=XCAST(fwSdk->GetPHSdk()->CreateShape(dSphere));
	//	}
	//	soSphere[i]->AddShape(shapeSphere);
	//}
}

void FWBoneCreate::DisplayPHBoneCenter(){
	//CDBoxDesc dBoxD;
	//CDBoxIf* shapeBoxD;
	//std::vector<PHSolidIf*> soBoxD;
	//for(int i=0; i<bone.size(); ++i){
	//	//soBoxD�̍쐬
	//	soBoxD.push_back(phScene->CreateSolid(desc));
	//	{
	//		soBoxD[i]->SetDynamical(false);
	//		soBoxD[i]->SetFramePosition(bone[i].centerPoint);
	//	}
	//	//shapeBoxD�̍쐬	
	//	{
	//		dBoxD.boxsize=(Vec3d(0.1,0.1,0.1));
	//		shapeBoxD=XCAST(fwSdk->GetPHSdk()->CreateShape(dBoxD));
	//	}
	//	soBoxD[i]->AddShape(shapeBoxD);
	//}
}

void FWBoneCreate::SetAffine(std::vector<Affinef> a){
	//�A�t�B���s������̂ɕۑ�
	//if (af.size()){
	//	for(int i=0 ;i<bone.size(); ++i){
	//		bone[i].transformAffine=af[i+1];
	//		//DSTR << i << std::endl << bone[i].grParentAffine << std::endl;
	//	}
	//}
}



}
