@echo off
@echo ���C�u�������}�[�W���܂��DVisualC++�̃��C�u�����Ǘ��c�[�� LIB.EXE �Ƀp�X��ʂ��Ă����Ă��������D

if "%1"=="RELEASE" set EXT=
if "%1"=="DEBUG" set EXT=D
if "%1"=="TRACE" set EXT=T
if "%1"=="MFCRELEASE" set EXT=M
if "%1"=="MFCDEBUG" set EXT=MD
if "%2"=="7" set EXT=7%EXT%
if "%2"=="8" set EXT=8%EXT%
if "%2"=="9" set EXT=9%EXT%
if "%2"=="10" set EXT=10%EXT%


set OUTPUT=../Lib/win32/Springhead%EXT%.lib
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
if "%INPUT%"=="" command /e:4096 /c%0 %1
if not "%INPUT%"=="" LIB /OUT:%OUTPUT% %INPUT%
