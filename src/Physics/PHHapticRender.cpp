#include <Physics/PHHapticRender.h>
#include <Physics/PHHapticEngine.h>

namespace Spr{;

PHHapticRender::PHHapticRender(){}

void PHHapticRender::HapticRendering(PHHapticRenderInfo info){
	*(PHHapticRenderInfo*)this = info;
	for(int i = 0; i < (int)pointers->size(); i++){
		PHHapticPointer* pointer = pointers->at(i);
		pointer->hapticForce.clear();
		switch (pointer->hapticRenderMode){
			case PHHapticPointerDesc::PENALTY:
				PenaltyBasedRendering(pointer);
				break;
			case PHHapticPointerDesc::CONSTRAINT:
				ConstraintBasedRendering(pointer);
				break;
			case PHHapticPointerDesc::VC:
				VirtualCoupling(pointer);
				break;
		}
 		VibrationRendering(pointer);
	}
}

PHIrs PHHapticRender::CompIntermediateRepresentation(PHHapticPointer* pointer){
	const double syncCount = pdt / hdt;
	double t = loopCount / syncCount;
	if(t > 1.0) t = 1.0;

	PHIrs irs;
	int nNeighbors = (int)pointer->neighborSolidIDs.size();
	for(int i = 0; i < nNeighbors; i++){
		int solidID = pointer->neighborSolidIDs[i];
		PHSolidPairForHaptic* sp = sps->item(solidID, pointer->GetPointerID());
		PHSolid* curSolid[2];
		curSolid[0] = hsolids->at(solidID)->GetLocalSolid();
		curSolid[1] = DCAST(PHSolid, pointer);
		PHIrs tempIrs = sp->CompIntermediateRepresentation(curSolid, t, bInterpolatePose);
		if(tempIrs.size() == 0) continue;
		irs.insert(irs.end(), tempIrs.begin(), tempIrs.end());
	}
	return irs;
}

// �͊o�����_�����O
// �i�Ƃ肠�����~�����j
// �ʌ`�󂪎g����PenaltyBasedRendering(multiple point intermediate representation)
// �ʌ`�󂪎g����constratint based rendering
// �ʌ`�󂪎g����virtual coupling
// �i�]�͂�����΁j
// ���^�|�C���^�݂̂��g����Proxy3DoF(�r�c���񂪍�����z�j
// proxy�Ɏ��ʂ�^���Astick-slip friction���񎦂ł���proxy simulation


// �e�ڐG���_�̐N���ʂɃo�l�W���������č��v�������̂��o��
// �o�l�W���������ƕs����ɂȂ�̂Œ�߂Ɂi1000N/m)���x�ɗ}����K�v����
void PHHapticRender::PenaltyBasedRendering(PHHapticPointer* pointer){
	// ���C�͒ǉ��ł��Ȃ��̂ŁA�����I��false
	const bool bFric = pointer->bFriction;
	if(pointer->bFriction){
		pointer->EnableFriction(false);
	}
	PHIrs irs = CompIntermediateRepresentation(pointer);
	SpatialVector outForce = SpatialVector();
	int NIrs = irs.size();
	if(NIrs > 0){
		for(int i = 0; i < NIrs; i++){
			PHIr* ir = irs[i];
			Vec3d ortho = ir->depth * ir->normal;
			Vec3d dv = ir->pointerPointVel - ir->contactPointVel;
			Vec3d dvortho = dv.norm() * ir->normal;

			//float K = ir->springK / pointer->GetPosScale();
			//float D = ir->damperD / pointer->GetPosScale();
			float K  = pointer->GetReflexSpring() / pointer->GetPosScale();
			float D = pointer->GetReflexDamper() / pointer->GetPosScale();

			Vec3d addforce = K * ortho + D * dvortho;
			outForce.v() += addforce;
			outForce.w() += Vec3d();
			
			Vec3d pointForce = -1 * addforce;
			hsolids->at(irs[i]->solidID)->AddForce(pointForce, irs[i]->contactPointW);
			PHSolid* localSolid = &hsolids->at(irs[i]->solidID)->localSolid;
			PHSolidPairForHaptic* sp = sps->item(irs[i]->solidID, pointer->GetPointerID());
			sp->force += pointForce;	// ����|�C���^�����̂ɉ������
			sp->torque += (irs[i]->contactPointW - localSolid->GetCenterPosition()) ^ pointForce;
		}
	}
	pointer->AddHapticForce(outForce);
	pointer->EnableFriction(bFric);
}

void PHHapticRender::ConstraintBasedRendering(PHHapticPointer* pointer){
	// ���ԕ\�������߂�B���C��Ԃ��X�V
	PHIrs irs = CompIntermediateRepresentation(pointer);
		
	SpatialVector outForce = SpatialVector();
	int Nirs = irs.size();
	if(Nirs > 0){
		// �v���L�V�p���v�Z�̂��߂̘A���s����
		VMatrixRow< double > c;
		c.resize(Nirs, Nirs);
		c.clear(0.0);
		VVector< double > d;
		d.resize(Nirs);
		d.clear(0.0);
		double massInv = pointer->GetMassInv();
		Matrix3d rotationMatInv =  (pointer->GetInertia() * pointer->GetRotationalWeight()).inv();
		for(int i = 0; i < Nirs; i++){
			for(int j = 0; j < Nirs; j++){
				Vec3d ri = irs[i]->r;
				Vec3d ni = irs[i]->normal;
				Vec3d rj = irs[j]->r;
				Vec3d nj = irs[j]->normal;

				// ���i�S��
				double trans = ni * nj * massInv;
				// ��]�S��
				double rotate = (ri % ni) *	rotationMatInv * (rj % nj);
				c[i][j] = trans + rotate;
			}
			d[i] = irs[i]->depth;
		}
		VVector< double > f;
		f.resize(Nirs);
		f.clear(0.0);

		// �A���s����������
		// ���i1�_�ڐG�j�Ŗ��C����̏ꍇ���ƐN�������ɉ�]��
		// �܂܂��B���͐��������A�v���L�V�̍X�V�����܂������Ȃ��Ȃ�̂ŁA
		// ��]�̏d�ݍs����Ȃ�ׂ��傫������K�v������B
		// ��]�̏d�ݍs���傫������ƃv���L�V�̉�]�ړ����Ȃ��Ȃ邽�߁A��]���C���łȂ��Ȃ�
		GaussSeidel(c, f, -d);

		// �|�C���^�ړ��ʂ����߂�
		Vec3d dr = Vec3d();
		Vec3d dtheta = Vec3d();
		Vec3d allDepth = Vec3d();
		for(int i = 0; i < Nirs; i++){
			f[i] = std::max(f[i], 0.0);
			// ���i��
			Vec3d tmpdr = f[i] * irs[i]->normal * massInv;
			dr += tmpdr;
			// ��]��
			Vec3d tmpdtheta = f[i] * rotationMatInv * (irs[i]->r % irs[i]->normal);
			dtheta += tmpdtheta;
				
			allDepth += -1 * irs[i]->normal * irs[i]->depth;
		}


		// �v���L�V�ʒu�p���X�V�i�ڕW�ʒu�p��������ԁj
		pointer->targetProxy.Ori() = ( Quaterniond::Rot(dtheta) * pointer->GetOrientation() ).unit();
		pointer->targetProxy.Pos() = pointer->GetFramePosition() + dr;

		// ���Έʒu�v�Z�p�̃v���L�V�ʒu�p��
		pointer->proxyPose.Pos() = pointer->targetProxy.Pos();
		
		/// �͊o�C���^�t�F�[�X�ɏo�͂���͂̌v�Z
		Vec3d last_dr = pointer->last_dr;
		Vec3d last_dtheta = pointer->last_dtheta;

		float K  = pointer->GetReflexSpring() / pointer->GetPosScale();
		float D = pointer->GetReflexDamper() / pointer->GetPosScale();
		float KOri = pointer->GetReflexSpringOri() * pointer->GetRotationalWeight();
		float DOri = pointer->GetReflexDamperOri() * pointer->GetRotationalWeight();

		outForce.v() = K * dr  + D * (dr - last_dr)/hdt;
		outForce.w() = (KOri * dtheta + DOri * ((dtheta - last_dtheta)/hdt));
		pointer->last_dr = dr;
		pointer->last_dtheta = dtheta; 

		// ���̂ɉ�����͂��v�Z
		// �����_�����O�����͂���e�ڐG�_�ɓ����͂��t�Z
		// ���܂������ĂȂ��\��������
		Vec3d ratio;
		double epsilon = 1e-10;
		for(int i = 0; i < 3; i++){
			ratio[i] = outForce.v()[i] / allDepth[i];
			if(abs(allDepth[i]) < epsilon) ratio[i] = 0.0;
		}
		//DSTR << "all" << outForce << std::endl;
		//DSTR << "ratio" << ratio << std::endl;
		//DSTR << "NIrs" << Nirs << std::endl;
		for(int i = 0; i < Nirs; i++){
			Vec3d pointForce = Vec3d();	// �e�ڐG�_�ɓ�����
			Vec3d dir = irs[i]->normal * irs[i]->depth;
			for(int j = 0; j < 3; j++){
				pointForce[j] = ratio[j] * dir[j];// *  hri.hdt / hri.pdt;
			}
			//DSTR << "pos" << irs[i]->contactPointW << std::endl;
			//DSTR << "depth" << irs[i]->depth << std::endl;
			//DSTR << "pointForce" << pointForce << std::endl;
			hsolids->at(irs[i]->solidID)->AddForce(pointForce, irs[i]->contactPointW);	// �e�|�C���^�����̂ɉ������S�Ă̗�
			PHSolid* localSolid = &hsolids->at(irs[i]->solidID)->localSolid;
			PHSolidPairForHaptic* sp = sps->item(irs[i]->solidID, pointer->GetPointerID());
			sp->force += pointForce;	// ����|�C���^�����̂ɉ������
			sp->torque += (irs[i]->contactPointW - localSolid->GetCenterPosition()) ^ pointForce;
			//DSTR << sp->force << std::endl;
			//DSTR << irs[i]->contactPointW << " " << localSolid->GetCenterPosition() << std::endl;
			//DSTR << irs[i]->contactPointW - localSolid->GetCenterPosition() << std::endl;
		}
	}
	pointer->AddHapticForce(outForce);
	//DSTR << pointer->GetFramePosition() << std::endl;
	//DSTR << "render" << outForce << std::endl;
	//CSVOUT << outForce[0] << "," << outForce[1] << "," << outForce[2] << "," << outForce[3] << "," << outForce[4] << "," << outForce[5] << "," <<std::endl;
}

void PHHapticRender::VibrationRendering(PHHapticPointer* pointer){
	if(!pointer->bVibration) return;
	int Nneigbors = pointer->neighborSolidIDs.size();
	for(int j = 0; j < (int)Nneigbors; j++){
		int solidID = pointer->neighborSolidIDs[j];
		PHSolidPairForHaptic* sp = sps->item(solidID, pointer->GetPointerID());
		PHSolid* solid = hsolids->at(solidID)->GetLocalSolid(); 
		if(sp->frictionState == sp->FREE) continue;
		if(sp->frictionState == sp->FIRST){
			sp->vibrationVel = pointer->GetVelocity() - solid->GetVelocity();
		}
		Vec3d vibV = sp->vibrationVel;
		double vibA = solid->GetShape(0)->GetVibA();
		double vibB = solid->GetShape(0)->GetVibB();
		double vibW = solid->GetShape(0)->GetVibW();
		double vibT = sp->contactCount * hdt;

		SpatialVector vibForce;
		// �@�������Ɏˉe����K�v������H
		vibForce.v() = vibA * vibV * exp(-vibB * vibT) * sin(2 * M_PI * vibW * vibT) / pointer->GetPosScale();		//�U���v�Z
		pointer->AddHapticForce(vibForce);
		//CSVOUT << vibForce.v().x << "," << vibForce.v().y << "," << vibForce.v().z << std::endl;
	}
}

void PHHapticRender::VirtualCoupling(PHHapticPointer* pointer){
//	const double syncCount = pdt / hdt;
//	double t = loopCount / syncCount;
//	if(t > 1.0) t = 1.0;
//
//	// �O��ƍ���̏�Ԃ���
//	Posed curPose = pointer->vcSolidCopied.GetPose();
//	//Posed lastPose = pointer->vcSolidCopied.GetLastPose();
//	Vec3d curVel = pointer->vcSolidCopied.GetVelocity();
//	//Vec3d lastVel = pointer->vcSolidCopied.GetLastVelocity();
//	Vec3d curAngVel = pointer->vcSolidCopied.GetAngularVelocity();
//	//Vec3d lastAngVel = pointer->vcSolidCopied.GetLastAngularVelocity();
//
//	Posed interpolationPose = interpolate(t, lastPose, curPose);
//	Vec3d interpolationVel = interpolate(t, lastVel, curVel);
//	Vec3d interpolationAngVel = interpolate(t, lastAngVel, curAngVel);
//
//	Vec3d dr = pointer->GetPose().Pos() - interpolationPose.Pos();
//	Vec3d dv = pointer->GetVelocity() - interpolationVel;
//	Vec3d dAngVel =  pointer->GetAngularVelocity() - interpolationAngVel;
//
//	SpatialTransform X[2];
//	X[0].r = pointer->vcSolidCopied.GetCenterPosition();
//	X[1].r = pointer->GetCenterPosition();
//	SpatialTransform Xj[2];
//	Xj[0].q = interpolationPose.Ori();
//	Xj[1].q = pointer->GetOrientation();
//
//	SpatialTransform Xjrel =  Xj[1] * X[1] * X[0].inv() * Xj[0].inv();
//	Quaterniond diff = Xjrel.q;
//	Vec3d prop = diff.RotationHalf();
//
////	float K  = 2000 / pointer->GetPosScale();
////	float D = 300 / pointer->GetPosScale();
//	float K  = 1000 / pointer->GetPosScale();
//	float D = 150 / pointer->GetPosScale();
//	float Kori = 0.01;
//	float Dori = 0.001;
//
//	SpatialVector outForce;
//	outForce.v() = -1 * (K * dr  + D * dv);
//	outForce.w() = -1 * (Kori * prop + Dori * dAngVel);
//
//	pointer->AddHapticForce(outForce / 20);
//	pointer->vcForce += -1 * outForce / syncCount;
//
//	//DSTR <<  pointer->vcForce <<std::endl;
//	//CSVOUT << outForce.v().x << "," << outForce.v().y << "," << outForce.v().x * 20 << std::endl;

}






}