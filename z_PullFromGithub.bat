@set folder="C:\Program Files\Git\tmp\pulls"
@cd %folder%
@git remote add origin https://github.com/123outerme/SDL-Seekers.git
@git init
git clone https://github.com/123outerme/SDL-Seekers.git
@ROBOCOPY %folder%\SDL-Seekers "C:\Program Files\Git\code\SDL-Seekers" /MIR
@IF EXIST %folder% (
    cd /d %folder%
    for /F "delims=" %%i in ('dir /b') do (rmdir "%%i" /s/q || del "%%i" /s/q)
)
@ROBOCOPY "C:\Program Files\Git\code\SDL-Seekers" %~dp0 *.c *.h *.txt *.ico *.rc *.png /MIR
@cd "C:\Program Files\Git\code\SDL-Seekers"
@git init
@git add -A
@cd %~dp0
@pause Done! Press anything to continue.