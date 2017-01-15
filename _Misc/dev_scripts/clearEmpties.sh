#!/bin/bash
source ./_paths.sh
CurDir=~+

path=$(realpath ../../bin/)
echo "looking at $path"
find ../../bin/ -size 0 -type f -exec echo deleted {} \; -exec rm {} \;
echo "Empty object files was removed!"
read -n 1

