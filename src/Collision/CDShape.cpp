#include "Collision.h"
#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

namespace Spr{;

/*
IfInfo* CDShape_BASE_IF[] = {&ObjectIf::ifInfo, NULL};								
IfInfoImp<CDShapeIf> CDShapeIf::ifInfo = IfInfoImp<CDShapeIf>("cls", CDShape_BASE_IF);	
int IfInfoImp<CDShapeIf>::BaseOffset(){										
	CDShape* ptr = (CDShape*)0x10000000;													
	return (char*)(CDShapeIf*)ptr - (char*)(ObjectIf*)(Object*)ptr;					
}																					
*/

IF_IMP(CDShape, Object);

//----------------------------------------------------------------------------
//	CDShape
OBJECTIMPABST(CDShape, NamedObject);

}
