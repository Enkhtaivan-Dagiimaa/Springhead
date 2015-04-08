@echo off
:: ===================================================================================
::  File:
::	RunSwig.bat
::
::  Description:
::	Springhead �̃��C�u����(DLL) �� C# ���痘�p���邽�߂̃R�[�h�𐶐�����B
::
::  Version:
::	Ver 1.0	 2015/01/26 F.Kanehori	����
:: ===================================================================================
setlocal enabledelayedexpansion
set CWD=%cd%
set DUMPTREE=0
if "%1" equ "dumptree" set DUMPTREE=1

:: ----------
::  �e���`
:: ----------
:: �f�B���N�g���̒�`
::
call :set_abspath SRCDIR ..\..\..\src
set SPRCS=SprCSharp
set CS_SRC=SprCSharp
set CS_IMP=SprImport
set CS_EXP=SprExport
echo using src directory: %SRCDIR%

:: �����ΏۂƂȂ�f�B���N�g��
::
set PROJECTS=Base Collision Creature FileIO Foundation Framework Graphics HumanInterface Physics

:: �g�p����p�X
::
call :set_abspath SWIGPATH ..\..\..\bin\swig
set PATH=%SWIGPATH%;%PATH% 

:: �g�p����v���O������
::
set SWIG=swig
set ARGS=-sprcs -c++ -I%SWIGPATH%\Lib -w305,312,319,325,401,402

:: ----------
::  �����J�n
:: ----------
rem echo start at %CWD%
for %%p in (%PROJECTS%) do (
    echo.
    echo *** %%p ***
    :: �e�f�B���N�g���ֈړ����� make �����s����
    cd %SRCDIR%\%%p
    if exist %%p.i (
	if %DUMPTREE% == 1 (
	    cmd /c %SWIG% -debug-module 1 %%p.i > %%p.i.tree
	) else (
	    cmd /c %SWIG% %ARGS% -DSWIG_%%p %%p.i
	    copy .\%%pCs.cs  %SRCDIR%\%SPRCS%\%CS_SRC% > NUL
	    copy .\%%pCs.csp %SRCDIR%\%SPRCS%\%CS_IMP% > NUL
	    copy .\%%pCs.cpp %SRCDIR%\%SPRCS%\%CS_EXP% > NUL
	    echo %CS_SRC%\%%pCs.cs  created
	    echo %CS_IMP%\%%pCs.csp created
	    echo %CS_EXP%\%%pCs.cpp created
	)
    ) else (
	echo "%%p.i" not found
    )
    cd %CWD%
)

:: ----------
::  �����I��
:: ----------
endlocal
exit /b

:set_abspath
    set %1=%~f2
exit /b
