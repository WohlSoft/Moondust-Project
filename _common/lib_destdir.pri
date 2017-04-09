linux-g++||unix:!macx:!android: {
    TARGETOS=linux
}

win32: {
    !win*-msvc*:{
        TARGETOS=win32
    } else {
        TARGETOS=win32-msvc
    }
}

macx:{
    TARGETOS=macos
}

android:{
    TARGETOS=android
}
