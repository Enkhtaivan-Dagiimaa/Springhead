if exist pack\springhead2 rmdir /S /Q pack\springhead2
if not exist pack mkdir pack
cd pack
svn export svn+ssh://springhead/home/svn/spr2/Springhead2/trunk ./Springhead2

cd springhead2

rem ���J�ł��Ȃ��t�@�C���̍폜�Ɛݒ�ύX
del /F /S /Q closed
echo #undef USE_CLOSED_SRC > include/SprDefs.h

set PATH=%PATH%;bin

minitar -c -z9 ../Springhead2.tgz *.*
cd ..
cd ..
