@echo off
setlocal enabledelayedexpansion
:: ============================================================================
::  SYNOPSIS
::	hook.bat test-repository
::	    test_repository:	�e�X�g�����s����f�B���N�g��.
::
::  DESCRIPTION
::	DailyBuild.bat ����Ăяo�����.
::	�e�X�g�ɓ���O�ɂ���Ă����ׂ�����(�R�~�b�g����O�Ƀe�X�g���邱�ƂȂ�)
::	�����s���邽�߂̃t�b�N�X�N���v�g.
::	���̃t�@�C����, demo �}�V���� "I:\DailyBuild" �ɃR�s�[���邱��.
::
:: ============================================================================
set PROG=%~n0
echo hook [%PROG%] was called.

endlocal
exit /b

:: end: hook.bat
