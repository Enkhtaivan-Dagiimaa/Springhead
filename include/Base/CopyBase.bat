if "%1"=="execute" goto execute
del *.h
set HEADERS=Env BaseDebug BaseUtility TVector TMatrix TinyVec TinyMat Affine TQuaternion TMatrixUtility
for %%f in (%HEADERS%) do call %0 execute %%f
goto end

:execute
echo //	���̃t�@�C����ҏW���Ȃ��ł��������DCopyBase.bat ���r���h�̓x�ɏ㏑�����܂��D > ..\..\include\Base\%2.h
echo //	�ҏW���K�v�ȏꍇ�́Csrc/Base/%2.h ��ҏW���Ă������� >> ..\..\include\Base\%2.h
copy ..\..\include\Base\%2.h+..\..\src\Base\%2.h ..\..\include\Base\%2.h

:end
