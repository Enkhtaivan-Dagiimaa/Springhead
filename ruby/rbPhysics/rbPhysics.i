%module rbPhysics

%{
// win32/win32.h��write��read���}�N����`����Ă��āC���ꂪstd�ƏՓ˂���
#undef write
#undef read
#include <SprPhysics.h>
using namespace Spr;
%}

// typemaps
%include <sprtypemaps.i>

//bool�̃f�t�H���g����true or false
//enum�^������or�߂�l
//Posed&
//Vec3d&
//double&
//const Vec3d&
//�߂�l��Vec3d
//GetSolids�̖߂�l��PHSolid**
//enum contactmode�̃f�t�H���g����
//setcontactmode�̈�����PHSolid**
//������Vec3f
//Vec3d�Ȃǂ̃����o�ϐ���get/set

%import	 <Base/Env.h>			// SPR_CDECL

%include <Foundation/SprObject.h>
%include <Foundation/SprScene.h>
%include "Physics/SprPHSdk.h"
%include "Physics/SprPHScene.h"
%include "Physics/SprPHSolid.h"
%include "Physics/SprPHJoint.h"
%include "Physics/SprPHEngine.h"
