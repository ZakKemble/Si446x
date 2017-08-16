@set MAKE=make.exe

@cd /d %~dp0
@%MAKE% clean
@%MAKE% -j 4
@if %errorlevel%==0 %MAKE% avrdude
pause
