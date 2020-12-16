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
::	Ver 1.0  2020/11/10 F.Kanehori	����
:: ============================================================================
set PROG=%~n0
set CWD=%CD%
set DEBUG=0

:: -------------------------------------------------------------
::  ���̃X�N���v�g�� "<SprTop>/core/src" �ɒu��
::
cd /d %~dp0
set ScriptDir=%CD%
cd %ScriptDir%

:: -------------------------------------------------------------
::  ����
::
call :backquote where where
set $where=%$result%

:: -------------------------------------------------------------
::  Python ��������
::
set $path=..\..\buildtool\win32
if exist "%$path%\python.exe" (
	set $python=%$path%\python.exe
	echo -- found python: !$python!
) else (
	rem  %HOME%\AppData\Local\Microsoft\WindowsApps ��������ꍇ
	rem	���� python �͎g���Ȃ�!
	rem
	set FAKE_PYTHON=%HOME%\AppData\Local\Microsoft\WindowsApps\python.exe
	call :backquote where python
	if "!$result!" equ "!FAKE_PYTHON!" (
		echo Found "!$result!", but you can't use it.
		goto :message
	)
	rem  ���s�ł��� python ����������
	rem
	call :which
	if not "!$result!" == "" (
		set $python=!$result!
		echo -- found python: !$python!
	)
)
if "%$python%" == "" (
	echo Can't find 'python'.
:message
	echo We need 'python' to build Springhead Library.
	echo "https://github.com/sprphys/buildtool" provides required environment for Windows.
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
%$python% setup.py %* %$python%

endlocal
exit /b

:: ----------------------------------------------------------------------------
::  ���[�J���֐�
::
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
