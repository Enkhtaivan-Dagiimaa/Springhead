cd Base
call CopyBase.bat
cd ..
set PATH=%SPRINGHEAD2%\BIN;%SPRINGHEAD2%\BIN\Graphviz\bin
mkdir html
doxygen springhead.doxy
:END
