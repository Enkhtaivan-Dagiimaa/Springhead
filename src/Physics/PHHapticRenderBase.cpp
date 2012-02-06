#include <Physics/PHHapticRenderBase.h>

namespace Spr{;

void PHHapticRenderBase::HapticRendering(PHHapticRenderInfo hri){
	for(int i = 0; i < (int)hri.pointers->size(); i++){
		PHHapticPointer* pointer = hri.pointers->at(i);
		PenaltyBasedRendering(pointer, hri);
	}
}

PHIrs PHHapticRenderBase::CompIntermediateRepresentation(PHHapticPointer* pointer, PHHapticRenderInfo hri){
	PHSolidsForHaptic* hsolids = hri.hsolids;
	PHSolidPairsForHaptic* sps = hri.sps;
	double pdt = hri.pdt;
	double hdt = hri.hdt;
	double loopCount = (double)hri.loopCount;
	bool bInterpolatePose = hri.bInterpolatePose;
	bool bMultiPoints = hri.bMultiPoints;

	const double syncCount = pdt / hdt;
	double t = loopCount / syncCount;
	if(t > 1.0) t = 1.0;
	
	PHIrs irs;
	int nNeighbors = (int)pointer->neighborSolidIDs.size();
	for(int i = 0; i < nNeighbors; i++){
		int solidID = pointer->neighborSolidIDs[i];
		PHSolidPairForHaptic* sp = sps->item(solidID, pointer->GetPointerID());
		PHSolid* curSolid[2];
		curSolid[0] = &hsolids->at(solidID)->localSolid;
		curSolid[1] = DCAST(PHSolid, pointer);
		PHIrs tempIrs = sp->CompIntermediateRepresentation(curSolid, t, bInterpolatePose, bMultiPoints);
		if(tempIrs.size() == 0) continue;
		irs.insert(irs.end(), tempIrs.begin(), tempIrs.end());
	}
	return irs;
}

void PHHapticRenderBase::PenaltyBasedRendering(PHHapticPointer* pointer, PHHapticRenderInfo hri){

	PHIrs irs = CompIntermediateRepresentation(pointer, hri);

	SpatialVector outForce = SpatialVector();
	int NIrs = irs.size();
	if(NIrs > 0){
		double K = 100;
		double D = 0.1;
		for(int i = 0; i < NIrs; i++){
			PHIr* ir = &irs[i];
			Vec3d ortho = ir->depth * ir->normal;
			Vec3d dv = ir->pointerPointVel - ir->contactPointVel;
			Vec3d dvortho = dv.norm() * ir->normal;

			Vec3d addforce = -1 * (K * ortho + D * dvortho);
			double ws4 = pow(pointer->GetWorldScale(), 4);
			outForce.v() += addforce / ws4;
			outForce.w() += Vec3d();
		
			ir->force = -1 * addforce;
		}
		pointer->allIrs.insert(pointer->allIrs.end(), irs.begin(), irs.end());
	}
	pointer->SetForce(outForce);
}

void PHHapticRenderBase::ConstraintBasedRendering(PHHapticRenderInfo hri){
	DSTR << "----------haptic rendering" << std::endl;
	for(int p = 0; p < (int)hri.pointers->size(); p++){
		PHHapticPointer* pointer = hri.pointers->at(p);

		// ���ԕ\�������߂�B���C��Ԃ��X�V
		PHIrs irs = CompIntermediateRepresentation(pointer, hri);
		
		SpatialVector outForce = SpatialVector();
		int Nirs = irs.size();
		if(Nirs > 0){
#if 1
			Vec3d debug;
			// ���C
			for(int i = 0; i < Nirs; i++){
				double mu = irs[i].mu;	// �����C�W��
				mu = 0.5;
				
				double l = mu * irs[i].depth; // ���C�~�����a
				Vec3d vps = irs[i].pointerPointW;
				Vec3d vq = irs[i].solidPair->relativePose * irs[i].pointerPointW;
				Vec3d dq = (vq - vps) * irs[i].normal * irs[i].normal;
				Vec3d vqs = vq - dq;
				Vec3d tangent = vqs - vps;

				DSTR << "vps" << vps << std::endl;
				DSTR << "vq" << vq << std::endl;
				DSTR << "tangent " << tangent << tangent.norm() << std::endl;

				PHIr ir;
				ir = irs[i];
				double epsilon = 1e-5;
				if(tangent.norm() < epsilon){
					// �Î~���
					DSTR << "rest" << std::endl;
				}
				if(epsilon < tangent.norm() && tangent.norm() <= l){
					 //�Ö��C�i�Î~���C���a���j
					ir.normal = tangent.unit();
					ir.depth = tangent.norm();
					irs.push_back(ir);
					DSTR << "static friction" << std::endl;
				}
				if(epsilon < l && l < tangent.norm()){
					// �����C
					ir.normal = tangent.unit();
					ir.depth = l;
					irs.push_back(ir);
					DSTR << "dynamic friction" << std::endl;
				}
			}
#endif
			// �v���L�V�p���v�Z�̂��߂̘A���s����
			Nirs = irs.size();
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
					Vec3d ri = irs[i].r;
					Vec3d ni = irs[i].normal;
					Vec3d rj = irs[j].r;
					Vec3d nj = irs[j].normal;

					// ���i�S��
					double trans = ni * nj * massInv;
					// ��]�S��
					double rotate = (ri % ni) *	rotationMatInv * (rj % nj);
					c[i][j] = trans + rotate;
				}
				d[i] = irs[i].depth;
			}
			VVector< double > f;
			f.resize(Nirs);
			f.clear(0.0);

			// �A���s����������
			// ���i1�_�ڐG�j�Ŗ��C����̏ꍇ���ƐN�������ɉ�]��
			// �܂܂��B���͐��������A�v���L�V�̍X�V�����܂������Ȃ��Ȃ�̂ŁA
			// ��]�̏d�ݍs����Ȃ�ׂ��傫������K�v������B
			GaussSeidel(c, f, -d);

			// �|�C���^�ړ��ʂ����߂�
			Vec3d dr = Vec3d();
			Vec3d dtheta = Vec3d();
			double fall = 0.0;
			for(int i = 0; i < Nirs; i++){
				f[i] = std::max(f[i], 0.0);
				// ���i��
				Vec3d tmpdr = f[i] * irs[i].normal * massInv;
				dr += tmpdr;
				// ��]��
				Vec3d tmpdtheta = f[i] * rotationMatInv * (irs[i].r % irs[i].normal);
				dtheta += tmpdtheta;
				// �͊o�v�Z�̔䗦
				fall += f[i];
			}
			// �v���L�V�ʒu�p���X�V�i�ڕW�ʒu�p��������ԁj
			pointer->targetProxy.Ori() = ( Quaterniond::Rot(dtheta) * pointer->GetOrientation() ).unit();
			pointer->targetProxy.Pos() = pointer->GetFramePosition() + dr;

			// ���Έʒu�v�Z�p�̃v���L�V�ʒu�p��
			pointer->proxyPose.Pos() = pointer->targetProxy.Pos();
		

			/// �͊o�C���^�t�F�[�X�ɏo�͂���͂̌v�Z
			double ws4 = pow(pointer->GetWorldScale(), 4);
			double hdt = hri.hdt;
			Vec3d last_dr = pointer->last_dr;
			Vec3d last_dtheta = pointer->last_dtheta;

			outForce.v() = 50 * dr  + 0.0 * (dr - last_dr)/hdt;
			//outForce.w() = (pointer->springOriK * dtheta + pointer->damperOriD * ((dtheta - last_dtheta)/hdt));
			pointer->last_dr = dr;
			pointer->last_dtheta = dtheta; 

			// ���̂ɉ�����͂��v�Z
			for(int i = 0; i < Nirs; i++){
				f[i] = std::max(f[i], 0.0);
				irs[i].force = -1 * outForce.v().norm() * irs[i].normal.unit() * f[i] / fall;
			}
		}
		pointer->SetForce(outForce);
	}
}

void PHHapticRenderBase::VirtualCoupling(PHHapticPointer* pointer){
}




// 1/11�͊o�����_�����O
// �����\��
// ���^�|�C���^�݂̂��g����Proxy3DoF(�r�c���񂪍�����z�j
// proxy�Ɏ��ʂ�^���Astick-slip friction���񎦂ł���proxy simulation
// �ʌ`�󂪎g����multiple point intermediate representation
// �ʌ`�󂪎g����constratint based rendering
// �ʌ`�󂪎g����virtual coupling


// PHSolidForHaptic��solid�̓|�C���^�Ȃ̂ŁAphysics���ւ̍��̂̃|�C���^�ɂȂ��Ă�
// ��������K�v������


}