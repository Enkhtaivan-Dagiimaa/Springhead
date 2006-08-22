@echo off
rem include/����/*.h ����C struct �̒�`�𔲂��o���āCTypeDesc�̒�`�����D
rem bin/typedesc.exe �������o�����s���D
if "%1"=="EXECUTE" goto Execute

set MAKEFILE=MakeTypeDescForOldSpringheadHandlerMAK.txt
set OUT=TypeDescDumpImpForOldSpringheadHandler.h
set HEADER=TypeDescDumpForOldSpringheadHandler.h
set TARGET=FWOldSpringheadNodeHandler.h

rem makefile�̍쐬��Make�̎��s
set PATHORG=%PATH%;
set PATH=%SPRINGHEAD2%\bin;%PATH%
echo source files:%TARGET%

echo #	Do not edit. MakeTypeDescForOldSpringheadHandler.bat will update this file.> %MAKEFILE%
echo all: %OUT%>>%MAKEFILE%
echo %OUT%: %TARGET%>>%MAKEFILE%
echo 	MakeTypeDescForOldSpringheadHandler.bat EXECUTE>>%MAKEFILE%
make -f%MAKEFILE%
set PATH=%PATHORG%
set PATHORG=

..\Foundation\MakeTypeDesc.bat Framework
goto end

:Execute
rem Make���Ăяo���D

echo //	Do not edit. MakeTypeDescForOldSpringheadHandler.bat will update this file.> %HEADER%
echo //	Do not edit. MakeTypeDescForOldSpringheadHandler.bat will update this file.> %OUT%
for %%f in (%TARGET%) do echo #include "%%f" >> %HEADER%
for %%f in (%TARGET%) do typedesc %%f >> %OUT%
:end
