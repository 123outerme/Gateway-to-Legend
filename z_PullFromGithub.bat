@set folder="C:\Program Files\Git\tmp\pulls"
@cd %folder%
@git remote add origin https://github.com/123outerme/Gateway-to-Legend.git
@git init
git clone https://github.com/123outerme/Gateway-to-Legend.git
@ROBOCOPY %folder%\Gateway-to-Legend "C:\Program Files\Git\code\Gateway-to-Legend" /MIR
@IF EXIST %folder% (
    cd /d %folder%
    for /F "delims=" %%i in ('dir /b') do (rmdir "%%i" /s/q || del "%%i" /s/q)
)
@ROBOCOPY "C:\Program Files\Git\code\Gateway-to-Legend" %~dp0 *.c *.h *.txt *.ico *.rc *.png /MIR
@cd "C:\Program Files\Git\code\Gateway-to-Legend"
@git init
@git add -A
@cd %~dp0
@pause Done! Press anything to continue.