@echo off

if "%1"=="LISTSRC" goto ListSrc
cmd /v:on /c%0 LISTSRC %1 %2 %3 %4 %5 %6 %7 %8 %9
goto end

:ListSrc
set TARGET=%9 %8 %7 %6 %5 %4 %3 %2
set MODULE=%2
set MAKEFILE=%MODULE%Stub.mak.txt
rem makefile�̍쐬��Make�̎��s
set PATHORG=%PATH%;


rem Create SJIS World
set PATH=..\..\bin;
if not exist ..\..\swigtemp mkdir ..\..\swigtemp
if not exist ..\..\swigtemp\src mkdir ..\..\swigtemp\src
if not exist ..\..\swigtemp\include mkdir ..\..\swigtemp\include
if not exist ..\..\swigtemp\src\Foundation mkdir ..\..\swigtemp\src\Foundation
if not exist ..\..\swigtemp\include\Base mkdir ..\..\swigtemp\include\Base

for %%p in (%TARGET%) do if not exist ..\..\swigtemp\src\%%p mkdir ..\..\swigtemp\src\%%p
for %%p in (%TARGET%) do if not exist ..\..\swigtemp\include\%%p mkdir ..\..\swigtemp\include\%%p

for %%p in (%TARGET%) do for %%f in (..\..\include\%%p\*.h) do nkf -s < ..\..\include\%%p\%%f > ..\..\swigtemp\include\%%p\%%f
for %%p in (%TARGET%) do for %%f in (..\%%p\*.h) do nkf -s < ..\%%p\%%f > ..\..\swigtemp\src\%%p\%%f
rem for %%p in (%TARGET%) do for %%f in (..\%%p\*.i) do nkf -s < ..\%%p\%%f > ..\..\swigtemp\src\%%p\%%f
nkf -s < ..\..\include\Springhead.h > ..\..\swigtemp\include\Springhead.h
nkf -s < ..\..\include\base\Env.h > ..\..\swigtemp\include\base\Env.h
nkf -s < ..\..\include\Base\BaseDebug.h > ..\..\swigtemp\include\Base\BaseDebug.h
nkf -s < ..\..\src\Foundation\UTTypeDesc.h > ..\..\swigtemp\src\Foundation\UTTypeDesc.h

cd ..\..\swigtemp\src\Foundation
set PATH=..\..\..\bin;..\..\..\bin\swig


set SRCINTF=
for %%p in (%TARGET%) do for %%f in (../../include/%%p/*.h) do set SRCINTF=!SRCINTF! ../../include/%%p/%%f
set SRCIMP=
for %%p in (%TARGET%) do for %%f in (../%%p/*.h) do set SRCIMP=!SRCIMP! ../%%p/%%f

rem echo interface files:%SRCINTF%
rem echo src files:%SRCIMP%

echo #	Do not edit. RunSwig.bat will update this file.> %MODULE%.i
echo %%module %MODULE%>> %MODULE%.i
echo #define DOUBLECOLON :: >> %MODULE%.i
echo %%include "../../include/Springhead.h">> %MODULE%.i
echo %%include "../../include/base/Env.h">> %MODULE%.i
for %%p in (%SRCINTF%) do echo %%include "%%p">> %MODULE%.i
echo %%include "../../include/Base/BaseDebug.h">> %MODULE%.i
echo %%include "../../src/Foundation/UTTypeDesc.h">> %MODULE%.i
for %%p in (%SRCIMP%) do echo %%include "%%p">> %MODULE%.i

echo #	Do not edit. RunSwig.bat will update this file.> %MAKEFILE%
echo all: %MODULE%Stub.cpp>>%MAKEFILE%
echo %MODULE%Stub.cpp: %SRCINTF% %SRCIMP%>>%MAKEFILE%
echo 	swig -spr -w312,319,325,401,402 -DSWIG_%MODULE% -c++ %MODULE%.i>>%MAKEFILE%
echo 	copy Spr%MODULE%Decl.hpp ..\..\..\include\%MODULE% >>%MAKEFILE%
echo 	copy %MODULE%Stub.cpp ..\..\..\src\%MODULE% >>%MAKEFILE%
echo 	copy %MODULE%Decl.hpp ..\..\..\src\%MODULE% >>%MAKEFILE%
make -f%MAKEFILE%
set PATH=%PATHORG%
set PATHORG=
:end
