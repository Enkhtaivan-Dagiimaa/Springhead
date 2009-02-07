/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include "CRTouchSensor.h"

#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

namespace Spr{;

void CRTouchSensor::Init() {
	CRSensor::Init();
}

void CRTouchSensor::Step() {
	CRBodyIf* body = creature->GetBody(0);

	// �ڐG���X�g�̍\�z
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
				if (body->GetSolid(n) == phScene->GetSolids()[i]) { iIsMe = true; }
				if (body->GetSolid(n) == phScene->GetSolids()[j]) { jIsMe = true; }
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
				for (int t=s+1; t<so2->NShape(); ++t) {

					// ���̕��@���Ɠ������̂̈قȂ�Shape�ɂ��Ă̐ڐG��
					// �قȂ�Contact�ƂȂ�B
					// ��������̂��Ƃɂ܂Ƃ߂�ɂ͔畆���o�̉��Z�ɂ��Ă̒m�����K�v�B
					// �Ƃ肠�������Ƃ܂킵 (mitake, 09/02/07)

					shapePair = solidPair->GetShapePair(s, t);

					if (!shapePair) { continue; }

					int			contactStat	= solidPair->GetContactState(s, t);
					unsigned	lastContCnt	= solidPair->GetLastContactCount(s, t);

					if (contactStat == 1 || (contactStat == 2 && lastContCnt == sceneCnt)) {
						totalForce += force;

						double		depth			= solidPair->GetContactDepth(s, t);
						int			nSectionVtx		= shapePair->NSectionVertexes();

						Contact contact;
						contact.pos		= solidPair->GetCommonPoint(s, t);
						contact.soMe	= soMe;
						contact.soOther	= soOther;
						if (nSectionVtx != 0) {
							contact.dimension	= shapePair->GetContactDimension();
							contact.normal		= shapePair->GetNormalVector();
							contact.pressure	= fabs(force.dot(contact.normal));
						} else {
							contact.dimension	= 1e-4;
							contact.pressure	= 10.0;
						}
						contactList.push_back(contact);
					}

				}
			}

		}
	}
}

} // namespace Spr
