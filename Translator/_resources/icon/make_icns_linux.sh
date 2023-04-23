#!/bin/bash

# This script requires ImageMagick and icnsutils packages being installed

TYPE="translator"
if [[ "$TYPE" == "" ]]; then
echo "Error: empty name!"
exit 1
fi

mkdir $TYPE".iconset"
cp $TYPE"16.png" $TYPE".iconset/icon_16x16.png"
cp $TYPE"32.png" $TYPE".iconset/icon_32x32.png"
# convert $TYPE".iconset/icon_32x32.png" -scale 64 $TYPE".iconset/icon_64x64.png"
cp $TYPE"48.png" $TYPE".iconset/icon_48x48.png"
# convert $TYPE".iconset/icon_48x48.png" -scale 96 $TYPE".iconset/icon_96x96.png"
cp $TYPE"256.png" $TYPE".iconset/icon_256x256.png"
convert $TYPE".iconset/icon_256x256.png" -scale 512  $TYPE".iconset/icon_512x512.png"
convert $TYPE".iconset/icon_256x256.png" -scale 128  $TYPE".iconset/icon_128x128.png"

#convert $TYPE".iconset/icon_256x256.png" -scale 512 $TYPE".iconset/icon_256x256@2x.png"
#sips -Z 512 $TYPE".iconset/icon_256x256.png" --out $TYPE".iconset/icon_256x256@2x.png"
#sips -Z 512 $TYPE".iconset/icon_256x256.png" --out $TYPE".iconset/icon_512x512.png"
#sips -Z 128 $TYPE".iconset/icon_256x256.png" --out $TYPE".iconset/icon_128x128.png"

#read -n 1

echo "makeIcon..."

png2icns $TYPE.icns $TYPE".iconset/icon_"*.png
# mv $TYPE".icns" ../

rm -Rf $TYPE".iconset"
