/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include <HumanInterface/HILeap.h>
#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

namespace Spr{

bool HILeap::Init(const void* desc) {
	#ifdef USE_LEAP
	// Leapmotion��������
	leap = new Leap::Controller();
	#endif
	return true;
}

void HILeap::Update(float dt) {
	#ifdef USE_LEAP
	if (leap) {
		// Leapmotion����Z���V���O���ʂ��擾
		Leap::Frame frame = leap->frame(0);

		// Skeleton�̕s������p��
		PrepareSkeleton(frame.hands().count());

		for (int h=0; h<frame.hands().count(); ++h) {
			Leap::Hand hand = frame.hands()[h];

			HISkeleton* skel = skeletons[h]->Cast();

			// ��S�̂̈ʒu�p�����Z�b�g
			skel->pose.Ori() = ToSpr(hand.basis()) * rotation;
			skel->pose.Pos() = ToSpr(hand.palmPosition()) + center;

			// �{�[��������
			skel->PrepareBone(20);

			// �e�w�̈ʒu�ƕ������Z�b�g
			int cnt = 0;
				
			for(int f=0; f<hand.fingers().count(); f++){
				for(int b = 0; b < 4; b++) {
					Leap::Bone::Type boneType = static_cast<Leap::Bone::Type>(b);
					Leap::Bone bone = hand.fingers()[f].bone(boneType);
					DCAST(HIBone,skel->bones[cnt])->position  = ToSpr(bone.center()) + center;
					DCAST(HIBone,skel->bones[cnt])->direction = ToSpr(bone.direction());
					DCAST(HIBone,skel->bones[cnt])->length    = bone.length() * scale;

					cnt++;
				}
			}
		}
	}
	#endif
}

}
