@echo off
if "%1"=="execute" goto execute
del ..\..\include\Base\*.h
set HEADERS=Env BaseDebug BaseUtility TVector TMatrix TinyVec TinyMat Affine TQuaternion TMatrixUtility BaseTypeInfo VectorDebug
for %%f in (%HEADERS%) do call %0 execute %%f
goto end

:execute
rem echo //	���̃t�@�C����ҏW���Ȃ��ł��������DCopyBase.bat ���r���h�̓x�ɏ㏑�����܂��D > ..\..\include\Base\tmp
rem echo //	�ҏW���K�v�ȏꍇ�́Csrc/Base/%2.h ��ҏW���Ă������� >> ..\..\include\Base\tmp
xcopy /D /Y ..\..\src\Base\%2.h ..\..\include\Base\

:end
