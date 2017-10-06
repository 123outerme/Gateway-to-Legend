@ROBOCOPY %cd% "C:\Program Files\Git\code\SDL-Seekers" *.c *.h *.txt *.ico *.bin *.rc *.bat SeekersTile48.png /MIR /XD .git
@cd "C:\Program Files\Git\code\SDL-Seekers"
@set commitMessage=""
@IF [%1]==[] (
	set commitMessage=Updated
) ELSE (
	set commitMessage=%1
)
@git init
@git add .
@git commit -m %commitMessage%
@git remote add origin https://github.com/123outerme/SDL-Seekers
@git push origin master
@cd %~dp0
@pause Done! Press anything to continue.