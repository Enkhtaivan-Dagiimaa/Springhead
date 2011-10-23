%module Base

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
#include "SprEPUtility.h"
#include "Utility/SprEPObject.h"
#include "Utility/SprEPCast.h"
#include <string>

using namespace std;

%}


namespace Spr{

%feature("not_newable","1");
	class IfInfo{};
	class ostream{};
	class istream{};
%feature("not_newable","");

}//namespace

%include "./Utility/PyPTM.i"