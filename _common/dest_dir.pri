
win32:{
DESTDIR = $$PWD/../bin-w32
} else {
	android:{
	DESTDIR = $$PWD/../bin/_android
	} else {
	DESTDIR = $$PWD/../bin
	}
}

