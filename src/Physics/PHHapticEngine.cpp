#include <Physics/PHHapticEngine.h>
#include <Physics/PHHapticRenderImpulse.h>

namespace Spr{;


//----------------------------------------------------------------------------
// PHSapePairForHaptic
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
			intersectionVertices.push_back(shapePoseW[0].Inv() * point);
		}
	}
	// �ʂ�������Ȃ������Ashape��convex or box�ł͂Ȃ������ꍇ
	if(intersectionVertices.size() == 0){
		DSTR << "hoge" << std::endl;
		intersectionVertices.push_back(closestPoint[1]);
		return false;
	}
	return true;
}

//----------------------------------------------------------------------------
// PHSolidPairForHaptic

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
}

//----------------------------------------------------------------------------
// PHHapticRenderImp
double PHHapticRenderImp::GetPhysicsTimeStep(){
	return engine->GetScene()->GetTimeStep();
}
double PHHapticRenderImp::GetHapticTimeStep(){
	return engine->GetScene()->GetHapticTimeStep();
}
int PHHapticRenderImp::NHapticPointers(){
	return (int)engine->hapticPointers.size();
}
int PHHapticRenderImp::NHapticSolids(){
	return (int)engine->hapticSolids.size();
}
PHHapticPointer* PHHapticRenderImp::GetHapticPointer(int i){
	return engine->hapticPointers[i];
}
PHSolidForHaptic* PHHapticRenderImp::GetHapticSolid(int i){
	return engine->hapticSolids[i];
}
PHSolidPairForHaptic* PHHapticRenderImp::GetSolidPairForHaptic(int i, int j){
	return engine->solidPairs.item(i, j);
}
PHHapticPointers* PHHapticRenderImp::GetHapticPointers(){
	return &engine->hapticPointers;
}
PHSolidsForHaptic* PHHapticRenderImp::GetHapticSolids(){
	return &engine->hapticSolids;
}

//PHHapticPointers* PHHapticRenderImp::GetHapticPointers(){
//	return engine->hapticPointers.empty() ? NULL : (PHHapticPointers*)&*engine->hapticPointers.begin();
//}
//PHSolidForHaptic** PHHapticRenderImp::GetHapticSolids(){
//	return engine->hapticSolids.empty() ? NULL : (PHSolidForHaptic**)&*engine->hapticSolids.begin();
//}

//----------------------------------------------------------------------------
// PHHapticEngine
PHHapticEngineDesc::PHHapticEngineDesc(){
	bHaptic = false;
}

PHHapticEngine::PHHapticEngine(){
		bHaptic = false;
		renderImp = DBG_NEW PHHapticRenderImpulse();
		renderImp->engine = this;
		renderMode = NONE;
		renderImps.push_back(renderImp);
}

void PHHapticEngine::SetRenderMode(RenderMode r){
	renderMode = r;

	switch(renderMode){
		case NONE:
				DSTR << "You must set haptic render mode." << std::endl; 
			break;
		case IMPULSE:		
			for(int i = 0; i < (int)renderImps.size(); i++){
				if(DCAST(PHHapticRenderImpulse, renderImps[i])){
					renderImp = renderImps[i];
					return;
				}
			}
			renderImp = DBG_NEW PHHapticRenderImpulse();
			break;
		default:
				return;
			break;
	}
	renderImp->engine = this;
	renderImps.push_back(renderImp);
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
		// 2.�ߖT���̂Ɣ���
		const int pointerID = pointer->GetPointerID();
		PHSolidPairForHaptic* sp = solidPairs.item(i, pointerID);
		if(nAxes == 3){
			sp->Detect(this, ct, dt);	// �`�󖈂̋ߖT�_�T���A�ڐG���
			pointer->neighborSolidIDs.push_back(i);
			PHSolidForHaptic* h = hapticSolids[i];
			h->localSolid = *h->sceneSolid;	// �ߖT�Ɣ��肳�ꂽ�̂ŃR�s�[
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
	}
}

bool PHHapticEngine::AddChildObject(ObjectIf* o){
#if 0
	PHSolid* s = DCAST(PHSolid, o);
	if(s && std::find(solids.begin(), solids.end(), s) == solids.end()){
		solids.push_back(s);				// solids��solid��ۑ�(pointer���܂܂��)
		PHSolidForHaptic* h = DBG_NEW PHSolidForHaptic();
		h->sceneSolid = s;
		h->localSolid = *s;
		hapticSolids.push_back(h);

		int N = (int)solids.size();	
		assert(solidPairs.height() == N-1 && solidPairs.width() == N-1);
		solidPairs.resize(N, N);
		for(int i = 0; i < N-1; i++){
			solidPairs.item(i, N-1) = DBG_NEW PHSolidPairForHaptic();
			solidPairs.item(i, N-1)->Init(solids[i], solids[N-1]);	
		}
		if(s->NShape())	UpdateShapePairs(s);

		// PHSolid��PHHapticPointer�̏ꍇ
		PHHapticPointer* p = DCAST(PHHapticPointer, o);
		if(p){
			p->SetID(N-1);
			hapticPointers.push_back(p);
			h->bPointer = true;
		}
		return true;
	}
	return false;
#else
	PHSolid* s = DCAST(PHSolid, o);
	if(s && std::find(solids.begin(), solids.end(), s) == solids.end()){
		// solids��solid��ۑ�(pointer���܂܂��)
		solids.push_back(s);				
		PHSolidForHaptic* h = DBG_NEW PHSolidForHaptic();
		h->sceneSolid = s;
		h->localSolid = *s;
		hapticSolids.push_back(h);

		int NSolids = (int)solids.size();
		int NPointers = (int)hapticPointers.size();

		// PHSolidPairFoHaptic��ǉ� �s solid, �� pointer
		// PHSolid��PHHapticPointer�̏ꍇ
		// �s�C���ǉ�
		PHHapticPointer* p = DCAST(PHHapticPointer, o);
		if(p){
			NPointers += 1;
			p->SetPointerID(NPointers - 1);
			p->SetSolidID(NSolids - 1);
			hapticPointers.push_back(p);
			h->bPointer = true;
			solidPairs.resize(NSolids, NPointers);
			for(int i = 0; i < NSolids; i++){
				solidPairs.item(i, NPointers - 1) = DBG_NEW PHSolidPairForHaptic();
				solidPairs.item(i, NPointers - 1)->Init(solids[i], s);
				solidPairs.item(i, NPointers - 1)->pointer = p;
			}
			if(s->NShape())	UpdateShapePairs(s);
			return true;
		}

		// PHSolid�̏ꍇ
		// �s�̂ݒǉ�
		solidPairs.resize(NSolids, NPointers);
		for(int i = 0; i < NPointers; i++){
			solidPairs.item(NSolids - 1, i) = DBG_NEW PHSolidPairForHaptic();
			solidPairs.item(NSolids - 1, i)->Init(solids[NSolids - 1], hapticPointers[i]);	
		}
		if(s->NShape())	UpdateShapePairs(s);
		return true;
	}
	return false;
#endif
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
	PHSolid *slhs, *srhs;
	// solid�̏ꍇ(�s�̍X�V�j
	for(i = 0; i < NHapticPointers(); i++){
		sp = solidPairs.item(isolid, i);
		slhs = solid;
		srhs = sp->solid[1];
		sp->shapePairs.resize(slhs->NShape(), srhs->NShape());
		for(j = 0; j < slhs->NShape(); j++){
			PHShapePairForHaptic* n = DBG_NEW PHShapePairForHaptic();
			n->shape[0] = slhs->GetShape(j)->Cast();
			n->shape[1] = srhs->GetShape(srhs->NShape()-1)->Cast();
			sp->shapePairs.item(j, srhs->NShape()-1) = n;
		}
	}

	PHHapticPointer* pointer = DCAST(PHHapticPointer, solid);
	if(!pointer) return;
	// PHHapticPointer�̏ꍇ�i��̍X�V�j
	int pointerID = pointer->GetPointerID();
	for(i = 0; i < (int)solids.size(); i++){
		sp = solidPairs.item(i, pointerID);
		slhs = sp->solid[0];
		srhs = solid;
		sp->shapePairs.resize(slhs->NShape(), srhs->NShape());
		for(j = 0; j < slhs->NShape(); j++){
			PHShapePairForHaptic* n = DBG_NEW PHShapePairForHaptic();
			n->shape[0] = slhs->GetShape(j)->Cast();
			n->shape[1] = srhs->GetShape(srhs->NShape()-1)->Cast();
			sp->shapePairs.item(j, srhs->NShape()-1) = n;
		}
	}
}

} // namespace Spr