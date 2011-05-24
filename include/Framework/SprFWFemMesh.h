/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_FWFEMMESHIF_H
#define SPR_FWFEMMESHIF_H

#include <Framework/SprFWObject.h>


namespace Spr{;

struct FWFemMeshDesc: public FWObjectDesc{
};

/** @brief Framework�̃I�u�W�F�N�g
	���́iPhysics���W���[����PHSolid�j�ƃV�[���O���t�̃t���[���iGraphics���W���[����GRFrame�j��
	�֘A�t����I�u�W�F�N�g�D
	����ɂ���ăV�~�����[�V�����ɂ�鍄�̂̈ʒu�̕ω����V�[���O���t�Ɏ����I�ɔ��f�����D
	Framework�𗘗p����ꍇ��PHSceneIf::CreateSolid��GRSceneIf::CreateFrame�̑����
	FWSceneIf::CreateObject���g�p����D
 */
struct FWFemMeshIf : FWObjectIf {
	SPR_IFDEF(FWFemMesh);
};


}

#endif
