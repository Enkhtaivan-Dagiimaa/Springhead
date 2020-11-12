@echo off
setlocal enabledelayedexpansion
:: ============================================================================
::  SYNOPSIS
::	setup [-f (--force) ]
::	  -f (--force)	�Z�b�g�A�b�v�t�@�C���𖳏����ɍč쐬����B
::	  -c (--check)	�Z�b�g�A�b�v�t�@�C���̐���������������B
::
::  DESCRIPTION
::	�����ł͎��s�ł��� python �������Asetup.py �����s����B
::	python �������鏇�Ԃ́A
::	  (1) buildtool ������΁A���̒��� python
::	  (2) �����Ȃ���΃f�t�H���g�œ��� python
::	
::	python ��������Ȃ��Ƃ��̓��b�Z�[�W��\�����ď����𒆎~����B
:: ----------------------------------------------------------------------------
::  VERSION
::	Ver 1.0  2020/11/09 F.Kanehori	����
:: ============================================================================
set PROG=%~n0
set DEBUG=0

call :backquote where where
set $where=%$result%

set $path=..\..\buildtool\win32
if exist "%$path%\python.exe" (
	set $python=%$path%\python.exe
	if not %DEBUG% equ 0 (echo -- found python: !$python!)
) else (
	call :which
	if not "!$result!" == "" (
		set $python=!$result!
		if not %DEBUG% equ 0 (echo -- found python: !$python!)
	)
)
if "%$python%" == "" (
	echo Can't find 'python'.
	echo We need python to build Springhead Library.
	echo "https://github.com/sprphys/buildtool" provides minimum python environment for Windows.
	exit /b 1
)

rem -------- �������� --------------------------------
rem �������� ��python�e�X�g�p�̃R�[�h
rem
if "%1" == "-o" (
	set $python=F:\OLD-Python\Python27\python.exe
)
rem -------- �����܂� --------------------------------

if not %DEBUG% equ 0 (echo %$python% setup.py %*)
%$python% setup.py %*

endlocal
exit /b

:which
	setlocal enabledelayedexpansion
	call :backquote python --version
	if not "%$result%" == "" (
		call :backquote %$where% python
	)
	(endlocal && set $result=%$result%)
	exit /b

:backquote
	setlocal enabledelayedexpansion
	set result=
	for /f "usebackq tokens=*" %%a in (`%* 2^> NUL`) do (
		if "!result!" == "" (set result=%%a)
	)
	(endlocal && set $result=%result%)
 	exit /b

::end: setup.bat
