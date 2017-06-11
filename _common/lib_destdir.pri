linux-g++||unix:!macx:!android: {
    TARGETOS=linux
}

win32: {
    !win*-msvc*:{
		!win64:{
			TARGETOS=win32
		} else {
			TARGETOS=win64
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
