@echo off
:: ***********************************************************************************
::  File:
::	    make_manager.bat [-A] {-c -d -r -t}
::
::  Description:
::	    �e�v���W�F�N�g�Ŏ��s���� make �̐�����s�Ȃ�.
::
::	�����F	-A	�ΏۂƂȂ邷�ׂẴv���W�F�N�g�ɂ���, �ȍ~�̈����Ŏ����ꂽ
::			���������s����.
::
::		�ȉ�, �ʂ̃v���W�F�N�g�ɑ΂��鏈��
::		    -c	Makefile.swig ���Ȃ���΍쐬����.
::		    -d	Makefile.swig ���폜����.
::		    -r	Makefile.swig.tmp �������, ����� Makefile.swig �Ƃ���.
::		    -t	Makefile.swig.tmp ���쐬���� (�e���|�����t�@�C��).
::
::	    �e���|�����t�@�C���̍쐬���[�h������̂�, make �̎��s���� Makefile.swig ��
::	    ���������Ă��܂��̂�������邽��.
::
:: ***********************************************************************************
::  Version:
::	Ver 1.0	 2013/01/07 F.Kanehori	����
::	Ver 1.1	 2013/03/13 F.Kanehori	���W���[�����ʕ�����ꕔ�ύX
:: ***********************************************************************************
setlocal enabledelayedexpansion
set CWD=%cd%
set DEBUG=0

:: ----------
::  �e���`
:: ----------
:: �g�p����v���V�[�W��
::
set MAKEMGR=make_manager.bat
set CREATEMKF=create_mkf.bat

:: �g�p����t�@�C����
::	%MAKEFILE% �� RunSwig.bat �����s���邽�߂� makefile ��
::	%MAKETEMP% �� makefile �X�V�̂��߂̃e���|�����t�@�C����
::	%PROJFILE% �� �ˑ��v���W�F�N�g��`�t�@�C����
::
set MAKEFILE=Makefile.swig
set MAKETEMP=Makefile.swig.tmp
set PROJFILE=do_swigall.projs
set PROJFILEONE=do_swigone.projs

:: �f�B���N�g���̒�`
::
set SRCDIR=..\..\src
set BINDIR=..\..\src\RunSwig
set ETCDIR=..\..\src\RunSwig

:: ------------------------
::  -A �I�v�V�����̏���
:: ------------------------
if "%1" equ "-A" (
    set NEEDONEFILE=
    if "%2" equ "-c" set NEEDONEFILE=1
    if "%3" equ "-c" set NEEDONEFILE=1
    if "%2" equ "-t" set NEEDONEFILE=1

    if !NEEDONEFILE! == 1 echo checking makefile existance...
    for /f "tokens=1,*" %%p in (%ETCDIR%\%PROJFILE%) do (
	    cd %SRCDIR%\%%p
	    if !NEEDONEFILE! == 1 (
	        echo.
	        echo     ***  %%p  ***
	        echo %%p %%q > %PROJFILEONE%
	    )
	    cmd /c %BINDIR%\%MAKEMGR% %2 %3 %4 %5 %6 %7 %8 %9
	    cd %CWD%
    )
    :: �I��
    exit /b
)

:: ------------------------
::  �ʃv���W�F�N�g�̏���
:: ------------------------
::  �����̉��
::
set ARG_C=0
set ARG_D=0
set ARG_R=0
set ARG_T=0
:ana_args
    if "%1" equ "-c" (set ARG_C=1
    ) else if "%1" equ "-d" (set ARG_D=1
    ) else if "%1" equ "-r" (set ARG_R=1
    ) else if "%1" equ "-t" (set ARG_T=1
    ) else goto :end_args
    shift
    goto :ana_args
:end_args

set /a NUM_ARGS=%ARG_C%+%ARG_D%+%ARG_R%+%ARG_T%
if %NUM_ARGS% == 0 (
    echo Usage: %0 {-A ^| -c ^| -d ^| -t ^| -r}
    exit /b
)

if %DEBUG% == 1 (
    set ARGS=
    if %ARG_C% == 1 set ARGS=!ARGS! -c
    if %ARG_D% == 1 set ARGS=!ARGS! -d
    if %ARG_R% == 1 set ARGS=!ARGS! -r
    if %ARG_T% == 1 set ARGS=!ARGS! -t
    echo arguments:!ARGS!
)

:: ------------------------
::  �ˑ��v���W�F�N�g�̎擾
:: ------------------------
::    PROJ = �v���W�F�N�g��
::    DEPT = �ˑ��v���W�F�N�g���i�J���}��؂�j
::
set /a INIT=%ARG_C%+%ARG_T%
set PROJ=
set DEPT=
if %INIT% gtr 0 (
    call :leaf_name %cd%
    for /f "tokens=1,*" %%p in (%PROJFILEONE%) do (
        if !PROJ! equ %%p (
            set DEPT="%%q"
	        if %DEBUG% == 1 (
		        echo.  project=!PROJ!
		        echo.  dependencies=!DEPT!
	        )
	        goto :exit_for
	    )
    )
    if "%DEPT%" equ "" (
	    echo make_manager: can not get dependencies.
	    exit /b
    )
)
:exit_for

:: ------------------------------------
::  ���� -d �̏����F %MAKEFILE% ���폜
:: ------------------------------------
::
if %ARG_D% == 1 (
    if exist %MAKEFILE% (
	    if %DEBUG% == 1 echo removing "%MAKEFILE%"
	    del %MAKEFILE%
    )
)

:: ------------------------------------
::  ���� -c �̏����F %MAKEFILE% ���쐬
:: ------------------------------------
::
if %ARG_C% == 1 (
    if not exist %MAKEFILE% (
    	if %DEBUG% == 1 echo creating "%MAKEFILE%"
    	cmd /c %BINDIR%\%CREATEMKF% %MAKEFILE% %PROJ% %DEPT%
    )
)

:: ------------------------------------
::  ���� -t �̏����F %MAKETEMP% ���쐬
:: ------------------------------------
::
if %ARG_T% == 1 (
    echo     temporary creation mode
    if exist %MAKETEMP% del %MAKETEMP%
    if %DEBUG% == 1 echo creating "%MAKETEMP%"
    cmd /c %BINDIR%\%CREATEMKF% %MAKETEMP% %PROJ% %DEPT%
)

:: --------------------------------------------------
::  ���� -r �̏����F %MAKETEMP% �� %MAKEFILE% �ɕύX
:: --------------------------------------------------
::
if %ARG_R% == 1 (
    if exist %MAKETEMP% (
	    if %DEBUG% == 1 echo renaming "%MAKETEMP%" to "%MAKEFILE%"
	    if exist %MAKEFILE% del %MAKEFILE%
	    rename %MAKETEMP% %MAKEFILE%
    )
)

:: ----------
::  �����I��
:: ----------
endlocal
exit /b

:: ============================================
::  �T�u���[�`���F
::	call :leaf_name path	���[�t���̎擾
::	���ʂ͊��ϐ� PROJ �ɐݒ肷��
:: ============================================
:leaf_name
    set PROJ=%~nx1
exit /b

:end make_manager.bat
