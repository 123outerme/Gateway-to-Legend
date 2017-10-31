@set folder="C:\Program Files\Git\tmp\pulls\"
@cd %folder%\Gateway-to-Legend
@git remote add origin https://github.com/123outerme/Gateway-to-Legend.git
@git init
git pull origin master --allow-unrelated-histories
@ROBOCOPY "%folder%\Gateway-to-Legend" "C:\Program Files\Git\code\Gateway-to-Legend" /MIR
IF EXIST "%folder%\Gateway-to-Legend" (
    cd /d "%folder%\Gateway-to-Legend"
    for /F "delims=" %%i in ('dir /b') do (rmdir "%%i" /s/q || del "%%i" /s/q)
)
@ROBOCOPY "C:\Program Files\Git\code\Gateway-to-Legend" %~dp0 *.c *.h *.txt *.ico *.rc *.png /MIR /XD dirs .git GtL GtL-bin
@cd "C:\Program Files\Git\code\Gateway-to-Legend"
@git init
@git add -A
@cd %~dp0
@pause Done! Press anything to continue.