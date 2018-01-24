linux-g++||unix:!macx:!android: {
    TARGETOS=linux
}

haiku-g++:{
    TARGETOS=haiku
}

win32: {
    !win*-msvc*:{
        win64:{
            TARGETOS=win64
        } else:win32-mingw-w64 {
            TARGETOS=win32-mingw-w64
        } else {
            TARGETOS=win32
        }
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

message("Target library path is $$TARGETOS!")
