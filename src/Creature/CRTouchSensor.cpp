/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include <Creature/CRTouchSensor.h>
#include <Creature/SprCRBody.h>
#include <Creature/SprCRCreature.h>
#include <Physics/SprPHScene.h>

#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

namespace Spr{;

void CRTouchSensor::Step() {
	// ������AddChildObject�Ŏw�肷��悤�ɂ��ׂ� (mitake, 09/07/19)
	CRBodyIf* body = DCAST(CRCreatureIf,DCAST(SceneObjectIf,this)->GetScene())->GetBody(0);
	PHSceneIf* phScene = DCAST(CRCreatureIf,DCAST(SceneObjectIf,this)->GetScene())->GetPHScene();

	// �ڐG���X�g�̍\�z���J�n����
	contactList.clear();

	int sceneCnt = phScene->GetCount();

	PHSolidPairForLCPIf* solidPair;
	PHShapePairForLCPIf* shapePair;

	Vec3d totalForce;

	// PHScene���̂��ׂĂ̍��̂ɂ��ă`�F�b�N
	for (int i=0; i<phScene->NSolids(); ++i) {
		for (int j=i+1; j<phScene->NSolids(); ++j) {
			// �����̑̂��\�����鍄�� �� ����ȊO�̍��� �̃y�A�݂̂Ɍ���
			bool iIsMe = false, jIsMe = false;
			for (int n=0; n<body->NSolids(); ++n) {
				if (body->GetSolid(n) && body->GetSolid(n)->GetPHSolid() == phScene->GetSolids()[i]) { iIsMe = true; }
				if (body->GetSolid(n) && body->GetSolid(n)->GetPHSolid() == phScene->GetSolids()[j]) { jIsMe = true; }
				if (iIsMe && jIsMe) { break; }
			}
			if ((iIsMe && jIsMe) || (!iIsMe && !jIsMe)) { continue; }

			PHSolidIf *so1, *so2, *soMe, *soOther;
			if (iIsMe) {
				so1 = soMe		= phScene->GetSolids()[i];
				so2 = soOther	= phScene->GetSolids()[j];
			} else {
				so1 = soOther	= phScene->GetSolids()[i];
				so2 = soMe		= phScene->GetSolids()[j];
			}

			solidPair = phScene->GetSolidPair(i, j);
			PHConstraintIf* constraint = phScene->GetConstraintEngine()->GetContactPoints()->FindBySolidPair(so1, so2);

			if (!solidPair)  { continue; }
			if (!constraint) { continue; }

			Vec3d force = phScene->GetConstraintEngine()->GetContactPoints()->GetTotalForce(so1, so2);

			for (int s=0; s<so1->NShape(); ++s) {
				for (int t=0; t<so2->NShape(); ++t) {

					// ���̕��@���Ɠ������̂̈قȂ�Shape�ɂ��Ă̐ڐG��
					// �قȂ�Contact�ƂȂ�B
					// ��������̂��Ƃɂ܂Ƃ߂�ɂ͔畆���o�̉��Z�ɂ��Ă̒m�����K�v�B
					// �Ƃ肠�������Ƃ܂킵 (mitake, 09/02/07)

					shapePair = solidPair->GetShapePair(s, t);

					if (!shapePair) { continue; }

					int			contactStat	= solidPair->GetContactState(s, t);
					unsigned	lastContCnt	= solidPair->GetLastContactCount(s, t);


					if (contactStat == 1 || (contactStat == 2 && (lastContCnt == sceneCnt-1))) {

						totalForce += force;

						double		depth			= solidPair->GetContactDepth(s, t);
						int			nSectionVtx		= shapePair->NSectionVertexes();

						CRContactInfo contact;
						contact.pos			= solidPair->GetCommonPoint(s, t);
						contact.soMe		= soMe;
						contact.soOther		= soOther;
						contact.force		= force;
						contact.pressure	= force.norm();
						/*
						if (nSectionVtx != 0) {
							contact.normal		= shapePair->GetNormalVector();
							contact.dimension	= shapePair->GetContactDimension();
							contact.pressure	= fabs(force.dot(contact.normal));
						} else {
							// �ڐG�ʂ̋ȗ����a
							CDConvexIf* sh1 = shapePair->GetShape(0)->Cast();
							int i1=0; for (; i1<so1->NShape(); ++i1) { if (so1->GetShape(i1)==sh1) break; }
							Posed q1 = so1->GetShapePose(i1).Inv() * so1->GetPose().Inv();
							double r1 = sh1->CurvatureRadius(q1 * contact.pos);

							CDConvexIf* sh2 = shapePair->GetShape(1)->Cast();
							int i2=0; for (; i2<so2->NShape(); ++i2) { if (so2->GetShape(i2)==sh2) break; }
							Posed q2 = so2->GetShapePose(i2).Inv() * so2->GetPose().Inv();
							double r2 = sh2->CurvatureRadius(q2 * contact.pos);

							// Hertz�̐ڐG����
							double E = 4.2e+5;	/// -- �畆��Young��   ���̕ӂɎ��̒l���g���̂͂��낢�떳�Ӗ������B
							double v = 0.49;	/// -- �畆��Poisson��
							double P = force.norm();  /// -- �������ݗ�
							double a = pow((3.0/4.0)*P*( 2*(1-v*v)/E )/(1/r1 + 1/r2), 1/3.0);
							double pMax = 3*P/(2*3.1415926*a*a);

							if (so1 == soMe) {
								contact.normal = q1.Ori().Inv() * sh1->Normal(q1 * contact.pos);
							} else {
								contact.normal = q2.Ori().Inv() * sh2->Normal(q2 * contact.pos);
							}
							contact.dimension	= 1e-4;
							contact.pressure	= pMax;
						}
						*/
						contactList.push_back(contact);
					}

				}
			}

		}
	}
}

} // namespace Spr
