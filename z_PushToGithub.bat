@ROBOCOPY %cd% "C:\Program Files\Git\code\Gateway-to-Legend" *.c *.h *.txt *.ico *.bin *.rc *.bat *.png *.ttf /MIR /XD .git
@cd "C:\Program Files\Git\code\Gateway-to-Legend"
@set commitMessage=""
@IF [%1]==[] (
	set commitMessage=Updated
) ELSE (
	set commitMessage=%1
)
@git init
@git add .
@git commit -m %commitMessage%
@git remote add origin https://github.com/123outerme/Gateway-to-Legend
@git push origin master
@cd %~dp0
@pause Done! Press anything to continue.