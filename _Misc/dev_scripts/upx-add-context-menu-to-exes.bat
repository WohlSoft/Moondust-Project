@echo off

rem Generate REG-Compatible current directory path
echo %cd%>%temp%\cd-value.temp
fart %temp%\cd-value.temp \ \\ > NUL
set /p UPXPath=<%temp%\cd-value.temp
del %temp%\cd-value.temp

copy _upx_context_menu.reg.template %temp%\_upx_context_menu.reg > NUL

fart %temp%\_upx_context_menu.reg @@CD@@ %UPXPath% > NUL

regedit /s %temp%\_upx_context_menu.reg

del %temp%\_upx_context_menu.reg

