@echo off
rem include/����/*.h ����C struct �̒�`�𔲂��o���āCTypeDesc�̒�`�����D
rem bin/typedesc.exe �������o�����s���D
if "%1"=="EXECUTE" goto Execute

set MAKEFILE=MakeTypeDescForHandlerMAK.txt
set OUT=TypeDescDumpImpForHandler.h
set HEADER=TypeDescDumpForHandler.h
set TARGET=FIOldSpringheadNode.h

rem makefile�̍쐬��Make�̎��s
set PATHORG=%PATH%;
set PATH=%SPRINGHEAD2%\bin;%PATH%
echo source files:%TARGET%

echo #	Do not edit. MakeTypeDescForHandler.bat will update this file.> %MAKEFILE%
echo all: %OUT%>>%MAKEFILE%
echo %OUT%: %TARGET%>>%MAKEFILE%
echo 	MakeTypeDescForHandler.bat EXECUTE>>%MAKEFILE%
make -f%MAKEFILE%
set PATH=%PATHORG%
set PATHORG=

..\Foundation\MakeTypeDesc.bat FileIO
goto end


:Execute
rem Make���Ăяo���D

echo //	Do not edit. MakeTypeDescForHandler.bat will update this file.> %HEADER%
echo //	Do not edit. MakeTypeDescForHandler.bat will update this file.> %OUT%
for %%f in (%TARGET%) do echo #include "%%f" >> %HEADER%
for %%f in (%TARGET%) do typedesc %%f >> %OUT%
:end
