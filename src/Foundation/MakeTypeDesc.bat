@echo off
rem include/����/*.h ����C struct �̒�`�𔲂��o���āCTypeDesc�̒�`�����D
rem bin/typedesc.exe �������o�����s���D
if "%1"=="LISTSRC" goto ListSrc
if "%1"=="EXECUTE" goto Execute

set OUT=TypeDescDumpImp.h
set HEADER=TypeDescDump.h
set TARGET=%1 %2 %3 %4 %5 %6 %7 %8 %9
set MAKEFILE=MakeTypeDescMAK.txt
cmd /v:on /c%0 LISTSRC
goto end

:ListSrc
rem makefile�̍쐬��Make�̎��s
set PATH=%SPRINGHEAD2%\bin
set SRC=
for %%p in (%TARGET%) do for %%f in (../../include/%%p/*.h) do set SRC=!SRC! ../../include/%%p/%%f
echo source files:%SRC%

echo #	Do not edit. MakeTypeDesc.bat will update this file.> %MAKEFILE%
echo all: %OUT%>>%MAKEFILE%
echo %OUT%: %SRC%>>%MAKEFILE%
echo 	MakeTypeDesc.bat EXECUTE>>%MAKEFILE%
make -f%MAKEFILE%
goto end


:Execute
rem Make���Ăяo���D

echo //	Do not edit. MakeTypeDesc.bat will update this file.> %OUT%
echo //	Do not edit. MakeTypeDesc.bat will update this file.> %HEADER%
for %%p in (%TARGET%) do for %%f in (../../include/%%p/*.h) do echo #include "../../include/%%p/%%f" >> %HEADER%

for %%p in (%TARGET%) do for %%f in (../../include/%%p/*.h) do typedesc ../../include/%%p/%%f >> %OUT%
if exist UTBaseType.h typedesc UTBaseType.h >> %OUT%
:end
