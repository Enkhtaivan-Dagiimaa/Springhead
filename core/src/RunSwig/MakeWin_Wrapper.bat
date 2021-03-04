@echo off
:: ----------------------------------------------------------------------------
::  SYNOPSIS
::	MakeWin_Wrapper.bat {-build | -rebuild | -clean}
::
::  DESCRIPTION
::	RunSwig/Makefile.win �� Visual Studio ������s���邽�߂̃��b�p�B
::	Python �����s�ł�����𐮂��Ă���e�������Ăяo���B
::
:: ----------------------------------------------------------------------------
::  Version:
::    Ver 1.0    2020/12/10 F.Kanehori	����
::    Ver 2.0    2021/03/04 F.Kanehori	Python���ݒ��do_python.bat�ōs�Ȃ�.
:: ----------------------------------------------------------------------------
setlocal enabledelayedexpansion
set CWD=%cd%
set DEBUG=0

rem echo [[[[ MakeWin_Wrapper: %* ]]]]
:: -------------------------------------------------------------
::  ���̃X�N���v�g�� "<SprTop>/core/src/RunSwig" �ɒu��
::
cd /d %~dp0
set ScriptDir=%CD%
set SrcDir=%ScriptDir%\..
cd %CWD%

:: ��ƃt�@�C����
set TMPF=MakeWin_Wrapper.tmp

:: -------------------------------------------------------------
::  �����̉��
::
if "%1" equ ""	( call :usage & exit /b 1)
if "%2" neq ""	( call :usage & exit /b 1)
if "%1" neq "-build" if "%1" neq "-rebuild" if "%1" neq "-clean" (
	call :usage & exit /b 1
)

:: -------------------------------------------------------------
::  �R�}���h�����s����
::
set python=%SrcDir%\do_python.bat
if "%1" equ "-build" (
	%python% make_manager.py -A -c && nmake -f Makefile.win
)
if "%1" equ "-rebuild" (
	%python%  make_manager.py -A -d -c && nmake -f Makefile.win
)
if "%1" equ "-clean" (
	%python%  make_manager.py -A -d
)

endlocal
exit /b 0

:: ----------------------------------------------------------------------------
:usage
	echo Usage: %~n0 { -build ^| -rebuild ^| -clean }
	exit /b

:end MakeWin_Wrapper.bat
