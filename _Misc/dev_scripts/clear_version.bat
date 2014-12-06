call _paths.bat

del %SOURCEDIR%\bin\_build\editor\_release\.obj\aboutdialog.o
del %SOURCEDIR%\bin\_build\editor\_release\.obj\pge_editor_res.o
del %SOURCEDIR%\bin\_build\_dynamic\editor\_release\.obj\aboutdialog.o
del %SOURCEDIR%\bin\_build\_dynamic\editor\_release\.obj\pge_editor_res.o

del %SOURCEDIR%\bin\_build\engine\_release\.obj\qrc_engine.o
del %SOURCEDIR%\bin\_build\engine\_release\.obj\engine_res.o
del %SOURCEDIR%\bin\_build\_dynamic\engine\_release\.obj\qrc_engine.o
del %SOURCEDIR%\bin\_build\_dynamic\engine\_release\.obj\engine_res.o

del %SOURCEDIR%\bin\_build\calibrator\_release\.obj\caltool_res.o
del %SOURCEDIR%\bin\_build\calibrator\_release\.obj\qrc_images.o
del %SOURCEDIR%\bin\_build\_dynamic\calibrator\_release\.obj\caltool_res.o
del %SOURCEDIR%\bin\_build\_dynamic\calibrator\_release\.obj\qrc_images.o

echo Removed all objects which contains version numbers
pause