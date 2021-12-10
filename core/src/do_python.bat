@echo off
setlocal enabledelayedexpansion
:: ============================================================================
::  SYNOPSIS
::	do_python.bat script.py [args..]	�iWindows ��p�j
::
::  DESCRIPTION
::	Python script �����s���邽�߂� Windows �p�A�_�v�^�B
::	"Springhead/buildtool" �����p�ł���Ƃ��́A������g���Ď��s����B
::	�����Ȃ���΁A�f�t�H���g�ŗ��p�ł��� python ���g���Ď��s����B
::	Python �����p�ł��Ȃ��ꍇ�̓��b�Z�[�W��\�����ď����𒆎~����B
::	
::	���̃t�@�C���� Windows ��p�ł���B
::	���̃t�@�C���� "<top-dir>/core/src" �ɒu���B
::
::  VERSION
::    Ver 1.0    2019/10/10 F.Kanehori	RunSwig ����ړ�. -SprTop �͔p�~.
::    Ver 1.1    2020/04/16 F.Kanehori	_SPRTOP_ �̃f�t�H���g��ύX.
::    Ver 2.0    2020/05/09 F.Kanehori	_SPRTOP_ �͔p�~.
::    Ver 3.0    2020/12/07 F.Kanehori	Setup �����ڍs���ԊJ�n.
::    Ver 4.0    2021/03/04 F.Kanehori	Setup�͔�W���Apython 2.7��OK.
::    Ver 4.1    2021/03/18 F.Kanehori	����y�у��b�Z�[�W�C��
::    Ver 4.1.1  2021/03/24 F.Kanehori	Bug fix.
:: ============================================================================
set dry_run=0
set verbose=0

:: -------------------------------------------------------------
::  ���̃X�N���v�g�� "<SprTop>/core/src" �ɒu��
::
set CWD=%cd%
cd /d %~dp0\..\..\..
set SprTop=%CD%
cd %CWD%

::-----------------------------------------------------------------------------
:: "%SprTop%/buildtool/win32/python.exe" ������΁A������g�p����B
:: �����Ȃ���΃f�t�H���g�� python ���g�p����B
::
set BUILDTOOL_PATH=%SprTop%\buildtool
if exist "%BUILDTOOL_PATH%\win32\python.exe" (
	Path=%BUILDTOOL_PATH%\win32;%BUILDTOOL_PATH%;!PATH!
)

set python_found=0
if %verbose% equ 0 (
	where python >NUL 2>&1
) else (
	echo where python
	where python
)
if %ERRORLEVEL% equ 0 (
	for /f "usebackq tokens=*" %%a in (`python --version`) do (
		set py_version=%%a
	)
	if "!py_version!" neq "" (
		set python_found=1
	)
)
if %python_found% neq 1 (	
	echo do_python.bat: Python not found.
	echo do_python.bat: We need python to build Springhead Library.
	endlocal
	exit /b
)

::----------------------------------------------
::  Python �����s����i�����͂��̂܂ܓn���j
::
set ARGS=%*
echo (SprTop: %SprTop%, using %py_version%)
echo (python %ARGS%)
if %dry_run% equ 0 (
	python %ARGS%
)

endlocal
exit /b

:: end: do_python.bat
