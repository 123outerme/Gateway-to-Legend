@ROBOCOPY %cd% "%cd%\SoU-bin\build" *.c *.h *.txt maps.bin
@ROBOCOPY %cd% "%cd%\SoU-bin" *.txt
@copy %cd% "%cd%\SoU-bin\execute" maps.bin
@ROBOCOPY %cd% "%cd%\SoU-bin\stuff" *.ico *.rc
@del %cd%\builds\binaries\SorceryOfUvutuPCbin.zip
@del %cd%\SoU-bin\execute\SorceryOfUvutu.exe
@del %cd%\SoU-bin\execute\sorceryConfig.ini
@del %cd%\SoU-bin\execute\SAVUVUTU.bin
@cd SoU-bin
@rem zip it up...
@%cd%\7za.exe a -y -tzip "%cd%\builds\binaries\SorceryOfUvutuPCbin.zip" "%cd%\SoU-bin" -mx5
@rem done...
@del %cd%\builds\prebuilt\SorceryOfUvutuPC.zip
@del %cd%\SoU\execute\SorceryOfUvutu.exe
@del %cd%\SoU\execute\sorceryConfig.ini
@del %cd%\SoU\execute\SAVUVUTU.bin
@cd build
call a_makeWindows.bat
@ROBOCOPY %cd%\SoU-bin\execute %cd%\SoU\execute *.exe *.ico maps.bin
@ROBOCOPY %cd% %cd%\SoU *.txt
@rem zip it up...
@%cd%\7za.exe a -y -tzip "%cd%\builds\prebuilt\SorceryOfUvutuPC.zip" "%cd%\SoU" -mx5
@rem done...
@cd ..\..