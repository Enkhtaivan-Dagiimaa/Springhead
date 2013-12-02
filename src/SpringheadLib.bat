setlocal enabledelayedexpansion
@echo off
@echo ���C�u�������}�[�W���܂��DVisualC++�̃��C�u�����Ǘ��c�[�� LIB.EXE �Ƀp�X��ʂ��Ă����Ă��������D

set CONVENTION=old
if /i "%1"=="win32" set CONVENTION=new
if /i "%1"=="win64" set CONVENTION=new

if "%CONVENTION%"=="new" (
    :: Visual Studio 2012 �ȍ~�ɓK�p
    set OUTDIR=%1
    set EXT=%2
)
if "%CONVENTION%"=="old" (
    rem
    rem Visual Studio 2010 �ȑO�ɓK�p
    rem
    if /i "%1"=="RELEASE" set EXT=
    if /i "%1"=="DEBUG" set EXT=D
    if /i "%1"=="TRACE" set EXT=T
    if /i "%1"=="MFCDEBUG" set EXT=MD
    if /i "%1"=="MFCRELEASE" set EXT=M
    if /i "%1"=="DEBUGDLL" set EXT=MD
    if /i "%1"=="RELEASEDLL" set EXT=M
    if "%2"=="7" set EXT=7!EXT!
    if "%2"=="8" set EXT=8!EXT!
    if "%2"=="9" set EXT=9!EXT!
    if "%2"=="10" set EXT=10!EXT!
    rem 
    rem ��3������ǉ����܂��iPlatform - Default��"Win32"�j
    if /i "%3"=="x64" (
	    set OUTDIR=win64
	    set EXT=!EXT!x64
    ) else (
	    set OUTDIR=win32
    )
)
rem echo param [%1],[%2]
rem echo OUTDIR,EXT [%OUTDIR%],[%EXT%]
rem exit /b

set OUTPUT=../Lib/%OUTDIR%/Springhead%EXT%.lib
set INPUT=Base/Base%EXT%.lib
set INPUT=%INPUT% Foundation/Foundation%EXT%.lib 
set INPUT=%INPUT% Collision/Collision%EXT%.lib
set INPUT=%INPUT% Physics/Physics%EXT%.lib
set INPUT=%INPUT% Graphics/Graphics%EXT%.lib
set INPUT=%INPUT% FileIO/FileIO%EXT%.lib
set INPUT=%INPUT% Framework/Framework%EXT%.lib
set INPUT=%INPUT% HumanInterface/HumanInterface%EXT%.lib
set INPUT=%INPUT% Creature/Creature%EXT%.lib

if "%INPUT%"=="" echo ���ϐ��p�������̋󂫂��s�����Ă��܂��D
if "%INPUT%"=="" echo �󂫂𑝂₵�Ă�����x���s���܂��D
if "%INPUT%"=="" echo ���̃��b�Z�[�W�������ĕ\�������ꍇ�́C
if "%INPUT%"=="" echo �R�}���h�v�����v�g�̐ݒ���C�����Ă��������D
if "%INPUT%"=="" command /e:4096 /c%0 %1 %2 %3
if not "%INPUT%"=="" (
    LIB /OUT:%OUTPUT% %INPUT%
    echo Springhead: %OUTPUT% created.
)
endlocal
exit /b
