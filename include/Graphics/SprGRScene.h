/**
 *	@file SprGRScene.h
 *	@brief �V�[���O���t
*/

/**	\addtogroup	gpGraphics	*/
//@{

#ifndef SPR_GRScene_H
#define SPR_GRScene_H

#include <Springhead.h>
#include <Graphics/SprGRFrame.h>

namespace Spr{;

///	@brief GRScene ��Desc�D���g�����D
struct GRSceneDesc{
};

struct GRSdkIf;
/**	@brief	�O���t�B�b�N�X�V�[���O���t */
struct GRSceneIf: public SceneIf{
	IF_DEF(GRScene);
	///	�t���[���̍쐬
	virtual GRFrameIf* CreateFrame(const GRFrameDesc& desc)=0;
	///	
	virtual void Render(GRRenderIf* r)=0;
	/** @brief ����Scene������SDK��Ԃ�
		@return SDK�̃C���^�t�F�[�X
	*/	
	virtual GRSdkIf* GetSdk()=0;
};


//@}
}
#endif
