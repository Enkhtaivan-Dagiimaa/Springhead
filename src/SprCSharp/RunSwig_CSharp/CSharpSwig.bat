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
set DEBUG=0
if %DEBUG% == 1 (
    set CS_INFO=SprInfo
    if not exist !CS_INFO! mkdir !CS_INFO!
)

set MODULE=%1
if "%MODULE%" equ "" (
    echo Error: Module name MUST be specified.
    exit /b
)

set DUMPTREE=0
if "%1" equ "dumptree" set DUMPTREE=1

:: ----------
::  �e���`
:: ----------
:: �f�B���N�g���̒�`
::
call :set_abspath SRCDIR ..\..\src
set CS_SRC=SprCSharp
set CS_IMP=SprImport
set CS_EXP=SprExport

echo. 
echo *** %MODULE% ***
echo using src directory: %SRCDIR%

:: �g�p����p�X
::
set SWIGPATH=..\..\bin\swig
set PATH=%SWIGPATH%;%PATH% 

:: �g�p����v���O������
::
set SWIG=swig_sprcs
set ARGS=-sprcs -DSWIG_CS_SPR -c++ -I%SWIGPATH%\Lib -w305,312,319,325,401,402

:: ----------
::  �����J�n
:: ----------
if exist %MODULE%.i (
    if %DUMPTREE% == 1 (
        cmd /c %SWIG% -debug-module 4 !MODULE!.i > CS%MODULE%.tree
    ) else (
	cmd /c %SWIG% %ARGS% -DSWIG_%MODULE% !MODULE!.i
	move /Y !MODULE!Cs.cs  %CS_SRC%\CS%MODULE%.cs  > NUL 2>&1
	move /Y !MODULE!CsP.cs %CS_IMP%\CS%MODULE%.cs  > NUL 2>&1
	move /Y !MODULE!Cs.cpp %CS_EXP%\CS%MODULE%.cpp > NUL 2>&1
	move /Y !MODULE!CsWrap.cs  %CS_SRC%\CS%MODULE%.wrapper.cs  > NUL 2>&1
	move /Y !MODULE!CsPWrap.cs %CS_IMP%\CS%MODULE%.wrapper.cs  > NUL 2>&1
	move /Y !MODULE!CsWrap.cpp %CS_EXP%\CS%MODULE%.wrapper.cpp > NUL 2>&1
	echo %CS_SRC%\CS%MODULE%.cs  created
	echo %CS_IMP%\CS%MODULE%.cs  created
	echo %CS_EXP%\CS%MODULE%.cpp created
	echo %CS_SRC%\CS%MODULE%.wrapper.cs  created
	echo %CS_IMP%\CS%MODULE%.wrapper.cs  created
	echo %CS_EXP%\CS%MODULE%.wrapper.cpp created
	if %DEBUG% == 1 (
	    move /Y %MODULE%CS.info %CS_INFO%\CS%MODULE%.info >NUL 2>&1
	) else (
	    del %MODULE%CS.info 2>NUL
	)
    )
) else (
    echo "%MODULE%.i" not found
)
echo. 

:: ----------
::  �����I��
:: ----------
endlocal
exit /b

:set_abspath
    set %1=%~f2
exit /b
