/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include "FWObject.h"
#include <Graphics/GRFrame.h>

#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

namespace Spr{;

FWObject::FWObject(const FWObjectDesc& d/*=FWObjectDesc()*/)
: desc(d), phSolid(NULL), grFrame(NULL)
{
}

void FWObject::Sync(){
	if (phSolid && grFrame){
		if(solidLength){
			//�{�[���t��X�t�@�C�����g�p����ꍇ
			if(bonePositionFlag==1){
				//�{�[���P�@�i���s�s��݂̂̃A�t�B���s��X�V�j
				Affinef af;
				Matrix3d matrix;
				phSolid->GetPose().ToAffine(af);
				af.Rot()=matrix;									//��]�����̏�����
				//af.PosZ()-=solidLength/2;							//���̒��S�̈ʒu���獄�̂̔������������炵�C�W���C���g�����̈ʒu�ɂ���
				DCAST(GRFrame, grFrame)->SetTransform(af);
			}else if(bonePositionFlag==2){
				//�{�[���Q�@�i��]�s��݂̂̃A�t�B���s��X�V�j
				Affinef af,afParent,afd,afl,AF;
				phSolid->GetPose().ToAffine(af);
				afParent=grFrame->GetParent()->GetWorldTransform();
					/*DSTR<<"phSolid"<<std::endl<<af<<std::endl;
					DSTR<<"afParent"<<std::endl<<afParent<<std::endl;*/
				afd=afParent.inv()*af;
					DSTR<<"afd"<<std::endl<<afd<<std::endl;
				afl.PosZ()+=solidLength/2;							//���̒��S�̈ʒu���獄�̂̔������������炵�C�W���C���g�����̈ʒu�ɂ���
				AF=afd*afl;
					//DSTR<<"solidLength"<<std::endl<<solidLength/2<<std::endl;
					//DSTR<<"afdz"<<std::endl<<afd<<std::endl;
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
			}
		}else{
			Affinef af;
			phSolid->GetPose().ToAffine(af);
			DCAST(GRFrame, grFrame)->SetTransform(af);
		}
	}else{
		//DSTR << "Warning: No solid or frame for " << GetName() << ":FWObject." << std::endl;
	}
}


bool FWObject::AddChildObject(ObjectIf* o){
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
			grFrame = obj;
			rv = true;
		}
	}
	return rv;
}

}
