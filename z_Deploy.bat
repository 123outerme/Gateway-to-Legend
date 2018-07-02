@ROBOCOPY %cd% "%cd%\GtL-bin\build" *.c *.h *.ico *.sh a_makeWindows.bat
@ROBOCOPY %cd% "%cd%\GtL-bin\execute" *.txt *.png *.ttf *.ogg *.mp3 /MIR /XD dirs "%cd%\GtL" "%cd%\GtL-bin" "%cd%\.github" "%cd%\.git" "%cd%\bin" "%cd%\builds" "%cd%\documentation" "%cd%\obj" "%cd%\media" "%cd%\docs" "%cd%\saves"
mkdir %cd%\GtL-bin\execute\saves
@ROBOCOPY %cd% "%cd%\GtL-bin" *.txt
@ROBOCOPY %cd% "%cd%\GtL-bin\stuff" *.ico
@del %cd%\builds\GatewayToLegendbin.zip
@del %cd%\GtL-bin\execute\GatewayToLegend.exe
@del %cd%\GtL-bin\execute\GatewayToLegend.cfg
@ROBOCOPY %cd% "%cd%\GtL\execute" *.txt *.png *.ttf *.ogg *.mp3 /MIR /XD dirs "%cd%\GtL" "%cd%\GtL-bin" "%cd%\.github" "%cd%\.git" "%cd%\bin" "%cd%\builds" "%cd%\documentation" "%cd%\obj" "%cd%\media" "%cd%\docs" "%cd%\saves"
mkdir %cd%\GtL\execute\saves
@cd GtL-bin
@rem zip it up...
@%cd%\..\7za.exe a -y -tzip "%cd%\..\builds\GatewayToLegendbin.zip" "%cd%" -mx5
@rem done...
@cd build
@del %cd%\..\..\builds\GatewayToLegend.zip
@del %cd%\..\..\GtL\execute\GatewayToLegend.exe
@del %cd%\..\..\GtL\execute\GatewayToLegend.cfg
call a_makeWindows.bat
@ROBOCOPY %cd%\..\execute %cd%\..\..\GtL\execute *.exe *.ico *.bat *.ttf
@rem zip it up...
cd ..\..
@%cd%\7za.exe a -y -tzip "%cd%\builds\GatewayToLegend.zip" "%cd%\GtL" -mx5
@rem done...