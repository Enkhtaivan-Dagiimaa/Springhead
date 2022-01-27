@echo off
setlocal enabledelayedexpansion
:: ============================================================================
::  SYNOPSIS
::	hook.bat test-repository
::	    tset-repository	�e�X�g���|�W�g����. ��΃p�X�ŗ^���邱��.
::
::  DESCRIPTION
::	DailyBuild.bat ����Ăяo����, dailybuild ���s���Ɏg�p����t�@�C����
::	�����ւ���d�g�݂�񋟂���. ���̎d�g�𗘗p���� github �ɃR�~�b�g����
::	���ƂȂ��f�o�b�O������d���ނ��Ƃ��\�Ƃ���.
::
::	���̃t�@�C��������ꏊ�� <hook>, �e�X�g���|�W�g���� <test> �Ƃ���Ƃ�
::	"<hook>/core" �ȉ��ɂ���t�@�C���� "<test>/core" �ȉ��̑Ή�����ꏊ��
::	�R�s�[����.
::
::	���̃t�@�C����, demo �}�V���̎��̃f�B���N�g���ɃR�s�[���Ďg�p����.
::	     "I:\DailyBuild\DailyBuildHook"
::	������, "I:\DailyBuild" ����Ăяo������.
::
:: ============================================================================
set PROG=%~n0
set CWD=%CD%
echo [%PROG%] was called from "%CWD%".
cd /d %~dp0

if "%1" == "" (
	echo Usage: %PROG% test-repository
	exit /b 1
)
set REPOSITORY=%1

if exist "ignore" (
	:: ���̃t�@�C�������݂����牽�������ɔ�����
	echo %PROG%: nothing applied ^("ignore" exists^).
	cd %CWD%
	exit /b
)

:: core �ȉ��ɂ���t�@�C�����R�s�[����
::
for /r core %%f in (*) do (
	set fname=%%f
	set fname=!fname:%CD%\=!
	set /p X=%PROG%: applying ".\!fname!" ... <NUL
	copy %%f %CWD%\%REPOSITORY%\!fname! >NUL 2>&1
	if !ERRORLEVEL! equ 0 (
		echo OK
	) else (
		echo NG
	)
)

cd %CWD%
endlocal
exit /b

:: end: hook.bat
