rem include/*/*.h ����C struct �̒�`�𔲂��o���āC
rem FIDesc.cpp��TypeDesc�̒�`�����D�D
rem bin/typedesc.exe �������o�����s���D
set OUT=TypeDescDump.cpp
set HEADER=TypeDescDump.h

set PATH=%SPRINGHEAD2%\bin
del %OUT%
del %HEADER%
echo //	Do not edit. MakeTypeDesc.bat will update this file.> %OUT%
for /D %%p in (../../include/*) do for %%f in (../../include/%%p/*.h) do echo #include "../../include/%%p/%%f" >> %HEADER%
echo #include "../Collision/Collision.h" >> %HEADER%
echo #include "../Foundation/Foundation.h" >> %HEADER%
echo #include "../Physics/Physics.h" >> %HEADER%
echo #include "../Graphics/Graphics.h" >> %HEADER%


for /D %%p in (../../include/*) do for %%f in (../../include/%%p/*.h) do typedesc ../../include/%%p/%%f >> %OUT%
typedesc FIBaseType.h >> %OUT%
