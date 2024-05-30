@echo on
@echo Starting VSCode for gfxDraw
@echo Please adapt to your local folder structures.
@echo.

call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
code "%USERPROFILE%\Projects\gfxDraw\gfxDraw.code-workspace"
exit
