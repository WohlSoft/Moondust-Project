@echo on

git submodule init
git submodule update

echo set QtDir=%QT_DIR%>> _paths.bat
echo set MinGW=%COMPILER%>> _paths.bat
if [%PLATFORM%]==[x64] echo set MINGWx64Dest=yes>> _paths.bat
rem if [%PLATFORM%]==[Win32] echo set MINGWx32Dest=yes>> _paths.bat
echo set GitDir=%GIT_BINARY_DIR%>> _paths.bat
echo set CMakeDir=%CMAKE_BINARY_DIR%>> _paths.bat
if [%APPVEYOR_PULL_REQUEST_NUMBER%]==[] "%BASH_EXE%" _common/appveyor-ci/curdate.sh > build_date_%APPVEYOR_REPO_BRANCH%_%WINXX_ARCH%.txt
if [%APPVEYOR_PULL_REQUEST_NUMBER%]==[] type build_date_%APPVEYOR_REPO_BRANCH%_%WINXX_ARCH%.txt

