@echo off
:: ***********************************************************************************
::  File:
::      TargetManager.bat
::
::  SYNOPSIS:
::	TargetManager target
::
::  Description:
::      �t�@�C��"target.last"�ɋL�^���ꂽ�^�[�Q�b�g���ƈ����Ŏw�肳�ꂽ�^�[�Q�b�g��
::	�Ƃ��قȂ��Ă����Ȃ�΁A�^�[�Q�b�g RunSwig_CSharp ����������t�@�C�����폜
::	���邱�Ƃł��̃^�[�Q�b�g���K�����s�����悤�ɂ���.
::
::  ARGUMENTS:
::	target:		�^�[�Q�b�g��
::	
:: ***********************************************************************************
::  Version:
::	Ver 1.0	 2016/12/07	F.Kanehori  ����
:: ***********************************************************************************
setlocal enabledelayedexpansion
set PROG=%~n0
set DEBUG=0

:: ------------
::  �����̏���
:: ------------
set TARGET=%1
if "%TARGET%" equ "" (
	echo %PROG%: Error: need arg "target"
	exit /b
)

:: --------------------
::  �g�p����t�@�C����
:: --------------------
set TARGETFILE=..\TargetManager\target.last

:: --------------------
::  �g�p����X�N���v�g
:: --------------------
set CLEAN=..\RunSwig_CSharp\RunSwig_Clean.bat

:: ----------------------------------
::  �L�^���ꂽ�^�[�Q�b�g����ǂݏo��
:: ----------------------------------
for /f %%t in (%TARGETFILE%) do set LASTTARGET=%%t
if %DEBUG% == 1 echo LASTTARGET: %LASTTARGET%

:: --------------------------
::  ��r���s�Ȃ����������߂�
:: --------------------------
if "%TARGET%" equ "%LASTTARGET%" (
	:: ��v�����̂ŉ������Ȃ��ėǂ�
	echo %PROG%: %LASTTARGET% -^> %TARGET%
) else (
	:: �قȂ����̂Ńt�@�C���̍폜���s�Ȃ�
	echo %PROG%: %LASTTARGET% -^> %TARGET%, clearing files
	%CLEAN% 
)

:: ----------
::  �����I��
:: ----------
endlocal

::end TargetManager.bat
