rem include/*/*.h ����C struct XXXDesc �� struct XXXState �̒�`�𔲂��o���āC
rem FIDesc.cpp�ɕ�����萔�̒�`�Ƃ��ď����o���D
rem bin/typedesc.exe �������o�����s���D

set PATH=%SPRINGHEAD2%\bin
del FIDesc.cpp
echo //	Don't edit. MakeTypeDesc.bat update this file.> FIDesc.cpp
for /D %%p in (../../include/*) do for %%f in (../../include/%%p/*.h) do typedesc <../../include/%%p/%%f >> FIDesc.cpp
