#!/bin/bash
source ./_paths.sh
CurDir=~+

## declare an array variable
declare -a arr=("$SOURCEDIR/bin/_build/" "$SOURCEDIR/bin/_build/_dynamic/" "$SOURCEDIR/bin/_build_x32/" "$SOURCEDIR/bin/_build_x32/_dynamic/" "$SOURCEDIR/bin/_build_x64/" "$SOURCEDIR/bin/_build_x64/" "$SOURCEDIR/bin/_build_x64/_dynamic/")

killf()
{
    ## now loop through the above array
    for i in "${arr[@]}"
    do
        if [ -f $i$1 ]; then
            rm $i$1
            echo "kill -> $i$1"
        fi
    done
}

killf pge_editor/_release/.obj/aboutdialog.o
killf pge_editor/_release/.obj/check_updates.o
killf pge_editor/_release/.obj/moc_aboutdialog.o
killf pge_editor/_release/.obj/moc_check_updates.o
killf pge_editor/_release/.obj/pge_editor_res.o
killf pge_engine/_release/.obj/qrc_engine.o
killf pge_engine/_release/.obj/engine_res.o
killf pge_engine/_release/.obj/main.o
killf pge_calibrator/_release/.obj/caltool_res.o
killf pge_calibrator/_release/.obj/qrc_images.o
killf png2gifs/_release/.obj/qrc_png2gifs.o
killf png2gifs/_release/.obj/png2gifs.o
killf GIFs2PNG/_release/.obj/qrc_gifs2png.o
killf GIFs2PNG/_release/.obj/gifs2png.o
killf LazyFixTool/_release/.obj/qrc_lazyfix_tool.o
killf LazyFixTool/_release/.obj/LazyFixTool.o

echo "Removed all objects which contains version numbers"
read -n 1

