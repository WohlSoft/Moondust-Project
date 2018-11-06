@echo on

if [%APPVEYOR_PULL_REQUEST_NUMBER%]==[] (
    echo "Prepating artifacts..."
    md %BIN_DIR%
    md %BIN_DIR%\_packed
    move bin-cmake-release\_packed\*.zip %BIN_DIR%\_packed
) else (
    echo "Pull-request detected, skipping artifacts..."
)

rem Artifact uploading probe
echo. > dummy.txt
appveyor PushArtifact dummy.txt

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
