/**
 *	@file GRDebugRender.h
 *	@brief �f�o�b�O��񃌃��_���[�i���́A�ʁj�@�@
*/
#ifndef GRDEBUGRENDER_H
#define GRDEBUGRENDER_H

#include <SprGraphics.h>

namespace Spr{;

/**	@class	GRDebugRender
    @brief	�f�o�b�O��񃌃��_���[�̎����@ */
class GRDebugRender:public InheritGRRender<GRDebugRenderIf, GRRender>{
	OBJECT_DEF(GRDebugRender);
protected:
	int matSampleCount;
	std::vector<GRMaterialDesc> matSample;		/// �����_���[�ŗp�ӂ��Ă���ގ�(24���)
public:
	/**  �R���X�g���N�^  */
	GRDebugRender();
	/**  �V�[�����̑S�ẴI�u�W�F�N�g�������_�����O����
	     @param  scene		�V�[��  */
	void DrawScene(PHSceneIf* scene);
	/**  ���̂������_�����O����
	     @param	so�@�@�@	����  */
	void DrawSolid(PHSolidIf* so);
	/**  �ʂ������_�����O������
		 @param	face�@�@�@	��  
		 @param	base�@�@�@	�ʌ`��̒��_�Q  */
	void DrawFace(CDFaceIf* face, Vec3f * base);
	/**  �w�肵���}�e���A�������蓖�Ă�
	     @param mat			�}�e���A���T���v��  */
	void SetMaterialSample(MaterialSample matname);
};

}
#endif
