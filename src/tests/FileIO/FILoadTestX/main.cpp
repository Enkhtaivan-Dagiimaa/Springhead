/**
 Springhead2/src/tests/FileIO/FILoadTestX/main.cpp

�y�T�v�z
  �EX�t�@�C�������[�h���A���[�h�����f�[�^���o�͂���B
  
�y�I����z
  �E�v���O����������I��������0��Ԃ��B

�y�����̗���z
�@�Etest.x�t�@�C�������[�h����B
�@�E���[�h���������o�͂���B

 */
#include <FileIO/FITypeDesc.h>
#include <FileIO/FIFileContext.h>
#include <FileIO/FIFileX.h>
#include <Springhead.h>
namespace Spr{
extern void RegisterTypes();
extern UTRef<FITypeDescDb> typeDescDb;
}

int main(){
	using namespace Spr;
	RegisterTypes();
	FIFileContext fc;
	fc.fileInfo.Push();
	fc.fileInfo.back().Map("test.x");
	PHSdkIf* sdk = CreatePHSdk();
	fc.objects.Push(sdk);
	FIFileX fileX;
	fileX.Init(typeDescDb);
	fileX.Load(&fc);
	sdk->Print(DSTR);

//	Spr::typeDescDb->Print(DSTR);
}

