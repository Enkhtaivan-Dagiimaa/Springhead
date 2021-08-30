@echo off
setlocal enabledelayedexpansion
:: =============================================================================
::  SYNOPSIS
::      RunSwig_Clean project-list
::
::  DESCRIPTION:
::      RunSwig_CSharp �ō쐬���ꂽ�t�@�C�����폜����.
::      �v���W�F�N�g RunSwig_CSharp �� [�\�����[�V�����̃N���[��] ����Ă΂��.
::
:: -----------------------------------------------------------------------------
::  VERSION:
::    Ver 1.0	 2015/03/18 F.Kanehori	����
::    Ver 2.0	 2016/12/05 F.Kanehori	�����N�\���w�����
::    Ver 2.1	 2017/01/16 F.Kanehori	NameManger ����
::    Ver 2.2	 2017/08/09 F.Kanehori	�폜����t�@�C����ǉ�
::    Ver 2.3	 2017/08/28 F.Kanehori	NameManager.bat ���Ȃ��Ƃ��̏�����ǉ�.
::    Ver 2.3.1	 2017/09/07 F.Kanehori	Bug fixed.
::    Ver 2.3.2	 2021/07/19 F.Kanehori	�v���W�F�N�g���擾���@�ύX.
:: =============================================================================
set PROG=%~n0
set DEBUG=0

:: ----------------------------------------------------------------------
::  ���̃X�N���v�g�� "<SprTop>/core/src/SprCSharp/RunSwig_CSharp" �ɒu��
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
	echo %PROG%
	echo+  CWD:          %CD%
	echo+  TARGETFILE:   %TARGETFILE%
	call :show_abspath TOPDIR %TOPDIR%
	call :show_abspath SRCDIR %SRCDIR%
	call :show_abspath ETCDIR %ETCDIR%
	call :show_abspath CS_SRC %CS_SRC%
	call :show_abspath CS_IMP %CS_IMP%
	call :show_abspath CS_EXP %CS_EXP%
	call :show_abspath RUNSWIGCSHARP %RUNSWIGCSHARP%
)

:: ----------------------------------
::  �������郂�W���[���ꗗ��ǂݍ���
:: ----------------------------------
set PYTHON=%SprTop%\buildtool\win32\python.exe
if not exist %PYTHON% (
	set PYTHON=python
)
%PYTHON% %RUNSWIGCSHARP%\make_projs_file.py all_projs.bat PROJECTS
call all_projs.bat PROJECTS
if %DEBUG% == 1 (
	echo+  Projects are: %PROJECTS%
)

:: ----------
::  �����J�n
:: ----------
for %%p in (%PROJECTS%) do (
    del %CS_SRC%\CS%%p*.cs  > NUL 2>&1
    del %CS_IMP%\CS%%p*.cs  > NUL 2>&1
    del %CS_EXP%\CS%%p*.cpp > NUL 2>&1
    del %CS_SRC%\module.wrapper.cs  > NUL 2>&1
    del %CS_IMP%\module.wrapper.cs  > NUL 2>&1
    del %CS_EXP%\module.wrapper.cpp > NUL 2>&1
    del %CS_SRC%\tmp\*.cs  > NUL 2>&1
    del %CS_IMP%\tmp\*.cs  > NUL 2>&1
    del %CS_EXP%\tmp\*.cpp > NUL 2>&1
)
set SUFFIX=exe dll pdb exp lib config manifest
for %%s in (%SUFFIX%) do (
	del /S /Q %SPRCSHARP%\14.0\*.%%s > NUL 2>&1
)
type NUL > %TARGETFILE%
echo+  target file:  %TARGETFILE%

:: ----------
::  �����I��
:: ----------
endlocal
exit /b

:: ---------------------------------------------------------------------
::  �f�o�b�O�p
::
:show_abspath
    echo+  %1:       %~f2
exit /b

::end RunSwig_Clean.bat
