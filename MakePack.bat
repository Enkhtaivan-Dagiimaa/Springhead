if exist pack\springhead rmdir /S /Q pack\springhead
if not exist pack mkdir pack
cd pack
svn export svn+ssh://springhead/home/svn/spr2/Springhead2/trunk ./Springhead2

cd springhead2

minitar -c -z9 ../Springhead2.tgz *.*
cd ..
cd ..
