@set project=Gateway-to-Legend
@ROBOCOPY %cd% "C:\Program Files\Git\code\%project%" *.c *.h *.txt *.ico *.bin *.rc *.bat *.png *.ttf *.ogg *.mp3 *.mp4 *.html *.css *.md /MIR /XD dirs .git GtL GtL-bin media saves
@cd "C:\Program Files\Git\code\%project%"
@set commitMessage=""
@IF [%1]==[] (
	set commitMessage=Updated
) ELSE (
	set commitMessage=%1
)
@set branch=""
@IF [%2]==[] (
	set branch=master
) ELSE (
	set branch=%2
)
@git init
@git add .
@git commit -m %commitMessage%
@git remote add origin https://github.com/123outerme/%project%
@git push origin %branch%
@cd %~dp0
@pause Done! Press anything to continue.