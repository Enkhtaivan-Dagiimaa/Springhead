@echo off

rem /v ���[�h�ōċN��
if "%1"=="LISTSRC" goto ListSrc
cmd /v:on /c%0 LISTSRC
goto end

rem ��������X�^�[�g
:ListSrc
set TARGET=Foundation Collision Physics Graphics Creature HumanInterface Framework
set MODULE=SprPy
set API_INCLUDE=../../include

rem makefile�̍쐬��Make�̎��s
set PATHORG=%PATH%;

set PATH=..\..\bin;..\..\bin\swig
set SRCINTF=
for %%p in (%TARGET%) do for %%f in (%API_INCLUDE%/%%p/*.h) do set SRCINTF=!SRCINTF! ../../include/%%p/%%f

set FILE=%MODULE%Files.i
echo #	Do not edit. RunSwig.bat will update this file.> %FILE%
for %%p in (%SRCINTF%) do echo %%include "%%p">> %FILE%

rem SWIG PART
swig -c++ -w312,319,325,362,401,402 -python -static_link %MODULE%.i 
rem 2> %0.log
copy SprPy.py ..\..\bin\src\Python\Lib
:end
