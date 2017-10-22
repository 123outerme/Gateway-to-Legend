@ROBOCOPY %cd% "%cd%\GtL-bin\build" *.c *.h *.ico *.rc
@ROBOCOPY %cd% "%cd%\GtL-bin\execute" *.txt *.png *.ttf /MIR /XD dirs "%cd%\GtL" "%cd%\GtL-bin" "%cd%\.github" "%cd%\.git" "%cd%\bin" "%cd%\builds" "%cd%\documentation" "%cd%\obj"
@ROBOCOPY %cd% "%cd%\GtL-bin" *.txt
@ROBOCOPY %cd% "%cd%\GtL-bin\stuff" *.ico *.rc
@del %cd%\builds\binaries\GatewayToLegendbin.zip
@del %cd%\GtL-bin\execute\GatewayToLegend.exe
@del %cd%\GtL-bin\execute\GatewayToLegendToolchain.exe
@del %cd%\GtL-bin\execute\GatewayToLegend.cfg
@ROBOCOPY %cd% "%cd%\GtL\execute" *.txt *.png /MIR /XD dirs "%cd%\GtL" "%cd%\GtL-bin" "%cd%\.github" "%cd%\.git" "%cd%\bin" "%cd%\builds" "%cd%\documentation" "%cd%\obj"
@cd GtL-bin
@rem zip it up...
@%cd%\..\7za.exe a -y -tzip "%cd%\..\builds\binaries\GatewayToLegendbin.zip" "%cd%" -mx5
@rem done...
@cd build
@del %cd%\..\..\builds\prebuilt\GatewayToLegend.zip
@del %cd%\..\..\GtL\execute\GatewayToLegend.exe
@del %cd%\GtL-bin\execute\GatewayToLegendToolchain.exe
@del %cd%\..\..\GtL\execute\GatewayToLegend.cfg
call a_makeWindows.bat
@ROBOCOPY %cd%\..\execute %cd%\..\..\GtL\execute *.exe *.ico *.bat *.ttf
@rem zip it up...
cd ..\..
@%cd%\7za.exe a -y -tzip "%cd%\builds\prebuilt\GatewayToLegend.zip" "%cd%\GtL" -mx5
@rem done...