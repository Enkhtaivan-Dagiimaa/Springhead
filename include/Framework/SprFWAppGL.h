#ifndef SPR_FWAPPGL_H
#define SPR_FWAPPGL_H
#include <Framework/SprFWApp.h>

namespace Spr{;

/** OpenGL�̃R���e�L�X�g�쐬�@�\�����A�v���P�[�V�����N���X
 */
class FWAppGL : public FWApp{
public:
	/** @brief OpenGL���g�p���邽�߂̏��������������s���� 
	 */
	void CreateRender();
};


}

#endif
