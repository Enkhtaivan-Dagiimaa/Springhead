rem .h ����C struct �̒�`�𔲂��o���D
rem bin/typedesc.exe �������o�����s���D
set OUT=TypeDescDumpImpForHandler.h
set HEADER=TypeDescDumpForHandler.h
set TARGET=FIOldSpringheadNode.h

set PATH=%SPRINGHEAD2%\bin
del %OUT%
del %HEADER%
echo //	Do not edit. MakeTypeDescForHandler.bat will update this file.> %OUT%
for %%f in (%TARGET%) do echo #include "%%f" >> %HEADER%

for %%f in (%TARGET%) do typedesc %%f >> %OUT%
