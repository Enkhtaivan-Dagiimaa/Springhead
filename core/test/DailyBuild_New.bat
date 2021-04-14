@echo off
setlocal enabledelayedexpansion
:: ============================================================================
::  SYNOPSIS
::	DailyBuild [test-repository [result-repository]]
::	    test_repository:	�e�X�g�����s����f�B���N�g��.
::				�������݂����Ȃ�΂���͂��ׂĔj�������.
::				�f�t�H���g�� "Springhead".
::	    result_repository:	�e�X�g���ʂ��u�����f�B���N�g��.
::
::  DESCRIPTION
::	DailyBuild �����s����.  Python �����s�ł��邱��.
::	���̃t�@�C����, demo �}�V���� "I:\DailyBuild" �ɃR�s�[����, ������
::	�N�����邱��.
::	
::  VERSION
::     Ver 1.0   2021/04/14 F.Kanehori	�o�b�`�t�@�C���̍č\�z.
:: ============================================================================
set PROG=%~n0

if not "%2" == "" (set RESULT_REPOSITORY=%2)
if not "%1" == "" (set TEST_REPOSITORY=%1)
if "%TEST_REPOSITORY%" == "" (
	set TEST_REPOSITORY=Springhead
)
if "%RESULT_REPOSITORY%" == "" (
	set RESULT_REPOSITORY=DailyBuildResult/Result
)
set REMOTE_REPOSITORY=https://github.com/sprphys/Springhead
echo TEST   %TEST_REPOSITORY%
echo RESULT %RESULT_REPOSITORY%

:: ---------------------------------------------------------------------
::  Step 1
::	�K�v�ȃc�[���̊m�F
::
set PYTHON=python.exe

set TOOLS=%PYTHON%
set OK=ok
for %%t in (%TOOLS%) do (
	where %%t >NUL 2>&1
	if !ERRORLEVEL! neq 0 (
		echo %PROG%: we need '%%t'
		set OK=no
	)
)
if "%OK%" neq "ok" (
	echo abort
	exit /b
)

:: ---------------------------------------------------------------------
::  Step 2
::	Springhead ���ŐV�̏�Ԃɂ���.
::
if exist %TEST_REPOSITORY% (
	echo %PROG%: removing directory "%TEST_REPOSITORY%"
	rmdir /s /q %TEST_REPOSITORY%
)
echo cloning %REMOTE_REPOSITORY%
git clone --recurse-submodules %REMOTE_REPOSITORY% %TEST_REPOSITORY%

:: ---------------------------------------------------------------------
::  Step 3
::	�e�X�g�����s����
::
cd %TEST_REPOSITORY%\core\test
echo %PROG%: test directory: "%CD%"
%PYTHON% DailyBuild.py -A -f %* %TEST_REPOSITORY% %RESULT_REPOSITORY%

endlocal
exit /b

:: end: DailyBuild.bat
