#include <Physics/PHHapticEngine.h>
#include <Physics/PHHapticRender.h>
#include <Physics/PHHapticEngineImpulse.h>
#include <Physics/PHHapticEngineSingleBase.h>
#include <Physics/PHHapticEngineLD.h>

namespace Spr{;

//----------------------------------------------------------------------------
// PHSolidForHaptic

PHSolidForHaptic::PHSolidForHaptic(){
	bPointer = false;	
	doSim = 0;	
	NLocalFirst = 0;
	NLocal = 0;
}
void PHSolidForHaptic::AddForce(Vec3d f){
	force += f;
}
void PHSolidForHaptic::AddForce(Vec3d f, Vec3d r){
	torque += (r - localSolid.pose * localSolid.center) ^ f;
	force += f;
}
//----------------------------------------------------------------------------
// PHSapePairForHaptic
PHShapePairForHaptic::PHShapePairForHaptic(){}
PHShapePairForHaptic::PHShapePairForHaptic(const PHShapePairForHaptic& s){
	*this = s;
}
bool PHShapePairForHaptic::Detect(unsigned ct, const Posed& pose0, const Posed& pose1){
	// 0:����, 1:�͊o�|�C���^
	// �O��̏�Ԃ�ۑ�
	for(int i = 0; i < 2; i++){
		lastShapePoseW[i] = shapePoseW[i];
		lastClosestPoint[i] = closestPoint[i];	
	}
	lastNormal = normal;

	// ����̏�Ԃ�ۑ�
	shapePoseW[0] = pose0;
	shapePoseW[1] = pose1;

	// �ŋߖT�_�΂�������
	Vec3d sep;
	double dist = FindClosestPoints(shape[0], shape[1], shapePoseW[0], shapePoseW[1],
									sep, closestPoint[0], closestPoint[1]);
	Vec3d w0 = shapePoseW[0] * closestPoint[0];
	Vec3d w1 = shapePoseW[1] * closestPoint[1];
	normal = (w1 - w0).unit();		// ����->�͊o�|�C���^�ւ̖@���x�N�g��
	commonPoint = (w0 + w1) * 0.5;	// ���L�_

	if(dist > 1e-3){
		// �ڐG���Ă��Ȃ�
		state = NONE;
	}else{							
		// �ڐG
		if (lastContactCount == unsigned(ct-1))	state = CONTINUE;
		else state = NEW;
		lastContactCount = ct;
	}
	return true;
}

int PHShapePairForHaptic::OnDetect(unsigned ct, const Vec3d& center0){
	Vec3d dir = -1 * lastNormal;	// �͊o�|�C���^���猩�����̂̈ړ�����
	if(dir == Vec3d()){
		dir = commonPoint - center0;
		DSTR << "dir = Vec3d() in PHShapePariForHaptic::OnOnDetectClosestPoints" << std::endl;
	}
	double dist = 0.0;
	int cp = ContFindCommonPoint(shape[0], shape[1], shapePoseW[0], shapePoseW[1], dir, 
							-DBL_MAX, 1, normal, closestPoint[0], closestPoint[1], dist);
	
	/// GJK�����s�������̏���
	if(cp != 1){
		static int cont = 0;
		ContFindCommonPointSaveParam(shape[0], shape[1], shapePoseW[0], shapePoseW[1], dir, 
							-DBL_MAX, 1, normal, closestPoint[0], closestPoint[1], dist);
		DSTR << cont << "ContFindCommonPoint do not find contact point" << std::endl;
		cont++;
	}	
	return cp;
}

#define SELECTION 1 // ���ԕ\���̖ʂɍڂ��Ă���_�͐ڐG�_�Ƃ��Ȃ�
extern bool bUseContactVolume;
bool PHShapePairForHaptic::AnalyzeContactRegion(){
	bUseContactVolume = true;
	static CDContactAnalysis analyzer;
	analyzer.FindIntersection(this->Cast());
	bUseContactVolume = false;

	// �N���̈�̒��_�̎擾
	for(CDQHPlane< CDContactAnalysisFace >* it = analyzer.planes.begin; it != analyzer.planes.end; ++it){
		if(it->deleted) continue;
		Vec3d point = it->normal/it->dist + commonPoint;	// �o�Εϊ��i�ʂ���_�ցj	

		// 0:����, 1:�͊o�|�C���^
		if(SELECTION){
			Vec3d w0 = shapePoseW[0] * closestPoint[0];	// ���ԕ\���ʏ�̓_�i���̂̋ߖT�_�j
			double dot = (point - w0) * normal;
			if(dot < -1e-5)	intersectionVertices.push_back(shapePoseW[1].Inv() * point);
		}else{
			intersectionVertices.push_back(shapePoseW[1].Inv() * point);
		}
	}
	// �ʂ�������Ȃ������Ashape��convex or box�ł͂Ȃ������ꍇ
	if(intersectionVertices.size() == 0){
		intersectionVertices.push_back(closestPoint[1]);
		return false;
	}
	return true;
}

bool PHShapePairForHaptic::CompIntermediateRepresentation(Posed curShapePoseW[2], double t, bool bInterpolatePose, bool bPoints){
	irs.clear();
	Vec3d sPoint = curShapePoseW[0] * closestPoint[0];	// �����solid�̋ߖT�_�iWorld)
	Vec3d pPoint = curShapePoseW[1] * closestPoint[1];	// �����pointer�̋ߖT�_�iWorld)
	Vec3d last_sPoint = lastShapePoseW[0] * lastClosestPoint[0]; // �O��̍��̋ߖT�_�iWorld)
	Vec3d interpolation_normal = normal;		// ��Ԗ@��
	Vec3d interpolation_sPoint = sPoint;		// ��ԍ��̋ߖT�_

	// ���̖̂ʂ̖@����ԁ@�O��̖@���ƌ��݂̖@���̊Ԃ���
	interpolation_normal = interpolate(t, lastNormal, normal);

	Vec3d dir = pPoint - interpolation_sPoint;			
	double dot = dir * interpolation_normal;
	
	//DSTR << sPoint << std::endl;
	//DSTR << curShapePoseW[0] << "," << closestPoint[0] << std::endl;

	if(dot >= 0.0) return false;
	PHIr irtemp;
	irtemp.springK = (shape[0]->GetReflexSpring() + shape[1]->GetReflexSpring()) * 0.5;
	irtemp.damperD = (shape[0]->GetReflexDamper() + shape[1]->GetReflexDamper()) * 0.5;
	irtemp.mu = (shape[0]->GetDynamicFriction() + shape[1]->GetDynamicFriction()) * 0.5;
	irtemp.mu0 = (shape[0]->GetStaticFriction() + shape[1]->GetStaticFriction()) * 0.5;

	if(bPoints){
		for(int i = 0; i < (int)intersectionVertices.size(); i++){
			Vec3d iv = intersectionVertices[i];
			Vec3d wiv = curShapePoseW[1] * iv; 	// �|�C���^�̐N���_(world)
			dot = (wiv - interpolation_sPoint) * interpolation_normal;	// �f�o�C�X�̐N���_���璆�Ԗʏ�̓_�ւ̃x�N�g���̃m�����i�f�o�C�X�̐N���ʁj
			if(dot > 0.0)	continue;
			PHIr* ir = DBG_NEW PHIr();
			*ir = irtemp;
			ir->normal = interpolation_normal;
			ir->pointerPointW = wiv;
			Vec3d ortho = dot * interpolation_normal; // ���̂̋ߖT�_����f�o�C�X�N���_�܂ł̃x�N�g����ʖ@���֎ˉe
			ir->contactPointW = wiv - ortho;		// solid�̐ڐG�_(world)
			ir->depth = ortho.norm();
			ir->interpolation_pose = curShapePoseW[0];
			irs.push_back(ir);
		}
	}else{
		PHIr* ir = DBG_NEW PHIr();
		*ir = irtemp;
		ir->normal = interpolation_normal;
		ir->pointerPointW = pPoint;
		Vec3d ortho = dot * interpolation_normal;
		ir->contactPointW = pPoint - ortho;
		ir->depth = ortho.norm();
		ir->interpolation_pose = curShapePoseW[0];
		irs.push_back(ir);
	}
	//std::sort(irs.begin, irs.end()):
	return true;
}


//----------------------------------------------------------------------------
// PHSolidPairForHaptic

PHSolidPairForHaptic::PHSolidPairForHaptic(){
	solidID[0] = -1;
	solidID[1] = -1;
}
PHSolidPairForHaptic::PHSolidPairForHaptic(const PHSolidPairForHaptic& s){
	*this = s;
	const int height = s.shapePairs.height();
	const int width = s.shapePairs.width();
	shapePairs.resize(height, width);
	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			shapePairs.item(i, j) = DBG_NEW PHShapePairForHaptic(*s.shapePairs.item(i, j));
		}
	}
}


bool PHSolidPairForHaptic::Detect(engine_type* engine, unsigned int ct, double dt){
	return PHSolidPair::Detect(engine, ct, dt);
}

void PHSolidPairForHaptic::OnDetect(PHShapePairForHaptic* sp, PHHapticEngine* engine, unsigned ct, double dt){
	sp->intersectionVertices.clear();
	if(sp->state == sp->NEW || sp->state == sp->CONTINUE){
		sp->OnDetect(ct, solid[1]->GetCenterPosition());	// CCDGJK�ŋߖT�_�΂��Ď擾
		sp->AnalyzeContactRegion();		// �N���̈�̒��_���擾
	}else{
		// �ڐG���Ă��Ȃ��ꍇ�A�ߖT�_��N���̈�̒��_�Ƃ���
		sp->intersectionVertices.push_back(sp->closestPoint[1]);
	}

	// �͂��߂ċߖT�̏ꍇ
	if(inLocal == 1){
		for(int i = 0; i < 2; i++){
			sp->lastShapePoseW[i] = sp->shapePoseW[i];
			sp->lastClosestPoint[i] = sp->closestPoint[i];
		}
		sp->lastNormal = sp->normal;
	}	

	//CSVOUT << (sp->shapePoseW[0] * sp->closestPoint[0]).y << "," << (sp->shapePoseW[1] * sp->closestPoint[1]).y << std::endl;
}

PHIrs PHSolidPairForHaptic::CompIntermediateRepresentation(PHSolid* curSolid[2], double t, bool bInterpolatePose, bool bMultiPoints){
	/* �͊o���艻�̂��߂̕��
	// Impulse�̏ꍇ�͑���̍��̂�Pose�̕�Ԃ��K�v�B
	// LocalDynamics�̏ꍇ�͖@���̕�Ԃ݂̂ł悢�B
	// �@���̕�Ԃ�PHShapePairForHaptic�ł��B
	*/
	force.clear();
	torque.clear();
	lastInterpolationPose = interpolationPose;
	interpolationPose = curSolid[0]->GetPose();
	if(bInterpolatePose){
		interpolationPose = interpolate(t, curSolid[0]->GetLastPose(), curSolid[0]->GetPose());
	}

	// �ڐG�����Ƃ��Ė��C�v�Z�̂��߂̑��Έʒu���v�Z
	PHHapticPointer* pointer = DCAST(PHHapticPointer, curSolid[1]);
#if 1
	// ���Ζ��C
	if(frictionState == FREE){
		frictionState = FIRST;
		contactCount = 0;
		initialRelativePose =  pointer->GetPose() * interpolationPose.Inv();
	}else{
		frictionState = STATIC;
		contactCount += 1;
		initialRelativePose =  pointer->lastProxyPose * lastInterpolationPose.Inv();
	}
	relativePose = initialRelativePose * interpolationPose * pointer->GetPose().Inv();		
#else
	// ��Ζ��C
	if(frictionState == FREE){
		frictionState = STATIC;
		initialRelativePose = Posed();
	}else{
		initialRelativePose =  pointer->lastProxyPose * pointer->GetPose().Inv();
	}
	relativePose = initialRelativePose;
#endif

	//DSTR << "pose" << pointer->GetPose() << std::endl;
	//DSTR << "lastProxy" << pointer->lastProxyPose << std::endl;
	//DSTR << "ini" << initialRelativePose << std::endl;
	//DSTR << "relativePose" << relativePose << std::endl;

	// ���ԕ\���̍쐬
	PHIrs irs;
	for(int i = 0; i < curSolid[0]->NShape(); i++){
		for(int j = 0; j < curSolid[1]->NShape(); j++){
			PHShapePairForHaptic* sp = shapePairs.item(i, j);
			Posed curShapePoseW[2];
			curShapePoseW[0] = interpolationPose * curSolid[0]->GetShapePose(i);
			curShapePoseW[1] = curSolid[1]->GetPose() * curSolid[1]->GetShapePose(j);
			sp->CompIntermediateRepresentation(curShapePoseW, t, bInterpolatePose, bMultiPoints);
			for(int k = 0; k < (int)sp->irs.size(); k++){
				PHIr* ir = sp->irs[k];
				ir->solidID = solidID[0];
				ir->solidPair = this;
				ir->r = ir->pointerPointW - curSolid[1]->GetCenterPosition();
				ir->contactPointVel = curSolid[0]->GetPointVelocity(ir->contactPointW);
				ir->pointerPointVel = curSolid[1]->GetPointVelocity(ir->pointerPointW);	
			}
			if(pointer->bFriction) CompFrictionIntermediateRepresentation(sp);
			for(int k = 0; k < (int)sp->irs.size(); k++){
				irs.push_back(sp->irs[k]);
			}
		}
	}

	if(irs.size() == 0){
		// �ڐG�Ȃ�
		frictionState = FREE;
		initialRelativePose = Posed();
		relativePose = Posed();
	}
	return irs;
}

bool PHSolidPairForHaptic::CompFrictionIntermediateRepresentation(PHShapePairForHaptic* sp){
	// ���C
	int Nirs = sp->irs.size();
	if(Nirs == 0) return false;
	for(int i = 0; i < Nirs; i++){
		PHIr* ir = sp->irs[i];
		double mu = ir->mu;	// �����C�W��
				
		double l = mu * ir->depth;		// ���C�~�����a

		Vec3d vps = ir->pointerPointW;
		Vec3d vq = relativePose * ir->pointerPointW;
		Vec3d dq = (vq - vps) * ir->normal * ir->normal;
		Vec3d vqs = vq - dq;
		Vec3d tangent = vqs - vps;

		//DSTR << "vps" << vps << std::endl;
		//DSTR << "vq" << vq << std::endl;
		//DSTR << "tangent " << tangent << tangent.norm() << std::endl;

		PHIr* fricIr = DBG_NEW PHIr();
		*fricIr = *ir;
		double epsilon = 1e-5;
		if(tangent.norm() < epsilon){
			// �Î~���
			delete fricIr;
			//DSTR << "rest" << std::endl;
		}
		if(epsilon < tangent.norm() && tangent.norm() <= l){
			//�Ö��C�i�Î~���C���a���j
			fricIr->normal = tangent.unit();
			fricIr->depth = tangent.norm();
			sp->irs.push_back(fricIr);
			//DSTR << "static friction" << std::endl;
		}

		if(epsilon < l && l < tangent.norm()){
			// �����C
			fricIr->normal = tangent.unit();
			fricIr->depth = l;
			sp->irs.push_back(fricIr);
			//DSTR << "dynamic friction" << std::endl;
		}
	}
	return true;
}

bool PHSolidPairForHaptic::CompFrictionIntermediateRepresentation2(PHShapePairForHaptic* sp){
	// ���C

	int Nirs = sp->irs.size();
	if(Nirs == 0) return false;
	for(int i = 0; i < Nirs; i++){
		PHIr* ir = sp->irs[i];
		double mu = ir->mu;	// �����C�W��				
		double l = mu * ir->depth;		// ���C�~�����a

		Vec3d vps = ir->pointerPointW;
		Vec3d vq = relativePose * ir->pointerPointW;
		Vec3d dq = (vq - vps) * ir->normal * ir->normal;
		Vec3d vqs = vq - dq;
		Vec3d tangent = vqs - vps;

		//DSTR << "vps" << vps << std::endl;
		//DSTR << "vq" << vq << std::endl;
		//DSTR << "tangent " << tangent << tangent.norm() << std::endl;

		PHIr* fricIr = DBG_NEW PHIr();
		*fricIr = *ir;
		double epsilon = 1e-5;
		if(tangent.norm() < epsilon){
			// �Î~���
			delete fricIr;
			//DSTR << "rest" << std::endl;
		}
		if(epsilon < tangent.norm() && tangent.norm() <= l){
			//�Ö��C�i�Î~���C���a���j
			fricIr->normal = tangent.unit();
			fricIr->depth = tangent.norm();
			sp->irs.push_back(fricIr);
			//DSTR << "static friction" << std::endl;
		}

		if(epsilon < l && l < tangent.norm()){
			// �����C
			fricIr->normal = tangent.unit();
			fricIr->depth = l;
			sp->irs.push_back(fricIr);
			//DSTR << "dynamic friction" << std::endl;
		}
	}
	return true;
}
//----------------------------------------------------------------------------
// PHHapticEngineImp
double PHHapticEngineImp::GetPhysicsTimeStep(){
	return engine->GetScene()->GetTimeStep();
}
double PHHapticEngineImp::GetHapticTimeStep(){
	return engine->GetScene()->GetHapticTimeStep();
}
int PHHapticEngineImp::NHapticPointers(){
	return (int)engine->hapticPointers.size();
}
int PHHapticEngineImp::NHapticSolids(){
	return (int)engine->hapticSolids.size();
}
PHHapticPointer* PHHapticEngineImp::GetHapticPointer(int i){
	return engine->hapticPointers[i];
}
PHSolidForHaptic* PHHapticEngineImp::GetHapticSolid(int i){
	return engine->hapticSolids[i];
}
PHSolidPairForHaptic* PHHapticEngineImp::GetSolidPairForHaptic(int i, int j){
	return engine->solidPairs.item(i, j);
}
PHHapticPointers* PHHapticEngineImp::GetHapticPointers(){
	return &engine->hapticPointers;
}
PHSolidsForHaptic* PHHapticEngineImp::GetHapticSolids(){
	return &engine->hapticSolids;
}
PHSolidPairsForHaptic* PHHapticEngineImp::GetSolidPairsForHaptic(){
	return (PHSolidPairsForHaptic*)&engine->solidPairs;
}
PHHapticRender* PHHapticEngineImp::GetHapticRender(){
	return engine->hapticRender->Cast();
}
void PHHapticEngineImp::StepSimulation(){
	engine->GetScene()->Step();
}

//----------------------------------------------------------------------------
// PHHapticEngine
PHHapticEngineDesc::PHHapticEngineDesc(){

}

PHHapticEngine::PHHapticEngine(){
	bHapticEngine = false;
	bPhysicStep = true;
	engineImp = DBG_NEW PHHapticEngineImpulse();
	engineImp->engine = this;
	engineMode = MULTI_THREAD;
	engineImps.push_back(engineImp);
	hapticRender = DBG_NEW PHHapticRender();
}

void PHHapticEngine::SetHapticEngineMode(HapticEngineMode mode){
	engineMode = mode;
	switch(engineMode){
		case SINGLE_THREAD:		
			for(int i = 0; i < (int)engineImps.size(); i++){
				if(DCAST(PHHapticEngineSingleBase, engineImps[i])){
					engineImp = engineImps[i];
					return;
				}
			}
			engineImp = DBG_NEW PHHapticEngineSingleBase();
			break;		
		case MULTI_THREAD:		
			for(int i = 0; i < (int)engineImps.size(); i++){
				if(DCAST(PHHapticEngineImpulse, engineImps[i])){
					engineImp = engineImps[i];
					return;
				}
			}
			engineImp = DBG_NEW PHHapticEngineImpulse();
			break;
		case LOCAL_DYNAMICS:
			for(int i = 0; i < (int)engineImps.size(); i++){
				if(DCAST(PHHapticEngineLD, engineImps[i])){
					engineImp = engineImps[i];
					return;
				}
			}
			engineImp = DBG_NEW PHHapticEngineLD();
			break;
		default:
			assert(0);
			return;
	}
	engineImp->engine = this;
	engineImps.push_back(engineImp);
}

PHHapticRenderIf* PHHapticEngine::GetHapticRender(){ 
	return hapticRender->Cast(); 
}

void PHHapticEngine::UpdateHapticPointer(){};

void PHHapticEngine::StartDetection(){
	// AABB�̍X�V
	UpdateEdgeList();
	// �͊o�|�C���^���ƂɋߖT���̂�������
	for(int i = 0; i < (int)hapticPointers.size(); i++){
		Detect(hapticPointers[i]);
	}
	// ���[�J���ŃV�~�����[�V�������邩�ǂ����̃t���O��ݒ�
	for(int i = 0; i < (int)hapticSolids.size(); i++){
		PHSolidForHaptic* h = hapticSolids[i];
		if(DCAST(PHHapticPointer, h->sceneSolid)) continue;
		if(h->NLocal == 0 && h->NLocalFirst > 0){
			h->doSim = 1;			// �͂��߂ċߖT�ɂȂ���
		}else if(h->NLocal > 0){
			h->doSim = 2;			// �ߖT��Ԃ��p��
		}else{
			h->doSim = 0;			// �ߖT�łȂ�
		}
		h->NLocal = 0; 
		h->NLocalFirst = 0;
	}
}

void PHHapticEngine::UpdateEdgeList(){
	edges.clear();
	Vec3f dir[3] = { Vec3f(1, 0, 0), Vec3f(0, 1, 0), Vec3f(0, 0, 1) };
	for(int i = 0; i < (int)solids.size(); i++){
		Edge e;
		for(int j = 0; j < 3; j++){
			float min, max = 0.0f;
			solids[i]->GetBBoxSupport(dir[j], min, max);
			e.min[j] = min; e.max[j] = max;
		}
		edges.push_back(e);
	}
}

void PHHapticEngine::Detect(PHHapticPointer* pointer){
	int ct = GetScene()->GetCount();
	int dt = GetScene()->GetTimeStep();
	const int pointerSolidID = pointer->GetSolidID();
	Vec3f range3d = Vec3f(1.0, 1.0, 1.0) * pointer->GetLocalRange(); // ���o臒l
	// pointer��BBox��range3d�������g����
	edges[pointerSolidID].min -= range3d;
	edges[pointerSolidID].max += range3d;
	
	Vec3f pMin = edges[pointerSolidID].min;
	Vec3f pMax = edges[pointerSolidID].max;
	int N = hapticSolids.size();
	pointer->neighborSolidIDs.clear();
	for(int i = 0; i < N; i++){
		if(i == pointerSolidID) continue;
		Vec3f soMin = edges[i].min;
		Vec3f soMax = edges[i].max;
		int nAxes = 0;		// �����̎��Ō������Ă��邩�ǂ���
		for(int i = 0; i < 3; i++){
			int inAxis = 0;	// ���̎��Ō������Ă��邩�ǂ���
			// pointer��BBox����solid��BBox�������������
			if(pMin[i] <= soMin[i] && soMin[i] <= pMax[i]) inAxis++; 
			if(pMin[i] <= soMax[i] && soMax[i] <= pMax[i]) inAxis++; 
			// �\���b�h��Box���ɃN�G����BBox�������������
			if(soMin[i] <= pMin[i] && pMin[i] <= soMax[i]) inAxis++;
			if(soMin[i] <= pMax[i] && pMax[i] <= soMax[i]) inAxis++;
			// in��1�ȏ�Ȃ炻�̎��Ō���
			if(inAxis > 0) nAxes++;
#if 0
				DSTR << i << " pMin[i] = " << pMin[i] << "  soMin[i] = " << soMin[i] << "  pMax[i] = " << pMax[i] << std::endl;
				DSTR << i << " pMin[i] = "  << pMin[i] << "  soMax[i] = " << soMax[i] << "  pMax[i] = " << pMax[i] << std::endl;
				DSTR << i << " soMin[i] = " << soMin[i] << "  pMin[i] = " << pMin[i] << "  soMax[i] = " << soMax[i] << std::endl;
				DSTR << i << " soMin[i] = " << soMin[i] << "  pMax[i] = " << pMax[i] << "  soMax[i] = " << soMax[i] << std::endl;
		}
			DSTR << "nAxes" << nAxes <<  std::endl;
			DSTR << "------------------------" << std::endl;
#else
		}
#endif
#if 1
		// 2.�ߖT���̂Ɣ���
		const int pointerID = pointer->GetPointerID();
		PHSolidPairForHaptic* sp = solidPairs.item(i, pointerID);
		if(nAxes == 3){
			sp->Detect(this, ct, dt);	// �`�󖈂̋ߖT�_�T���A�ڐG���
			pointer->neighborSolidIDs.push_back(i);
			PHSolidForHaptic* h = hapticSolids[i];
			*h->GetLocalSolid() = *h->sceneSolid;	// �ߖT�Ɣ��肳�ꂽ�̂ŃR�s�[
			if(sp->inLocal == 0){
				// ���߂ċߖT�ɂȂ���
				sp->inLocal = 1;	
				h->NLocalFirst += 1;
			}else{
				// �p�����ċߖT�ł���
				sp->inLocal = 2;
				h->NLocal += 1;
			}
		}else{
			// �ߖT�łȂ�
			sp->inLocal = 0;
		}
#endif
	}
}

bool PHHapticEngine::AddChildObject(ObjectIf* o){
	PHSolid* s = DCAST(PHSolid, o);
	if(s && std::find(solids.begin(), solids.end(), s) == solids.end()){
		// solids��solid��ۑ�(pointer���܂܂��)
		solids.push_back(s);				
		PHSolidForHaptic* h = DBG_NEW PHSolidForHaptic();
		h->sceneSolid = s;
		*h->GetLocalSolid() = *s;
		hapticSolids.push_back(h);

		int NSolids = (int)solids.size();
		int NPointers = (int)hapticPointers.size();

		// PHSolidPairFoHaptic��ǉ� �s solid, �� pointer
		// PHSolid��PHHapticPointer�̏ꍇ
		// ���ǉ�
		PHHapticPointer* p = DCAST(PHHapticPointer, o);
		if(p){
			NPointers += 1;
			int pointerID = NPointers - 1;
			p->SetPointerID(pointerID);
			p->SetSolidID(NSolids - 1);
			hapticPointers.push_back(p);
			h->bPointer = true;
			solidPairs.resize(NSolids, NPointers);
			for(int i = 0; i < NSolids; i++){
				solidPairs.item(i, pointerID) = DBG_NEW PHSolidPairForHaptic();
				solidPairs.item(i, pointerID)->Init(solids[i], s);
				solidPairs.item(i, pointerID)->solidID[0] = i;
				solidPairs.item(i, pointerID)->solidID[1] = NSolids - 1;
			}
			if(s->NShape())	UpdateShapePairs(s);
		}

		// PHSolid�̏ꍇ
		// �s�̂ݒǉ�
		solidPairs.resize(NSolids, NPointers);
		for(int i = 0; i < NPointers; i++){
			solidPairs.item(NSolids - 1, i) = DBG_NEW PHSolidPairForHaptic();
			solidPairs.item(NSolids - 1, i)->Init(solids[NSolids - 1], hapticPointers[i]);	
			solidPairs.item(NSolids - 1, i)->solidID[0] = NSolids - 1;	
			solidPairs.item(NSolids - 1, i)->solidID[1] = hapticPointers[i]->GetSolidID();	
		}
		if(s->NShape())	UpdateShapePairs(s);
		return true;

		// �͊o�|�C���^���V�[���̐ڐG����؂�
		for(int i = 0; i < NPointers; i++){
			GetScene()->SetContactMode(hapticPointers[i]->Cast(), PHSceneDesc::MODE_NONE);
		}
	}
	return false;
}

// ������
bool PHHapticEngine::DelChildObject(ObjectIf* o){
	//PHSolid* s = DCAST(PHSolid, o);
	//if(s){
	//	PHSolids::iterator is = find(solids.begin(), solids.end(), s);
	//	if(is != solids.end() && solids.size() > 0){
	//		int idx = (int)(is - solids.begin());
	//		solids.erase(is);
	//		solidPairs.erase_col(idx);
	//		PHSolidsForHaptic::iterator it;
	//		for(it = hapticSolids.begin(); it < hapticSolids.end(); it++){
	//			if(DCAST(PHSolid, (*it)->sceneSolid) == s) hapticSolids.erase(it);
	//		}
	//		PHHapticPointer* p = DCAST(PHHapticPointer, o);
	//		if(p){
	//			PHHapticPointers::iterator ip = find(hapticPointers.begin(), hapticPointers.end(), p);
	//			if(ip != hapticPointers.end() && hapticPointers.size() > 0){
	//				hapticPointers.erase(ip);
	//			}
	//		}
	//	}
	//	return true;
	//}
	return false;
}

void PHHapticEngine::UpdateShapePairs(PHSolid* solid){
	PHSolids::iterator it = std::find(solids.begin(), solids.end(), solid);
	if(it == solids.end())
		return;
	int isolid = (int)(it - solids.begin());
	int i, j;
	PHSolidPairForHaptic* sp;
	PHSolid* s[2];
	// solid�̏ꍇ(�s�̍X�V�j
	for(i = 0; i < NHapticPointers(); i++){
		sp = solidPairs.item(isolid, i);
		s[0] = solid;
		s[1] = sp->solid[1];
		sp->shapePairs.resize(s[0]->NShape(), s[1]->NShape());
		for(j = 0; j < s[1]->NShape(); j++){
			PHShapePairForHaptic* n = DBG_NEW PHShapePairForHaptic();
			n->shape[0] = s[0]->GetShape(s[0]->NShape() - 1)->Cast();
			n->shape[1] = s[1]->GetShape(j)->Cast();
			sp->shapePairs.item(s[0]->NShape() - 1, j) = n;
		}
	}

	PHHapticPointer* pointer = DCAST(PHHapticPointer, solid);
	if(!pointer) return;
	// PHHapticPointer�̏ꍇ�i��̍X�V�j
	int pointerID = pointer->GetPointerID();
	int pointerSolidID = pointer->GetSolidID();
	for(i = 0; i < (int)solids.size(); i++){
		if(i == pointerSolidID) continue;
		sp = solidPairs.item(i, pointerID);
		s[0] = sp->solid[0];
		s[1] = solid;
		sp->shapePairs.resize(s[0]->NShape(), s[1]->NShape());
		for(j = 0; j < s[0]->NShape(); j++){
			PHShapePairForHaptic* n = DBG_NEW PHShapePairForHaptic();
			n->shape[0] = s[0]->GetShape(j)->Cast();
			n->shape[1] = s[1]->GetShape(s[1]->NShape()-1)->Cast(); 
			sp->shapePairs.item(j, s[1]->NShape()-1) = n;
		}
	}
}

} // namespace Spr