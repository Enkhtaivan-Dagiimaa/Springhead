#ifndef SPR_PHSDKIF_H
#define SPR_PHSDKIF_H
#include <Collision/SprCDShape.h>

namespace Spr{;

/** \addtogroup gpPhysics �����G���W���̊�b	*/
//@{

struct PHSceneIf;
struct PHSceneDesc;

//	�������[�h�̂��߂̒��g�Ȃ��̃f�X�N���v�^
struct PHSdkDesc{
};

///	�����V�~�����[�V����SDK
struct PHSdkIf : public NameManagerIf{
	IF_DEF(PHSdk);
	///	Scene�̍쐬
	virtual PHSceneIf* CreateScene()=0;
	///	Scene�̍쐬
	virtual PHSceneIf* CreateScene(const PHSceneDesc& desc)=0;
	///	Scene�̐����擾
	virtual int NScene()=0;
	///	Scene�̎擾
	virtual PHSceneIf** GetScenes()=0;
	
	///	Shape�쐬
	virtual CDShapeIf* CreateShape(const CDShapeDesc& desc)=0;
	///	Shape�̐�
	virtual int NShape()=0;
	///	Shape�̎擾
	virtual CDShapeIf* GetShape(int i)=0;
	//virtual CDShapeIf** GetShapes()=0;
};
PHSdkIf* SPR_CDECL CreatePHSdk();

//@}

}	//	namespace Spr
#endif
