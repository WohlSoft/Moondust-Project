
#LatestSDL='SDL-a5667285680d'
LatestSDL=$(find . -maxdepth 1 -name "SDL-*.tar.gz" | sed "s/\.tar\.gz//;s/\.\///");

echo "=====Latest SDL is $LatestSDL====="

