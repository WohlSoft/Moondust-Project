#!/bin/bash

DEST_DIR="/Applications/Moondust Project/"
# /Users/vitaly/Repos/Moondust-Project/build/Desktop_arm_darwin_generic_mach_o_64bit-Debug/TEST/Moondust
SRC_DIR="$1"

pkgbuild \
    --identifier ru.wohlsoft.moondust \
    --root "$SRC_DIR" \
    --install-location "$DEST_DIR" \
    --scripts ./scripts/ \
    --min-os-version "10.12" \
    \
    $2
