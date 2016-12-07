@echo off
:: ***********************************************************************************
::  File:
::      RunSwig_Clean.bat
::
::  Description:
::      RunSwig_CSharp �ō쐬���ꂽ�t�@�C�����폜����.
::      �v���W�F�N�g RunSwig_CSharp �� [�\�����[�V�����̃N���[��] ����Ă΂��.
::
::    �@���s����v���W�F�N�g�� ..\..\src\RunSwig\do_swigall.projs �ɒ�`����Ă���
::      ���̂��g�p����. �������v���W�F�N�g Base �͒�`�̗L���Ɋւ��Ȃ����s����.
::
:: ***********************************************************************************
::  Version:
::	Ver 2.0	 2016/12/05	F.Kanehori  �^�[�Q�b�g�w�����
::	Ver 1.0	 2015/03/18	F.Kanehori  ����
:: ***********************************************************************************
setlocal enabledelayedexpansion
set PROG=%~n0
set CWD=%cd%
set DEBUG=1

set TOPDIR=..\..\..
set SRCDIR=%TOPDIR%\src
set ETCDIR=%SRCDIR%\RunSwig

set CSBASE=..
set CS_SRC=%CSBASE%\SprCSharp
set CS_IMP=%CSBASE%\SprImport
set CS_EXP=%CSBASE%\SprExport

if %DEBUG% == 1 (
    echo %PROG%
    echo CWD: %CWD%
    call :show_abs_path TOPDIR %TOPDIR%
    call :show_abs_path SRCDIR %SRCDIR%
    call :show_abs_path ETCDIR %ETCDIR%
    call :show_abs_path CSBASE %CSBASE%
    call :show_abs_path CS_SRC %CS_SRC%
    call :show_abs_path CS_IMP %CS_IMP%
    call :show_abs_path CS_EXP %CS_EXP%
    echo. 
)

:: --------------------
::  �g�p����t�@�C����
:: --------------------
set PROJFILE=do_swigall.projs
set TARGETFILE=..\TargetManager\target.last

:: ------------------------------
::  �������郂�W���[���ꗗ���쐬
:: ------------------------------
set PROJECTS=Base
for /f "tokens=1,*" %%m in (%ETCDIR%\%PROJFILE%) do set PROJECTS=!PROJECTS! %%m
if %DEBUG% == 1 echo Projects are: %PROJECTS%

:: ----------
::  �����J�n
:: ----------
for %%p in (%PROJECTS%) do (
    del %CS_SRC%\CS%%p*.cs  > NUL 2>&1
    del %CS_IMP%\CS%%p*.cs  > NUL 2>&1
    del %CS_EXP%\CS%%p*.cpp > NUL 2>&1
    del %CS_SRC%\module_wrapper.cs  > NUL 2>&1
    del %CS_IMP%\module_wrapper.cs  > NUL 2>&1
    del %CS_EXP%\module_wrapper.cpp > NUL 2>&1
)
echo. > %TARGETFILE%

:: ----------
::  �����I��
:: ----------
endlocal
exit /b

:: -----------------------------------------------------------------------------------
::  �f�o�b�O�p
:: -----------------------------------------------------------------------------------
:show_abs_path
    echo %1:  [%~f2]
exit /b

::end RunSwig_Clean.bat
