﻿/*
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
	//	3次元テクスチャ使用枚数
	//unsigned kogePics;		// 焦げ表示切り替えテクスチャ(画像)枚数
	//unsigned thermoPics;	// 温度表示切り替えテクスチャ
	//unsigned waterPics;		// 水分情報切り替えテクスチャ
	//FWFemMeshDesc(){Init();}
	//void Init(){
	//	kogePics	= 5;
	//	thermoPics	= 6;
	//	waterPics	= 2;
	//}
};

/** 
 */
struct FWFemMeshIf : FWObjectIf {
	SPR_IFDEF(FWFemMesh);
	PHFemMeshIf* GetPHMesh();
	void DrawIHBorderXZPlane(bool sw);		// FEMMeshにXZ平面を表示：使うとき：swにtrue、使わない：false
	void DrawFaceEdge();	//	メッシュのエッジ境界を描画
	void SetTexmode(unsigned mode);
};


}

#endif
