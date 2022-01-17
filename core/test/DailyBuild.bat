@echo off
setlocal enabledelayedexpansion
:: ============================================================================
::  SYNOPSIS
::	DailyBuild [options] [test-repository [result-repository]]
::
::	OPTIONS:
::	    --do-not-clone:	�N���[�������s���������̃��|�W�g�����g�p����.
::				--hook �I�v�V�������O�Ɏw�肷�邱��.
::	    --hook:		�N���[����, "DailyBuildHook\hook.bat" ���K�p
::				�����.
::	    -c conf:		Configurations (Debug | Release).
::	    -p plat:		Platform (x86 | x64).
::	    -s file:		Setup file name (default: setup.conf).
::	    -t tool:		Visual Studio toolset ID. (Windows only)
::
::	ARGUMENTS:
::	    test-repository:	�e�X�g�����s����f�B���N�g��.
::				�����̃f�B���N�g���̏ꍇ���̓��e�͔j�������.
::				�f�t�H���g�� "Springhead".
::	    result-repository:	�e�X�g���ʂ��u�����f�B���N�g��.
::				�f�t�H���g�� "DailyBuildResult\Result"
::
::  DESCRIPTION
::	DailyBuild �����s�𐧌䂷�� (Windows task �ɓo�^����).
::	  (1) test-repository �ɍŐV�̃R�~�b�g�� clone ����.
::	  (2) �w�肪����� hook ��K�p���� (�t�@�C���̂����ւ�).
::	  (3) �e�X�g�����s����.
::
::	Python �����s�ł��邱��. ���̃t�@�C���� demo �}�V����
::	  "I:\DailyBuild"
::	�ɃR�s�[��, ��������N������.
::
::	�N���p�����[�^�̗�
::	  DailyBuild.bat -t 15.0 -c Release -p x64 \ (���̍s�ɑ���)
::				Springhead DailyBuildResult/Result
::	�^�X�N�X�P�W���[���̊J�n (�I�v�V����)(T) �� "I:\DailyBuild" �Ƃ���.
::
::  IMPORTANT
::	���̃t�@�C����u���ʒu
::	    .../DailyBuild/DailyBuildHook
::			  /DailyBuildTestTools	�c�c �K�v�ȃc�[����u���ʒu
::			  /Springhead		�c�c �e�X�g���|�W�g��
::			  DailyBuild.bat	�c�c ���̃t�@�C���͂����ɒu��
::
::	���̃t�@�C�� "DailyBuild.bat" �y�� "DailyBuildHook/hook.bat" ��,
::	DailyBuild �����s���Ă������I�ɂ̓A�b�v�f�[�g����Ȃ�.
::	�ύX���������Ƃ��͎蓮�ŃR�s�[���邱��.
::
::	Springhead Library �쐬�͊O���c�[���̑��݂�O��Ƃ����Ɏ��s�\�Ƃ���.
::	���̂��� python, nkf �Ȃǂ̃c�[���� PATH �ɂ͓o�^���Ȃ�.
::	������ DailyBuild �����s���邽�߂ɂ͂����̃c�[�����K�v�ƂȂ�̂�,
::	DailyBuildTestTools �ɔz�u�����c�[�����΃p�X�N���̏����Ŏg�p����.
::	
::  VERSION
::     Ver 1.0   2021/05/10 F.Kanehori	�o�b�`�t�@�C���̍č\�z.
::     Ver 1.1   2021/07/05 F.Kanehori	DailyBuildTestTools �̓���.
:: ============================================================================
set PROG=%~n0
set CWD=%CD%
set DEBUG=0

set CLONE=yes
set HOOK=no

:: ---------------------------------------------------------------------
::  Step 0
::	�����̉��
::
set OPTS=
:loop1
	if "%1" == "-h"		(call :usage & exit /b)
	if "%1" == "--help"	(call :usage & exit /b)
	if "%1" == "--do-not-clone"    (set CLONE=no & shift & goto :loop1)
	if "%1" == "--hook"	       (set HOOK=yes & shift & goto :loop1)
	if "%1" == "-c"	 (set OPTS=!OPTS! %1 %2 & shift & shift & goto :loop1)
	if "%1" == "-p"	 (set OPTS=!OPTS! %1 %2 & shift & shift & goto :loop1)
	if "%1" == "-t"	 (set OPTS=!OPTS! %1 %2 & shift & shift & goto :loop1)

if not "%2" == "" (set RESULT_REPOSITORY=%2)
if not "%1" == "" (set TEST_REPOSITORY=%1)
if "%TEST_REPOSITORY%" == "" (
	set TEST_REPOSITORY=Springhead
)
if "%RESULT_REPOSITORY%" == "" (
	set RESULT_REPOSITORY=DailyBuildResult/Result
)
set REMOTE_REPOSITORY=https://github.com/sprphys/Springhead
set HOOKFILE=DailyBuildHook\hook.bat

set CLONE=%CLONE: =%
set HOOK=%HOOK: =%
echo test repository:   [%TEST_REPOSITORY%]
echo result repository: [%RESULT_REPOSITORY%]
echo opts:  [%OPTS%]
echo clone: [%CLONE%]
echo hook:  [%HOOK%]
if "%HOOK%" neq "no" (
	if not exist %HOOKFILE% (
		echo --hook specified, but "%HOOKFILE%" does not exist.
		echo abort
		exit /b
	)
)
if %DEBUG% neq 0 (
	set /p x="OK [y/n]? "
	if not "!x!" == "y" (echo done & exit /b)
)

:: ---------------------------------------------------------------------
::  Step 1
::	�K�v�ȃc�[���̊m�F
::
call :abspath DailyBuildTestTools
set TOOLSBASE=%$result%
set PYTHON=%TOOLSBASE%\Python\python.exe
set NKF=%TOOLSBASE%\bin\nkf.exe
set SED=%TOOLSBASE%\sed\bin\sed.exe
set CMAKE=%TOOLSBASE%\cmake\bin\cmake.exe

set TOOLS=%PYTHON% %NKF% %SED% %CMAKE%
set OK=ok
for %%t in (%TOOLS%) do (
	%%t --version >NUL 2>&1
	if !ERRORLEVEL! neq 0 (
		echo Error: "%%t" not found
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
if "%CLONE%" neq "yes" goto :apply_hook
if exist %TEST_REPOSITORY% (
	rem �Â��e�X�g���|�W�g������������폜���čX�̏�Ԃɂ���.
	rem
	echo %PROG%: removing directory "%TEST_REPOSITORY%"
	rmdir /s /q %TEST_REPOSITORY%
)
echo cloning %REMOTE_REPOSITORY%
git clone --recurse-submodules %REMOTE_REPOSITORY% %TEST_REPOSITORY%

:: ---------------------------------------------------------------------
::  Step 3
::	Hook �t�@�C�������݂����炻����R�s�[����.
::
:apply_hook
if "%HOOK%" neq "no" (
	echo %PROG%: calling hook "%HOOKFILE%".
	call %HOOKFILE% %TEST_REPOSITORY%
)

:: ---------------------------------------------------------------------
::  Step 4
::	�e�X�g�����s����
::
cd %TEST_REPOSITORY%\core\test
echo %PROG%: test directory: "%CD%"
echo %PYTHON% DailyBuild.py -A -f %OPTS% %TEST_REPOSITORY% %RESULT_REPOSITORY%
%PYTHON% DailyBuild.py -A -f %OPTS% %TEST_REPOSITORY% %RESULT_REPOSITORY%

endlocal
exit /b

:: ---------------------------------------------------------------------
:abspath
	set $result=%~f1
	exit /b

:usage
	echo DailyBuild [options] [test-repository [result-repository]]
	echo+
	echo options:
	echo     --do-not-clone: do not clone source tree (must be first option).
	echo     --hook file:    apply hook script.
	echo+
	echo arguments:
	echo     test_repository:   test repository (default: Springhead)
	echo     result_repository: result repository (default: DailyBuildResult/Result)
	echo+
	exit /b

::end: DailyBuild.bat
