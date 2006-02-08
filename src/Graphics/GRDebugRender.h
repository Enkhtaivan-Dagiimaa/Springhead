/**
  @file GRDebugRender.h
  @brief �f�o�b�O��񃌃��_���[�i���́A�ʁj�@�@
*/
#ifndef GRDEBUGRENDER_H
#define GRDEBUGRENDER_H

#include <SprGraphics.h>

namespace Spr{;

/**	@class	GRDebugRender
    @brief	�f�o�b�O��񃌃��_���[�̎����@ */
class GRDebugRender:public GRRender, public GRDebugRenderIf{
	OBJECTDEF(GRDebugRender);
	BASEIMP_GRRENDER(GRRender);
	/**	Viewport�Ǝˉe�s���ݒ�
		@param	screen		�E�B���h�E�T�C�Y */
	void Reshape(Vec2f screen);
	/** ���̂������_�����O����
	    @param	so�@�@�@	���� */
	void DrawSolid(PHSolidIf* so);
	/** �ʂ������_�����O������
		@param	face�@�@�@ ��  
		@param	base�@�@�@ �ʌ`��̒��_�Q */
	void DrawFace(CDFaceIf* face, Vec3f * base);
	/** �f�o�b�O�p��񃌃|�[�g */
	void Print(std::ostream& os) const{ GRRender::Print(os); }
};

}
#endif
