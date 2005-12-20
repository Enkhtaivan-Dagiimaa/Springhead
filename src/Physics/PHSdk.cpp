#include "Physics.h"

#pragma hdrstop
//	���������[�N�`�F�b�J
#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


namespace Spr{;

struct Sdks{
	typedef std::vector< PHSdkIf* > Cont;
	Cont* cont;

	Sdks(){
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		cont = DBG_NEW Cont;
	}
	~Sdks(){
		while(cont->size()){
			PHSdkIf* sdk = cont->back();
			cont->erase(cont->end()-1);
			sdk->DelRef();
			if (sdk->RefCount()==0){
				delete sdk;
			}
		}
		delete cont;
	}
};
static Sdks sdks;
PHSdkIf* _cdecl CreatePHSdk(){
	PHSdkIf* rv = DBG_NEW PHSdk;
	sdks.cont->push_back(rv);
	rv->AddRef();
	return rv;
}

//----------------------------------------------------------------------------
//	PHSdk
OBJECTIMP(PHSdk, Object);
PHSdk::~PHSdk(){
	for(Sdks::Cont::iterator it = sdks.cont->begin(); it != sdks.cont->end(); ++it){
		if (*it == this){
			sdks.cont->erase(it);
			DelRef();
			break;
		}
	}
}

PHSceneIf* PHSdk::CreateScene(){
	PHSceneIf* rv = DBG_NEW PHScene(this);
	scenes.push_back(rv);
	return rv;
}

}
