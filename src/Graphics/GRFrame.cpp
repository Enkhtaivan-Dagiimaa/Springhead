/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include "Graphics.h"
#include "GRFrame.h"
#include "GRMesh.h"

namespace Spr{;

//-----------------------------------------------------------------
//	GRFrame
//
GRFrame::GRFrame(const GRFrameDesc& desc):GRFrameDesc(desc){
	parent = NULL;

}

void GRFrame::Render(GRRenderIf* r){
	r->PushModelMatrix();
	r->MultModelMatrix(transform);
	for(GRVisualIfs::iterator it = children.begin(); it != children.end(); ++it){
		(*it)->Render(r);
	}
	for(GRVisualIfs::reverse_iterator it = children.rbegin(); it != children.rend(); ++it){
		(*it)->Rendered(r);
	}
	r->PopModelMatrix();
}
void GRFrame::Rendered(GRRenderIf* r){
}
void GRFrame::SetParent(GRFrameIf* fr){
	if((GRFrameIf*)(parent->Cast()) == fr) return;
	UTRef<GRFrame> tmp = this->Cast();	// delete�΍�
	if (fr){
		fr->AddChildObject(this->Cast());
	}
}
bool GRFrame::AddChildObject(ObjectIf* o){
	GRVisual* v = o->Cast();
	if (v){
		v->SetNameManager(GetNameManager());
		children.push_back(v->Cast());
		GRFrame*	frame	= DCAST(GRFrame, v);
		GRMaterial* mat		= DCAST(GRMaterial, v);
		GRLight*	light	= DCAST(GRLight, v);
		GRMesh*		mesh	= DCAST(GRMesh, v);
		if (frame && frame->parent != this){
			//	�����Ō��̎����傩��폜����̂͂�肷���ł́H	by tazaki ?
			//	 -> ����AFrame �� parent���ЂƂȂ̂ŁA�����̃t���[���̎q�ɂȂ�̂͂��߂ł��B by hase
			if (frame->parent) frame->parent->DelChildObject(frame->Cast());
			frame->parent = this;
		}
		// �f�t�H���g�l�[���ݒ�
		if(strcmp(v->GetName(), "") == 0){
			char name[256]="";
			if(frame)
				sprintf(name, "%s_frame%d", GetName(), children.size()-1);
			if(mat)
				sprintf(name, "%s_mat%d", GetName(), children.size()-1);
			if(light)
				sprintf(name, "%s_light%d", GetName(), children.size()-1);
			if(mesh)
				sprintf(name, "%s_mesh%d", GetName(), children.size()-1);
			v->SetName(name);
		}
		return true;
	}
	return false;
}
bool GRFrame::DelChildObject(ObjectIf* v){
	for(GRVisualIfs::iterator it = children.begin(); it != children.end(); ++it){
		if (*it == v){
			children.erase(it);
			return true;
		}
	}
	return false;
}
size_t GRFrame::NChildObject() const {
	return children.size();
}
ObjectIf* GRFrame::GetChildObject(size_t pos){
	return children[pos]->Cast();
}

void GRFrame::Print(std::ostream& os) const {
	GRVisual::PrintHeader(os, false);
	os.width(os.width()+2);
	os << transform;
	os.width(os.width()-2);
	GRVisual::PrintChildren(os);
	GRVisual::PrintFooter(os);
}

void GRFrame::AddRBFKeyFrame(PTM::VVector<float> pos){
	Affinef aff = GetTransform();
	// std::cout << "Aff : " << aff << std::endl;

	// �L�[�t���[�����W��ǉ�
	kfPositions.push_back(pos);

	// �����̎擾
	int nKeys	= kfPositions.size();
	int nKeyDim	= pos.size();
	int nMatDim	= nKeys + 1 + nKeyDim;

	// �ڕW�l�x�N�g���̎����E�l�ݒ�ƌW���x�N�g���̎����ݒ�
	for (int i=0; i<4; ++i) {
		for (int j=0; j<4; ++j) {
			PTM::VVector<float> p; p.resize(kfAffines[i][j].size()); p = kfAffines[i][j];
			kfAffines[i][j].resize(nMatDim);
			for (unsigned q=0; q<p.size(); ++q) {
				kfAffines[i][j][q] = p[q];
			}
			kfAffines[i][j][nKeys-1] = aff[i][j];
			for (int k=nKeys+1; k<nMatDim; ++k) {
				kfAffines[i][j][k] = 0;
			}
			kfCoeffs[i][j].resize(nMatDim);
		}
	}

	// �v�Z�p�s��̗p��
	PTM::VMatrixCol<float> A;	A.resize(nMatDim, nMatDim);
	/// ����
	for (int i=0; i<nKeys; ++i) {
		for (int j=i; j<nKeys; ++j) {
			A[i][j] = A[j][i] = (kfPositions[i] - kfPositions[j]).norm();
		}
	}
	/// �E�㒆�E������
	for (int i=nKeys; i<nKeys+1; ++i) {
		for (int j=0; j<nKeys; ++j) {
			A[i][j] = A[j][i] = 1;
		}
	}
	/// �E��E����
	for (int i=nKeys+1; i<nMatDim; ++i) {
		for (int j=0; j<nKeys; ++j) {
			int m = i - (nKeys+1);
			A[i][j] = A[j][i] = kfPositions[j][m];
		}
	}
	/// �E��
	for (int i=nKeys; i<nMatDim; ++i) {
		for (int j=nKeys; j<nMatDim; ++j) {
			A[i][j] = 0;
		}
	}

	// �t�s��v�Z
	A = A.inv();

	// �W���x�N�g���̌v�Z
	for (int i=0; i<4; ++i) {
		for (int j=0; j<4; ++j) {
			kfCoeffs[i][j] = A * kfAffines[i][j];
			/*
			std::cout << "A : " << A << std::endl;
			std::cout << "kAf : " << kfAffines[i][j] << std::endl;
			std::cout << "kfC : " << kfCoeffs[i][j] << std::endl;
			*/
		}
	}

	// �q�v�f�ɍċA
	for (unsigned i=0; i<children.size(); ++i) {
		GRFrame* fr = children[i]->Cast();
		if (fr) {
			fr->AddRBFKeyFrame(pos);
		}
	}
}

void GRFrame::BlendRBF(PTM::VVector<float> pos){
	Affinef aff;

	// �����̎擾
	int nKeys	= kfPositions.size();
	int nKeyDim	= pos.size();
	int nMatDim	= nKeys + 1 + nKeyDim;

	// �v�Z�p�x�N�g���̗p��
	PTM::VVector<float> input; input.resize(nMatDim);
	/// ��
	for (int i=0; i<nKeys; ++i) {
		input[i] = (kfPositions[i] - pos).norm();
	}
	/// ��
	for (int i=nKeys; i<nKeys+1; ++i) {
		input[i] = 1;
	}
	/// �E
	for (int i=nKeys+1; i<nMatDim; ++i) {
		int m = i - (nKeys+1);
		input[i] = pos[m];
	}

	// �u�����h�l�̌v�Z
	// std::cout << "inp : " << input << std::endl;
	for (int i=0; i<4; ++i) {
		for (int j=0; j<4; ++j) {
			aff[i][j] = PTM::dot(kfCoeffs[i][j], input);
			// std::cout << "kfC : " << kfCoeffs[i][j] << std::endl;
		}
	}

	// ���K������
	aff.Ex() /= aff.Ex().norm();
	aff.Ey() /= aff.Ey().norm();
	aff.Ez() /= aff.Ez().norm();
	for (int i=0; i<10; ++i) {
		Vec3f u = PTM::cross(aff.Ey(), aff.Ez());
		Vec3f v = PTM::cross(aff.Ez(), aff.Ex());
		Vec3f w = PTM::cross(aff.Ex(), aff.Ey());
		aff.Ex() = (aff.Ex()+u)/2.0f;
		aff.Ey() = (aff.Ey()+v)/2.0f;
		aff.Ez() = (aff.Ez()+w)/2.0f;
		aff.Ex() /= aff.Ex().norm();
		aff.Ey() /= aff.Ey().norm();
		aff.Ez() /= aff.Ez().norm();
		float r = 0.0f;
		r += PTM::dot(aff.Ex(), aff.Ey())*PTM::dot(aff.Ex(), aff.Ey());
		r += PTM::dot(aff.Ey(), aff.Ez())*PTM::dot(aff.Ey(), aff.Ez());
		r += PTM::dot(aff.Ez(), aff.Ex())*PTM::dot(aff.Ez(), aff.Ex());
		if (r < 0.000001) {
			break;
		}
	}

	// �ϊ��s��̃Z�b�g
	SetTransform(aff);
	// std::cout << this->GetName() << std::endl;
	// std::cout << "Aff : " << aff << std::endl;

	// �q�v�f�ɍċA
	for (unsigned  i=0; i<children.size(); ++i) {
		GRFrame* fr = children[i]->Cast();
		if (fr) {
			fr->BlendRBF(pos);
		}
	}
}


//-----------------------------------------------------------------
//	GRDummyFrame
//
GRDummyFrame::GRDummyFrame(const GRDummyFrameDesc& desc):GRDummyFrameDesc(desc){	
}

bool GRDummyFrame::AddChildObject(ObjectIf* o){
	GRVisualIf* v = o->Cast();
	if (v){
		children.push_back(v);
		return true;
	}
	return false;
}
bool GRDummyFrame::DelChildObject(ObjectIf* v){
	for(GRVisualIfs::iterator it = children.begin(); it != children.end(); ++it){
		if (*it == v){
			children.erase(it);
			return true;
		}
	}
	return false;
}
size_t GRDummyFrame::NChildObject() const{ return children.size(); }
ObjectIf* GRDummyFrame::GetChildObject(size_t pos){
	if (pos >= children.size()) return NULL; 
	return children[pos];
}

//-----------------------------------------------------------------
//	GRAnimation
//
void GRAnimation::BlendPose(float time, float weight){
	BlendPose(time, weight, false);
}
void GRAnimation::BlendPose(float time, float weight, bool add){
	//	�^�[�Q�b�g�ɕϊ���������
	Affinef transform;
	for(std::vector<GRAnimationKey>::iterator it = keys.begin(); it != keys.end(); ++it){
		GRAnimationKey& anim = *it;
		//	�����ŃL�[������
		for(unsigned i=0; i < anim.keys.size(); ++i){
			float blended[16];
			if (anim.keys[i].time > time){	//	���������̂Ńu�����h�����ϊ����v�Z
				if (i==0){	//	i=0�������Z�b�g
					for(unsigned v=0; v<anim.keys[i].values.size(); ++v){
						blended[v] = anim.keys[i].values[v];
					}
				}else{		//	i-1��i���u�����h
					float k = (anim.keys[i].time - time) / 
							(anim.keys[i].time - anim.keys[i-1].time);
					for(unsigned v=0; v<anim.keys[i].values.size(); ++v){
						blended[v] = (1-k) * anim.keys[i].values[v]
							+ k * anim.keys[i-1].values[v];
					}						
				}
				//	���������ϊ���transform�ɓK�p
				switch(anim.keyType){
					case GRAnimationDesc::ROTATION:{
						Affinef mat;
						//((Quaternionf*)blended)->ToMatrix(mat);
						Quaternionf q = *((Quaternionf*) blended);
						q.w*=-1;
						q.ToMatrix(mat.Rot());
						transform = mat * transform;
						}break;
					case GRAnimationDesc::SCALE:
						transform.Ex() *= blended[0];
						transform.Ey() *= blended[1];
						transform.Ez() *= blended[2];
						break;
					case GRAnimationDesc::POSITION:
						transform.Pos()[0] += blended[0];
						transform.Pos()[1] += blended[1];
						transform.Pos()[2] += blended[2];
						break;
					case GRAnimationDesc::MATRIX:
//						transform =  *((Affinef*)blended) * transform ;
						break;
				}
				break;
			}
		}
	}
	//	transform ���^�[�Q�b�g�ɓK�p
	for(Targets::iterator it = targets.begin(); it!= targets.end(); ++it){
		it->target->SetTransform(transform);
	}
}
void GRAnimation::ResetPose(){
	//	transform �������l�ɖ߂�
	for(Targets::iterator it = targets.begin(); it!= targets.end(); ++it){
		it->target->SetTransform(it->initalTransform);
	}
}
void GRAnimation::LoadInitialPose(){
	//	�t���[���̕ϊ��ɏ����l��ݒ肷��
	for(Targets::iterator it = targets.begin(); it!= targets.end(); ++it){
		it->initalTransform = it->target->GetTransform();
	}
}
bool GRAnimation::AddChildObject(ObjectIf* o){
	GRFrame* fr = o->Cast();
	if (fr){
		targets.push_back(Target());
		targets.back().target = fr->Cast();
		targets.back().initalTransform = fr->GetTransform();
		return true;
	}
	return false;
}

//-----------------------------------------------------------------
//	GRAnimationSet
//
bool GRAnimationSet::AddChildObject(ObjectIf* o){
	GRAnimation* ani = o->Cast();
	if (ani){
		animations.push_back(ani);
		return true;
	}
	return false;
}

bool GRAnimationSet::DelChildObject(ObjectIf* o){
	GRAnimation* ani = o->Cast();
	if (ani){
		for(Animations::iterator it = animations.begin(); it != animations.end(); ++it){
			if(ani == *it){
				animations.erase(it);
				return true;
			}
		}
	}
	return false;
}
int GRAnimationSet::NChildObject(){
	return animations.size();
}


ObjectIf* GRAnimationSet::GetChildObject(size_t p){
	return animations[p]->Cast();
}
void GRAnimationSet::BlendPose(float time, float weight){
	BlendPose(time, weight, false);
}
void GRAnimationSet::BlendPose(float time, float weight, bool add){
	for (Animations::iterator it = animations.begin(); it != animations.end(); ++it){
		(*it)->BlendPose(time, weight, add);
	}
}
void GRAnimationSet::ResetPose(){
	for (Animations::iterator it = animations.begin(); it != animations.end(); ++it){
		(*it)->ResetPose();
	}	
}
void GRAnimationSet::LoadInitialPose(){
	for (Animations::iterator it = animations.begin(); it != animations.end(); ++it){
		(*it)->LoadInitialPose();
	}
}


bool GRAnimationController::AddChildObject(ObjectIf* o){
	GRAnimationSet* ani = o->Cast();
	if (ani){
		sets.insert(std::make_pair(ani->GetName(), ani));
		return true;
	}
	return false;
}

bool GRAnimationController::DelChildObject(ObjectIf* o){
	GRAnimationSet* ani = o->Cast();
	if (ani){
		Sets::iterator it = sets.find(ani->GetName());
		if (it->second == ani){
			sets.erase(it);
			return true;
		}
	}
	return false;
}
int GRAnimationController::NChildObject(){
	return sets.size();
}

ObjectIf* GRAnimationController::GetChildObject(size_t p){
	Sets::iterator it = sets.begin();
	for(unsigned i=0; i<p; ++i) ++it;
	return it->second->Cast();
}
void GRAnimationController::BlendPose(UTString name, float time, float weight){
	BlendPose(name, time, weight, false);
}
void GRAnimationController::BlendPose(UTString name, float time, float weight, bool add){
	Sets::iterator it = sets.find(name);
	if (it != sets.end()){
		it->second->BlendPose(time, weight, add);
	}
}
void GRAnimationController::ResetPose(){
	for(Sets::iterator it = sets.begin(); it != sets.end(); ++it){
		it->second->ResetPose();
	}
}
void GRAnimationController::LoadInitialPose(){
	for(Sets::iterator it = sets.begin(); it != sets.end(); ++it){
		it->second->LoadInitialPose();
	}
}


}
