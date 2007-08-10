/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 *	@file GRDebugRenderD3D.h
 *	@brief DirectX�̃f�o�b�O��񃌃��_���[�i���́A�ʁj�@�@
*/
#ifndef GRDEBUGRENDERD3D_H
#define GRDEBUGRENDERD3D_H

#include "GraphicsD3D.h"

namespace Spr{;

/**	@class	GRDebugRenderD3D
    @brief	DirectX�̃f�o�b�O��񃌃��_���[�̎����@ */
class GRDebugRenderD3D:public GRDebugRender, public GRDebugRenderD3DIfInit{
	OBJECTDEF(GRDebugRenderD3D, GRDebugRender);
public:
	/**  �V�[�����̑S�ẴI�u�W�F�N�g�������_�����O����
	     @param  scene		�V�[��  */
	void DrawScene(PHSceneIf* scene);
	/**  ���̂������_�����O����
	     @param	so�@�@�@	����  */
	void DrawSolid(PHSolidIf* so);
};

}
#endif
