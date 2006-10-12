/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 *	@file SprGRSphere.h
 *	@brief ��
*/

/**	\addtogroup	gpGraphics	*/
//@{

#ifndef SPR_GRSphere_H
#define SPR_GRSphere_H

#include <Graphics/SprGRFrame.h>

namespace Spr{;

///	@brief �\���p��Sphere(GRSphere)�̃f�X�N���v�^�D
struct GRSphereDesc{
	float radius;			///< ���a
	int   slices;			///< �o�������̕�����
	int   stacks;			///< �ܐ������̕�����	
	GRSphereDesc(){
		radius = 1.0;
		slices = 16;
		stacks = 16;
	}
};


///	@brief	�O���t�B�b�N�X�Ŏg���\���p��Sphere
struct GRSphereIf: public GRVisualIf{
	IF_DEF(GRSphere);
};


//@}
}
#endif
