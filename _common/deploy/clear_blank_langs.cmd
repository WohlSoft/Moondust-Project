@echo off
setlocal
for /f  "usebackq delims=;" %%A in (`dir /b *.qm`) do If %%~zA LSS 1000 del "%%A"

