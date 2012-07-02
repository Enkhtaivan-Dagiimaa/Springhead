﻿#include <Physics/PHHapticEngineImpulse.h>

namespace Spr{;

//----------------------------------------------------------------------------
// PHHapticLoopImpulse

// 1/7はここから
void PHHapticLoopImpulse::Step(){
	UpdateInterface();
	HapticRendering();
}

void PHHapticLoopImpulse::HapticRendering(){
	PHHapticRenderInfo info;
	info.pointers = GetHapticPointers();
	info.hsolids = GetHapticSolids();
	info.sps = GetSolidPairsForHaptic();
	info.hdt = GetHapticTimeStep();
	info.pdt = GetPhysicsTimeStep();
	info.loopCount = loopCount;
	info.bInterpolatePose = true;
	GetHapticRender()->HapticRendering(info);
}


//----------------------------------------------------------------------------
// PHHapticEngineImpulse
PHHapticEngineImpulse::PHHapticEngineImpulse(){ 
	hapticLoop = &hapticLoopImpulse;
	hapticLoop->engineImp = this;
}

void PHHapticEngineImpulse::Step1(){};
void PHHapticEngineImpulse::Step2(){
	for(int i = 0; i < NHapticSolids(); i++){
		PHSolid* solid = GetHapticSolid(i)->sceneSolid;
		//if(i == 1) //CSVOUT << solid->GetVelocity().y << "," << solid->GetFramePosition().y  << std::endl;
	}
	engine->StartDetection();
}

void PHHapticEngineImpulse::SyncHaptic2Physic(){
	// physics <------ haptic
	// PHSolidForHapticの同期
	// PHSolidPairForHaptic(力覚ポインタと近傍の物体)の各種情報の同期
	for(int i = 0; i < hapticLoop->NHapticPointers(); i++){
		PHHapticPointer* hpointer = hapticLoop->GetHapticPointer(i);
		int hpointerID = hpointer->GetPointerID();
		int nNeighbors = (int)hpointer->neighborSolidIDs.size();
		// 近傍物体であるペアだけ同期
		for(int j = 0; j < nNeighbors; j++){
			int solidID = hpointer->neighborSolidIDs[j];
			PHSolidPairForHaptic* hpair = hapticLoop->GetSolidPairForHaptic(solidID, hpointerID);
			PHSolidPairForHaptic* ppair = GetSolidPairForHaptic(solidID, hpointerID);
			PHSolidPairForHapticSt* hst = (PHSolidPairForHapticSt*)hpair;
			PHSolidPairForHapticSt* pst = (PHSolidPairForHapticSt*)ppair;
			*pst = *hst;	// haptic側で保持しておくべき情報を同期
		}
	}
	// レンダリングした力をシーンに反映
	for(int i = 0; i < (int)hapticLoop->NHapticSolids(); i++){
		PHSolidForHaptic* hsolid = hapticLoop->GetHapticSolid(i);
		if(hsolid->bPointer) continue;
		PHSolid* sceneSolid = hsolid->sceneSolid;
		sceneSolid->AddForce(hsolid->force * GetHapticTimeStep() / GetPhysicsTimeStep());
		sceneSolid->AddTorque(hsolid->torque * GetHapticTimeStep() / GetPhysicsTimeStep());
		hsolid->force.clear();
		hsolid->torque.clear();
	}
}

void PHHapticEngineImpulse::SyncPhysic2Haptic(){
	// haptic <------ physics
	// PHSolidForHapticの同期
	for(int i = 0; i < NHapticSolids(); i++){
		PHSolidForHaptic* psolid = GetHapticSolid(i);
		PHSolidForHaptic* hsolid = hapticLoop->GetHapticSolid(i);
		*psolid->GetLocalSolid() = *psolid->sceneSolid;	//	impulseの場合は常時sceneで管理されているsolidと同期				
		*hsolid = PHSolidForHaptic(*psolid);			// LocalDynamicsの場合はdosimによって同期情報をかえる必要がある
	}
	// solidpair, shapepairの同期
	// 近傍物体のみ同期させる
	for(int i = 0; i < NHapticPointers(); i++){
		PHHapticPointer* ppointer = GetHapticPointer(i);
		const int ppointerID = ppointer->GetPointerID();
		const int nNeighbors = (int)ppointer->neighborSolidIDs.size();
		for(int j = 0; j < nNeighbors; j++){
			const int solidID = ppointer->neighborSolidIDs[j];
			PHSolidPairForHaptic* hpair = hapticLoop->GetSolidPairForHaptic(solidID, ppointerID);
			PHSolidPairForHaptic* ppair = GetSolidPairForHaptic(solidID, ppointerID);
			*hpair = PHSolidPairForHaptic(*ppair);
		}
	}
}

}