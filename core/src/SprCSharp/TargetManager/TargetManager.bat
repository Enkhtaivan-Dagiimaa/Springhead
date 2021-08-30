@echo off
setlocal enabledelayedexpansion
:: =============================================================================
::  SYNOPSIS:
::	TargetManager target
::	    target:		�^�[�Q�b�g��
::
::  DESCRIPTION:
::	�w�肳�ꂽ�^�[�Q�b�g���K�����s�����悤�ɐ��䂷��.
::	��̓I�ɂ�, �w�肳�ꂽ�^�[�Q�b�g�����t�@�C�� target.last �ɋL�^�����
::	������̂Ƃ��قȂ�Ȃ��, �^�[�Q�b�g RunSwig_CSharp ����������t�@�C��
::	���폜����.
::
:: -----------------------------------------------------------------------------
::  VERSION:
::    Ver 1.0	 2016/12/07 F.Kanehori	����
::    Ver 1.1	 2017/01/16 F.Kanehori	NameManger �̓���.
::    Ver 1.1.1	 2021/07/28 F.Kanehori	������.
:: =============================================================================
set PROG=%~n0
set DEBUG=0

:: ----------------------------------------------------------------------
::  ���̃X�N���v�g�� "<SprTop>/core/src/SprCSharp/TargetManager" �ɒu��
::
set CWD=%cd%
cd /d %~dp0\..\..\..\..
set SprTop=%CD%
cd %CWD%
set CspDir=%SprTop%\core\src\SprCSharp

:: ------------------------
::  ���ʊ��ϐ���ǂݍ���
:: ------------------------
set NAMEMANAGER=%CspDir%\NameManager\NameManager.bat
if not exist %NAMEMANAGER% (
	:: NameManager.bat �����݂��Ȃ��Ƃ��͉������Ȃ�
	echo "NameManager.bat" not found.
	exit /b
)
call %NAMEMANAGER%
if %DEBUG% == 1 (
	echo TARGET FILE:  %TARGETFILE%
	echo CLEAN SCRIPT: %CSHARPCLEAN%
)

:: ------------
::  �����̏���
:: ------------
set TARGET=%1
if "%TARGET%" equ "" (
	echo %PROG%: Error: missing arg "target"
	echo.
	call :usage
	exit /b
)

:: ----------------------------------
::  �L�^���ꂽ�^�[�Q�b�g����ǂݏo��
:: ----------------------------------
for /f %%t in (%TARGETFILE%) do set LASTTARGET=%%t
if %DEBUG% == 1 (
	echo LASTTARGET: %LASTTARGET%
)

:: --------------------------
::  ��r���s�Ȃ����������߂�
:: --------------------------
if "%TARGET%" equ "%LASTTARGET%" (
	:: ��v�����̂ŉ������Ȃ��ėǂ�
	echo+  %PROG%: %LASTTARGET% -^> %TARGET%
) else (
	:: �قȂ����̂Ńt�@�C���̍폜���s�Ȃ�
	echo+  %PROG%: %LASTTARGET% -^> %TARGET%, clearing files
	%CSHARPCLEAN%
	echo %TARGET% > %TARGETFILE%
)

:: ----------
::  �����I��
:: ----------
endlocal
exit /b

:: ---------------------------------------------------------------------
:usage
	echo Usage: %PROG% target
	echo            target:	ALL ^| Physics ^| ...
exit /b

::end TargetManager.bat
