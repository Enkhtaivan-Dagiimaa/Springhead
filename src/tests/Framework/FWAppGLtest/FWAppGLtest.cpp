// FWAppGLtest.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include <tchar.h>
#include <Springhead.h>
#include <SprFramework.h>

namespace Spr{
	void PHRegisterTypeDescs();
	void CDRegisterTypeDescs();
	void GRRegisterTypeDescs();
	void FIRegisterTypeDescs();
	void FWRegisterTypeDescs();

	void FWRegisterFactories();
}

using namespace Spr;

int _tmain(int argc, _TCHAR* argv[])
{
	PHRegisterTypeDescs();
	CDRegisterTypeDescs();
	GRRegisterTypeDescs();
	FIRegisterTypeDescs();
	FWRegisterTypeDescs();

	FWRegisterFactories();

	FWAppGLIf* fwApp = CreateFWAppGL();
	fwApp->StartApp("test.x", 200);

	return 0;
}

