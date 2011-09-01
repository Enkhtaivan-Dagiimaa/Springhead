%module Utility

%begin %{
#include "Base/TQuaternion.h"
#include "Base/Affine.h"

#ifdef SWIG
#define __EPDECL
#else
#if _DEBUG
#define __EPDECL __cdecl
#else
#define __EPDECL __fastcall
#endif
#endif

//swig
#include "Utility/SprEPFoundation.h"
#include "Utility/SprEPObject.h"
#include "Utility/SprEPvoid.h"
#include "Utility/SprEPCast.h"
#include "Utility/UTCriticalSection.h"//CriticalSection�p
#include <string>

using namespace std;
%}


namespace Spr{

%feature("not_newable","1");
	class IfInfo{};
	class ostream{};
	class istream{};
%feature("not_newable","");

	//�N���e�B�J���Z�N�V����
	UTCriticalSection EPCriticalSection;

}//namespace

%include "./Utility/PyPTM-sprpy.i"