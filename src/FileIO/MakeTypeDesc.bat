rem include/*/*.h ����C struct �̒�`�𔲂��o���āC
rem FIDesc.cpp��TypeDesc�̒�`�����D�D
rem bin/typedesc.exe �������o�����s���D
set OUT=TypeDescDumpImp.h
set HEADER=TypeDescDump.h
set TARGET=Collision Foundation Graphics Physics FileIO Creature

set PATH=%SPRINGHEAD2%\bin
del %OUT%
del %HEADER%
echo //	Do not edit. MakeTypeDesc.bat will update this file.> %OUT%
for %%p in (%TARGET%) do for %%f in (../../include/%%p/*.h) do echo #include "../../include/%%p/%%f" >> %HEADER%

for %%p in (%TARGET%) do for %%f in (../../include/%%p/*.h) do typedesc ../../include/%%p/%%f >> %OUT%
typedesc FIBaseType.h >> %OUT%
