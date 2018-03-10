@set project=Gateway-to-Legend
@ROBOCOPY %cd% "C:\Program Files\Git\code\%project%" *.c *.h *.txt *.ico *.bin *.rc *.bat *.png *.ttf *.ogg *.mp4 *.html *.css *.md /MIR /XD dirs .git GtL GtL-bin media
@cd "C:\Program Files\Git\code\%project%"
@set commitMessage=""
@IF [%1]==[] (
	set commitMessage=Updated
) ELSE (
	set commitMessage=%1
)
@git init
@git add .
@git commit -m %commitMessage%
@git remote add origin https://github.com/123outerme/%project%
@git push origin master
@cd %~dp0
@pause Done! Press anything to continue.