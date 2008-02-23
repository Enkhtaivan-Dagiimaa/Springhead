/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SBASE_H
#define SBASE_H
/**	@page Base Base���C�u����
	<a href = "../../html/index.html" target=_top> Springhead </a> �̒��ŁC
	���Ɋ�{�I�ȃ��[�e�B���e�B�����������W�߁C
	Base���C�u�����Ƃ��܂����DBase���C�u�����́C
	- ���w�E�x�N�g���E�s�񃉃C�u����
		- @ref pgPTM n�~m�s��En�����x�N�g�� (PTM::TMatrix, PTM::TVector)
		- @ref pgAffine �A�t�B���s��E2/3�����x�N�g�� (Spr::TVec2, Spr::TVec3, Spr::TAffine)
	- rtti���g��Ȃ����s���^��� (BaseTypeInfo.h)
	- �f�o�b�O�x���@�\ (BaseDebug.h)
	- �R���p�C���Ή��}�N�� (Env.h)
	- @ref gpBaseUtility ���̑��̃��[�e�B���e�B�[ (BaseUtility.h)
		- @ref gpUTRef

����\������܂��D
*/

namespace Spr{}
#include "Env.h"
#include "BaseDebug.h"
#include "BaseUtility.h"
#include "BaseTypeInfo.h"
#include "StrBufBase.h"
#include "TVector.h"
#include "TMatrix.h"
#include "TinyVec.h"
#include "TinyMat.h"
#include "TQuaternion.h"
#include "Affine.h"

#endif
