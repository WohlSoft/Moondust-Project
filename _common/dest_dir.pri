#
# Set the bin directory in the repository root as compiled binary destination
#

win32:{
	!win64:{
		DESTDIR = $$PWD/../bin-w32
	} else {
		DESTDIR = $$PWD/../bin-w64
	}
} else {
	android:{
	    DESTDIR = $$PWD/../bin/_android
	} else {
	    DESTDIR = $$PWD/../bin
	}
}
