call _paths.bat
set TARGETDIR=%LINUXSERV1%

CALL ./_copydata.bat

IF NOT '%1'=='nopause' pause
