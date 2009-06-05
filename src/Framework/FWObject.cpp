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
	solidLength=0;
}

void FWObject::Sync(){
	if (phSolid && grFrame){
		if(solidLength){
			//�{�[���t��X�t�@�C�����g�p����ꍇ
			Affinef af,afParent,afd,afl,AF;
			phSolid->GetPose().ToAffine(af);	
			afParent=grFrame->GetParent()->GetWorldTransform();	//�e��World���W����݂�Frame���擾
			afd=afParent.inv()*af;								//�K�w�\������Affin�s��ɕϊ�����
			afl.PosZ()+=(float)solidLength/2.0f;							//���̒��S�̈ʒu���獄�̂̔������������炵�C�W���C���g�����̈ʒu�ɂ���
			AF=afd*afl;
			DCAST(GRFrame, grFrame)->SetTransform(AF);
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
