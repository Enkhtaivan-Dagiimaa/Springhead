call CopyBase.bat
set PATHORG=%PATH%
set PATH=%SPRINGHEAD2%\BIN;%SPRINGHEAD2%\BIN\Graphviz\bin;%PATH%
doxygen base.doxy 2>base.log
set PATH=%PATHORG%
:END
