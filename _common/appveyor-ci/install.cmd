@echo off

git submodule init
git submodule update

rem Create dummy _paths.bat file as all it's variables are declared from outside
echo. > _paths.bat

if [%APPVEYOR_PULL_REQUEST_NUMBER%]==[] (
    "%BASH_EXE%" _common/appveyor-ci/curdate.sh > build_date_%APPVEYOR_REPO_BRANCH%_%WINXX_ARCH%.txt
    type build_date_%APPVEYOR_REPO_BRANCH%_%WINXX_ARCH%.txt
)
