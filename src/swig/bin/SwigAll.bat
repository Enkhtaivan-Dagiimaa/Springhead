@echo off
:: ***************************************************************************
::  File:
::	SwigAll.bat
::
::  Description:
::	���̂��ׂẴv���W�F�N�g�̃r���h�ɐ旧���āA�K�v�� *stub.cpp ���쐬
::	���邽�߂̃X�N���v�g.
::
::	������ RunSwig �v���W�F�N�g (make �v���W�F�N�g) �ōs�Ȃ����߁Astub ��
::	�쐬���K�v�ƂȂ�v���W�F�N�g�ɂ��ẮA���̂��Ƃ����Ă�������.
::	  (1) �v���W�F�N�g�̈ˑ��֌W: RunSwig �v���W�F�N�g�Ɉˑ�������.
::	  (2) �����̑ΏۂƂȂ�v���W�F�N�g�����A���̃t�@�C���ɋL�q����.
::		$(ETCDIR)/swigprojs.list
::	�L�q�́A�e�v���W�F�N�g1�s���Ƃ���.
::	���̃t�@�C����(="swigprojs.list")�́A���̃X�N���v�g���ŕύX�ł���.
::		$(BINDIR)/do_swigall.bat
::
::	����ł͎��̂悤�ɂȂ��Ă���.
::		$(BINDIR) = .../Springhead2/src/swig/bin
::		$(ETCDIR) = .../Springhead2/src/swig/etc
::	
::	�����̓��e�ɂ��ẮAmake �v���W�F�N�g���Q�Ƃ̂���.
::		�� ���ۂ̏����� $(BINDIR)/do_swigall.bat ���s�Ȃ�.
:: ***************************************************************************
::  Version:
::	Ver 1.0	  2012/10/17	F.Kanehori
::	Ver 1.0a  2012/11/12	F.Kanehori  ���̃X�N���v�g�̊i�[�ꏊ��ύX
:: ***************************************************************************
setlocal
set CWD=%cd%

set SOLUTION=..\src\Springhead10.sln
devenv %SOLUTION% /build  Debug

cd %CWD%
endlocal
exit /b
