@echo off
@echo ���C�u�������}�[�W���܂��DVisualC++�̃��C�u�����Ǘ��c�[�� LIB.EXE �Ƀp�X��ʂ��Ă����Ă��������D

if "%1"=="RELEASE" set EXT=
if "%1"=="DEBUG" set EXT=D
if "%1"=="MFCRELEASE" set EXT=M
if "%1"=="MFCDEBUG" set EXT=MD
if "%2"=="7" set EXT=7%EXT%

set OUTPUT=../Lib/win32/Springhead%EXT%.lib
set INPUT=Base/Base%EXT%.lib Foundation/Foundation%EXT%.lib 
set INPUT=%INPUT% Collision/Collision%EXT%.lib Physics/Physics%EXT%.lib
set INPUT=%INPUT% Graphics/Graphics%EXT%.lib FileIO/FileIO%EXT%.lib 
rem set INPUT=Device/Device%EXT%.lib HIS/HIS%EXT%.lib

if "%INPUT%"=="" echo ���ϐ��p�������̋󂫂��s�����Ă��܂��D
if "%INPUT%"=="" echo �󂫂𑝂₵�Ă�����x���s���܂��D
if "%INPUT%"=="" echo ���̃��b�Z�[�W�������ĕ\�������ꍇ�́C
if "%INPUT%"=="" echo �R�}���h�v�����v�g�̐ݒ���C�����Ă��������D
if "%INPUT%"=="" command /e:4096 /c%0 %1
if not "%INPUT%"=="" LIB /OUT:%OUTPUT% %INPUT%
