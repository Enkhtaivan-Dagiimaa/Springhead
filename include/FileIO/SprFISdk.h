#ifndef SPR_FISDKIF_H
#define SPR_FISDKIF_H
#include <Foundation/SprObject.h>

namespace Spr{;


/** \addtogroup gpFileIO	*/
//@{

struct FIFileXIf;

///	�t�@�C�����o��SDK
struct FISdkIf : public ObjectIf{
	IF_DEF(FISdk);
	virtual FIFileXIf* CreateFileX()=0;
	virtual void Clear()=0;
};
///	�t�@�C�����o��SDK�̍쐬
FISdkIf* SPR_CDECL CreateFISdk();
///	���낢���SDK�̍쐬
ObjectIf* SPR_CDECL CreateSdk(const IfInfo* info, const void* desc);

//@}

}	//	namespace Spr
#endif
