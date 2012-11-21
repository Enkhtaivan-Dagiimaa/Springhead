/*
 *  Copyright (c) 2003-2012, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_FW_FEMMESH_NEW_IF_H
#define SPR_FW_FEMMESH_NEW_IF_H

#include <Framework/SprFWObject.h>
#include <Physics/SprPHFemMeshNew.h>

namespace Spr{;

struct FWFemMeshNewDesc: public FWObjectDesc{
};

/** 
 */
struct FWFemMeshNewIf : FWObjectIf{
	SPR_IFDEF(FWFemMeshNew);
	PHFemMeshNewIf* GetPHFemMesh();
	//void DrawIHBorderXZPlane(bool sw);		// FEMMesh��XZ���ʂ�\���F�g���Ƃ��Fsw��true�A�g��Ȃ��Ffalse
	void DrawFaceEdge();	//	���b�V���̃G�b�W���E��`��
};


}

#endif
