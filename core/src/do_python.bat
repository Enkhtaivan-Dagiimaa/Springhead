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
::	�f�t�H���g�� python �����p�ł��Ȃ��ꍇ�̓��b�Z�[�W��\������
::	�����𒆎~����B
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
:: ============================================================================
set PROG=%~n0
set START_DIR=%CD%
set verbose=0
set dry_run=0

::-----------------------------------------------------------------------------
:: Springhead tree �̃g�b�v�ֈړ�����
::	���̃X�N���v�g�� "<top>/core/src" �ɒu���B
::
cd /d %~dp0
cd ..\..
set SprTop=%CD%
cd %START_DIR%

::-----------------------------------------------------------------------------
:: "%SprTop%/buildtool/win32/python.exe" ������΁A������g�p����B
:: �����Ȃ���΃f�t�H���g�� python ���g�p����B
::
set BUILDTOOL_PATH=%SprTop%\buildtool\win32
if exist "%BUILDTOOL_PATH%\python.exe" (
	set path=%BUILDTOOL_PATH%;!PATH!
	if %verbose% geq 2 (
		echo buildtool found at "%BUILDTOOL_PATH%"
	)
)
) else (
	where python >NUL 2>& 1
	if !ERRORLEVEL! neq 0 (
		echo Python not found.
		echo We need python to build Springhead Library.
		endlocal
		exit /b
	)
)
if %verbose% geq 1 (
	python --version
)

::----------------------------------------------
::  Python �����s����i�����͂��̂܂ܓn���j
::
set ARGS=%*
if %dry_run% neq 0 (
	echo python %ARGS%
) else (
	python %ARGS%
)

endlocal
exit /b

:: end: do_python.bat
