@echo on

git submodule init
git submodule update

rem Create dummy _paths.bat file as all it's variables are declared from outside
echo. >> _paths.bat

rem echo set QtDir=%QT_DIR%>> _paths.bat
rem echo set MinGW=%COMPILER%>> _paths.bat
rem if [%PLATFORM%]==[x64] echo set MINGWx64Dest=yes>> _paths.bat
rem if [%PLATFORM%]==[Win32] echo set MINGWx32Dest=yes>> _paths.bat
rem echo set GitDir=%GIT_BINARY_DIR%>> _paths.bat
rem echo set CMakeDir=%CMAKE_BINARY_DIR%>> _paths.bat

if [%APPVEYOR_PULL_REQUEST_NUMBER%]==[] (
    "%BASH_EXE%" _common/appveyor-ci/curdate.sh > build_date_%APPVEYOR_REPO_BRANCH%_%WINXX_ARCH%.txt
    type build_date_%APPVEYOR_REPO_BRANCH%_%WINXX_ARCH%.txt
)
