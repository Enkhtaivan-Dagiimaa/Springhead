#include "SampleModel.h"

PHSolidIf* CreateBox(FWSdkIf* fwSdk){
	// ���̂̍쐬
	PHSolidDesc desc;
	desc.mass = 0.05;
	desc.inertia *= 0.033;
	PHSolidIf* soBox = fwSdk->GetScene(0)->GetPHScene()->CreateSolid(desc);
	// �`��̍쐬
	CDBoxDesc bd;
	bd.boxsize = Vec3f(2,2,2);
	CDShapeIf* shapeBox = fwSdk->GetPHSdk()->CreateShape(bd);
	// ���̂Ɍ`���t��
	soBox->AddShape(shapeBox);
	soBox->SetFramePosition(Vec3d(0, 10, 0));
	return soBox;
}

PHSolidIf* CreateSphere(FWSdkIf* fwSdk){
	// ���̂̍쐬
	PHSolidDesc desc;
	desc.mass = 0.05;
	desc.inertia *= 0.03;
	PHSolidIf* soSphere = fwSdk->GetScene(0)->GetPHScene()->CreateSolid(desc);
	// �`��̍쐬
	CDSphereDesc sd;
	sd.radius = 2.0;
	CDShapeIf* shapeSphere = fwSdk->GetPHSdk()->CreateShape(sd);
	// ���̂Ɍ`���t������
	soSphere->AddShape(shapeSphere);
	soSphere->SetFramePosition(Vec3d(0, 15, 0));
	return soSphere;
}

PHSolidIf* CreateCapsule(FWSdkIf* fwSdk){
	// ���̂̍쐬
	PHSolidDesc desc;
	desc.mass = 0.05;
	desc.inertia *= 1;
	PHSolidIf*  soCapsule = fwSdk->GetScene(0)->GetPHScene()->CreateSolid(desc);
	// �`��̍쐬
	CDCapsuleDesc cd;
	cd.length = 3.0;
	cd.radius = 1.0;
	CDShapeIf* shapeCapsule = fwSdk->GetPHSdk()->CreateShape(cd);
	// ���̂Ɍ`���t������
	soCapsule->AddShape(shapeCapsule);
	soCapsule->SetFramePosition(Vec3d(0, 15, 0));
	return soCapsule;
}

PHSolidIf* CreateRoundCone(FWSdkIf* fwSdk){
	// ���̂̍쐬
	PHSolidDesc desc;
	desc.mass = 0.05;
	desc.inertia *= 1;
	PHSolidIf*  soRCone = fwSdk->GetScene(0)->GetPHScene()->CreateSolid(desc);
	// �`��̍쐬
	CDRoundConeDesc rd;
	rd.length = 3.0;
	rd.radius[0] = 1.5;
	rd.radius[1] = 0.5;
	CDShapeIf* shapeRCone = fwSdk->GetPHSdk()->CreateShape(rd);
	// ���̂Ɍ`���t������
	soRCone->AddShape(shapeRCone);
	soRCone->SetFramePosition(Vec3d(0, 15, 0));
	return soRCone;
}