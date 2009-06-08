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
	K				= 10;
	D1				= 2;
	D2				= 0.1;
	yieldStress		= 0.1;
	hardnessRate	= 1e3;
	SocketPos		=Vec3f(0.0,0.0,0.0);
	PlugPos			=Vec3f(0.0,0.0,0.0);
}

FWBone::FWBone(const FWBoneDesc& d/*=FWObjectDesc()*/)
: desc(d)
{
}
void FWBone::SetJointKDD2(double K,double D, double D2){
	PH3ElementBallJointIf* ball=joint->Cast();
	ball->SetSpring(K);
	ball->SetDamper(D);
	ball->SetSecondDamper(D2);
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
	FWSkinMeshAdapt();	//FWObject�ɑ�����C�X�L�����b�V����K������
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
				bone.back()->shapeBone				= BoneShapeCone(frame1,frame2);
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
/*2��grFrame����shape(���E���h�R�[���j���쐬*/
CDRoundConeIf* FWBoneCreate::BoneShapeCone(GRFrameIf* frame1,GRFrameIf* frame2){
	double wide=0.5;
	double lengthRate=1;
	double length=BoneLength(frame1,frame2);
	CDRoundConeDesc desc;
	desc.length=((float)length-(float)wide*2.0f)*((float)lengthRate);
	desc.radius=Vec2f(wide,wide);
	shapeBone.push_back(XCAST(fwSdk->GetPHSdk()->CreateShape(desc)));
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
		desc.mass = 0.0005;
		desc.inertia = 0.033 * Matrix3d::Unit();
		soBone.push_back(phScene->CreateSolid(desc));
		bone[i]->phSolid=soBone[i];
		{
			if(i==0){
				bone[i]->phSolid->SetDynamical(true);
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
		for(size_t i=0 ;i<bone.size(); ++i){
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
				joint.push_back( phSceneIf->CreateJoint(bone[i]->parentBone->phSolid, bone[i]->phSolid, d3Ball));
				bone[i]->joint=joint.back();
			}
		}
	}
}
/*�ڐG����̐ݒ�(�׍������̂̐ڐG��؂�)*/
void FWBoneCreate::ContactCanceler(){
	if (bone.size()){
		for(size_t i=0 ;i<bone.size(); ++i){
			if(!(bone[i]->parentBone==NULL)){
				//�e�q�̐ڐG��؂�
				phSceneIf->SetContactMode(bone[i]->parentBone->phSolid, bone[i]->phSolid, PHSceneDesc::MODE_NONE);
				//�����e�������̂̐ڐG��؂�
				for(size_t j=0 ;j<bone.size(); ++j){
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
	for(size_t i=0; i<bone.size(); ++i){
		//�{�[���P�̐ݒ������i���s�s��݂̂̃A�t�B���s��X�V�j
		fwObject.push_back(fwSdk->GetScene()->CreateFWObject());
		fwObject[i]->SetPHSolid(bone[i]->phSolid);
		fwObject[i]->SetGRFrame(bone[i]->grFrame);
		fwObject[i]->SetSolidLength(bone[i]->length);
		fwSdk->GetScene()->SetFWBones(DCAST(FWBoneIf,bone[i]));
	}
}

}
