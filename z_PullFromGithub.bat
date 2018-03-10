@set folder=C:\Program Files\Git\tmp\pulls
@set project=Gateway-to-Legend
@cd %folder%\%project%
@IF EXIST "%folder%\%project%\.git" (
	cd /d "%folder%\%project%\.git"
	for /F "delims=" %%i in ('dir /b') do (rmdir "%%i" /s/q || del "%%i" /s/q)
	cd ..
)

@git init
@git remote add origin https://github.com/123outerme/%project%.git
git pull origin master --allow-unrelated-histories
@ROBOCOPY "%folder%\%project%" "C:\Program Files\Git\code\%project%" /MIR
@IF EXIST "%folder%\%project%" (
	@cd /d "%folder%\%project%"
	@for /F "delims=" %%i in ('dir /b') do (rmdir "%%i" /s/q || del "%%i" /s/q)
)
@ROBOCOPY "C:\Program Files\Git\code\%project%" %~dp0 *.c *.h *.txt *.ico *.rc *.png *.bat *.html *.css *.md /MIR /XD dirs .git GtL GtL-bin media
@cd "C:\Program Files\Git\code\%project%"
@git init
@git add -A
@cd %~dp0
@rem @pause Done! Press anything to continue.