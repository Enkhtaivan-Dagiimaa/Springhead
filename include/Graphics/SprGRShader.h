﻿/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_GRSHADER_H
#define SPR_GRSHADER_H

#include <Base/Affine.h>

namespace Spr{;

	
/**	
	@brief	シェーダフォーマット
	
	処理の流れ：\n
　　　 GRRender->SetShaderFormat() でシェーダおよびシェーダフォーマット指定						\n
　　　 GRRender->CreateShader() 後に、															\n
　　　 GRRender->GetShaderLocation() でシェーダフォーマットに対するロケーション情報を取得する．	\n
*/
///	@name ロケーション情報および
//@{		
struct SFBlendLocation{
	unsigned int uBlendMatrix;
	unsigned int aWeight;
	unsigned int aMatrixIndices;
	unsigned int aNumMatrix;
};	
//@}	

struct GRShaderFormat {

	///	@name シェーダタイプ
	//@{	
	enum ShaderType {
		shP3fB4f			= 0,
		shC4bP3fB4f			= 1,
		shC3fP3fB4f			= 2,
		shN3fP3fB4f			= 3,
		shC4fN3fP3fB4f		= 4,
		shT2fP3fB4f			= 5,
		shT2fC4bP3fB4f		= 6,
		shT2fN3fP3fB4f		= 7,
		shT2fC4fN3fP3fB4f	= 8
	};
	//@}	
		
};

}
#endif
