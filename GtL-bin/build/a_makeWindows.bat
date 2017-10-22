@windres resource.rc resource.o
gcc -L"%cd%\SDL" main.c SDLGateway.c SDLGateway.h outermeSDL.c outermeSDL.h resource.o -o GatewayToLegend.exe -mwindows -lSDL2 -lSDL2_image -lSDL2_ttf
gcc -L"%cd%\SDL" mainToolchain.c outermeSDL.c outermeSDL.h resource.o -o GatewayToLegendToolchain.exe -lSDL2 -lSDL2_image -lSDL2_ttf
@rem the above line can't be muted or it won't compile
@move /Y %~dp0\GatewayToLegend.exe %~dp0\..\execute\
@move /Y %~dp0\GatewayToLegendToolchain.exe %~dp0\..\execute\
@cd ..\execute
@set execDir=%cd%
@cd ..\build
@powershell "$s=(New-Object -COM WScript.Shell).CreateShortcut('%userprofile%\Desktop\Gateway to Legend.lnk');$s.TargetPath='%execDir%\GatewayToLegend.exe';$s.IconLocation='%cd%\icon.ico';$s.workingDirectory='%execDir%';$s.Save()"
@echo Done assembling! You can find and launch Gateway to Legend from your Desktop, hopefully. If not, you can make a shortcut from the .exe in the execute subfolder.
@pause