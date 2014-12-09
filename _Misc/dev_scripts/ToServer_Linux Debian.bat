call _paths.bat
set TARGETDIR=%LINUXSERV2%

CALL ./_copydata.bat

IF NOT '%1'=='nopause' pause
