/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include "CRNeckController.h"
#include <string>

#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

namespace Spr{;
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
// 
void CRNeckController::LookAt(Vec3f pos, Vec3f vel, float attractiveness){
	this->pos = pos;
	this->vel = vel;
	this->attractiveness = attractiveness;
}

void CRNeckController::Init(){
	orig = Quaterniond();
	csHead  = NULL;
}

void CRNeckController::Step(){
	if (csHead) {
		PHSolidIf*    soHead = csHead->GetPHSolid();
		PHIKOriCtlIf* cpHead = NULL;
		for (int i=0; i<csHead->NIKControlPoints(); ++i) {
			if (!cpHead) {
				cpHead = csHead->GetIKControlPoint(i)->Cast();
			} else {
				break;
			}
		}
		Vec3d rot = PTM::cross(soHead->GetPose().Ori()*Vec3d(0,1,0), (pos-(soHead->GetPose().Pos())).unit());
		if (!(attractiveness <= lowerAttractiveness)) {
			if (attractiveness < upperAttractiveness) {
				rot = rot * ((attractiveness - lowerAttractiveness) / (upperAttractiveness - lowerAttractiveness));
			}
			Quaterniond qt = Quaterniond::Rot(rot.norm(), rot.unit());
			cpHead->SetTargetPosition(qt*soHead->GetPose().Ori());
			/*
			if (t > 0) {
				double ratio = 1 - (10*pow(t,3) - 15*pow(t,4) + 6*pow(t,5));
				// ratio = 0.1;
				if (ratio > 0) {
					rot = rot * ratio;
					Quaterniond qt = Quaterniond::Rot(rot.norm(), rot.unit());
					// cpHead->SetTargetPosition(qt*soHead->GetPose().Ori());
					cpHead->SetTargetPosition(qt * orig);
					std::cout << "r = " << ratio << std::endl;
				} else {
					std::cout << "r = " << ratio << std::endl;
					// cpHead->SetTargetPosition(soHead->GetPose().Ori());
					cpHead->SetTargetPosition(orig);
				}
				t -= 0.005;
			} else {
				if (rot.norm() < Rad(5)) {
					// std::cout << Deg(rot.norm()) << std::endl;
					Quaterniond qt = Quaterniond::Rot(rot.norm(), rot.unit());
					cpHead->SetTargetPosition(qt*soHead->GetPose().Ori());
				} else {
					// std::cout << Deg(rot.norm()) << std::endl;
					t = 1.0;
					orig = soHead->GetPose().Ori();
				}
			}
			*/
			cpHead->Enable(true);
		}
	} else {
		// ������AddChildObject�Ŏw�肷��悤�ɂ��ׂ� (mitake, 09/07/19)
		CRBodyIf* body = DCAST(CRCreatureIf,DCAST(SceneObjectIf,this)->GetScene())->GetBody(0);
		PHSceneIf* phScene = DCAST(CRCreatureIf,DCAST(SceneObjectIf,this)->GetScene())->GetPHScene();

		for (size_t i=0; i<body->NChildObject(); ++i) {
			CRIKSolidIf* cso = body->GetChildObject(i)->Cast();
			if (cso) {
				if (std::string("Head")  == cso->GetLabel()) { csHead  = cso; }
			}
		}
	}
}

}
