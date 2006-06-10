/**	\addtogroup	gpGraphics	*/
//@{

#ifndef SPR_GRFrame_H
#define SPR_GRFrame_H
#include <SprFoundation.h>

namespace Spr{;

struct GRRenderIf;
/**	@brief	�O���t�B�b�N�X�ŕ\���ɉe����^�������	*/
struct GRVisualIf: public NamedObjectIf{
	IF_DEF(GRVisual);
	///	�����_�����O�D�q�m�[�h�C��m�[�h�̃����_�����O���O�ɌĂ΂��D
	virtual void Render(GRRenderIf* r)=0;
	///	�����_�����O�I�������D�q�m�[�h�C��m�[�h�̃����_�����O���I����Ă���Ă΂��D
	virtual void Rendered(GRRenderIf* r)=0;
};
	
	
///	DirectX �� Frame �̕ϊ���\�� FrameTransformMatrix �m�[�h��ǂނ��߂�Desc
struct GRFrameTransformMatrix{
	Affinef transform;
};

///	@brief GRFrame ��Desc�D���W�n���w�肷��
struct GRFrameDesc{
	Affinef transform;
};
	
/**	@brief	�O���t�B�b�N�X�V�[���O���t�̃c���[�̃m�[�h�D���W�n�����D*/
struct GRFrameIf: public GRVisualIf{
	IF_DEF(GRFrame);
	virtual GRFrameIf* GetParent()=0;
	virtual void SetParent(GRFrameIf* fr)=0;
	virtual Affinef GetWorldTransform()=0;
};

//@}
}
#endif
