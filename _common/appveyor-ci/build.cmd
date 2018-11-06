@echo on

rem build_deps.bat buildlibs nopause
rem build.bat nopause # noengine nocalibrator nogifs2png nopng2gifs nolazyfixtool nomanager nomaintainer nomusicplayer
rem cd _Misc\dev_scripts\deploy\
rem if [%PLATFORM%]==[Win32] deploy_windows_dynamic.bat nopause
rem if [%PLATFORM%]==[Win32] deploy_windows_dynamic_32_w64.bat nopause
rem if [%PLATFORM%]==[x64] deploy_windows_dynamic_x64.bat nopause
rem cd ..\..\..\

build.bat nopause cmake-it ninja deploy

if [%APPVEYOR_PULL_REQUEST_NUMBER%]==[] (
    echo "Prepating artifacts..."
    md %BIN_DIR%
    md %BIN_DIR%\_packed
    move bin-cmake-release\_packed\*.zip %BIN_DIR%\_packed
) else (
    echo "Pull-request detected, skipping artifacts..."
)

rem  == Disabled due next error: ==
rem  'appveyor' is not recognized as an internal or external command,
rem  operable program or batch file.
rem  ==============================
rem # Upload artifacts
rem   - if [%APPVEYOR_PULL_REQUEST_NUMBER%]==[] appveyor PushArtifact "%BIN_DIR%\_packed\pge-project-%APPVEYOR_REPO_BRANCH%-%WINXX_ARCH%.zip"
rem   - if [%APPVEYOR_PULL_REQUEST_NUMBER%]==[] appveyor PushArtifact "%BIN_DIR%\_packed\install-pge-common-%APPVEYOR_REPO_BRANCH%-%WINXX_ARCH%.zip"
rem   - if [%APPVEYOR_PULL_REQUEST_NUMBER%]==[] appveyor PushArtifact "%BIN_DIR%\_packed\install-pge-editor-%APPVEYOR_REPO_BRANCH%-%WINXX_ARCH%.zip"
rem   - if [%APPVEYOR_PULL_REQUEST_NUMBER%]==[] appveyor PushArtifact "%BIN_DIR%\_packed\install-pge-engine-%APPVEYOR_REPO_BRANCH%-%WINXX_ARCH%.zip"
rem   - if [%APPVEYOR_PULL_REQUEST_NUMBER%]==[] appveyor PushArtifact "%BIN_DIR%\_packed\install-pge-tools-%APPVEYOR_REPO_BRANCH%-%WINXX_ARCH%.zip"
rem #Standalone package of PGE Maintainer (work outside PGE)
rem   - if [%APPVEYOR_PULL_REQUEST_NUMBER%]==[] appveyor PushArtifact "%BIN_DIR%\_packed\pge-maintainer-%APPVEYOR_REPO_BRANCH%-%WINXX_ARCH%.zip"
rem #Standalone package of PGE MusPlay (work outside PGE)
rem   - if [%APPVEYOR_PULL_REQUEST_NUMBER%]==[] appveyor PushArtifact "%BIN_DIR%\_packed\pge-musplay-%APPVEYOR_REPO_BRANCH%-%WINXX_ARCH%.zip"
rem #Standalone package of Playable Character calibration tool (work outside PGE)
rem   - if [%APPVEYOR_PULL_REQUEST_NUMBER%]==[] appveyor PushArtifact "%BIN_DIR%\_packed\pge-calibrator-%APPVEYOR_REPO_BRANCH%-%WINXX_ARCH%.zip"
rem   - if [%APPVEYOR_PULL_REQUEST_NUMBER%]==[] appveyor PushArtifact "%BIN_DIR%\_packed\GIFs2PNG-%APPVEYOR_REPO_BRANCH%-%WINXX_ARCH%.zip"
rem   - if [%APPVEYOR_PULL_REQUEST_NUMBER%]==[] appveyor PushArtifact "%BIN_DIR%\_packed\PNG2GIFs-%APPVEYOR_REPO_BRANCH%-%WINXX_ARCH%.zip"
rem   - if [%APPVEYOR_PULL_REQUEST_NUMBER%]==[] appveyor PushArtifact "%BIN_DIR%\_packed\LazyFixTool-%APPVEYOR_REPO_BRANCH%-%WINXX_ARCH%.zip"
rem   - if [%APPVEYOR_PULL_REQUEST_NUMBER%]==[] appveyor PushArtifact "build_date_%APPVEYOR_REPO_BRANCH%_%WINXX_ARCH%.txt"
