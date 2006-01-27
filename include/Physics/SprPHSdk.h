#ifndef SPR_PHSDKIF_H
#define SPR_PHSDKIF_H
#include <Collision/SprCDShape.h>

namespace Spr{;

struct PHSceneIf;
///	�����V�~�����[�V����SDK
struct PHSdkIf : public ObjectIf{
	IF_DEF(PHSdk);
	///	Scene�̍쐬
	virtual PHSceneIf* CreateScene()=0;
	///	Scene�̐����擾
	virtual int GetNScene()=0;
	///	Scene�̎擾
	virtual PHSceneIf** GetScenes()=0;
	
	///	Shape�쐬
	virtual CDShapeIf* CreateShape(const CDShapeDesc& desc)=0;
	///	Shape�̐�
	virtual int GetNShape()=0;
	///	Shape�̎擾
	virtual CDShapeIf** GetShapes()=0;
};
PHSdkIf* _cdecl CreatePHSdk();

}	//	namespace Spr
#endif
