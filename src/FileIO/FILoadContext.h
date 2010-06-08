/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef FILOADCONTEXT_H
#define FILOADCONTEXT_H

#include <Foundation/UTLoadContext.h>
#include <Foundation/UTLoadHandler.h>

namespace Spr{;

class FISdk;
class Import;

///	FILoadContext
class FILoadContext: public UTLoadContext{
public:
	UTStack<Import*>		importStack;	///< �C���|�[�g�X�^�b�N
	FISdk*					sdk;			///< FISdk�ւ̎Q�Ɓi�C���|�[�g����FIFile�I�u�W�F�N�g�쐬�p�j

	virtual void PushFileMap(const UTString fn, bool binary);
	virtual void PopFileMap();
	virtual ~FILoadContext(){}
};

}
#endif
