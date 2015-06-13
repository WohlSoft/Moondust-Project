#!/bin/bash
source ./_paths.sh
CurDir=~+

rm $SOURCEDIR/bin/_build/editor/_release/.obj/aboutdialog.o
rm $SOURCEDIR/bin/_build/editor/_release/.obj/pge_editor_res.o
rm $SOURCEDIR/bin/_build/_dynamic/editor/_release/.obj/aboutdialog.o
rm $SOURCEDIR/bin/_build/_dynamic/editor/_release/.obj/pge_editor_res.o

rm $SOURCEDIR/bin/_build/engine/_release/.obj/qrc_engine.o
rm $SOURCEDIR/bin/_build/engine/_release/.obj/engine_res.o
rm $SOURCEDIR/bin/_build/_dynamic/engine/_release/.obj/qrc_engine.o
rm $SOURCEDIR/bin/_build/_dynamic/engine/_release/.obj/engine_res.o

rm $SOURCEDIR/bin/_build/calibrator/_release/.obj/caltool_res.o
rm $SOURCEDIR/bin/_build/calibrator/_release/.obj/qrc_images.o
rm $SOURCEDIR/bin/_build/_dynamic/calibrator/_release/.obj/caltool_res.o
rm $SOURCEDIR/bin/_build/_dynamic/calibrator/_release/.obj/qrc_images.o

echo "Removed all objects which contains version numbers"
read -n 1

